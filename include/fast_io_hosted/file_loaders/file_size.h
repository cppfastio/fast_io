#pragma once

#if defined(__linux__) && defined(__NR_statx) && !defined(__statx_defined)
#include<linux/stat.h>
#endif

#if defined(__has_builtin)
#if !__has_builtin(__builtin_malloc) || !__has_builtin(__builtin_free)
#include<cstdlib>
#endif
#elif !defined(_MSC_VER)
#include<cstdlib>
#endif

#if (defined(_WIN32) && !defined(__BIONIC__)) || defined(__CYGWIN__)
namespace fast_io::win32::details
{
inline ::std::size_t win32_load_file_get_file_size(void* handle)
{
	::fast_io::win32::by_handle_file_information bhdi;
	if(!GetFileInformationByHandle(handle,__builtin_addressof(bhdi)))
		throw_win32_error();
	if constexpr(sizeof(::std::size_t)<sizeof(::std::uint_least64_t))
	{
		if(bhdi.nFileSizeHigh)
			throw_win32_error(0x000000DF);
		if constexpr(sizeof(::std::size_t)<sizeof(::std::uint_least32_t))
		{
			if(bhdi.nFileSizeLow>static_cast<::std::uint_least32_t>(SIZE_MAX))
				throw_win32_error(0x000000DF);
		}
		return static_cast<::std::size_t>(bhdi.nFileSizeLow);
	}
	else
	{
		return static_cast<::std::size_t>((static_cast<::std::uint_least64_t>(bhdi.nFileSizeHigh)<<32)|bhdi.nFileSizeLow);
	}
}
}
#endif

namespace fast_io::details
{

inline ::std::size_t posix_loader_get_file_size(int fd)
{
#if defined(_WIN32) && !defined(__BIONIC__)&&!defined(__WINE__)
//windows 95 and windows 98 msvcrt do not provide struct __stat64. Directly invoke win32 api
	return ::fast_io::win32::details::win32_load_file_get_file_size(reinterpret_cast<void*>(noexcept_call(_get_osfhandle,fd)));
#elif defined(__linux__) && defined(__NR_statx)
//Linux kernel provides new __NR_statx syscall. That allows us to only extract part of information and avoid libc braindeath beneath.
	struct statx statxbuf;
	system_call_throw_error(system_call<__NR_statx,int>(fd,reinterpret_cast<char const*>(u8""),
#if defined(AT_EMPTY_PATH)
		AT_EMPTY_PATH
#else
		0x1000	//AT_EMPTY_PATH requires _GNU_SOURCE gets defined which might not be available for all libcs
#endif
		,STATX_SIZE,__builtin_addressof(statxbuf)));
	using stx_size_unsigned_type = ::std::make_unsigned_t<decltype(statxbuf.stx_size)>;
	if constexpr(sizeof(stx_size_unsigned_type)>sizeof(::std::size_t))
	{
		if(static_cast<stx_size_unsigned_type>(statxbuf.stx_size)>static_cast<stx_size_unsigned_type>(SIZE_MAX))
			throw_posix_error(EINVAL);
	}
	return static_cast<::std::size_t>(statxbuf.stx_size);
#else
#if defined(__linux__) && !defined(__MLIBC_O_CLOEXEC)
	struct stat64 st;
#else
	struct stat st;
#endif
	if(
#if defined(__linux__) && !defined(__MLIBC_O_CLOEXEC)
fstat64
#else
fstat
#endif
	(fd,__builtin_addressof(st))<0)
		throw_posix_error();
	using st_size_unsigned_type = ::std::make_unsigned_t<decltype(st.st_size)>;
	if constexpr(sizeof(st_size_unsigned_type)>sizeof(::std::size_t))
	{
		if(static_cast<st_size_unsigned_type>(st.st_size)>static_cast<st_size_unsigned_type>(SIZE_MAX))
			throw_posix_error(EINVAL);
	}
	return static_cast<::std::size_t>(st.st_size);
#endif
}

}
