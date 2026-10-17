#pragma once

/*
schannel async: the record layer is synchronous CPU work on buffered
ciphertext -- async is only the transport underneath. A pending-read
submits socket reads until DecryptMessage yields plaintext; a
pending-write runs EncryptMessage then async-sends the encrypted
pieces. No kTLS equivalent exists for schannel; every direction goes
through these software paths.
*/

#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)

namespace fast_io::tls::details
{

/*
one DecryptMessage step. Returns the plaintext size delivered to the
caller (>= 0), -1 when more ciphertext is needed, or -2 on close_notify.
On -1/-2 any EXTRA bytes were slid back into ct_pending_ so the next
round continues from them.
*/
template <typename client_type>
inline ::std::ptrdiff_t
schannel_tls_decrypt_step(client_type *client, ::std::byte *buf,
						  ::std::size_t buf_size) FAST_IO_HERBCEPTIONS_THROWS
{
	if (client->pt_pending_size_ != 0)
	{
		::std::size_t const n{client->pt_pending_size_ < buf_size
								  ? client->pt_pending_size_
								  : buf_size};
		__builtin_memcpy(buf, client->pt_pending_, n);
		__builtin_memmove(client->pt_pending_, client->pt_pending_ + n,
						  client->pt_pending_size_ - n);
		client->pt_pending_size_ -= n;
		return static_cast<::std::ptrdiff_t>(n);
	}
	schannel_buffer ib[4]{
		{static_cast<::std::uint_least32_t>(client->ct_pending_size_),
		 secbuffer_data, client->ct_pending_},
		{0, secbuffer_empty, nullptr},
		{0, secbuffer_empty, nullptr},
		{0, secbuffer_empty, nullptr}};
	schannel_buffer_desc id{0, 4, ib};
	auto const status{DecryptMessage(__builtin_addressof(client->ctx_),
									 __builtin_addressof(id), 0, nullptr)};

	::std::byte const *data{};
	::std::size_t data_size{};
	::std::byte const *extra{};
	::std::size_t extra_size{};
	for (::std::uint_least32_t i{}; i != id.count; ++i)
	{
		if (id.buffers[i].type == secbuffer_data)
		{
			data = static_cast<::std::byte const *>(id.buffers[i].data);
			data_size = id.buffers[i].size;
		}
		else if (id.buffers[i].type == secbuffer_extra)
		{
			extra = static_cast<::std::byte const *>(id.buffers[i].data);
			extra_size = id.buffers[i].size;
		}
	}
	if (status == sec_e_ok || status == sec_i_renegotiate ||
		status == sec_i_context_expired)
	{
		if (data != nullptr)
		{
			::std::size_t const n{data_size < buf_size ? data_size : buf_size};
			__builtin_memcpy(buf, data, n);
			if (n < data_size)
			{
				__builtin_memcpy(client->pt_pending_, data + n, data_size - n);
				client->pt_pending_size_ = data_size - n;
			}
		}
		client->ct_pending_size_ = 0;
		if (extra != nullptr)
		{
			__builtin_memmove(client->ct_pending_, extra, extra_size);
			client->ct_pending_size_ = extra_size;
		}
		if (data != nullptr)
		{
			return static_cast<::std::ptrdiff_t>(
				data_size < buf_size ? data_size : buf_size);
		}
		if (status == sec_i_context_expired)
		{
			return -2;
		}
		return -1;
	}
	if (status == sec_e_incomplete_message)
	{
		if (extra != nullptr)
		{
			__builtin_memmove(client->ct_pending_, extra, extra_size);
			client->ct_pending_size_ = extra_size;
		}
		return -1;
	}
	schannel_throw(status);
	return -1;
}

template <typename sched_t, typename client_type, typename alloc_type>
struct schannel_tls_recv_state_base
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{allocator_type::has_status};

	sched_t sched;
	::fast_io::posix_statx_timestamp_opt timeout;
	client_type *client;
	::std::byte *buf{};
	::std::size_t buf_size{};
	void (*finish)(schannel_tls_recv_state_base *st, ::std::cxx_std_error err,
				   ::std::size_t delivered) noexcept {};
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status,
												   typename allocator_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

template <typename sched_t, typename client_type, typename alloc_type, typename func>
struct schannel_tls_recv_state;

template <typename sched_t, typename client_type, typename alloc_type, typename func>
inline void schannel_tls_recv_finish_cb(
	schannel_tls_recv_state_base<sched_t, client_type, alloc_type> *st,
	::std::cxx_std_error err, ::std::size_t delivered) noexcept
{
	auto *self{static_cast<schannel_tls_recv_state<sched_t, client_type, alloc_type, func> *>(st)};
	auto cb{::std::move(self->callback)};
	::fast_io::details::async_delete_state(self);
	cb(err, delivered);
}

template <typename sched_t, typename client_type, typename alloc_type, typename func>
struct schannel_tls_recv_state
	: schannel_tls_recv_state_base<sched_t, client_type, alloc_type>
{
	using base_type = schannel_tls_recv_state_base<sched_t, client_type, alloc_type>;
	func callback;

	inline schannel_tls_recv_state(sched_t s, ::fast_io::posix_statx_timestamp_opt to,
								   client_type *c, ::std::byte *b, ::std::size_t n,
								   func cb) noexcept
		: base_type{s, to, c, b, n}, callback{::std::move(cb)}
	{
		this->finish = &schannel_tls_recv_finish_cb<sched_t, client_type, alloc_type, func>;
	}
};

template <typename sched_t, typename client_type, typename alloc_type>
inline void schannel_tls_recv_finish(
	schannel_tls_recv_state_base<sched_t, client_type, alloc_type> *st, ::std::cxx_std_error err,
	::std::size_t delivered) noexcept
{
	st->finish(st, err, delivered);
}

template <typename sched_t, typename client_type, typename alloc_type>
inline void schannel_tls_recv_step(void *vst, ::std::cxx_std_error err, ::std::size_t n) noexcept
{
	auto *st{static_cast<schannel_tls_recv_state_base<sched_t, client_type, alloc_type> *>(vst)};
	if (err.domain != nullptr)
	{
		schannel_tls_recv_finish(st, err, 0);
		return;
	}
	if (n == 0)
	{
		schannel_tls_recv_finish(st, ::std::cxx_std_error{}, 0);
		return;
	}
	st->client->ct_pending_size_ += n;
	schannel_tls_recv_pump(st);
}

template <typename sched_t, typename client_type, typename alloc_type>
inline void schannel_tls_recv_pump(
	schannel_tls_recv_state_base<sched_t, client_type, alloc_type> *st) noexcept
{
	::std::ptrdiff_t r{};
	FAST_IO_HERBCEPTIONS_TRY
	{
		r = schannel_tls_decrypt_step(st->client, st->buf, st->buf_size);
	}
	catch throws(::std::error e)
	{
		schannel_tls_recv_finish(st, e.release(), 0);
		return;
	}
	if (r > 0)
	{
		schannel_tls_recv_finish(st, ::std::cxx_std_error{},
								 static_cast<::std::size_t>(r));
		return;
	}
	if (r == -2)
	{
		schannel_tls_recv_finish(st, ::std::cxx_std_error{}, 0);
		return;
	}
	if (st->client->ct_pending_size_ == sizeof(st->client->ct_pending_))
	{
		schannel_tls_recv_finish(st,
								 ::fast_io::details::async_make_error(::std::errc::protocol_error),
								 0);
		return;
	}
	async_pread_some_bytes_underflow_callback_define(
		st->sched, st->timeout, st->client->sock_,
		st->client->ct_pending_ + st->client->ct_pending_size_,
		sizeof(st->client->ct_pending_) - st->client->ct_pending_size_,
		::fast_io::intfpos_opt{},
		::fast_io::details::async_bytes_step_cb{
			st, &schannel_tls_recv_step<sched_t, client_type, alloc_type>});
}

template <typename sched_t, typename client_type, typename alloc_type>
struct schannel_tls_send_state_base
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{allocator_type::has_status};

	sched_t sched;
	::fast_io::posix_statx_timestamp_opt timeout;
	client_type *client;
	::std::byte wire[16384 + 256];
	::std::byte const *piece[3]{};
	::std::uint_least32_t piece_size[3]{};
	::std::uint_least8_t piece_i{};
	::std::size_t consumed{};
	void (*finish)(schannel_tls_send_state_base *st, ::std::cxx_std_error err,
				   ::std::size_t consumed) noexcept {};
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status,
												   typename allocator_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

template <typename sched_t, typename client_type, typename alloc_type, typename func>
struct schannel_tls_send_state;

template <typename sched_t, typename client_type, typename alloc_type, typename func>
inline void schannel_tls_send_finish_cb(
	schannel_tls_send_state_base<sched_t, client_type, alloc_type> *st,
	::std::cxx_std_error err, ::std::size_t consumed) noexcept
{
	auto *self{static_cast<schannel_tls_send_state<sched_t, client_type, alloc_type, func> *>(st)};
	auto cb{::std::move(self->callback)};
	::fast_io::details::async_delete_state(self);
	cb(err, consumed);
}

template <typename sched_t, typename client_type, typename alloc_type, typename func>
struct schannel_tls_send_state
	: schannel_tls_send_state_base<sched_t, client_type, alloc_type>
{
	using base_type = schannel_tls_send_state_base<sched_t, client_type, alloc_type>;
	func callback;

	inline schannel_tls_send_state(sched_t s, ::fast_io::posix_statx_timestamp_opt to,
								   client_type *c, ::std::size_t, func cb) noexcept
		: base_type{s, to, c, {}}, callback{::std::move(cb)}
	{
		this->finish = &schannel_tls_send_finish_cb<sched_t, client_type, alloc_type, func>;
	}
};

template <typename sched_t, typename client_type, typename alloc_type>
inline void schannel_tls_send_finish(
	schannel_tls_send_state_base<sched_t, client_type, alloc_type> *st, ::std::cxx_std_error err,
	::std::size_t consumed) noexcept
{
	st->finish(st, err, consumed);
}

template <typename sched_t, typename client_type, typename alloc_type>
inline void schannel_tls_send_step(void *vst, ::std::cxx_std_error err, ::std::size_t) noexcept
{
	auto *st{static_cast<schannel_tls_send_state_base<sched_t, client_type, alloc_type> *>(vst)};
	if (err.domain != nullptr)
	{
		schannel_tls_send_finish(st, err, 0);
		return;
	}
	++st->piece_i;
	schannel_tls_send_piece(st);
}

template <typename sched_t, typename client_type, typename alloc_type>
inline void schannel_tls_send_piece(
	schannel_tls_send_state_base<sched_t, client_type, alloc_type> *st) noexcept
{
	if (st->piece_i == 3)
	{
		schannel_tls_send_finish(st, ::std::cxx_std_error{}, st->consumed);
		return;
	}
	auto const i{st->piece_i};
	async_pwrite_some_bytes_overflow_callback_define(
		st->sched, st->timeout, st->client->sock_,
		st->piece[i], st->piece_size[i], ::fast_io::intfpos_opt{},
		::fast_io::details::async_bytes_step_cb{
			st, &schannel_tls_send_step<sched_t, client_type, alloc_type>});
}

template <typename sched_t, typename client_type, typename alloc_type>
inline void schannel_tls_send_submit(
	schannel_tls_send_state_base<sched_t, client_type, alloc_type> *st, ::std::byte const *first,
	::std::size_t count) noexcept
{
	auto *client{st->client};
	::std::size_t const limit{client->stream_sizes_.maximum_message};
	::std::size_t const n{count < limit ? count : limit};
	::std::size_t const total{client->stream_sizes_.header + n + client->stream_sizes_.trailer};
	if (total > sizeof(st->wire))
	{
		schannel_tls_send_finish(st,
								 ::fast_io::details::async_make_error(::std::errc::message_size),
								 0);
		return;
	}
	::std::byte *const data{st->wire + client->stream_sizes_.header};
	__builtin_memcpy(data, first, n);
	schannel_buffer ob[4]{
		{client->stream_sizes_.header, secbuffer_stream_header, st->wire},
		{static_cast<::std::uint_least32_t>(n), secbuffer_data, data},
		{client->stream_sizes_.trailer, secbuffer_stream_trailer, data + n},
		{0, secbuffer_empty, nullptr}};
	schannel_buffer_desc od{0, 4, ob};
	auto const status{EncryptMessage(__builtin_addressof(client->ctx_), 0,
									 __builtin_addressof(od), 0)};
	if (status < 0)
	{
		schannel_tls_send_finish(
			st, ::fast_io::details::async_make_error(static_cast<::fast_io::freestanding::win32_errc>(static_cast<::std::uint_least32_t>(status))),
			0);
		return;
	}
	st->consumed = n;
	st->piece[0] = static_cast<::std::byte const *>(ob[0].data);
	st->piece_size[0] = ob[0].size;
	st->piece[1] = static_cast<::std::byte const *>(ob[1].data);
	st->piece_size[1] = ob[1].size;
	st->piece[2] = static_cast<::std::byte const *>(ob[2].data);
	st->piece_size[2] = ob[2].size;
	schannel_tls_send_piece(st);
}

} // namespace fast_io::tls::details

namespace fast_io::tls
{

template <typename async_scheduler_type, ::std::integral ch_type,
		  typename socket_observer_type, typename func>
	requires(::fast_io::tls::details::tls_generic_sched<
			 ::std::remove_cvref_t<async_scheduler_type>>)

inline void async_pread_some_bytes_underflow_callback_define(
	async_scheduler_type &&sched, ::fast_io::posix_statx_timestamp_opt timeout,
	basic_schannel_tls_io_observer<ch_type, socket_observer_type> tob,
	::std::byte *first, ::std::size_t count, ::fast_io::intfpos_opt off,
	func callback) noexcept
{
	::std::cxx_std_error err{};
	if (off.has_opt)
	{
		err = ::fast_io::details::async_make_error(::std::errc::io_error);
	}
	else
	{
		auto *client{tob.handle};
		if (!client->established_)
		{
			async_pread_some_bytes_underflow_callback_define(
				sched, timeout, client->sock_, first, count, off,
				::std::move(callback));
			return;
		}
		using alloc_type = ::fast_io::details::async_scheduler_allocator_t<
			::std::remove_cvref_t<async_scheduler_type>>;
		using state_type =
			::fast_io::tls::details::schannel_tls_recv_state<
				::std::remove_cvref_t<async_scheduler_type>,
				::std::remove_cvref_t<decltype(*client)>, alloc_type,
				::std::remove_cvref_t<func>>;
		FAST_IO_HERBCEPTIONS_TRY
		{
			auto *st{::fast_io::details::async_new_state<state_type>(
				sched, timeout, client, first, count, ::std::move(callback))};
			::fast_io::tls::details::schannel_tls_recv_pump(st);
			return;
		}
		catch throws(::std::error e)
		{
			err = e.release();
		}
	}
	callback(err, ::std::size_t{});
}

template <typename async_scheduler_type, ::std::integral ch_type,
		  typename socket_observer_type, typename func>
	requires(::fast_io::tls::details::tls_generic_sched<
			 ::std::remove_cvref_t<async_scheduler_type>>)

inline void async_pwrite_some_bytes_overflow_callback_define(
	async_scheduler_type &&sched, ::fast_io::posix_statx_timestamp_opt timeout,
	basic_schannel_tls_io_observer<ch_type, socket_observer_type> tob,
	::std::byte const *first, ::std::size_t count, ::fast_io::intfpos_opt off,
	func callback) noexcept
{
	::std::cxx_std_error err{};
	if (off.has_opt)
	{
		err = ::fast_io::details::async_make_error(::std::errc::io_error);
	}
	else
	{
		auto *client{tob.handle};
		if (!client->established_)
		{
			async_pwrite_some_bytes_overflow_callback_define(
				sched, timeout, client->sock_, first, count, off,
				::std::move(callback));
			return;
		}
		using alloc_type = ::fast_io::details::async_scheduler_allocator_t<
			::std::remove_cvref_t<async_scheduler_type>>;
		using state_type =
			::fast_io::tls::details::schannel_tls_send_state<
				::std::remove_cvref_t<async_scheduler_type>,
				::std::remove_cvref_t<decltype(*client)>, alloc_type,
				::std::remove_cvref_t<func>>;
		FAST_IO_HERBCEPTIONS_TRY
		{
			auto *st{::fast_io::details::async_new_state<state_type>(
				sched, timeout, client, ::std::size_t{}, ::std::move(callback))};
			::fast_io::tls::details::schannel_tls_send_submit(st, first, count);
			return;
		}
		catch throws(::std::error e)
		{
			err = e.release();
		}
	}
	callback(err, ::std::size_t{});
}

} // namespace fast_io::tls

#endif
