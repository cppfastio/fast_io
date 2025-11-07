#pragma once

#include <chrono>
#include <ranges>
#include <cstdint>
#include <utility>
#include <functional>
#include <type_traits>
#include "../../../fast_io_dsal/tuple.h"
#include "../../../fast_io_core_impl/allocation/common.h"

namespace fast_io::win32
{

namespace details
{

class win32_thread_start_routine_tuple_allocate_guard
{
public:
	void *ptr_{nullptr};

	constexpr win32_thread_start_routine_tuple_allocate_guard() = default;

	constexpr win32_thread_start_routine_tuple_allocate_guard(void *ptr) : ptr_{ptr}
	{}

	constexpr win32_thread_start_routine_tuple_allocate_guard(win32_thread_start_routine_tuple_allocate_guard const &) noexcept = delete;
	constexpr win32_thread_start_routine_tuple_allocate_guard(win32_thread_start_routine_tuple_allocate_guard &&other) noexcept = default;

	constexpr ~win32_thread_start_routine_tuple_allocate_guard()
	{
		if (ptr_ != nullptr)
		{
			::fast_io::generic_allocator_adapter<::fast_io::win32_heapalloc_allocator>::deallocate(this->ptr_);
		}
	}
};

template <typename Tuple, ::std::size_t... Is>
constexpr ::std::uint_least32_t FAST_IO_WINSTDCALL thread_start_routine(void *args) noexcept(noexcept(
	::std::invoke(::fast_io::get<Is>(*reinterpret_cast<Tuple *>(args))...)))
{
	[[maybe_unused]] ::fast_io::win32::details::win32_thread_start_routine_tuple_allocate_guard _(args);
	::std::invoke(::fast_io::get<Is>(*reinterpret_cast<Tuple *>(args))...);
	return 0;
}

template <typename Tuple, ::std::size_t... Is>
constexpr auto get_thread_start_routine(::std::index_sequence<Is...>) noexcept
{
	return ::fast_io::win32::details::thread_start_routine<Tuple, Is...>;
}

} // namespace details

class win32_thread
{
public:
	using id = ::std::uint_least32_t;
	using native_handle_type = void *;

private:
	id id_{};
	native_handle_type handle_{nullptr};

public:
	win32_thread() noexcept = default;

	template <typename Func, typename... Args>
		requires(::std::invocable<Func, Args...>)
	constexpr win32_thread(Func &&func, Args &&...args)
	{
		using start_routine_tuple_type = ::fast_io::tuple<decltype(func), decltype(args)...>;
		void *start_routine_tuple{::fast_io::generic_allocator_adapter<::fast_io::win32_heapalloc_allocator>::allocate(sizeof(start_routine_tuple_type))};
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif
		::new (start_routine_tuple) start_routine_tuple_type{::std::forward<Func>(func), ::std::forward<Args>(args)...};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
		auto start_routine = ::fast_io::win32::details::get_thread_start_routine<start_routine_tuple_type>(
			::std::make_index_sequence<sizeof...(Args) + 1>{});
		this->handle_ = ::fast_io::win32::CreateThread(
			nullptr,
			0,
			start_routine,
			start_routine_tuple, // args of func
			0,
			__builtin_addressof(this->id_));
		if (this->handle_ == nullptr) [[unlikely]]
		{
			::fast_io::throw_win32_error();
		}
	}

	constexpr win32_thread(win32_thread const &) noexcept = delete;

	constexpr win32_thread(win32_thread &&other) noexcept = default;

	constexpr ~win32_thread() noexcept
	{
		if (handle_ != nullptr)
		{
			if (this->joinable()) [[unlikely]]
			{
				::fast_io::fast_terminate();
			}
			if (!::fast_io::win32::CloseHandle(this->handle_)) [[unlikely]]
			{
				::fast_io::fast_terminate();
			}
		}
	}

	constexpr win32_thread &operator=(win32_thread const &) noexcept = delete;

	constexpr win32_thread &operator=(win32_thread &&other) noexcept
	{
		if (this == __builtin_addressof(other)) [[unlikely]]
		{
			return *this;
		}
		this->swap(other);
		return *this;
	}
	constexpr bool joinable() const noexcept
	{
		return this->id_ != 0;
	}

#if __cpp_constexpr >= 202207L
	// https://en.cppreference.com/w/cpp/compiler_support/23.html#cpp_constexpr_202207L
	// for reduce some warning purpose
	constexpr
#endif
		void join()
	{
		if (!this->joinable()) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		::fast_io::win32::WaitForSingleObject(this->handle_, /* INFINITE = */ 0xffffffff);
		this->id_ = 0;
	}

#if __cpp_constexpr >= 202207L
	// https://en.cppreference.com/w/cpp/compiler_support/23.html#cpp_constexpr_202207L
	// for reduce some warning purpose
	constexpr
#endif
		void detach()
	{
		if (!this->joinable()) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		if (!::fast_io::win32::CloseHandle(this->handle_)) [[unlikely]]
		{
			::fast_io::throw_win32_error();
		}
		this->handle_ = nullptr;
		this->id_ = 0;
	}

	constexpr void swap(win32_thread &other) noexcept
	{
		::std::ranges::swap(handle_, other.handle_);
		::std::ranges::swap(id_, other.id_);
	}

	[[nodiscard]]
	constexpr auto get_id() const noexcept
	{
		return this->id_;
	}

	[[nodiscard]]
	constexpr auto native_handle() const noexcept
	{
		return this->handle_;
	}

	[[nodiscard]]
	static
#if __cpp_constexpr >= 202207L
		// https://en.cppreference.com/w/cpp/compiler_support/23.html#cpp_constexpr_202207L
		// for reduce some warning purpose
		constexpr
#endif
		::std::uint_least32_t hardware_concurrency() noexcept
	{
		::fast_io::win32::system_info si{};
		::fast_io::win32::GetSystemInfo(__builtin_addressof(si));
		return si.dwNumberOfProcessors;
	}
};

namespace this_thread
{

[[nodiscard]]
#if __cpp_constexpr >= 202207L
// https://en.cppreference.com/w/cpp/compiler_support/23.html#cpp_constexpr_202207L
// for reduce some warning purpose
constexpr
#endif
	auto get_id() noexcept -> ::fast_io::win32::win32_thread::id
{
	return ::fast_io::win32::GetCurrentThreadId();
}

template <typename Rep, typename Period>
constexpr void sleep_for(::std::chrono::duration<Rep, Period> const &sleep_duration) noexcept
{
	::fast_io::win32::Sleep(static_cast<::std::uint_least32_t>(::std::chrono::duration_cast<::std::chrono::milliseconds>(sleep_duration).count()));
}

template <typename Clock, typename Duration>
constexpr void sleep_until(::std::chrono::time_point<Clock, Duration> const &expect_time) noexcept
{

	auto now = Clock::now();
	if (now < expect_time)
	{
		::fast_io::win32::Sleep(static_cast<::std::uint_least32_t>(
			::std::chrono::duration_cast<::std::chrono::milliseconds>(expect_time - now).count()));
	}
}

} // namespace this_thread

} // namespace fast_io::win32
