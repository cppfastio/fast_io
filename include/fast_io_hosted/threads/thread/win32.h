#pragma once

#include <chrono>
#include <ranges>
#include <cstdint>
#include <utility>
#include <functional>
#include <type_traits>

namespace fast_io
{
namespace win32
{

namespace details
{

template <typename Tuple>
class win32_thread_start_routine_tuple_allocate_guard
{
public:
	void *ptr_{nullptr};

	inline constexpr win32_thread_start_routine_tuple_allocate_guard() = default;

	inline constexpr win32_thread_start_routine_tuple_allocate_guard(void *ptr) : ptr_{ptr}
	{}

	inline constexpr win32_thread_start_routine_tuple_allocate_guard(win32_thread_start_routine_tuple_allocate_guard const &) noexcept = delete;
	inline constexpr win32_thread_start_routine_tuple_allocate_guard(win32_thread_start_routine_tuple_allocate_guard &&other) noexcept = delete;
	inline constexpr win32_thread_start_routine_tuple_allocate_guard &operator=(win32_thread_start_routine_tuple_allocate_guard const &) noexcept = delete;
	inline constexpr win32_thread_start_routine_tuple_allocate_guard &operator=(win32_thread_start_routine_tuple_allocate_guard &&other) noexcept = delete;

	inline constexpr ~win32_thread_start_routine_tuple_allocate_guard()
	{
		if (ptr_ != nullptr)
		{
			::std::destroy_at(reinterpret_cast<Tuple *>(this->ptr_));

			using alloc = ::fast_io::native_typed_global_allocator<Tuple>;
			alloc::deallocate_n(reinterpret_cast<Tuple *>(this->ptr_), 1u);
		}
	}
};

template <typename Tuple, ::std::size_t... Is>
inline constexpr ::std::uint_least32_t FAST_IO_WINSTDCALL thread_start_routine(void *args) noexcept
{
	/*
	 * Just call the API function. Any CRT specific processing is done in
	 * DllMain DLL_THREAD_ATTACH
	 */

	[[maybe_unused]] ::fast_io::win32::details::win32_thread_start_routine_tuple_allocate_guard<Tuple> _(args);

#ifdef FAST_IO_CPP_EXCEPTIONS
	try
#endif
	{
		::std::invoke(::fast_io::containers::get<Is>(*reinterpret_cast<Tuple *>(args))...);
	}
#ifdef FAST_IO_CPP_EXCEPTIONS
	catch (...)
	{
		::fast_io::fast_terminate();
	}
#endif

	return 0;
}

template <typename Tuple, ::std::size_t... Is>
inline constexpr auto get_thread_start_routine(::std::index_sequence<Is...>) noexcept
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
	inline constexpr win32_thread() noexcept = default;

	template <typename Func, typename... Args>
		requires(::std::invocable<Func, Args...>)
	inline constexpr win32_thread(Func &&func, Args &&...args)
	{
		using start_routine_tuple_type = ::fast_io::containers::tuple<::std::decay_t<Func>, ::std::decay_t<Args>...>;
		using alloc = ::fast_io::native_typed_global_allocator<start_routine_tuple_type>;

		auto start_routine_tuple{alloc::allocate(1u)};
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
			// Creation failed; manual release is required
			::std::destroy_at(reinterpret_cast<start_routine_tuple_type *>(start_routine_tuple));
			alloc::deallocate_n(start_routine_tuple, 1u);

			::fast_io::throw_win32_error();
		}
	}

	inline constexpr win32_thread(win32_thread const &) noexcept = delete;

	inline constexpr win32_thread(win32_thread &&other) noexcept : id_{other.id_}, handle_{other.handle_}
	{
		other.id_ = 0;
		other.handle_ = nullptr;
	}

	inline constexpr ~win32_thread() noexcept
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

	inline constexpr win32_thread &operator=(win32_thread const &) noexcept = delete;

	inline constexpr win32_thread &operator=(win32_thread &&other) noexcept
	{
		if (this == __builtin_addressof(other)) [[unlikely]]
		{
			return *this;
		}
		this->swap(other);
		return *this;
	}
	inline constexpr bool joinable() const noexcept
	{
		return this->id_ != 0;
	}

	inline
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

		auto const value{::fast_io::win32::WaitForSingleObject(this->handle_, /* INFINITE = */ 0xFFFFFFFFu)};

		if (value == 0xFFFFFFFFu) [[unlikely]]
		{
			::fast_io::throw_win32_error();
		}

		if (!::fast_io::win32::CloseHandle(this->handle_)) [[unlikely]]
		{
			::fast_io::throw_win32_error();
		}

		this->handle_ = nullptr;
		this->id_ = 0;
	}

	inline
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

	inline constexpr void swap(win32_thread &other) noexcept
	{
		::std::ranges::swap(handle_, other.handle_);
		::std::ranges::swap(id_, other.id_);
	}

	[[nodiscard]]
	inline constexpr auto get_id() const noexcept
	{
		return this->id_;
	}

	[[nodiscard]]
	inline constexpr auto native_handle() const noexcept
	{
		return this->handle_;
	}

	[[nodiscard]]
	inline static
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
inline
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
inline
#if __cpp_constexpr >= 202207L
	// https://en.cppreference.com/w/cpp/compiler_support/23.html#cpp_constexpr_202207L
	// for reduce some warning purpose
	constexpr
#endif
	void sleep_for(::std::chrono::duration<Rep, Period> const &sleep_duration) noexcept
{
	auto const ms64{::std::chrono::duration_cast<::std::chrono::milliseconds>(sleep_duration).count()};
	if (ms64 <= 0)
	{
		return;
	}

	auto const u64{static_cast<::std::uint_least64_t>(ms64)};
	auto const ms{u64 > 0xFFFFFFFFu ? static_cast<::std::uint_least32_t>(0xFFFFFFFFu)
									: static_cast<::std::uint_least32_t>(u64)};
	::fast_io::win32::Sleep(ms);
}

template <typename Clock, typename Duration>
inline
#if __cpp_constexpr >= 202207L
	// https://en.cppreference.com/w/cpp/compiler_support/23.html#cpp_constexpr_202207L
	// for reduce some warning purpose
	constexpr
#endif
	void sleep_until(::std::chrono::time_point<Clock, Duration> const &expect_time) noexcept
{

	auto const now{Clock::now()};
	if (now < expect_time)
	{
		auto const ms64{::std::chrono::duration_cast<::std::chrono::milliseconds>(expect_time - now).count()};
		if (ms64 <= 0)
		{
			return;
		}
		auto const u64{static_cast<::std::uint_least64_t>(ms64)};
		auto const ms{u64 > 0xFFFFFFFFu ? static_cast<::std::uint_least32_t>(0xFFFFFFFFu)
										: static_cast<::std::uint_least32_t>(u64)};
		::fast_io::win32::Sleep(ms);
	}
}

template <::std::int_least64_t off_to_epoch>
inline
#if __cpp_constexpr >= 202207L
	// https://en.cppreference.com/w/cpp/compiler_support/23.html#cpp_constexpr_202207L
	// for reduce some warning purpose
	constexpr
#endif
	void sleep_for(::fast_io::basic_timestamp<off_to_epoch> const &sleep_duration) noexcept
{

	if (sleep_duration.seconds < 0)
	{
		return;
	}

	constexpr ::std::uint_least64_t mul_factor{::fast_io::uint_least64_subseconds_per_second / 1000000000u};

	auto const win_100ns{
		static_cast<::std::uint_least64_t>(static_cast<::std::uint_least64_t>(sleep_duration.seconds) * 10'000'000ULL +
										   sleep_duration.subseconds / mul_factor / 100u)};

	auto const ms64{win_100ns / 10'000ULL};
	auto const ms{ms64 > 0xFFFFFFFFu ? static_cast<::std::uint_least32_t>(0xFFFFFFFFu)
									 : static_cast<::std::uint_least32_t>(ms64)};
	if (ms == 0)
	{
		return;
	}

	::fast_io::win32::Sleep(ms);
}

template <::std::int_least64_t off_to_epoch>
inline
#if __cpp_constexpr >= 202207L
	// https://en.cppreference.com/w/cpp/compiler_support/23.html#cpp_constexpr_202207L
	// for reduce some warning purpose
	constexpr
#endif
	void sleep_until(::fast_io::basic_timestamp<off_to_epoch> const &expect_time) noexcept
{
	::fast_io::win32::filetime ft{};
	::fast_io::win32::GetSystemTimeAsFileTime(__builtin_addressof(ft));
	auto const now_100ns{(static_cast<::std::uint_least64_t>(ft.dwHighDateTime) << 32) |
						 static_cast<::std::uint_least64_t>(ft.dwLowDateTime)};


	auto const win32_ts{static_cast<::fast_io::win32_timestamp>(expect_time)};

	if (win32_ts.seconds < 0)
	{
		return;
	}

	constexpr ::std::uint_least64_t mul_factor{::fast_io::uint_least64_subseconds_per_second / 1000000000u};

	auto const expect_100ns{
		static_cast<::std::uint_least64_t>(static_cast<::std::uint_least64_t>(win32_ts.seconds) * 10'000'000ULL +
										   win32_ts.subseconds / mul_factor / 100u)};

	if (expect_100ns <= now_100ns)
	{
		return;
	}

	auto const delta_100ns{expect_100ns - now_100ns};
	auto const ms64{delta_100ns / 10'000ULL};
	auto const ms{ms64 > 0xFFFFFFFFu ? static_cast<::std::uint_least32_t>(0xFFFFFFFFu)
									 : static_cast<::std::uint_least32_t>(ms64)};
	if (ms == 0)
	{
		return;
	}

	// Since Win9x lacks CreateWaitableTimerW, all implementations on WinNT use NT threads. Therefore, sleep is used here as a simulation.
	::fast_io::win32::Sleep(ms);
}

} // namespace this_thread

} // namespace win32

using win32_thread = win32::win32_thread;

#if ((defined(_WIN32) && !defined(__WINE__)) && !defined(__CYGWIN__)) && defined(_WIN32_WINDOWS)
using native_thread = win32_thread;

namespace this_thread
{
using ::fast_io::win32::this_thread::get_id;
using ::fast_io::win32::this_thread::sleep_for;
using ::fast_io::win32::this_thread::sleep_until;
} // namespace this_thread
#endif
} // namespace fast_io
