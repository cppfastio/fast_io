#pragma once

#include<stdint.h>
#include<limits.h>

#if defined(_WIN32) && !defined(__WINE__) || defined(__CYGWIN__)
#define __WINE_UNIX_DLLEXPORT __declspec(dllexport)
#define __WINE_UNIX_DEFAULTCALL __fastcall
#else
#define __WINE_UNIX_DLLEXPORT
#define __WINE_UNIX_DEFAULTCALL
#endif

#ifdef __cplusplus
#ifdef __cplusplus <= 201107
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
typedef int_least32_t __wine_host_fd_t;
#else
typedef intptr_t __wine_host_fd_t;
#endif

typedef __wine_host_fd_t __wine_host_flags_t;
typedef __wine_host_fd_t __wine_host_mode_t;

typedef struct
{
	uintptr_t errcode;
}__wine_unix_result0_t;

typedef struct
{
	uintptr_t errcode;
	uintptr_t value0;
}__wine_unix_result1_t;

typedef struct
{
	uintptr_t errcode;
	uintptr_t value0;
	uintptr_t value1;

}__wine_unix_result2_t;

typedef struct
{
	uintptr_t errcode;
	uintptr_t value0;
	uintptr_t value1;
	uintptr_t value2;

}__wine_unix_result3_t;

typedef struct
{
	void *iov_base;
	size_t iov_len;
}__wine_unix_iovec_t;

__WINE_UNIX_DLLEXPORT __wine_unix_result3_t __WINE_UNIX_DEFAULTCALL __wine_unix_writev(__wine_host_fd_t, __wine_unix_iovec_t const*, size_t) __WINE_UNIX_NOEXCEPT;
__WINE_UNIX_DLLEXPORT __wine_unix_result3_t __WINE_UNIX_DEFAULTCALL __wine_unix_readv(__wine_host_fd_t, __wine_unix_iovec_t*, size_t) __WINE_UNIX_NOEXCEPT;
__WINE_UNIX_DLLEXPORT __wine_unix_result1_t __WINE_UNIX_DEFAULTCALL __wine_unix_openat(__wine_host_fd_t, void const*, size_t, __wine_host_flags_t, __wine_host_mode_t) __WINE_UNIX_NOEXCEPT;
__WINE_UNIX_DLLEXPORT __wine_unix_result0_t __WINE_UNIX_DEFAULTCALL __wine_unix_close(__wine_host_fd_t) __WINE_UNIX_NOEXCEPT;

#ifdef __cplusplus
}
#endif
