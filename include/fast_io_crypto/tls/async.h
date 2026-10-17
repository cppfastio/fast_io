#pragma once

/*
Async support for the TLS stream.

Post-handshake the kernel owns record AEAD, so most async operations --
write/scatter-write/transmit -- forward straight to the bound socket
observer and run on whatever posix backend the scheduler selects
(io_uring, thread pool). They carry the same established() precondition
as the synchronous byte ops.

Reads and close are the exceptions: a kTLS socket has no read_iter, so
receiving needs recvmsg's TLS_GET_RECORD_TYPE cmsg to recover the record
type -- the io_uring defines below run a dedicated RECVMSG op that pumps
control records in-kernel fashion until application data arrives. Close
is close_notify first, fd close second -- a best-effort alert sendmsg leg
on io_uring, a worker-side sendmsg on the thread pool.

The handshake is different: it is userspace protocol work (parse, verify,
rekey) rather than a device op, so it is offered only on schedulers that
can run blocking work -- the posix thread pool. On io_uring the define is
absent and async_handshake* fails to constrain rather than silently
blocking the event pump.
*/

#if defined(__linux__) && defined(__HERBCEPTIONS__)

namespace fast_io::tls
{


/* no async_close forwarder: the close contract consumes the handle, but
   a tls observer does not own the fd -- the basic_tls::socket member or the
   caller's file does. Close the socket owner directly. */

namespace details
{

/* ---------------- io_uring kTLS recvmsg path ---------------- */

/* TLS_GET_RECORD_TYPE from a completed recvmsg's control buffer.
   Records without the cmsg report application_data -- that also covers a
   bare TCP EOF (res == 0, no cmsg), which reads as a 0-length result. */
inline content_type tls_recv_ctype(ktls_msghdr const &msg) noexcept
{
	::std::size_t off{};
	while (off + sizeof(ktls_cmsghdr) <= msg.controllen)
	{
		auto const &cmsg{*reinterpret_cast<ktls_cmsghdr const *>(
			static_cast<::std::byte const *>(msg.control) + off)};
		if (cmsg.len < sizeof(ktls_cmsghdr))
		{
			break;
		}
		if (cmsg.level == sol_tls && cmsg.type == tls_get_record_type &&
			cmsg.len >= sizeof(ktls_cmsghdr) + 1)
		{
			return static_cast<content_type>(*reinterpret_cast<::std::uint_least8_t const *>(
				static_cast<::std::byte const *>(msg.control) + off + sizeof(ktls_cmsghdr)));
		}
		off += ktls_cmsg_align_up(cmsg.len);
	}
	return content_type::application_data;
}

/* copy up to k received bytes at logical offset off out of the iov list
   (scalar or the caller's scatter array); total caps the walk at what
   the cqe actually delivered so over-long iovecs cannot leak stale
   bytes */
inline ::std::size_t tls_record_gather(ktls_msghdr const &msg, ::std::size_t total,
									   ::std::size_t off, ::std::byte *out,
									   ::std::size_t k) noexcept
{
	::std::size_t got{};
	::std::size_t pos{};
	for (::std::size_t i{}; i < msg.iovlen && got < k && off < total; ++i)
	{
		auto const &e{msg.iov[i]};
		if (off < pos + e.len)
		{
			::std::size_t const skip{off - pos};
			::std::size_t take{e.len - skip};
			if (take > total - off)
			{
				take = total - off;
			}
			if (take > k - got)
			{
				take = k - got;
			}
			__builtin_memcpy(out + got, static_cast<::std::byte const *>(e.base) + skip, take);
			got += take;
			off += take;
		}
		pos += e.len;
	}
	return got;
}

/*
One pending kTLS read on io_uring: one IORING_OP_RECVMSG per record. The
msghdr, iovec and control buffer live in the cookie -- the kernel
dereferences them when the op executes, long after submission returned.

The cookies erase both the client type and the completion functor: the
pump bodies (round/deliver/invoke/finish) touch neither -- the fd is
snapped at submit and the few typed operations (KeyUpdate rekey, record
open, the callback itself) run through thunks -- so the machinery emits
once instead of once per client_t x func pair.

The invoke/deliver pair ports read_some's record pump: each round's
cqes (op plus the armed link-timeout) funnel into deliver when
tlink.pending drains; a consumed control record (CCS, NST, KeyUpdate)
resubmits a fresh round on the same cookie instead of completing, so the
user callback only ever sees application_data, an alert, an error or
eof. The timeout is a per-round duration -- a peer streaming control
records cannot starve the pump, and each recvmsg gets the full window.
*/
struct io_uring_tls_recv_cookie
{
	using allocator_type = ::fast_io::native_global_allocator;
	static inline constexpr bool alloc_with_status{allocator_type::has_status};
	::fast_io::liburing::details::io_uring_invoke_func invoke;
	::fast_io::liburing::details::io_uring_timeout_link_block tlink;
	::std::size_t transferred{};
	int errn{};
	::fast_io::liburing::io_uring_timespec ts{};
	::fast_io::linux_io_uring_observer sched{};
	int fd{};
	void *client{};
	::fast_io::io_scatter_t const *scatters{}; /* nullptr: scalar buf */
	::std::size_t nscatters{};
	::std::byte *buf{};
	::std::size_t buf_size{};
	::fast_io::posix_statx_timestamp_opt timeout{};
	ktls_iovec iov{};
	ktls_msghdr msg{};
	alignas(::std::size_t)::std::byte control[ktls_cmsg_space];
	/* KeyUpdate rekey on the stream -- err->domain set on failure */
	void (*key_update)(void *client, ::std::uint_least8_t req,
					   ::std::cxx_std_error *err) noexcept {};
	/* terminal: free the (derived) cookie, invoke the stored functor */
	void (*finish)(io_uring_tls_recv_cookie *self,
				   ::std::cxx_std_error err) noexcept {};
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status,
												   typename allocator_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};

	inline io_uring_tls_recv_cookie(::fast_io::linux_io_uring_observer s,
									void *c, int f,
									::std::byte *b, ::std::size_t bs,
									::fast_io::io_scatter_t const *sc, ::std::size_t nsc,
									::fast_io::posix_statx_timestamp_opt tmo) noexcept
		: sched{s}, fd{f}, client{c}, scatters{sc}, nscatters{nsc}, buf{b},
		  buf_size{bs}, timeout{tmo}
	{
	}
};

/* the typed callbacks carried on the base cookie: KeyUpdate goes through
   the client's own function; terminal delivery adapts the functor's
   signature */
template <typename client_t>
inline void io_uring_tls_recv_key_update_cb(void *cc, ::std::uint_least8_t req,
											::std::cxx_std_error *err) noexcept
{
	try
	{
		tls_client_key_update_received(static_cast<client_t *>(cc), req);
	}
	catch throws(::std::error e)
	{
		*err = e.release();
	}
}

template <typename client_t, typename func>
struct io_uring_tls_recv_cookie_of;

template <typename client_t, typename func>
inline void io_uring_tls_recv_finish_cb(io_uring_tls_recv_cookie *self,
										::std::cxx_std_error err) noexcept
{
	auto *cookie{static_cast<io_uring_tls_recv_cookie_of<client_t, func> *>(self)};
	auto callback{::std::move(cookie->callback)};
	if constexpr (::std::is_invocable_v<func, ::std::cxx_std_error,
										::fast_io::io_scatter_status_t>)
	{
		::fast_io::io_scatter_status_t const status{
			::fast_io::scatter_size_to_status(cookie->transferred, cookie->scatters,
											  cookie->nscatters)};
		::fast_io::details::async_delete_state(cookie);
		callback(err, status);
	}
	else
	{
		::std::size_t const transferred{cookie->transferred};
		::fast_io::details::async_delete_state(cookie);
		callback(err, transferred);
	}
}

/* the submission-time cookie: the typed functor plus the thunks that
   adapt it onto the erased base */
template <typename client_t, typename func>
struct io_uring_tls_recv_cookie_of : io_uring_tls_recv_cookie
{
	func callback;

	inline io_uring_tls_recv_cookie_of(::fast_io::linux_io_uring_observer s,
									   client_t *c,
									   ::std::byte *b, ::std::size_t bs,
									   ::fast_io::io_scatter_t const *sc, ::std::size_t nsc,
									   ::fast_io::posix_statx_timestamp_opt tmo,
									   func &&cb) noexcept
		: io_uring_tls_recv_cookie{s, c, c->sock_.fd, b, bs, sc, nsc, tmo},
		  callback{::std::move(cb)}
	{
		this->key_update = &io_uring_tls_recv_key_update_cb<client_t>;
		this->finish = &io_uring_tls_recv_finish_cb<client_t, func>;
	}
};

/* terminal delivery: frees the cookie, then reports the logical read in
   the callback's own signature -- size_t for scalar, scatter status for
   vectored */
inline void io_uring_tls_recv_finish(io_uring_tls_recv_cookie *cookie,
									 ::std::cxx_std_error err) noexcept
{
	cookie->finish(cookie, err);
}

inline void io_uring_tls_recv_deliver(void *self) noexcept;

/* arm one recvmsg round: fresh control state, the op sqe plus the
   adjacent link-timeout sqe when a deadline is armed */
inline void io_uring_tls_recv_round(
	io_uring_tls_recv_cookie *cookie) FAST_IO_HERBCEPTIONS_THROWS
{
	auto &ring{*cookie->sched.ring};
	auto &msg{cookie->msg};
	msg = {};
	if (cookie->scatters != nullptr)
	{
		msg.iov = const_cast<ktls_iovec *>(
			reinterpret_cast<ktls_iovec const *>(cookie->scatters));
		msg.iovlen = cookie->nscatters;
	}
	else
	{
		cookie->iov = {cookie->buf, cookie->buf_size};
		msg.iov = __builtin_addressof(cookie->iov);
		msg.iovlen = 1;
	}
	msg.control = cookie->control;
	msg.controllen = sizeof(cookie->control);
	::fast_io::liburing::details::io_uring_reserve_sqes(ring,
														cookie->timeout.has_opt ? 2 : 1);
	auto *sqe{::fast_io::liburing::io_uring_get_sqe(ring)};
	::fast_io::liburing::io_uring_prep_recvmsg(sqe, cookie->fd,
											   __builtin_addressof(msg), 0);
	::fast_io::liburing::details::io_uring_arm_timeout(
		ring, cookie, sqe, io_uring_tls_recv_deliver,
		cookie->timeout);
	::fast_io::liburing::details::io_uring_commit(ring);
}

inline void io_uring_tls_recv_deliver(void *self) noexcept
{
	auto *cookie{static_cast<io_uring_tls_recv_cookie *>(self)};
	if (cookie->tlink.fired)
	{
		io_uring_tls_recv_finish(cookie,
								 ::fast_io::details::async_make_error(::std::errc::timed_out));
		return;
	}
	if (cookie->errn != 0)
	{
		io_uring_tls_recv_finish(cookie,
								 {::std::error_domain<::std::errc>::domain(),
								  static_cast<::std::size_t>(cookie->errn)});
		return;
	}
	if ((cookie->msg.flags & 0x8 /* MSG_CTRUNC */) != 0)
	{
		io_uring_tls_recv_finish(
			cookie, ::fast_io::details::async_make_error(::std::errc::message_size));
		return;
	}
	::std::size_t const got{cookie->transferred};
	switch (tls_recv_ctype(cookie->msg))
	{
	case content_type::application_data:
		io_uring_tls_recv_finish(cookie, {});
		return;
	case content_type::alert:
	{
		::std::byte body[2]{};
		::std::size_t const k{got < 2 ? got : 2};
		tls_record_gather(cookie->msg, got, 0, body, k);
		cookie->transferred = 0; /* alert bytes are protocol-consumed */
		if (k == 2 && body[0] == ::std::byte{1} && body[1] == ::std::byte{0})
		{
			io_uring_tls_recv_finish(cookie, {}); /* close_notify: eof */
			return;
		}
		io_uring_tls_recv_finish(cookie, tls_peer_alert_error(body, k));
		return;
	}
	case content_type::handshake:
	{
		/* post-handshake messages: KeyUpdate rekeys, NST and friends are
		   dropped -- walk the record, then keep pumping */
		::std::size_t off{};
		while (off + 4 <= got)
		{
			::std::byte hdr[4];
			tls_record_gather(cookie->msg, got, off, hdr, 4);
			::std::uint_least32_t const mlen{
				(static_cast<::std::uint_least32_t>(
					 static_cast<::std::uint_least8_t>(hdr[1]))
				 << 16) |
				(static_cast<::std::uint_least32_t>(
					 static_cast<::std::uint_least8_t>(hdr[2]))
				 << 8) |
				static_cast<::std::uint_least32_t>(
					static_cast<::std::uint_least8_t>(hdr[3]))};
			if (mlen > got - off - 4)
			{
				break; /* split/malformed: kTLS keeps records atomic */
			}
			if (static_cast<handshake_type>(
					static_cast<::std::uint_least8_t>(hdr[0])) ==
					handshake_type::key_update &&
				mlen == 1)
			{
				::std::byte req{};
				tls_record_gather(cookie->msg, got, off + 4,
								  __builtin_addressof(req), 1);
				::std::cxx_std_error kuerr{};
				cookie->key_update(cookie->client,
								   static_cast<::std::uint_least8_t>(req),
								   __builtin_addressof(kuerr));
				if (kuerr.domain != nullptr)
				{
					io_uring_tls_recv_finish(cookie, kuerr);
					return;
				}
			}
			off += 4 + mlen;
		}
		break;
	}
	default:
		break;
	}
	/* control record consumed -- start another round on this cookie */
	cookie->transferred = 0;
	cookie->errn = 0;
	try
	{
		io_uring_tls_recv_round(cookie);
	}
	catch throws(::std::error e)
	{
		io_uring_tls_recv_finish(cookie, e.release());
	}
}

inline void io_uring_tls_recv_invoke(void *self, ::std::size_t transferred, int errn) noexcept
{
	auto *cookie{static_cast<io_uring_tls_recv_cookie *>(self)};
	cookie->transferred = transferred;
	cookie->errn = errn;
	if (--cookie->tlink.pending == 0)
	{
		io_uring_tls_recv_deliver(cookie);
	}
}

template <typename client_t, typename func>
inline void io_uring_tls_recv_submit(
	::fast_io::linux_io_uring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	client_t *client, ::std::byte *buf,
	::std::size_t buf_size, ::fast_io::io_scatter_t const *scatters,
	::std::size_t nscatters, func callback) noexcept
{
	using cookie_type =
		io_uring_tls_recv_cookie_of<client_t, ::std::remove_cvref_t<func>>;
	try
	{
		::fast_io::liburing::details::io_uring_submit_guard<cookie_type> guard{
			::fast_io::liburing::details::io_uring_new_state<cookie_type>(
				sched, sched, client, buf, buf_size, scatters, nscatters, timeout,
				::std::move(callback))};
		guard.cookie->invoke = io_uring_tls_recv_invoke;
		io_uring_tls_recv_round(guard.cookie);
		guard.release();
	}
	catch throws(::std::error e)
	{
		auto err{e.release()};
		if constexpr (::std::is_invocable_v<::std::remove_cvref_t<func>,
											::std::cxx_std_error,
											::fast_io::io_scatter_status_t>)
		{
			callback(err, ::fast_io::io_scatter_status_t{});
		}
		else
		{
			callback(err, ::std::size_t{});
		}
	}
}

/* ---------------- io_uring userspace-mode TLS ops ---------------- */

/*
When the kernel has no kTLS the stream's wire bytes are ciphertext and
plain READ/WRITE on the fd are honest fd ops -- the TLS record layer is
the client's. One pending sw read: READ rounds accumulate ciphertext
into the cookie until a full record is framed, then the open_record
thunk unwraps it; application data copies into the caller's target and
finishes, control records resubmit. Decrypted overflow joins the
client's rx_pending -- drained synchronously at submit.
*/
struct io_uring_tls_sw_recv_cookie
{
	using allocator_type = ::fast_io::native_global_allocator;
	static inline constexpr bool alloc_with_status{allocator_type::has_status};
	::fast_io::liburing::details::io_uring_invoke_func invoke;
	::fast_io::liburing::details::io_uring_timeout_link_block tlink;
	::std::size_t transferred{}; /* plaintext delivered to the caller */
	::std::size_t res_got{};     /* ciphertext bytes the cqe produced */
	int errn{};
	::fast_io::liburing::io_uring_timespec ts{};
	::fast_io::linux_io_uring_observer sched{};
	int fd{};
	void *client{};
	::fast_io::io_scatter_t const *scatters{}; /* nullptr: scalar buf */
	::std::size_t nscatters{};
	::std::byte *buf{};
	::std::size_t buf_size{};
	::fast_io::posix_statx_timestamp_opt timeout{};
	/* ciphertext accumulation -- records fragment arbitrarily across
	   cqes; a full record is at most 5 + 2^14 + 256 wire bytes */
	alignas(::std::size_t)::std::byte ct[::fast_io::tls::details::tls_max_record];
	::std::size_t ct_have{};
	/* plaintext staging for one opened record */
	::std::byte inner[::fast_io::tls::details::tls_max_ciphertext];
	/* open one framed record into inner -- false + err set on failure */
	bool (*open_record)(void *client, ::std::byte const *rec,
						::std::size_t rec_size, ::std::byte *inner,
						sw_record_result *rr,
						::std::cxx_std_error *err) noexcept {};
	/* stash plaintext the caller's buffer could not take */
	void (*rx_stash)(void *client, ::std::byte const *pt, ::std::size_t n,
					 ::std::cxx_std_error *err) noexcept {};
	void (*finish)(io_uring_tls_sw_recv_cookie *self,
				   ::std::cxx_std_error err) noexcept {};
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status,
												   typename allocator_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};

	inline io_uring_tls_sw_recv_cookie(::fast_io::linux_io_uring_observer s,
									   void *c, int f,
									   ::std::byte *b, ::std::size_t bs,
									   ::fast_io::io_scatter_t const *sc, ::std::size_t nsc,
									   ::fast_io::posix_statx_timestamp_opt tmo) noexcept
		: sched{s}, fd{f}, client{c}, scatters{sc}, nscatters{nsc}, buf{b},
		  buf_size{bs}, timeout{tmo}
	{
	}
};

/* typed callbacks carried on the sw-recv base: record open and the
   rx-pending stash both go through the client; terminal delivery adapts
   the functor's signature */
template <typename client_t>
inline bool io_uring_tls_sw_open_record_cb(void *cc, ::std::byte const *rec,
										   ::std::size_t rec_size, ::std::byte *inner,
										   sw_record_result *rr,
										   ::std::cxx_std_error *err) noexcept
{
	try
	{
		*rr = tls_client_sw_open_record(static_cast<client_t *>(cc), rec, rec_size, inner);
		return true;
	}
	catch throws(::std::error e)
	{
		*err = e.release();
		return false;
	}
}

template <typename client_t>
inline void io_uring_tls_sw_rx_stash_cb(void *cc, ::std::byte const *pt,
										::std::size_t n,
										::std::cxx_std_error *err) noexcept
{
	try
	{
		tls_client_rx_pending_stash(static_cast<client_t *>(cc), pt, n);
	}
	catch throws(::std::error e)
	{
		*err = e.release();
	}
}

template <typename client_t, typename func>
struct io_uring_tls_sw_recv_cookie_of;

template <typename client_t, typename func>
inline void io_uring_tls_sw_recv_finish_cb(io_uring_tls_sw_recv_cookie *self,
										   ::std::cxx_std_error err) noexcept
{
	auto *cookie{static_cast<io_uring_tls_sw_recv_cookie_of<client_t, func> *>(self)};
	auto callback{::std::move(cookie->callback)};
	if constexpr (::std::is_invocable_v<func, ::std::cxx_std_error,
										::fast_io::io_scatter_status_t>)
	{
		::fast_io::io_scatter_status_t const status{
			::fast_io::scatter_size_to_status(cookie->transferred, cookie->scatters,
											  cookie->nscatters)};
		::fast_io::details::async_delete_state(cookie);
		callback(err, status);
	}
	else
	{
		::std::size_t const transferred{cookie->transferred};
		::fast_io::details::async_delete_state(cookie);
		callback(err, transferred);
	}
}

template <typename client_t, typename func>
struct io_uring_tls_sw_recv_cookie_of : io_uring_tls_sw_recv_cookie
{
	func callback;

	inline io_uring_tls_sw_recv_cookie_of(::fast_io::linux_io_uring_observer s,
										  client_t *c,
										  ::std::byte *b, ::std::size_t bs,
										  ::fast_io::io_scatter_t const *sc, ::std::size_t nsc,
										  ::fast_io::posix_statx_timestamp_opt tmo,
										  func &&cb) noexcept
		: io_uring_tls_sw_recv_cookie{s, c, c->sock_.fd, b, bs, sc, nsc, tmo},
		  callback{::std::move(cb)}
	{
		this->open_record = &io_uring_tls_sw_open_record_cb<client_t>;
		this->rx_stash = &io_uring_tls_sw_rx_stash_cb<client_t>;
		this->finish = &io_uring_tls_sw_recv_finish_cb<client_t, func>;
	}
};

inline void io_uring_tls_sw_recv_finish(
	io_uring_tls_sw_recv_cookie *cookie,
	::std::cxx_std_error err) noexcept
{
	cookie->finish(cookie, err);
}

inline void io_uring_tls_sw_recv_deliver(void *self) noexcept;

/* copy plaintext into the caller's scalar buffer or scatter walk */
inline ::std::size_t io_uring_tls_sw_recv_copy_out(
	io_uring_tls_sw_recv_cookie *cookie,
	::std::byte const *src, ::std::size_t n) noexcept
{
	if (cookie->scatters == nullptr)
	{
		::std::size_t const take{n < cookie->buf_size ? n : cookie->buf_size};
		__builtin_memcpy(cookie->buf, src, take);
		return take;
	}
	::std::size_t done{};
	for (::std::size_t i{}; i != cookie->nscatters && done < n; ++i)
	{
		auto const &e{cookie->scatters[i]};
		::std::size_t const room{n - done};
		::std::size_t const take{e.len < room ? e.len : room};
		__builtin_memcpy(const_cast<::std::byte *>(static_cast<::std::byte const *>(e.base)),
						 src + done, take);
		done += take;
	}
	return done;
}

/* arm one READ round into the ciphertext tail -- bounded to the framed
   record's own end: bytes past it belong to the next record and would
   be freed with the op */
inline void io_uring_tls_sw_recv_round(
	io_uring_tls_sw_recv_cookie *cookie) FAST_IO_HERBCEPTIONS_THROWS
{
	auto &ring{*cookie->sched.ring};
	::fast_io::liburing::details::io_uring_reserve_sqes(ring,
														cookie->timeout.has_opt ? 2 : 1);
	auto *sqe{::fast_io::liburing::io_uring_get_sqe(ring)};
	::fast_io::liburing::io_uring_prep_rw(::fast_io::liburing::io_uring_op_read, sqe,
										  cookie->fd, cookie->ct + cookie->ct_have,
										  static_cast<::std::uint_least32_t>(
											  ::fast_io::tls::details::tls_record_read_need(
												  cookie->ct, cookie->ct_have)),
										  0);
	::fast_io::liburing::details::io_uring_arm_timeout(
		ring, cookie, sqe, io_uring_tls_sw_recv_deliver,
		cookie->timeout);
	::fast_io::liburing::details::io_uring_commit(ring);
}

inline void io_uring_tls_sw_recv_deliver(void *self) noexcept
{
	auto *cookie{static_cast<io_uring_tls_sw_recv_cookie *>(self)};
	if (cookie->tlink.fired)
	{
		io_uring_tls_sw_recv_finish(
			cookie, ::fast_io::details::async_make_error(::std::errc::timed_out));
		return;
	}
	if (cookie->errn != 0)
	{
		io_uring_tls_sw_recv_finish(
			cookie, ::fast_io::liburing::details::io_uring_cqe_error(false, cookie->errn));
		return;
	}
	cookie->ct_have += cookie->res_got;
	for (;;)
	{
		/* frame one record */
		::std::size_t const reclen{
			::fast_io::tls::details::tls_record_frame(cookie->ct, cookie->ct_have)};
		if (reclen == ~static_cast<::std::size_t>(0))
		{
			io_uring_tls_sw_recv_finish(
				cookie, ::fast_io::tls::details::tls_alert_error(
							2, static_cast<::std::uint_least8_t>(
								   ::fast_io::tls::alert_description::record_overflow)));
			return;
		}
		if (reclen == 0)
		{
			break;
		}
		sw_record_result rr{};
		::std::cxx_std_error operr{};
		if (!cookie->open_record(cookie->client, cookie->ct, reclen, cookie->inner,
								 __builtin_addressof(rr), __builtin_addressof(operr)))
		{
			io_uring_tls_sw_recv_finish(cookie, operr);
			return;
		}
		/* consume the record: compact the accumulator */
		cookie->ct_have -= reclen;
		if (cookie->ct_have != 0)
		{
			__builtin_memmove(cookie->ct, cookie->ct + reclen, cookie->ct_have);
		}
		if (rr.eof)
		{
			io_uring_tls_sw_recv_finish(cookie, ::std::cxx_std_error{});
			return;
		}
		if (rr.inner != ::fast_io::tls::content_type::application_data)
		{
			continue; /* control record consumed -- pump on */
		}
		::std::size_t const delivered{
			io_uring_tls_sw_recv_copy_out(cookie, cookie->inner, rr.plaintext_size)};
		if (rr.plaintext_size > delivered)
		{
			::std::cxx_std_error serr{};
			cookie->rx_stash(cookie->client, cookie->inner + delivered,
							 rr.plaintext_size - delivered, __builtin_addressof(serr));
			if (serr.domain != nullptr)
			{
				io_uring_tls_sw_recv_finish(cookie, serr);
				return;
			}
		}
		cookie->transferred = delivered;
		io_uring_tls_sw_recv_finish(cookie, ::std::cxx_std_error{});
		return;
	}
	/* need more wire bytes -- resubmit a fresh round on this cookie */
	try
	{
		cookie->tlink.pending = 1;
		cookie->tlink.fired = false;
		io_uring_tls_sw_recv_round(cookie);
	}
	catch throws(::std::error e)
	{
		auto err{e.release()};
		io_uring_tls_sw_recv_finish(cookie, err);
	}
}

inline void io_uring_tls_sw_recv_invoke(void *self, ::std::size_t transferred, int errn) noexcept
{
	auto *cookie{static_cast<io_uring_tls_sw_recv_cookie *>(self)};
	cookie->res_got = transferred;
	cookie->errn = errn;
	if (--cookie->tlink.pending == 0)
	{
		io_uring_tls_sw_recv_deliver(cookie);
	}
}

template <typename client_t, typename func>
inline void io_uring_tls_sw_recv_submit(
	::fast_io::linux_io_uring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	client_t *client, ::std::byte *buf,
	::std::size_t buf_size, ::fast_io::io_scatter_t const *scatters,
	::std::size_t nscatters, func callback) noexcept
{
	using cookie_type =
		io_uring_tls_sw_recv_cookie_of<client_t, ::std::remove_cvref_t<func>>;
	::std::cxx_std_error err{};
	::std::size_t drained{};
	try
	{
		/* decrypted leftover beats the wire -- deliver without a sqe */
		if (scatters != nullptr)
		{
			drained = ::fast_io::tls::details::tls_client_rx_pending_drain(client, scatters, nscatters);
		}
		else
		{
			drained = ::fast_io::tls::details::tls_client_rx_pending_drain(client, buf, buf_size);
		}
		if (drained == 0)
		{
			::fast_io::liburing::details::io_uring_submit_guard<cookie_type> guard{
				::fast_io::liburing::details::io_uring_new_state<cookie_type>(
					sched, sched, client, buf, buf_size, scatters, nscatters, timeout,
					::std::move(callback))};
			guard.cookie->invoke = io_uring_tls_sw_recv_invoke;
			io_uring_tls_sw_recv_round(guard.cookie);
			guard.release();
			return;
		}
	}
	catch throws(::std::error e)
	{
		err = e.release();
	}
	if constexpr (::std::is_invocable_v<::std::remove_cvref_t<func>,
										::std::cxx_std_error,
										::fast_io::io_scatter_status_t>)
	{
		callback(err,
				 drained != 0 ? ::fast_io::scatter_size_to_status(drained, scatters, nscatters)
							  : ::fast_io::io_scatter_status_t{});
	}
	else
	{
		callback(err, drained);
	}
}

/*
One pending sw write: the plaintext is sealed into the cookie's record
buffer at submit, then IORING_OP_WRITE ships it -- a short ciphertext
write resubmits the remainder (records are atomic on the wire). The
callback reports the plaintext consumed, not the ciphertext sent.
*/
struct io_uring_tls_sw_write_cookie
{
	using allocator_type = ::fast_io::native_global_allocator;
	static inline constexpr bool alloc_with_status{allocator_type::has_status};
	::fast_io::liburing::details::io_uring_invoke_func invoke;
	::fast_io::liburing::details::io_uring_timeout_link_block tlink;
	::std::size_t sent{};    /* ciphertext committed so far */
	::std::size_t res_got{}; /* ciphertext the latest cqe committed */
	int errn{};
	::fast_io::liburing::io_uring_timespec ts{};
	::fast_io::linux_io_uring_observer sched{};
	int fd{};
	::std::size_t plaintext{};
	::fast_io::posix_statx_timestamp_opt timeout{};
	::std::size_t rec_size{};
	::std::byte rec[::fast_io::tls::details::record_header_size + (1u << 14u) + 1 + 16];
	void (*finish)(io_uring_tls_sw_write_cookie *self,
				   ::std::cxx_std_error err) noexcept {};
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status,
												   typename allocator_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};

	inline io_uring_tls_sw_write_cookie(::fast_io::linux_io_uring_observer s,
										int f,
										::fast_io::posix_statx_timestamp_opt tmo) noexcept
		: sched{s}, fd{f}, timeout{tmo}
	{
	}
};

template <typename client_t, typename func>
struct io_uring_tls_sw_write_cookie_of;

template <typename client_t, typename func>
inline void io_uring_tls_sw_write_finish_cb(io_uring_tls_sw_write_cookie *self,
											::std::cxx_std_error err) noexcept
{
	auto *cookie{static_cast<io_uring_tls_sw_write_cookie_of<client_t, func> *>(self)};
	auto callback{::std::move(cookie->callback)};
	::std::size_t const n{cookie->plaintext};
	if constexpr (::std::is_invocable_v<func, ::std::cxx_std_error,
										::fast_io::io_scatter_status_t>)
	{
		::fast_io::details::async_delete_state(cookie);
		callback(err, ::fast_io::io_scatter_status_t{n, 0zu});
	}
	else
	{
		::fast_io::details::async_delete_state(cookie);
		callback(err, n);
	}
}

template <typename client_t, typename func>
struct io_uring_tls_sw_write_cookie_of : io_uring_tls_sw_write_cookie
{
	func callback;

	inline io_uring_tls_sw_write_cookie_of(::fast_io::linux_io_uring_observer s,
										   client_t *c,
										   ::fast_io::posix_statx_timestamp_opt tmo,
										   func &&cb) noexcept
		: io_uring_tls_sw_write_cookie{s, c->sock_.fd, tmo},
		  callback{::std::move(cb)}
	{
		this->finish = &io_uring_tls_sw_write_finish_cb<client_t, func>;
	}
};

inline void io_uring_tls_sw_write_finish(
	io_uring_tls_sw_write_cookie *cookie,
	::std::cxx_std_error err) noexcept
{
	cookie->finish(cookie, err);
}

inline void io_uring_tls_sw_write_deliver(void *self) noexcept;

inline void io_uring_tls_sw_write_round(
	io_uring_tls_sw_write_cookie *cookie) FAST_IO_HERBCEPTIONS_THROWS
{
	auto &ring{*cookie->sched.ring};
	::fast_io::liburing::details::io_uring_reserve_sqes(ring,
														cookie->timeout.has_opt ? 2 : 1);
	auto *sqe{::fast_io::liburing::io_uring_get_sqe(ring)};
	::fast_io::liburing::io_uring_prep_rw(::fast_io::liburing::io_uring_op_write, sqe,
										  cookie->fd, cookie->rec + cookie->sent,
										  static_cast<::std::uint_least32_t>(
											  cookie->rec_size - cookie->sent),
										  0);
	::fast_io::liburing::details::io_uring_arm_timeout(
		ring, cookie, sqe, io_uring_tls_sw_write_deliver,
		cookie->timeout);
	::fast_io::liburing::details::io_uring_commit(ring);
}

inline void io_uring_tls_sw_write_deliver(void *self) noexcept
{
	auto *cookie{static_cast<io_uring_tls_sw_write_cookie *>(self)};
	if (cookie->tlink.fired)
	{
		io_uring_tls_sw_write_finish(
			cookie, ::fast_io::details::async_make_error(::std::errc::timed_out));
		return;
	}
	if (cookie->errn != 0)
	{
		io_uring_tls_sw_write_finish(
			cookie, ::fast_io::liburing::details::io_uring_cqe_error(false, cookie->errn));
		return;
	}
	cookie->sent += cookie->res_got;
	if (cookie->sent == cookie->rec_size)
	{
		io_uring_tls_sw_write_finish(cookie, ::std::cxx_std_error{});
		return;
	}
	/* short ciphertext write -- ship the rest of the record */
	try
	{
		cookie->tlink.pending = 1;
		cookie->tlink.fired = false;
		io_uring_tls_sw_write_round(cookie);
	}
	catch throws(::std::error e)
	{
		auto err{e.release()};
		io_uring_tls_sw_write_finish(cookie, err);
	}
}

inline void io_uring_tls_sw_write_invoke(void *self, ::std::size_t transferred, int errn) noexcept
{
	auto *cookie{static_cast<io_uring_tls_sw_write_cookie *>(self)};
	cookie->res_got = transferred;
	cookie->errn = errn;
	if (--cookie->tlink.pending == 0)
	{
		io_uring_tls_sw_write_deliver(cookie);
	}
}

template <typename client_t, typename func>
inline void io_uring_tls_sw_write_submit(
	::fast_io::linux_io_uring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	client_t *client, ::std::byte const *first,
	::std::size_t count, ::fast_io::io_scatter_t const *scatters, ::std::size_t nscatters,
	func callback) noexcept
{
	using cookie_type =
		io_uring_tls_sw_write_cookie_of<client_t, ::std::remove_cvref_t<func>>;
	try
	{
		::fast_io::liburing::details::io_uring_submit_guard<cookie_type> guard{
			::fast_io::liburing::details::io_uring_new_state<cookie_type>(
				sched, sched, client, timeout, ::std::move(callback))};
		guard.cookie->invoke = io_uring_tls_sw_write_invoke;
		::fast_io::io_scatter_t const one{first, count};
		auto const rr{::fast_io::tls::details::tls_client_sw_seal_appdata(client, guard.cookie->rec,
																		  scatters != nullptr ? scatters
																							  : __builtin_addressof(one),
																		  scatters != nullptr ? nscatters : 1zu)};
		guard.cookie->rec_size = rr.wire_size;
		guard.cookie->plaintext = rr.plaintext_consumed;
		io_uring_tls_sw_write_round(guard.cookie);
		guard.release();
	}
	catch throws(::std::error e)
	{
		auto err{e.release()};
		if constexpr (::std::is_invocable_v<::std::remove_cvref_t<func>,
											::std::cxx_std_error,
											::fast_io::io_scatter_status_t>)
		{
			callback(err, ::fast_io::io_scatter_status_t{});
		}
		else
		{
			callback(err, ::std::size_t{});
		}
	}
}

/* ---------------- io_uring kTLS close path ---------------- */

/*
Pending kTLS close on io_uring: an optional close_notify SENDMSG leg
(established sessions only) followed by IORING_OP_CLOSE on the fd. The
notify leg is best-effort -- any cqe result still proceeds to close, so
the descriptor is consumed either way. `closing` selects which leg a
completion belongs to; a timeout-cancelled close is finished inline,
matching the backend's own close submission. Each leg re-arms the
caller's timeout.
*/
struct io_uring_tls_close_cookie
{
	using allocator_type = ::fast_io::native_global_allocator;
	static inline constexpr bool alloc_with_status{allocator_type::has_status};
	::fast_io::liburing::details::io_uring_invoke_func invoke;
	::fast_io::liburing::details::io_uring_timeout_link_block tlink;
	int errn{};
	::fast_io::liburing::io_uring_timespec ts{};
	::fast_io::linux_io_uring_observer sched{};
	int fd{};
	void *client{};
	::fast_io::posix_statx_timestamp_opt timeout{};
	bool closing{};                                      /* false: close_notify in flight; true: close in flight */
	bool sw{};                                           /* true: notify leg is a sealed-record WRITE, not sendmsg */
	::std::byte body[2]{::std::byte{1}, ::std::byte{0}}; /* warning + close_notify */
	::std::size_t notify_size{};
	::std::byte notify_rec[::fast_io::tls::details::record_header_size + 3 + 16]{};
	/* seal the close_notify record under the stream's tx keys */
	::std::size_t (*seal_alert)(void *client, ::std::byte *out,
								alert_description desc) noexcept {};
	void (*finish)(io_uring_tls_close_cookie *self,
				   ::std::cxx_std_error err) noexcept {};
	ktls_iovec iov{};
	ktls_msghdr msg{};
	alignas(::std::size_t)::std::byte control[ktls_cmsg_space];
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status,
												   typename allocator_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};

	inline io_uring_tls_close_cookie(::fast_io::linux_io_uring_observer s,
									 void *c, int f,
									 ::fast_io::posix_statx_timestamp_opt tmo) noexcept
		: sched{s}, fd{f}, client{c}, timeout{tmo}
	{
	}
};

template <typename client_t>
inline ::std::size_t io_uring_tls_close_seal_alert_cb(void *cc, ::std::byte *out,
													  alert_description desc) noexcept
{
	return tls_client_sw_seal_alert(static_cast<client_t *>(cc), out, desc);
}

template <typename client_t, typename func>
struct io_uring_tls_close_cookie_of;

template <typename client_t, typename func>
inline void io_uring_tls_close_finish_cb(io_uring_tls_close_cookie *self,
										 ::std::cxx_std_error err) noexcept
{
	auto *cookie{static_cast<io_uring_tls_close_cookie_of<client_t, func> *>(self)};
	auto callback{::std::move(cookie->callback)};
	::fast_io::details::async_delete_state(cookie);
	callback(err);
}

template <typename client_t, typename func>
struct io_uring_tls_close_cookie_of : io_uring_tls_close_cookie
{
	func callback;

	inline io_uring_tls_close_cookie_of(::fast_io::linux_io_uring_observer s,
										client_t *c,
										::fast_io::posix_statx_timestamp_opt tmo,
										func &&cb) noexcept
		: io_uring_tls_close_cookie{s, c, c->sock_.fd, tmo},
		  callback{::std::move(cb)}
	{
		this->seal_alert = &io_uring_tls_close_seal_alert_cb<client_t>;
		this->finish = &io_uring_tls_close_finish_cb<client_t, func>;
	}
};

inline void io_uring_tls_close_deliver(void *self) noexcept;

/* arm the close leg: IORING_OP_CLOSE plus the adjacent link-timeout */
inline void io_uring_tls_close_arm_close(
	io_uring_tls_close_cookie *cookie) FAST_IO_HERBCEPTIONS_THROWS
{
	auto &ring{*cookie->sched.ring};
	::fast_io::liburing::details::io_uring_reserve_sqes(ring,
														cookie->timeout.has_opt ? 2 : 1);
	auto *sqe{::fast_io::liburing::io_uring_get_sqe(ring)};
	::fast_io::liburing::io_uring_prep_rw(::fast_io::liburing::io_uring_op_close, sqe,
										  cookie->fd, nullptr, 0, 0);
	::fast_io::liburing::details::io_uring_arm_timeout(
		ring, cookie, sqe, io_uring_tls_close_deliver,
		cookie->timeout);
	::fast_io::liburing::details::io_uring_commit(ring);
}

/* arm the notify leg: offloaded -> IORING_OP_SENDMSG carrying a
   TLS_SET_RECORD_TYPE alert cmsg plus {warning, close_notify}; userspace
   mode -> a plain WRITE of the pre-sealed alert record */
inline void io_uring_tls_close_arm_notify(
	io_uring_tls_close_cookie *cookie) FAST_IO_HERBCEPTIONS_THROWS
{
	auto &ring{*cookie->sched.ring};
	::fast_io::liburing::details::io_uring_reserve_sqes(ring,
														cookie->timeout.has_opt ? 2 : 1);
	auto *sqe{::fast_io::liburing::io_uring_get_sqe(ring)};
	if (cookie->sw)
	{
		cookie->notify_size = cookie->seal_alert(cookie->client,
												 cookie->notify_rec, alert_description::close_notify);
		::fast_io::liburing::io_uring_prep_rw(::fast_io::liburing::io_uring_op_write, sqe,
											  cookie->fd, cookie->notify_rec,
											  static_cast<::std::uint_least32_t>(
												  cookie->notify_size),
											  0);
	}
	else
	{
		ktls_fill_send_msghdr(cookie->msg, cookie->iov, cookie->control,
							  content_type::alert, cookie->body, 2);
		::fast_io::liburing::io_uring_prep_sendmsg(sqe, cookie->fd,
												   __builtin_addressof(cookie->msg), 0);
	}
	::fast_io::liburing::details::io_uring_arm_timeout(
		ring, cookie, sqe, io_uring_tls_close_deliver,
		cookie->timeout);
	::fast_io::liburing::details::io_uring_commit(ring);
}

inline void io_uring_tls_close_deliver(void *self) noexcept
{
	auto *cookie{static_cast<io_uring_tls_close_cookie *>(self)};
	if (!cookie->closing)
	{
		/* close_notify leg finished -- the result is advisory; proceed
		   to the close regardless */
		cookie->closing = true;
		cookie->errn = 0;
		try
		{
			io_uring_tls_close_arm_close(cookie);
		}
		catch throws(::std::error e)
		{
			/* close submission failed -- the op owns the descriptor, so
			   consume it inline rather than leak it */
			::fast_io::details::sys_close(cookie->fd);
			cookie->finish(cookie, e.release());
		}
		return;
	}
	::std::cxx_std_error const err{
		::fast_io::liburing::details::io_uring_cqe_error(cookie->tlink.fired, cookie->errn)};
	bool const cancelled{cookie->tlink.fired};
	if (cancelled) [[unlikely]]
	{
		/* the linked timeout aborted the close before it ran -- the
		   descriptor is still open, so finish it inline */
		::fast_io::details::sys_close(cookie->fd);
	}
	cookie->finish(cookie, err);
}

inline void io_uring_tls_close_invoke(void *self, ::std::size_t, int errn) noexcept
{
	auto *cookie{static_cast<io_uring_tls_close_cookie *>(self)};
	cookie->errn = errn;
	if (--cookie->tlink.pending == 0)
	{
		io_uring_tls_close_deliver(cookie);
	}
}

template <typename client_t, typename func>
inline void io_uring_tls_close_submit(
	::fast_io::linux_io_uring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	client_t *client, func callback) noexcept
{
	using cookie_type =
		io_uring_tls_close_cookie_of<client_t, ::std::remove_cvref_t<func>>;
	int const fd{client->sock_.fd};
	try
	{
		::fast_io::liburing::details::io_uring_submit_guard<cookie_type> guard{
			::fast_io::liburing::details::io_uring_new_state<cookie_type>(
				sched, sched, client, timeout, ::std::move(callback))};
		guard.cookie->invoke = io_uring_tls_close_invoke;
		guard.cookie->sw = client->established_ && !client->offloaded_;
		if (client->established_)
		{
			io_uring_tls_close_arm_notify(guard.cookie);
		}
		else
		{
			guard.cookie->closing = true;
			io_uring_tls_close_arm_close(guard.cookie);
		}
		guard.release();
	}
	catch throws(::std::error e)
	{
		/* submission failure: the op still owns the handle -- close it
		   inline so it cannot leak */
		::fast_io::details::sys_close(fd);
		callback(e.release());
	}
}

} // namespace details

/*
 * io_uring async read on an established kTLS stream: IORING_OP_RECVMSG
 * with a TLS_GET_RECORD_TYPE control buffer, pumped per-record until
 * application data or a terminal condition. cb is invoked once as
 * cb(::std::cxx_std_error, ::std::size_t) noexcept. Positional reads
 * report illegal_seek -- TLS is a stream, matching the sync socket's
 * ESPIPE. A peer alert arrives as a tls_alert-domain error; close_notify
 * is a 0-byte success (eof).
 */
template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto,
		  typename func>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_pread_some_bytes_underflow_callback_define(
	::fast_io::linux_io_uring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob, ::std::byte *first,
	::std::size_t count, ::fast_io::intfpos_opt off, func callback) noexcept
{
	::std::cxx_std_error err{};
	if (off.has_opt)
	{
		err = ::fast_io::details::async_make_error(::std::errc::invalid_seek);
	}
	else if (count != 0)
	{
		if (tob.handle->offloaded_)
		{
			::fast_io::tls::details::io_uring_tls_recv_submit(sched, timeout, tob.handle, first,
															  count, nullptr, 0,
															  ::std::move(callback));
		}
		else
		{
			::fast_io::tls::details::io_uring_tls_sw_recv_submit(sched, timeout, tob.handle, first,
																 count, nullptr, 0,
																 ::std::move(callback));
		}
		return;
	}
	callback(err, ::std::size_t{});
}

/*
 * Vectored form: one recvmsg whose iov is the caller's scatter list
 * (io_scatter_t is layout-compatible with the kernel's iovec); the array
 * must outlive the operation, same contract as the io_uring backend's
 * own scatter submit. cb is invoked once as cb(::std::cxx_std_error,
 * io_scatter_status_t) noexcept.
 */
template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto,
		  typename func>
	requires ::fast_io::operations::decay::defines::async_scatter_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_scatter_pread_some_bytes_underflow_callback_define(
	::fast_io::linux_io_uring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob,
	::fast_io::io_scatter_t const *scatters, ::std::size_t n, ::fast_io::intfpos_opt off,
	func callback) noexcept
{
	::std::cxx_std_error err{};
	::std::size_t const neff{
		n < ::fast_io::liburing::details::io_uring_max_iov
			? n
			: ::fast_io::liburing::details::io_uring_max_iov};
	if (off.has_opt)
	{
		err = ::fast_io::details::async_make_error(::std::errc::invalid_seek);
	}
	else if (neff != 0)
	{
		if (tob.handle->offloaded_)
		{
			::fast_io::tls::details::io_uring_tls_recv_submit(sched, timeout, tob.handle, nullptr,
															  0, scatters, neff,
															  ::std::move(callback));
		}
		else
		{
			::fast_io::tls::details::io_uring_tls_sw_recv_submit(sched, timeout, tob.handle, nullptr,
																 0, scatters, neff,
																 ::std::move(callback));
		}
		return;
	}
	callback(err, ::fast_io::io_scatter_status_t{});
}

/*
 * io_uring async write: offloaded streams write the fd directly (the
 * kernel seals records); userspace-mode streams seal one record into the
 * cookie and WRITE it. cb is invoked once as cb(::std::cxx_std_error,
 * ::std::size_t) noexcept -- the size is plaintext consumed. Positional
 * writes report invalid_seek.
 */
template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto,
		  typename func>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_pwrite_some_bytes_overflow_callback_define(
	::fast_io::linux_io_uring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob, ::std::byte const *first,
	::std::size_t count, ::fast_io::intfpos_opt off, func callback) noexcept
{
	::std::cxx_std_error err{};
	if (off.has_opt)
	{
		err = ::fast_io::details::async_make_error(::std::errc::invalid_seek);
	}
	else if (tob.handle->offloaded_)
	{
		async_pwrite_some_bytes_overflow_callback_define(
			sched, timeout, tob.handle->sock_, first, count, off, ::std::move(callback));
		return;
	}
	else if (count != 0)
	{
		::fast_io::tls::details::io_uring_tls_sw_write_submit(sched, timeout, tob.handle, first,
															  count, nullptr, 0,
															  ::std::move(callback));
		return;
	}
	callback(err, ::std::size_t{});
}

/* vectored form -- cb(::std::cxx_std_error, io_scatter_status_t) */
template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto,
		  typename func>
	requires ::fast_io::operations::decay::defines::async_scatter_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_scatter_pwrite_some_bytes_overflow_callback_define(
	::fast_io::linux_io_uring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob,
	::fast_io::io_scatter_t const *scatters, ::std::size_t n, ::fast_io::intfpos_opt off,
	func callback) noexcept
{
	::std::cxx_std_error err{};
	::std::size_t const neff{
		n < ::fast_io::liburing::details::io_uring_max_iov
			? n
			: ::fast_io::liburing::details::io_uring_max_iov};
	if (off.has_opt)
	{
		err = ::fast_io::details::async_make_error(::std::errc::invalid_seek);
	}
	else if (tob.handle->offloaded_)
	{
		async_scatter_pwrite_some_bytes_overflow_callback_define(
			sched, timeout, tob.handle->sock_, scatters, n, off, ::std::move(callback));
		return;
	}
	else if (neff != 0)
	{
		::fast_io::tls::details::io_uring_tls_sw_write_submit(sched, timeout, tob.handle, nullptr,
															  0, scatters, neff,
															  ::std::move(callback));
		return;
	}
	callback(err, ::fast_io::io_scatter_status_t{});
}

/*
 * io_uring async close: a TLS close is close_notify followed by the fd
 * close -- a best-effort alert SENDMSG leg (skipped when the session was
 * never established), then IORING_OP_CLOSE. cb is invoked once as
 * cb(::std::cxx_std_error) noexcept; the descriptor is consumed on every
 * path, including submission failure and a fired timeout.
 */
template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto,
		  typename func>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_close_define(
	::fast_io::linux_io_uring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob, func callback) noexcept
{
	::fast_io::tls::details::io_uring_tls_close_submit(sched, timeout, tob.handle,
													   ::std::move(callback));
}

} // namespace fast_io::tls

#endif /* __linux__ */

#if defined(__HERBCEPTIONS__)

namespace fast_io::tls
{

/*
 * Fallback async close for schedulers without a dedicated TLS path:
 * emit close_notify inline -- send_close_notify is noexcept and
 * best-effort -- then forward the fd close to the bound socket's own
 * async_close_define. tls_generic_sched marks schedulers that carry
 * their own define (io_uring, the pools), so this only fills gaps.
 */
template <typename async_scheduler_type, ::std::integral ch_type, typename allocator_type,
		  typename socket_observer_type, typename crypto,
		  typename func>
	requires(::fast_io::tls::details::tls_generic_sched<::std::remove_cvref_t<async_scheduler_type>>)
inline void async_close_define(
	async_scheduler_type sched, ::fast_io::posix_statx_timestamp_opt timeout,
	basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob, func callback) noexcept
{
	if (tob.handle->established_)
	{
		::fast_io::tls::details::tls_client_send_close_notify(tob.handle);
	}
	async_close_define(sched, timeout, tob.handle->sock_, ::std::move(callback));
}

} // namespace fast_io::tls

#endif /* __HERBCEPTIONS__ */

#if (!defined(_WIN32) || defined(__WINE__)) && !defined(__MSDOS__) && !defined(__wasi__) && defined(__HERBCEPTIONS__)

namespace fast_io::tls
{

namespace details
{

/*
handshake cookie for schedulers that run blocking work: the pool worker
handshake() executes synchronously. The hostname is copied into
the cookie so the caller's string need not outlive submission.
*/
/*
handshake cookie for schedulers that run blocking work: the pool worker
runs handshake() synchronously. Client type and functor are erased onto
the base -- run/dispatch emit once, only the per-func dispatch thunk is
typed. The hostname is copied into the cookie so the caller's string
need not outlive submission.
*/
inline void posix_thread_pool_tls_handshake_run(
	::fast_io::details::posix_thread_pool_node *p) noexcept;
inline void posix_thread_pool_tls_server_handshake_run(
	::fast_io::details::posix_thread_pool_node *p) noexcept;
inline void posix_thread_pool_tls_close_run(
	::fast_io::details::posix_thread_pool_node *p) noexcept;

struct posix_thread_pool_tls_handshake_cookie_base : ::fast_io::details::posix_thread_pool_node
{
	using allocator_type = ::fast_io::native_global_allocator;
	void *client{};
	::fast_io::u8string hostname;
	::timespec deadline{};
	bool has_deadline{};
	::std::cxx_std_error err{};
	void (*handshake_fn)(void *client, ::fast_io::u8cstring_view host,
						 ::std::cxx_std_error *err) noexcept {};

	inline posix_thread_pool_tls_handshake_cookie_base(
		void *c, ::fast_io::u8cstring_view host,
		::fast_io::posix_statx_timestamp_opt timeout) FAST_IO_HERBCEPTIONS_THROWS
		: client{c},
		  hostname{host.data(), host.data() + host.size()},
		  has_deadline{timeout.has_opt}
	{
		if (timeout.has_opt)
		{
			this->deadline = ::fast_io::details::posix_thread_pool_deadline(timeout.opt);
		}
	}
};

template <typename client_t>
inline void posix_thread_pool_tls_handshake_cb(void *cc,
											   ::fast_io::u8cstring_view h,
											   ::std::cxx_std_error *err) noexcept
{
	try
	{
		tls_client_handshake(static_cast<client_t *>(cc), h);
	}
	catch throws(::std::error e)
	{
		*err = e.release();
	}
}

template <typename client_t, typename func>
struct posix_thread_pool_tls_handshake_cookie;

template <typename client_t, typename func>
inline void posix_thread_pool_tls_handshake_dispatch_cb(
	::fast_io::details::posix_thread_pool_node *p) noexcept
{
	auto *self{static_cast<posix_thread_pool_tls_handshake_cookie<client_t, func> *>(p)};
	auto callback{::std::move(self->callback)};
	auto err{self->err};
	::fast_io::details::async_delete_state(self);
	callback(err);
}

template <typename client_t, typename func>
struct posix_thread_pool_tls_handshake_cookie
	: posix_thread_pool_tls_handshake_cookie_base
{
	func callback;

	inline posix_thread_pool_tls_handshake_cookie(
		client_t *c, ::fast_io::u8cstring_view host,
		::fast_io::posix_statx_timestamp_opt timeout,
		func &&cb) FAST_IO_HERBCEPTIONS_THROWS
		: posix_thread_pool_tls_handshake_cookie_base{c, host, timeout},
		  callback{::std::move(cb)}
	{
		this->handshake_fn = &posix_thread_pool_tls_handshake_cb<client_t>;
		this->run = posix_thread_pool_tls_handshake_run;
		this->dispatch = &posix_thread_pool_tls_handshake_dispatch_cb<client_t, func>;
	}
};

inline void posix_thread_pool_tls_handshake_run(
	::fast_io::details::posix_thread_pool_node *p) noexcept
{
	auto *self{
		static_cast<posix_thread_pool_tls_handshake_cookie_base *>(p)};
	if (self->has_deadline &&
		::fast_io::details::posix_thread_pool_expired(self->deadline))
	{
		self->err = ::fast_io::details::async_make_error(::std::errc::timed_out);
		return;
	}
	self->handshake_fn(self->client,
					   ::fast_io::u8cstring_view{::fast_io::freestanding::from_range,
												 self->hostname},
					   __builtin_addressof(self->err));
}

/*
server-side sibling: same worker model, but the argument is a
tls_server_config -- the shallow copy is safe because the sync driver
borrows the same caller-owned DER pointers.
*/
struct posix_thread_pool_tls_server_handshake_cookie_base : ::fast_io::details::posix_thread_pool_node
{
	using allocator_type = ::fast_io::native_global_allocator;
	void *client{};
	tls_server_config cfg{};
	::timespec deadline{};
	bool has_deadline{};
	::std::cxx_std_error err{};
	void (*handshake_fn)(void *client, tls_server_config const *cfg,
						 ::std::cxx_std_error *err) noexcept {};

	inline posix_thread_pool_tls_server_handshake_cookie_base(
		void *c, tls_server_config const &server_cfg,
		::fast_io::posix_statx_timestamp_opt timeout) noexcept
		: client{c}, cfg{server_cfg}, has_deadline{timeout.has_opt}
	{
		if (timeout.has_opt)
		{
			this->deadline = ::fast_io::details::posix_thread_pool_deadline(timeout.opt);
		}
	}
};

template <typename client_t>
inline void posix_thread_pool_tls_server_handshake_cb(void *cc, tls_server_config const *cfg,
													  ::std::cxx_std_error *err) noexcept
{
	try
	{
		tls_server_handshake(static_cast<client_t *>(cc), cfg);
	}
	catch throws(::std::error e)
	{
		*err = e.release();
	}
}

template <typename client_t, typename func>
struct posix_thread_pool_tls_server_handshake_cookie;

template <typename client_t, typename func>
inline void posix_thread_pool_tls_server_handshake_dispatch_cb(
	::fast_io::details::posix_thread_pool_node *p) noexcept
{
	auto *self{static_cast<posix_thread_pool_tls_server_handshake_cookie<client_t, func> *>(p)};
	auto callback{::std::move(self->callback)};
	auto err{self->err};
	::fast_io::details::async_delete_state(self);
	callback(err);
}

template <typename client_t, typename func>
struct posix_thread_pool_tls_server_handshake_cookie
	: posix_thread_pool_tls_server_handshake_cookie_base
{
	func callback;

	inline posix_thread_pool_tls_server_handshake_cookie(
		client_t *c, tls_server_config const &server_cfg,
		::fast_io::posix_statx_timestamp_opt timeout,
		func &&cb) FAST_IO_HERBCEPTIONS_THROWS
		: posix_thread_pool_tls_server_handshake_cookie_base{c, server_cfg, timeout},
		  callback{::std::move(cb)}
	{
		this->handshake_fn = &posix_thread_pool_tls_server_handshake_cb<client_t>;
		this->run = posix_thread_pool_tls_server_handshake_run;
		this->dispatch = &posix_thread_pool_tls_server_handshake_dispatch_cb<client_t, func>;
	}
};

inline void posix_thread_pool_tls_server_handshake_run(
	::fast_io::details::posix_thread_pool_node *p) noexcept
{
	auto *self{
		static_cast<posix_thread_pool_tls_server_handshake_cookie_base *>(p)};
	if (self->has_deadline &&
		::fast_io::details::posix_thread_pool_expired(self->deadline))
	{
		self->err = ::fast_io::details::async_make_error(::std::errc::timed_out);
		return;
	}
	self->handshake_fn(self->client, __builtin_addressof(self->cfg),
					   __builtin_addressof(self->err));
}

/*
 * close cookie for the pool: the worker emits close_notify (a blocking
 * sendmsg is fine on a parked worker) then closes the fd. The deadline
 * applies only until dequeue; an expired op still consumes the fd, it
 * merely reports timed_out -- same contract as the pool's plain close.
 */
struct posix_thread_pool_tls_close_cookie_base : ::fast_io::details::posix_thread_pool_node
{
	using allocator_type = ::fast_io::native_global_allocator;
	void *client{};
	int fd{};
	::timespec deadline{};
	bool has_deadline{};
	::std::cxx_std_error err{};
	void (*close_notify)(void *client, ::std::cxx_std_error *err) noexcept {};

	inline posix_thread_pool_tls_close_cookie_base(
		void *c, int f,
		::fast_io::posix_statx_timestamp_opt timeout) noexcept
		: client{c}, fd{f}, has_deadline{timeout.has_opt}
	{
		if (timeout.has_opt)
		{
			this->deadline = ::fast_io::details::posix_thread_pool_deadline(timeout.opt);
		}
	}
};

template <typename client_t>
inline void posix_thread_pool_tls_close_notify_cb(void *cc,
												  ::std::cxx_std_error *err) noexcept
{
	try
	{
		tls_client_send_close_notify(static_cast<client_t *>(cc));
	}
	catch throws(::std::error e)
	{
		*err = e.release();
	}
}

template <typename client_t, typename func>
struct posix_thread_pool_tls_close_cookie;

template <typename client_t, typename func>
inline void posix_thread_pool_tls_close_dispatch_cb(
	::fast_io::details::posix_thread_pool_node *p) noexcept
{
	auto *self{static_cast<posix_thread_pool_tls_close_cookie<client_t, func> *>(p)};
	auto callback{::std::move(self->callback)};
	auto err{self->err};
	::fast_io::details::async_delete_state(self);
	callback(err);
}

template <typename client_t, typename func>
struct posix_thread_pool_tls_close_cookie
	: posix_thread_pool_tls_close_cookie_base
{
	func callback;

	inline posix_thread_pool_tls_close_cookie(
		client_t *c,
		::fast_io::posix_statx_timestamp_opt timeout, func &&cb) noexcept
		: posix_thread_pool_tls_close_cookie_base{c, c->sock_.fd, timeout},
		  callback{::std::move(cb)}
	{
		this->close_notify = &posix_thread_pool_tls_close_notify_cb<client_t>;
		this->run = posix_thread_pool_tls_close_run;
		this->dispatch = &posix_thread_pool_tls_close_dispatch_cb<client_t, func>;
	}
};

inline void posix_thread_pool_tls_close_run(
	::fast_io::details::posix_thread_pool_node *p) noexcept
{
	auto *self{
		static_cast<posix_thread_pool_tls_close_cookie_base *>(p)};
	try
	{
		if (self->has_deadline &&
			::fast_io::details::posix_thread_pool_expired(self->deadline))
		{
			::fast_io::details::sys_close(self->fd);
			self->err = ::fast_io::details::async_make_error(::std::errc::timed_out);
			return;
		}
		self->close_notify(self->client, __builtin_addressof(self->err));
		if (self->err.domain != nullptr)
		{
			return;
		}
		::fast_io::details::sys_close_throw_error(self->fd);
	}
	catch throws(::std::error e)
	{
		self->err = e.release();
	}
}

} // namespace details

/*
 * async_handshake_callback_define (thread-pool backend): the worker runs
 * the blocking userspace handshake — the TLS exchange is a sequence of
 * dependent record round-trips plus certificate verification, so a parked
 * worker is the honest async model here, same as the pool's async_connect.
 * cb is invoked once as cb(::std::cxx_std_error): domain == nullptr means
 * the stream is established and ready for application I/O.
 */
template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto,
		  typename func>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_handshake_callback_define(
	::fast_io::posix_thread_pool_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob, ::fast_io::u8cstring_view hostname,
	func callback) noexcept
{
	using client_type = ::std::remove_cvref_t<decltype(*tob.handle)>;
	using cookie_type =
		::fast_io::tls::details::posix_thread_pool_tls_handshake_cookie<client_type,
																		::std::remove_cvref_t<func>>;
	try
	{
		auto *cookie{::fast_io::details::async_new_state_plain<cookie_type>(
			sched, tob.handle, hostname, timeout, ::std::move(callback))};
		::fast_io::details::posix_thread_pool_submit(sched.native_handle(), cookie);
	}
	catch throws(::std::error e)
	{
		callback(e.release());
	}
}

/*
 * server-side async_handshake_callback_define: same worker model, the
 * argument is a tls_server_config. The shallow copy keeps the
 * caller-owned DER lifetime contract of the sync handshake.
 */
template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto,
		  typename func>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_handshake_callback_define(
	::fast_io::posix_thread_pool_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob, tls_server_config cfg,
	func callback) noexcept
{
	using client_type = ::std::remove_cvref_t<decltype(*tob.handle)>;
	using cookie_type =
		::fast_io::tls::details::posix_thread_pool_tls_server_handshake_cookie<client_type,
																			   ::std::remove_cvref_t<func>>;
	try
	{
		auto *cookie{::fast_io::details::async_new_state_plain<cookie_type>(
			sched, tob.handle, cfg, timeout, ::std::move(callback))};
		::fast_io::details::posix_thread_pool_submit(sched.native_handle(), cookie);
	}
	catch throws(::std::error e)
	{
		callback(e.release());
	}
}

/*
 * async_close_define (thread-pool backend): a worker emits close_notify
 * then closes the fd — the TLS shutdown stays off the submission thread.
 * cb is invoked once as cb(::std::cxx_std_error) noexcept; the fd is
 * consumed on every path, including an expired deadline.
 */
template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto,
		  typename func>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_close_define(
	::fast_io::posix_thread_pool_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob, func callback) noexcept
{
	using client_type = ::std::remove_cvref_t<decltype(*tob.handle)>;
	using cookie_type =
		::fast_io::tls::details::posix_thread_pool_tls_close_cookie<client_type,
																	::std::remove_cvref_t<func>>;
	try
	{
		auto *cookie{::fast_io::details::async_new_state_plain<cookie_type>(
			sched, tob.handle, timeout, ::std::move(callback))};
		::fast_io::details::posix_thread_pool_submit(sched.native_handle(), cookie);
	}
	catch throws(::std::error e)
	{
		/* submission failure: the op still owns the fd — close it
		   inline so it cannot leak */
		::fast_io::details::sys_close(tob.handle->sock_.fd);
		callback(e.release());
	}
}

} // namespace fast_io::tls


#endif
