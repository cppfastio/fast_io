#pragma once

namespace fast_io
{
namespace details::posix
{
extern unsigned my_usleep(unsigned usec) noexcept __asm__("_usleep");
extern int sched_yield() noexcept __asm__("_sched_yield");
} // namespace details::posix

namespace newlib::this_thread
{

using dos_thread_id = unsigned;

inline constexpr dos_thread_id get_id() noexcept
{
	return 0u;
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

	constexpr ::std::uint_least64_t mul_factor{::fast_io::uint_least64_subseconds_per_second / 1000000u};

	auto const us64{
		static_cast<::std::uint_least64_t>(static_cast<::std::uint_least64_t>(sleep_duration.seconds) * 1000000u +
										   sleep_duration.subseconds / mul_factor)};
	if (us64 == 0)
	{
		return;
	}

	auto remaining{us64};
	constexpr ::std::uint_least64_t max_chunk{static_cast<::std::uint_least64_t>(static_cast<unsigned>(-1))};
	while (remaining != 0)
	{
		auto const chunk{remaining > max_chunk ? max_chunk : remaining};
		::fast_io::details::posix::my_usleep(static_cast<unsigned>(chunk));
		remaining -= chunk;
	}
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

	auto const unix_expect{static_cast<::fast_io::unix_timestamp>(expect_time)};
	auto const now{::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)};
	if (!(now < unix_expect))
	{
		return;
	}

	auto const delta{unix_expect - now};
	::fast_io::newlib::this_thread::sleep_for(delta);
}

inline
#if __cpp_constexpr >= 202207L
	// https://en.cppreference.com/w/cpp/compiler_support/23.html#cpp_constexpr_202207L
	// for reduce some warning purpose
	constexpr
#endif
	void yield() noexcept
{
	::fast_io::details::posix::sched_yield();
}

} // namespace newlib::this_thread

#if defined(__NEWLIB__)

namespace this_thread
{
using ::fast_io::newlib::this_thread::get_id;
using ::fast_io::newlib::this_thread::sleep_for;
using ::fast_io::newlib::this_thread::sleep_until;
using ::fast_io::newlib::this_thread::yield;
} // namespace this_thread
#endif

} // namespace fast_io
