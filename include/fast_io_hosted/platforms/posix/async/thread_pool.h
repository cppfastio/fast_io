#pragma once
/*
 * POSIX thread-pool async backend — the portable fallback scheduler for
 * platforms without a true completion kernel (everything that is not
 * Linux io_uring or Windows IOCP).
 *
 * A pool of pthread workers runs the synchronous byte operations off the
 * submission thread; each finished op lands on a completion queue that
 * io_async_wait/io_async_peek/io_async_wait_timeout drain and dispatch on
 * the pumping thread — the callback contract is identical to the real
 * backends, so the generic operations layer works unchanged.
 *
 * Honest limitations of a readiness-less fallback:
 * - a blocking syscall (accept on a quiet listener, a read on an empty
 *   pipe) parks a worker thread for its duration;
 * - posix_statx_timestamp_opt is a DURATION despite the name (same
 *   convention as the other backends); the pool can only report
 *   errc::timed_out for work a worker has not picked up yet — a running
 *   syscall cannot be preempted;
 * - destroy the pool only after pumping it to quiescence; completions
 *   queued after the last pump are leaked, matching every async API's
 *   "the scheduler outlives its submissions" rule.
 */

#include <errno.h>
#include <poll.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

namespace fast_io
{

namespace details
{

/* intrusive queue node; run() executes the sync op on a worker and
 * records the result, dispatch() invokes the callback on the pump
 * thread and frees the cookie */
struct posix_thread_pool_node
{
	posix_thread_pool_node *next{};
	void (*run)(posix_thread_pool_node *) noexcept {};
	void (*dispatch)(posix_thread_pool_node *) noexcept {};
};

struct posix_thread_pool_state
{
	pthread_mutex_t mutex{};
	pthread_cond_t work_cond{};
	pthread_cond_t done_cond{};
	posix_thread_pool_node *work_head{};
	posix_thread_pool_node *work_tail{};
	posix_thread_pool_node *done_head{};
	bool stop{};
};

inline void *posix_thread_pool_worker(void *arg) noexcept
{
	auto *st{static_cast<posix_thread_pool_state *>(arg)};
	for (;;)
	{
		::fast_io::noexcept_call(::pthread_mutex_lock, __builtin_addressof(st->mutex));
		while (!st->stop && st->work_head == nullptr)
		{
			::fast_io::noexcept_call(::pthread_cond_wait, __builtin_addressof(st->work_cond),
									 __builtin_addressof(st->mutex));
		}
		if (st->stop && st->work_head == nullptr)
		{
			::fast_io::noexcept_call(::pthread_mutex_unlock, __builtin_addressof(st->mutex));
			return nullptr;
		}
		auto *node{st->work_head};
		st->work_head = node->next;
		if (st->work_head == nullptr)
		{
			st->work_tail = nullptr;
		}
		::fast_io::noexcept_call(::pthread_mutex_unlock, __builtin_addressof(st->mutex));

		node->run(node);

		::fast_io::noexcept_call(::pthread_mutex_lock, __builtin_addressof(st->mutex));
		node->next = st->done_head;
		st->done_head = node;
		::fast_io::noexcept_call(::pthread_cond_signal, __builtin_addressof(st->done_cond));
		::fast_io::noexcept_call(::pthread_mutex_unlock, __builtin_addressof(st->mutex));
	}
}

inline void posix_thread_pool_submit(posix_thread_pool_state *st,
									 posix_thread_pool_node *node) noexcept
{
	::fast_io::noexcept_call(::pthread_mutex_lock, __builtin_addressof(st->mutex));
	node->next = nullptr;
	if (st->work_tail != nullptr)
	{
		st->work_tail->next = node;
	}
	else
	{
		st->work_head = node;
	}
	st->work_tail = node;
	::fast_io::noexcept_call(::pthread_cond_signal, __builtin_addressof(st->work_cond));
	::fast_io::noexcept_call(::pthread_mutex_unlock, __builtin_addressof(st->mutex));
}

/*
nonblocking fds answer EAGAIN/EWOULDBLOCK when they are not ready --
the worker parks on poll() and the op retries rather than reporting
the error; the error path is for real failures. Any readiness bit
(readable, hup, err, nval) wakes the poll -- the retried syscall
itself decides what the actual outcome is.
*/
inline void posix_thread_pool_wait_io(int fd, short events) FAST_IO_HERBCEPTIONS_THROWS
{
	::pollfd pfd{fd, events, 0};
	for (;;)
	{
		int const rc{::fast_io::noexcept_call(::poll, __builtin_addressof(pfd), ::nfds_t{1}, -1)};
		if (rc < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			::fast_io::throw_posix_error();
		}
		return;
	}
}

/* a caught op error is a would-block when its code says so */
inline bool posix_thread_pool_would_block(::std::error const &e) noexcept
{
	if (e.code() == static_cast<::std::size_t>(EAGAIN))
	{
		return true;
	}
#if defined(EWOULDBLOCK) && (!defined(EAGAIN) || EWOULDBLOCK != EAGAIN)
	if (e.code() == static_cast<::std::size_t>(EWOULDBLOCK))
	{
		return true;
	}
#endif
	return false;
}

/* pop one completed node; abs == nullptr waits indefinitely */
inline posix_thread_pool_node *posix_thread_pool_reap(posix_thread_pool_state *st,
													  ::timespec const *abs) noexcept
{
	::fast_io::noexcept_call(::pthread_mutex_lock, __builtin_addressof(st->mutex));
	while (st->done_head == nullptr)
	{
		if (abs == nullptr)
		{
			::fast_io::noexcept_call(::pthread_cond_wait, __builtin_addressof(st->done_cond),
									 __builtin_addressof(st->mutex));
		}
		else
		{
			int const rc{::fast_io::noexcept_call(::pthread_cond_timedwait,
												  __builtin_addressof(st->done_cond),
												  __builtin_addressof(st->mutex), abs)};
			if (rc == ETIMEDOUT)
			{
				::fast_io::noexcept_call(::pthread_mutex_unlock, __builtin_addressof(st->mutex));
				return nullptr;
			}
		}
	}
	auto *node{st->done_head};
	st->done_head = node->next;
	::fast_io::noexcept_call(::pthread_mutex_unlock, __builtin_addressof(st->mutex));
	return node;
}

inline posix_thread_pool_node *posix_thread_pool_reap_try(posix_thread_pool_state *st) noexcept
{
	::fast_io::noexcept_call(::pthread_mutex_lock, __builtin_addressof(st->mutex));
	auto *node{st->done_head};
	if (node != nullptr)
	{
		st->done_head = node->next;
	}
	::fast_io::noexcept_call(::pthread_mutex_unlock, __builtin_addressof(st->mutex));
	return node;
}

/* duration {sec, nsec} -> CLOCK_REALTIME deadline; shared by the pump's
 * timed wait and the per-op timeout */
inline ::timespec posix_thread_pool_deadline(::fast_io::posix_statx_timestamp64 dur) noexcept
{
	::timespec abs{};
	::fast_io::noexcept_call(::clock_gettime, CLOCK_REALTIME, __builtin_addressof(abs));
	abs.tv_sec += static_cast<decltype(abs.tv_sec)>(dur.tv_sec);
	auto const ns{static_cast<long long>(abs.tv_nsec) + static_cast<long long>(dur.tv_nsec)};
	abs.tv_sec += static_cast<decltype(abs.tv_sec)>(ns / 1000000000);
	abs.tv_nsec = static_cast<decltype(abs.tv_nsec)>(ns % 1000000000);
	return abs;
}

inline bool posix_thread_pool_expired(::timespec const &deadline) noexcept
{
	::timespec now{};
	::fast_io::noexcept_call(::clock_gettime, CLOCK_REALTIME, __builtin_addressof(now));
	return now.tv_sec > deadline.tv_sec ||
		   (now.tv_sec == deadline.tv_sec && now.tv_nsec >= deadline.tv_nsec);
}

/*
 * Cookie for both read and write ops — is_write selects direction.
 * run() executes the synchronous decay on a worker (a plain read when
 * the offset is empty, matching the "has_opt == false means use the
 * object's own position" semantics) and records either the error or the
 * transferred count; dispatch() invokes the callback on the pump thread.
 */
template <bool is_write, typename stmtype, typename func>
struct posix_thread_pool_rw_cookie : posix_thread_pool_node
{
	using allocator_type = ::fast_io::native_global_allocator;
	using buf_type = ::std::conditional_t<is_write, ::std::byte const *, ::std::byte *>;
	stmtype stm;
	buf_type buf{};
	::std::size_t count{};
	::fast_io::intfpos_opt off{};
	::timespec deadline{};
	bool has_deadline{};
	func callback;
	::std::cxx_std_error err{};
	::std::size_t transferred{};

	inline posix_thread_pool_rw_cookie(stmtype s, buf_type b, ::std::size_t n,
									   ::fast_io::intfpos_opt o,
									   ::fast_io::posix_statx_timestamp_opt timeout,
									   func &&cb) noexcept
		: stm{s}, buf{b}, count{n}, off{o}, has_deadline{timeout.has_opt},
		  callback{::std::move(cb)}
	{
		if (timeout.has_opt)
		{
			this->deadline = posix_thread_pool_deadline(timeout.opt);
		}
	}
};

template <bool is_write, typename stmtype, typename func>
inline void posix_thread_pool_rw_run(posix_thread_pool_node *p) noexcept
{
	auto *self{static_cast<posix_thread_pool_rw_cookie<is_write, stmtype, func> *>(p)};
	try
	{
		if (self->has_deadline && posix_thread_pool_expired(self->deadline))
		{
			self->err = ::fast_io::details::async_make_error(::std::errc::timed_out);
			return;
		}
		if constexpr (is_write)
		{
			::std::byte const *last;
			if constexpr (::fast_io::operations::decay::defines::bytes_pwritable<stmtype>)
			{
				if (self->off.has_opt)
				{
					last = ::fast_io::operations::decay::pwrite_some_bytes_decay(
						self->stm, self->buf, self->count, self->off.opt);
					self->transferred = static_cast<::std::size_t>(last - self->buf);
					return;
				}
			}
			else if (self->off.has_opt)
			{
				/* stream has no positional writes (sockets, TLS, ...) */
				self->err = ::fast_io::details::async_make_error(::std::errc::invalid_seek);
				return;
			}
			for (;;)
			{
				try
				{
					last = ::fast_io::operations::decay::write_some_bytes_decay(
						self->stm, self->buf, self->count);
					break;
				}
				catch throws(::std::error e)
				{
					/* a nonblocking fd waits on poll() instead of
					   reporting EAGAIN as the result */
					if constexpr (requires { self->stm.fd; })
					{
						if (posix_thread_pool_would_block(e))
						{
							posix_thread_pool_wait_io(self->stm.fd, POLLOUT);
							continue;
						}
					}
					throw throws e;
				}
			}
			self->transferred = static_cast<::std::size_t>(last - self->buf);
		}
		else
		{
			::std::byte *last;
			if constexpr (::fast_io::operations::decay::defines::bytes_preadable<stmtype>)
			{
				if (self->off.has_opt)
				{
					last = ::fast_io::operations::decay::pread_some_bytes_decay(
						self->stm, self->buf, self->count, self->off.opt);
					self->transferred = static_cast<::std::size_t>(last - self->buf);
					return;
				}
			}
			else if (self->off.has_opt)
			{
				self->err = ::fast_io::details::async_make_error(::std::errc::invalid_seek);
				return;
			}
			for (;;)
			{
				try
				{
					last = ::fast_io::operations::decay::read_some_bytes_decay(
						self->stm, self->buf, self->count);
					break;
				}
				catch throws(::std::error e)
				{
					if constexpr (requires { self->stm.fd; })
					{
						if (posix_thread_pool_would_block(e))
						{
							posix_thread_pool_wait_io(self->stm.fd, POLLIN);
							continue;
						}
					}
					throw throws e;
				}
			}
			self->transferred = static_cast<::std::size_t>(last - self->buf);
		}
	}
	catch throws(::std::error e)
	{
		self->err = e.release();
	}
}

template <bool is_write, typename stmtype, typename func>
inline void posix_thread_pool_rw_dispatch(posix_thread_pool_node *p) noexcept
{
	auto *self{static_cast<posix_thread_pool_rw_cookie<is_write, stmtype, func> *>(p)};
	auto callback{::std::move(self->callback)};
	auto err{self->err};
	auto transferred{self->transferred};
	::fast_io::details::async_delete_state(self);
	callback(err, transferred);
}

/* accept cookie: the worker blocks in accept() — a parked worker is the
 * cost of the fallback; the deadline applies only until dequeue */
template <typename listenstmtype, typename func>
struct posix_thread_pool_accept_cookie : posix_thread_pool_node
{
	using allocator_type = ::fast_io::native_global_allocator;
	listenstmtype listenstm;
	::timespec deadline{};
	bool has_deadline{};
	func callback;
	::std::cxx_std_error err{};
	int accepted_fd{-1};

	inline posix_thread_pool_accept_cookie(listenstmtype s,
										   ::fast_io::posix_statx_timestamp_opt timeout,
										   func &&cb) noexcept
		: listenstm{s}, has_deadline{timeout.has_opt}, callback{::std::move(cb)}
	{
		if (timeout.has_opt)
		{
			this->deadline = posix_thread_pool_deadline(timeout.opt);
		}
	}
};

template <typename listenstmtype, typename func>
inline void posix_thread_pool_accept_run(posix_thread_pool_node *p) noexcept
{
	auto *self{static_cast<posix_thread_pool_accept_cookie<listenstmtype, func> *>(p)};
	try
	{
		if (self->has_deadline && posix_thread_pool_expired(self->deadline))
		{
			self->err = ::fast_io::details::async_make_error(::std::errc::timed_out);
			return;
		}
		for (;;)
		{
			try
			{
				self->accepted_fd =
					::fast_io::details::posix_accept_posix_socket_impl(self->listenstm.fd, nullptr,
																	   nullptr);
				break;
			}
			catch throws(::std::error e)
			{
				if (!posix_thread_pool_would_block(e))
				{
					throw throws e;
				}
				/* a no_block listener answers EAGAIN -- park this
				   worker on the fd rather than resubmitting */
				posix_thread_pool_wait_io(self->listenstm.fd, POLLIN);
			}
		}
	}
	catch throws(::std::error e)
	{
		self->err = e.release();
	}
}

template <typename listenstmtype, typename func>
inline void posix_thread_pool_accept_dispatch(posix_thread_pool_node *p) noexcept
{
	auto *self{static_cast<posix_thread_pool_accept_cookie<listenstmtype, func> *>(p)};
	auto callback{::std::move(self->callback)};
	auto err{self->err};
	auto fd{self->accepted_fd};
	::fast_io::details::async_delete_state(self);
	callback(err, fd);
}

/*
 * close cookie: the worker runs the synchronous close — the handle is
 * consumed off the submission thread, which is what "async close" means
 * on this backend. The deadline applies only until dequeue like the
 * other ops, but the fd is closed either way — an expired op still
 * consumes its handle, it merely reports timed_out.
 */
template <typename func>
struct posix_thread_pool_close_cookie : posix_thread_pool_node
{
	using allocator_type = ::fast_io::native_global_allocator;
	int fd{-1};
	::timespec deadline{};
	bool has_deadline{};
	func callback;
	::std::cxx_std_error err{};

	inline posix_thread_pool_close_cookie(int f, ::fast_io::posix_statx_timestamp_opt timeout,
										  func &&cb) noexcept
		: fd{f}, has_deadline{timeout.has_opt}, callback{::std::move(cb)}
	{
		if (timeout.has_opt)
		{
			this->deadline = posix_thread_pool_deadline(timeout.opt);
		}
	}
};

template <typename func>
inline void posix_thread_pool_close_run(posix_thread_pool_node *p) noexcept
{
	auto *self{static_cast<posix_thread_pool_close_cookie<func> *>(p)};
	try
	{
		if (self->has_deadline && posix_thread_pool_expired(self->deadline))
		{
			/* never left the queue — still consume the descriptor */
			::fast_io::details::sys_close(self->fd);
			self->fd = -1;
			self->err = ::fast_io::details::async_make_error(::std::errc::timed_out);
			return;
		}
		::fast_io::details::sys_close_throw_error(self->fd);
	}
	catch throws(::std::error e)
	{
		self->err = e.release();
	}
}

template <typename func>
inline void posix_thread_pool_close_dispatch(posix_thread_pool_node *p) noexcept
{
	auto *self{static_cast<posix_thread_pool_close_cookie<func> *>(p)};
	auto callback{::std::move(self->callback)};
	auto err{self->err};
	::fast_io::details::async_delete_state(self);
	callback(err);
}

/*
 * connect cookie: the worker blocks in connect() — connect is bounded
 * (the kernel's SYN retransmission cap terminates it), so a parked
 * worker is acceptable here, unlike an unbounded read. The peer address
 * is copied into the cookie at submission; the deadline applies only
 * until dequeue like the other pool ops. The socket stays with the
 * caller either way — a failed connect leaves it unconnected.
 */
template <typename func>
struct posix_thread_pool_connect_cookie : posix_thread_pool_node
{
	using allocator_type = ::fast_io::native_global_allocator;
	int fd{-1};
	::fast_io::posix_sockaddr_storage addr{};
	::fast_io::posix_socklen_t addrlen{};
	::timespec deadline{};
	bool has_deadline{};
	func callback;
	::std::cxx_std_error err{};

	inline posix_thread_pool_connect_cookie(int f, void const *a, ::std::size_t alen,
											::fast_io::posix_statx_timestamp_opt timeout,
											func &&cb) noexcept
		: fd{f}, addrlen{static_cast<::fast_io::posix_socklen_t>(alen)},
		  has_deadline{timeout.has_opt}, callback{::std::move(cb)}
	{
		if (alen <= sizeof(this->addr)) [[likely]]
		{
			__builtin_memcpy(__builtin_addressof(this->addr), a, alen);
		}
		if (timeout.has_opt)
		{
			this->deadline = posix_thread_pool_deadline(timeout.opt);
		}
	}
};

template <typename func>
inline void posix_thread_pool_connect_run(posix_thread_pool_node *p) noexcept
{
	auto *self{static_cast<posix_thread_pool_connect_cookie<func> *>(p)};
	try
	{
		if (self->has_deadline && posix_thread_pool_expired(self->deadline))
		{
			self->err = ::fast_io::details::async_make_error(::std::errc::timed_out);
			return;
		}
		if (self->addrlen > sizeof(self->addr)) [[unlikely]]
		{
			self->err = ::fast_io::details::async_make_error(::std::errc::invalid_argument);
			return;
		}
		try
		{
			::fast_io::details::posix_connect_posix_socket_impl(
				self->fd, __builtin_addressof(self->addr), self->addrlen);
		}
		catch throws(::std::error e)
		{
			/* a nonblocking connect reports EINPROGRESS/EALREADY --
			   wait writable then read the verdict out of SO_ERROR */
			if (e.code() != static_cast<::std::size_t>(EINPROGRESS) &&
				e.code() != static_cast<::std::size_t>(EALREADY))
			{
				throw throws e;
			}
			posix_thread_pool_wait_io(self->fd, POLLOUT);
			int soerr{};
			::fast_io::posix_socklen_t n{static_cast<::fast_io::posix_socklen_t>(sizeof(soerr))};
			if (::fast_io::noexcept_call(::getsockopt, self->fd, SOL_SOCKET, SO_ERROR,
										 __builtin_addressof(soerr),
										 __builtin_addressof(n)) != 0)
			{
				::fast_io::throw_posix_error();
			}
			if (soerr != 0)
			{
				::fast_io::throw_posix_error(soerr);
			}
		}
	}
	catch throws(::std::error e)
	{
		self->err = e.release();
	}
}

template <typename func>
inline void posix_thread_pool_connect_dispatch(posix_thread_pool_node *p) noexcept
{
	auto *self{static_cast<posix_thread_pool_connect_cookie<func> *>(p)};
	auto callback{::std::move(self->callback)};
	auto err{self->err};
	::fast_io::details::async_delete_state(self);
	callback(err);
}

/* the pool's rw defines take ANY stream type so arbitrary synchronous
 * streams (hash sinks, memory devices, user types) can run on the pool —
 * but buffered refs carry their own async define; excluding them keeps
 * overload resolution unambiguous */
template <typename T>
inline constexpr bool posix_thread_pool_is_io_buffer_ref{false};
template <typename io_buffer_type>
inline constexpr bool
	posix_thread_pool_is_io_buffer_ref<::fast_io::basic_io_buffer_ref<io_buffer_type>>{true};

} // namespace details

/*
 * Non-owning scheduler observer: the first parameter of every async
 * operation. native_handle() yields the pool state.
 */
class posix_thread_pool_observer
{
public:
	using native_handle_type = ::fast_io::details::posix_thread_pool_state *;
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
 * Owning pool. Workers reference the embedded state, so the object is
 * immovable — construct it where it lives. `workers == 0` picks one
 * worker per online CPU. Shutdown drains queued work: workers finish
 * every queued op before exiting, but their completions still need a
 * pump to be dispatched.
 */
class posix_thread_pool : public posix_thread_pool_observer
{
public:
	details::posix_thread_pool_state storage{};
	/* every socket write through this service must see EPIPE, not a
	 * process kill */
	::fast_io::posix_net_service net_service{};
	pthread_t *workers{};
	::std::size_t worker_count{};

	inline explicit posix_thread_pool(::std::size_t worker_cap) throws
	{
		int ec{::fast_io::noexcept_call(::pthread_mutex_init, __builtin_addressof(this->storage.mutex),
										nullptr)};
		if (ec != 0)
		{
			::fast_io::throw_posix_error(ec);
		}
		if ((ec = ::fast_io::noexcept_call(::pthread_cond_init,
										   __builtin_addressof(this->storage.work_cond), nullptr)) != 0)
		{
			::fast_io::noexcept_call(::pthread_mutex_destroy, __builtin_addressof(this->storage.mutex));
			::fast_io::throw_posix_error(ec);
		}
		if ((ec = ::fast_io::noexcept_call(::pthread_cond_init,
										   __builtin_addressof(this->storage.done_cond), nullptr)) != 0)
		{
			::fast_io::noexcept_call(::pthread_cond_destroy, __builtin_addressof(this->storage.work_cond));
			::fast_io::noexcept_call(::pthread_mutex_destroy, __builtin_addressof(this->storage.mutex));
			::fast_io::throw_posix_error(ec);
		}
		this->state = __builtin_addressof(this->storage);
		try
		{
			if (worker_cap == 0)
			{
				auto const online{::fast_io::noexcept_call(::sysconf, _SC_NPROCESSORS_ONLN)};
				worker_cap = online > 0 ? static_cast<::std::size_t>(online) : 4zu;
			}
			this->workers = ::fast_io::native_typed_global_allocator<pthread_t>::allocate(worker_cap);
			for (; this->worker_count < worker_cap; ++this->worker_count)
			{
				int const create_ec{::fast_io::noexcept_call(::pthread_create,
															 this->workers + this->worker_count,
															 nullptr,
															 &::fast_io::details::posix_thread_pool_worker,
															 __builtin_addressof(this->storage))};
				if (create_ec != 0)
				{
					::fast_io::throw_posix_error(create_ec);
				}
			}
		}
		catch throws(::std::error e)
		{
			this->shutdown();
			::fast_io::noexcept_call(::pthread_cond_destroy, __builtin_addressof(this->storage.done_cond));
			::fast_io::noexcept_call(::pthread_cond_destroy, __builtin_addressof(this->storage.work_cond));
			::fast_io::noexcept_call(::pthread_mutex_destroy, __builtin_addressof(this->storage.mutex));
			throw throws e.release();
		}
	}

	inline explicit posix_thread_pool(::fast_io::io_async_t) throws
		: posix_thread_pool(0)
	{
	}

	posix_thread_pool(posix_thread_pool const &) = delete;
	posix_thread_pool &operator=(posix_thread_pool const &) = delete;
	/* workers hold &storage — the pool is immovable */
	posix_thread_pool(posix_thread_pool &&) = delete;
	posix_thread_pool &operator=(posix_thread_pool &&) = delete;

	inline void shutdown() noexcept
	{
		if (this->workers == nullptr)
		{
			return;
		}
		::fast_io::noexcept_call(::pthread_mutex_lock, __builtin_addressof(this->storage.mutex));
		this->storage.stop = true;
		::fast_io::noexcept_call(::pthread_cond_broadcast, __builtin_addressof(this->storage.work_cond));
		::fast_io::noexcept_call(::pthread_mutex_unlock, __builtin_addressof(this->storage.mutex));
		for (::std::size_t i{}; i != this->worker_count; ++i)
		{
			::fast_io::noexcept_call(::pthread_join, this->workers[i], nullptr);
		}
		::fast_io::native_typed_global_allocator<pthread_t>::deallocate_n(this->workers,
																		  this->worker_count);
		this->workers = nullptr;
		this->worker_count = 0;
		this->state = nullptr;
	}

	inline ~posix_thread_pool()
	{
		this->shutdown();
		::fast_io::noexcept_call(::pthread_cond_destroy, __builtin_addressof(this->storage.done_cond));
		::fast_io::noexcept_call(::pthread_cond_destroy, __builtin_addressof(this->storage.work_cond));
		::fast_io::noexcept_call(::pthread_mutex_destroy, __builtin_addressof(this->storage.mutex));
	}
};

/*
 * async_scheduler_ref_define: the owning pool decays to its
 * trivially-copyable observer for the decay layer.
 */
inline constexpr posix_thread_pool_observer
async_scheduler_ref_define(posix_thread_pool &pool) noexcept
{
	return {pool.native_handle()};
}

/*
 * Event pump: pop one completion and dispatch it to its callback on the
 * caller's thread. io_async_wait blocks; io_async_peek returns false when
 * nothing is queued; io_async_wait_timeout takes a duration and returns
 * false when it elapses.
 */
inline void io_async_wait(posix_thread_pool_observer sched) throws
{
	auto *node{::fast_io::details::posix_thread_pool_reap(sched.native_handle(), nullptr)};
	node->dispatch(node);
}

inline bool io_async_peek(posix_thread_pool_observer sched) throws
{
	auto *node{::fast_io::details::posix_thread_pool_reap_try(sched.native_handle())};
	if (node == nullptr)
	{
		return false;
	}
	node->dispatch(node);
	return true;
}

inline bool io_async_wait_timeout(posix_thread_pool_observer sched,
								  ::fast_io::posix_statx_timestamp64 timeout) throws
{
	::timespec abs{::fast_io::details::posix_thread_pool_deadline(timeout)};
	auto *node{::fast_io::details::posix_thread_pool_reap(sched.native_handle(),
														  __builtin_addressof(abs))};
	if (node == nullptr)
	{
		return false;
	}
	node->dispatch(node);
	return true;
}

/*
 * async_pread_some_bytes_underflow_callback_define /
 * async_pwrite_some_bytes_overflow_callback_define: the required low-level
 * defines; everything else (scatter, transmit, buffered underflow/flush)
 * is built on these generically. The noexcept submission wraps a
 * throws-try so allocation failure arrives through the callback.
 */
template <typename instmtype, typename func>
	requires(::fast_io::operations::decay::defines::async_bytes_completion_callback<
				 ::std::remove_cvref_t<func>> &&
			 !::fast_io::details::posix_thread_pool_is_io_buffer_ref<
				 ::std::remove_cvref_t<instmtype>>)
inline void async_pread_some_bytes_underflow_callback_define(
	posix_thread_pool_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	instmtype instm, ::std::byte *first, ::std::size_t count, ::fast_io::intfpos_opt off,
	func callback) noexcept
{
	using cookie_type =
		::fast_io::details::posix_thread_pool_rw_cookie<false, instmtype, func>;
	try
	{
		auto *cookie{::fast_io::details::async_new_state_plain<cookie_type>(
			sched, instm, first, count, off, timeout, ::std::move(callback))};
		cookie->run = &::fast_io::details::posix_thread_pool_rw_run<false, instmtype, func>;
		cookie->dispatch =
			&::fast_io::details::posix_thread_pool_rw_dispatch<false, instmtype, func>;
		::fast_io::details::posix_thread_pool_submit(sched.native_handle(), cookie);
	}
	catch throws(::std::error e)
	{
		callback(e.release(), 0zu);
	}
}

template <typename outstmtype, typename func>
	requires(::fast_io::operations::decay::defines::async_bytes_completion_callback<
				 ::std::remove_cvref_t<func>> &&
			 !::fast_io::details::posix_thread_pool_is_io_buffer_ref<
				 ::std::remove_cvref_t<outstmtype>>)
inline void async_pwrite_some_bytes_overflow_callback_define(
	posix_thread_pool_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	outstmtype outstm, ::std::byte const *first, ::std::size_t count,
	::fast_io::intfpos_opt off, func callback) noexcept
{
	using cookie_type =
		::fast_io::details::posix_thread_pool_rw_cookie<true, outstmtype, func>;
	try
	{
		auto *cookie{::fast_io::details::async_new_state_plain<cookie_type>(
			sched, outstm, first, count, off, timeout, ::std::move(callback))};
		cookie->run = &::fast_io::details::posix_thread_pool_rw_run<true, outstmtype, func>;
		cookie->dispatch =
			&::fast_io::details::posix_thread_pool_rw_dispatch<true, outstmtype, func>;
		::fast_io::details::posix_thread_pool_submit(sched.native_handle(), cookie);
	}
	catch throws(::std::error e)
	{
		callback(e.release(), 0zu);
	}
}

/*
 * async_close_define: a worker runs the synchronous close on the fd —
 * genuinely asynchronous (the syscall parks a worker, not the
 * submission thread), and the completion rides the done queue like
 * every other op.
 */
template <::fast_io::posix_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_close_define(
	posix_thread_pool_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_posix_family_io_observer<family, char_type> piob, func callback) noexcept
{
	using cookie_type = ::fast_io::details::posix_thread_pool_close_cookie<func>;
	try
	{
		auto *cookie{::fast_io::details::async_new_state_plain<cookie_type>(
			sched, piob.fd, timeout, ::std::move(callback))};
		cookie->run = &::fast_io::details::posix_thread_pool_close_run<func>;
		cookie->dispatch = &::fast_io::details::posix_thread_pool_close_dispatch<func>;
		::fast_io::details::posix_thread_pool_submit(sched.native_handle(), cookie);
	}
	catch throws(::std::error e)
	{
		/* submission failure: the op still owns the fd — close it
		 * inline so it cannot leak */
		::fast_io::details::sys_close(piob.fd);
		callback(e.release());
	}
}

/*
 * async_accept_callback_define: the worker blocks in accept(); `mode`'s
 * no_block bit needs nothing on posix, same as the io_uring backend.
 */
template <::fast_io::posix_family family, ::std::integral char_type, typename func>
	requires ::std::is_nothrow_invocable_v<func, ::std::cxx_std_error, int>
inline void async_accept_callback_define(
	posix_thread_pool_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_posix_family_io_observer<family, char_type> listenstm,
	::fast_io::open_mode, func callback) noexcept
{
	using listenstmtype = ::fast_io::basic_posix_family_io_observer<family, char_type>;
	using cookie_type =
		::fast_io::details::posix_thread_pool_accept_cookie<listenstmtype, func>;
	try
	{
		auto *cookie{::fast_io::details::async_new_state_plain<cookie_type>(
			sched, listenstm, timeout, ::std::move(callback))};
		cookie->run = &::fast_io::details::posix_thread_pool_accept_run<listenstmtype, func>;
		cookie->dispatch =
			&::fast_io::details::posix_thread_pool_accept_dispatch<listenstmtype, func>;
		::fast_io::details::posix_thread_pool_submit(sched.native_handle(), cookie);
	}
	catch throws(::std::error e)
	{
		callback(e.release(), -1);
	}
}

/*
 * async_connect_define: a worker runs the blocking connect() — bounded
 * by the kernel's SYN timeout, so parking a worker is acceptable. The
 * socket is not consumed; a failed connect leaves it unconnected.
 */
template <::fast_io::posix_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_connect_define(
	posix_thread_pool_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_posix_family_io_observer<family, char_type> sockstm, void const *addr,
	::std::size_t addrlen, func callback) noexcept
{
	using cookie_type = ::fast_io::details::posix_thread_pool_connect_cookie<func>;
	try
	{
		auto *cookie{::fast_io::details::async_new_state_plain<cookie_type>(
			sched, sockstm.fd, addr, addrlen, timeout, ::std::move(callback))};
		cookie->run = &::fast_io::details::posix_thread_pool_connect_run<func>;
		cookie->dispatch = &::fast_io::details::posix_thread_pool_connect_dispatch<func>;
		::fast_io::details::posix_thread_pool_submit(sched.native_handle(), cookie);
	}
	catch throws(::std::error e)
	{
		callback(e.release());
	}
}

/* io_async selects the pool on POSIX platforms with no real backend */
#if !defined(__linux__)
template <::std::integral char_type>
inline constexpr ::fast_io::io_type_t<posix_thread_pool_observer> async_scheduler_type(
	::fast_io::basic_posix_family_io_observer<::fast_io::posix_family::api, char_type>) noexcept
{
	return {};
}

using io_async_observer = posix_thread_pool_observer;
using io_async_scheduler = posix_thread_pool;
#endif

} // namespace fast_io
