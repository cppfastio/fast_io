#pragma once

namespace fast_io
{

enum class utime_flags : ::std::uint_fast8_t
{
	none,
	now,
	omit
};

struct statx_timestamp_option
{
	utime_flags flags{utime_flags::omit};
	posix_statx_timestamp64 timestamp{};
	inline constexpr statx_timestamp_option() noexcept = default;
	inline constexpr statx_timestamp_option(posix_statx_timestamp64 ts) noexcept
		: flags(utime_flags::none), timestamp(ts)
	{}
	inline constexpr statx_timestamp_option(utime_flags fg) noexcept
		: flags(fg)
	{}
};

} // namespace fast_io
