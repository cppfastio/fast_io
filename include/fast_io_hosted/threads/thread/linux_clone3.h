#pragma once

#if !defined(__linux__)
#error "This file is for linux only"
#endif

#error "We do not recommend using clone3, as it omits the allocation of numerous TLS signals and other components."

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <ranges>
#include <unistd.h>
#include <signal.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <linux/sched.h>
#include "stack_pointer.h"

namespace fast_io
{

class linux_clone3_thread
{
	using id = ::pid_t;

private:
	template <typename F, typename... A>
	struct alignas(16) child_ctx
	{
		F func;
		::fast_io::containers::tuple<A...> args;
	};

	struct alignas(16) clone3_thread_stack
	{
		static constexpr ::std::size_t stack_size_{1024 * 1024};

		::std::byte tail_[stack_size_];
		alignas(4)::std::atomic<int> futex_word_{1};
	};

	using thread_stack_type_allocator =
		::fast_io::typed_generic_allocator_adapter<::fast_io::generic_allocator_adapter<::fast_io::c_malloc_allocator>, clone3_thread_stack>;

	id id_{};
	clone3_thread_stack *stack_{};

private:
	template <typename F, typename... A>
	[[gnu::noreturn]]
	inline static constexpr void child_entry(child_ctx<F, A...> *ctx) noexcept
	{
#ifdef FAST_IO_CPP_EXCEPTIONS
		try
#endif
		{
			apply(ctx->func, ctx->args);
		}
#ifdef FAST_IO_CPP_EXCEPTIONS
		catch (...)
		{
			::fast_io::fast_terminate();
		}
#endif
		// Kernel-dependent CLONE_CHILD_CLEARTID is cleared to 0 upon exit and wakes the futex

		// libc will auto destroy stack_

		::fast_io::fast_exit(0);
	}

public:
	inline constexpr linux_clone3_thread() noexcept = default;

	template <typename Func, typename... Args>
		requires std::invocable<Func, Args...>
	inline constexpr linux_clone3_thread(Func &&func, Args &&...args)
	{
		// Assume the returned address is 16 aligned
		this->stack_ = thread_stack_type_allocator::allocate(1u);
		::new (this->stack_) clone3_thread_stack{};

		using decF = ::std::decay_t<Func>;
		using decArgsTuple = ::fast_io::containers::tuple<::std::decay_t<Args>...>;
		using ctx_t = child_ctx<decF, ::std::decay_t<Args>...>;

		// Pre-construct the context at the top of the child thread stack; the child thread derives the address by backtracking via RSP.
		auto tail_base{reinterpret_cast<::std::uintptr_t>(__builtin_addressof(this->stack_->tail_))};
		auto top_of_stack{tail_base + clone3_thread_stack::stack_size_};
		constexpr ::std::size_t ctx_size{sizeof(ctx_t)};
		constexpr ::std::size_t ctx_align{alignof(ctx_t)};
		auto ctx_addr{(top_of_stack - ctx_size) & ~static_cast<::std::uintptr_t>(ctx_align - 1u)};
		auto ctx_ptr{reinterpret_cast<ctx_t *>(ctx_addr)};
		::new (ctx_ptr) ctx_t{::std::forward<Func>(func), decArgsTuple{::std::forward<Args>(args)...}};

		// TODO support TLS (currently avoid using any glibc wrappers requiring TLS)
		clone_args clone3_arg{
			.flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD | CLONE_CHILD_CLEARTID | CLONE_CHILD_SETTID,
			.pidfd = 0,
			.child_tid = reinterpret_cast<::std::size_t>(__builtin_addressof(this->stack_->futex_word_)),
			.parent_tid = 0,
			.exit_signal = 0,
			.stack = reinterpret_cast<::std::size_t>(__builtin_addressof(this->stack_->tail_)),
			.stack_size = clone3_thread_stack::stack_size_,
			.tls = 0,
			.set_tid = 0,
			.set_tid_size = 0,
			.cgroup = 0,
		};

		// futex_word_ is initially set to 1 (running). The kernel clears it to 0 and wakes it up upon exit.

		auto clone3_result = ::fast_io::inline_syscall<__NR_clone3, ::pid_t>(__builtin_addressof(clone3_arg), sizeof(clone_args));
		if (clone3_result < 0)
		{
			::std::destroy_at(this->stack_);
			thread_stack_type_allocator::deallocate_n(this->stack_, 1u);

			::fast_io::throw_posix_error();
		}
		else if (clone3_result == 0)
		{
			// Child thread: Derive the ctx address solely from RSP to avoid accessing the parent stack and this.
			constexpr ::std::size_t ctx_size_c{sizeof(ctx_t)};
			constexpr ::std::size_t ctx_align_c{alignof(ctx_t)};
			auto sp{reinterpret_cast<::std::uintptr_t>(::fast_io::details::get_stack_pointer())};
			auto ctx_addr_child{(sp - ctx_size_c) & ~static_cast<::std::uintptr_t>(ctx_align_c - 1u)};
			auto ctx_child{reinterpret_cast<ctx_t *>(ctx_addr_child)};
			// Reserve sufficient stack space to prevent subsequent calls from overwriting the context, and align to 16 bytes.
			auto reserve{(sp - ctx_addr_child) + static_cast<::std::uintptr_t>(128u)};
			reserve = (reserve + 15u) & ~static_cast<::std::uintptr_t>(15u);

			linux_clone3_thread::child_entry<decF, ::std::decay_t<Args>...>(ctx_child);

			__builtin_unreachable();
		}
		else
		{
			this->id_ = clone3_result;
		}
	}

	inline constexpr linux_clone3_thread(linux_clone3_thread &&other) noexcept = default;

	inline constexpr linux_clone3_thread(linux_clone3_thread const &) noexcept = delete;

	inline constexpr ~linux_clone3_thread()
	{
		if (this->joinable()) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}

		thread_stack_type_allocator::deallocate_n(this->stack_, 1u);
	}

	inline constexpr linux_clone3_thread &operator=(linux_clone3_thread const &) noexcept = delete;

	inline constexpr linux_clone3_thread &operator=(linux_clone3_thread &&other) & noexcept
	{
		this->swap(other);
		return *this;
	}

	inline constexpr void swap(linux_clone3_thread &other) noexcept
	{
		::std::ranges::swap(this->id_, other.id_);
		::std::ranges::swap(this->stack_, other.stack_);
	}

	inline constexpr bool joinable() const noexcept
	{
		return this->stack_->futex_word_.load() != 0;
	}

	inline constexpr void join()
	{
		if (!this->joinable()) [[unlikely]]
		{
			::fast_io::throw_posix_error();
		}
		// Wait for futex_word_ to change from 1 to 0 (cleared by the kernel upon child thread exit and awakened)
		for (;;)
		{
			int v = this->stack_->futex_word_.load(::std::memory_order_acquire);
			if (v == 0)
			{
				break;
			}
			(void)::fast_io::system_call<__NR_futex, long>(
				reinterpret_cast<int *>(__builtin_addressof(this->stack_->futex_word_)),
				FUTEX_WAIT,
				v,
				nullptr,
				nullptr,
				0);
		}
	}

	inline constexpr void detach() noexcept
	{
		this->id_ = 0;
	}
};

} // namespace fast_io