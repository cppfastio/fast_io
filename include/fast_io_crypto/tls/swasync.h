#pragma once

/*
Scheduler-agnostic async TLS for the userspace record layer.

Wherever no backend-specific TLS path exists (win32 IOCP, any future
scheduler) the record pump runs between the socket's own async byte
ops: ciphertext arrives through async socket reads, records are framed
and unsealed in userspace, and the completion sees plaintext. Each op
owns one heap state that outlives resubmission rounds; a round submits
a single socket async read/write and the completion re-enters the pump.

kTLS can't ride this path -- read() on an offloaded socket is EIO;
records need recvmsg. Offloaded reads therefore fail fast here, while
offloaded writes forward plaintext (the kernel seals it). The sw-mode
stream gets the full pump.
*/

namespace fast_io::tls::details
{

/* backends owning a dedicated TLS async path opt out of the generic
   sw machinery (io_uring has the recvmsg op; the posix pool runs the
   sync ops on a worker) */
template <typename sched_t>
inline constexpr bool tls_generic_sched{true};
#if defined(__linux__)
template <>
inline constexpr bool tls_generic_sched<::fast_io::linux_io_uring_observer>{false};
#endif
#if (!defined(_WIN32) || defined(__WINE__)) && !defined(__MSDOS__) && !defined(__wasi__) && defined(__HERBCEPTIONS__)
template <>
inline constexpr bool tls_generic_sched<::fast_io::posix_thread_pool_observer>{false};
#endif
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
template <>
inline constexpr bool tls_generic_sched<::fast_io::win32_thread_pool_observer>{false};
#endif


/* deliver a byte-count completion to either callback shape
   (scalar size_t or io_scatter_status_t) */
template <typename func>
inline void tls_sw_cb_deliver(func &&cb, ::std::cxx_std_error err, ::std::size_t n,
							  ::fast_io::io_scatter_t const *sc,
							  ::std::size_t nsc) noexcept
{
	if constexpr (::std::is_invocable_v<func, ::std::cxx_std_error,
										::fast_io::io_scatter_status_t>)
	{
		cb(err, ::fast_io::scatter_size_to_status(n, sc, nsc));
	}
	else
	{
		cb(err, n);
	}
}

/* socket byte-op completions ride the shared untyped functor
   (::fast_io::details::async_bytes_step_cb): a lambda would mint a
   fresh closure type -- and a whole rw-cookie instantiation -- per
   enclosing template arg pack. st plus its typed step function recover
   everything the completion needs. */

/* ---------------- generic sw recv ---------------- */

template <typename sched_t, typename client_t, typename alloc_type>
struct tls_sw_recv_state_base
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{allocator_type::has_status};

	sched_t sched;
	::fast_io::posix_statx_timestamp_opt timeout;
	client_t *client;
	::std::byte *buf{};
	::std::size_t buf_size{};
	::fast_io::io_scatter_t const *scatters{};
	::std::size_t nscatters{};
	::std::byte ct[tls_max_record];
	::std::size_t ct_have{};
	::std::byte inner[tls_max_ciphertext];
	/* terminal delivery: frees the derived state, invokes the functor in
	   its own signature (scalar n or io_scatter_status_t) -- the pump
	   never sees the callback type */
	void (*finish)(tls_sw_recv_state_base *st, ::std::cxx_std_error err,
				   ::std::size_t delivered) noexcept {};
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status,
												   typename allocator_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

template <typename sched_t, typename client_t, typename func, typename alloc_type>
struct tls_sw_recv_state;

template <typename sched_t, typename client_t, typename func, typename alloc_type>
inline void tls_sw_recv_finish_cb(
	tls_sw_recv_state_base<sched_t, client_t, alloc_type> *st, ::std::cxx_std_error err,
	::std::size_t delivered) noexcept
{
	auto *self{static_cast<tls_sw_recv_state<sched_t, client_t, func, alloc_type> *>(st)};
	auto cb{::std::move(self->callback)};
	::fast_io::io_scatter_status_t st_status{};
	if (st->scatters != nullptr)
	{
		st_status = ::fast_io::scatter_size_to_status(delivered, st->scatters, st->nscatters);
	}
	::fast_io::details::async_delete_state(self);
	if constexpr (::std::is_invocable_v<func, ::std::cxx_std_error,
										::fast_io::io_scatter_status_t>)
	{
		cb(err, st_status);
	}
	else
	{
		cb(err, delivered);
	}
}

template <typename sched_t, typename client_t, typename func, typename alloc_type>
struct tls_sw_recv_state : tls_sw_recv_state_base<sched_t, client_t, alloc_type>
{
	using base_type = tls_sw_recv_state_base<sched_t, client_t, alloc_type>;
	func callback;

	inline tls_sw_recv_state(sched_t s, ::fast_io::posix_statx_timestamp_opt to,
							 client_t *c, ::std::byte *b, ::std::size_t n,
							 ::fast_io::io_scatter_t const *sc, ::std::size_t nsc,
							 func cb) noexcept
		: base_type{s, to, c, b, n, sc, nsc, {}, {}, {}, {}, {}}, callback{::std::move(cb)}
	{
		this->finish = &tls_sw_recv_finish_cb<sched_t, client_t, func, alloc_type>;
	}
};

template <typename sched_t, typename client_t, typename alloc_type>
inline void tls_sw_recv_finish(
	tls_sw_recv_state_base<sched_t, client_t, alloc_type> *st, ::std::cxx_std_error err,
	::std::size_t delivered) noexcept
{
	st->finish(st, err, delivered);
}

/* copy plaintext into the scalar or scatter target */
template <typename sched_t, typename client_t, typename alloc_type>
inline ::std::size_t tls_sw_copy_out(
	tls_sw_recv_state_base<sched_t, client_t, alloc_type> *st, ::std::byte const *src,
	::std::size_t n) noexcept
{
	if (st->buf != nullptr)
	{
		::std::size_t const take{n < st->buf_size ? n : st->buf_size};
		::fast_io::freestanding::non_overlapped_copy_n(src, take, st->buf);
		return take;
	}
	::std::size_t copied{};
	for (::std::size_t i{}; i != st->nscatters && copied != n; ++i)
	{
		::std::size_t const cap{st->scatters[i].len};
		::std::size_t const take{n - copied < cap ? n - copied : cap};
		::fast_io::freestanding::non_overlapped_copy_n(
			src + copied, take,
			static_cast<::std::byte *>(const_cast<void *>(st->scatters[i].base)));
		copied += take;
	}
	return copied;
}

template <typename sched_t, typename client_t, typename alloc_type>
inline void tls_sw_recv_step(void *vst, ::std::cxx_std_error err, ::std::size_t n) noexcept
{
	auto *st{static_cast<tls_sw_recv_state_base<sched_t, client_t, alloc_type> *>(vst)};
	if (err.domain != nullptr)
	{
		tls_sw_recv_finish(st, err, 0);
		return;
	}
	if (n == 0)
	{
		tls_sw_recv_finish(st, ::std::cxx_std_error{}, 0);
		return;
	}
	st->ct_have += n;
	tls_sw_recv_pump(st);
}

template <typename sched_t, typename client_t, typename alloc_type>
inline void tls_sw_recv_round(
	tls_sw_recv_state_base<sched_t, client_t, alloc_type> *st) noexcept
{
	/* the backend define is noexcept -- submission failures arrive
	   through the callback it was given. the round is bounded to the
	   framed record's own end -- bytes past it belong to the next
	   record and would be freed with the op */
	async_pread_some_bytes_underflow_callback_define(
		st->sched, st->timeout, st->client->sock_,
		st->ct + st->ct_have,
		::fast_io::tls::details::tls_record_read_need(st->ct, st->ct_have),
		::fast_io::intfpos_opt{},
		::fast_io::details::async_bytes_step_cb{st, &tls_sw_recv_step<sched_t, client_t, alloc_type>});
}

/* drain pending plaintext -> frame -> open -> dispatch; resubmits a
   socket read whenever the accumulator can't frame a whole record. */
template <typename sched_t, typename client_t, typename alloc_type>
inline void tls_sw_recv_pump(
	tls_sw_recv_state_base<sched_t, client_t, alloc_type> *st) noexcept
{
	for (;;)
	{
		::std::size_t drained{};
		if (st->buf != nullptr)
		{
			drained = ::fast_io::tls::details::tls_client_rx_pending_drain(st->client, st->buf, st->buf_size);
		}
		else
		{
			drained = ::fast_io::tls::details::tls_client_rx_pending_drain(st->client, st->scatters, st->nscatters);
		}
		if (drained != 0)
		{
			tls_sw_recv_finish(st, ::std::cxx_std_error{}, drained);
			return;
		}
		::std::size_t const reclen{tls_record_frame(st->ct, st->ct_have)};
		if (reclen == ~static_cast<::std::size_t>(0))
		{
			tls_sw_recv_finish(
				st,
				tls_alert_error(2,
								static_cast<::std::uint_least8_t>(
									alert_description::record_overflow)),
				0);
			return;
		}
		if (reclen == 0)
		{
			tls_sw_recv_round(st);
			return;
		}
		FAST_IO_HERBCEPTIONS_TRY
		{
			auto const rr{::fast_io::tls::details::tls_client_sw_open_record(st->client, st->ct, reclen, st->inner)};
			st->ct_have -= reclen;
			if (st->ct_have != 0)
			{
				__builtin_memmove(st->ct, st->ct + reclen, st->ct_have);
			}
			if (rr.eof)
			{
				tls_sw_recv_finish(st, ::std::cxx_std_error{}, 0);
				return;
			}
			if (rr.inner != content_type::application_data)
			{
				continue; /* control record consumed -- pump on */
			}
			::std::size_t const delivered{tls_sw_copy_out(st, st->inner, rr.plaintext_size)};
			if (rr.plaintext_size > delivered)
			{
				::fast_io::tls::details::tls_client_rx_pending_stash(st->client, st->inner + delivered,
																	 rr.plaintext_size - delivered);
			}
			tls_sw_recv_finish(st, ::std::cxx_std_error{}, delivered);
			return;
		}
		catch throws(::std::error e)
		{
			auto err{e.release()};
			tls_sw_recv_finish(st, err, 0);
			return;
		}
		FAST_IO_HERBCEPTIONS_CATCH_ALL
		{
			tls_sw_recv_finish(
				st,
				::fast_io::details::async_make_error(::std::errc::protocol_error),
				0);
			return;
		}
	}
}

template <typename async_scheduler_type, typename client_t, typename func>
inline void tls_sw_recv_submit(
	async_scheduler_type &&sched, ::fast_io::posix_statx_timestamp_opt timeout,
	client_t *client, ::std::byte *buf, ::std::size_t buf_size,
	::fast_io::io_scatter_t const *scatters, ::std::size_t nscatters,
	func &&callback) noexcept
{
	using sched_t = ::std::remove_cvref_t<
		decltype(::fast_io::operations::async_scheduler_ref(sched))>;
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<sched_t>;
	using state_type =
		tls_sw_recv_state<sched_t, client_t, ::std::remove_cvref_t<func>, alloc_type>;
	using fcb = ::std::remove_cvref_t<func>;
	auto sched_ref{::fast_io::operations::async_scheduler_ref(sched)};
	FAST_IO_HERBCEPTIONS_TRY
	{
		tls_sw_recv_pump(::fast_io::details::async_new_state<state_type>(
			sched_ref, timeout, client, buf, buf_size, scatters,
			nscatters, static_cast<fcb &&>(callback)));
	}
	catch throws(::std::error e)
	{
		auto err{e.release()};
		tls_sw_cb_deliver(callback, err, 0, scatters, nscatters);
	}
}

/* ---------------- generic sw write ---------------- */

template <typename sched_t, typename client_t, typename alloc_type>
struct tls_sw_write_state_base
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{allocator_type::has_status};

	sched_t sched;
	::fast_io::posix_statx_timestamp_opt timeout;
	client_t *client;
	::std::byte wire[tls_max_record];
	::std::size_t wire_size{};
	::std::size_t wire_done{};
	::std::size_t plaintext_consumed{};
	::fast_io::io_scatter_t const *scatters{};
	::std::size_t nscatters{};
	void (*finish)(tls_sw_write_state_base *st, ::std::cxx_std_error err) noexcept {};
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status,
												   typename allocator_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

template <typename sched_t, typename client_t, typename func, typename alloc_type>
struct tls_sw_write_state;

template <typename sched_t, typename client_t, typename func, typename alloc_type>
inline void tls_sw_write_finish_cb(
	tls_sw_write_state_base<sched_t, client_t, alloc_type> *st,
	::std::cxx_std_error err) noexcept
{
	auto *self{static_cast<tls_sw_write_state<sched_t, client_t, func, alloc_type> *>(st)};
	auto cb{::std::move(self->callback)};
	::fast_io::details::async_delete_state(self);
	tls_sw_cb_deliver(cb, err, st->plaintext_consumed, st->scatters, st->nscatters);
}

template <typename sched_t, typename client_t, typename func, typename alloc_type>
struct tls_sw_write_state : tls_sw_write_state_base<sched_t, client_t, alloc_type>
{
	using base_type = tls_sw_write_state_base<sched_t, client_t, alloc_type>;
	func callback;

	inline tls_sw_write_state(sched_t s, ::fast_io::posix_statx_timestamp_opt to,
							  client_t *c, ::fast_io::io_scatter_t const *sc,
							  ::std::size_t nsc, func cb) noexcept
		: base_type{s, to, c, {}, 0, 0, 0, sc, nsc, {}, {}}, callback{::std::move(cb)}
	{
		this->finish = &tls_sw_write_finish_cb<sched_t, client_t, func, alloc_type>;
	}
};

template <typename sched_t, typename client_t, typename alloc_type>
inline void tls_sw_write_finish(
	tls_sw_write_state_base<sched_t, client_t, alloc_type> *st,
	::std::cxx_std_error err) noexcept
{
	st->finish(st, err);
}

template <typename sched_t, typename client_t, typename alloc_type>
inline void tls_sw_write_step(void *vst, ::std::cxx_std_error err, ::std::size_t n) noexcept
{
	auto *st{static_cast<tls_sw_write_state_base<sched_t, client_t, alloc_type> *>(vst)};
	if (err.domain != nullptr)
	{
		tls_sw_write_finish(st, err);
		return;
	}
	st->wire_done += n;
	if (st->wire_done != st->wire_size)
	{
		tls_sw_write_round(st); /* short ciphertext write */
		return;
	}
	tls_sw_write_finish(st, ::std::cxx_std_error{});
}

template <typename sched_t, typename client_t, typename alloc_type>
inline void tls_sw_write_round(
	tls_sw_write_state_base<sched_t, client_t, alloc_type> *st) noexcept
{
	async_pwrite_some_bytes_overflow_callback_define(
		st->sched, st->timeout, st->client->sock_,
		st->wire + st->wire_done, st->wire_size - st->wire_done,
		::fast_io::intfpos_opt{},
		::fast_io::details::async_bytes_step_cb{st, &tls_sw_write_step<sched_t, client_t, alloc_type>});
}

template <typename async_scheduler_type, typename client_t, typename func>
inline void tls_sw_write_submit(
	async_scheduler_type &&sched, ::fast_io::posix_statx_timestamp_opt timeout,
	client_t *client, ::std::byte const *first, ::std::size_t count,
	::fast_io::io_scatter_t const *scatters, ::std::size_t nscatters,
	func &&callback) noexcept
{
	using sched_t = ::std::remove_cvref_t<
		decltype(::fast_io::operations::async_scheduler_ref(sched))>;
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<sched_t>;
	using state_type =
		tls_sw_write_state<sched_t, client_t, ::std::remove_cvref_t<func>, alloc_type>;
	using fcb = ::std::remove_cvref_t<func>;
	auto sched_ref{::fast_io::operations::async_scheduler_ref(sched)};
	FAST_IO_HERBCEPTIONS_TRY
	{
		auto *st{::fast_io::details::async_new_state<state_type>(
			sched_ref, timeout, client, scatters, nscatters,
			static_cast<fcb &&>(callback))};
		/* seal the first record at submit; the callback reports the
		   plaintext consumed, not the ciphertext on the wire */
		::fast_io::io_scatter_t in{};
		if (scatters == nullptr)
		{
			in = {first, count};
			scatters = __builtin_addressof(in);
			nscatters = 1;
		}
		auto const rr{::fast_io::tls::details::tls_client_sw_seal_appdata(client, st->wire, scatters, nscatters)};
		st->wire_size = rr.wire_size;
		st->plaintext_consumed = rr.plaintext_consumed;
		tls_sw_write_round(st);
	}
	catch throws(::std::error e)
	{
		auto err{e.release()};
		callback(err, ::std::size_t{});
	}
}

/* ---------------- generic sw close (sealed notify, then socket close) ---------------- */

template <typename sched_t, typename client_t, typename alloc_type>
struct tls_sw_close_state_base
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{allocator_type::has_status};

	sched_t sched;
	::fast_io::posix_statx_timestamp_opt timeout;
	client_t *client;
	::std::byte wire[64];
	::std::size_t wire_size{};
	::std::size_t wire_done{};
	/* the close's terminal leg hands the functor to the socket's own
	   close define -- typed inside the derived state */
	void (*proceed)(tls_sw_close_state_base *st) noexcept {};
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status,
												   typename allocator_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

template <typename sched_t, typename client_t, typename func, typename alloc_type>
struct tls_sw_close_state;

template <typename sched_t, typename client_t, typename func, typename alloc_type>
inline void tls_sw_close_proceed_cb(
	tls_sw_close_state_base<sched_t, client_t, alloc_type> *st) noexcept
{
	auto *self{static_cast<tls_sw_close_state<sched_t, client_t, func, alloc_type> *>(st)};
	auto cb{::std::move(self->callback)};
	auto sock{st->client->sock_};
	auto sched{st->sched};
	auto timeout{st->timeout};
	::fast_io::details::async_delete_state(self);
	async_close_define(sched, timeout, sock, ::std::move(cb));
}

template <typename sched_t, typename client_t, typename func, typename alloc_type>
struct tls_sw_close_state : tls_sw_close_state_base<sched_t, client_t, alloc_type>
{
	using base_type = tls_sw_close_state_base<sched_t, client_t, alloc_type>;
	func callback;

	inline tls_sw_close_state(sched_t s, ::fast_io::posix_statx_timestamp_opt to,
							  client_t *c, func cb) noexcept
		: base_type{s, to, c, {}, 0, 0, {}, {}}, callback{::std::move(cb)}
	{
		this->proceed = &tls_sw_close_proceed_cb<sched_t, client_t, func, alloc_type>;
	}
};

/* leg 2 -- whatever the notify did, the raw transport still closes.
   The close's own result is what reaches the caller; the notify's
   error is intentionally dropped (close_notify is best-effort). */
template <typename sched_t, typename client_t, typename alloc_type>
inline void tls_sw_close_socket(
	tls_sw_close_state_base<sched_t, client_t, alloc_type> *st) noexcept
{
	st->proceed(st);
}

/* leg 1 -- the sealed close_notify write; failures go straight to leg 2 */
template <typename sched_t, typename client_t, typename alloc_type>
inline void tls_sw_close_step(void *vst, ::std::cxx_std_error err, ::std::size_t n) noexcept
{
	auto *st{static_cast<tls_sw_close_state_base<sched_t, client_t, alloc_type> *>(vst)};
	if (err.domain != nullptr)
	{
		tls_sw_close_socket(st); /* notify failed -- close anyway */
		return;
	}
	st->wire_done += n;
	if (st->wire_done != st->wire_size)
	{
		tls_sw_close_round(st);
		return;
	}
	tls_sw_close_socket(st);
}

template <typename sched_t, typename client_t, typename alloc_type>
inline void tls_sw_close_round(
	tls_sw_close_state_base<sched_t, client_t, alloc_type> *st) noexcept
{
	async_pwrite_some_bytes_overflow_callback_define(
		st->sched, st->timeout, st->client->sock_,
		st->wire + st->wire_done, st->wire_size - st->wire_done,
		::fast_io::intfpos_opt{},
		::fast_io::details::async_bytes_step_cb{st, &tls_sw_close_step<sched_t, client_t, alloc_type>});
}

#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
/* ---------------- win32 thread pool: handshake runs on a worker ---------------- */

/*
 * The TLS exchange is a sequence of dependent record round-trips plus
 * certificate verification — a parked pool worker is the honest async
 * model here, same as the posix pool's async_connect. cb is invoked
 * once as cb(::std::cxx_std_error): domain == nullptr means established.
 */
template <typename client_t, typename func>
struct tls_win32_pool_handshake_cookie : ::fast_io::details::win32_thread_pool_node
{
	using allocator_type = ::fast_io::native_global_allocator;
	client_t *client{};
	::fast_io::u8string hostname;
	::std::uint_least64_t deadline{};
	func callback;
	::std::cxx_std_error err{};

	inline tls_win32_pool_handshake_cookie(client_t *c, ::fast_io::u8cstring_view host,
										   ::fast_io::posix_statx_timestamp_opt timeout,
										   func &&cb) noexcept
		: client{c}, hostname{host}, callback{::std::move(cb)}
	{
		if (timeout.has_opt)
		{
			this->deadline =
				::fast_io::details::win32_thread_pool_deadline(timeout.opt);
		}
	}
};

template <typename client_t, typename func>
inline void tls_win32_pool_handshake_run(::fast_io::details::win32_thread_pool_node *p) noexcept
{
	auto *self{static_cast<tls_win32_pool_handshake_cookie<client_t, func> *>(p)};
	try
	{
		if (::fast_io::details::win32_thread_pool_expired(self->deadline))
		{
			self->err =
				::fast_io::details::async_make_error(::std::errc::timed_out);
			return;
		}
		::fast_io::tls::details::tls_client_handshake(self->client,
													  ::fast_io::u8cstring_view{::fast_io::freestanding::from_range,
																				self->hostname});
	}
	catch throws(::std::error e)
	{
		self->err = e.release();
	}
}

template <typename client_t, typename func>
inline void tls_win32_pool_handshake_dispatch(
	::fast_io::details::win32_thread_pool_node *p) noexcept
{
	auto *self{static_cast<tls_win32_pool_handshake_cookie<client_t, func> *>(p)};
	auto callback{::std::move(self->callback)};
	auto err{self->err};
	::fast_io::details::async_delete_state(self);
	callback(err);
}

/* server-side sibling: the argument is a tls_server_config -- the
   shallow copy keeps the caller-owned DER lifetime contract */
template <typename client_t, typename func>
struct tls_win32_pool_server_handshake_cookie : ::fast_io::details::win32_thread_pool_node
{
	using allocator_type = ::fast_io::native_global_allocator;
	client_t *client{};
	tls_server_config cfg{};
	::std::uint_least64_t deadline{};
	func callback;
	::std::cxx_std_error err{};

	inline tls_win32_pool_server_handshake_cookie(client_t *c, tls_server_config const &server_cfg,
												  ::fast_io::posix_statx_timestamp_opt timeout,
												  func &&cb) noexcept
		: client{c}, cfg{server_cfg}, callback{::std::move(cb)}
	{
		if (timeout.has_opt)
		{
			this->deadline =
				::fast_io::details::win32_thread_pool_deadline(timeout.opt);
		}
	}
};

template <typename client_t, typename func>
inline void tls_win32_pool_server_handshake_run(::fast_io::details::win32_thread_pool_node *p) noexcept
{
	auto *self{static_cast<tls_win32_pool_server_handshake_cookie<client_t, func> *>(p)};
	try
	{
		if (::fast_io::details::win32_thread_pool_expired(self->deadline))
		{
			self->err =
				::fast_io::details::async_make_error(::std::errc::timed_out);
			return;
		}
		::fast_io::tls::details::tls_server_handshake(self->client,
													  __builtin_addressof(self->cfg));
	}
	catch throws(::std::error e)
	{
		self->err = e.release();
	}
}

template <typename client_t, typename func>
inline void tls_win32_pool_server_handshake_dispatch(
	::fast_io::details::win32_thread_pool_node *p) noexcept
{
	auto *self{static_cast<tls_win32_pool_server_handshake_cookie<client_t, func> *>(p)};
	auto callback{::std::move(self->callback)};
	auto err{self->err};
	::fast_io::details::async_delete_state(self);
	callback(err);
}

#if !defined(_WIN32_WINDOWS)
/* ---------------- win11 IoRing: handshake on a worker, ferried through the ring ---------------- */

/*
 * IoRing has no socket ops — the same emulation as this backend's
 * socket read/write and close: a default-pool worker runs the
 * synchronous handshake and a no-match cancel-request sqe carries the
 * cookie back to the pump thread. The deadline applies only until the
 * work item dequeues.
 */
struct tls_ioring_handshake_state_base : ::fast_io::details::win32_ioring_state_base
{
	::fast_io::u8string hostname;
	::std::uint_least64_t deadline{};
	::std::cxx_std_error result{};
};

template <typename client_t, typename func, typename alloc_type>
struct tls_ioring_handshake_cookie
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{allocator_type::has_status};
	tls_ioring_handshake_state_base base;
	client_t *client;
	func callback;
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status,
												   typename allocator_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

template <typename alloc_type, typename client_t, typename func>
inline void tls_ioring_handshake_deliver(void *self, ::std::uintptr_t,
										 ::std::uint_least32_t) noexcept
{
	using cookie_type = tls_ioring_handshake_cookie<client_t, func, alloc_type>;
	auto *cookie{static_cast<cookie_type *>(self)};
	auto callback{::std::move(cookie->callback)};
	auto err{cookie->base.result};
	::fast_io::details::async_delete_state(cookie);
	callback(err);
}

template <typename cookie_type>
inline ::std::uint_least32_t FAST_IO_WINSTDCALL tls_ioring_handshake_work(void *context) noexcept
{
	auto *cookie{static_cast<cookie_type *>(context)};
	try
	{
		if (::fast_io::details::win32_thread_pool_expired(cookie->base.deadline))
		{
			cookie->base.result =
				::fast_io::details::async_make_error(::std::errc::timed_out);
		}
		else
		{
			::fast_io::tls::details::tls_client_handshake(cookie->client,
														  ::fast_io::u8cstring_view{::fast_io::freestanding::from_range,
																					cookie->base.hostname});
		}
	}
	catch throws(::std::error e)
	{
		cookie->base.result = e.release();
	}
	if (!::fast_io::details::win32_ioring_ferry_completion(
			__builtin_addressof(cookie->base))) [[unlikely]]
	{
		cookie->base.invoke(__builtin_addressof(cookie->base), 0, 0);
	}
	return 0;
}

/* server-side sibling of the ioring cookie: a tls_server_config rides
   in the state base instead of a hostname */
struct tls_ioring_server_handshake_state_base : ::fast_io::details::win32_ioring_state_base
{
	tls_server_config cfg{};
	::std::uint_least64_t deadline{};
	::std::cxx_std_error result{};
};

template <typename client_t, typename func, typename alloc_type>
struct tls_ioring_server_handshake_cookie
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{allocator_type::has_status};
	tls_ioring_server_handshake_state_base base;
	client_t *client;
	func callback;
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status,
												   typename allocator_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

template <typename alloc_type, typename client_t, typename func>
inline void tls_ioring_server_handshake_deliver(void *self, ::std::uintptr_t,
												::std::uint_least32_t) noexcept
{
	using cookie_type = tls_ioring_server_handshake_cookie<client_t, func, alloc_type>;
	auto *cookie{static_cast<cookie_type *>(self)};
	auto callback{::std::move(cookie->callback)};
	auto err{cookie->base.result};
	::fast_io::details::async_delete_state(cookie);
	callback(err);
}

template <typename cookie_type>
inline ::std::uint_least32_t FAST_IO_WINSTDCALL tls_ioring_server_handshake_work(void *context) noexcept
{
	auto *cookie{static_cast<cookie_type *>(context)};
	try
	{
		if (::fast_io::details::win32_thread_pool_expired(cookie->base.deadline))
		{
			cookie->base.result =
				::fast_io::details::async_make_error(::std::errc::timed_out);
		}
		else
		{
			::fast_io::tls::details::tls_server_handshake(cookie->client,
														  __builtin_addressof(cookie->base.cfg));
		}
	}
	catch throws(::std::error e)
	{
		cookie->base.result = e.release();
	}
	if (!::fast_io::details::win32_ioring_ferry_completion(
			__builtin_addressof(cookie->base))) [[unlikely]]
	{
		cookie->base.invoke(__builtin_addressof(cookie->base), 0, 0);
	}
	return 0;
}

#endif

#endif

} // namespace fast_io::tls::details

#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
namespace fast_io
{

template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto,
		  typename func>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_handshake_callback_define(
	::fast_io::win32_thread_pool_observer sched,
	::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::tls::basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob,
	::fast_io::u8cstring_view hostname, func callback) noexcept
{
	using client_type = ::std::remove_cvref_t<decltype(*tob.handle)>;
	using fcb = ::std::remove_cvref_t<func>;
	using cookie_type =
		::fast_io::tls::details::tls_win32_pool_handshake_cookie<client_type, fcb>;
	try
	{
		auto *cookie{::fast_io::details::async_new_state_plain<cookie_type>(
			sched, tob.handle, hostname, timeout, ::std::move(callback))};
		cookie->run =
			&::fast_io::tls::details::tls_win32_pool_handshake_run<client_type, fcb>;
		cookie->dispatch =
			&::fast_io::tls::details::tls_win32_pool_handshake_dispatch<client_type, fcb>;
		::fast_io::details::win32_thread_pool_submit(sched.native_handle(), cookie);
	}
	catch throws(::std::error e)
	{
		callback(e.release());
	}
}

/* server-side: the argument is a tls_server_config */
template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto,
		  typename func>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_handshake_callback_define(
	::fast_io::win32_thread_pool_observer sched,
	::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::tls::basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob,
	::fast_io::tls::tls_server_config cfg, func callback) noexcept
{
	using client_type = ::std::remove_cvref_t<decltype(*tob.handle)>;
	using fcb = ::std::remove_cvref_t<func>;
	using cookie_type =
		::fast_io::tls::details::tls_win32_pool_server_handshake_cookie<client_type, fcb>;
	try
	{
		auto *cookie{::fast_io::details::async_new_state_plain<cookie_type>(
			sched, tob.handle, cfg, timeout, ::std::move(callback))};
		cookie->run =
			&::fast_io::tls::details::tls_win32_pool_server_handshake_run<client_type, fcb>;
		cookie->dispatch =
			&::fast_io::tls::details::tls_win32_pool_server_handshake_dispatch<client_type, fcb>;
		::fast_io::details::win32_thread_pool_submit(sched.native_handle(), cookie);
	}
	catch throws(::std::error e)
	{
		callback(e.release());
	}
}

#if !defined(_WIN32_WINDOWS)
/*
 * win11 IoRing handshake: IoRing has no socket ops, so a default-pool
 * worker runs the synchronous exchange and the cookie ferries back
 * through the ring — the callback lands on the pump thread like a real
 * cqe. A rejected work item reports through the callback rather than
 * running the handshake on the submission thread.
 */
template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto,
		  typename func>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_handshake_callback_define(
	::fast_io::win32_ioring_observer sched,
	::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::tls::basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob,
	::fast_io::u8cstring_view hostname, func callback) noexcept
{
	using client_type = ::std::remove_cvref_t<decltype(*tob.handle)>;
	using fcb = ::std::remove_cvref_t<func>;
	using alloc_type =
		::fast_io::details::async_scheduler_allocator_t<::fast_io::win32_ioring_observer>;
	using cookie_type =
		::fast_io::tls::details::tls_ioring_handshake_cookie<client_type, fcb, alloc_type>;
	::std::cxx_std_error err{};
	try
	{
		auto *cookie{::fast_io::details::async_new_state_plain<cookie_type>(
			sched,
			::fast_io::tls::details::tls_ioring_handshake_state_base{
				{&::fast_io::tls::details::tls_ioring_handshake_deliver<alloc_type, client_type, fcb>,
				 sched.native_handle(),
				 reinterpret_cast<void *>(tob.handle->sock_.hsocket)},
				::fast_io::u8string{hostname},
				timeout.has_opt
					? ::fast_io::details::win32_thread_pool_deadline(timeout.opt)
					: 0u,
				{}},
			tob.handle, fcb{::std::move(callback)})};
		/* WT_EXECUTELONGFUNCTION: the handshake is several blocking
		   round-trips */
		if (::fast_io::win32::QueueUserWorkItem(
				&::fast_io::tls::details::tls_ioring_handshake_work<cookie_type>, cookie,
				0x00000010u) == 0) [[unlikely]]
		{
			auto cb{::std::move(cookie->callback)};
			::fast_io::details::async_delete_state(cookie);
			err = ::fast_io::details::async_make_error(
				static_cast<::fast_io::freestanding::win32_errc>(
					static_cast<::std::uint_least32_t>(::fast_io::win32::GetLastError())));
		}
		else
		{
			return;
		}
	}
	catch throws(::std::error e)
	{
		err = e.release();
	}
	callback(err);
}

/* server-side ioring handshake: tls_server_config rides in the state base */
template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto,
		  typename func>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_handshake_callback_define(
	::fast_io::win32_ioring_observer sched,
	::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::tls::basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob,
	::fast_io::tls::tls_server_config cfg, func callback) noexcept
{
	using client_type = ::std::remove_cvref_t<decltype(*tob.handle)>;
	using fcb = ::std::remove_cvref_t<func>;
	using alloc_type =
		::fast_io::details::async_scheduler_allocator_t<::fast_io::win32_ioring_observer>;
	using cookie_type =
		::fast_io::tls::details::tls_ioring_server_handshake_cookie<client_type, fcb, alloc_type>;
	::std::cxx_std_error err{};
	try
	{
		auto *cookie{::fast_io::details::async_new_state_plain<cookie_type>(
			sched,
			::fast_io::tls::details::tls_ioring_server_handshake_state_base{
				{&::fast_io::tls::details::tls_ioring_server_handshake_deliver<alloc_type, client_type, fcb>,
				 sched.native_handle(),
				 reinterpret_cast<void *>(tob.handle->sock_.hsocket)},
				cfg,
				timeout.has_opt
					? ::fast_io::details::win32_thread_pool_deadline(timeout.opt)
					: 0u,
				{}},
			tob.handle, fcb{::std::move(callback)})};
		if (::fast_io::win32::QueueUserWorkItem(
				&::fast_io::tls::details::tls_ioring_server_handshake_work<cookie_type>, cookie,
				0x00000010u) == 0) [[unlikely]]
		{
			auto cb{::std::move(cookie->callback)};
			::fast_io::details::async_delete_state(cookie);
			err = ::fast_io::details::async_make_error(
				static_cast<::fast_io::freestanding::win32_errc>(
					static_cast<::std::uint_least32_t>(::fast_io::win32::GetLastError())));
		}
		else
		{
			return;
		}
	}
	catch throws(::std::error e)
	{
		err = e.release();
	}
	callback(err);
}
#endif

} // namespace fast_io
#endif

namespace fast_io::operations::decay::defines
{

template <typename async_scheduler_type, typename streamtype, typename argtype, typename callback_type>
concept has_async_handshake_callback_define = requires(async_scheduler_type sched,
													   ::fast_io::posix_statx_timestamp_opt timeout,
													   streamtype stm, argtype arg,
													   callback_type callback) {
	async_handshake_callback_define(sched, timeout, stm, arg, callback);
};

} // namespace fast_io::operations::decay::defines

namespace fast_io::details
{

/*
handshake argument lifetime: the caller's view (hostname) decays to an
owned copy; value arguments (tls_server_config) store as-is.
*/
template <typename T>
struct async_handshake_stored_arg
{
	using type = ::std::remove_cvref_t<T>;
	static constexpr type store(type t) noexcept(noexcept(type{static_cast<type &&>(t)}))
	{
		return static_cast<type &&>(t);
	}
};
template <>
struct async_handshake_stored_arg<::fast_io::u8cstring_view>
{
	using type = ::fast_io::u8string;
	static type store(::fast_io::u8cstring_view v) FAST_IO_HERBCEPTIONS_THROWS
	{
		return type{v.data(), v.data() + v.size()};
	}
};

/* the submit-time shape of a stored argument: u8string goes back out
   as the view the define wants, everything else forwards as stored */
template <typename stored_t>
struct async_handshake_submit_arg
{
	static constexpr stored_t const &get(stored_t const &t) noexcept
	{
		return t;
	}
};
template <>
struct async_handshake_submit_arg<::fast_io::u8string>
{
	static ::fast_io::u8cstring_view get(::fast_io::u8string const &t) noexcept
	{
		return ::fast_io::u8cstring_view{::fast_io::freestanding::from_range, t};
	}
};

} // namespace fast_io::details

namespace fast_io::operations::decay
{

/*
 * Public callback entry for an asynchronous TLS handshake: submits the
 * stream's async_handshake_callback_define, found by ADL on the
 * scheduler/stream types. instm is reduced to its io_stream_ref first,
 * so basic_tls and buffered TLS streams arrive as the observer. The arg
 * is the handshake parameter -- u8cstring_view hostname for a client,
 * tls_server_config for a server.
 *
 * The functor is invoked once as callback(::std::cxx_std_error)
 * noexcept: domain == nullptr means the handshake completed and the
 * stream carries established application traffic keys.
 */
template <typename async_scheduler_type, typename streamtype, typename argtype, typename callback_type>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
				 ::std::remove_cvref_t<callback_type>> &&
			 ::fast_io::operations::decay::defines::has_async_handshake_callback_define<
				 async_scheduler_type, streamtype, argtype, ::std::remove_cvref_t<callback_type>>
inline void async_handshake_decay_callback(async_scheduler_type scheduler,
										   ::fast_io::posix_statx_timestamp_opt timeout,
										   streamtype stm, argtype arg,
										   callback_type callback) noexcept
{
	async_handshake_callback_define(scheduler, timeout, stm, arg, ::std::move(callback));
}

} // namespace fast_io::operations::decay

namespace fast_io::details
{

/*
 * coroutine awaiter for async_handshake_decay. The argument is stored
 * in the awaiter — the co_await expression's temporaries are gone by
 * the time await_suspend submits.
 */
template <typename scheduler, typename streamtype, typename stored_arg>
struct async_handshake_awaiter : async_awaiter_result<void>
{
	scheduler sched;
	streamtype stm;
	stored_arg arg;
	::fast_io::posix_statx_timestamp_opt timeout;
	inline bool await_suspend(::std::coroutine_handle<> h) noexcept
	{
		this->coro = h;
		::fast_io::operations::decay::async_handshake_decay_callback(
			sched, timeout, stm,
			::fast_io::details::async_handshake_submit_arg<stored_arg>::get(arg),
			::fast_io::details::async_awaiter_callback<void>{this});
		return this->async_suspend_done();
	}
	inline constexpr void await_resume() FAST_IO_HERBCEPTIONS_THROWS
	{
		if (this->err.domain != nullptr)
		{
			auto e{this->err};
			this->err = {};
			throw throws e;
		}
	}
};

} // namespace fast_io::details

namespace fast_io::operations::decay
{

/*
 * Coroutine form of async_handshake: suspends until the handshake
 * resolves; await_resume() rethrows the error through the channel.
 */
template <typename async_scheduler_type, typename streamtype, typename argtype>
inline auto async_handshake_decay(async_scheduler_type scheduler,
								  ::fast_io::posix_statx_timestamp_opt timeout, streamtype stm,
								  argtype arg) noexcept
{
	using traits = ::fast_io::details::async_handshake_stored_arg<::std::remove_cvref_t<argtype>>;
	return ::fast_io::details::async_handshake_awaiter<async_scheduler_type, streamtype,
													   typename traits::type>{
		{}, scheduler, stm, traits::store(arg), timeout};
}

} // namespace fast_io::operations::decay


namespace fast_io::operations
{

/*
 * async_handshake: drives a TLS 1.3 handshake on the stream without
 * blocking the caller — on the thread-pool scheduler a worker runs the
 * synchronous handshake; backends without a blocking-work facility do
 * not satisfy the constraint. The stream object must outlive the
 * operation.
 *
 * Callback form invokes callback(::std::cxx_std_error) once; the
 * coroutine form suspends until the handshake resolves and
 * await_resume() rethrows the error through the channel.
 */
template <typename async_scheduler_type, typename streamtype, typename argtype>
inline auto async_handshake(async_scheduler_type &&scheduler,
							::fast_io::posix_statx_timestamp_opt timeout, streamtype &&stm,
							argtype arg) noexcept
	requires(::fast_io::operations::decay::defines::has_async_handshake_callback_define<
			 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
			 ::std::remove_cvref_t<decltype(::fast_io::operations::io_stream_ref(stm))>,
			 decltype(::fast_io::details::async_handshake_submit_arg<
					  ::std::remove_cvref_t<argtype>>::get(*static_cast<::std::remove_cvref_t<argtype> const *>(nullptr))),
			 ::fast_io::details::async_io_error_callback>)
{
	return ::fast_io::operations::decay::async_handshake_decay(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::io_stream_ref(stm), arg);
}

template <typename async_scheduler_type, typename streamtype, typename argtype, typename callback_type>
inline void async_handshake_callback(async_scheduler_type &&scheduler,
									 ::fast_io::posix_statx_timestamp_opt timeout, streamtype &&stm,
									 argtype arg,
									 callback_type callback) noexcept
	requires(::fast_io::operations::decay::defines::has_async_handshake_callback_define<
			 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
			 ::std::remove_cvref_t<decltype(::fast_io::operations::io_stream_ref(stm))>,
			 decltype(::fast_io::details::async_handshake_submit_arg<
					  ::std::remove_cvref_t<argtype>>::get(*static_cast<::std::remove_cvref_t<argtype> const *>(nullptr))),
			 ::std::remove_cvref_t<callback_type>>)
{
	::fast_io::operations::decay::async_handshake_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::io_stream_ref(stm), arg, ::std::move(callback));
}

} // namespace fast_io::operations


/* ---------------- public defines (non-dedicated backends) ---------------- */

/*
These match TLS streams on any scheduler except the ones with a
dedicated path (io_uring recvmsg machinery; posix_thread_pool's generic
sync-worker). Backends without a TLS define end up here.
*/


namespace fast_io
{

template <typename async_scheduler_type, ::std::integral ch_type, typename allocator_type,
		  typename socket_observer_type, typename crypto, typename func>
	requires(::fast_io::tls::details::tls_generic_sched<
			 ::std::remove_cvref_t<async_scheduler_type>>)
inline void async_pread_some_bytes_underflow_callback_define(
	async_scheduler_type &&sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::tls::basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob,
	::std::byte *first, ::std::size_t count, ::fast_io::intfpos_opt off,
	func callback) noexcept
{
	if (off.has_opt || tob.handle->offloaded_)
	{
		callback(::fast_io::details::async_make_error(::std::errc::io_error),
				 ::std::size_t{});
		return;
	}
	::fast_io::tls::details::tls_sw_recv_submit(sched, timeout, tob.handle, first,
												count, nullptr, 0,
												::std::move(callback));
}

template <typename async_scheduler_type, ::std::integral ch_type, typename allocator_type,
		  typename socket_observer_type, typename crypto, typename func>
	requires(::fast_io::tls::details::tls_generic_sched<
			 ::std::remove_cvref_t<async_scheduler_type>>)
inline void async_scatter_pread_some_bytes_underflow_callback_define(
	async_scheduler_type &&sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::tls::basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob,
	::fast_io::io_scatter_t const *pscatters, ::std::size_t n,
	::fast_io::intfpos_opt off, func callback) noexcept
{
	if (off.has_opt || tob.handle->offloaded_)
	{
		callback(::fast_io::details::async_make_error(::std::errc::io_error),
				 ::fast_io::io_scatter_status_t{});
		return;
	}
	::fast_io::tls::details::tls_sw_recv_submit(sched, timeout, tob.handle, nullptr,
												0, pscatters, n,
												::std::move(callback));
}

template <typename async_scheduler_type, ::std::integral ch_type, typename allocator_type,
		  typename socket_observer_type, typename crypto, typename func>
	requires(::fast_io::tls::details::tls_generic_sched<
			 ::std::remove_cvref_t<async_scheduler_type>>)
inline void async_pwrite_some_bytes_overflow_callback_define(
	async_scheduler_type &&sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::tls::basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob,
	::std::byte const *first, ::std::size_t count, ::fast_io::intfpos_opt off,
	func callback) noexcept
{
	if (off.has_opt)
	{
		callback(::fast_io::details::async_make_error(::std::errc::invalid_seek),
				 ::std::size_t{});
		return;
	}
	if (tob.handle->offloaded_)
	{
		/* plaintext write -- the kernel seals the records */
		async_pwrite_some_bytes_overflow_callback_define(
			sched, timeout, tob.handle->sock_, first, count, off,
			::std::move(callback));
		return;
	}
	::fast_io::tls::details::tls_sw_write_submit(sched, timeout, tob.handle, first,
												 count, nullptr, 0,
												 ::std::move(callback));
}

template <typename async_scheduler_type, ::std::integral ch_type, typename allocator_type,
		  typename socket_observer_type, typename crypto, typename func>
	requires(::fast_io::tls::details::tls_generic_sched<
			 ::std::remove_cvref_t<async_scheduler_type>>)
inline void async_scatter_pwrite_some_bytes_overflow_callback_define(
	async_scheduler_type &&sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::tls::basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob,
	::fast_io::io_scatter_t const *pscatters, ::std::size_t n,
	::fast_io::intfpos_opt off, func callback) noexcept
{
	if (off.has_opt)
	{
		callback(::fast_io::details::async_make_error(::std::errc::invalid_seek),
				 ::fast_io::io_scatter_status_t{});
		return;
	}
	if (tob.handle->offloaded_)
	{
		async_scatter_pwrite_some_bytes_overflow_callback_define(
			sched, timeout, tob.handle->sock_, pscatters, n, off,
			::std::move(callback));
		return;
	}
	::fast_io::tls::details::tls_sw_write_submit(sched, timeout, tob.handle, nullptr,
												 0, pscatters, n,
												 ::std::move(callback));
}

template <typename async_scheduler_type, ::std::integral ch_type, typename allocator_type,
		  typename socket_observer_type, typename crypto, typename func>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_close_define(
	async_scheduler_type &&sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::tls::basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob,
	func callback) noexcept
{
	auto *client{tob.handle};
	auto const sock{client->sock_};
	::std::cxx_std_error err{};
	if (client->offloaded_)
	{
		/* cmsg close_notify is a cheap best-effort syscall; then the
		   socket's own async close takes the fd */
		::fast_io::tls::details::tls_client_send_close_notify(client);
		FAST_IO_HERBCEPTIONS_TRY
		{
			async_close_define(sched, timeout, sock, ::std::move(callback));
			return;
		}
		catch throws(::std::error e)
		{
			err = e.release();
		}
	}
	else
	{
		using sched_t = ::std::remove_cvref_t<
			decltype(::fast_io::operations::async_scheduler_ref(sched))>;
		using alloc_type = ::fast_io::details::async_scheduler_allocator_t<sched_t>;
		using state_type = ::fast_io::tls::details::tls_sw_close_state<
			sched_t, ::std::remove_cvref_t<decltype(*client)>, ::std::remove_cvref_t<func>,
			alloc_type>;
		auto sched_ref{::fast_io::operations::async_scheduler_ref(sched)};
		FAST_IO_HERBCEPTIONS_TRY
		{
			auto *st{::fast_io::details::async_new_state<state_type>(
				sched_ref, timeout, client, ::std::move(callback))};
			st->wire_size = ::fast_io::tls::details::tls_client_sw_seal_alert(client, st->wire, ::fast_io::tls::alert_description::close_notify);
			::fast_io::tls::details::tls_sw_close_round(st);
			return;
		}
		catch throws(::std::error e)
		{
			err = e.release();
		}
		FAST_IO_HERBCEPTIONS_CATCH_ALL
		{
			err = ::fast_io::details::async_make_error(::std::errc::protocol_error);
		}
	}
	callback(err);
}

/*
tls<->tls transmit: kernel splice only when both directions are
offloaded (the fd carries plaintext only while SOL_TLS owns the record
layer). A userspace-mode stream feeds/consumes ciphertext, so the
generic bounce-buffer emulation takes over -- it drives this observer's
async read/write defines, which seal/open records correctly per mode.
*/
template <typename async_scheduler_type, ::std::integral ch_type_out, typename allocator_type_out,
		  typename socket_observer_type_out, typename crypto_out, ::std::integral ch_type_in,
		  typename allocator_type_in, typename socket_observer_type_in, typename crypto_in, typename func>
inline void async_transmit_some_bytes_overflow_underflow_callback_define(
	async_scheduler_type sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::tls::basic_tls_io_observer<ch_type_out, allocator_type_out, socket_observer_type_out, crypto_out> outstm,
	::fast_io::intfpos_opt off_out,
	::fast_io::tls::basic_tls_io_observer<ch_type_in, allocator_type_in, socket_observer_type_in, crypto_in> instm,
	::fast_io::intfpos_opt off_in, ::fast_io::size_t_opt bound, func callback) noexcept
{
	if (outstm.handle->offloaded_ && instm.handle->offloaded_)
	{
		async_transmit_some_bytes_overflow_underflow_callback_define(
			sched, timeout, outstm.handle->sock_, off_out, instm.handle->sock_, off_in,
			bound, ::std::move(callback));
		return;
	}
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<
		::std::remove_cvref_t<async_scheduler_type>>;
	using state_type = ::fast_io::details::async_transmit_bytes_state<
		false, ::std::remove_cvref_t<async_scheduler_type>,
		::fast_io::tls::basic_tls_io_observer<ch_type_out, allocator_type_out, socket_observer_type_out, crypto_out>,
		::fast_io::tls::basic_tls_io_observer<ch_type_in, allocator_type_in, socket_observer_type_in, crypto_in>, alloc_type,
		::std::remove_cvref_t<func>>;
	::std::size_t const remaining{bound.has_opt ? bound.opt
												: ::std::numeric_limits<::std::size_t>::max()};
	FAST_IO_HERBCEPTIONS_TRY
	{
		::fast_io::details::async_transmit_bytes_read_round(
			::fast_io::details::async_new_state<state_type>(
				sched, outstm, off_out, instm, off_in, remaining, timeout,
				::std::move(callback), 0zu, 0zu, 0zu));
	}
	catch throws(::std::error e)
	{
		callback(e.release(), 0zu);
	}
}

} // namespace fast_io
