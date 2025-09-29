#pragma once

namespace fast_io
{

namespace details
{

inline ::std::byte *posix_pread_bytes_impl(int fd, ::std::byte *first, ::std::byte *last, ::fast_io::intfpos_t off)
{
	// Here, functions from the operations module must be selected because Windows 9x may not provide the p-series functions.
    return ::fast_io::operations::pread_some_bytes(::fast_io::win32_io_observer{::fast_io::details::my_get_osfile_handle(fd)}, first, last, off);
}

inline ::std::byte const *posix_pwrite_bytes_impl(int fd, ::std::byte const *first, ::std::byte const *last,
												  ::fast_io::intfpos_t off)
{
	// Here, functions from the operations module must be selected because Windows 9x may not provide the p-series functions.
	return ::fast_io::operations::pwrite_some_bytes(::fast_io::win32_io_observer{::fast_io::details::my_get_osfile_handle(fd)}, first, last, off);
}

} // namespace details

template <::std::integral char_type>
inline ::std::byte *pread_some_bytes_underflow_define(::fast_io::basic_posix_io_observer<char_type> piob,
													  ::std::byte *first, ::std::byte *last, ::fast_io::intfpos_t off)
{
	return ::fast_io::details::posix_pread_bytes_impl(piob.fd, first, last, off);
}

template <::std::integral char_type>
inline ::std::byte const *pwrite_some_bytes_overflow_define(::fast_io::basic_posix_io_observer<char_type> piob,
															::std::byte const *first, ::std::byte const *last,
															::fast_io::intfpos_t off)
{
	return ::fast_io::details::posix_pwrite_bytes_impl(piob.fd, first, last, off);
}

} // namespace fast_io
