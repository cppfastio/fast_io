#pragma once

namespace fast_io::wine
{
FAST_IO_DLLIMPORT extern ::std::ptrdiff_t
	wine_unix_host_fd_to_nt_handle(::fast_io::wine_unix::host_fd_t)
		return_failure{::fast_io::freestanding::wine_errc} FAST_IO_WINCDECL_RENAME(__wine_unix_host_fd_to_nt_handle, 4);
FAST_IO_DLLIMPORT extern ::fast_io::wine_unix::host_fd_t
wine_unix_nt_handle_to_host_fd_ref(::std::ptrdiff_t)
	return_failure{::fast_io::freestanding::wine_errc} FAST_IO_WINCDECL_RENAME(__wine_unix_nt_handle_to_host_fd_ref, 4);
FAST_IO_DLLIMPORT extern ::fast_io::wine_unix::host_fd_t
wine_unix_openat(::fast_io::wine_unix::host_fd_t, char const *, ::std::size_t,
				 ::fast_io::wine_unix::flags_t, ::fast_io::wine_unix::mode_t)
	return_failure{::fast_io::freestanding::wine_errc} FAST_IO_WINCDECL_RENAME(__wine_unix_openat, 20);
FAST_IO_DLLIMPORT extern ::fast_io::wine_unix::host_fd_t
wine_unix_open(char const *, ::std::size_t,
			   ::fast_io::wine_unix::flags_t, ::fast_io::wine_unix::mode_t)
	return_failure{::fast_io::freestanding::wine_errc} FAST_IO_WINCDECL_RENAME(__wine_unix_open, 16);
FAST_IO_DLLIMPORT extern ::fast_io::wine_unix::rwv_result_t
wine_unix_writev(::fast_io::wine_unix::host_fd_t, ::fast_io::wine_unix::iovec_t const *,
				 ::std::size_t) return_failure{::fast_io::freestanding::wine_errc} FAST_IO_WINCDECL_RENAME(__wine_unix_writev, 12);
FAST_IO_DLLIMPORT extern ::fast_io::wine_unix::rwv_result_t
wine_unix_readv(::fast_io::wine_unix::host_fd_t, ::fast_io::wine_unix::iovec_t const *,
				::std::size_t) return_failure{::fast_io::freestanding::wine_errc} FAST_IO_WINCDECL_RENAME(__wine_unix_readv, 12);
FAST_IO_DLLIMPORT extern ::fast_io::wine_unix::rwv_result_t
wine_unix_pwritev(::fast_io::wine_unix::host_fd_t, ::fast_io::wine_unix::iovec_t const *,
				  ::std::size_t, ::fast_io::wine_unix::off_t)
	return_failure{::fast_io::freestanding::wine_errc} FAST_IO_WINCDECL_RENAME(__wine_unix_pwritev, 20);
FAST_IO_DLLIMPORT extern ::fast_io::wine_unix::rwv_result_t
wine_unix_preadv(::fast_io::wine_unix::host_fd_t, ::fast_io::wine_unix::iovec_t const *,
				 ::std::size_t, ::fast_io::wine_unix::off_t)
	return_failure{::fast_io::freestanding::wine_errc} FAST_IO_WINCDECL_RENAME(__wine_unix_preadv, 20);
FAST_IO_DLLIMPORT extern ::fast_io::wine_unix::rw_result_t
wine_unix_write(::fast_io::wine_unix::host_fd_t, void const *, ::std::size_t)
	return_failure{::fast_io::freestanding::wine_errc} FAST_IO_WINCDECL_RENAME(__wine_unix_write, 12);
FAST_IO_DLLIMPORT extern ::fast_io::wine_unix::rw_result_t
wine_unix_read(::fast_io::wine_unix::host_fd_t, void *, ::std::size_t)
	return_failure{::fast_io::freestanding::wine_errc} FAST_IO_WINCDECL_RENAME(__wine_unix_read, 12);
} // namespace fast_io::wine
