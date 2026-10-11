#pragma once
/*
 * io_uring backend for fast_io's generic async layer, implemented on top
 * of fast_io's own system_call<> wrappers. The liburing-equivalent ring
 * machinery lives in linux_liburing.h; this header provides the scheduler
 * observer, the event pump and the async_*_define entry points the
 * generic operations layer calls.
 *
 * Backend contract (operations/refs/async.h):
 * - async_pread_some_bytes_underflow_callback_define /
 *   async_pwrite_some_bytes_overflow_callback_define: required.
 * - async_scatter_p{read,write}_some_bytes_*_callback_define: native
 *   READV/WRITEV fast paths.
 * - async_transmit_some_bytes_overflow_underflow_callback_define: SPLICE
 *   fast path with an automatic bounce-buffer fallback when the kernel
 *   reports the fd pair cannot be spliced.
 * All defines are noexcept; every error — including submission
 * failures — is delivered through the callback as ::std::cxx_std_error.
 *
 * Timeouts use io_uring_op_link_timeout: the op sqe is submitted with
 * IOSQE_IO_LINK followed by an adjacent LINK_TIMEOUT sqe, so a timeout
 * reservation never splits across a flush. When the timer fires the op
 * CQE reports ECANCELED and the timeout CQE reports ETIME; the user
 * callback receives errc::timed_out (ETIMEDOUT) — ETIME itself is
 * kernel-ABI detail and is never surfaced.
 */


namespace fast_io
{

/*
 * Non-owning scheduler observer: the first parameter of every async
 * operation. native_handle() yields the mmap'd ring state.
 */
class linux_io_uring_observer
{
public:
	using native_handle_type = ::fast_io::liburing::io_uring_ring_state *;
	/* scheduler-provided allocator for the async state objects;
	 * schedulers without one fall back to native_global_allocator */
	using allocator_type = ::fast_io::native_global_allocator;
	native_handle_type ring{};
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
 * Owning io_uring object. The ring state is embedded (no heap
 * allocation); linux_io_uring_observer provides the non-owning view the
 * async defines take.
 */
class linux_io_uring : public linux_io_uring_observer
{
public:
	liburing::io_uring_ring_state storage{};

	inline linux_io_uring() noexcept
	{
		this->ring = __builtin_addressof(this->storage);
	}
	inline explicit linux_io_uring(::fast_io::native_interface_t, ::std::uint_least32_t entries,
								   ::std::uint_least32_t flags) FAST_IO_HERBCEPTIONS_THROWS
		: linux_io_uring()
	{
		liburing::details::io_uring_queue_init_impl(this->storage, entries, flags);
	}
	inline explicit linux_io_uring(::fast_io::io_async_t) FAST_IO_HERBCEPTIONS_THROWS
		: linux_io_uring(::fast_io::native_interface, 64, 0)
	{
	}
	linux_io_uring(linux_io_uring const &) = delete;
	linux_io_uring &operator=(linux_io_uring const &) = delete;
	inline linux_io_uring(linux_io_uring &&other) noexcept
		: linux_io_uring()
	{
		this->storage = other.storage;
		other.storage = {};
	}
	inline linux_io_uring &operator=(linux_io_uring &&other) noexcept
	{
		if (this == __builtin_addressof(other)) [[unlikely]]
		{
			return *this;
		}
		liburing::details::io_uring_queue_exit_impl(this->storage);
		this->storage = other.storage;
		other.storage = {};
		return *this;
	}
	inline ~linux_io_uring()
	{
		liburing::details::io_uring_queue_exit_impl(this->storage);
	}
};

/*
 * async_scheduler_ref_define: the owning ring decays to its
 * trivially-copyable observer for the decay layer. The observer itself
 * needs no define — async_scheduler_ref copies it straight through.
 */
inline constexpr linux_io_uring_observer async_scheduler_ref_define(linux_io_uring &ring) noexcept
{
	return {ring.native_handle()};
}

} // namespace fast_io

namespace fast_io::liburing
{

namespace details
{
/*
 * Internal user_data ABI: sqe->user_data points at a state object (or a
 * sub-object of it) whose FIRST member is an io_uring_invoke_func; the
 * event loop calls it once per cqe with (bytes, 0) on success and
 * (0, errno) on failure. Not part of the public API.
 */
using io_uring_invoke_func = void (*)(void *, ::std::size_t, int) noexcept;
/* the per-op finisher both CQEs funnel into once every expected cqe has
 * arrived */
using io_uring_deliver_func = void (*)(void *) noexcept;

/* ETIME is the kernel's ABI name for "the armed timeout expired"; the C++
 * standard deprecated ETIME, so the only thing that ever reaches a user
 * callback is errc::timed_out (ETIMEDOUT) */
inline constexpr int io_uring_etime_expired{
#if defined(ETIME)
	ETIME
#else
	62 /* asm-generic ETIME */
#endif
};

/*
 * Sub-object of an op cookie that owns the linked-timeout cqe. Its first
 * member is the dispatch invoke, so sqe->user_data can point at the block
 * directly; owner/deliver route back to the cookie. `pending` is the
 * number of cqes still expected for the whole operation (2 with an armed
 * timeout, 1 otherwise) and lives here so both invoke sites can reach it.
 */
struct io_uring_timeout_link_block
{
	io_uring_invoke_func invoke;
	io_uring_deliver_func deliver;
	void *owner;
	::std::uint_least32_t pending;
	bool fired;
};

inline void io_uring_timeout_link_invoke(void *self, ::std::size_t, int errn) noexcept
{
	auto *link{static_cast<io_uring_timeout_link_block *>(self)};
	if (errn == io_uring_etime_expired)
	{
		link->fired = true;
	}
	if (--link->pending == 0)
	{
		link->deliver(link->owner);
	}
}

/* Build the user-facing error for a finished op: a fired timeout beats
 * the op's own result (the kernel reports it as ECANCELED). */
inline ::std::cxx_std_error io_uring_cqe_error(bool timeout_fired, int errn) noexcept
{
	if (timeout_fired)
	{
		return ::fast_io::details::async_make_error(::std::errc::timed_out);
	}
	if (errn != 0)
	{
		return {::std::error_domain<::std::errc>::domain(), static_cast<::std::size_t>(errn)};
	}
	return {};
}

/* allocate + construct a T cookie through the scheduler's allocator —
 * like details::async_new_state but without a leading scheduler ctor
 * argument (the first member of every cookie is its invoke function) */
template <typename T, typename scheduler, typename... Args>
inline T *io_uring_new_state(scheduler sched, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		::fast_io::typed_generic_allocator_adapter<::fast_io::details::async_scheduler_allocator_t<scheduler>, T>::throws_on_allocation_failure ||
		!::std::is_nothrow_constructible_v<T, Args &&...>)
{
	using guard_type =
		::fast_io::details::async_state_ptr<T, ::fast_io::details::async_scheduler_allocator_t<scheduler>>;
	if constexpr (guard_type::alloc_with_status)
	{
		guard_type guard{sched.alloc_handle, 1};
		new (guard.ptr) T(::std::forward<Args>(args)...);
		guard.ptr->alloc_handle = sched.alloc_handle;
		return guard.release();
	}
	else
	{
		guard_type guard{1};
		new (guard.ptr) T(::std::forward<Args>(args)...);
		return guard.release();
	}
}

/* RAII: frees the cookie when the define exits before the sqes were
 * committed. release() MUST run before the tail is published — once the
 * kernel can see an sqe its cqe will eventually reach the cookie. */
template <typename T>
class io_uring_submit_guard
{
public:
	T *cookie;
	inline explicit io_uring_submit_guard(T *c) noexcept
		: cookie{c}
	{
	}
	io_uring_submit_guard(io_uring_submit_guard const &) = delete;
	io_uring_submit_guard &operator=(io_uring_submit_guard const &) = delete;
	inline ~io_uring_submit_guard() noexcept
	{
		if (cookie != nullptr)
		{
			::fast_io::details::async_delete_state(cookie);
		}
	}
	inline constexpr void release() noexcept
	{
		cookie = nullptr;
	}
};

/*
 * Commit prepared sqes to the kernel. Once the tail is published the ops
 * are irrevocably queued, so an io_uring_enter failure must NOT free the
 * cookie nor be delivered as the op's error — the entries stay in the sq
 * and ride the next pump (every wait path flushes first).
 */
inline void io_uring_commit(::fast_io::liburing::io_uring_ring_state &ring) noexcept
{
	try
	{
		io_uring_submit(ring);
	}
	catch throws(::std::error)
	{
	}
}

/* shared tail of every submission: fill the timeout-link block and, when
 * a timeout is armed, the adjacent LINK_TIMEOUT sqe. Requires the slots
 * to be reserved already. */
template <typename cookie_type>
inline void io_uring_arm_timeout(::fast_io::liburing::io_uring_ring_state &ring, cookie_type *cookie,
								 io_uring_sqe *sqe, io_uring_deliver_func deliver,
								 ::fast_io::posix_statx_timestamp_opt timeout) noexcept
{
	cookie->tlink.invoke = __builtin_addressof(io_uring_timeout_link_invoke);
	cookie->tlink.deliver = deliver;
	cookie->tlink.owner = cookie;
	cookie->tlink.pending = 1;
	cookie->tlink.fired = false;
	io_uring_sqe_set_data(sqe, cookie);
	if (timeout.has_opt)
	{
		cookie->ts = {static_cast<::std::int_least64_t>(timeout.opt.tv_sec),
					  static_cast<::std::int_least64_t>(timeout.opt.tv_nsec)};
		sqe->flags = static_cast<::std::uint_least8_t>(sqe->flags | io_uring_sqe_io_link);
		io_uring_sqe *tsqe{io_uring_get_sqe(ring)};
		io_uring_prep_link_timeout(tsqe, __builtin_addressof(cookie->ts), 0);
		io_uring_sqe_set_data(tsqe, __builtin_addressof(cookie->tlink));
		cookie->tlink.pending = 2;
	}
}

/* sqe->off: empty opt = use (and advance) the file's own position */
inline constexpr ::std::uint_least64_t io_uring_use_file_position{static_cast<::std::uint_least64_t>(-1)};

inline constexpr ::std::uint_least64_t io_uring_off_or_current(::fast_io::intfpos_opt off) noexcept
{
	return off.has_opt ? static_cast<::std::uint_least64_t>(off.opt) : io_uring_use_file_position;
}

/* cqe->res is int32: clamp requests so a huge count cannot wrap */
inline constexpr ::std::uint_least32_t io_uring_clamp_count(::std::size_t count) noexcept
{
	constexpr ::std::size_t mx{static_cast<::std::size_t>(::std::numeric_limits<::std::int32_t>::max())};
	return static_cast<::std::uint_least32_t>(count < mx ? count : mx);
}

inline void io_uring_dispatch_cqe(linux_io_uring_observer ring, io_uring_cqe *cqe) noexcept
{
	if (cqe == nullptr)
	{
		return;
	}
	void *data{io_uring_cqe_get_data(cqe)};
	::std::int_least32_t res{cqe->res};
	io_uring_cqe_seen(*ring.ring, cqe);
	if (data == nullptr) [[unlikely]]
	{
		return;
	}
	auto invoke{*static_cast<io_uring_invoke_func *>(data)};
	if (res < 0)
	{
		invoke(data, 0, -res);
	}
	else
	{
		invoke(data, static_cast<::std::size_t>(res), 0);
	}
}

/* ======================= pread / pwrite ======================= */

/*
 * Cookie for one pending positional read or write; both ops share the
 * layout — invoke stays the first member because io_uring_dispatch_cqe
 * dereferences user_data as a pointer to it. `ts` holds the armed
 * timeout's timespec: the kernel reads it through the sqe's address
 * field, potentially after submission returns (SQPOLL), so it must live
 * in the cookie and never on the define's stack.
 */
template <typename alloc_type, typename T>
struct io_uring_rw_cookie
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	io_uring_invoke_func invoke;
	io_uring_timeout_link_block tlink;
	::std::size_t transferred{};
	int errn{};
	::fast_io::liburing::io_uring_timespec ts{};
	T callback;
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

template <typename alloc_type, typename T>
inline void io_uring_rw_deliver(void *self) noexcept
{
	using cookie_type = io_uring_rw_cookie<alloc_type, T>;
	auto *cookie{static_cast<cookie_type *>(self)};
	auto callback{::std::move(cookie->callback)};
	::std::size_t const transferred{cookie->transferred};
	::std::cxx_std_error err{io_uring_cqe_error(cookie->tlink.fired, cookie->errn)};
	::fast_io::details::async_delete_state(cookie);
	callback(err, transferred);
}

template <typename alloc_type, typename T>
inline void io_uring_rw_invoke(void *self, ::std::size_t transferred, int errn) noexcept
{
	using cookie_type = io_uring_rw_cookie<alloc_type, T>;
	auto *cookie{static_cast<cookie_type *>(self)};
	cookie->transferred = transferred;
	cookie->errn = errn;
	if (--cookie->tlink.pending == 0)
	{
		io_uring_rw_deliver<alloc_type, T>(cookie);
	}
}

template <typename sched_type, typename T>
inline void io_uring_rw_submit(sched_type sched, ::fast_io::liburing::io_uring_ring_state &ring,
							   ::fast_io::liburing::io_uring_op op, int fd, void const *addr,
							   ::std::size_t count, ::fast_io::intfpos_opt off,
							   ::fast_io::posix_statx_timestamp_opt timeout, T callback) noexcept
{
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<sched_type>;
	using cookie_type = io_uring_rw_cookie<alloc_type, T>;
	try
	{
		::fast_io::liburing::details::io_uring_submit_guard<cookie_type> guard{
			io_uring_new_state<cookie_type>(sched, io_uring_invoke_func{},
											io_uring_timeout_link_block{}, 0zu, 0,
											::fast_io::liburing::io_uring_timespec{},
											::std::move(callback))};
		guard.cookie->invoke = io_uring_rw_invoke<alloc_type, T>;
		io_uring_reserve_sqes(ring, timeout.has_opt ? 2 : 1);
		io_uring_sqe *sqe{io_uring_get_sqe(ring)};
		io_uring_prep_rw(op, sqe, fd, addr, io_uring_clamp_count(count),
						 io_uring_off_or_current(off));
		io_uring_arm_timeout(ring, guard.cookie, sqe, io_uring_rw_deliver<alloc_type, T>, timeout);
		guard.release();
		io_uring_commit(ring);
	}
	catch throws(::std::error e)
	{
		callback(e.release(), 0zu);
	}
}

/* ======================= scatter readv / writev ======================= */

/* The kernel caps vectored ios at UIO_MAXIOV; anything beyond is a legal
 * short transfer under `some` semantics rather than an error */
inline constexpr ::std::size_t io_uring_max_iov{1024};

template <typename alloc_type, typename T>
struct io_uring_scatter_cookie
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	io_uring_invoke_func invoke;
	io_uring_timeout_link_block tlink;
	::std::size_t transferred{};
	int errn{};
	::fast_io::liburing::io_uring_timespec ts{};
	::fast_io::io_scatter_t const *scatters{};
	::std::size_t n{};
	T callback;
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

template <typename alloc_type, typename T>
inline void io_uring_scatter_deliver(void *self) noexcept
{
	using cookie_type = io_uring_scatter_cookie<alloc_type, T>;
	auto *cookie{static_cast<cookie_type *>(self)};
	auto callback{::std::move(cookie->callback)};
	::fast_io::io_scatter_status_t status{::fast_io::scatter_size_to_status(
		cookie->transferred, cookie->scatters, cookie->n)};
	::std::cxx_std_error err{io_uring_cqe_error(cookie->tlink.fired, cookie->errn)};
	::fast_io::details::async_delete_state(cookie);
	callback(err, status);
}

template <typename alloc_type, typename T>
inline void io_uring_scatter_invoke(void *self, ::std::size_t transferred, int errn) noexcept
{
	using cookie_type = io_uring_scatter_cookie<alloc_type, T>;
	auto *cookie{static_cast<cookie_type *>(self)};
	cookie->transferred = transferred;
	cookie->errn = errn;
	if (--cookie->tlink.pending == 0)
	{
		io_uring_scatter_deliver<alloc_type, T>(cookie);
	}
}

template <typename sched_type, typename T>
inline void io_uring_scatter_submit(sched_type sched, ::fast_io::liburing::io_uring_ring_state &ring,
									::fast_io::liburing::io_uring_op op, int fd,
									::fast_io::io_scatter_t const *scatters, ::std::size_t n,
									::fast_io::intfpos_opt off,
									::fast_io::posix_statx_timestamp_opt timeout, T callback) noexcept
{
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<sched_type>;
	using cookie_type = io_uring_scatter_cookie<alloc_type, T>;
	::std::size_t const neff{n < io_uring_max_iov ? n : io_uring_max_iov};
	if (neff == 0)
	{
		callback(::std::cxx_std_error{}, ::fast_io::io_scatter_status_t{0, 0});
		return;
	}
	try
	{
		::fast_io::liburing::details::io_uring_submit_guard<cookie_type> guard{
			io_uring_new_state<cookie_type>(sched, io_uring_invoke_func{},
											io_uring_timeout_link_block{}, 0zu, 0,
											::fast_io::liburing::io_uring_timespec{}, scatters, neff,
											::std::move(callback))};
		guard.cookie->invoke = io_uring_scatter_invoke<alloc_type, T>;
		io_uring_reserve_sqes(ring, timeout.has_opt ? 2 : 1);
		io_uring_sqe *sqe{io_uring_get_sqe(ring)};
		io_uring_prep_rw(op, sqe, fd, scatters, static_cast<::std::uint_least32_t>(neff),
						 io_uring_off_or_current(off));
		io_uring_arm_timeout(ring, guard.cookie, sqe, io_uring_scatter_deliver<alloc_type, T>,
							 timeout);
		guard.release();
		io_uring_commit(ring);
	}
	catch throws(::std::error e)
	{
		callback(e.release(), ::fast_io::io_scatter_status_t{});
	}
}

/* ======================= splice transmit ======================= */

/*
 * splice(2) — the only fd-to-fd transfer io_uring exposes — needs a pipe
 * on at least one end. Rather than paying two fstat syscalls up front,
 * the sqe is submitted optimistically; when the cqe comes back with one
 * of the "this method cannot do it" errnos (the same set the synchronous
 * posix transmit dispatch uses) the cookie drops into the generic
 * bounce-buffer engine instead of reporting the failure.
 */
inline constexpr bool io_uring_splice_unsupported(int errn) noexcept
{
	return errn == ENOSYS || errn == EINVAL || errn == EXDEV || errn == EOPNOTSUPP || errn == EPERM ||
		   errn == EBADF;
}

template <typename sched_type, typename outstmtype, typename instmtype, typename alloc_type,
		  typename T>
struct io_uring_transmit_cookie
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	io_uring_invoke_func invoke;
	io_uring_timeout_link_block tlink;
	::std::size_t transferred{};
	int errn{};
	::fast_io::liburing::io_uring_timespec ts{};
	sched_type sched;
	outstmtype outstm;
	::fast_io::intfpos_opt off_out;
	instmtype instm;
	::fast_io::intfpos_opt off_in;
	::fast_io::size_t_opt bound;
	::fast_io::posix_statx_timestamp_opt timeout;
	T callback;
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

template <typename sched_type, typename outstmtype, typename instmtype, typename alloc_type,
		  typename T>
inline void io_uring_transmit_deliver(void *self) noexcept
{
	using cookie_type = io_uring_transmit_cookie<sched_type, outstmtype, instmtype, alloc_type, T>;
	auto *cookie{static_cast<cookie_type *>(self)};
	::std::size_t const transferred{cookie->transferred};
	if (cookie->tlink.fired)
	{
		auto callback{::std::move(cookie->callback)};
		::fast_io::details::async_delete_state(cookie);
		callback(::fast_io::details::async_make_error(::std::errc::timed_out), transferred);
		return;
	}

	if (cookie->errn == 0 || !io_uring_splice_unsupported(cookie->errn))
	{
		auto callback{::std::move(cookie->callback)};
		::std::cxx_std_error err{io_uring_cqe_error(false, cookie->errn)};
		::fast_io::details::async_delete_state(cookie);
		callback(err, transferred);
		return;
	}
	/* splice cannot serve this fd pair (no pipe endpoint, EXDEV, ...):
	 * rebuild as the generic bounce-buffer emulation. The emulation runs
	 * the scalar pread/pwrite defines, which exist for the same stream
	 * types, and reports through the same callback contract. */
	auto callback{::std::move(cookie->callback)};
	using state_type =
		::fast_io::details::async_transmit_bytes_state<false, sched_type, outstmtype, instmtype,
													   alloc_type, T>;
	sched_type const sched{cookie->sched};
	outstmtype const outstm{cookie->outstm};
	instmtype const instm{cookie->instm};
	::fast_io::intfpos_opt const off_out{cookie->off_out};
	::fast_io::intfpos_opt const off_in{cookie->off_in};
	::std::size_t const remaining{
		cookie->bound.has_opt ? cookie->bound.opt : ::std::numeric_limits<::std::size_t>::max()};
	::fast_io::posix_statx_timestamp_opt const timeout{cookie->timeout};
	::fast_io::details::async_delete_state(cookie);
	try
	{
		::fast_io::details::async_transmit_bytes_read_round(
			::fast_io::details::async_new_state<state_type>(sched, outstm, off_out, instm, off_in,
															remaining, timeout, ::std::move(callback),
															0zu, 0zu, 0zu));
	}
	catch throws(::std::error e)
	{
		callback(e.release(), transferred);
	}
}

template <typename sched_type, typename outstmtype, typename instmtype, typename alloc_type,
		  typename T>
inline void io_uring_transmit_invoke(void *self, ::std::size_t transferred, int errn) noexcept
{
	using cookie_type = io_uring_transmit_cookie<sched_type, outstmtype, instmtype, alloc_type, T>;
	auto *cookie{static_cast<cookie_type *>(self)};
	cookie->transferred = transferred;
	cookie->errn = errn;
	if (--cookie->tlink.pending == 0)
	{
		io_uring_transmit_deliver<sched_type, outstmtype, instmtype, alloc_type, T>(cookie);
	}
}

/* ======================= close ======================= */

/*
 * Cookie for one pending IORING_OP_CLOSE: the sqe carries the fd and
 * nothing else. `fd` is retained so a cancelled close — the linked
 * timeout fired and the kernel reports the op ECANCELED without having
 * run it — can still be closed synchronously: the "handle is always
 * consumed" contract holds either way.
 */
template <typename alloc_type, typename T>
struct io_uring_close_cookie
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	io_uring_invoke_func invoke;
	io_uring_timeout_link_block tlink;
	int errn{};
	int fd{-1};
	::fast_io::liburing::io_uring_timespec ts{};
	T callback;
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

template <typename alloc_type, typename T>
inline void io_uring_close_deliver(void *self) noexcept
{
	using cookie_type = io_uring_close_cookie<alloc_type, T>;
	auto *cookie{static_cast<cookie_type *>(self)};
	auto callback{::std::move(cookie->callback)};
	::std::cxx_std_error err{io_uring_cqe_error(cookie->tlink.fired, cookie->errn)};
	int const fd{cookie->fd};
	bool const cancelled{cookie->tlink.fired};
	::fast_io::details::async_delete_state(cookie);
	if (cancelled) [[unlikely]]
	{
		/* the linked timeout aborted the close before it ran — the
		 * descriptor is still open, so finish it inline */
		::fast_io::details::sys_close(fd);
	}
	callback(err);
}

template <typename alloc_type, typename T>
inline void io_uring_close_invoke(void *self, ::std::size_t, int errn) noexcept
{
	using cookie_type = io_uring_close_cookie<alloc_type, T>;
	auto *cookie{static_cast<cookie_type *>(self)};
	cookie->errn = errn;
	if (--cookie->tlink.pending == 0)
	{
		io_uring_close_deliver<alloc_type, T>(cookie);
	}
}

template <typename sched_type, typename T>
inline void io_uring_close_submit(sched_type sched, ::fast_io::liburing::io_uring_ring_state &ring,
								  int fd, ::fast_io::posix_statx_timestamp_opt timeout,
								  T callback) noexcept
{
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<sched_type>;
	using cookie_type = io_uring_close_cookie<alloc_type, T>;
	try
	{
		::fast_io::liburing::details::io_uring_submit_guard<cookie_type> guard{
			io_uring_new_state<cookie_type>(sched, io_uring_invoke_func{},
											io_uring_timeout_link_block{}, 0, fd,
											::fast_io::liburing::io_uring_timespec{},
											::std::move(callback))};
		guard.cookie->invoke = io_uring_close_invoke<alloc_type, T>;
		io_uring_reserve_sqes(ring, timeout.has_opt ? 2 : 1);
		io_uring_sqe *sqe{io_uring_get_sqe(ring)};
		io_uring_prep_rw(io_uring_op_close, sqe, fd, nullptr, 0, 0);
		io_uring_arm_timeout(ring, guard.cookie, sqe, io_uring_close_deliver<alloc_type, T>,
							 timeout);
		guard.release();
		io_uring_commit(ring);
	}
	catch throws(::std::error e)
	{
		/* submission failure: the op still owns the handle — close it
		 * inline so it cannot leak */
		::fast_io::details::sys_close(fd);
		callback(e.release());
	}
}

/* ======================= accept ======================= */

/*
 * Cookie for one pending accept: the sqe is IORING_OP_ACCEPT; the cqe's
 * res is the accepted fd on success. tlink/ts ride the same
 * linked-timeout machinery as the rw cookies.
 */
template <typename alloc_type, typename T>
struct io_uring_accept_cookie
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	io_uring_invoke_func invoke;
	io_uring_timeout_link_block tlink;
	int accepted{};
	int errn{};
	::fast_io::liburing::io_uring_timespec ts{};
	T callback;
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

template <typename alloc_type, typename T>
inline void io_uring_accept_deliver(void *self) noexcept
{
	using cookie_type = io_uring_accept_cookie<alloc_type, T>;
	auto *cookie{static_cast<cookie_type *>(self)};
	auto callback{::std::move(cookie->callback)};
	int const accepted{cookie->accepted};
	::std::cxx_std_error err{io_uring_cqe_error(cookie->tlink.fired, cookie->errn)};
	::fast_io::details::async_delete_state(cookie);
	callback(err, accepted);
}

template <typename alloc_type, typename T>
inline void io_uring_accept_invoke(void *self, ::std::size_t res, int errn) noexcept
{
	using cookie_type = io_uring_accept_cookie<alloc_type, T>;
	auto *cookie{static_cast<cookie_type *>(self)};
	cookie->accepted = static_cast<int>(res);
	cookie->errn = errn;
	if (--cookie->tlink.pending == 0)
	{
		io_uring_accept_deliver<alloc_type, T>(cookie);
	}
}

template <typename sched_type, typename T>
inline void io_uring_accept_submit(sched_type sched, ::fast_io::liburing::io_uring_ring_state &ring,
								   int fd, ::fast_io::open_mode m,
								   ::fast_io::posix_statx_timestamp_opt timeout,
								   T callback) noexcept
{
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<sched_type>;
	using cookie_type = io_uring_accept_cookie<alloc_type, T>;
	try
	{
		::fast_io::liburing::details::io_uring_submit_guard<cookie_type> guard{
			io_uring_new_state<cookie_type>(sched, io_uring_invoke_func{},
											io_uring_timeout_link_block{}, 0, 0,
											::fast_io::liburing::io_uring_timespec{},
											::std::move(callback))};
		guard.cookie->invoke = io_uring_accept_invoke<alloc_type, T>;
		io_uring_reserve_sqes(ring, timeout.has_opt ? 2 : 1);
		io_uring_sqe *sqe{io_uring_get_sqe(ring)};
		/* accepted sockets used with io_uring must not be nonblocking —
		 * a would-block op reports EAGAIN in its cqe; the mode's
		 * no_block bit is an async-capability marker here, only the
		 * CLOEXEC choice survives */
		::fast_io::liburing::io_uring_prep_accept(
			sqe, fd, nullptr, nullptr,
			static_cast<::std::uint_least32_t>(
				::fast_io::to_posix_sock_open_mode(m & ~::fast_io::open_mode::no_block)));
		io_uring_arm_timeout(ring, guard.cookie, sqe,
							 io_uring_accept_deliver<alloc_type, T>, timeout);
		guard.release();
		io_uring_commit(ring);
	}
	catch throws(::std::error e)
	{
		callback(e.release(), 0);
	}
}

/* ======================= connect ======================= */

/*
 * Cookie for one pending connect: the sqe is IORING_OP_CONNECT; the cqe's
 * res is 0 on success, -errno on failure. addr is stored BY VALUE — the
 * kernel dereferences it when the op runs, which may be long after
 * submission returned. tlink/ts ride the linked-timeout machinery; a
 * fired deadline cancels the connect and reports timed_out, leaving the
 * socket in the platform's unconnected state (it is not owned by the
 * op).
 */
template <typename alloc_type, typename T>
struct io_uring_connect_cookie
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	io_uring_invoke_func invoke;
	io_uring_timeout_link_block tlink;
	int errn{};
	::fast_io::liburing::io_uring_timespec ts{};
	::fast_io::posix_sockaddr_storage addr{};
	T callback;
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
												   ::fast_io::details::empty>
		alloc_handle{};
};

template <typename alloc_type, typename T>
inline void io_uring_connect_deliver(void *self) noexcept
{
	using cookie_type = io_uring_connect_cookie<alloc_type, T>;
	auto *cookie{static_cast<cookie_type *>(self)};
	auto callback{::std::move(cookie->callback)};
	::std::cxx_std_error err{io_uring_cqe_error(cookie->tlink.fired, cookie->errn)};
	::fast_io::details::async_delete_state(cookie);
	callback(err);
}

template <typename alloc_type, typename T>
inline void io_uring_connect_invoke(void *self, ::std::size_t, int errn) noexcept
{
	using cookie_type = io_uring_connect_cookie<alloc_type, T>;
	auto *cookie{static_cast<cookie_type *>(self)};
	cookie->errn = errn;
	if (--cookie->tlink.pending == 0)
	{
		io_uring_connect_deliver<alloc_type, T>(cookie);
	}
}

template <typename sched_type, typename T>
inline void io_uring_connect_submit(sched_type sched, ::fast_io::liburing::io_uring_ring_state &ring,
									int fd, void const *addr, ::std::size_t addrlen,
									::fast_io::posix_statx_timestamp_opt timeout,
									T callback) noexcept
{
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<sched_type>;
	using cookie_type = io_uring_connect_cookie<alloc_type, T>;
	if (addr == nullptr || addrlen > sizeof(::fast_io::posix_sockaddr_storage)) [[unlikely]]
	{
		callback(::fast_io::details::async_make_error(::std::errc::invalid_argument));
		return;
	}
	try
	{
		::fast_io::liburing::details::io_uring_submit_guard<cookie_type> guard{
			io_uring_new_state<cookie_type>(sched, io_uring_invoke_func{},
											io_uring_timeout_link_block{}, 0,
											::fast_io::liburing::io_uring_timespec{},
											::fast_io::posix_sockaddr_storage{},
											::std::move(callback))};
		guard.cookie->invoke = io_uring_connect_invoke<alloc_type, T>;
		__builtin_memcpy(__builtin_addressof(guard.cookie->addr), addr, addrlen);
		io_uring_reserve_sqes(ring, timeout.has_opt ? 2 : 1);
		io_uring_sqe *sqe{io_uring_get_sqe(ring)};
		::fast_io::liburing::io_uring_prep_connect(
			sqe, fd, __builtin_addressof(guard.cookie->addr),
			static_cast<::std::uint_least32_t>(addrlen));
		io_uring_arm_timeout(ring, guard.cookie, sqe,
							 io_uring_connect_deliver<alloc_type, T>, timeout);
		guard.release();
		io_uring_commit(ring);
	}
	catch throws(::std::error e)
	{
		callback(e.release());
	}
}

} // namespace details
} // namespace fast_io::liburing

namespace fast_io
{

/* ======================= backend defines ======================= */

/*
 * async_pread_some_bytes_underflow_callback_define: required low-level
 * define. cb is invoked once as cb(::std::cxx_std_error, ::std::size_t)
 * noexcept with the bytes read this round; a fired timeout is reported as
 * errc::timed_out. piob is the stream's input-stream ref (an fd observer
 * for posix streams).
 */
template <::fast_io::posix_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_pread_some_bytes_underflow_callback_define(
	::fast_io::linux_io_uring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_posix_family_io_observer<family, char_type> piob, ::std::byte *first,
	::std::size_t count, ::fast_io::intfpos_opt off, func callback) noexcept
{
	::fast_io::liburing::details::io_uring_rw_submit(sched, *sched.ring, ::fast_io::liburing::io_uring_op_read,
													 piob.fd, first, count, off, timeout,
													 ::std::move(callback));
}

template <::fast_io::posix_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_pwrite_some_bytes_overflow_callback_define(
	::fast_io::linux_io_uring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_posix_family_io_observer<family, char_type> piob, ::std::byte const *first,
	::std::size_t count, ::fast_io::intfpos_opt off, func callback) noexcept
{
	::fast_io::liburing::details::io_uring_rw_submit(sched, *sched.ring, ::fast_io::liburing::io_uring_op_write,
													 piob.fd, first, count, off, timeout,
													 ::std::move(callback));
}

/*
 * Native vectored fast paths: one IORING_OP_READV/WRITEV per call.
 * cb is invoked once as cb(::std::cxx_std_error, io_scatter_status_t).
 */
template <::fast_io::posix_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_scatter_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_scatter_pread_some_bytes_underflow_callback_define(
	::fast_io::linux_io_uring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_posix_family_io_observer<family, char_type> piob,
	::fast_io::io_scatter_t const *scatters, ::std::size_t n, ::fast_io::intfpos_opt off,
	func callback) noexcept
{
	::fast_io::liburing::details::io_uring_scatter_submit(sched, *sched.ring,
														  ::fast_io::liburing::io_uring_op_readv, piob.fd,
														  scatters, n, off, timeout,
														  ::std::move(callback));
}

template <::fast_io::posix_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_scatter_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_scatter_pwrite_some_bytes_overflow_callback_define(
	::fast_io::linux_io_uring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_posix_family_io_observer<family, char_type> piob,
	::fast_io::io_scatter_t const *scatters, ::std::size_t n, ::fast_io::intfpos_opt off,
	func callback) noexcept
{
	::fast_io::liburing::details::io_uring_scatter_submit(sched, *sched.ring,
														  ::fast_io::liburing::io_uring_op_writev, piob.fd,
														  scatters, n, off, timeout,
														  ::std::move(callback));
}

/*
 * Native transmit fast path via IORING_OP_SPLICE. splice needs a pipe
 * endpoint; fd pairs it cannot serve fall back to the generic
 * bounce-buffer engine automatically — the callback contract is
 * unchanged: cb(::std::cxx_std_error, ::std::size_t) once.
 */
template <::fast_io::posix_family family_out, ::fast_io::posix_family family_in,
		  ::std::integral char_type_out, ::std::integral char_type_in, typename func>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_transmit_some_bytes_overflow_underflow_callback_define(
	::fast_io::linux_io_uring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_posix_family_io_observer<family_out, char_type_out> outstm,
	::fast_io::intfpos_opt off_out,
	::fast_io::basic_posix_family_io_observer<family_in, char_type_in> instm,
	::fast_io::intfpos_opt off_in, ::fast_io::size_t_opt bound, func callback) noexcept
{
	using outstmtype =
		::fast_io::basic_posix_family_io_observer<family_out, char_type_out>;
	using instmtype =
		::fast_io::basic_posix_family_io_observer<family_in, char_type_in>;
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<::fast_io::linux_io_uring_observer>;
	using cookie_type =
		::fast_io::liburing::details::io_uring_transmit_cookie<::fast_io::linux_io_uring_observer,
															   outstmtype, instmtype, alloc_type,
															   ::std::remove_cvref_t<func>>;
	if (bound.has_opt && bound.opt == 0)
	{
		callback(::std::cxx_std_error{}, 0zu);
		return;
	}
	try
	{
		::fast_io::liburing::details::io_uring_submit_guard<cookie_type> guard{
			::fast_io::liburing::details::io_uring_new_state<cookie_type>(
				sched, ::fast_io::liburing::details::io_uring_invoke_func{},
				::fast_io::liburing::details::io_uring_timeout_link_block{}, 0zu, 0,
				::fast_io::liburing::io_uring_timespec{}, sched, outstm, off_out, instm, off_in,
				bound, timeout, ::std::move(callback))};
		guard.cookie->invoke = ::fast_io::liburing::details::io_uring_transmit_invoke<
			::fast_io::linux_io_uring_observer, outstmtype, instmtype, alloc_type,
			::std::remove_cvref_t<func>>;
		::std::size_t want{bound.has_opt ? bound.opt
										 : ::std::numeric_limits<::std::size_t>::max()};
		::fast_io::liburing::details::io_uring_reserve_sqes(*sched.ring, timeout.has_opt ? 2 : 1);
		::fast_io::liburing::io_uring_sqe *sqe{
			::fast_io::liburing::io_uring_get_sqe(*sched.ring)};
		::fast_io::liburing::io_uring_prep_splice(
			sqe, instm.fd,
			off_in.has_opt ? static_cast<::std::int_least64_t>(off_in.opt)
						   : static_cast<::std::int_least64_t>(-1),
			outstm.fd,
			off_out.has_opt ? static_cast<::std::int_least64_t>(off_out.opt)
							: static_cast<::std::int_least64_t>(-1),
			::fast_io::liburing::details::io_uring_clamp_count(want), 0);
		::fast_io::liburing::details::io_uring_arm_timeout(
			*sched.ring, guard.cookie, sqe,
			::fast_io::liburing::details::io_uring_transmit_deliver<
				::fast_io::linux_io_uring_observer, outstmtype, instmtype, alloc_type,
				::std::remove_cvref_t<func>>,
			timeout);
		guard.release();
		::fast_io::liburing::details::io_uring_commit(*sched.ring);
	}
	catch throws(::std::error e)
	{
		callback(e.release(), 0zu);
	}
}

/* async_accept_file_type for posix observers is shared by every posix
 * backend — it lives in posix_netmode.h next to the accept impl. */

/*
 * async_accept_callback_define: submits IORING_OP_ACCEPT on the
 * listener; the callback receives the accepted fd through
 * cb(::std::cxx_std_error, int). mode's no_block bit is deliberately not
 * mapped to SOCK_NONBLOCK — io_uring ops on a nonblocking socket report
 * EAGAIN from would-block completions; posix async sockets need nothing
 * from it. inherit drops SOCK_CLOEXEC.
 */
template <::fast_io::posix_family family, ::std::integral char_type, typename func>
	requires ::std::is_nothrow_invocable_v<func, ::std::cxx_std_error, int>
inline void async_accept_callback_define(
	::fast_io::linux_io_uring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_posix_family_io_observer<family, char_type> instm,
	::fast_io::open_mode m, func callback) noexcept
{
	::fast_io::liburing::details::io_uring_accept_submit(
		sched, *sched.ring, instm.fd, m, timeout, ::std::move(callback));
}

/*
 * async_connect_define: submits IORING_OP_CONNECT on the socket; the
 * callback receives cb(::std::cxx_std_error) — domain == nullptr means
 * connected. The peer address is copied into the op's state at
 * submission, and the linked-timeout machinery cancels an overlong
 * connect (reported as timed_out). The socket is not consumed — a
 * failed connect leaves it unconnected for the caller to close.
 */
template <::fast_io::posix_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_connect_define(
	::fast_io::linux_io_uring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_posix_family_io_observer<family, char_type> sockstm, void const *addr,
	::std::size_t addrlen, func callback) noexcept
{
	::fast_io::liburing::details::io_uring_connect_submit(
		sched, *sched.ring, sockstm.fd, addr, addrlen, timeout, ::std::move(callback));
}

/*
 * async_close_define: submits IORING_OP_CLOSE on the stream's fd. The fd
 * rides the sqe and is closed by the kernel on completion — including a
 * synchronous close() fallback when the submission itself fails or the
 * linked timeout cancels the op, so the descriptor is always consumed.
 * cb is invoked once as cb(::std::cxx_std_error).
 */
template <::fast_io::posix_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_close_define(
	::fast_io::linux_io_uring_observer sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_posix_family_io_observer<family, char_type> piob, func callback) noexcept
{
	::fast_io::liburing::details::io_uring_close_submit(
		sched, *sched.ring, piob.fd, timeout, ::std::move(callback));
}

/*
 * Event pump: reap one completion and dispatch it to its cookie.
 * io_async_wait blocks; io_async_peek returns false when nothing is
 * ready; io_async_wait_timeout returns false when the deadline elapsed.
 */
inline void io_async_wait(linux_io_uring_observer ring) FAST_IO_HERBCEPTIONS_THROWS
{
	liburing::io_uring_cqe *cqe{liburing::io_uring_wait_cqe(*ring.ring)};
	::fast_io::liburing::details::io_uring_dispatch_cqe(ring, cqe);
}

inline bool io_async_peek(linux_io_uring_observer ring) FAST_IO_HERBCEPTIONS_THROWS
{
	liburing::io_uring_cqe *cqe{liburing::io_uring_peek_cqe(*ring.ring)};
	if (cqe == nullptr)
	{
		return false;
	}
	::fast_io::liburing::details::io_uring_dispatch_cqe(ring, cqe);
	return true;
}

inline bool io_async_wait_timeout(linux_io_uring_observer ring,
								  ::fast_io::posix_statx_timestamp64 timestamp) FAST_IO_HERBCEPTIONS_THROWS
{
	liburing::io_uring_cqe *cqe{liburing::io_uring_wait_cqe_timeout(*ring.ring, timestamp)};
	if (cqe == nullptr)
	{
		return false;
	}
	::fast_io::liburing::details::io_uring_dispatch_cqe(ring, cqe);
	return true;
}

} // namespace fast_io
