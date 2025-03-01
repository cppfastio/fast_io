#pragma once

namespace fast_io
{
enum process_mode : ::std::uint_least64_t
{
	none = 0,
	// *indicates that the process mode has not been evaluated yet
	new_session = static_cast<::std::uint_least64_t>(1) << 0,
	// [POSIX] setsid(), [WINDOWS, WINNT] CREATE_NEW_PROCESS_GROUP (Windows is currently not implemented)
	alloc_new_console = static_cast<::std::uint_least64_t>(1) << 1
	// [WINDOWS, WINNT] CREATE_NEW_CONSOLE (Automatically assign a console to new threads)
};

inline constexpr process_mode operator&(process_mode x, process_mode y) noexcept
{
	using utype = typename ::std::underlying_type<process_mode>::type;
	return static_cast<process_mode>(static_cast<utype>(x) & static_cast<utype>(y));
}

inline constexpr process_mode operator|(process_mode x, process_mode y) noexcept
{
	using utype = typename ::std::underlying_type<process_mode>::type;
	return static_cast<process_mode>(static_cast<utype>(x) | static_cast<utype>(y));
}

inline constexpr process_mode operator^(process_mode x, process_mode y) noexcept
{
	using utype = typename ::std::underlying_type<process_mode>::type;
	return static_cast<process_mode>(static_cast<utype>(x) ^ static_cast<utype>(y));
}

inline constexpr process_mode operator~(process_mode x) noexcept
{
	using utype = typename ::std::underlying_type<process_mode>::type;
	return static_cast<process_mode>(~static_cast<utype>(x));
}

inline constexpr process_mode &operator&=(process_mode &x, process_mode y) noexcept
{
	return x = x & y;
}

inline constexpr process_mode &operator|=(process_mode &x, process_mode y) noexcept
{
	return x = x | y;
}

inline constexpr process_mode &operator^=(process_mode &x, process_mode y) noexcept
{
	return x = x ^ y;
}

}
