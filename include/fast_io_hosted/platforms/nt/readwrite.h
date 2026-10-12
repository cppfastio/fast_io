#pragma once

namespace fast_io::win32::nt::details
{

template <::fast_io::nt_family family>
inline ::std::uint_least64_t nt_calculate_current_file_offset(void *__restrict handle)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::nt::io_status_block block;
	::std::uint_least64_t fps{};
	auto status{::fast_io::win32::nt::nt_query_information_file<family == ::fast_io::nt_family::zw>(handle, __builtin_addressof(block), __builtin_addressof(fps),
																									static_cast<::std::uint_least32_t>(sizeof(::std::uint_least64_t)),
																									::fast_io::win32::nt::file_information_class::FilePositionInformation)};
	if (status)
	{
		::fast_io::herbceptions::throws_nt_errc_with_value(status);
	}
	return fps;
}

inline ::std::int_least64_t nt_calculate_offset_impl(::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if constexpr (sizeof(::fast_io::intfpos_t) > sizeof(::std::int_least64_t))
	{
		[[maybe_unused]] constexpr ::std::int_least64_t mn{::std::numeric_limits<::std::int_least64_t>::min()};
		constexpr ::std::int_least64_t mx{::std::numeric_limits<::std::int_least64_t>::max()};
		if (off < 0 || off > mx)
		{
			::fast_io::herbceptions::throws_nt_errc_with_value(0xC0000106);
		}
	}
	return static_cast<::std::int_least64_t>(off);
}

template <nt_family family>
inline ::std::byte *nt_read_pread_some_bytes_common_impl(void *__restrict handle, ::std::byte *first, ::std::size_t count,
														 ::std::int_least64_t *pbyteoffset)
	FAST_IO_HERBCEPTIONS_THROWS
{
	// some poeple in zwclose7 forum said we do not need to initialize io_status_block
	::fast_io::win32::nt::io_status_block block;
	auto const status{::fast_io::win32::nt::nt_read_file<family == ::fast_io::nt_family::zw>(handle, nullptr, nullptr, nullptr, __builtin_addressof(block), first,
																							 ::fast_io::details::read_write_bytes_compute<::std::uint_least32_t>(first, count), pbyteoffset, nullptr)};
	if (status) [[unlikely]]
	{
		if (status == 0xC0000011 /*file*/ || status == 0xC000014B /*pipe*/) [[likely]]
		{
			return first;
		}
		::fast_io::herbceptions::throws_nt_errc_with_value(status);
	}
	return first + block.Information;
}

template <nt_family family>
inline ::std::byte *nt_read_some_bytes_impl(void *__restrict handle, ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::nt::details::nt_read_pread_some_bytes_common_impl<family>(handle, first, count, nullptr);
}

template <nt_family family>
inline ::std::byte *nt_pread_some_bytes_impl(void *__restrict handle, ::std::byte *first, ::std::size_t count,
											 ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	// The difference between P-series functions in Windows synchronization mode and POSIX is that under Windows,
	// the functions will advance the position by the number of bytes written or read after each write/read operation.

	::std::int_least64_t offs{nt_calculate_offset_impl(off)};
	return ::fast_io::win32::nt::details::nt_read_pread_some_bytes_common_impl<family>(handle, first, count,
																					   __builtin_addressof(offs));
}

// The kernel only takes a per-call ByteOffset; it cannot advance an offset across the
// chunked syscalls the scatter emulation issues, so the offset accumulation and the
// pre-read overflow check live here. Negative ByteOffset values (-1/-2 special
// positions) pass through. A short return from the common impl is EOF, not a stall.
template <::fast_io::nt_family family>
inline ::std::byte *nt_read_pread_some_thunk(void *__restrict handle, ::std::byte *first, ::std::size_t count, ::std::int_least64_t *__restrict pbyteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::int_least64_t off{};
	if (pbyteoffset != nullptr && 0 <= (off = *pbyteoffset))
	{
		auto request{static_cast<::std::int_least64_t>(count)};
#if FAST_IO_HAS_BUILTIN(__builtin_add_overflow)
		::std::int_least64_t nxt;
		if (__builtin_add_overflow(off, request, __builtin_addressof(nxt))) [[unlikely]]
#else
		constexpr ::std::int_least64_t mx{::std::numeric_limits<::std::int_least64_t>::max()};
		if (mx - off < request) [[unlikely]]
#endif
		{
			::fast_io::herbceptions::throws_nt_errc_with_value(0xC0000095); // STATUS_INTEGER_OVERFLOW
		}
	}
	auto written{::fast_io::nt::details::nt_read_pread_some_bytes_common_impl<family>(
		handle, first, count, pbyteoffset == nullptr ? nullptr : __builtin_addressof(off))};
	if (pbyteoffset != nullptr && 0 <= off)
	{
		*pbyteoffset = off + static_cast<::std::int_least64_t>(written - first);
	}
	return written;
}

// read-all: retries until [first, last) is filled; a zero-progress round is EOF
// (the read common impl maps EOF statuses to a first-return), reported as
// STATUS_END_OF_FILE, the status the kernel returned before it was mapped away.
template <::fast_io::nt_family family>
inline void nt_read_pread_all_thunk(void *__restrict handle, ::std::byte *first, ::std::size_t count, ::std::int_least64_t *__restrict pbyteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	auto const e{first + count};
	while (first != e)
	{
		auto written{::fast_io::nt::details::nt_read_pread_some_thunk<family>(handle, first, static_cast<::std::size_t>(e - first), pbyteoffset)};
		if (written == first) [[unlikely]]
		{
			::fast_io::herbceptions::throws_nt_errc_with_value(0xC0000011); // STATUS_END_OF_FILE
		}
		first = written;
	}
}

template <nt_family family>
inline ::std::byte const *nt_write_pwrite_some_bytes_common_impl(void *__restrict handle, ::std::byte const *first, ::std::size_t count,
																 ::std::int_least64_t *pbyteoffset)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::nt::io_status_block block;
	auto const status{::fast_io::win32::nt::nt_write_file<family == ::fast_io::nt_family::zw>(handle, nullptr, nullptr, nullptr, __builtin_addressof(block), first,
																							  ::fast_io::details::read_write_bytes_compute<::std::uint_least32_t>(first, count), pbyteoffset, nullptr)};
	if (status)
	{
		::fast_io::herbceptions::throws_nt_errc_with_value(status);
	}
	return first + block.Information;
}


template <::fast_io::nt_family family>
inline ::std::byte const *nt_write_pwrite_some_thunk(void *__restrict handle, ::std::byte const *first, ::std::size_t count, ::std::int_least64_t *__restrict pbyteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::int_least64_t off{};
	if (pbyteoffset != nullptr && 0 <= (off = *pbyteoffset))
	{
		auto request{static_cast<::std::int_least64_t>(count)};
#if FAST_IO_HAS_BUILTIN(__builtin_add_overflow)
		::std::int_least64_t nxt;
		if (__builtin_add_overflow(off, request, __builtin_addressof(nxt))) [[unlikely]]
#else
		constexpr ::std::int_least64_t mx{::std::numeric_limits<::std::int_least64_t>::max()};
		if (mx - off < request) [[unlikely]]
#endif
		{
			::fast_io::herbceptions::throws_nt_errc_with_value(0xC0000095); // STATUS_INTEGER_OVERFLOW
		}
	}
	auto written{::fast_io::nt::details::nt_write_pwrite_some_bytes_common_impl<family>(
		handle, first, count, pbyteoffset == nullptr ? nullptr : __builtin_addressof(off))};
	if (pbyteoffset != nullptr && 0 <= off)
	{
		*pbyteoffset = off + static_cast<::std::int_least64_t>(written - first);
	}
	return written;
}

template <::fast_io::nt_family family>
inline void nt_write_pwrite_all_thunk(void *__restrict handle, ::std::byte const *first, ::std::size_t count, ::std::int_least64_t *__restrict pbyteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	auto const e{first + count};
	while (first != e)
	{
		auto written{::fast_io::nt::details::nt_write_pwrite_some_thunk<family>(handle, first, static_cast<::std::size_t>(e - first), pbyteoffset)};
		if (written == first) [[unlikely]]
		{
			::fast_io::herbceptions::throws_nt_errc_with_value(0xC0000185); // STATUS_IO_DEVICE_ERROR
		}
		first = written;
	}
}

template <::fast_io::nt_family family>
inline ::std::byte const *nt_write_some_bytes_impl(void *__restrict handle, ::std::byte const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::nt::details::nt_write_pwrite_some_bytes_common_impl<family>(handle, first, count, nullptr);
}

template <::fast_io::nt_family family>
inline ::std::byte const *nt_pwrite_some_bytes_impl(void *__restrict handle, ::std::byte const *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	// The difference between P-series functions in Windows synchronization mode and POSIX is that under Windows,
	// the functions will advance the position by the number of bytes written or read after each write/read operation.

	::std::int_least64_t offs{nt_calculate_offset_impl(off)};
	return ::fast_io::win32::nt::details::nt_write_pwrite_some_bytes_common_impl<family>(handle, first, count,
																						 __builtin_addressof(offs));
}

template <::fast_io::nt_family family>
inline void nt_write_all_bytes_impl(void *__restrict handle, ::std::byte const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::nt::details::nt_write_pwrite_all_thunk<family>(handle, first, count, nullptr);
}

template <::fast_io::nt_family family>
inline void nt_pwrite_all_bytes_impl(void *__restrict handle, ::std::byte const *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	// The difference between P-series functions in Windows synchronization mode and POSIX is that under Windows,
	// the functions will advance the position by the number of bytes written or read after each write/read operation.

	::std::int_least64_t offs{nt_calculate_offset_impl(off)};
	::fast_io::win32::nt::details::nt_write_pwrite_all_thunk<family>(handle, first, count,
																	 __builtin_addressof(offs));
}

} // namespace fast_io::win32::nt::details

namespace fast_io
{

template <nt_family family, ::std::integral ch_type>
inline ::std::byte *read_some_bytes_underflow_define(basic_nt_family_io_observer<family, ch_type> niob,
													 ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::nt::details::nt_read_some_bytes_impl<family>(niob.handle, first, count);
}

template <nt_family family, ::std::integral ch_type>
inline ::std::byte const *write_some_bytes_overflow_define(basic_nt_family_io_observer<family, ch_type> niob,
														   ::std::byte const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::nt::details::nt_write_some_bytes_impl<family>(niob.handle, first, count);
}

template <nt_family family, ::std::integral ch_type>
inline ::std::byte *pread_some_bytes_underflow_define(basic_nt_family_io_observer<family, ch_type> niob,
													  ::std::byte *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::nt::details::nt_pread_some_bytes_impl<family>(niob.handle, first, count, off);
}

template <nt_family family, ::std::integral ch_type>
inline ::std::byte const *pwrite_some_bytes_overflow_define(basic_nt_family_io_observer<family, ch_type> niob,
															::std::byte const *first, ::std::size_t count,
															::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::nt::details::nt_pwrite_some_bytes_impl<family>(niob.handle, first, count, off);
}


template <nt_family family, ::std::integral ch_type>
inline void write_all_bytes_overflow_define(basic_nt_family_io_observer<family, ch_type> niob,
											::std::byte const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::nt::details::nt_write_all_bytes_impl<family>(niob.handle, first, count);
}

template <nt_family family, ::std::integral ch_type>
inline void pwrite_all_bytes_overflow_define(basic_nt_family_io_observer<family, ch_type> niob,
											 ::std::byte const *first, ::std::size_t count,
											 ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::nt::details::nt_pwrite_all_bytes_impl<family>(niob.handle, first, count, off);
}


} // namespace fast_io
