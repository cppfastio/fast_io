#pragma once

#if !FAST_IO_HAS_BUILTIN(__builtin_malloc) || !FAST_IO_HAS_BUILTIN(__builtin_free) || !defined(_MSC_VER)
#include <cstdlib>
#endif
namespace fast_io
{

#if (defined(_WIN32) && !defined(__BIONIC__)) || defined(__CYGWIN__)
namespace win32
{
namespace details
{
inline ::std::size_t win32_load_file_get_file_size(void *handle)
{
	::fast_io::win32::by_handle_file_information bhdi;
	if (!::fast_io::win32::GetFileInformationByHandle(handle, __builtin_addressof(bhdi)))
	{
		throw_win32_error();
	}
	if constexpr (sizeof(::std::size_t) < sizeof(::std::uint_least64_t))
	{
		if (bhdi.nFileSizeHigh)
		{
			throw_win32_error(0x000000DF);
		}
		if constexpr (sizeof(::std::size_t) < sizeof(::std::uint_least32_t))
		{
			if (bhdi.nFileSizeLow > static_cast<::std::uint_least32_t>(SIZE_MAX))
			{
				throw_win32_error(0x000000DF);
			}
		}
		return static_cast<::std::size_t>(bhdi.nFileSizeLow);
	}
	else
	{
		return static_cast<::std::size_t>((static_cast<::std::uint_least64_t>(bhdi.nFileSizeHigh) << 32) |
										  bhdi.nFileSizeLow);
	}
}
} // namespace details

namespace nt::details
{
template <bool zw>
inline ::std::size_t nt_load_file_get_file_size(void *handle)
{
	::fast_io::win32::nt::file_standard_information fsi;
	::fast_io::win32::nt::io_status_block block;
	auto status{::fast_io::win32::nt::nt_query_information_file<zw>(
		handle, __builtin_addressof(block), __builtin_addressof(fsi),
		static_cast<::std::uint_least32_t>(sizeof(::fast_io::win32::nt::file_standard_information)),
		::fast_io::win32::nt::file_information_class::FileStandardInformation)};
	if (status)
	{
		throw_nt_error(status);
	}
	if constexpr (sizeof(::std::size_t) < sizeof(::std::uint_least64_t))
	{
		if (SIZE_MAX < fsi.end_of_file)
		{
			throw_nt_error(0xC0000040 /*STATUS_SECTION_TOO_BIG*/);
		}
	}
	return static_cast<::std::size_t>(fsi.end_of_file);
}
} // namespace nt::details
} // namespace win32
#endif

namespace details
{
struct linux_statx_timestamp
{
	::std::int_least64_t tv_sec;
	::std::uint_least32_t tv_nsec, pad;
};

struct linux_struct_statx
{
	::std::uint_least32_t stx_mask;
	::std::uint_least32_t stx_blksize;
	::std::uint_least64_t stx_attributes;
	::std::uint_least32_t stx_nlink;
	::std::uint_least32_t stx_uid;
	::std::uint_least32_t stx_gid;
	::std::uint_least16_t stx_mode;
	::std::uint_least16_t pad0[1];
	::std::uint_least64_t stx_ino;
	::std::uint_least64_t stx_size;
	::std::uint_least64_t stx_blocks;
	::std::uint_least64_t stx_attributes_mask;
	::fast_io::details::linux_statx_timestamp stx_atime;
	::fast_io::details::linux_statx_timestamp stx_btime;
	::fast_io::details::linux_statx_timestamp stx_ctime;
	::fast_io::details::linux_statx_timestamp stx_mtime;
	::std::uint_least32_t stx_rdev_major;
	::std::uint_least32_t stx_rdev_minor;
	::std::uint_least32_t stx_dev_major;
	::std::uint_least32_t stx_dev_minor;
	::std::uint_least64_t pad1[14];
};

inline constexpr ::std::uint_least32_t linux_statx_size{0x200U};

inline ::std::size_t posix_loader_get_file_size(int fd)
{
#if defined(_WIN32) && !defined(__BIONIC__) && !defined(__WINE__)
	// windows 95 and windows 98 msvcrt do not provide struct __stat64. Directly invoke win32 api
	return ::fast_io::win32::details::win32_load_file_get_file_size(
		reinterpret_cast<void *>(noexcept_call(_get_osfhandle, fd)));
#elif defined(__linux__) && defined(__NR_statx)
	// Linux kernel provides new __NR_statx syscall. That allows us to only extract part of information and avoid libc
	// braindeath beneath.
	linux_struct_statx statxbuf;
	system_call_throw_error(system_call<__NR_statx, int>(fd, reinterpret_cast<char const *>(u8""),
#if defined(AT_EMPTY_PATH)
														 AT_EMPTY_PATH
#else
														 0x1000 // AT_EMPTY_PATH requires _GNU_SOURCE gets defined which
																// might not be available for all libcs
#endif
														 ,
														 linux_statx_size, __builtin_addressof(statxbuf)));
	using stx_size_unsigned_type = ::std::make_unsigned_t<decltype(statxbuf.stx_size)>;
	if constexpr (sizeof(stx_size_unsigned_type) > sizeof(::std::size_t))
	{
		if (static_cast<stx_size_unsigned_type>(statxbuf.stx_size) > static_cast<stx_size_unsigned_type>(SIZE_MAX))
		{
			throw_posix_error(EINVAL);
		}
	}
	return static_cast<::std::size_t>(statxbuf.stx_size);
#else
#if defined(__linux__) && !defined(__MLIBC_O_CLOEXEC)
	struct stat64 st;
#else
	struct stat st;
#endif
	if (
#if defined(__linux__) && !defined(__MLIBC_O_CLOEXEC)
		fstat64
#else
		fstat
#endif
		(fd, __builtin_addressof(st)) < 0)
		throw_posix_error();
	using st_size_unsigned_type = ::std::make_unsigned_t<decltype(st.st_size)>;
	if constexpr (sizeof(st_size_unsigned_type) > sizeof(::std::size_t))
	{
		if (static_cast<st_size_unsigned_type>(st.st_size) > static_cast<st_size_unsigned_type>(SIZE_MAX))
		{
			throw_posix_error(EINVAL);
		}
	}
	return static_cast<::std::size_t>(st.st_size);
#endif
}

} // namespace details

#if (defined(_WIN32) && !defined(__BIONIC__)) || defined(__CYGWIN__)
template <win32_family family, ::std::integral char_type>
inline ::std::size_t file_size(::fast_io::basic_win32_family_io_observer<family, char_type> observer)
{
	return win32::details::win32_load_file_get_file_size(observer.handle);
}

template <nt_family family, ::std::integral char_type>
inline ::std::size_t file_size(::fast_io::basic_nt_family_io_observer<family, char_type> observer)
{
	return win32::nt::details::nt_load_file_get_file_size(observer.handle);
}
#endif

template <posix_family family, ::std::integral char_type>
inline ::std::size_t file_size(::fast_io::basic_posix_family_io_observer<family, char_type> observer)
{
	return details::posix_loader_get_file_size(observer.fd);
}
} // namespace fast_io
