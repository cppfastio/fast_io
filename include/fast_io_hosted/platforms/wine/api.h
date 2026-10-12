#pragma once
/*
fast_io's private view of wineunix.dll's abi — the imports are declared here
with __asm__ labels bound to the dll's export names, so the wine layer needs
no winelibc headers and a TU including both fast_io and the real
<__wine_unix/__wine_unix.h> sees no collision. types and flag values mirror
that header (linux/asm-generic abi).
*/

namespace fast_io::wine_unix
{

using host_fd_t = ::std::size_t;
using flags_t = host_fd_t;
using mode_t = host_fd_t;
using off_t = ::std::conditional_t<sizeof(::std::intptr_t) < sizeof(::std::int_least64_t),
								   ::std::int_least64_t, ::std::ptrdiff_t>;
using status_t = ::std::int_least32_t;

struct iovec_t
{
	void const *iov_base;
	::std::size_t iov_len;
};

struct host_fd_status_t
{
	status_t status;
	host_fd_t host_fd;
};

struct nt_handle_status_t
{
	status_t status;
	::std::ptrdiff_t handle;
};

/*
vectored results report where the transfer stopped: baseindex = iovec the
transfer stopped at, index = bytes consumed within it. the byte total is
derivable from the caller's iovecs and is not marshalled.
*/
struct rwv_status_t
{
	status_t status;
	::std::size_t baseindex;
	::std::size_t index;
};

struct rwv_result_t
{
	::std::size_t baseindex;
	::std::size_t index;
};

struct rw_status_t
{
	status_t status;
	::std::size_t total;
};

struct rw_result_t
{
	::std::size_t total;
};

/* host abi open() flag values — linux asm-generic set the dll speaks */
inline constexpr flags_t o_accmode{3};
inline constexpr flags_t o_rdonly{0};
inline constexpr flags_t o_wronly{1};
inline constexpr flags_t o_rdwr{2};
inline constexpr flags_t o_creat{0x40};
inline constexpr flags_t o_excl{0x80};
inline constexpr flags_t o_noctty{0x100};
inline constexpr flags_t o_trunc{0x200};
inline constexpr flags_t o_append{0x400};
inline constexpr flags_t o_nonblock{0x800};
inline constexpr flags_t o_dsync{0x1000};
inline constexpr flags_t o_direct{0x4000};
inline constexpr flags_t o_largefile{0x8000};
inline constexpr flags_t o_directory{0x10000};
inline constexpr flags_t o_nofollow{0x20000};
inline constexpr flags_t o_noatime{0x40000};
inline constexpr flags_t o_cloexec{0x80000};
inline constexpr flags_t o_sync{0x101000};
inline constexpr flags_t o_path{0x200000};
inline constexpr flags_t o_tmpfile{0x410000};

} // namespace fast_io::wine_unix

namespace fast_io
{

using wine_host_fd_t = ::fast_io::wine_unix::host_fd_t;

} // namespace fast_io

namespace fast_io::wine
{

/*
host_fd <-> HANDLE conversions TRANSFER ownership: on success the source
is consumed and the result is owned by the caller.
wine_unix_nt_handle_to_host_fd_ref is the non-consuming direction for
observer-style views: the handle stays owned by the caller. unixcall impl
mints a fresh fd owned by the caller (wine_unix_close frees it); nt impl
aliases the handle — never close it.
*/
FAST_IO_DLLIMPORT extern ::fast_io::wine_unix::nt_handle_status_t
	wine_unix_host_fd_to_nt_handle_returns_status(::fast_io::wine_unix::host_fd_t) noexcept
	FAST_IO_WINCDECL_RENAME(__wine_unix_host_fd_to_nt_handle_returns_status, 4);
FAST_IO_DLLIMPORT extern ::fast_io::wine_unix::host_fd_status_t
wine_unix_nt_handle_to_host_fd_ref_returns_status(::std::ptrdiff_t) noexcept
	FAST_IO_WINCDECL_RENAME(__wine_unix_nt_handle_to_host_fd_ref_returns_status, 4);

FAST_IO_DLLIMPORT extern ::fast_io::wine_unix::host_fd_status_t
wine_unix_openat_returns_status(::fast_io::wine_unix::host_fd_t, char const *, ::std::size_t,
								::fast_io::wine_unix::flags_t, ::fast_io::wine_unix::mode_t) noexcept
	FAST_IO_WINCDECL_RENAME(__wine_unix_openat_returns_status, 20);
/* plain open(): cwd-relative without fetching the at_fdcwd token first */
FAST_IO_DLLIMPORT extern ::fast_io::wine_unix::host_fd_status_t
wine_unix_open_returns_status(char const *, ::std::size_t,
							  ::fast_io::wine_unix::flags_t, ::fast_io::wine_unix::mode_t) noexcept
	FAST_IO_WINCDECL_RENAME(__wine_unix_open_returns_status, 16);
FAST_IO_DLLIMPORT extern ::fast_io::wine_unix::status_t
	wine_unix_close_returns_status(::fast_io::wine_unix::host_fd_t) noexcept
	FAST_IO_WINCDECL_RENAME(__wine_unix_close_returns_status, 4);
FAST_IO_DLLIMPORT extern ::fast_io::wine_unix::rwv_status_t
wine_unix_writev_returns_status(::fast_io::wine_unix::host_fd_t,
								::fast_io::wine_unix::iovec_t const *, ::std::size_t) noexcept
	FAST_IO_WINCDECL_RENAME(__wine_unix_writev_returns_status, 12);
FAST_IO_DLLIMPORT extern ::fast_io::wine_unix::rwv_status_t
wine_unix_readv_returns_status(::fast_io::wine_unix::host_fd_t,
							   ::fast_io::wine_unix::iovec_t const *, ::std::size_t) noexcept
	FAST_IO_WINCDECL_RENAME(__wine_unix_readv_returns_status, 12);
FAST_IO_DLLIMPORT extern ::fast_io::wine_unix::rwv_status_t
wine_unix_pwritev_returns_status(::fast_io::wine_unix::host_fd_t,
								 ::fast_io::wine_unix::iovec_t const *, ::std::size_t,
								 ::fast_io::wine_unix::off_t) noexcept
	FAST_IO_WINCDECL_RENAME(__wine_unix_pwritev_returns_status, 20);
FAST_IO_DLLIMPORT extern ::fast_io::wine_unix::rwv_status_t
wine_unix_preadv_returns_status(::fast_io::wine_unix::host_fd_t,
								::fast_io::wine_unix::iovec_t const *, ::std::size_t,
								::fast_io::wine_unix::off_t) noexcept
	FAST_IO_WINCDECL_RENAME(__wine_unix_preadv_returns_status, 20);
FAST_IO_DLLIMPORT extern ::fast_io::wine_unix::rw_status_t
wine_unix_write_returns_status(::fast_io::wine_unix::host_fd_t, void const *, ::std::size_t) noexcept
	FAST_IO_WINCDECL_RENAME(__wine_unix_write_returns_status, 12);
FAST_IO_DLLIMPORT extern ::fast_io::wine_unix::rw_status_t
wine_unix_read_returns_status(::fast_io::wine_unix::host_fd_t, void *, ::std::size_t) noexcept
	FAST_IO_WINCDECL_RENAME(__wine_unix_read_returns_status, 12);
FAST_IO_DLLIMPORT FAST_IO_GNU_CONST extern ::fast_io::wine_unix::host_fd_status_t
	wine_unix_get_std_host_fd_returns_status(::std::int_least32_t) noexcept
	FAST_IO_WINCDECL_RENAME(__wine_unix_get_std_host_fd_returns_status, 4);

/* nonzero on the unix backend (wine + libwineunix.so), 0 on nt; resolved at dll load, infallible */
FAST_IO_DLLIMPORT FAST_IO_GNU_CONST extern ::std::uint_least8_t
wine_unix_is_unix() noexcept FAST_IO_WINCDECL_RENAME(__wine_unix_is_unix, 0);

/* impl-defined at_fdcwd token for host_dirfd; infallible, 0 if dispatch is broken */
FAST_IO_DLLIMPORT FAST_IO_GNU_CONST extern ::fast_io::wine_unix::host_fd_t
wine_unix_at_fdcwd() noexcept FAST_IO_WINCDECL_RENAME(__wine_unix_at_fdcwd, 0);

} // namespace fast_io::wine

/*
two spellings of the same fast_io::wine::wine_unix_* surface:

apiherb.h  (herbceptions)   the dll's return_failure exports, imported under
							wine:: so a plain call inside a throws function
							auto-propagates failure as an exception.
apishim.h  (otherwise)      the same names emulated on the *_returns_status
							api: status -> throw_wine_errc.
*/
#if defined(__HERBCEPTIONS__)
#include "apiherb.h"
#else
#include "apishim.h"
#endif
