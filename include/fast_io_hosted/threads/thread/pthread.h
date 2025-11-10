#pragma once

#include <chrono>
#include <ranges>
#include <cstdint>
#include <utility>
#include <functional>
#include <type_traits>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

namespace fast_io
{

namespace posix
{

namespace details
{

template <typename Tuple>
class pthread_thread_start_routine_tuple_allocate_guard
{
public:
	void *ptr_{nullptr};

	inline constexpr pthread_thread_start_routine_tuple_allocate_guard() = default;

	inline constexpr pthread_thread_start_routine_tuple_allocate_guard(void *ptr) : ptr_{ptr}
	{}

	inline constexpr pthread_thread_start_routine_tuple_allocate_guard(pthread_thread_start_routine_tuple_allocate_guard const &) noexcept = delete;
	inline constexpr pthread_thread_start_routine_tuple_allocate_guard(pthread_thread_start_routine_tuple_allocate_guard &&other) noexcept = delete;
	inline constexpr pthread_thread_start_routine_tuple_allocate_guard& operator=(pthread_thread_start_routine_tuple_allocate_guard const &) noexcept = delete;
	inline constexpr pthread_thread_start_routine_tuple_allocate_guard& operator=(pthread_thread_start_routine_tuple_allocate_guard &&other) noexcept = delete;

	inline constexpr ~pthread_thread_start_routine_tuple_allocate_guard()
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
inline constexpr void *thread_start_routine(void *args) noexcept
{
	/*
	 * Just call the API function then destroy/deallocate tuple.
	 */

	[[maybe_unused]] ::fast_io::posix::details::pthread_thread_start_routine_tuple_allocate_guard<Tuple> _(args);

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

	return nullptr;
}

template <typename Tuple, ::std::size_t... Is>
inline constexpr auto get_thread_start_routine(::std::index_sequence<Is...>) noexcept
{
	return ::fast_io::posix::details::thread_start_routine<Tuple, Is...>;
}

} // namespace details

class pthread_thread
{
public:
	using id = ::pthread_t;
	using native_handle_type = ::pthread_t;

private:
	id id_{};
	bool joinable_{false};

private:


public:
	inline constexpr pthread_thread() noexcept = default;

	template <typename Func, typename... Args>
		requires(::std::invocable<Func, Args...>)
	inline constexpr pthread_thread(Func &&func, Args &&...args)
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
		auto start_routine = ::fast_io::posix::details::get_thread_start_routine<start_routine_tuple_type>(
			::std::make_index_sequence<sizeof...(Args) + 1>{});
		int ec{::fast_io::noexcept_call(::pthread_create, __builtin_addressof(this->id_), nullptr, start_routine, start_routine_tuple)};
		if (ec != 0) [[unlikely]]
		{
			// Creation failed; manual release is required.
			::std::destroy_at(reinterpret_cast<start_routine_tuple_type *>(start_routine_tuple));
			alloc::deallocate_n(start_routine_tuple, 1u);

			::fast_io::throw_posix_error(ec);
		}
		this->joinable_ = true;
	}

	inline constexpr pthread_thread(pthread_thread const &) noexcept = delete;

	inline constexpr pthread_thread(pthread_thread &&other) noexcept : id_{other.id_}, joinable_{other.joinable_}
	{
		other.id_ = 0;
		other.joinable_ = false;
	}

	inline constexpr ~pthread_thread() noexcept
	{
		if (this->joinable()) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
	}

	inline constexpr pthread_thread &operator=(pthread_thread const &) noexcept = delete;

	inline constexpr pthread_thread &operator=(pthread_thread &&other) noexcept
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
		return this->joinable_;
	}

	inline constexpr void join()
	{
		if (!this->joinable()) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		int ec{::fast_io::noexcept_call(::pthread_join, this->id_, nullptr)};
		if (ec != 0) [[unlikely]]
		{
			::fast_io::throw_posix_error(ec);
		}
		this->joinable_ = false;
	}

	inline constexpr void detach()
	{
		if (!this->joinable()) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		int ec{::fast_io::noexcept_call(::pthread_detach, this->id_)};
		if (ec != 0) [[unlikely]]
		{
			::fast_io::throw_posix_error(ec);
		}
		this->joinable_ = false;
	}

	inline constexpr void swap(pthread_thread &other) noexcept
	{
		::std::ranges::swap(this->id_, other.id_);
		::std::ranges::swap(this->joinable_, other.joinable_);
	}

	[[nodiscard]]
	inline constexpr auto get_id() const noexcept
	{
		return this->id_;
	}

	[[nodiscard]]
	inline constexpr auto native_handle() const noexcept
	{
		return this->id_;
	}

	[[nodiscard]]
	inline static constexpr ::std::uint_least32_t hardware_concurrency() noexcept
	{
#ifdef _SC_NPROCESSORS_ONLN
		long v = ::fast_io::noexcept_call(::sysconf, _SC_NPROCESSORS_ONLN);
		if (v <= 0)
		{
			return 1u;
		}
		return static_cast<::std::uint_least32_t>(v);
#else
		return 0u;
#endif
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
::fast_io::posix::pthread_thread::id get_id() noexcept
{
	return ::fast_io::noexcept_call(::pthread_self);
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
	auto const ns64{::std::chrono::duration_cast<::std::chrono::nanoseconds>(sleep_duration).count()};
	if (ns64 <= 0)
	{
		return;
	}
	::timespec req{};
	req.tv_sec = static_cast<::time_t>(ns64 / 1'000'000'000LL);
	req.tv_nsec = static_cast<long>(ns64 % 1'000'000'000LL);
	(void)::fast_io::noexcept_call(::nanosleep, __builtin_addressof(req), nullptr);
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
		auto const ns64{::std::chrono::duration_cast<::std::chrono::nanoseconds>(expect_time - now).count()};
		if (ns64 <= 0)
		{
			return;
		}
		::timespec delta{};
		delta.tv_sec = static_cast<::time_t>(ns64 / 1'000'000'000LL);
		delta.tv_nsec = static_cast<long>(ns64 % 1'000'000'000LL);
#if defined(CLOCK_REALTIME) && defined(TIMER_ABSTIME)
		::timespec abs_ts{};
		(void)::fast_io::noexcept_call(::clock_gettime, CLOCK_REALTIME, __builtin_addressof(abs_ts));
		abs_ts.tv_sec += delta.tv_sec;
		abs_ts.tv_nsec += delta.tv_nsec;
		if (abs_ts.tv_nsec >= 1'000'000'000L)
		{
			abs_ts.tv_nsec -= 1'000'000'000L;
			++abs_ts.tv_sec;
		}
		::fast_io::noexcept_call(::clock_nanosleep, CLOCK_REALTIME, TIMER_ABSTIME, __builtin_addressof(abs_ts), nullptr);
#else
		::fast_io::noexcept_call(::nanosleep, __builtin_addressof(delta), nullptr);
#endif
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

	::timespec req{};
	req.tv_sec = static_cast<::time_t>(sleep_duration.seconds);
	constexpr ::std::uint_least64_t mul_factor{::fast_io::uint_least64_subseconds_per_second / 1000000000u};
	req.tv_nsec = static_cast<long>(sleep_duration.subseconds / mul_factor);

	if (req.tv_sec == 0 && req.tv_nsec <= 0)
	{
		return;
	}
	
    ::fast_io::noexcept_call(::nanosleep, __builtin_addressof(req), nullptr);
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
	if (expect_time.seconds < 0)
	{
		return;
	}

	::timespec ts{};

    auto const unix_ts{static_cast<::fast_io::unix_timestamp>(expect_time)};
	ts.tv_sec = static_cast<::time_t>(unix_ts.seconds);
	constexpr ::std::uint_least64_t mul_factor{::fast_io::uint_least64_subseconds_per_second / 1000000000u};
	ts.tv_nsec = static_cast<long>(unix_ts.subseconds / mul_factor);

#if defined(CLOCK_REALTIME) && defined(TIMER_ABSTIME)
	::fast_io::noexcept_call(::clock_nanosleep, CLOCK_REALTIME, TIMER_ABSTIME, __builtin_addressof(ts), nullptr);
#else
	::timespec now{};
	::fast_io::noexcept_call(::clock_gettime, CLOCK_REALTIME, __builtin_addressof(now));
	
    if ((ts.tv_sec < now.tv_sec) || (ts.tv_sec == now.tv_sec && ts.tv_nsec <= now.tv_nsec))
	{
		return;
	}
	::timespec delta{};
	delta.tv_sec = ts.tv_sec - now.tv_sec;
	delta.tv_nsec = ts.tv_nsec - now.tv_nsec;
	if (delta.tv_nsec < 0)
	{
		delta.tv_nsec += 1'000'000'000L;
		delta.tv_sec -= 1;
	}
	
    ::fast_io::noexcept_call(::nanosleep, __builtin_addressof(delta), nullptr);
#endif
}

} // namespace this_thread

} // namespace posix

using pthread_thread = posix::pthread_thread;

#if !((defined(_WIN32) && !defined(__WINE__)) && !defined(__CYGWIN__)) && \
	!defined(__SINGLE_THREAD__) && !defined(__MSDOS__) && !defined(__wasi__) && __has_include(<pthread.h>)
using native_thread = pthread_thread;
namespace this_thread
{
using ::fast_io::posix::this_thread::get_id;
using ::fast_io::posix::this_thread::sleep_for;
using ::fast_io::posix::this_thread::sleep_until;
} // namespace this_thread
#endif

} // namespace fast_io