#pragma once

namespace fast_io
{

namespace win32::details
{

inline ::fast_io::intfpos_t seek_impl(void *handle, ::fast_io::intfpos_t offset, seekdir s)
	FAST_IO_HERBCEPTIONS_THROWS
{
#if (defined(_WIN32_WINNT) && _WIN32_WINNT <= 0x0500) || defined(_WIN32_WINDOWS)
	if constexpr (sizeof(::fast_io::intfpos_t) > sizeof(::std::int_least32_t))
	{
		constexpr ::fast_io::intfpos_t l32mx{INT_LEAST32_MAX};
		constexpr ::fast_io::intfpos_t l32mn{INT_LEAST32_MIN};
		if (offset > l32mx || offset < l32mn)
		{
			throw_win32_error(0x00000057);
		}
	}

	::std::int_least32_t distance_to_move_high{};
	constexpr ::std::uint_least32_t invalid{UINT_LEAST32_MAX};
	auto const low{::fast_io::win32::SetFilePointer(handle, static_cast<::std::int_least32_t>(offset),
													__builtin_addressof(distance_to_move_high), static_cast<::std::uint_least32_t>(s))};

	if (low == invalid) [[unlikely]]
	{
		auto const err{::fast_io::win32::GetLastError()};
		if (err != 0u) [[unlikely]]
		{
			throw_win32_error(err);
		}
	}

	::std::uint_least64_t const combined{(static_cast<::std::uint_least64_t>(static_cast<::std::uint_least32_t>(distance_to_move_high)) << 32u) | static_cast<::std::uint_least32_t>(low)};
	return static_cast<::fast_io::intfpos_t>(combined);
#else
	if constexpr (sizeof(::fast_io::intfpos_t) > sizeof(::std::int_least64_t))
	{
		constexpr ::fast_io::intfpos_t l64mx{INT_LEAST64_MAX};
		constexpr ::fast_io::intfpos_t l64mn{INT_LEAST64_MIN};
		if (offset > l64mx || offset < l64mn)
		{
			throw_win32_error(0x00000057);
		}
	}
	::std::int_least64_t distance_full{};
	if (!::fast_io::win32::SetFilePointerEx(handle, static_cast<::std::int_least64_t>(offset),
											__builtin_addressof(distance_full),
											static_cast<::std::uint_least32_t>(s)))
	{
		throw_win32_error();
	}
	return static_cast<::fast_io::intfpos_t>(distance_full);
#endif
}

} // namespace win32::details

template <win32_family family, ::std::integral ch_type>
inline ::fast_io::intfpos_t io_stream_seek_bytes_define(basic_win32_family_io_observer<family, ch_type> wiob,
														::fast_io::intfpos_t off, ::fast_io::seekdir sdir)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::details::seek_impl(wiob.handle, off, sdir);
}

} // namespace fast_io
