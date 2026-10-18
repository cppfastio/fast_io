#pragma once
/*
 * Win32 thread-pool async backend — the OS-managed default process pool
 * (QueueUserWorkItem) runs synchronous byte operations off the
 * submission thread; each finished op lands on a completion queue
 * guarded by a critical section and signalled through a manual-reset
 * event that io_async_wait/io_async_peek/io_async_wait_timeout drain on
 * the pumping thread.
 *
 * Honest limitations, same as the posix pool:
 * - a blocking syscall (a read on an empty socket) parks an OS worker
 *   thread for its duration;
 * - a timeout applies only until the work item is dequeued — a running
 *   syscall cannot be preempted;
 * - destroy the pool only after pumping it to quiescence; completions
 *   queued after the last pump are leaked, matching every async API's
 *   "the scheduler outlives its submissions" rule.
 */

namespace fast_io
{

namespace details
{

/* intrusive queue node; run() executes the sync op on a pool worker and
 * records the result, dispatch() invokes the callback on the pump
 * thread and frees the cookie */
struct win32_thread_pool_node
{
	win32_thread_pool_node *next{};
	void (*run)(win32_thread_pool_node *) noexcept {};
	void (*dispatch)(win32_thread_pool_node *) noexcept {};
};

struct win32_thread_pool_state
{
	::fast_io::win32::nt::rtl_critical_section mutex;
	void *done_event{}; /* manual-reset: raised while done_head is nonempty */
	win32_thread_pool_node *done_head{};
};

inline ::std::uint_least32_t __stdcall win32_thread_pool_worker(void *ctx) noexcept
{
	auto *node{static_cast<win32_thread_pool_node *>(ctx)};
	node->run(node);
	/* the completion lands on the shared queue; the event wakes whichever
	   pump thread is sleeping in io_async_wait */
	auto *st{reinterpret_cast<win32_thread_pool_state *>(node->next)};
	::fast_io::win32::EnterCriticalSection(__builtin_addressof(st->mutex));
	node->next = st->done_head;
	st->done_head = node;
	::fast_io::win32::LeaveCriticalSection(__builtin_addressof(st->mutex));
	::fast_io::win32::SetEvent(st->done_event);
	return 0;
}

/* st is smuggled through node->next at submission time */
inline void win32_thread_pool_submit(win32_thread_pool_state *st,
									 win32_thread_pool_node *node) FAST_IO_HERBCEPTIONS_THROWS
{
	node->next = reinterpret_cast<win32_thread_pool_node *>(st);
	if (::fast_io::win32::QueueUserWorkItem(&win32_thread_pool_worker, node, 0) == 0)
	{
		::fast_io::throw_win32_error();
	}
}

/* pop one completed node; milliseconds < 0 waits indefinitely */
inline win32_thread_pool_node *win32_thread_pool_reap(win32_thread_pool_state *st,
													  ::std::int_least64_t milliseconds) noexcept
{
	for (;;)
	{
		::fast_io::win32::EnterCriticalSection(__builtin_addressof(st->mutex));
		if (auto *node{st->done_head}; node != nullptr)
		{
			st->done_head = node->next;
			if (st->done_head == nullptr)
			{
				::fast_io::win32::ResetEvent(st->done_event);
			}
			::fast_io::win32::LeaveCriticalSection(__builtin_addressof(st->mutex));
			return node;
		}
		::fast_io::win32::ResetEvent(st->done_event);
		::fast_io::win32::LeaveCriticalSection(__builtin_addressof(st->mutex));
		if (milliseconds >= 0)
		{
			auto const waited{::fast_io::win32::WaitForSingleObject(
				st->done_event, static_cast<::std::uint_least32_t>(milliseconds))};
			if (waited != 0u) /* anything but WAIT_OBJECT_0 ends the wait */
			{
				return nullptr;
			}
			milliseconds = -1; /* event may be signalled again below */
			continue;
		}
		::fast_io::win32::WaitForSingleObject(st->done_event, 0xFFFFFFFFu);
	}
}

/* duration {sec, nsec} -> the deadline used for "still queued" checks and
   the pump's timed wait; QueryUnbiasedInterruptTime ticks at 100ns */
inline ::std::uint_least64_t win32_thread_pool_now() noexcept
{
	::std::uint_least64_t t{};
	::fast_io::win32::QueryUnbiasedInterruptTime(__builtin_addressof(t));
	return t;
}

inline ::std::uint_least64_t
win32_thread_pool_deadline(::fast_io::posix_statx_timestamp64 dur) noexcept
{
	return win32_thread_pool_now() + static_cast<::std::uint_least64_t>(dur.tv_sec) * 10000000u +
		   static_cast<::std::uint_least64_t>(dur.tv_nsec) / 100u;
}

inline bool win32_thread_pool_expired(::std::uint_least64_t deadline) noexcept
{
	return deadline != 0 && win32_thread_pool_now() >= deadline;
}

/*
 * Cookie for both read and write ops — is_write selects direction.
 * run() executes the synchronous decay on a worker (a plain read when
 * the offset is empty) and records either the error or the transferred
 * count; dispatch() invokes the callback on the pump thread.
 */
template <bool is_write, typename stmtype, typename func>
struct win32_thread_pool_rw_cookie : win32_thread_pool_node
{
	using allocator_type = ::fast_io::native_global_allocator;
	using buf_type = ::std::conditional_t<is_write, ::std::byte const *, ::std::byte *>;
	stmtype stm;
	buf_type buf{};
	::std::size_t count{};
	::fast_io::intfpos_opt off{};
	::std::uint_least64_t deadline{};
	func callback;
	::std::cxx_std_error err{};
	::std::size_t transferred{};

	inline win32_thread_pool_rw_cookie(stmtype s, buf_type b, ::std::size_t n,
									   ::fast_io::intfpos_opt o,
									   ::fast_io::posix_statx_timestamp_opt timeout,
									   func &&cb) noexcept
		: stm{s}, buf{b}, count{n}, off{o}, callback{::std::move(cb)}
	{
		if (timeout.has_opt)
		{
			this->deadline = win32_thread_pool_deadline(timeout.opt);
		}
	}
};

template <bool is_write, typename stmtype, typename func>
inline void win32_thread_pool_rw_run(win32_thread_pool_node *p) noexcept
{
	auto *self{static_cast<win32_thread_pool_rw_cookie<is_write, stmtype, func> *>(p)};
	try
	{
		if (win32_thread_pool_expired(self->deadline))
		{
			self->err = ::fast_io::details::async_make_error(::std::errc::timed_out);
			return;
		}
		if constexpr (is_write)
		{
			::std::size_t last;
			if constexpr (::fast_io::operations::decay::defines::bytes_pwritable<stmtype>)
			{
				if (self->off.has_opt)
				{
					last = ::fast_io::operations::decay::pwrite_some_bytes_decay(
						self->stm, self->buf, self->count, self->off.opt);
				}
				else
				{
					last = ::fast_io::operations::decay::write_some_bytes_decay(
						self->stm, self->buf, self->count);
				}
			}
			else
			{
				/* sockets have no positional write -- the sequential op
				   covers it, matching the "off on a stream" contract */
				last = ::fast_io::operations::decay::write_some_bytes_decay(
					self->stm, self->buf, self->count);
			}
			self->transferred = last;
		}
		else
		{
			::std::size_t last;
			if constexpr (::fast_io::operations::decay::defines::bytes_preadable<stmtype>)
			{
				if (self->off.has_opt)
				{
					last = ::fast_io::operations::decay::pread_some_bytes_decay(
						self->stm, self->buf, self->count, self->off.opt);
				}
				else
				{
					last = ::fast_io::operations::decay::read_some_bytes_decay(
						self->stm, self->buf, self->count);
				}
			}
			else
			{
				last = ::fast_io::operations::decay::read_some_bytes_decay(
					self->stm, self->buf, self->count);
			}
			self->transferred = last;
		}
	}
	catch throws(::std::error e)
	{
		self->err = e.release();
	}
}

template <bool is_write, typename stmtype, typename func>
inline void win32_thread_pool_rw_dispatch(win32_thread_pool_node *p) noexcept
{
	auto *self{static_cast<win32_thread_pool_rw_cookie<is_write, stmtype, func> *>(p)};
	auto callback{::std::move(self->callback)};
	auto err{self->err};
	auto transferred{self->transferred};
	::fast_io::details::async_delete_state(self);
	callback(err, transferred);
}

/*
 * The close kind dispatch mirrors win32_iocp's win32_close_handle_now;
 * duplicated here because the pool exists on families where IOCP does
 * not (ansi_9x has no completion ports).
 */
inline ::std::cxx_std_error win32_thread_pool_close_handle_now(void *handle,
															   int kind) noexcept
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
		if (!::fast_io::win32::CloseHandle(handle))
		{
			return ::fast_io::details::async_make_error(
				static_cast<::fast_io::freestanding::win32_errc>(
					static_cast<::std::uint_least32_t>(::fast_io::win32::GetLastError())));
		}
		break;
	}
	return {};
}

/*
 * close cookie: the worker runs the synchronous close — the handle is
 * consumed off the submission thread, which is what "async close" means
 * on this backend. The deadline applies only until dequeue like the
 * other ops, but the handle is closed either way — an expired op still
 * consumes its handle, it merely reports timed_out.
 */
template <typename func>
struct win32_thread_pool_close_cookie : win32_thread_pool_node
{
	using allocator_type = ::fast_io::native_global_allocator;
	void *handle{};
	int kind{};
	::std::uint_least64_t deadline{};
	func callback;
	::std::cxx_std_error err{};

	inline win32_thread_pool_close_cookie(void *h, int k,
										  ::fast_io::posix_statx_timestamp_opt timeout,
										  func &&cb) noexcept
		: handle{h}, kind{k}, callback{::std::move(cb)}
	{
		if (timeout.has_opt)
		{
			this->deadline = win32_thread_pool_deadline(timeout.opt);
		}
	}
};

template <typename func>
inline void win32_thread_pool_close_run(win32_thread_pool_node *p) noexcept
{
	auto *self{static_cast<win32_thread_pool_close_cookie<func> *>(p)};
	if (win32_thread_pool_expired(self->deadline))
	{
		(void)win32_thread_pool_close_handle_now(self->handle, self->kind);
		self->err = ::fast_io::details::async_make_error(::std::errc::timed_out);
		return;
	}
	self->err = win32_thread_pool_close_handle_now(self->handle, self->kind);
}

template <typename func>
inline void win32_thread_pool_close_dispatch(win32_thread_pool_node *p) noexcept
{
	auto *self{static_cast<win32_thread_pool_close_cookie<func> *>(p)};
	auto callback{::std::move(self->callback)};
	auto err{self->err};
	::fast_io::details::async_delete_state(self);
	callback(err);
}


} // namespace details

/*
 * Non-owning scheduler observer: the first parameter of every async
 * operation. native_handle() yields the pool state.
 */
class win32_thread_pool_observer
{
public:
	using native_handle_type = ::fast_io::details::win32_thread_pool_state *;
	using allocator_type = ::fast_io::native_global_allocator;
	native_handle_type state{};
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return state;
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{state};
		state = nullptr;
		return temp;
	}
	inline constexpr explicit operator bool() const noexcept
	{
		return state != nullptr;
	}
};

/*
 * Owning pool: a completion queue signalled by a manual-reset event.
 * Workers come from the OS default process pool, so there is no thread
 * count to configure; submissions queue onto the shared QueueUserWorkItem
 * pool. The object is immovable — submitted cookies reference the
 * embedded state. Pump to quiescence before destruction.
 */
class win32_thread_pool
{
public:
	using native_handle_type = ::fast_io::details::win32_thread_pool_state *;
	::fast_io::details::win32_thread_pool_state storage{};

	inline win32_thread_pool() FAST_IO_HERBCEPTIONS_THROWS
	{
		::fast_io::win32::InitializeCriticalSection(__builtin_addressof(storage.mutex));
		this->storage.done_event =
			::fast_io::win32::CreateEventW(nullptr, 1 /* manual reset */, 0, nullptr);
		if (this->storage.done_event == nullptr) [[unlikely]]
		{
			::fast_io::win32::DeleteCriticalSection(__builtin_addressof(storage.mutex));
			::fast_io::throw_win32_error();
		}
	}

	inline native_handle_type native_handle() noexcept
	{
		return __builtin_addressof(this->storage);
	}
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return const_cast<native_handle_type>(__builtin_addressof(this->storage));
	}

	win32_thread_pool(win32_thread_pool const &) = delete;
	win32_thread_pool &operator=(win32_thread_pool const &) = delete;
	win32_thread_pool(win32_thread_pool &&) = delete;
	win32_thread_pool &operator=(win32_thread_pool &&) = delete;

	inline ~win32_thread_pool()
	{
		if (this->storage.done_event != nullptr)
		{
			::fast_io::win32::CloseHandle(this->storage.done_event);
		}
		::fast_io::win32::DeleteCriticalSection(__builtin_addressof(storage.mutex));
	}
};

/*
 * async_scheduler_ref_define: the owning pool decays to its
 * trivially-copyable observer for the decay layer.
 */
inline constexpr win32_thread_pool_observer
async_scheduler_ref_define(win32_thread_pool &pool) noexcept
{
	return {pool.native_handle()};
}

/*
 * Event pump: pop one completion and dispatch it to its callback on the
 * caller's thread. io_async_wait blocks; io_async_peek returns false when
 * nothing is queued; io_async_wait_timeout takes a duration and returns
 * false when it elapses.
 */
inline void io_async_wait(win32_thread_pool_observer sched) FAST_IO_HERBCEPTIONS_THROWS
{
	auto *node{::fast_io::details::win32_thread_pool_reap(sched.native_handle(), -1)};
	node->dispatch(node);
}

inline bool io_async_peek(win32_thread_pool_observer sched) FAST_IO_HERBCEPTIONS_THROWS
{
	auto *node{::fast_io::details::win32_thread_pool_reap(sched.native_handle(), 0)};
	if (node == nullptr)
	{
		return false;
	}
	node->dispatch(node);
	return true;
}

inline bool io_async_wait_timeout(win32_thread_pool_observer sched,
								  ::fast_io::posix_statx_timestamp64 timeout) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::uint_least64_t const ms{static_cast<::std::uint_least64_t>(timeout.tv_sec) * 1000u +
								   static_cast<::std::uint_least64_t>(timeout.tv_nsec) / 1000000u};
	auto *node{::fast_io::details::win32_thread_pool_reap(
		sched.native_handle(),
		ms > static_cast<::std::uint_least64_t>(0xFFFFFFFFu - 1u) ? -1ll
																  : static_cast<::std::int_least64_t>(ms))};
	if (node == nullptr)
	{
		return false;
	}
	node->dispatch(node);
	return true;
}

/*
 * async_pread/pwrite defines: a worker runs the synchronous decay — the
 * required low-level pair; everything else (scatter, transmit, buffered
 * underflow/flush) is built on these generically. The noexcept
 * submission wraps a throws-try so allocation failure arrives through
 * the callback.
 */
template <typename instmtype, typename func>
	requires(::fast_io::operations::decay::defines::async_bytes_completion_callback<
			 ::std::remove_cvref_t<func>>)
inline void async_pread_some_bytes_underflow_callback_define(
	win32_thread_pool_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	instmtype instm, ::std::byte *first, ::std::size_t count, ::fast_io::intfpos_opt off,
	func callback) noexcept
{
	using cookie_type =
		::fast_io::details::win32_thread_pool_rw_cookie<false, instmtype, func>;
	try
	{
		auto *cookie{::fast_io::details::async_new_state_plain<cookie_type>(
			sched, instm, first, count, off, timeout, ::std::move(callback))};
		cookie->run =
			&::fast_io::details::win32_thread_pool_rw_run<false, instmtype, func>;
		cookie->dispatch =
			&::fast_io::details::win32_thread_pool_rw_dispatch<false, instmtype, func>;
		::fast_io::details::win32_thread_pool_submit(sched.native_handle(), cookie);
	}
	catch throws(::std::error e)
	{
		callback(e.release(), 0zu);
	}
}

template <typename outstmtype, typename func>
	requires(::fast_io::operations::decay::defines::async_bytes_completion_callback<
			 ::std::remove_cvref_t<func>>)
inline void async_pwrite_some_bytes_overflow_callback_define(
	win32_thread_pool_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	outstmtype outstm, ::std::byte const *first, ::std::size_t count,
	::fast_io::intfpos_opt off, func callback) noexcept
{
	using cookie_type =
		::fast_io::details::win32_thread_pool_rw_cookie<true, outstmtype, func>;
	try
	{
		auto *cookie{::fast_io::details::async_new_state_plain<cookie_type>(
			sched, outstm, first, count, off, timeout, ::std::move(callback))};
		cookie->run =
			&::fast_io::details::win32_thread_pool_rw_run<true, outstmtype, func>;
		cookie->dispatch =
			&::fast_io::details::win32_thread_pool_rw_dispatch<true, outstmtype, func>;
		::fast_io::details::win32_thread_pool_submit(sched.native_handle(), cookie);
	}
	catch throws(::std::error e)
	{
		callback(e.release(), 0zu);
	}
}

template <typename func>
inline void win32_thread_pool_close_submit(win32_thread_pool_observer sched,
										   void *handle, int kind,
										   ::fast_io::posix_statx_timestamp_opt timeout,
										   func &&callback) noexcept
{
	using fcb = ::std::remove_cvref_t<func>;
	using cookie_type =
		::fast_io::details::win32_thread_pool_close_cookie<fcb>;
	try
	{
		auto *cookie{::fast_io::details::async_new_state_plain<cookie_type>(
			sched, handle, kind, timeout, ::std::move(callback))};
		cookie->run = &::fast_io::details::win32_thread_pool_close_run<fcb>;
		cookie->dispatch = &::fast_io::details::win32_thread_pool_close_dispatch<fcb>;
		::fast_io::details::win32_thread_pool_submit(sched.native_handle(), cookie);
	}
	catch throws(::std::error e)
	{
		(void)::fast_io::details::win32_thread_pool_close_handle_now(handle, kind);
		callback(e.release());
	}
}

/*
 * async_close_define: a worker consumes the handle off the submission
 * thread. Submission failure still consumes the handle inline — the op
 * took ownership when it was handed the released handle.
 */
/*
 * async_close_define: a worker consumes the handle off the submission
 * thread — Windows has no kernel close op, exactly like the iocp
 * backend. kind selects the family's synchronous closer.
 */
template <::fast_io::nt_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_close_define(
	win32_thread_pool_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_nt_family_io_observer<family, char_type> ntiob, func callback) noexcept
{
	win32_thread_pool_close_submit(
		sched, ntiob.handle, family == ::fast_io::nt_family::zw ? 3 : 2, timeout,
		::std::move(callback));
}

template <::fast_io::win32_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_close_define(
	win32_thread_pool_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_win32_family_io_observer<family, char_type> wiob, func callback) noexcept
{
	win32_thread_pool_close_submit(sched, wiob.handle, 0, timeout,
								   ::std::move(callback));
}

template <::fast_io::win32_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_close_define(
	win32_thread_pool_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_win32_family_socket_io_observer<family, char_type> wsiob,
	func callback) noexcept
{
	win32_thread_pool_close_submit(
		sched, reinterpret_cast<void *>(wsiob.hsocket), 1, timeout, ::std::move(callback));
}

} // namespace fast_io
