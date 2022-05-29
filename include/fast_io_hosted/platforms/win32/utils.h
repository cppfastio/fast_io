#pragma once

namespace fast_io::win32
{

constexpr unix_timestamp to_unix_timestamp(filetime ft) noexcept
{
	std::uint_least64_t date_time{(static_cast<std::uint_least64_t>(ft.dwHighDateTime)<<32)|ft.dwLowDateTime};

/*
116444736000000000
18446744073709551616
 999999999
1000000000
*/

	constexpr std::uint_least64_t gap{11644473600000ULL * 10000ULL};
	constexpr std::uint_least64_t mul_factor{uint_least64_subseconds_per_second/10000000u};
	std::uint_least64_t unix_time{date_time-gap};
	if(date_time<gap)[[unlikely]]
		unix_time=0;
	return {static_cast<std::int_least64_t>(unix_time/10000000ULL),static_cast<std::uint_least64_t>(unix_time%10000000ULL)*mul_factor};
}

inline constexpr win32_timestamp to_win32_timestamp_ftu64(std::uint_least64_t ftu64) noexcept
{
	std::uint_least64_t seconds{ftu64/10000000ULL};
	std::uint_least64_t subseconds{ftu64%10000000ULL};
	constexpr std::uint_least64_t mul_factor{uint_least64_subseconds_per_second/10000000u};
	return {static_cast<std::int_least64_t>(seconds),static_cast<std::uint_least64_t>(subseconds*mul_factor)};
}

inline constexpr std::uint_least64_t filetime_to_uint_least64_t(filetime ft) noexcept
{
	return (static_cast<std::uint_least64_t>(ft.dwHighDateTime)<<32)|ft.dwLowDateTime;
}

inline constexpr win32_timestamp to_win32_timestamp(filetime ft) noexcept
{
	return to_win32_timestamp_ftu64((static_cast<std::uint_least64_t>(ft.dwHighDateTime)<<32)|ft.dwLowDateTime);
}

}