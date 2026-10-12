#pragma once

namespace fast_io::win32
{

inline constexpr posix_statx_timestamp64 to_unix_timestamp(filetime ft) noexcept
{
	::std::uint_least64_t date_time{(static_cast<::std::uint_least64_t>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime};

	/*
	116444736000000000
	18446744073709551616
	 999999999
	1000000000
	*/

	constexpr ::std::uint_least64_t gap{11644473600000ULL * 10000ULL};
	::std::uint_least64_t unix_time{date_time - gap};
	if (date_time < gap) [[unlikely]]
	{
		unix_time = 0;
	}
	return {static_cast<::std::int_least64_t>(unix_time / 10000000ULL),
			static_cast<::std::uint_least32_t>(unix_time % 10000000ULL) * 100u};
}

inline constexpr ::std::uint_least64_t filetime_to_uint_least64_t(filetime ft) noexcept
{
	return (static_cast<::std::uint_least64_t>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
}

inline constexpr filetime unix_timestamp_to_filetime(posix_statx_timestamp64 wt) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::uint_least64_t ftu64{::fast_io::posix_statx_timestamp64_to_ftu64(wt)};
	return {.dwLowDateTime = static_cast<::std::uint_least32_t>(ftu64), .dwHighDateTime = static_cast<::std::uint_least32_t>(ftu64 >> 32)};
}
} // namespace fast_io::win32
