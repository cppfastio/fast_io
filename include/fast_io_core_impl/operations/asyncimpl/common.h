#pragma once

#include <coroutine>

namespace fast_io::details
{

/* scheduler-provided allocator: a fast_io generic allocator adapter.
 * Async defines allocate and free their operation state through
 * it; schedulers without one fall back to native_global_allocator. */
template <typename scheduler>
concept async_scheduler_has_allocator = requires { typename scheduler::allocator_type; };

/* lazily resolved: scheduler::allocator_type is named only when the
 * scheduler actually provides one — schedulers without it fall back to
 * native_global_allocator */
template <typename scheduler, bool = async_scheduler_has_allocator<scheduler>>
struct async_scheduler_allocator
{
	using type = ::fast_io::native_global_allocator;
};
template <typename scheduler>
struct async_scheduler_allocator<scheduler, true>
{
	using type = typename scheduler::allocator_type;
};

template <typename scheduler>
using async_scheduler_allocator_t = typename async_scheduler_allocator<scheduler>::type;

/* RAII owner of raw storage for one state object — the
 * local_operator_new_array_ptr pattern, minus array construction
 * (states hold lambdas which are never default-constructible). The
 * block frees itself on destruction unless release()d; allocation is
 * the plain allocate/handle_allocate entry point, so it throws only
 * when the adapter carries throws_on_allocation_failure — the same
 * conditional contract as buffer_alloc_arr_ptr. */
template <typename T, typename allocator>
class async_state_ptr
{
public:
	using allocator_type = allocator;
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator, T>;
	static inline constexpr bool alloc_with_status{typed_allocator_type::has_status};
	using handle_type =
		::std::conditional_t<alloc_with_status, typename typed_allocator_type::handle_type,
							 ::fast_io::details::empty>;
	FAST_IO_NO_UNIQUE_ADDRESS handle_type allochdl{};
	T *ptr{};
	::std::size_t size{};

	inline explicit async_state_ptr(::std::size_t n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(typed_allocator_type::throws_on_allocation_failure)
		requires(!alloc_with_status)
		: ptr(static_cast<T *>(typed_allocator_type::allocate(n))), size(n)
	{
	}
	inline explicit async_state_ptr(handle_type hdl, ::std::size_t n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(typed_allocator_type::throws_on_allocation_failure)
		requires(alloc_with_status)
		: allochdl(hdl), ptr(static_cast<T *>(typed_allocator_type::handle_allocate(hdl, n))), size(n)
	{
	}
	async_state_ptr(async_state_ptr const &) = delete;
	async_state_ptr &operator=(async_state_ptr const &) = delete;
	inline ~async_state_ptr() noexcept
	{
		if (ptr) [[likely]]
		{
			if constexpr (alloc_with_status)
			{
				typed_allocator_type::handle_deallocate_n(allochdl, ptr, size);
			}
			else if constexpr (typed_allocator_type::has_deallocate)
			{
				typed_allocator_type::deallocate(ptr);
			}
			else
			{
				typed_allocator_type::deallocate_n(ptr, size);
			}
		}
	}
	inline constexpr T *get() noexcept
	{
		return ptr;
	}
	inline constexpr T *release() noexcept
	{
		T *p{ptr};
		ptr = nullptr;
		return p;
	}
};

/* allocate + construct a T state object through the scheduler's
 * allocator: async_state_ptr frees the raw block if the T constructor
 * throws. Every state object's first ctor argument is the scheduler; a
 * scheduler whose allocator_type has status supplies the handle as an
 * alloc_handle member, which the object copies into itself so
 * async_delete_state can free without the scheduler. */
template <typename T, typename scheduler, typename... Args>
inline T *async_new_state(scheduler sched, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		::fast_io::typed_generic_allocator_adapter<async_scheduler_allocator_t<scheduler>, T>::throws_on_allocation_failure ||
		!::std::is_nothrow_constructible_v<T, scheduler &, Args &&...>)
{
	using guard_type = async_state_ptr<T, async_scheduler_allocator_t<scheduler>>;
	if constexpr (guard_type::alloc_with_status)
	{
		guard_type guard{sched.alloc_handle, 1};
		new (guard.ptr) T(sched, ::std::forward<Args>(args)...);
		guard.ptr->alloc_handle = sched.alloc_handle;
		return guard.release();
	}
	else
	{
		guard_type guard{1zu};
		new (guard.ptr) T(sched, ::std::forward<Args>(args)...);
		return guard.release();
	}
}

/*
 * like async_new_state but without the leading scheduler ctor argument —
 * backends whose cookie's first member is a dispatch base (the io_uring
 * invoke pointer, the IOCP OVERLAPPED) construct T purely from args */
template <typename T, typename scheduler, typename... Args>
inline T *async_new_state_plain(scheduler sched, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		::fast_io::typed_generic_allocator_adapter<async_scheduler_allocator_t<scheduler>, T>::throws_on_allocation_failure ||
		!::std::is_nothrow_constructible_v<T, Args &&...>)
{
	using guard_type = async_state_ptr<T, async_scheduler_allocator_t<scheduler>>;
	if constexpr (guard_type::alloc_with_status)
	{
		guard_type guard{sched.alloc_handle, 1};
		new (guard.ptr) T(::std::forward<Args>(args)...);
		guard.ptr->alloc_handle = sched.alloc_handle;
		return guard.release();
	}
	else
	{
		guard_type guard{1zu};
		new (guard.ptr) T(::std::forward<Args>(args)...);
		return guard.release();
	}
}

/* every self-owning state object exposes its allocator as
 * allocator_type; status allocators additionally carry the handle in
 * alloc_handle */
template <typename T>
inline void async_delete_state(T *p) noexcept
{
	using typed_alloc = ::fast_io::typed_generic_allocator_adapter<typename T::allocator_type, T>;
	if constexpr (typed_alloc::has_status)
	{
		auto handle{p->alloc_handle};
		p->~T();
		typed_alloc::handle_deallocate_n(handle, p, 1);
	}
	else
	{
		p->~T();
		typed_alloc::deallocate(p);
	}
}

/* synthesize a cxx_std_error for a registered errc type the way
 * `throw throws` would: error_domain<T>::domain() + code(ec). Used for
 * generic-layer errors such as end_of_file on an unfinished all loop. */
template <typename T>
inline constexpr ::std::cxx_std_error async_make_error(T ec) noexcept
{
	return {::std::error_domain<T>::domain(), ::std::error_domain<T>::code(ec)};
}

/*
 * Release a held error value's payload. A cxx_std_error is an owning
 * handle: domain == nullptr is the success value (not an error at all),
 * and when domain->do_cleanup is non-null the code slot carries a
 * resource — e.g. a C++ exception object handle — that must be released.
 * Ownership of every cxx_std_error a callback receives transfers to the
 * callee; forwarding the value transfers it onward. Whatever path is
 * terminal for the handle runs this once — or rethrows it through
 * `throw throws`, which hands ownership back to the error channel and
 * lets the caught std::error's destructor clean up.
 */
inline void async_dispose_error(::std::cxx_std_error err) noexcept
{
	if (err.domain != nullptr) [[unlikely]]
	{
		auto domain{static_cast<::std::error_domain_singleton const *>(err.domain)};
		if (domain->do_cleanup != nullptr)
		{
			domain->do_cleanup(err.code);
		}
	}
}

/*
 * Submit one backend operation and translate a synchronous submission
 * failure into the callback contract: submission runs inside a
 * throws-try so a backend that throws before committing delivers the
 * error through onerr(::std::cxx_std_error) instead of escaping the
 * noexcept generic functions.
 */
template <typename submitfunc, typename errfunc>
inline void async_submit_catching(submitfunc &&submit, errfunc &&onerr) noexcept
{
	try
	{
		::std::forward<submitfunc>(submit)();
	}
	catch throws(::std::error e)
	{
		::std::forward<errfunc>(onerr)(e.release());
	}
}

/*
 * Type-erased async completions. A lambda passed to a backend's
 * *_callback_define mints a fresh closure type -- and a whole backend
 * cookie instantiation -- per call site. The erasure carries the state
 * pointer plus the typed step function that knows how to continue, so
 * every generic async chain shares the same callback type at the
 * backend boundary.
 */
struct async_bytes_step_cb
{
	void *ctx;
	void (*step)(void *ctx, ::std::cxx_std_error err, ::std::size_t n) noexcept;
	inline void operator()(::std::cxx_std_error err, ::std::size_t n) const noexcept
	{
		step(ctx, err, n);
	}
};

struct async_err_step_cb
{
	void *ctx;
	void (*step)(void *ctx, ::std::cxx_std_error err) noexcept;
	inline void operator()(::std::cxx_std_error err) const noexcept
	{
		step(ctx, err);
	}
};

/* submit-side work for async_submit_catching: aux carries the round's
 * computed operand (chunk/left/bound) */
struct async_work_step_cb
{
	void *ctx;
	::std::size_t aux;
	void (*work)(void *ctx, ::std::size_t aux) noexcept;
	inline void operator()() const noexcept
	{
		work(ctx, aux);
	}
};

/* Coroutine plumbing shared by every async_*_decay awaiter: the awaiter
 * stores params by value, awaits submission of the callback form, and
 * await_resume rethrows a recorded error through the channel. */

} // namespace fast_io::details
