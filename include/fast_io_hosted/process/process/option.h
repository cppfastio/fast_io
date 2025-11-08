#pragma once

namespace fast_io
{
enum class process_mode : ::std::uint_least64_t
{
	none = 0,
	// *indicates that the process mode has not been evaluated yet
	new_session = static_cast<::std::uint_least64_t>(1) << 0,
	// [POSIX] setsid(), [WINDOWS, WINNT] CREATE_NEW_PROCESS_GROUP (Windows is currently not implemented)
	// POSIX: This parameter is only valid under fork; vfork does not support it.
	alloc_new_console = static_cast<::std::uint_least64_t>(1) << 1,
	// [WINDOWS, WINNT] CREATE_NEW_CONSOLE (Automatically assign a console to new threads)
	argv0_no_path_append = static_cast<::std::uint_least64_t>(1) << 2,
	// [POSIX, WINDOWS, WINNT] Do not automatically append appname to argv0
	posix_vfork = static_cast<::std::uint_least64_t>(1) << 3,
	// [POSIX] Using vfork to create processes, but this prevents many parameters from taking effect (vfork does not allow modification of global memory before exec).
	follow = static_cast<::std::uint_least64_t>(1) << 4,
	// [POSIX, WINDOWS, WINNT] Allow symbolic links to follow
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
