#pragma once

namespace fast_io
{

namespace win32::details
{

inline void win32_calculate_offset_impl([[maybe_unused]] void *__restrict handle, ::fast_io::win32::overlapped &overlap,
										::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::std::uint_least64_t u64off{static_cast<::std::uint_least64_t>(
		::fast_io::win32::nt::details::nt_calculate_offset_impl(off))};
	overlap.dummy_union_name.dummy_struct_name = {static_cast<::std::uint_least32_t>(u64off),
												  static_cast<::std::uint_least32_t>(u64off >> 32)};
}

inline ::std::byte *read_or_pread_some_bytes_common_impl(void *__restrict handle, ::std::byte *first, ::std::size_t count,
														 ::fast_io::win32::overlapped *lpoverlapped)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::std::uint_least32_t number_of_bytes{};
	if (!::fast_io::win32::ReadFile(handle, first,
									::fast_io::details::read_write_bytes_compute<::std::uint_least32_t>(first, count),
									__builtin_addressof(number_of_bytes), lpoverlapped)) [[unlikely]]
	{
		auto err(::fast_io::win32::GetLastError());
		if (err == 109) [[likely]]
		{
			return first;
		}
		throw_win32_error(err);
	}
	return first + number_of_bytes;
}

inline ::std::byte *win32_read_some_bytes_impl(void *__restrict handle, ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::details::read_or_pread_some_bytes_common_impl(handle, first, count, nullptr);
}

inline ::std::byte *win32_ntw_pread_some_bytes_impl(void *__restrict handle, ::std::byte *first, ::std::size_t count,
													::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	// The difference between P-series functions in Windows synchronization mode and POSIX is that under Windows,
	// the functions will advance the position by the number of bytes written or read after each write/read operation.

	// The use of overlapped behavior is not supported in Windows 95.
	::fast_io::win32::overlapped overlap{};
	::fast_io::win32::details::win32_calculate_offset_impl(handle, overlap, off);
	return ::fast_io::win32::details::read_or_pread_some_bytes_common_impl(handle, first, count,
																		   __builtin_addressof(overlap));
}

inline ::std::byte const *write_or_pwrite_some_bytes_common_impl(void *__restrict handle, ::std::byte const *first, ::std::size_t count,
																 ::fast_io::win32::overlapped *lpoverlapped)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::std::uint_least32_t number_of_bytes{};
	if (!::fast_io::win32::WriteFile(handle, first,
									 ::fast_io::details::read_write_bytes_compute<::std::uint_least32_t>(first, count),
									 __builtin_addressof(number_of_bytes), lpoverlapped))
	{
		throw_win32_error();
	}
	return first + number_of_bytes;
}

inline ::std::byte const *win32_write_some_bytes_impl(void *__restrict handle, ::std::byte const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::details::write_or_pwrite_some_bytes_common_impl(handle, first, count, nullptr);
}

inline ::std::byte const *win32_ntw_pwrite_some_bytes_impl(void *__restrict handle, ::std::byte const *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	// The difference between P-series functions in Windows synchronization mode and POSIX is that under Windows,
	// the functions will advance the position by the number of bytes written or read after each write/read operation.

	// The use of overlapped behavior is not supported in Windows 95.
	::fast_io::win32::overlapped overlap{};
	::fast_io::win32::details::win32_calculate_offset_impl(handle, overlap, off);
	return ::fast_io::win32::details::write_or_pwrite_some_bytes_common_impl(handle, first, count,
																			 __builtin_addressof(overlap));
}

// ReadFile/WriteFile only take a per-call LPOVERLAPPED; they cannot advance an offset
// across the chunked calls the scatter emulation issues, so the offset accumulation and
// the pre-call overflow check live here. off < 0 passes through raw (no check/advance).
inline ::std::byte *win32_read_pread_some_thunk(void *__restrict handle, ::std::byte *first, ::std::size_t count, ::std::int_least64_t *__restrict pbyteoffset)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::overlapped overlap{};
	::fast_io::win32::overlapped *poverlap{};
	::std::int_least64_t off{};
	if (pbyteoffset != nullptr)
	{
		off = *pbyteoffset;
		if (0 <= off)
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
				throw_win32_error(534); // ERROR_ARITHMETIC_OVERFLOW
			}
		}
		win32_calculate_offset_impl(handle, overlap, static_cast<::fast_io::intfpos_t>(off));
		poverlap = __builtin_addressof(overlap);
	}
	auto written{read_or_pread_some_bytes_common_impl(handle, first, count, poverlap)};
	if (pbyteoffset != nullptr && 0 <= off)
	{
		*pbyteoffset = off + static_cast<::std::int_least64_t>(written - first);
	}
	return written;
}

inline ::std::byte const *win32_write_pwrite_some_thunk(void *__restrict handle, ::std::byte const *first, ::std::size_t count, ::std::int_least64_t *__restrict pbyteoffset)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::overlapped overlap{};
	::fast_io::win32::overlapped *poverlap{};
	::std::int_least64_t off{};
	if (pbyteoffset != nullptr)
	{
		off = *pbyteoffset;
		if (0 <= off)
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
				throw_win32_error(534); // ERROR_ARITHMETIC_OVERFLOW
			}
		}
		win32_calculate_offset_impl(handle, overlap, static_cast<::fast_io::intfpos_t>(off));
		poverlap = __builtin_addressof(overlap);
	}
	auto written{write_or_pwrite_some_bytes_common_impl(handle, first, count, poverlap)};
	if (pbyteoffset != nullptr && 0 <= off)
	{
		*pbyteoffset = off + static_cast<::std::int_least64_t>(written - first);
	}
	return written;
}

// read-all: retries until [first, last) is filled; a zero-progress round is EOF
// (the common impl maps ERROR_BROKEN_PIPE to a first-return), reported as
// ERROR_HANDLE_EOF.
inline void win32_read_pread_all_thunk(void *__restrict handle, ::std::byte *first, ::std::size_t count, ::std::int_least64_t *__restrict pbyteoffset)
	FAST_IO_HERBCEPTIONS_THROWS
{
	auto const e{first + count};
	while (first != e)
	{
		auto written{win32_read_pread_some_thunk(handle, first, static_cast<::std::size_t>(e - first), pbyteoffset)};
		if (written == first) [[unlikely]]
		{
			throw_win32_error(38); // ERROR_HANDLE_EOF
		}
		first = written;
	}
}

// write-all: retries until [first, last) is written; a zero-progress round means the
// write stopped -> ERROR_IO_DEVICE.
inline void win32_write_pwrite_all_thunk(void *__restrict handle, ::std::byte const *first, ::std::size_t count, ::std::int_least64_t *__restrict pbyteoffset)
	FAST_IO_HERBCEPTIONS_THROWS
{
	auto const e{first + count};
	while (first != e)
	{
		auto written{win32_write_pwrite_some_thunk(handle, first, static_cast<::std::size_t>(e - first), pbyteoffset)};
		if (written == first) [[unlikely]]
		{
			throw_win32_error(1117); // ERROR_IO_DEVICE
		}
		first = written;
	}
}

inline void win32_write_all_bytes_impl(void *__restrict handle, ::std::byte const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	win32_write_pwrite_all_thunk(handle, first, count, nullptr);
}

inline void win32_ntw_pwrite_all_bytes_impl(void *__restrict handle, ::std::byte const *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	// The difference between P-series functions in Windows synchronization mode and POSIX is that under Windows,
	// the functions will advance the position by the number of bytes written or read after each write/read operation.

	::std::int_least64_t offs{::fast_io::win32::nt::details::nt_calculate_offset_impl(off)};
	win32_write_pwrite_all_thunk(handle, first, count, __builtin_addressof(offs));
}

} // namespace win32::details

template <win32_family family, ::std::integral ch_type>
inline ::std::byte *read_some_bytes_underflow_define(basic_win32_family_io_observer<family, ch_type> wiob,
													 ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::details::win32_read_some_bytes_impl(wiob.handle, first, count);
}

template <win32_family family, ::std::integral ch_type>
inline ::std::byte const *write_some_bytes_overflow_define(basic_win32_family_io_observer<family, ch_type> wiob,
														   ::std::byte const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::details::win32_write_some_bytes_impl(wiob.handle, first, count);
}

template <win32_family family, ::std::integral ch_type>
inline void write_all_bytes_overflow_define(basic_win32_family_io_observer<family, ch_type> wiob,
											::std::byte const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::details::win32_write_all_bytes_impl(wiob.handle, first, count);
}

/*
I am not confident that i understand semantics correctly. Disabled first and test it later
*/

// Windows 9x does not support atomic p-series functions. When using operation::p-series functions,
// they will be emulated through seek and non-p-series functions.
// For Win9x, use the operation::p series functions, which automatically combine seek operations with non-p series functions.
#if !defined(_WIN32_WINDOWS)
template <win32_family family, ::std::integral ch_type>
inline ::std::byte *pread_some_bytes_underflow_define(basic_win32_family_io_observer<family, ch_type> niob,
													  ::std::byte *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::details::win32_ntw_pread_some_bytes_impl(niob.handle, first, count, off);
}

template <win32_family family, ::std::integral ch_type>
inline ::std::byte const *pwrite_some_bytes_overflow_define(basic_win32_family_io_observer<family, ch_type> niob,
															::std::byte const *first, ::std::size_t count,
															::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::details::win32_ntw_pwrite_some_bytes_impl(niob.handle, first, count, off);
}

template <win32_family family, ::std::integral ch_type>
inline void pwrite_all_bytes_overflow_define(basic_win32_family_io_observer<family, ch_type> niob,
											 ::std::byte const *first, ::std::size_t count,
											 ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::details::win32_ntw_pwrite_all_bytes_impl(niob.handle, first, count, off);
}
#endif

} // namespace fast_io
