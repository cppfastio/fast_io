#pragma once

#include <stdint.h>
#include <limits.h>
#include <stddef.h>

#if defined(_WIN32) && !defined(__WINE__) || defined(__CYGWIN__)
#define __WINE_UNIX_DLLEXPORT __declspec(dllexport)
#define __WINE_UNIX_DEFAULTCALL __fastcall
#else
#define __WINE_UNIX_DLLEXPORT
#define __WINE_UNIX_DEFAULTCALL
#endif

#ifdef __cplusplus
#if __cplusplus <= 201107
#define __WINE_UNIX_NOEXCEPT throw()
#else
#define __WINE_UNIX_NOEXCEPT noexcept
#endif
extern "C"
{
#else
#define __WINE_UNIX_NOEXCEPT
#endif
#if INTPTR_MAX < INT_LEAST32_MAX
	typedef uint_least32_t __wine_host_fd_t;
	typedef uint_least32_t __wine_errno_t;
#else
typedef size_t __wine_host_fd_t;
typedef size_t __wine_errno_t;
#endif
#if INTPTR_MAX < INT_LEAST64_MAX
	typedef int_least64_t __wine_off_t;
#else
typedef ptrdiff_t __wine_off_t;
#endif

	typedef __wine_host_fd_t __wine_host_flags_t;
	typedef __wine_host_fd_t __wine_host_mode_t;

	typedef struct
	{
		void const *iov_base;
		size_t iov_len;
	} __wine_unix_iovec_t;

	typedef struct
	{
		__wine_errno_t errcode;
		size_t total;
		size_t baseindex;
		size_t index;
	} __wine_unix_result_ret_readwritev;

	__WINE_UNIX_DLLEXPORT __wine_unix_result_ret_readwritev __WINE_UNIX_DEFAULTCALL __wine_unix_ret_writev(__wine_host_fd_t, __wine_unix_iovec_t const *, size_t) __WINE_UNIX_NOEXCEPT;

	__WINE_UNIX_DLLEXPORT __wine_unix_result_ret_readwritev __WINE_UNIX_DEFAULTCALL __wine_unix_ret_readv(__wine_host_fd_t, __wine_unix_iovec_t const *, size_t) __WINE_UNIX_NOEXCEPT;

	typedef struct
	{
		__wine_errno_t errcode;
		__wine_host_fd_t host_fd;
	} __wine_unix_result_ret_host_fd;

	typedef struct
	{
		__wine_errno_t errcode;
		ptrdiff_t handle;
	} __wine_unix_result_ret_nt_handle;

	__WINE_UNIX_DLLEXPORT __wine_errno_t __WINE_UNIX_DEFAULTCALL __wine_unix_ret_host_fd_to_unix_fd(__wine_host_fd_t host_fd, void *punixfd, size_t unixfdsizeof) __WINE_UNIX_NOEXCEPT;
	__WINE_UNIX_DLLEXPORT __wine_unix_result_ret_host_fd __WINE_UNIX_DEFAULTCALL __wine_unix_ret_unix_fd_to_host_fd(void const *punixfd, size_t unixfdsizeof) __WINE_UNIX_NOEXCEPT;
	__WINE_UNIX_DLLEXPORT __wine_unix_result_ret_nt_handle __WINE_UNIX_DEFAULTCALL __wine_unix_ret_host_fd_to_nt_handle(__wine_host_fd_t host_fd) noexcept;
	__WINE_UNIX_DLLEXPORT __wine_unix_result_ret_host_fd __WINE_UNIX_DEFAULTCALL __wine_unix_ret_nt_handle_to_host_fd(ptrdiff_t handle) noexcept;
	__WINE_UNIX_DLLEXPORT __wine_unix_result_ret_host_fd __WINE_UNIX_DEFAULTCALL __wine_unix_ret_openat(__wine_host_fd_t, void const *, size_t, __wine_host_flags_t, __wine_host_mode_t) __WINE_UNIX_NOEXCEPT;
	__WINE_UNIX_DLLEXPORT __wine_errno_t __WINE_UNIX_DEFAULTCALL __wine_unix_ret_close(__wine_host_fd_t) __WINE_UNIX_NOEXCEPT;

#ifdef __cplusplus
}
#endif
