#pragma once

namespace fast_io
{

enum class utime_flags : ::std::uint_fast8_t
{
	none,
	now,
	omit
};

struct unix_timestamp_option
{
	utime_flags flags{utime_flags::omit};
	unix_timestamp timestamp{};
	inline constexpr unix_timestamp_option() noexcept = default;
	inline constexpr unix_timestamp_option(unix_timestamp ts) noexcept
		: flags(utime_flags::none), timestamp(ts)
	{}
	inline constexpr unix_timestamp_option(utime_flags fg) noexcept
		: flags(fg)
	{}
};

} // namespace fast_io
