#pragma once
/*
 * IOCP backend for fast_io's generic async layer, on the raw win32 API
 * surface declared in win32/api/apis.h.
 *
 * The scheduler is a kernel completion port: the owning object is
 * io_async_scheduler (win32_file) opened with fi::io_async — its ctor
 * runs CreateIoCompletionPort(INVALID_HANDLE_VALUE, ...); the
 * trivially-copyable io_async_observer (win32_io_observer) is what every
 * async operation takes.
 *
 * Streams must be opened with open_mode::no_block — it maps to
 * FILE_FLAG_OVERLAPPED for files/pipes and WSA_FLAG_OVERLAPPED for
 * sockets. A stream is associated with the port lazily at submission
 * (CreateIoCompletionPort on an already-associated handle reports
 * ERROR_INVALID_PARAMETER and is left alone).
 *
 * Backend contract (operations/refs/async.h):
 * - async_pread_some_bytes_underflow_callback_define /
 *   async_pwrite_some_bytes_overflow_callback_define: required.
 *   Everything else (all forms, scatter, transmit, buffered) is built on
 *   them generically.
 * All defines are noexcept; every error — including submission
 * failures — is delivered through the callback as ::std::cxx_std_error.
 *
 * Timeouts are per-operation: posix_statx_timestamp_opt is a relative
 * deadline. There is no linked-timeout primitive here, so each timed op
 * arms a threadpool timer; on expiry the callback flags the cookie and
 * runs CancelIoEx, and the aborted completion reports errc::timed_out.
 * The timer callback never owns or frees state — the cookie lives until
 * its op completion arrives, and the completion path joins outstanding
 * timer callbacks before freeing.
 */

#if !defined(_WIN32_WINDOWS)

namespace fast_io
{

/*
 * Owning-scheduler reduction: a win32 file holding a completion port
 * decays to its observer for the decay layer.
 */
template <win32_family family, ::std::integral char_type>
inline constexpr basic_win32_family_io_observer<family, char_type>
async_scheduler_ref_define(basic_win32_family_file<family, char_type> &file) noexcept
{
	return {file.native_handle()};
}

/* every win32 stream family (nt, win32, socket) schedules on a port
 * observer regardless of its own char type */
template <nt_family family, ::std::integral char_type>
inline constexpr ::fast_io::io_type_t<win32_io_observer>
async_scheduler_type(basic_nt_family_io_observer<family, char_type>) noexcept
{
	return {};
}

template <win32_family family, ::std::integral char_type>
inline constexpr ::fast_io::io_type_t<win32_io_observer>
async_scheduler_type(basic_win32_family_io_observer<family, char_type>) noexcept
{
	return {};
}

template <win32_family family, ::std::integral char_type>
inline constexpr ::fast_io::io_type_t<win32_io_observer>
async_scheduler_type(basic_win32_family_socket_io_observer<family, char_type>) noexcept
{
	return {};
}

namespace details
{

/*
 * Internal completion ABI: GetQueuedCompletionStatus hands back the
 * OVERLAPPED* the op was submitted with. Every IOCP state object embeds
 * win32_iocp_state_base FIRST so the completion pointer IS the state
 * pointer; invoke() is the per-op finisher. The timer thunk also reaches
 * file_handle/timed_out through this base, so it needs no knowledge of
 * the concrete cookie type. Not part of the public API.
 */
using win32_iocp_invoke_func = void (*)(void *, ::std::size_t, ::std::uint_least32_t) noexcept;

struct win32_iocp_state_base
{
	::fast_io::win32::overlapped ovl;
	win32_iocp_invoke_func invoke;
	void *file_handle{}; /* stream handle, for CancelIoEx */
	void *timer{};       /* PTP_TIMER when a deadline is armed */
	bool timed_out{};
};

inline void win32_iocp_dispatch(::fast_io::win32::overlapped *over, ::std::size_t transferred,
								::std::uint_least32_t err) noexcept
{
	auto *base{reinterpret_cast<win32_iocp_state_base *>(over)};
	base->invoke(base, transferred, err);
}

/*
 * Threadpool-timer thunk: runs on a pool worker. Only touches the state
 * base — flags the cookie and cancels the pending op; the aborted
 * completion is what frees it.
 */
inline void FAST_IO_WINSTDCALL win32_iocp_timer_thunk(void *, void *context, void *) noexcept
{
	auto *state{static_cast<win32_iocp_state_base *>(context)};
	state->timed_out = true;
	::fast_io::win32::CancelIoEx(state->file_handle, __builtin_addressof(state->ovl));
}

/*
 * Cookie for one pending read or write; both ops share the layout —
 * win32_iocp_state_base stays the first member because
 * win32_iocp_dispatch dereferences the OVERLAPPED* as it. The wsabuf
 * slot feeds socket submissions (WSARecv/WSASend take a wsabuf, not a
 * raw byte pointer); file submissions leave it unused.
 */
template <typename alloc_type, typename T>
struct win32_iocp_rw_cookie
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	win32_iocp_state_base base;
	::fast_io::win32::wsabuf wsa{};
	T callback;
	/* set when the op was submitted at an explicit offset emulating
	 * FILE_USE_FILE_POINTER_POSITION — the file position is advanced by
	 * the transferred count on delivery */
	bool advance_position{};
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

/*
 * Completion finisher for a read or write cookie: joins the deadline
 * timer, frees the cookie, invokes the user callback exactly once.
 */
template <typename alloc_type, typename T, bool is_read>
inline void win32_iocp_rw_deliver(void *self, ::std::size_t transferred,
								  ::std::uint_least32_t err) noexcept
{
	using cookie_type = win32_iocp_rw_cookie<alloc_type, T>;
	auto *cookie{static_cast<cookie_type *>(self)};
	if (auto *timer{cookie->base.timer}; timer != nullptr)
	{
		::fast_io::win32::SetThreadpoolTimer(timer, nullptr, 0, 0);
		::fast_io::win32::WaitForThreadpoolTimerCallbacks(timer, 1);
		::fast_io::win32::CloseThreadpoolTimer(timer);
	}
	bool const timed_out{cookie->base.timed_out};
	if (cookie->advance_position)
	{
		/* FILE_USE_FILE_POINTER_POSITION emulation: move the file object's
		 * position past the bytes actually transferred */
		auto const &ovloff{cookie->base.ovl.dummy_union_name.dummy_struct_name};
		::fast_io::win32::SetFilePointerEx(
			cookie->base.file_handle,
			static_cast<::std::int_least64_t>(
				(static_cast<::std::uint_least64_t>(ovloff.OffsetHigh) << 32u) | ovloff.Offset) +
				static_cast<::std::int_least64_t>(transferred),
			nullptr, 0u /* FILE_BEGIN */);
	}
	auto callback{::std::move(cookie->callback)};
	::fast_io::details::async_delete_state(cookie);
	::std::cxx_std_error e{};
	if (timed_out && err != 0) [[unlikely]]
	{
		/* the timer ran CancelIoEx and the completion is the aborted op;
		 * a completion that beat the deadline arrives with err == 0 and
		 * still reports success */
		e = ::fast_io::details::async_make_error(::std::errc::timed_out);
	}
	else if constexpr (is_read)
	{
		/* ERROR_HANDLE_EOF (38) / ERROR_BROKEN_PIPE (109) on a read are
		 * stream EOF, not errors: a zero-byte completion matches posix
		 * read() semantics */
		if (err == 38u || err == 109u)
		{
			transferred = 0;
			err = 0;
		}
	}
	if (err != 0) [[unlikely]]
	{
		e = ::fast_io::details::async_make_error(
			static_cast<::fast_io::freestanding::win32_errc>(err));
	}
	callback(e, transferred);
}

/*
 * Associate the stream handle with the port. Re-association of an
 * already-associated handle fails with ERROR_INVALID_PARAMETER (87) —
 * benign, the handle rides whichever port it already has.
 */
inline void win32_iocp_associate(void *port, void *handle) throws
{
	/* IOCP only delivers packets for handles opened with overlapped
	 * semantics; on a synchronous-mode handle the op completes inline and
	 * nothing is ever queued — the coroutine would hang instead of
	 * failing. FileModeInformation reports the create flags: synchronous
	 * handles carry FILE_SYNCHRONOUS_IO_{ALERT,NONALERT}. Query failure
	 * (e.g. non-file object types) is left alone — the op itself decides
	 * whether it can go async. */
	::fast_io::win32::nt::io_status_block isb{};
	::std::uint_least32_t fmode{};
	if (::fast_io::win32::nt::NtQueryInformationFile(
			handle, __builtin_addressof(isb), __builtin_addressof(fmode),
			static_cast<::std::uint_least32_t>(sizeof(fmode)),
			::fast_io::win32::nt::file_information_class::FileModeInformation) == 0 &&
		(fmode & 0x30u) != 0) [[unlikely]]
	{
		throw_win32_error(50u /* ERROR_NOT_SUPPORTED */);
	}
	if (::fast_io::win32::CreateIoCompletionPort(handle, port, 0, 0) == nullptr) [[unlikely]]
	{
		auto err{::fast_io::win32::GetLastError()};
		if (err != 87u) // ERROR_INVALID_PARAMETER
		{
			throw_win32_error(err);
		}
	}
}

/* posix_statx_timestamp64 -> relative FILETIME due time (negative
 * 100ns units), rounded up and clamped away from the disarm value 0 */
inline ::fast_io::win32::filetime
win32_iocp_relative_deadline(::fast_io::posix_statx_timestamp64 timeout) noexcept
{
	::std::int_least64_t due{-(
		static_cast<::std::int_least64_t>(timeout.tv_sec) * 10000000 +
		static_cast<::std::int_least64_t>((timeout.tv_nsec + 99u) / 100u))};
	if (due == 0)
	{
		due = -1;
	}
	auto u{static_cast<::std::uint_least64_t>(due)};
	return {static_cast<::std::uint_least32_t>(u), static_cast<::std::uint_least32_t>(u >> 32)};
}

/*
 * One read/write submission against either a file-family handle
 * (ReadFile/WriteFile) or a socket (WSARecv/WSASend); is_socket is the
 * only branch difference. Every failure — including submission
 * failures — is delivered through the callback exactly once.
 */
template <bool is_write, typename sched_type, typename stream_type, typename T>
inline void win32_iocp_rw_submit(sched_type sched, stream_type stream, void *first,
								 ::std::size_t count, ::fast_io::intfpos_opt off,
								 ::fast_io::posix_statx_timestamp_opt timeout, T &&callback) noexcept
{
	using callback_type = ::std::remove_cvref_t<T>;
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<sched_type>;
	using cookie_type = win32_iocp_rw_cookie<alloc_type, callback_type>;
	constexpr bool is_socket{
		::std::same_as<typename ::std::remove_cvref_t<stream_type>::native_handle_type,
					   ::std::size_t>};

	::std::cxx_std_error early_err{};
	void *file_handle{};
	if constexpr (is_socket)
	{
		if (off.has_opt) [[unlikely]]
		{
			early_err = ::fast_io::details::async_make_error(::std::errc::invalid_seek);
		}
		file_handle = reinterpret_cast<void *>(stream.native_handle());
	}
	else
	{
		file_handle = stream.native_handle();
	}
	if (early_err.domain == nullptr)
	{
		try
		{
			win32_iocp_associate(sched.native_handle(), file_handle);
		}
		catch throws(::std::error e)
		{
			early_err = e.release();
		}
	}
	if (early_err.domain != nullptr)
	{
		auto e{early_err};
		early_err = {};
		callback(e, 0zu);
		return;
	}

	cookie_type *cookie;
	try
	{
		cookie = ::fast_io::details::async_new_state_plain<cookie_type>(
			sched,
			win32_iocp_state_base{{},
								  &win32_iocp_rw_deliver<alloc_type, callback_type, !is_write>,
								  file_handle},
			::fast_io::win32::wsabuf{}, callback_type{::std::forward<T>(callback)});
	}
	catch throws(::std::error e)
	{
		callback(e.release(), 0zu);
		return;
	}

	if (off.has_opt)
	{
		try
		{
			::fast_io::win32::details::win32_calculate_offset_impl(file_handle, cookie->base.ovl,
																   off.opt);
		}
		catch throws(::std::error e)
		{
			auto cb{::std::move(cookie->callback)};
			::fast_io::details::async_delete_state(cookie);
			cb(e.release(), 0zu);
			return;
		}
	}
	else if constexpr (!is_socket)
	{
		/* empty offset = current file position: FILE_USE_FILE_POINTER_POSITION.
		 * Wine does not implement the ~0/~0 sentinel in overlapped I/O
		 * (ERROR_INVALID_PARAMETER), so emulate it: take the file object's
		 * current position as an explicit offset and advance it on
		 * delivery. Non-seekable handles (pipes) can't report a position —
		 * their offset fields are ignored by the kernel, so 0 is fine. */
		::std::int_least64_t cur{};
		if (::fast_io::win32::SetFilePointerEx(file_handle, 0,
											   __builtin_addressof(cur),
											   1u /* FILE_CURRENT */))
		{
			cookie->base.ovl.dummy_union_name.dummy_struct_name = {
				static_cast<::std::uint_least32_t>(cur),
				static_cast<::std::uint_least32_t>(static_cast<::std::uint_least64_t>(cur) >> 32u)};
			cookie->advance_position = true;
		}
		else
		{
			cookie->base.ovl.dummy_union_name.dummy_struct_name = {0u, 0u};
		}
	}
	::std::uint_least32_t const len{
		::fast_io::details::read_write_bytes_compute<::std::uint_least32_t>(
			static_cast<::std::byte *>(first), count)};
	if constexpr (is_socket)
	{
		cookie->wsa = {len, static_cast<char *>(first)};
	}

	/* the deadline timer is created before the op goes out so a
	 * submission failure tears down cleanly; it is armed only once the
	 * kernel owns the op. A zero-length timeout fails immediately. */
	if (timeout.has_opt)
	{
		if (timeout.opt.tv_sec == 0 && timeout.opt.tv_nsec == 0) [[unlikely]]
		{
			auto cb{::std::move(cookie->callback)};
			::fast_io::details::async_delete_state(cookie);
			cb(::fast_io::details::async_make_error(::std::errc::timed_out), 0zu);
			return;
		}
		cookie->base.timer = ::fast_io::win32::CreateThreadpoolTimer(
			win32_iocp_timer_thunk, cookie, nullptr);
		if (cookie->base.timer == nullptr) [[unlikely]]
		{
			auto err{::fast_io::win32::GetLastError()};
			auto cb{::std::move(cookie->callback)};
			::fast_io::details::async_delete_state(cookie);
			cb(::fast_io::details::async_make_error(
				   static_cast<::fast_io::freestanding::win32_errc>(err)),
			   0zu);
			return;
		}
	}

	::std::uint_least32_t err{};
	bool pending{};
	if constexpr (is_socket)
	{
		auto const sock{stream.native_handle()};
		if constexpr (is_write)
		{
			pending = ::fast_io::win32::WSASend(sock, __builtin_addressof(cookie->wsa), 1u, nullptr,
												0u, __builtin_addressof(cookie->base.ovl),
												nullptr) == 0;
			if (!pending)
			{
				err = static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError());
				pending = err == 997u; // WSA_IO_PENDING
			}
		}
		else
		{
			::std::uint_least32_t flags{};
			pending = ::fast_io::win32::WSARecv(sock, __builtin_addressof(cookie->wsa), 1u, nullptr,
												__builtin_addressof(flags),
												__builtin_addressof(cookie->base.ovl),
												nullptr) == 0;
			if (!pending)
			{
				err = static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError());
				pending = err == 997u;
			}
		}
	}
	else if constexpr (is_write)
	{
		pending = ::fast_io::win32::WriteFile(file_handle, first, len, nullptr,
											  __builtin_addressof(cookie->base.ovl)) != 0;
		if (!pending)
		{
			err = ::fast_io::win32::GetLastError();
			pending = err == 997u; // ERROR_IO_PENDING
		}
	}
	else
	{
		pending = ::fast_io::win32::ReadFile(file_handle, first, len, nullptr,
											 __builtin_addressof(cookie->base.ovl)) != 0;
		if (!pending)
		{
			err = ::fast_io::win32::GetLastError();
			pending = err == 997u;
		}
	}

	if (pending)
	{
		/* synchronous success and ERROR_IO_PENDING both end here: the
		 * kernel queues the completion packet either way */
		if (auto *timer{cookie->base.timer}; timer != nullptr)
		{
			auto due{win32_iocp_relative_deadline(timeout.opt)};
			::fast_io::win32::SetThreadpoolTimer(timer, __builtin_addressof(due), 0, 0);
		}
		return;
	}

	if (auto *timer{cookie->base.timer}; timer != nullptr)
	{
		::fast_io::win32::CloseThreadpoolTimer(timer);
	}
	auto cb{::std::move(cookie->callback)};
	::fast_io::details::async_delete_state(cookie);
	cb(::fast_io::details::async_make_error(
		   static_cast<::fast_io::freestanding::win32_errc>(err)),
	   0zu);
}

/*
 * Cookie for one pending AcceptEx: the accept socket is created at
 * submission (AcceptEx takes a pre-created socket), closed on any
 * failure, and SO_UPDATE_ACCEPT_CONTEXT'd on success so it can receive
 * and send. file_handle in the base stays the LISTEN socket — CancelIoEx
 * targets the op's owner and SO_UPDATE_ACCEPT_CONTEXT needs it.
 */
template <typename alloc_type, typename T>
struct win32_iocp_accept_cookie
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	win32_iocp_state_base base;
	::std::size_t accept_sock{};
	::std::uint_least32_t received{};
	T callback;
	/* AcceptEx writes the local and remote addresses into the output
	 * buffer: each slot is sockaddr+16 per MSDN. Wine rejects a null
	 * lpOutputBuffer outright (WSAEINVAL) even when no addresses are
	 * wanted, so the buffer must always be real. */
	::std::byte addrbuf[2 * (sizeof(::fast_io::posix_sockaddr_in6) + 16)]{};
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

/*
 * Resolves AcceptEx through the socket extension-function ioctl; the
 * entry point lives in mswsock.dll so there is no import to pragma-map.
 * WSAID_ACCEPTEX = {b5367df1-cbac-11cf-95ca-00805f48a192} in the
 * wire-layout byte order GUIDs marshal in.
 */
inline ::fast_io::win32::acceptex_func win32_iocp_acceptex(::std::size_t listen_sock) throws
{
	static constexpr ::std::byte const guid[16]{
		::std::byte{0xf1}, ::std::byte{0x7d}, ::std::byte{0x36}, ::std::byte{0xb5},
		::std::byte{0xac}, ::std::byte{0xcb}, ::std::byte{0xcf}, ::std::byte{0x11},
		::std::byte{0x95}, ::std::byte{0xca}, ::std::byte{0x00}, ::std::byte{0x80},
		::std::byte{0x5f}, ::std::byte{0x48}, ::std::byte{0xa1}, ::std::byte{0x92}};
	::fast_io::win32::acceptex_func fp{};
	::std::uint_least32_t nbytes{};
	/* SIO_GET_EXTENSION_FUNCTION_POINTER */
	if (::fast_io::win32::WSAIoctl(listen_sock, 0xc8000006u,
								   const_cast<::std::byte *>(guid),
								   static_cast<::std::uint_least32_t>(sizeof(guid)),
								   __builtin_addressof(fp),
								   static_cast<::std::uint_least32_t>(sizeof(fp)),
								   __builtin_addressof(nbytes), nullptr, nullptr) != 0 ||
		fp == nullptr) [[unlikely]]
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	}
	return fp;
}

/*
 * Accepted-socket teardown + success fixup. On success the socket still
 * lacks a transport context until SO_UPDATE_ACCEPT_CONTEXT stamps the
 * listener's — the kernel rejects recv/send on it otherwise.
 */
template <typename alloc_type, typename T>
inline void win32_iocp_accept_deliver(void *self, ::std::size_t,
									  ::std::uint_least32_t err) noexcept
{
	using cookie_type = win32_iocp_accept_cookie<alloc_type, T>;
	auto *cookie{static_cast<cookie_type *>(self)};
	if (auto *timer{cookie->base.timer}; timer != nullptr)
	{
		::fast_io::win32::SetThreadpoolTimer(timer, nullptr, 0, 0);
		::fast_io::win32::WaitForThreadpoolTimerCallbacks(timer, 1);
		::fast_io::win32::CloseThreadpoolTimer(timer);
	}
	bool const timed_out{cookie->base.timed_out};
	::std::size_t const accept_sock{cookie->accept_sock};
	::std::size_t const listen_sock{
		reinterpret_cast<::std::size_t>(cookie->base.file_handle)};
	auto callback{::std::move(cookie->callback)};
	::fast_io::details::async_delete_state(cookie);

	::std::cxx_std_error e{};
	if (timed_out && err != 0) [[unlikely]]
	{
		/* the timer ran CancelIoEx and this completion is the aborted op;
		 * a completion that beat the deadline arrives with err == 0 and
		 * still reports success */
		e = ::fast_io::details::async_make_error(::std::errc::timed_out);
	}
	else if (err != 0) [[unlikely]]
	{
		e = ::fast_io::details::async_make_error(
			static_cast<::fast_io::freestanding::win32_errc>(err));
	}
	else if (::fast_io::win32::setsockopt(accept_sock, 0xffffu /* SOL_SOCKET */,
										  0x700bu /* SO_UPDATE_ACCEPT_CONTEXT */,
										  __builtin_addressof(listen_sock),
										  static_cast<int>(sizeof(listen_sock))) != 0) [[unlikely]]
	{
		e = ::fast_io::details::async_make_error(
			static_cast<::fast_io::freestanding::win32_errc>(
				static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError())));
	}
	if (e.domain != nullptr) [[unlikely]]
	{
		::fast_io::win32::closesocket(accept_sock);
		callback(e, 0zu);
		return;
	}
	callback(e, accept_sock);
}

/*
 * One AcceptEx submission: resolve the extension pointer, mint the
 * accept socket with the listener's family (getsockname), associate the
 * listener with the port, submit. Every failure goes through the
 * callback exactly once.
 */
template <::fast_io::win32_family family, typename sched_type, typename stream_type, typename T>
inline void win32_iocp_accept_submit(sched_type sched, stream_type stream,
									 ::fast_io::open_mode m,
									 ::fast_io::posix_statx_timestamp_opt timeout,
									 T &&callback) noexcept
{
	using callback_type = ::std::remove_cvref_t<T>;
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<sched_type>;
	using cookie_type = win32_iocp_accept_cookie<alloc_type, callback_type>;

	::std::size_t const listen_sock{stream.native_handle()};
	::fast_io::win32::acceptex_func acceptex{};
	::std::size_t accept_sock{};
	try
	{
		acceptex = win32_iocp_acceptex(listen_sock);
		/* the accept socket's family has to match the listener's;
		 * getsockname reports the bound address's family */
		::fast_io::posix_sockaddr_in6 addrbuf{};
		int addrlen{static_cast<int>(sizeof(addrbuf))};
		if (::fast_io::win32::getsockname(listen_sock, __builtin_addressof(addrbuf),
										  __builtin_addressof(addrlen)) != 0) [[unlikely]]
		{
			throw_win32_error(
				static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
		}
		::std::uint_least32_t dwflags{
			(family == ::fast_io::win32_family::wide_nt
				 ? ::fast_io::to_win32_sock_open_mode(m)
				 : ::fast_io::to_win32_sock_open_mode_9xa(m)) |
			/* WSA_FLAG_OVERLAPPED: async sockets always need it */
			0x01u};
		accept_sock = ::fast_io::win32::details::open_win32_socket_raw_impl<family>(
			addrbuf.sin6_family, ::fast_io::to_win32_sock_type(::fast_io::sock_type::stream),
			::fast_io::to_win32_sock_protocol(::fast_io::sock_protocol::tcp), dwflags);
		if ((m & ::fast_io::open_mode::no_block) == ::fast_io::open_mode::no_block)
		{
			::fast_io::win32::details::win32_socket_apply_no_block(accept_sock);
		}
		win32_iocp_associate(sched.native_handle(), reinterpret_cast<void *>(listen_sock));
	}
	catch throws(::std::error e)
	{
		if (accept_sock != 0)
		{
			::fast_io::win32::closesocket(accept_sock);
		}
		callback(e.release(), 0zu);
		return;
	}

	cookie_type *cookie;
	try
	{
		cookie = ::fast_io::details::async_new_state_plain<cookie_type>(
			sched,
			win32_iocp_state_base{{},
								  &win32_iocp_accept_deliver<alloc_type, callback_type>,
								  reinterpret_cast<void *>(listen_sock)},
			accept_sock, 0u, callback_type{::std::forward<T>(callback)});
	}
	catch throws(::std::error e)
	{
		::fast_io::win32::closesocket(accept_sock);
		callback(e.release(), 0zu);
		return;
	}

	/* the deadline timer is created before the op goes out so a
	 * submission failure tears down cleanly; it is armed only once the
	 * kernel owns the op */
	if (timeout.has_opt)
	{
		if (timeout.opt.tv_sec == 0 && timeout.opt.tv_nsec == 0) [[unlikely]]
		{
			auto cb{::std::move(cookie->callback)};
			::fast_io::details::async_delete_state(cookie);
			::fast_io::win32::closesocket(accept_sock);
			cb(::fast_io::details::async_make_error(::std::errc::timed_out), 0zu);
			return;
		}
		cookie->base.timer = ::fast_io::win32::CreateThreadpoolTimer(
			win32_iocp_timer_thunk, cookie, nullptr);
		if (cookie->base.timer == nullptr) [[unlikely]]
		{
			auto err{::fast_io::win32::GetLastError()};
			auto cb{::std::move(cookie->callback)};
			::fast_io::details::async_delete_state(cookie);
			::fast_io::win32::closesocket(accept_sock);
			cb(::fast_io::details::async_make_error(
				   static_cast<::fast_io::freestanding::win32_errc>(err)),
			   0zu);
			return;
		}
	}

	/* dwReceiveDataLength 0: the accepted socket is returned without
	 * waiting for payload. The address buffer stays real — wine's
	 * AcceptEx rejects a null lpOutputBuffer with WSAEINVAL, and real
	 * Windows wants sockaddr_storage+16 per address slot anyway.
	 * Synchronous success and WSA_IO_PENDING both queue a completion */
	constexpr ::std::uint_least32_t addrslotlen{
		static_cast<::std::uint_least32_t>(sizeof(::fast_io::posix_sockaddr_in6) + 16)};
	if (acceptex(listen_sock, accept_sock, cookie->addrbuf, 0u, addrslotlen, addrslotlen,
				 __builtin_addressof(cookie->received),
				 __builtin_addressof(cookie->base.ovl)) == 0)
	{
		auto const err{
			static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError())};
		if (err != 997u) [[unlikely]] // WSA_IO_PENDING
		{
			if (auto *timer{cookie->base.timer}; timer != nullptr)
			{
				::fast_io::win32::CloseThreadpoolTimer(timer);
			}
			auto cb{::std::move(cookie->callback)};
			::fast_io::details::async_delete_state(cookie);
			::fast_io::win32::closesocket(accept_sock);
			cb(::fast_io::details::async_make_error(
				   static_cast<::fast_io::freestanding::win32_errc>(err)),
			   0zu);
			return;
		}
	}
	if (auto *timer{cookie->base.timer}; timer != nullptr)
	{
		auto due{win32_iocp_relative_deadline(timeout.opt)};
		::fast_io::win32::SetThreadpoolTimer(timer, __builtin_addressof(due), 0, 0);
	}
}

/* ======================= connect ======================= */

/*
 * Cookie for one pending ConnectEx: unlike accept the socket is the
 * caller's — nothing is owned or cleaned up by the op. addr is copied
 * at submission; ConnectEx dereferences it while the op is pending.
 * file_handle in the base is the connecting socket, for CancelIoEx.
 */
template <typename alloc_type, typename T>
struct win32_iocp_connect_cookie
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	win32_iocp_state_base base;
	::std::uint_least32_t received{};
	::fast_io::posix_sockaddr_storage addr{};
	T callback;
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

/*
 * Resolves ConnectEx through the socket extension-function ioctl.
 * WSAID_CONNECTEX = {25a207b9-ddf3-4660-8ee9-76e58c74063e} in the
 * wire-layout byte order GUIDs marshal in.
 */
inline ::fast_io::win32::connectex_func win32_iocp_connectex(::std::size_t sock) throws
{
	static constexpr ::std::byte const guid[16]{
		::std::byte{0xb9}, ::std::byte{0x07}, ::std::byte{0xa2}, ::std::byte{0x25},
		::std::byte{0xf3}, ::std::byte{0xdd}, ::std::byte{0x60}, ::std::byte{0x46},
		::std::byte{0x8e}, ::std::byte{0xe9}, ::std::byte{0x76}, ::std::byte{0xe5},
		::std::byte{0x8c}, ::std::byte{0x74}, ::std::byte{0x06}, ::std::byte{0x3e}};
	::fast_io::win32::connectex_func fp{};
	::std::uint_least32_t nbytes{};
	/* SIO_GET_EXTENSION_FUNCTION_POINTER */
	if (::fast_io::win32::WSAIoctl(sock, 0xc8000006u,
								   const_cast<::std::byte *>(guid),
								   static_cast<::std::uint_least32_t>(sizeof(guid)),
								   __builtin_addressof(fp),
								   static_cast<::std::uint_least32_t>(sizeof(fp)),
								   __builtin_addressof(nbytes), nullptr, nullptr) != 0 ||
		fp == nullptr) [[unlikely]]
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	}
	return fp;
}

/*
 * On success the socket still lacks its connection context —
 * SO_UPDATE_CONNECT_CONTEXT stamps it so getpeername/getsockname and
 * the shutdown options behave like a synchronous connect()'s socket.
 * The socket stays with the caller on every path.
 */
template <typename alloc_type, typename T>
inline void win32_iocp_connect_deliver(void *self, ::std::size_t,
									   ::std::uint_least32_t err) noexcept
{
	using cookie_type = win32_iocp_connect_cookie<alloc_type, T>;
	auto *cookie{static_cast<cookie_type *>(self)};
	if (auto *timer{cookie->base.timer}; timer != nullptr)
	{
		::fast_io::win32::SetThreadpoolTimer(timer, nullptr, 0, 0);
		::fast_io::win32::WaitForThreadpoolTimerCallbacks(timer, 1);
		::fast_io::win32::CloseThreadpoolTimer(timer);
	}
	bool const timed_out{cookie->base.timed_out};
	::std::size_t const sock{
		reinterpret_cast<::std::size_t>(cookie->base.file_handle)};
	auto callback{::std::move(cookie->callback)};
	::fast_io::details::async_delete_state(cookie);

	::std::cxx_std_error e{};
	if (timed_out && err != 0) [[unlikely]]
	{
		e = ::fast_io::details::async_make_error(::std::errc::timed_out);
	}
	else if (err != 0) [[unlikely]]
	{
		e = ::fast_io::details::async_make_error(
			static_cast<::fast_io::freestanding::win32_errc>(err));
	}
	else if (::fast_io::win32::setsockopt(sock, 0xffffu /* SOL_SOCKET */,
										  0x7010u /* SO_UPDATE_CONNECT_CONTEXT */, nullptr,
										  0) != 0) [[unlikely]]
	{
		e = ::fast_io::details::async_make_error(
			static_cast<::fast_io::freestanding::win32_errc>(
				static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError())));
	}
	callback(e);
}

/*
 * One ConnectEx submission: resolve the extension pointer, associate the
 * socket with the port, bind it to the peer family's wildcard (ConnectEx
 * requires a bound socket), submit. The socket is never touched on
 * failure paths — the op does not own it.
 */
template <::fast_io::win32_family family, typename sched_type, typename stream_type, typename T>
inline void win32_iocp_connect_submit(sched_type sched, stream_type stream, void const *addr,
									  ::std::size_t addrlen,
									  ::fast_io::posix_statx_timestamp_opt timeout,
									  T &&callback) noexcept
{
	using callback_type = ::std::remove_cvref_t<T>;
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<sched_type>;
	using cookie_type = win32_iocp_connect_cookie<alloc_type, callback_type>;

	::std::size_t const sock{stream.native_handle()};
	if (addr == nullptr || addrlen == 0 ||
		addrlen > sizeof(::fast_io::posix_sockaddr_storage)) [[unlikely]]
	{
		callback(::fast_io::details::async_make_error(::std::errc::invalid_argument));
		return;
	}

	::fast_io::win32::connectex_func connectex{};
	try
	{
		connectex = win32_iocp_connectex(sock);
		win32_iocp_associate(sched.native_handle(), reinterpret_cast<void *>(sock));
	}
	catch throws(::std::error e)
	{
		callback(e.release());
		return;
	}

	cookie_type *cookie;
	try
	{
		cookie = ::fast_io::details::async_new_state_plain<cookie_type>(
			sched,
			win32_iocp_state_base{{},
								  &win32_iocp_connect_deliver<alloc_type, callback_type>,
								  reinterpret_cast<void *>(sock)},
			0u, ::fast_io::posix_sockaddr_storage{},
			callback_type{::std::forward<T>(callback)});
	}
	catch throws(::std::error e)
	{
		callback(e.release());
		return;
	}
	__builtin_memcpy(__builtin_addressof(cookie->addr), addr, addrlen);

	try
	{
		/* ConnectEx requires a bound socket: bind the peer family's
		 * wildcard address so the OS picks the source port/address */
		::fast_io::posix_sockaddr_storage any{};
		any.ss_family =
			static_cast<::fast_io::posix_sockaddr const *>(addr)->sa_family;
		if (::fast_io::win32::bind(sock, __builtin_addressof(any),
								   static_cast<int>(addrlen)) != 0) [[unlikely]]
		{
			throw_win32_error(
				static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
		}
	}
	catch throws(::std::error e)
	{
		auto cb{::std::move(cookie->callback)};
		::fast_io::details::async_delete_state(cookie);
		cb(e.release());
		return;
	}

	if (timeout.has_opt)
	{
		if (timeout.opt.tv_sec == 0 && timeout.opt.tv_nsec == 0) [[unlikely]]
		{
			auto cb{::std::move(cookie->callback)};
			::fast_io::details::async_delete_state(cookie);
			cb(::fast_io::details::async_make_error(::std::errc::timed_out));
			return;
		}
		cookie->base.timer = ::fast_io::win32::CreateThreadpoolTimer(
			win32_iocp_timer_thunk, cookie, nullptr);
		if (cookie->base.timer == nullptr) [[unlikely]]
		{
			auto err{::fast_io::win32::GetLastError()};
			auto cb{::std::move(cookie->callback)};
			::fast_io::details::async_delete_state(cookie);
			cb(::fast_io::details::async_make_error(
				static_cast<::fast_io::freestanding::win32_errc>(err)));
			return;
		}
	}

	/* synchronous success and WSA_IO_PENDING both queue a completion */
	if (connectex(sock, __builtin_addressof(cookie->addr), static_cast<int>(addrlen), nullptr,
				  0u, __builtin_addressof(cookie->received),
				  __builtin_addressof(cookie->base.ovl)) == 0)
	{
		auto const err{
			static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError())};
		if (err != 997u) [[unlikely]] // WSA_IO_PENDING
		{
			if (auto *timer{cookie->base.timer}; timer != nullptr)
			{
				::fast_io::win32::CloseThreadpoolTimer(timer);
			}
			auto cb{::std::move(cookie->callback)};
			::fast_io::details::async_delete_state(cookie);
			cb(::fast_io::details::async_make_error(
				static_cast<::fast_io::freestanding::win32_errc>(err)));
			return;
		}
	}
	if (auto *timer{cookie->base.timer}; timer != nullptr)
	{
		auto due{win32_iocp_relative_deadline(timeout.opt)};
		::fast_io::win32::SetThreadpoolTimer(timer, __builtin_addressof(due), 0, 0);
	}
}

/* ======================= close ======================= */

/*
 * Shared handle-close worker for the win32 schedulers: Windows has no
 * kernel-side asynchronous close op (IoRing's op enum has none either),
 * so async_close_define runs the family's synchronous close on a
 * threadpool work item and ferries the completion back through the
 * scheduler's pump — a posted OVERLAPPED packet for IOCP, a queued
 * cancel-request sqe for IoRing. The stored result is what the user
 * callback reports; the ferry's own outcome is irrelevant.
 * kind: 0 CloseHandle (win32-family files/pipes), 1 closesocket,
 *       2 NtClose, 3 ZwClose (nt-family observers).
 */
inline ::std::cxx_std_error win32_close_handle_now(void *handle, int kind) noexcept
{
	switch (kind)
	{
	case 1:
		if (::fast_io::win32::closesocket(reinterpret_cast<::std::size_t>(handle)) != 0)
		{
			return ::fast_io::details::async_make_error(
				static_cast<::fast_io::freestanding::win32_errc>(
					static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError())));
		}
		break;
	case 3:
		if (auto const status{::fast_io::win32::nt::ZwClose(handle)}; status != 0)
		{
			return ::fast_io::details::async_make_error(
				static_cast<::fast_io::freestanding::nt_errc>(status));
		}
		break;
	case 2:
		if (auto const status{::fast_io::win32::nt::NtClose(handle)}; status != 0)
		{
			return ::fast_io::details::async_make_error(
				static_cast<::fast_io::freestanding::nt_errc>(status));
		}
		break;
	default:
		if (::fast_io::win32::CloseHandle(handle) == 0)
		{
			return ::fast_io::details::async_make_error(
				static_cast<::fast_io::freestanding::win32_errc>(
					::fast_io::win32::GetLastError()));
		}
		break;
	}
	return {};
}

/*
 * Close cookie: the state base rides the posted OVERLAPPED packet — a
 * posted packet always reports success, so the worker records the
 * close's real outcome in result and the pump-side deliver reads it.
 */
struct win32_iocp_close_state_base : win32_iocp_state_base
{
	void *port{}; /* the port to ferry the completion through */
	int close_kind{};
	::std::cxx_std_error result{};
};

template <typename alloc_type, typename T>
struct win32_iocp_close_cookie
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	win32_iocp_close_state_base base;
	T callback;
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

/* pool worker: run the synchronous close, then post the packet so the
 * callback lands on the pump thread like a real op's */
inline ::std::uint_least32_t FAST_IO_WINSTDCALL win32_iocp_close_work(void *context) noexcept
{
	auto *state{static_cast<win32_iocp_close_state_base *>(context)};
	state->result = win32_close_handle_now(state->file_handle, state->close_kind);
	if (::fast_io::win32::PostQueuedCompletionStatus(
			state->port, 0u, 0u, __builtin_addressof(state->ovl)) == 0) [[unlikely]]
	{
		/* the port is gone — deliver on this worker rather than lose
		 * the callback */
		state->invoke(state, 0, 0);
	}
	return 0;
}

template <typename alloc_type, typename T>
inline void win32_iocp_close_deliver(void *self, ::std::size_t,
									 ::std::uint_least32_t) noexcept
{
	using cookie_type = win32_iocp_close_cookie<alloc_type, T>;
	auto *cookie{static_cast<cookie_type *>(self)};
	auto callback{::std::move(cookie->callback)};
	auto err{cookie->base.result};
	::fast_io::details::async_delete_state(cookie);
	callback(err);
}

/*
 * One emulated-close submission: allocate the cookie, then queue the
 * worker. Any submission failure still closes the handle inline — the
 * op owns it from the moment the define is invoked.
 */
template <typename sched_type, typename T>
inline void win32_iocp_close_submit(sched_type sched, void *port, void *handle, int kind,
									::fast_io::posix_statx_timestamp_opt, T callback) noexcept
{
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<sched_type>;
	using callback_type = ::std::remove_cvref_t<T>;
	using cookie_type = win32_iocp_close_cookie<alloc_type, callback_type>;
	cookie_type *cookie;
	try
	{
		cookie = ::fast_io::details::async_new_state_plain<cookie_type>(
			sched,
			win32_iocp_close_state_base{
				{{}, &win32_iocp_close_deliver<alloc_type, callback_type>, handle},
				port,
				kind,
				{}},
			callback_type{::std::move(callback)});
	}
	catch throws(::std::error e)
	{
		win32_close_handle_now(handle, kind);
		callback(e.release());
		return;
	}
	/* WT_EXECUTELONGFUNCTION: a close can block in driver teardown. If
	 * the default pool rejects the work, run it inline — the completion
	 * still ferries through the port */
	if (::fast_io::win32::QueueUserWorkItem(::fast_io::details::win32_iocp_close_work, cookie,
											0x00000010u) == 0) [[unlikely]]
	{
		win32_iocp_close_work(cookie);
	}
}

} // namespace details

/*
 * Event pump: dequeue one completion and dispatch it to its cookie.
 * io_async_wait blocks; io_async_peek returns false when nothing is
 * queued; io_async_wait_timeout returns false when the deadline elapsed.
 * A completion carrying no OVERLAPPED (PostQueuedCompletionStatus) is an
 * external wakeup — consumed, nothing dispatched.
 */
inline void io_async_wait(win32_io_observer port) throws
{
	::std::uint_least32_t transferred{};
	::std::size_t completionkey{};
	::fast_io::win32::overlapped *over{};
	::std::uint_least32_t err{};
	if (!::fast_io::win32::GetQueuedCompletionStatus(port.native_handle(),
													 __builtin_addressof(transferred),
													 __builtin_addressof(completionkey),
													 __builtin_addressof(over),
													 ~0u)) [[unlikely]]
	{
		if (over == nullptr)
		{
			throw_win32_error();
		}
		err = ::fast_io::win32::GetLastError();
	}
	if (over != nullptr)
	{
		::fast_io::details::win32_iocp_dispatch(over, transferred, err);
	}
}

inline bool io_async_peek(win32_io_observer port) throws
{
	::std::uint_least32_t transferred{};
	::std::size_t completionkey{};
	::fast_io::win32::overlapped *over{};
	::std::uint_least32_t err{};
	if (!::fast_io::win32::GetQueuedCompletionStatus(port.native_handle(),
													 __builtin_addressof(transferred),
													 __builtin_addressof(completionkey),
													 __builtin_addressof(over),
													 0u))
	{
		if (over == nullptr)
		{
			auto code{::fast_io::win32::GetLastError()};
			if (code == 258u) // WAIT_TIMEOUT
			{
				return false;
			}
			throw_win32_error(code);
		}
		err = ::fast_io::win32::GetLastError();
	}
	if (over != nullptr)
	{
		::fast_io::details::win32_iocp_dispatch(over, transferred, err);
	}
	return true;
}

inline bool io_async_wait_timeout(win32_io_observer port,
								  ::fast_io::posix_statx_timestamp64 timeout) throws
{
	/* the port bound is a relative duration here, same as the uring
	 * backend — convert {sec, nsec} to GQCS milliseconds */
	auto ms{static_cast<::std::uint_least64_t>(timeout.tv_sec) * 1000u +
			timeout.tv_nsec / 1000000u};
	if (ms > 0xFFFFFFFEu) [[unlikely]]
	{
		ms = 0xFFFFFFFEu; // anything longer saturates below INFINITE
	}
	::std::uint_least32_t transferred{};
	::std::size_t completionkey{};
	::fast_io::win32::overlapped *over{};
	::std::uint_least32_t err{};
	if (!::fast_io::win32::GetQueuedCompletionStatus(port.native_handle(),
													 __builtin_addressof(transferred),
													 __builtin_addressof(completionkey),
													 __builtin_addressof(over),
													 static_cast<::std::uint_least32_t>(ms)))
	{
		if (over == nullptr)
		{
			auto code{::fast_io::win32::GetLastError()};
			if (code == 258u) // WAIT_TIMEOUT
			{
				return false;
			}
			throw_win32_error(code);
		}
		err = ::fast_io::win32::GetLastError();
	}
	if (over != nullptr)
	{
		::fast_io::details::win32_iocp_dispatch(over, transferred, err);
	}
	return true;
}

/* ======================= backend defines ======================= */

/*
 * async_pread_some_bytes_underflow_callback_define: required low-level
 * define. cb is invoked once as cb(::std::cxx_std_error, ::std::size_t)
 * noexcept; a fired timeout reports errc::timed_out. Files and pipes
 * ride ReadFile; sockets ride WSARecv (posix_statx offsets are
 * meaningless there and are rejected). Handle family observers (nt and
 * win32) both arrive through the void* handle path.
 */
template <nt_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_pread_some_bytes_underflow_callback_define(
	::fast_io::win32_io_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_nt_family_io_observer<family, char_type> ntiob, ::std::byte *first,
	::std::size_t count, ::fast_io::intfpos_opt off, func callback) noexcept
{
	::fast_io::details::win32_iocp_rw_submit<false>(
		sched, ntiob, first, count, off, timeout, ::std::move(callback));
}

template <win32_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_pread_some_bytes_underflow_callback_define(
	::fast_io::win32_io_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_win32_family_io_observer<family, char_type> wiob, ::std::byte *first,
	::std::size_t count, ::fast_io::intfpos_opt off, func callback) noexcept
{
	::fast_io::details::win32_iocp_rw_submit<false>(
		sched, wiob, first, count, off, timeout, ::std::move(callback));
}

template <win32_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_pread_some_bytes_underflow_callback_define(
	::fast_io::win32_io_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_win32_family_socket_io_observer<family, char_type> wsiob, ::std::byte *first,
	::std::size_t count, ::fast_io::intfpos_opt off, func callback) noexcept
{
	::fast_io::details::win32_iocp_rw_submit<false>(
		sched, wsiob, first, count, off, timeout, ::std::move(callback));
}

template <nt_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_pwrite_some_bytes_overflow_callback_define(
	::fast_io::win32_io_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_nt_family_io_observer<family, char_type> ntiob, ::std::byte const *first,
	::std::size_t count, ::fast_io::intfpos_opt off, func callback) noexcept
{
	::fast_io::details::win32_iocp_rw_submit<true>(
		sched, ntiob, const_cast<::std::byte *>(first), count, off, timeout,
		::std::move(callback));
}

template <win32_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_pwrite_some_bytes_overflow_callback_define(
	::fast_io::win32_io_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_win32_family_io_observer<family, char_type> wiob, ::std::byte const *first,
	::std::size_t count, ::fast_io::intfpos_opt off, func callback) noexcept
{
	::fast_io::details::win32_iocp_rw_submit<true>(
		sched, wiob, const_cast<::std::byte *>(first), count, off, timeout,
		::std::move(callback));
}

template <win32_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_pwrite_some_bytes_overflow_callback_define(
	::fast_io::win32_io_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_win32_family_socket_io_observer<family, char_type> wsiob,
	::std::byte const *first, ::std::size_t count, ::fast_io::intfpos_opt off,
	func callback) noexcept
{
	::fast_io::details::win32_iocp_rw_submit<true>(
		sched, wsiob, const_cast<::std::byte *>(first), count, off, timeout,
		::std::move(callback));
}

template <win32_family family, ::std::integral char_type>
struct ::fast_io::operations::decay::defines::async_accept_file_type<
	::fast_io::basic_win32_family_socket_io_observer<family, char_type>>
{
	using type = ::fast_io::basic_win32_family_socket_file<family, char_type>;
};

/*
 * async_accept_callback_define: AcceptEx on the listen socket; the
 * callback receives the accepted SOCKET through
 * cb(::std::cxx_std_error, ::std::size_t). The accept socket is created
 * overlapped-capable (WSA_FLAG_OVERLAPPED forced on) and gets
 * SO_UPDATE_ACCEPT_CONTEXT on completion; no_block additionally applies
 * FIONBIO so sync calls report WSAEWOULDBLOCK like posix O_NONBLOCK.
 */
template <win32_family family, ::std::integral char_type, typename func>
	requires ::std::is_nothrow_invocable_v<func, ::std::cxx_std_error, ::std::size_t>
inline void async_accept_callback_define(
	::fast_io::win32_io_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_win32_family_socket_io_observer<family, char_type> wsiob,
	::fast_io::open_mode m, func callback) noexcept
{
	::fast_io::details::win32_iocp_accept_submit<family>(
		sched, wsiob, m, timeout, ::std::move(callback));
}

/*
 * async_connect_define: ConnectEx on an unconnected socket — the socket
 * must carry WSA_FLAG_OVERLAPPED (win32 sockets minted through the
 * factory always do). The peer address is copied at submission and the
 * socket is bound to the family's wildcard first, as ConnectEx requires;
 * on success it gets SO_UPDATE_CONNECT_CONTEXT. The socket is not
 * consumed — a failed connect leaves it unconnected for the caller to
 * close.
 */
template <win32_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_connect_define(
	::fast_io::win32_io_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_win32_family_socket_io_observer<family, char_type> wsiob, void const *addr,
	::std::size_t addrlen, func callback) noexcept
{
	::fast_io::details::win32_iocp_connect_submit<family>(
		sched, wsiob, addr, addrlen, timeout, ::std::move(callback));
}

/*
 * async_close_define: Windows exposes no kernel close op, so the
 * family's synchronous close runs on a threadpool worker and its
 * completion is ferried through the port — the callback still arrives
 * on the pump thread like a real op's. The stream needs no port
 * association: PostQueuedCompletionStatus targets the port directly.
 * The timeout is advisory and ignored — a queued work item always runs.
 */
template <nt_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_close_define(
	::fast_io::win32_io_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_nt_family_io_observer<family, char_type> ntiob, func callback) noexcept
{
	::fast_io::details::win32_iocp_close_submit(
		sched, sched.native_handle(), ntiob.handle,
		family == nt_family::zw ? 3 : 2, timeout, ::std::move(callback));
}

template <win32_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_close_define(
	::fast_io::win32_io_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_win32_family_io_observer<family, char_type> wiob, func callback) noexcept
{
	::fast_io::details::win32_iocp_close_submit(
		sched, sched.native_handle(), wiob.handle, 0, timeout, ::std::move(callback));
}

template <win32_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_close_define(
	::fast_io::win32_io_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_win32_family_socket_io_observer<family, char_type> wsiob,
	func callback) noexcept
{
	::fast_io::details::win32_iocp_close_submit(
		sched, sched.native_handle(), reinterpret_cast<void *>(wsiob.hsocket), 1, timeout,
		::std::move(callback));
}

} // namespace fast_io

#endif
