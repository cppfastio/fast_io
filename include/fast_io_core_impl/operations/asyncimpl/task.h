#pragma once

namespace fast_io
{

/*
 * io_async_task<allocator>: the coroutine return type for the async
 * operations layer — a lazy fire-and-forget/co_await-able task.
 *
 * A coroutine declared `throws` delivers herbception errors escaping its
 * body to promise.unhandled_herbception(cxx_std_error) automatically;
 * callers never touch the error slot themselves:
 *  - an awaiting caller receives the error back as a real herbception
 *    at the co_await's await_resume() — thrown through the channel like
 *    any throws function;
 *  - a non-coroutine caller runs rethrow_if_error() once done() reports
 *    completion, or simply lets the frame go: the promise's destructor
 *    releases an unreceived error payload through its domain's cleanup.
 *
 * The task object owns the coroutine frame — no manual
 * coroutine_handle::destroy() anywhere; dropping a suspended task tears
 * the frame down like any other RAII object.
 *
 * T is the co_return value: void tasks return_void; value tasks use
 * return_value and co_await/rethrow_if_error hands the result back —
 *     io_async_task<int> f(args...) FAST_IO_HERBCEPTIONS_THROWS { co_return 42; }
 *
 * The frame is allocated through a fast_io allocator — a template
 * parameter, not a global default — so the task also works where there
 * is no global operator new. Statusless allocators
 * (generic_allocator_adapter<alloc>::has_status == false) need nothing
 * extra:
 *     io_async_task<void, alloc> f(args...) FAST_IO_HERBCEPTIONS_THROWS { ... }
 * Status (handle-based) allocators take the allocator handle as the
 * coroutine's first parameter; the compiler passes the coroutine
 * arguments to promise_type::operator new:
 *     io_async_task<void, alloc> f(alloc::handle_type handle, args...) FAST_IO_HERBCEPTIONS_THROWS
 * A status coroutine without the leading handle is a hard error rather
 * than a silent fall back to global operator new.
 *
 * Frame allocation failures are herbceptions: operator new uses the
 * *_try entry points and is itself throws, so a throws coroutine
 * propagates the failure to the caller through the channel like any
 * other ramp-phase failure.
 */

template <typename T = void, typename allocator = ::fast_io::native_global_allocator>
struct io_async_task
{
	using value_type = T;
	using allocator_type = allocator;
	using untyped_allocator_type = ::fast_io::generic_allocator_adapter<allocator_type>;
	static inline constexpr bool alloc_with_status{untyped_allocator_type::has_status};
	using alloc_handle_type =
		::std::conditional_t<alloc_with_status, typename untyped_allocator_type::handle_type,
							 allocator_type>;
	/* status handles ride in a header ahead of the coroutine frame inside
	 * the same allocation so operator delete — which receives no
	 * coroutine parameters — can recover the handle to deallocate with */
	static inline constexpr ::std::size_t status_prefix_size{
		((sizeof(alloc_handle_type) + untyped_allocator_type::default_alignment - 1) /
		 untyped_allocator_type::default_alignment) *
		untyped_allocator_type::default_alignment};

	/* co_return payload for value tasks: lives in the promise so a coroutine
	 * that dies to an herbception never constructs a T just to destroy it,
	 * and the frame destructor disposes a deposited result nobody took. */
	struct result_storage
	{
		bool ready{};
		union value_union
		{
			char dummy{};
			T value;
			constexpr value_union() noexcept
			{}
			inline ~value_union() noexcept
			{}
		};
		value_union store{};
		inline ~result_storage() noexcept
		{
			if (ready)
			{
				store.value.~T();
			}
		}
	};

	/* the promise's co_return entry lives in a base — a promise may name
	 * either return_void or return_value, never both */
	struct void_return_member
	{
		static inline constexpr void return_void() noexcept
		{}
	};
	struct value_return_member
	{
		result_storage result{};
		inline void return_value(T v) noexcept(::std::is_nothrow_move_constructible_v<T>)
		{
			new (__builtin_addressof(result.store.value)) T(::std::move(v));
			result.ready = true;
		}
	};
	using return_member =
		::std::conditional_t<::std::is_void_v<T>, void_return_member, value_return_member>;

	struct promise_type : return_member
	{
		::std::cxx_std_error error{};
		::std::coroutine_handle<> continuation{};
		bool detached{};

		inline io_async_task get_return_object() noexcept
		{
			return {::std::coroutine_handle<promise_type>::from_promise(*this)};
		}
		static inline constexpr ::std::suspend_always initial_suspend() noexcept
		{
			return {};
		}
		struct final_awaiter
		{
			static inline constexpr bool await_ready() noexcept
			{
				return false;
			}
			static inline ::std::coroutine_handle<>
			await_suspend(::std::coroutine_handle<promise_type> h) noexcept
			{
				if (h.promise().detached)
				{
					/* detached frames own themselves: completion destroys
					 * the frame right at final_suspend — asio's detached
					 * spawn semantics. The promise destructor still
					 * disposes a stored error payload. */
					h.destroy();
					return ::std::noop_coroutine();
				}
				auto c{h.promise().continuation};
				if (c != nullptr)
				{
					return c;
				}
				return ::std::noop_coroutine();
			}
			static inline constexpr void await_resume() noexcept
			{}
		};
		static inline constexpr final_awaiter final_suspend() noexcept
		{
			return {};
		}
		inline void unhandled_herbception(::std::cxx_std_error e) noexcept
		{
			error = e;
		}
		inline void unhandled_exception() noexcept
		{
			::fast_io::fast_terminate();
		}
		static inline void *operator new(::std::size_t n, auto &&...) FAST_IO_HERBCEPTIONS_THROWS
			requires(!alloc_with_status)
		{
			if constexpr (untyped_allocator_type::has_native_allocate_try)
			{
				return untyped_allocator_type::allocate_try(n);
			}
			else
			{
				return untyped_allocator_type::allocate_die(n);
			}
		}
		/* the first coroutine parameter carries the allocator handle —
		 * either the handle itself or a scheduler/device exposing
		 * .alloc_handle, which is how async APIs pass the scheduler:
		 *     f(sched, ...) — the scheduler decides frame allocation */
		static inline void *operator new(::std::size_t n, auto &&first, auto &&...) FAST_IO_HERBCEPTIONS_THROWS
			requires(alloc_with_status)
		{
			alloc_handle_type handle;
			if constexpr (requires { first.alloc_handle; })
			{
				handle = ::std::forward<decltype(first)>(first).alloc_handle;
			}
			else
			{
				static_assert(::std::convertible_to<decltype(first), alloc_handle_type>,
							  "status-allocator coroutine: first parameter must be the "
							  "allocator handle or expose .alloc_handle (e.g. a scheduler)");
				handle = ::std::forward<decltype(first)>(first);
			}
			void *base;
			if constexpr (untyped_allocator_type::has_native_handle_allocate_try)
			{
				base = untyped_allocator_type::handle_allocate_try(handle, status_prefix_size + n);
			}
			else
			{
				base = untyped_allocator_type::handle_allocate_die(handle, status_prefix_size + n);
			}
			*static_cast<alloc_handle_type *>(base) = handle;
			return static_cast<char unsigned *>(base) + status_prefix_size;
		}
		/* a status-allocator coroutine without the handle parameter must
		 * not silently fall back to global operator new */
		static void *operator new(::std::size_t) noexcept
			requires(alloc_with_status)
		= delete;

		static inline void operator delete(void *p, ::std::size_t n) noexcept
		{
			if constexpr (alloc_with_status)
			{
				auto *base{static_cast<char unsigned *>(p) - status_prefix_size};
				auto handle{*reinterpret_cast<alloc_handle_type const *>(base)};
				untyped_allocator_type::handle_deallocate_n(handle, base, status_prefix_size + n);
			}
			else
			{
				untyped_allocator_type::deallocate_n(p, n);
			}
		}

		/* a retained error is an owning handle — release its payload if
		 * nobody rethrew it before the frame went away */
		inline ~promise_type() noexcept
		{
			::fast_io::details::async_dispose_error(error);
		}
	};
	::std::coroutine_handle<promise_type> handle{};

	io_async_task() noexcept = default;
	inline constexpr io_async_task(::std::coroutine_handle<promise_type> h) noexcept
		: handle{h}
	{
	}
	io_async_task(io_async_task const &) = delete;
	io_async_task &operator=(io_async_task const &) = delete;
	inline io_async_task(io_async_task &&other) noexcept
		: handle{other.handle}
	{
		other.handle = nullptr;
	}
	inline io_async_task &operator=(io_async_task &&other) noexcept
	{
		if (this == __builtin_addressof(other)) [[unlikely]]
		{
			return *this;
		}
		if (handle != nullptr)
		{
			handle.destroy();
		}
		handle = other.handle;
		other.handle = nullptr;
		return *this;
	}
	/* owns the coroutine frame: destroys it when the task object dies.
	 * An awaited temporary task frees its (already completed) frame when
	 * the await expression ends, so child tasks don't leak either. */
	inline ~io_async_task() noexcept
	{
		if (handle != nullptr)
		{
			handle.destroy();
		}
	}

	inline ::std::coroutine_handle<promise_type> native_handle() const noexcept
	{
		return handle;
	}
	inline void resume() const noexcept
	{
		handle.resume();
	}
	/* fire-and-forget: releases ownership, marks the frame detached and
	 * starts it; the frame destroys itself at final_suspend. Any stored
	 * error payload is disposed by the promise destructor — a detached
	 * task's failure is silent unless the coroutine handled it itself. */
	inline void detach() noexcept
	{
		auto h{handle};
		handle = nullptr;
		h.promise().detached = true;
		h.resume();
	}
	inline bool done() const noexcept
	{
		return handle == nullptr || handle.done();
	}
	/* synchronous error check for non-coroutine callers: rethrows a
	 * stored herbception error through the channel. The payload is moved
	 * out — the promise must not dispose it again */
	inline void rethrow_if_error() FAST_IO_HERBCEPTIONS_THROWS
	{
		auto e{handle.promise().error};
		handle.promise().error = {};
		if (e.domain != nullptr)
		{
			throw throws e;
		}
	}

	inline constexpr bool await_ready() const noexcept
	{
		return false;
	}
	inline ::std::coroutine_handle<> await_suspend(::std::coroutine_handle<> h) noexcept
	{
		handle.promise().continuation = h;
		return handle;
	}
	/* co_await yields the stored herbception error back to the awaiting
	 * coroutine as a real throw — no manual error slot inspection; for
	 * value tasks the deposited result is moved out on success */
	inline void await_resume() FAST_IO_HERBCEPTIONS_THROWS
		requires(::std::is_void_v<T>)
	{
		rethrow_if_error();
	}
	inline T await_resume() FAST_IO_HERBCEPTIONS_THROWS
		requires(!::std::is_void_v<T>)
	{
		rethrow_if_error();
		return ::std::move(handle.promise().result.store.value);
	}
};

/* co_await this inside an io_async_task coroutine to reach its promise */
template <typename T, typename allocator>
struct io_async_task_promise_access
{
	using promise_type = typename io_async_task<T, allocator>::promise_type;
	promise_type *promise{};
	inline constexpr bool await_ready() const noexcept
	{
		return false;
	}
	inline bool await_suspend(::std::coroutine_handle<promise_type> h) noexcept
	{
		promise = __builtin_addressof(h.promise());
		return false;
	}
	inline promise_type &await_resume() noexcept
	{
		return *promise;
	}
};

} // namespace fast_io
