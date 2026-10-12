#pragma once

namespace fast_io
{

#if ((!defined(_WIN32) || defined(__WINE__)) || defined(__CYGWIN__))
namespace posix
{
#if defined(__DARWIN_C_LEVEL) || defined(__MSDOS__)
extern int libc_ioctl(int fd, unsigned long request, ...) noexcept __asm__("_ioctl");
#else
extern int libc_ioctl(int fd, unsigned long request, ...) noexcept __asm__("ioctl");
#endif
} // namespace posix
#endif

enum class posix_family
{
	api,
	syscall,
#ifdef __MSDOS__
	dos,
#endif
	native = api
};

struct posix_fs_dirent
{
	int fd{-1};
	char const *filename{};
	inline explicit constexpr posix_fs_dirent() = default;
	inline explicit constexpr posix_fs_dirent(int fdd, char const *fnm) noexcept
		: fd(fdd), filename(fnm)
	{}
};

struct posix_io_redirection
{
	int *pipe_fds{};
	int fd{-1};
	bool dev_null{};

	// return true when a redirection is needed
	inline constexpr operator bool() const noexcept
	{
		return pipe_fds || fd != -1 || dev_null;
	}
};

struct posix_io_redirection_std : posix_io_redirection
{
	inline constexpr posix_io_redirection_std() noexcept = default;
	template <typename T>
		requires requires(T &&t) {
			{ redirect(::std::forward<T>(t)) } -> ::std::same_as<posix_io_redirection>;
		}
	inline constexpr posix_io_redirection_std(T &&t) noexcept
		: posix_io_redirection(redirect(::std::forward<T>(t)))
	{
	}
};

struct posix_process_io
{
	posix_io_redirection_std in;
	posix_io_redirection_std out;
	posix_io_redirection_std err;
};

struct posix_dev_null_t
{
};

inline constexpr posix_dev_null_t posix_dev_null() noexcept
{
	return {};
}
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)

inline constexpr win32_io_redirection redirect(posix_dev_null_t) noexcept
{
	return {.is_dev_null = true};
}

#else

inline constexpr posix_io_redirection redirect(posix_dev_null_t) noexcept
{
	return {.dev_null = true};
}

#endif

#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)

namespace details
{

/*
Warning! cygwin's _get_osfhandle has the same name as msvcrt or ucrt's name, but they are completely different
functions. Also, it returns long, not ::std::ptrdiff_t
*/
#if defined(__CYGWIN__)
FAST_IO_DLLIMPORT extern long FAST_IO_WINCDECL cygwin_get_osfhandle(int) noexcept FAST_IO_WINCDECL_RENAME(_get_osfhandle, 4);
#endif

inline void *my_get_osfile_handle(int fd) noexcept
{
	if (fd == -1)
	{
		return nullptr;
	}
#if defined(__BIONIC__)
	return nullptr;
#elif defined(__CYGWIN__)
	long ret{cygwin_get_osfhandle(fd)};
	if (ret == -1)
	{
		return nullptr;
	}
	return reinterpret_cast<void *>(static_cast<::std::size_t>(static_cast<unsigned long>(ret)));
#else
	::std::ptrdiff_t ret{noexcept_call(_get_osfhandle, fd)};
	if (ret == -1)
	{
		return nullptr;
	}
	return reinterpret_cast<void *>(ret);
#endif
}

#if defined(FAST_IO_HAS_WINE_UNIX)
inline ::fast_io::wine_host_fd_t my_fd_to_wine_host_fd_impl(int fd) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::wine::wine_unix_nt_handle_to_host_fd_ref(
		static_cast<::std::ptrdiff_t>(reinterpret_cast<::std::uintptr_t>(my_get_osfile_handle(fd))));
}
#endif

} // namespace details
#endif

template <::fast_io::posix_family family, ::std::integral ch_type>
class basic_posix_family_io_observer
{
public:
	using char_type = ch_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
	using native_handle_type = int;
	native_handle_type fd = -1;
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return fd;
	}
	inline explicit constexpr operator bool() const noexcept
	{
		return fd != -1;
	}
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) || defined(__CYGWIN__)
	template <win32_family fam>
	inline explicit operator basic_win32_family_io_observer<fam, char_type>() const noexcept
	{
		return {details::my_get_osfile_handle(fd)};
	}
	template <nt_family fam>
	inline explicit operator basic_nt_family_io_observer<fam, char_type>() const noexcept
	{
		return {details::my_get_osfile_handle(fd)};
	}
#if defined(FAST_IO_HAS_WINE_UNIX)
	template <wine_family fam>
	inline explicit operator basic_wine_family_io_observer<fam, char_type>() const
		FAST_IO_HERBCEPTIONS_THROWS
	{
		return {details::my_fd_to_wine_host_fd_impl(fd)};
	}
#endif
#endif

	template <posix_family fam>
		requires(fam != family)
	inline explicit operator basic_posix_family_io_observer<fam, char_type>() const noexcept
	{
		return {fd};
	}

	inline constexpr native_handle_type release() noexcept
	{
		auto temp{fd};
		fd = -1;
		return temp;
	}
};

template <::fast_io::posix_family family, ::std::integral ch_type>
inline constexpr bool operator==(basic_posix_family_io_observer<family, ch_type> a, basic_posix_family_io_observer<family, ch_type> b) noexcept
{
	return a.fd == b.fd;
}

#if __cpp_impl_three_way_comparison >= 201907L
template <::fast_io::posix_family family, ::std::integral ch_type>
inline constexpr auto operator<=>(basic_posix_family_io_observer<family, ch_type> a, basic_posix_family_io_observer<family, ch_type> b) noexcept
{
	return a.fd <=> b.fd;
}
#endif

template <::fast_io::posix_family family, ::std::integral ch_type>
inline constexpr posix_at_entry at(basic_posix_family_io_observer<family, ch_type> piob) noexcept
{
	return posix_at_entry{piob.fd};
}

template <::fast_io::posix_family family, ::std::integral ch_type>
inline constexpr basic_posix_family_io_observer<family, ch_type> io_stream_ref_define(basic_posix_family_io_observer<family, ch_type> other) noexcept
{
	return other;
}

template <::fast_io::posix_family family, ::std::integral ch_type>
inline constexpr basic_posix_family_io_observer<family, char>
io_bytes_stream_ref_define(basic_posix_family_io_observer<family, ch_type> other) noexcept
{
	return {other.fd};
}

#if defined(__CYGWIN__)

// https://github.com/cygwin/cygwin/blob/c43ec5f5951c7f4b882a0f8e619601a45ae70a91/newlib/libc/include/sys/_default_fcntl.h#L168

inline constexpr posix_at_entry posix_at_fdcwd() noexcept
{
	return posix_at_entry(
#if defined(AT_FDCWD)
		AT_FDCWD
#else
		-2
#endif
	);
}

inline constexpr posix_at_entry at_fdcwd() noexcept
{
	return posix_at_entry(
#if defined(AT_FDCWD)
		AT_FDCWD
#else
		-2
#endif
	);
}

#elif !(defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && defined(AT_FDCWD)

inline constexpr posix_at_entry posix_at_fdcwd() noexcept
{
	return posix_at_entry(AT_FDCWD);
}

inline constexpr posix_at_entry at_fdcwd() noexcept
{
	return posix_at_entry(AT_FDCWD);
}

#elif defined(__MSDOS__) || defined(__DJGPP__)

inline constexpr posix_at_entry posix_at_fdcwd() noexcept
{
	return posix_at_entry(-100);
}

inline constexpr posix_at_entry at_fdcwd() noexcept
{
	return posix_at_entry(-100);
}

#endif

namespace details
{

#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
using mode_t = int;
#endif

inline constexpr perms st_mode_to_perms(mode_t m) noexcept
{
	return static_cast<perms>(m);
}

inline constexpr file_type st_mode_to_file_type(mode_t m) noexcept
{
/*
https://linux.die.net/man/2/fstat64
The following POSIX macros are defined to check the file type using the st_mode field:

S_ISREG(m)
is it a regular file?

S_ISDIR(m)

directory?

S_ISCHR(m)

character device?

S_ISBLK(m)

block device?

S_ISFIFO(m)

FIFO (named pipe)?

S_ISLNK(m)

symbolic link? (Not in POSIX.1-1996.)

S_ISSOCK(m)

socket? (Not in POSIX.1-1996.)
*/
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
	/*
	https://github.com/Alexpux/mingw-w64/blob/master/mingw-w64-headers/crt/sys/stat.h

	#define _S_IFDIR 0x4000
	#define _S_IFCHR 0x2000
	#define _S_IFIFO 0x1000
	#define _S_IFREG 0x8000
	#define	_S_IFBLK 0x3000
	*/
	if ((m & 0xF000) == 0x8000)
	{
		return file_type::regular;
	}
	else if ((m & 0xF000) == 0x4000)
	{
		return file_type::directory;
	}
	else if ((m & 0xF000) == 0x2000)
	{
		return file_type::character;
	}
	else if ((m & 0xF000) == 0x3000)
	{
		return file_type::block;
	}
	else if ((m & 0xF000) == 0x1000)
	{
		return file_type::fifo;
	}
	else
	{
		return file_type::unknown;
	}

#else
	if (S_ISREG(m))
	{
		return file_type::regular;
	}
	else if (S_ISDIR(m))
	{
		return file_type::directory;
	}
	else if (S_ISCHR(m))
	{
		return file_type::character;
	}
	else if (S_ISBLK(m))
	{
		return file_type::block;
	}
	else if (S_ISFIFO(m))
	{
		return file_type::fifo;
	}
#ifdef S_ISLNK
	else if (S_ISLNK(m))
	{
		return file_type::symlink;
	}
#endif
#ifdef S_ISSOCK
	else if (S_ISSOCK(m))
	{
		return file_type::socket;
	}
#endif
	else
	{
		return file_type::unknown;
	}
#endif
}

template <typename stat_model>
inline constexpr posix_file_status struct_stat_to_posix_file_status(stat_model &st) noexcept
{
	return {static_cast<::fast_io::uintfpos_t>(st.st_dev),
			static_cast<::fast_io::uintfpos_t>(st.st_ino),
			st_mode_to_perms(st.st_mode),
			st_mode_to_file_type(st.st_mode),
			static_cast<::fast_io::uintfpos_t>(st.st_nlink),
			static_cast<::fast_io::uintfpos_t>(st.st_uid),
			static_cast<::fast_io::uintfpos_t>(st.st_gid),
			static_cast<::fast_io::uintfpos_t>(st.st_rdev),
			static_cast<::fast_io::uintfpos_t>(st.st_size),
#if ((defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)) || defined(__MSDOS__)
			131072,
			static_cast<::fast_io::uintfpos_t>(st.st_size / 512),
			{st.st_atime, {}},
			{st.st_mtime, {}},
			{st.st_ctime, {}},
			{0, 0},
#elif !defined(__NEWLIB__) && !defined(__CYGWIN__) && (defined(__BSD_VISIBLE) || defined(__DARWIN_C_LEVEL)) && !defined(_PICOLIBC__) && \
	!defined(__linux__)
			static_cast<::fast_io::uintfpos_t>(st.st_blksize),
			static_cast<::fast_io::uintfpos_t>(st.st_blocks),
			timespec_to_unix_timestamp(st.st_atimespec),
			timespec_to_unix_timestamp(st.st_mtimespec),
			timespec_to_unix_timestamp(st.st_ctimespec),
#if defined(__DARWIN_C_LEVEL)
			{0, 0}
#else
			timespec_to_unix_timestamp(st.st_birthtimespec)
#endif
			,
#else
		static_cast<::fast_io::uintfpos_t>(st.st_blksize),
		static_cast<::fast_io::uintfpos_t>(st.st_blocks),
#if defined(__svr4__) && !defined(__PPC__) && !defined(__sun__)
		{st.st_atime, 0},
		{st.st_mtime, 0},
		{st.st_ctime, 0},
#else
		timespec_to_unix_timestamp(st.st_atim),
		timespec_to_unix_timestamp(st.st_mtim),
		timespec_to_unix_timestamp(st.st_ctim),
#endif
#if defined(__CYGWIN__)
		timespec_to_unix_timestamp(st.st_birthtim)
#else
		{0, 0}
#endif
			,
#endif
#if !defined(__NEWLIB__) && !defined(__CYGWIN__) && (defined(__BSD_VISIBLE) || defined(__DARWIN_C_LEVEL)) && !defined(_PICOLIBC__) && \
	!defined(__linux__)
			st.st_flags,
			st.st_gen
#else
			0,
			0
#endif
	};
}

inline posix_file_status fstat_impl(int fd)
	FAST_IO_HERBCEPTIONS_THROWS
{
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
	struct __stat64 st;
#elif defined(__linux__) && defined(__USE_LARGEFILE64)
	struct stat64 st;
#else
	struct stat st;
#endif
	if (::fast_io::noexcept_call(
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
#if (!defined(__MINGW32__) || __has_include(<_mingw_stat64.h>))
			::_fstat64
#else
			::_fstati64
#endif
#elif defined(__linux__) && defined(__USE_LARGEFILE64)
			::fstat64
#else
			::fstat
#endif
			,
			fd, __builtin_addressof(st)) == -1) [[unlikely]]
		throw_posix_error();
	return struct_stat_to_posix_file_status(st);
}

} // namespace details

template <::fast_io::posix_family family, ::std::integral ch_type>
inline posix_file_status status(basic_posix_family_io_observer<family, ch_type> piob)
	FAST_IO_HERBCEPTIONS_THROWS
{
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
	return status(static_cast<basic_win32_io_observer<ch_type>>(piob));
#else
	return details::fstat_impl(piob.fd);
#endif
}

template <::fast_io::posix_family family, ::std::integral ch_type>
inline bool is_character_device(basic_posix_family_io_observer<family, ch_type> piob) noexcept
{
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
	// _isatty is what the CRT itself uses to decide whether a file gets a stdio buffer.
	return ::fast_io::noexcept_call(::_isatty, piob.fd) != 0;
#else
	return ::fast_io::noexcept_call(::isatty, piob.fd) != 0;
#endif
}

#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
template <::fast_io::posix_family family, ::std::integral ch_type>
inline auto redirect_handle(basic_posix_family_io_observer<family, ch_type> h) noexcept
{
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__))
	return details::my_get_osfile_handle(h.fd);
#else
	return h.fd;
#endif
}
#else

template <::fast_io::posix_family family, ::std::integral ch_type>
inline constexpr posix_io_redirection redirect(basic_posix_family_io_observer<family, ch_type> h) noexcept
{
	return {.fd = h.fd};
}

#endif
} // namespace fast_io
