#pragma once

#include <chrono>
#include <ranges>
#include <cstdint>
#include <utility>
#include <functional>
#include <type_traits>

namespace fast_io
{
namespace win32::nt
{

namespace details
{

template <typename Tuple>
class nt_thread_start_routine_tuple_allocate_guard
{
public:
	void *ptr_{nullptr};

	inline constexpr nt_thread_start_routine_tuple_allocate_guard() = default;

	inline constexpr nt_thread_start_routine_tuple_allocate_guard(void *ptr)
		: ptr_{ptr}
	{}

	inline constexpr nt_thread_start_routine_tuple_allocate_guard(nt_thread_start_routine_tuple_allocate_guard const &) noexcept = delete;
	inline constexpr nt_thread_start_routine_tuple_allocate_guard(nt_thread_start_routine_tuple_allocate_guard &&other) noexcept = delete;
	inline constexpr nt_thread_start_routine_tuple_allocate_guard &operator=(nt_thread_start_routine_tuple_allocate_guard const &) noexcept = delete;
	inline constexpr nt_thread_start_routine_tuple_allocate_guard &operator=(nt_thread_start_routine_tuple_allocate_guard &&other) noexcept = delete;

	inline constexpr ~nt_thread_start_routine_tuple_allocate_guard()
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

	[[maybe_unused]] ::fast_io::win32::nt::details::nt_thread_start_routine_tuple_allocate_guard<Tuple> _(args);

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
	return ::fast_io::win32::nt::details::thread_start_routine<Tuple, Is...>;
}

} // namespace details

template <bool zw>
class nt_thread
{
public:
	using id = void *;
	using native_handle_type = void *;

private:
	id id_{nullptr};
	native_handle_type handle_{nullptr};

public:
	inline constexpr nt_thread() noexcept = default;

	template <typename Func, typename... Args>
		requires(::std::invocable<Func, Args...>)
	inline constexpr nt_thread(Func &&func, Args &&...args)
	{
		using start_routine_tuple_type = ::fast_io::containers::tuple<::std::decay_t<Func>, ::std::decay_t<Args>...>;
		using alloc = ::fast_io::native_typed_global_allocator<start_routine_tuple_type>;
#if __has_cpp_attribute(indeterminate)
		::fast_io::win32::nt::client_id cid [[indeterminate]];
#else
		::fast_io::win32::nt::client_id cid;
#endif
		auto start_routine_tuple{alloc::allocate(1u)};
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif
		::new (start_routine_tuple) start_routine_tuple_type{::std::forward<Func>(func), ::std::forward<Args>(args)...};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
		auto start_routine = ::fast_io::win32::nt::details::get_thread_start_routine<start_routine_tuple_type>(
			::std::make_index_sequence<sizeof...(Args) + 1>{});
		::std::uint_least32_t status{::fast_io::win32::nt::RtlCreateUserThread(
			reinterpret_cast<void *>(-1), // Result of GetCurrentProcess()
			nullptr,                      // SecurityDescriptor
			false,                        // no suspand
			0,                            // StackZeroBits
			0,                            // StackReserved
			0,                            // StackCommit
			start_routine,
			start_routine_tuple, // args of func
			__builtin_addressof(this->handle_),
			__builtin_addressof(cid))};

		if (status) [[unlikely]]
		{
			// Creation failed; manual release is required.
			::std::destroy_at(reinterpret_cast<start_routine_tuple_type *>(start_routine_tuple));
			alloc::deallocate_n(start_routine_tuple, 1u);

			::fast_io::throw_nt_error(status);
		}

		this->id_ = cid.UniqueThread;
	}

	inline constexpr nt_thread(nt_thread const &) noexcept = delete;

	inline constexpr nt_thread(nt_thread &&other) noexcept : id_{other.id_}, handle_{other.handle_}
	{
		other.id_ = 0;
		other.handle_ = nullptr;
	}

	inline constexpr ~nt_thread() noexcept
	{
		if (handle_ != nullptr)
		{
			if (this->joinable()) [[unlikely]]
			{
				::fast_io::fast_terminate();
			}
			auto status{::fast_io::win32::nt::nt_close<zw>(this->handle_)};
			if (status) [[unlikely]]
			{
				::fast_io::fast_terminate();
			}
		}
	}

	inline constexpr nt_thread &operator=(nt_thread const &) noexcept = delete;

	inline constexpr nt_thread &operator=(nt_thread &&other) noexcept
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
		return this->id_ != nullptr;
	}

	inline constexpr void join()
	{
		if (!this->joinable()) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		auto status{::fast_io::win32::nt::nt_wait_for_single_object<zw>(this->handle_, false, nullptr)};
		if (status) [[unlikely]]
		{
			::fast_io::throw_nt_error(status);
		}
		status = ::fast_io::win32::nt::nt_close<zw>(this->handle_);
		if (status) [[unlikely]]
		{
			::fast_io::throw_nt_error(status);
		}
		this->handle_ = nullptr;
		this->id_ = nullptr;
	}

	inline constexpr void detach()
	{
		if (!this->joinable()) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		auto status{::fast_io::win32::nt::nt_close<zw>(this->handle_)};
		if (status) [[unlikely]]
		{
			::fast_io::throw_nt_error(status);
		}
		this->handle_ = nullptr;
		this->id_ = nullptr;
	}

	inline constexpr void swap(nt_thread &other) noexcept
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

	/**
	 * @brief Get the win32 id of the thread.
	 * @note same as win32 GetCurrentThreadId
	 */
	inline constexpr ::std::uint_least32_t get_win32_id() const noexcept
	{
		::fast_io::win32::nt::thread_basic_information tbi; // no init

		auto status{::fast_io::win32::nt::nt_query_information_thread<zw>(
			this->handle_, ::fast_io::win32::nt::thread_information_class::ThreadBasicInformation, __builtin_addressof(tbi), sizeof(tbi), nullptr)};

		if (status) [[unlikely]]
		{
			::fast_io::throw_nt_error(status);
		}

		return static_cast<::std::uint_least32_t>(reinterpret_cast<::std::size_t>(tbi.ClientId.UniqueThread));
	}

	inline static constexpr ::std::uint_least32_t hardware_concurrency()
	{
		::fast_io::win32::nt::system_basic_information sb{};
		auto status{::fast_io::win32::nt::nt_query_system_information<zw>(::fast_io::win32::nt::system_information_class::SystemBasicInformation,
																		  __builtin_addressof(sb), static_cast<::std::uint_least32_t>(sizeof(sb)), nullptr)};
		if (status) [[unlikely]]
		{
			::fast_io::throw_nt_error(status);
		}
		return static_cast<::std::uint_least32_t>(sb.NumberOfProcessors);
	}
};

namespace this_thread
{

template <bool zw = false>
inline
#if __cpp_constexpr >= 202207L
	// https://en.cppreference.com/w/cpp/compiler_support/23.html#cpp_constexpr_202207L
	// for reduce some warning purpose
	constexpr
#endif
	::fast_io::win32::nt::nt_thread<zw>::id get_id()
{
	::fast_io::win32::nt::thread_basic_information tbi;
	::std::uint_least32_t status{::fast_io::win32::nt::nt_query_information_thread<zw>(
		reinterpret_cast<void *>(-2), // NtCurrentThread
		::fast_io::win32::nt::thread_information_class::ThreadBasicInformation,
		__builtin_addressof(tbi),
		::std::uint_least32_t(sizeof(tbi)),
		nullptr)};
	if (status) [[unlikely]]
	{
		::fast_io::throw_nt_error(status);
	}
	return tbi.ClientId.UniqueThread;
}

inline
#if __cpp_constexpr >= 202207L
	// https://en.cppreference.com/w/cpp/compiler_support/23.html#cpp_constexpr_202207L
	// for reduce some warning purpose
	constexpr
#endif
	::std::uint_least32_t get_win32_id() noexcept
{
	auto teb{::fast_io::win32::nt::nt_current_teb()};
	return static_cast<::std::uint_least32_t>(reinterpret_cast<::std::size_t>(teb->ClientId.UniqueThread));
}

template <bool zw = false, typename Rep, typename Period>
inline
#if __cpp_constexpr >= 202207L
	// https://en.cppreference.com/w/cpp/compiler_support/23.html#cpp_constexpr_202207L
	// for reduce some warning purpose
	constexpr
#endif
	void sleep_for(::std::chrono::duration<Rep, Period> const &sleep_duration)
{
	auto const count{static_cast<::std::uint_least64_t>(
		::std::chrono::duration_cast<::std::chrono::microseconds>(sleep_duration).count() * 10u +
		::std::chrono::duration_cast<::std::chrono::nanoseconds>(sleep_duration).count() / 100u % 10u)};

	if (count > static_cast<::std::uint_least64_t>(::std::numeric_limits<::std::int_least64_t>::max()))
	{
		::fast_io::throw_nt_error(0xC000000D);
	}

	auto val{-static_cast<::std::int_least64_t>(count)};
	::std::uint_least32_t status{::fast_io::win32::nt::nt_delay_execution<zw>(false, __builtin_addressof(val))};
	if (status) [[unlikely]]
	{
		::fast_io::throw_nt_error(status);
	}
}

template <bool zw = false, ::std::int_least64_t off_to_epoch>
inline
#if __cpp_constexpr >= 202207L
	// https://en.cppreference.com/w/cpp/compiler_support/23.html#cpp_constexpr_202207L
	// for reduce some warning purpose
	constexpr
#endif
	void sleep_for(::fast_io::basic_timestamp<off_to_epoch> const &sleep_duration)
{
	if (sleep_duration.seconds < 0) [[unlikely]]
	{
		::fast_io::throw_nt_error(0xC000000D);
	}

	constexpr ::std::uint_least64_t mul_factor{::fast_io::uint_least64_subseconds_per_second / 1000000000u};

	auto const win_100ns_seconds{static_cast<::std::uint_least64_t>(static_cast<::std::uint_least64_t>(sleep_duration.seconds) * 10'000'000u + sleep_duration.subseconds / mul_factor / 100u)};

	if (win_100ns_seconds > static_cast<::std::uint_least64_t>(::std::numeric_limits<::std::int_least64_t>::max())) [[unlikely]]
	{
		::fast_io::throw_nt_error(0xC000000D);
	}

	auto val = -static_cast<::std::int_least64_t>(win_100ns_seconds);
	::std::uint_least32_t status{::fast_io::win32::nt::nt_delay_execution<zw>(false, __builtin_addressof(val))};
	if (status) [[unlikely]]
	{
		::fast_io::throw_nt_error(status);
	}
}

template <bool zw = false, typename Clock, typename Duration>
inline
#if __cpp_constexpr >= 202207L
	// https://en.cppreference.com/w/cpp/compiler_support/23.html#cpp_constexpr_202207L
	// for reduce some warning purpose
	constexpr
#endif
	void sleep_until(::std::chrono::time_point<Clock, Duration> const &expect_time)
{
	auto const unix_ts = ::std::chrono::duration_cast<std::chrono::seconds>(
							 expect_time.time_since_epoch())
							 .count();
	auto const unix_subsec_ts = ::std::chrono::duration_cast<std::chrono::nanoseconds>(
									expect_time.time_since_epoch())
									.count() /
								100u % 10000000u;

	auto const unix_to_nt_secs{static_cast<::std::int_least64_t>(unix_ts) + 11644473600};
	if (unix_to_nt_secs < 0) [[unlikely]]
	{
		::fast_io::throw_nt_error(0xC000000D);
	}

	auto const count{static_cast<::std::uint_least64_t>(unix_to_nt_secs) * 10000000u + static_cast<::std::uint_least64_t>(unix_subsec_ts)};

	if (count > static_cast<::std::uint_least64_t>(::std::numeric_limits<::std::int_least64_t>::max()))
	{
		::fast_io::throw_nt_error(0xC000000D);
	}

	auto nt_ts{static_cast<::std::int_least64_t>(count)};
	::std::uint_least32_t status{::fast_io::win32::nt::nt_delay_execution<zw>(false, __builtin_addressof(nt_ts))};
	if (status) [[unlikely]]
	{
		::fast_io::throw_nt_error(status);
	}
}

template <bool zw = false, ::std::int_least64_t off_to_epoch>
inline
#if __cpp_constexpr >= 202207L
	// https://en.cppreference.com/w/cpp/compiler_support/23.html#cpp_constexpr_202207L
	// for reduce some warning purpose
	constexpr
#endif
	void sleep_until(::fast_io::basic_timestamp<off_to_epoch> const &expect_time)
{
	if (expect_time.seconds < 0) [[unlikely]]
	{
		::fast_io::throw_nt_error(0xC000000D);
	}

	auto const win32_ts{static_cast<::fast_io::win32_timestamp>(expect_time)};

	constexpr ::std::uint_least64_t mul_factor{::fast_io::uint_least64_subseconds_per_second / 1000000000u};
	auto const win_100ns_seconds = static_cast<::std::uint_least64_t>(win32_ts.seconds) * 10'000'000u + win32_ts.subseconds / mul_factor / 100u;

	if (win_100ns_seconds > static_cast<::std::uint_least64_t>(::std::numeric_limits<::std::int_least64_t>::max())) [[unlikely]]
	{
		::fast_io::throw_nt_error(0xC000000D);
	}

	auto val = static_cast<::std::int_least64_t>(win_100ns_seconds);
	::std::uint_least32_t status{::fast_io::win32::nt::nt_delay_execution<zw>(false, __builtin_addressof(val))};
	if (status) [[unlikely]]
	{
		::fast_io::throw_nt_error(status);
	}
}

} // namespace this_thread

} // namespace win32::nt

using nt_thread = win32::nt::nt_thread<false>;
using zw_thread = win32::nt::nt_thread<true>;

#if ((defined(_WIN32) && !defined(__WINE__)) && !defined(__CYGWIN__)) && !defined(_WIN32_WINDOWS)
using native_thread = nt_thread;

namespace this_thread
{
using ::fast_io::win32::nt::this_thread::get_id;
using ::fast_io::win32::nt::this_thread::sleep_for;
using ::fast_io::win32::nt::this_thread::sleep_until;
} // namespace this_thread
#endif

} // namespace fast_io
