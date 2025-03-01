#pragma once
namespace fast_io
{
enum class ipc_mode : ::std::uint_least64_t
{
	none = 0,
	//	*indicates that the open mode has not been evaluated yet
	in = static_cast<::std::uint_least64_t>(1) << 0,
	//	*["r"]open for reading or receiving.
	out = static_cast<::std::uint_least64_t>(1) << 1,
	//	*["w"]open for writing or sending.
	sync = static_cast<::std::uint_least64_t>(1) << 2,
	//	*POSIX O_SYNC
	no_block = static_cast<::std::uint_least64_t>(1) << 3,
	//	*POSIX O_NONBLOCK
	message = static_cast<::std::uint_least64_t>(1) << 4,
	// Data is written as a message stream into the pipeline.
	// The pipeline considers the bytes written during each write operation as message units.
	huge_page_2M = static_cast<::std::uint_least64_t>(1) << 5,
	huge_page_1G = static_cast<::std::uint_least64_t>(1) << 6,
};

inline constexpr ipc_mode operator&(ipc_mode x, ipc_mode y) noexcept
{
	using utype = typename ::std::underlying_type<ipc_mode>::type;
	return static_cast<ipc_mode>(static_cast<utype>(x) & static_cast<utype>(y));
}

inline constexpr ipc_mode operator|(ipc_mode x, ipc_mode y) noexcept
{
	using utype = typename ::std::underlying_type<ipc_mode>::type;
	return static_cast<ipc_mode>(static_cast<utype>(x) | static_cast<utype>(y));
}

inline constexpr ipc_mode operator^(ipc_mode x, ipc_mode y) noexcept
{
	using utype = typename ::std::underlying_type<ipc_mode>::type;
	return static_cast<ipc_mode>(static_cast<utype>(x) ^ static_cast<utype>(y));
}

inline constexpr ipc_mode operator~(ipc_mode x) noexcept
{
	using utype = typename ::std::underlying_type<ipc_mode>::type;
	return static_cast<ipc_mode>(~static_cast<utype>(x));
}

inline constexpr ipc_mode &operator&=(ipc_mode &x, ipc_mode y) noexcept
{
	return x = x & y;
}

inline constexpr ipc_mode &operator|=(ipc_mode &x, ipc_mode y) noexcept
{
	return x = x | y;
}

inline constexpr ipc_mode &operator^=(ipc_mode &x, ipc_mode y) noexcept
{
	return x = x ^ y;
}

}
