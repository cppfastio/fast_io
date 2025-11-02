#pragma once

#include <chrono>
#include <ranges>
#include <cstdint>
#include <utility>
#include <functional>
#include <type_traits>
#include "../../../fast_io_dsal/tuple.h"
#include "../../../fast_io_core_impl/allocation/common.h"

namespace fast_io::win32::nt
{

namespace details
{

class nt_thread_start_routine_tuple_allocate_guard
{
public:
	void *ptr_{nullptr};

	constexpr nt_thread_start_routine_tuple_allocate_guard() = default;

	constexpr nt_thread_start_routine_tuple_allocate_guard(void *ptr) : ptr_{ptr}
	{}

	constexpr nt_thread_start_routine_tuple_allocate_guard(nt_thread_start_routine_tuple_allocate_guard const &) noexcept = delete;
	constexpr nt_thread_start_routine_tuple_allocate_guard(nt_thread_start_routine_tuple_allocate_guard &&other) noexcept = default;

	constexpr ~nt_thread_start_routine_tuple_allocate_guard()
	{
		if (ptr_ != nullptr)
		{
			::fast_io::generic_allocator_adapter<::fast_io::nt_rtlallocateheap_allocator>::deallocate(this->ptr_);
		}
	}
};

template <typename Tuple, ::std::size_t... Is>
constexpr ::std::uint_least32_t thread_start_routine(void *args) noexcept(noexcept(
	::std::invoke(::fast_io::get<Is>(*reinterpret_cast<Tuple *>(args))...)))
{
	[[maybe_unused]] ::fast_io::win32::nt::details::nt_thread_start_routine_tuple_allocate_guard _(args);
	::std::invoke(::fast_io::get<Is>(*reinterpret_cast<Tuple *>(args))...);
	return 0;
}

template <typename Tuple, ::std::size_t... Is>
constexpr auto get_thread_start_routine(::std::index_sequence<Is...>) noexcept
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
	constexpr nt_thread() noexcept = default;

	template <typename Func, typename... Args>
		requires(::std::invocable<Func, Args...>)
	constexpr nt_thread(Func &&func, Args &&...args)
	{
		using start_routine_tuple_type = ::fast_io::tuple<decltype(func), decltype(args)...>;
#if __has_cpp_attribute(indeterminate)
		::fast_io::win32::nt::client_id cid [[indeterminate]];
#else
		::fast_io::win32::nt::client_id cid;
#endif
		void *start_routine_tuple{::fast_io::generic_allocator_adapter<::fast_io::nt_rtlallocateheap_allocator>::allocate(sizeof(start_routine_tuple_type))};
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
			reinterpret_cast<void *>(start_routine),
			start_routine_tuple, // args of func
			__builtin_addressof(this->handle_),
			__builtin_addressof(cid))};
		if (status) [[unlikely]]
		{
			::fast_io::throw_nt_error(status);
		}
		this->id_ = cid.UniqueThread;
	}

	constexpr nt_thread(nt_thread const &) noexcept = delete;

	constexpr nt_thread(nt_thread &&other) noexcept = default;

	constexpr ~nt_thread() noexcept
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

	constexpr nt_thread &operator=(nt_thread const &) noexcept = delete;

	constexpr nt_thread &operator=(nt_thread &&other) noexcept
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
		return this->id_ != nullptr;
	}

	constexpr void join()
	{
		if (!this->joinable()) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		auto status{::fast_io::win32::nt::nt_wait_for_single_object<zw>(this->handle_, /* INFINITE = */ int(0xffffffff), nullptr)};
		if (status) [[unlikely]]
		{
			::fast_io::throw_nt_error(status);
		}
		this->id_ = nullptr;
	}

	constexpr void detach()
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

	constexpr void swap(nt_thread &other) noexcept
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

	/**
	 * @brief Get the win32 id of the thread.
	 * @note same as win32 GetCurrentThreadId
	 */
	constexpr ::std::uint_least32_t get_win32_id() const noexcept
	{
		auto peb = ::fast_io::win32::nt::nt_current_teb();
		return static_cast<::std::uint_least32_t>(reinterpret_cast<::std::size_t>(peb->ClientId.UniqueThread));
	}

	static constexpr ::std::uint_least32_t hardware_concurrency()
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
constexpr auto get_id() -> ::fast_io::win32::nt::nt_thread<zw>::id
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

template <bool zw = false, typename Rep, typename Period>
constexpr void sleep_for(::std::chrono::duration<Rep, Period> const &sleep_duration)
{
	auto val = -static_cast<::std::int_least64_t>(::std::chrono::duration_cast<::std::chrono::microseconds>(sleep_duration).count() * 10);
	::std::uint_least32_t status{::fast_io::win32::nt::nt_delay_execution<zw>(false, __builtin_addressof(val))};
	if (status) [[unlikely]]
	{
		::fast_io::throw_nt_error(status);
	}
}

template <bool zw = false, typename Clock, typename Duration>
constexpr void sleep_until(::std::chrono::time_point<Clock, Duration> const &expect_time)
{
	auto const unix_ts = ::std::chrono::duration_cast<std::chrono::seconds>(
					   expect_time.time_since_epoch())
					   .count();
	auto nt_ts = (unix_ts + 11644473600) * 10000000;
	::std::uint_least32_t status{::fast_io::win32::nt::nt_delay_execution<zw>(false, __builtin_addressof(nt_ts))};
	if (status) [[unlikely]]
	{
		::fast_io::throw_nt_error(status);
	}
}

} // namespace this_thread

} // namespace fast_io::win32::nt
