#pragma once
/*
 * IoRing backend for fast_io's generic async layer — the Windows 11
 * answer to io_uring: kernel-managed submission/completion ring buffers
 * instead of per-op OVERLAPPED packets.
 *
 * The API surface (CreateIoRing/SubmitIoRing/PopIoRingCompletion/
 * BuildIoRing*) lives in api-ms-win-core-ioring-l1-1-0.dll — available
 * only on Windows 11+ and absent under wine — so every entry point is
 * resolved at runtime through GetProcAddress rather than statically
 * imported: constructing a win32_ioring scheduler on a system without
 * the dll fails cleanly with an error, while win32_file{io_async}
 * (IOCP) remains the default io_async backend unchanged.
 *
 * Coverage: IoRing supports file-object ops only — read, write, flush,
 * cancel, scatter/gather, open/close, registration. There are no socket
 * ops, so sockets (accept/read/write) stay on the IOCP backend; this
 * backend defines read/write only for nt- and win32-family handle
 * streams opened with open_mode::no_block (FILE_FLAG_OVERLAPPED).
 *
 * Native scatter/gather (BuildIoRingReadFileScatter/WriteFileGather)
 * requires page-aligned segments like ReadFileScatter/WriteFileGather —
 * unusable for arbitrary byte buffers, so scatter falls back to the
 * generic per-segment composition.
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
 * deadline. Like the IOCP backend there is no linked-timeout primitive,
 * so each timed op arms a threadpool timer; on expiry the callback
 * flags the cookie and queues a BuildIoRingCancelRequest whose
 * opToCancel is the op's userData, and the aborted cqe reports
 * errc::timed_out. The timer callback never owns or frees state — the
 * cookie lives until its op's cqe arrives, and the completion path
 * joins outstanding timer callbacks before freeing.
 */

#if defined(_WIN32) && !defined(_WIN32_WINDOWS)

namespace fast_io
{

namespace details
{

/* -------- ioringapi ABI (resolved at runtime; see ioringapi.h) -------- */

struct win32_ioring_create_flags
{
	::std::uint_least32_t required;
	::std::uint_least32_t advisory;
};
static_assert(sizeof(win32_ioring_create_flags) == 8);

struct win32_ioring_capabilities
{
	::std::uint_least32_t max_version;
	::std::uint_least32_t max_submission_queue_size;
	::std::uint_least32_t max_completion_queue_size;
	::std::uint_least32_t feature_flags;
};

struct win32_ioring_handle_ref
{
	::std::uint_least32_t kind; /* 0 raw handle, 1 registered index */
	union
	{
		void *handle;
		::std::uint_least32_t index;
	} data;
};

struct win32_ioring_buffer_ref
{
	::std::uint_least32_t kind; /* 0 raw pointer, 1 registered index+offset */
	union
	{
		void *address;
		struct
		{
			::std::uint_least32_t index;
			::std::uint_least32_t offset;
		} registered;
	} data;
};

struct win32_ioring_cqe
{
	::std::uintptr_t user_data;
	::std::int_least32_t result_code;
	::std::uintptr_t information;
};

using win32_ioring_create_fn = ::std::int_least32_t(FAST_IO_WINSTDCALL *)(
	::std::uint_least32_t version, win32_ioring_create_flags flags,
	::std::uint_least32_t sq_size, ::std::uint_least32_t cq_size, void **ring) noexcept;
using win32_ioring_submit_fn = ::std::int_least32_t(FAST_IO_WINSTDCALL *)(
	void *ring, ::std::uint_least32_t wait_ops, ::std::uint_least32_t ms,
	::std::uint_least32_t *submitted) noexcept;
using win32_ioring_pop_fn =
	::std::int_least32_t(FAST_IO_WINSTDCALL *)(void *ring, win32_ioring_cqe *cqe) noexcept;
using win32_ioring_close_fn =
	::std::int_least32_t(FAST_IO_WINSTDCALL *)(void *ring) noexcept;
using win32_ioring_set_event_fn =
	::std::int_least32_t(FAST_IO_WINSTDCALL *)(void *ring, void *event) noexcept;
using win32_ioring_query_caps_fn =
	::std::int_least32_t(FAST_IO_WINSTDCALL *)(win32_ioring_capabilities *caps) noexcept;
using win32_ioring_is_supported_fn = ::std::int_least32_t(FAST_IO_WINSTDCALL *)(
	void *ring, ::std::uint_least32_t op) noexcept;
using win32_ioring_build_read_fn = ::std::int_least32_t(FAST_IO_WINSTDCALL *)(
	void *ring, win32_ioring_handle_ref file, win32_ioring_buffer_ref buf,
	::std::uint_least32_t bytes, ::std::uint_least64_t offset,
	::std::uintptr_t user_data, ::std::uint_least32_t sqe_flags) noexcept;
using win32_ioring_build_write_fn = ::std::int_least32_t(FAST_IO_WINSTDCALL *)(
	void *ring, win32_ioring_handle_ref file, win32_ioring_buffer_ref buf,
	::std::uint_least32_t bytes, ::std::uint_least64_t offset,
	::std::uint_least32_t write_flags, ::std::uintptr_t user_data,
	::std::uint_least32_t sqe_flags) noexcept;
using win32_ioring_build_flush_fn = ::std::int_least32_t(FAST_IO_WINSTDCALL *)(
	void *ring, win32_ioring_handle_ref file, ::std::uint_least32_t flush_mode,
	::std::uintptr_t user_data, ::std::uint_least32_t sqe_flags) noexcept;
using win32_ioring_build_cancel_fn = ::std::int_least32_t(FAST_IO_WINSTDCALL *)(
	void *ring, win32_ioring_handle_ref file, ::std::uintptr_t op_to_cancel,
	::std::uintptr_t user_data) noexcept;

struct win32_ioring_api_table
{
	win32_ioring_create_fn create{};
	win32_ioring_submit_fn submit{};
	win32_ioring_pop_fn pop{};
	win32_ioring_close_fn close{};
	win32_ioring_build_read_fn build_read{};
	win32_ioring_build_write_fn build_write{};
	win32_ioring_build_cancel_fn build_cancel{};
	/* optional: flush / completion-event / capability probe — absent on
	 * older builds; the backend works without them */
	win32_ioring_build_flush_fn build_flush{};
	win32_ioring_set_event_fn set_event{};
	win32_ioring_query_caps_fn query_caps{};
	win32_ioring_is_supported_fn is_supported{};
	bool ok{};
};

/* Resolve one entry point from kernelbase first, then the ioring
 * apiset dll if a win11 build keeps the export there instead. */
inline void *win32_ioring_resolve_one(void *kb, void *apiset, char const *name) noexcept
{
	if (kb != nullptr)
	{
		if (auto p{::fast_io::win32::GetProcAddress(kb, name)}; p != nullptr)
		{
			return reinterpret_cast<void *>(p);
		}
	}
	if (apiset != nullptr)
	{
		return reinterpret_cast<void *>(::fast_io::win32::GetProcAddress(apiset, name));
	}
	return nullptr;
}

inline win32_ioring_api_table win32_ioring_load_api() noexcept
{
	win32_ioring_api_table t{};
	void *const kb{::fast_io::win32::GetModuleHandleA("kernelbase.dll")};
	void *apiset{::fast_io::win32::LoadLibraryExA(
		"api-ms-win-core-ioring-l1-1-0.dll", nullptr,
		0x00000800u /* LOAD_LIBRARY_SEARCH_SYSTEM32 */)};
	auto get{[&](char const *name) noexcept -> void * { return win32_ioring_resolve_one(kb, apiset, name); }};
	t.create = reinterpret_cast<win32_ioring_create_fn>(get("CreateIoRing"));
	t.submit = reinterpret_cast<win32_ioring_submit_fn>(get("SubmitIoRing"));
	t.pop = reinterpret_cast<win32_ioring_pop_fn>(get("PopIoRingCompletion"));
	t.close = reinterpret_cast<win32_ioring_close_fn>(get("CloseIoRing"));
	t.build_read = reinterpret_cast<win32_ioring_build_read_fn>(get("BuildIoRingReadFile"));
	t.build_write = reinterpret_cast<win32_ioring_build_write_fn>(get("BuildIoRingWriteFile"));
	t.build_cancel = reinterpret_cast<win32_ioring_build_cancel_fn>(get("BuildIoRingCancelRequest"));
	t.build_flush = reinterpret_cast<win32_ioring_build_flush_fn>(get("BuildIoRingFlushFile"));
	t.set_event = reinterpret_cast<win32_ioring_set_event_fn>(get("SetIoRingCompletionEvent"));
	t.query_caps = reinterpret_cast<win32_ioring_query_caps_fn>(get("QueryIoRingCapabilities"));
	t.is_supported = reinterpret_cast<win32_ioring_is_supported_fn>(get("IsIoRingOpSupported"));
	t.ok = t.create != nullptr && t.submit != nullptr && t.pop != nullptr &&
		   t.close != nullptr && t.build_read != nullptr && t.build_write != nullptr &&
		   t.build_cancel != nullptr;
	return t;
}

inline win32_ioring_api_table const *win32_ioring_api() noexcept
{
	static win32_ioring_api_table const table{win32_ioring_load_api()};
	return table.ok ? __builtin_addressof(table) : nullptr;
}

/* HRESULT from a cqe (or a builder) -> cxx_std_error.
 * Windows IoRing surfaces NT status codes HRESULT-wrapped
 * (HRESULT_FROM_NTSTATUS sets FACILITY_NT_BIT, 0x20000000) and
 * occasionally raw NTSTATUS; win32 errors ride FACILITY_WIN32.
 * Returns {nullptr,*} success for S_OK and for EOF on reads. */
inline ::std::cxx_std_error win32_ioring_result_to_error(::std::uint_least32_t hr,
														 bool is_read) noexcept
{
	if (static_cast<::std::int_least32_t>(hr) >= 0)
	{
		return {};
	}
	/* STATUS_END_OF_FILE raw (0xC0000011) or wrapped (0xD0000011): like
	 * ERROR_HANDLE_EOF in the IOCP backend — stream end, not an error */
	if (is_read && (hr == 0xC0000011u || hr == 0xD0000011u))
	{
		return {};
	}
	if ((hr & 0xFFFF0000u) == 0x80070000u) /* FACILITY_WIN32 */
	{
		return ::fast_io::details::async_make_error(
			static_cast<::fast_io::freestanding::win32_errc>(hr & 0xFFFFu));
	}
	if (hr & 0x20000000u) /* FACILITY_NT_BIT: HRESULT_FROM_NTSTATUS */
	{
		return ::fast_io::details::async_make_error(
			static_cast<::fast_io::freestanding::nt_errc>(hr & ~0x20000000u));
	}
	if ((hr & 0xC0000000u) != 0) /* raw NTSTATUS error severity */
	{
		return ::fast_io::details::async_make_error(
			static_cast<::fast_io::freestanding::nt_errc>(hr));
	}
	return ::fast_io::details::async_make_error(
		static_cast<::fast_io::freestanding::com_errc>(hr));
}

/*
 * Completion ABI: a cqe's userData is the cookie pointer the sqe was
 * built with. Every state object embeds win32_ioring_state_base FIRST —
 * userData IS the state pointer; invoke() is the per-op finisher.
 * Internal sqes (cancel requests) use userData == 0 and are dropped by
 * the pump.
 */
using win32_ioring_invoke_func =
	void (*)(void *, ::std::uintptr_t, ::std::uint_least32_t) noexcept;

struct win32_ioring_state_base
{
	win32_ioring_invoke_func invoke;
	void *ring{};        /* HIORING — for BuildIoRingCancelRequest */
	void *file_handle{}; /* stream handle — for the cancel's file ref */
	void *timer{};       /* PTP_TIMER when a deadline is armed */
	bool timed_out{};
};

inline void win32_ioring_dispatch(::std::uintptr_t user_data, ::std::uintptr_t info,
								  ::std::uint_least32_t hr) noexcept
{
	auto *base{reinterpret_cast<win32_ioring_state_base *>(user_data)};
	base->invoke(base, info, hr);
}

/*
 * Threadpool-timer thunk: flags the cookie and queues a cancel request
 * for the op whose userData is the cookie; the aborted cqe is what frees
 * it. Needs its own SubmitIoRing kick — a queued sqe is invisible to the
 * kernel until submitted.
 */
inline void FAST_IO_WINSTDCALL win32_ioring_timer_thunk(void *, void *context, void *) noexcept
{
	auto *state{static_cast<win32_ioring_state_base *>(context)};
	state->timed_out = true;
	auto const *api{win32_ioring_api()};
	if (api != nullptr) [[likely]]
	{
		win32_ioring_handle_ref file{};
		file.kind = 0;
		file.data.handle = state->file_handle;
		api->build_cancel(state->ring, file,
						  reinterpret_cast<::std::uintptr_t>(state), 0u);
		api->submit(state->ring, 0u, 0u, nullptr);
	}
}

template <typename alloc_type, typename T>
struct win32_ioring_rw_cookie
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	win32_ioring_state_base base;
	T callback;
	/* FILE_USE_FILE_POINTER_POSITION emulation: when the op went out with
	 * no explicit offset the file position advances by the delivered byte
	 * count */
	bool advance_position{};
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

template <typename alloc_type, typename T, bool is_read>
inline void win32_ioring_rw_deliver(void *self, ::std::uintptr_t information,
									::std::uint_least32_t hr) noexcept
{
	using cookie_type = win32_ioring_rw_cookie<alloc_type, T>;
	auto *cookie{static_cast<cookie_type *>(self)};
	if (auto *timer{cookie->base.timer}; timer != nullptr)
	{
		::fast_io::win32::SetThreadpoolTimer(timer, nullptr, 0, 0);
		::fast_io::win32::WaitForThreadpoolTimerCallbacks(timer, 1);
		::fast_io::win32::CloseThreadpoolTimer(timer);
	}
	bool const timed_out{cookie->base.timed_out};
	::std::size_t const transferred{static_cast<::std::size_t>(information)};
	if (cookie->advance_position)
	{
		::fast_io::win32::SetFilePointerEx(
			cookie->base.file_handle, static_cast<::std::int_least64_t>(transferred), nullptr,
			1u /* FILE_CURRENT */);
	}
	auto callback{::std::move(cookie->callback)};
	::fast_io::details::async_delete_state(cookie);

	::std::cxx_std_error e{};
	if (timed_out && static_cast<::std::int_least32_t>(hr) < 0) [[unlikely]]
	{
		/* the timer queued a cancel and this cqe is the aborted op; a
		 * completion that beat the deadline arrives successful and still
		 * reports success */
		e = ::fast_io::details::async_make_error(::std::errc::timed_out);
	}
	else
	{
		e = win32_ioring_result_to_error(hr, is_read);
	}
	callback(e, transferred);
}

/* Same async-capability check as the IOCP backend: IoRing ops are async
 * IRPs, so the file object must have been created without
 * FILE_SYNCHRONOUS_IO_* (i.e. opened with open_mode::no_block). */
inline void win32_ioring_check_overlapped(void *file_handle) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::nt::io_status_block isb{};
	::std::uint_least32_t fmode{};
	if (::fast_io::win32::nt::NtQueryInformationFile(
			file_handle, __builtin_addressof(isb), __builtin_addressof(fmode),
			static_cast<::std::uint_least32_t>(sizeof(fmode)),
			::fast_io::win32::nt::file_information_class::FileModeInformation) == 0 &&
		(fmode & 0x30u) != 0) [[unlikely]]
	{
		throw_win32_error(50u /* ERROR_NOT_SUPPORTED */);
	}
}

/*
 * One read/write submission against a file-family handle. Every failure
 * — including submission failures — is delivered through the callback
 * exactly once.
 */
template <bool is_write, typename sched_type, typename stream_type, typename T>
inline void win32_ioring_rw_submit(sched_type sched, stream_type stream, void *first,
								   ::std::size_t count, ::fast_io::intfpos_opt off,
								   ::fast_io::posix_statx_timestamp_opt timeout,
								   T &&callback) noexcept
{
	using callback_type = ::std::remove_cvref_t<T>;
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<sched_type>;
	using cookie_type = win32_ioring_rw_cookie<alloc_type, callback_type>;

	auto const *api{win32_ioring_api()};
	void *const file_handle{stream.native_handle()};

	::std::cxx_std_error early_err{};
	if (api == nullptr) [[unlikely]]
	{
		early_err = ::fast_io::details::async_make_error(::std::errc::not_supported);
	}
	else
	{
		try
		{
			win32_ioring_check_overlapped(file_handle);
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
			win32_ioring_state_base{&win32_ioring_rw_deliver<alloc_type, callback_type, !is_write>,
									sched.native_handle(), file_handle},
			callback_type{::std::forward<T>(callback)});
	}
	catch throws(::std::error e)
	{
		callback(e.release(), 0zu);
		return;
	}

	::std::uint_least64_t offset{};
	if (off.has_opt)
	{
		offset = static_cast<::std::uint_least64_t>(off.opt);
	}
	else
	{
		/* empty offset = current file position: FILE_USE_FILE_POINTER_POSITION.
		 * Emulate like the IOCP backend: read the object's position as an
		 * explicit offset and advance it on delivery. Non-seekable handles
		 * can't report one — their offset is ignored, so 0 is fine. */
		::std::int_least64_t cur{};
		if (::fast_io::win32::SetFilePointerEx(file_handle, 0, __builtin_addressof(cur),
											   1u /* FILE_CURRENT */))
		{
			offset = static_cast<::std::uint_least64_t>(cur);
			cookie->advance_position = true;
		}
	}

	::std::uint_least32_t const len{
		::fast_io::details::read_write_bytes_compute<::std::uint_least32_t>(
			static_cast<::std::byte *>(first), count)};

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
			win32_ioring_timer_thunk, cookie, nullptr);
		if (cookie->base.timer == nullptr) [[unlikely]]
		{
			auto cb{::std::move(cookie->callback)};
			::fast_io::details::async_delete_state(cookie);
			cb(::fast_io::details::async_make_error(::std::errc::not_enough_memory), 0zu);
			return;
		}
	}

	win32_ioring_handle_ref file_ref{};
	file_ref.kind = 0;
	file_ref.data.handle = file_handle;
	win32_ioring_buffer_ref buf_ref{};
	buf_ref.kind = 0;
	buf_ref.data.address = first;

	::std::int_least32_t hr{};
	if constexpr (is_write)
	{
		hr = api->build_write(sched.native_handle(), file_ref, buf_ref, len, offset,
							  0u /* FILE_WRITE_FLAGS_NONE */,
							  reinterpret_cast<::std::uintptr_t>(cookie), 0u);
	}
	else
	{
		hr = api->build_read(sched.native_handle(), file_ref, buf_ref, len, offset,
							 reinterpret_cast<::std::uintptr_t>(cookie), 0u);
	}
	if (hr >= 0)
	{
		/* flush the pending sqe without waiting for completions */
		hr = api->submit(sched.native_handle(), 0u, 0u, nullptr);
	}
	if (hr >= 0)
	{
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
	cb(win32_ioring_result_to_error(static_cast<::std::uint_least32_t>(hr), !is_write), 0zu);
}

/* ======================= close ======================= */

/*
 * Emulated async close: IoRing's public op enum has no close op, so the
 * family's synchronous close runs on a threadpool worker and the
 * completion is ferried through the ring — a cancel-request sqe whose
 * opToCancel (userData 1) matches no live op completes instantly, and
 * its cqe carries our cookie back to the pump. The stored result is
 * what the user callback reports; the ferry's own cqe result is ignored.
 */
struct win32_ioring_close_state_base : win32_ioring_state_base
{
	int close_kind{};
	::std::cxx_std_error result{};
};

template <typename alloc_type, typename T>
struct win32_ioring_close_cookie
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	win32_ioring_close_state_base base;
	T callback;
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

/*
 * Push an internal sqe whose cqe carries the cookie back to the pump
 * thread: a cancel-request whose opToCancel (userData 1) matches no
 * live op completes instantly. Workers that finished a synchronous op
 * off-thread call this so the callback lands on the pump like a real
 * op's. Returns false when the api surface is gone.
 */
inline bool win32_ioring_ferry_completion(win32_ioring_state_base *state) noexcept
{
	auto const *api{win32_ioring_api()};
	if (api == nullptr) [[unlikely]]
	{
		return false;
	}
	win32_ioring_handle_ref file{};
	file.kind = 0;
	file.data.handle = state->file_handle;
	return api->build_cancel(state->ring, file, 1u /* opToCancel */,
							 reinterpret_cast<::std::uintptr_t>(state)) >= 0 &&
		   api->submit(state->ring, 0u, 0u, nullptr) >= 0;
}

/* pool worker: run the synchronous close, then ferry the cookie through
 * the ring so the callback lands on the pump thread like a real op's */
inline ::std::uint_least32_t FAST_IO_WINSTDCALL win32_ioring_close_work(void *context) noexcept
{
	auto *state{static_cast<win32_ioring_close_state_base *>(context)};
	state->result = ::fast_io::details::win32_close_handle_now(state->file_handle,
															   state->close_kind);
	if (!win32_ioring_ferry_completion(state)) [[unlikely]]
	{
		/* no ferry available — deliver on this worker rather than
		 * lose the callback */
		state->invoke(state, 0, 0);
	}
	return 0;
}

template <typename alloc_type, typename T>
inline void win32_ioring_close_deliver(void *self, ::std::uintptr_t,
									   ::std::uint_least32_t) noexcept
{
	using cookie_type = win32_ioring_close_cookie<alloc_type, T>;
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
inline void win32_ioring_close_submit(sched_type sched, void *ring, void *handle, int kind,
									  ::fast_io::posix_statx_timestamp_opt, T callback) noexcept
{
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<sched_type>;
	using callback_type = ::std::remove_cvref_t<T>;
	using cookie_type = win32_ioring_close_cookie<alloc_type, callback_type>;
	cookie_type *cookie;
	try
	{
		cookie = ::fast_io::details::async_new_state_plain<cookie_type>(
			sched,
			win32_ioring_close_state_base{
				{&win32_ioring_close_deliver<alloc_type, callback_type>, ring, handle},
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
	 * the default pool rejects the work, run it inline — the close and
	 * the ferry still happen on this thread */
	if (::fast_io::win32::QueueUserWorkItem(::fast_io::details::win32_ioring_close_work, cookie,
											0x00000010u) == 0) [[unlikely]]
	{
		win32_ioring_close_work(cookie);
	}
}

/* ======================= sockets ======================= */

/*
 * IoRing's public op enum has no socket requests, so read/write on a
 * socket observer are emulated exactly like close: the synchronous
 * WSARecv/WSASend runs on a default-pool worker and the completion is
 * ferried through the ring — the callback still lands on the pump
 * thread. Same honest limitation as win32_thread_pool: a recv on an
 * empty socket parks an OS worker for its duration, and a deadline
 * applies only until the work item dequeues — a running syscall cannot
 * be preempted.
 */
struct win32_ioring_sock_rw_state_base : win32_ioring_state_base
{
	::std::size_t hsocket{};
	::std::byte *buf{};
	::std::uint_least32_t count{};
	::std::uint_least64_t deadline{};
	::std::cxx_std_error result{};
	::std::size_t transferred{};
	bool is_write{};
};

template <typename alloc_type, typename T>
struct win32_ioring_sock_rw_cookie
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	win32_ioring_sock_rw_state_base base;
	T callback;
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

template <typename alloc_type, typename T>
inline void win32_ioring_sock_rw_deliver(void *self, ::std::uintptr_t,
										 ::std::uint_least32_t) noexcept
{
	using cookie_type = win32_ioring_sock_rw_cookie<alloc_type, T>;
	auto *cookie{static_cast<cookie_type *>(self)};
	auto callback{::std::move(cookie->callback)};
	auto err{cookie->base.result};
	auto const transferred{cookie->base.transferred};
	::fast_io::details::async_delete_state(cookie);
	callback(err, transferred);
}

/* pool worker: blocking WSARecv/WSASend, then the ring ferry */
inline ::std::uint_least32_t FAST_IO_WINSTDCALL win32_ioring_sock_rw_work(void *context) noexcept
{
	auto *state{static_cast<win32_ioring_sock_rw_state_base *>(context)};
	if (win32_thread_pool_expired(state->deadline))
	{
		state->result = ::fast_io::details::async_make_error(::std::errc::timed_out);
	}
	else
	{
		::fast_io::win32::wsabuf w{};
		w.buf = reinterpret_cast<char *>(state->buf);
		w.len = state->count;
		::std::uint_least32_t got{};
		int rc;
		if (state->is_write)
		{
			rc = ::fast_io::win32::WSASend(state->hsocket, __builtin_addressof(w), 1,
										   __builtin_addressof(got), 0, nullptr, nullptr);
		}
		else
		{
			::std::uint_least32_t flags{};
			rc = ::fast_io::win32::WSARecv(state->hsocket, __builtin_addressof(w), 1,
										   __builtin_addressof(got), __builtin_addressof(flags),
										   nullptr, nullptr);
		}
		if (rc != 0)
		{
			state->result = ::fast_io::details::async_make_error(
				static_cast<::fast_io::freestanding::win32_errc>(
					static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError())));
		}
		else
		{
			state->transferred = static_cast<::std::size_t>(got);
		}
	}
	if (!win32_ioring_ferry_completion(state)) [[unlikely]]
	{
		state->invoke(state, 0, 0);
	}
	return 0;
}

/*
 * One emulated socket read/write: allocate the cookie, then queue the
 * worker. A WSARecv can block indefinitely, so a rejected work item is
 * reported through the callback rather than run inline — unlike close,
 * running it on the submission thread could hang it.
 */
template <bool is_write, typename sched_type, typename stream_type, typename T>
inline void win32_ioring_sock_rw_submit(sched_type sched, stream_type stream, void *first,
										::std::size_t count,
										::fast_io::posix_statx_timestamp_opt timeout,
										T &&callback) noexcept
{
	using callback_type = ::std::remove_cvref_t<T>;
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<sched_type>;
	using cookie_type = win32_ioring_sock_rw_cookie<alloc_type, callback_type>;
	cookie_type *cookie;
	try
	{
		cookie = ::fast_io::details::async_new_state_plain<cookie_type>(
			sched,
			win32_ioring_sock_rw_state_base{
				{&win32_ioring_sock_rw_deliver<alloc_type, callback_type>,
				 sched.native_handle(), reinterpret_cast<void *>(stream.hsocket)},
				stream.hsocket,
				static_cast<::std::byte *>(first),
				::fast_io::details::read_write_bytes_compute<::std::uint_least32_t>(
					static_cast<::std::byte *>(first), count),
				timeout.has_opt ? win32_thread_pool_deadline(timeout.opt) : 0u,
				{},
				{},
				is_write},
			callback_type{::std::forward<T>(callback)});
	}
	catch throws(::std::error e)
	{
		callback(e.release(), 0zu);
		return;
	}
	/* WT_EXECUTELONGFUNCTION: the sync socket call may block */
	if (::fast_io::win32::QueueUserWorkItem(&win32_ioring_sock_rw_work, cookie,
											0x00000010u) == 0) [[unlikely]]
	{
		auto cb{::std::move(cookie->callback)};
		::fast_io::details::async_delete_state(cookie);
		cb(::fast_io::details::async_make_error(
			   static_cast<::fast_io::freestanding::win32_errc>(
				   static_cast<::std::uint_least32_t>(::fast_io::win32::GetLastError()))),
		   0zu);
	}
}

} // namespace details

/*
 * Non-owning IoRing observer: the first parameter of every async
 * operation. native_handle() yields the HIORING; `event` is the ring's
 * completion event (SetIoRingCompletionEvent) — the pump waits on it
 * instead of SubmitIoRing's waitOperations, which real IoRing rejects
 * with E_INVALIDARG when nothing is pending.
 */
class win32_ioring_observer
{
public:
	using native_handle_type = void *;
	using allocator_type = ::fast_io::native_global_allocator;
	native_handle_type ring{};
	void *event{};
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return ring;
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{ring};
		ring = nullptr;
		return temp;
	}
	inline constexpr explicit operator bool() const noexcept
	{
		return ring != nullptr;
	}
};

/*
 * Owning IoRing scheduler. The api surface is resolved at construction;
 * on systems without it (Windows 10, wine) the ctor fails with a win32
 * error — the IOCP io_async backend remains the default.
 *
 * A manual-reset event is bound via SetIoRingCompletionEvent — the
 * kernel signals it whenever a cqe is pushed, including completions
 * queued by other threads (the emulated-close ferry), which the
 * waitOperations path cannot wake for.
 *
 * Sizes are submission/completion entry counts. Defaults are modest —
 * every outstanding async op consumes one sqe slot for its lifetime.
 */
class win32_ioring : public win32_ioring_observer
{
public:
	inline explicit win32_ioring(::std::uint_least32_t submission_entries = 256,
								 ::std::uint_least32_t completion_entries = 512) FAST_IO_HERBCEPTIONS_THROWS
	{
		auto const *api{details::win32_ioring_api()};
		if (api == nullptr) [[unlikely]]
		{
			/* ERROR_MOD_NOT_FOUND: no ioring apiset on this system */
			throw_win32_error(126u);
		}
		/* Try newest-first: 400 (scatter/gather), 300 (write+flush+drain),
		 * 2, 1. An unrecognized version just fails creation; the cqes do
		 * not care which ring version produced them. */
		void *ring{};
		::std::int_least32_t hr{-1};
		for (::std::uint_least32_t const ver : {400u, 300u, 2u, 1u})
		{
			hr = api->create(ver, {0u, 0u}, submission_entries, completion_entries,
							 __builtin_addressof(ring));
			if (hr >= 0)
			{
				break;
			}
		}
		if (hr < 0) [[unlikely]]
		{
			auto e{details::win32_ioring_result_to_error(
				static_cast<::std::uint_least32_t>(hr), false)};
			if (e.domain != nullptr)
			{
				throw throws e;
			}
			throw_win32_error(50u /* ERROR_NOT_SUPPORTED */);
		}
		this->ring = ring;
		/* bind a manual-reset completion event so the pump can wait for
		 * cqes pushed by any thread — a plain SubmitIoRing wait only
		 * works while ops are pending and E_INVALIDARGs on an empty SQ */
		if (api->set_event != nullptr)
		{
			this->event = ::fast_io::win32::CreateEventW(nullptr, 1, 0, nullptr);
			if (this->event != nullptr && api->set_event(ring, this->event) < 0) [[unlikely]]
			{
				::fast_io::win32::CloseHandle(this->event);
				this->event = nullptr;
			}
		}
	}

	inline explicit win32_ioring(::fast_io::io_async_t) FAST_IO_HERBCEPTIONS_THROWS
		: win32_ioring()
	{
	}

	win32_ioring(win32_ioring const &) = delete;
	win32_ioring &operator=(win32_ioring const &) = delete;
	win32_ioring(win32_ioring &&) = delete;
	win32_ioring &operator=(win32_ioring &&) = delete;

	inline ~win32_ioring()
	{
		if (this->event != nullptr)
		{
			::fast_io::win32::CloseHandle(this->event);
			this->event = nullptr;
		}
		if (this->ring != nullptr)
		{
			auto const *api{details::win32_ioring_api()};
			api->close(this->ring);
		}
	}
};

/* scheduler reduction: the owning ring decays to its observer */
inline constexpr win32_ioring_observer
async_scheduler_ref_define(win32_ioring &ring) noexcept
{
	return {ring.native_handle(), ring.event};
}

/*
 * Event pump: flush pending sqes, then pop completions and dispatch one
 * to its cookie. The blocking waits ride the ring's completion event
 * (SetIoRingCompletionEvent) — the kernel signals it on every cqe push,
 * whoever submitted the sqe; SubmitIoRing's waitOperations is unusable
 * here because real IoRing answers E_INVALIDARG when the SQ is empty.
 * The reset-pop-wait order matters: a cqe landing between ResetEvent
 * and WaitForSingleObject re-signals the event, so the wait still
 * returns and the loop finds it on the next pop. Rings without a bound
 * event (old api surface) fall back to the waitOperations path.
 * Cqes with userData == 0 are internal sqes — consumed, not dispatched.
 */
inline void io_async_wait(win32_ioring_observer sched) FAST_IO_HERBCEPTIONS_THROWS
{
	auto const *api{details::win32_ioring_api()};
	if (api == nullptr) [[unlikely]]
	{
		throw_win32_error(126u /* ERROR_MOD_NOT_FOUND */);
	}
	for (;;)
	{
		auto const hr{api->submit(sched.native_handle(), 0u, 0u, nullptr)};
		if (hr < 0) [[unlikely]]
		{
			auto e{details::win32_ioring_result_to_error(
				static_cast<::std::uint_least32_t>(hr), false)};
			if (e.domain != nullptr)
			{
				throw throws e;
			}
		}
		for (;;)
		{
			if (sched.event != nullptr)
			{
				::fast_io::win32::ResetEvent(sched.event);
			}
			details::win32_ioring_cqe cqe{};
			auto const pr{api->pop(sched.native_handle(), __builtin_addressof(cqe))};
			if (pr >= 0 && cqe.user_data != 0)
			{
				details::win32_ioring_dispatch(cqe.user_data, cqe.information,
											   static_cast<::std::uint_least32_t>(cqe.result_code));
				return;
			}
			if (pr < 0)
			{
				if (sched.event != nullptr)
				{
					::fast_io::win32::WaitForSingleObject(sched.event, ~0u /* INFINITE */);
				}
				else
				{
					auto const wr{api->submit(sched.native_handle(), 1u,
											  ~0u /* INFINITE */, nullptr)};
					if (wr < 0) [[unlikely]]
					{
						auto e{details::win32_ioring_result_to_error(
							static_cast<::std::uint_least32_t>(wr), false)};
						if (e.domain != nullptr)
						{
							throw throws e;
						}
					}
				}
				break;
			}
		}
	}
}

inline bool io_async_peek(win32_ioring_observer sched) FAST_IO_HERBCEPTIONS_THROWS
{
	auto const *api{details::win32_ioring_api()};
	if (api == nullptr) [[unlikely]]
	{
		throw_win32_error(126u /* ERROR_MOD_NOT_FOUND */);
	}
	auto const hr{api->submit(sched.native_handle(), 0u, 0u, nullptr)};
	if (hr < 0) [[unlikely]]
	{
		auto e{details::win32_ioring_result_to_error(
			static_cast<::std::uint_least32_t>(hr), false)};
		if (e.domain != nullptr)
		{
			throw throws e;
		}
	}
	for (;;)
	{
		details::win32_ioring_cqe cqe{};
		if (api->pop(sched.native_handle(), __builtin_addressof(cqe)) < 0)
		{
			return false;
		}
		if (cqe.user_data == 0)
		{
			continue;
		}
		details::win32_ioring_dispatch(cqe.user_data, cqe.information,
									   static_cast<::std::uint_least32_t>(cqe.result_code));
		return true;
	}
}

inline bool io_async_wait_timeout(win32_ioring_observer sched,
								  ::fast_io::posix_statx_timestamp64 timeout) FAST_IO_HERBCEPTIONS_THROWS
{
	/* relative duration -> WaitForSingleObject milliseconds, rounded up
	 * and clamped below INFINITE */
	auto ms{static_cast<::std::uint_least64_t>(timeout.tv_sec) * 1000u +
			(timeout.tv_nsec + 999999u) / 1000000u};
	if (ms > 0xFFFFFFFEu) [[unlikely]]
	{
		ms = 0xFFFFFFFEu;
	}
	auto const *api{details::win32_ioring_api()};
	if (api == nullptr) [[unlikely]]
	{
		throw_win32_error(126u /* ERROR_MOD_NOT_FOUND */);
	}
	auto const hr{api->submit(sched.native_handle(), 0u, 0u, nullptr)};
	if (hr < 0) [[unlikely]]
	{
		auto e{details::win32_ioring_result_to_error(
			static_cast<::std::uint_least32_t>(hr), false)};
		if (e.domain != nullptr)
		{
			throw throws e;
		}
	}
	for (;;)
	{
		if (sched.event != nullptr)
		{
			::fast_io::win32::ResetEvent(sched.event);
		}
		for (;;)
		{
			details::win32_ioring_cqe cqe{};
			if (api->pop(sched.native_handle(), __builtin_addressof(cqe)) < 0)
			{
				break;
			}
			if (cqe.user_data == 0)
			{
				continue;
			}
			details::win32_ioring_dispatch(cqe.user_data, cqe.information,
										   static_cast<::std::uint_least32_t>(cqe.result_code));
			return true;
		}
		if (sched.event == nullptr ||
			::fast_io::win32::WaitForSingleObject(
				sched.event, static_cast<::std::uint_least32_t>(ms)) != 0 /* WAIT_OBJECT_0 */)
		{
			return false;
		}
	}
}

/* ======================= backend defines ======================= */
/*
 * async_pread_some_bytes_underflow_callback_define /
 * async_pwrite_some_bytes_overflow_callback_define: the required
 * low-level defines. cb is invoked once as
 * cb(::std::cxx_std_error, ::std::size_t). IoRing is file-only — nt and
 * win32 family observers are covered; sockets are not (they stay on the
 * IOCP backend, which owns socket defines).
 */
template <nt_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_pread_some_bytes_underflow_callback_define(
	::fast_io::win32_ioring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_nt_family_io_observer<family, char_type> ntiob, ::std::byte *first,
	::std::size_t count, ::fast_io::intfpos_opt off, func callback) noexcept
{
	::fast_io::details::win32_ioring_rw_submit<false>(
		sched, ntiob, first, count, off, timeout, ::std::move(callback));
}

template <win32_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_pread_some_bytes_underflow_callback_define(
	::fast_io::win32_ioring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_win32_family_io_observer<family, char_type> wiob, ::std::byte *first,
	::std::size_t count, ::fast_io::intfpos_opt off, func callback) noexcept
{
	::fast_io::details::win32_ioring_rw_submit<false>(
		sched, wiob, first, count, off, timeout, ::std::move(callback));
}

template <nt_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_pwrite_some_bytes_overflow_callback_define(
	::fast_io::win32_ioring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_nt_family_io_observer<family, char_type> ntiob, ::std::byte const *first,
	::std::size_t count, ::fast_io::intfpos_opt off, func callback) noexcept
{
	::fast_io::details::win32_ioring_rw_submit<true>(
		sched, ntiob, const_cast<::std::byte *>(first), count, off, timeout,
		::std::move(callback));
}

template <win32_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_pwrite_some_bytes_overflow_callback_define(
	::fast_io::win32_ioring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_win32_family_io_observer<family, char_type> wiob, ::std::byte const *first,
	::std::size_t count, ::fast_io::intfpos_opt off, func callback) noexcept
{
	::fast_io::details::win32_ioring_rw_submit<true>(
		sched, wiob, const_cast<::std::byte *>(first), count, off, timeout,
		::std::move(callback));
}

/*
 * Socket read/write: emulated — the blocking WSARecv/WSASend runs on a
 * default-pool worker and the completion is ferried through the ring.
 * Sockets have no positional I/O: a non-empty off reports invalid_seek.
 */
template <win32_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_pread_some_bytes_underflow_callback_define(
	::fast_io::win32_ioring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_win32_family_socket_io_observer<family, char_type> wsiob, ::std::byte *first,
	::std::size_t count, ::fast_io::intfpos_opt off, func callback) noexcept
{
	if (off.has_opt) [[unlikely]]
	{
		callback(::fast_io::details::async_make_error(::std::errc::invalid_seek), 0zu);
		return;
	}
	::fast_io::details::win32_ioring_sock_rw_submit<false>(
		sched, wsiob, first, count, timeout, ::std::move(callback));
}

template <win32_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_pwrite_some_bytes_overflow_callback_define(
	::fast_io::win32_ioring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_win32_family_socket_io_observer<family, char_type> wsiob,
	::std::byte const *first, ::std::size_t count, ::fast_io::intfpos_opt off,
	func callback) noexcept
{
	if (off.has_opt) [[unlikely]]
	{
		callback(::fast_io::details::async_make_error(::std::errc::invalid_seek), 0zu);
		return;
	}
	::fast_io::details::win32_ioring_sock_rw_submit<true>(
		sched, wsiob, const_cast<::std::byte *>(first), count, timeout, ::std::move(callback));
}

/*
 * async_close_define: IoRing has no close op in its public enum, so the
 * family's synchronous close runs on a threadpool worker and the
 * completion is ferried through the ring via a no-match cancel-request
 * sqe — the callback still arrives on the pump thread. Since no file op
 * is involved the emulation covers sockets too. The timeout is advisory
 * and ignored — a queued work item always runs.
 */
template <nt_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_close_define(
	::fast_io::win32_ioring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_nt_family_io_observer<family, char_type> ntiob, func callback) noexcept
{
	::fast_io::details::win32_ioring_close_submit(
		sched, sched.native_handle(), ntiob.handle,
		family == nt_family::zw ? 3 : 2, timeout, ::std::move(callback));
}

template <win32_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_close_define(
	::fast_io::win32_ioring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_win32_family_io_observer<family, char_type> wiob, func callback) noexcept
{
	::fast_io::details::win32_ioring_close_submit(
		sched, sched.native_handle(), wiob.handle, 0, timeout, ::std::move(callback));
}

template <win32_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_close_define(
	::fast_io::win32_ioring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_win32_family_socket_io_observer<family, char_type> wsiob,
	func callback) noexcept
{
	::fast_io::details::win32_ioring_close_submit(
		sched, sched.native_handle(), reinterpret_cast<void *>(wsiob.hsocket), 1, timeout,
		::std::move(callback));
}

} // namespace fast_io

#endif
