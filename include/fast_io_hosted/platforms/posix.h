#pragma once

#if ((defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__))&&!defined(__CYGWIN__)) || defined(__MSDOS__)
#if __has_include(<corecrt_io.h>)
#include<corecrt_io.h>
#elif __has_include(<io.h>)
#include<io.h>
#endif
#endif

#if __has_include(<fcntl.h>)
#include<fcntl.h>
#endif

#if __has_include(<sys/stat.h>)
#include<sys/stat.h>
#endif

#if (!defined(_WIN32)||defined(__WINE__)||defined(__BIONIC__)) || defined(__CYGWIN__)

#if __has_include(<features.h>)
#include<features.h>
#endif

#if __has_include(<unistd.h>)
#include<unistd.h>
#endif

#if __has_include(<sys/uio.h>)
#include<sys/uio.h>
#endif

#if __has_include(<sys/types.h>)
#include<sys/types.h>
#endif

#if __has_include(<sys/socket.h>)
#include<sys/socket.h>
#endif

#if defined(__wasi__)
#include <wasi/api.h>
#endif

#endif
#include"systemcall_details.h"

#if ((!defined(_WIN32)||defined(__WINE__)) || defined(__CYGWIN__)) && __has_include(<sys/socket.h>) && __has_include(<netinet/in.h>) && !defined(__wasi__)
#include <netinet/in.h>
#include "posix_netmode.h"
#endif

#if defined(F_RDLCK) && defined(F_WRLCK) && defined(F_UNLCK) && !defined(__wasi__)
#include"posix_file_lock.h"
#endif

namespace fast_io
{

namespace details
{
#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__)

inline constexpr int calculate_posix_open_mode_for_win32_handle_impl(open_mode value,int mode) noexcept
{
	constexpr auto supported_values{open_mode::out|open_mode::app|open_mode::in};
	using utype = typename std::underlying_type<open_mode>::type;
	switch(static_cast<utype>(supported_values)&static_cast<utype>(value))
	{
//Action if file already exists;	Action if file does not exist;	c-style mode;	Explanation
//Read from start;	Failure to open;	"r";	Open a file for reading
	case static_cast<utype>(open_mode::in):
		return mode | O_RDONLY;
//Destroy contents;	Create new;	"w";	Create a file for writing
	case static_cast<utype>(open_mode::out):
//Read from start;	Error;	"r+";		Open a file for read/write
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::in):
		return mode;
//Append to file;	Create new;	"a";	Append to a file
	case static_cast<utype>(open_mode::app):
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::app):
		return mode | O_APPEND;
//Write to end;	Create new;	"a+";	Open a file for read/write
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::in)|static_cast<utype>(open_mode::app):
	case static_cast<utype>(open_mode::in)|static_cast<utype>(open_mode::app):
		return mode | O_APPEND;
//Destroy contents;	Error;	"wx";	Create a file for writing
	default:
		return mode;
	}
}

enum class posix_open_mode_text_behavior
{
always_binary=0,text,wide_text,u8_text,u16_text
};

template<posix_open_mode_text_behavior behavior>
inline constexpr int calculate_posix_open_mode_for_win32_handle(open_mode value) noexcept
{
	int mode{};
	if constexpr(behavior==posix_open_mode_text_behavior::always_binary)
		mode = _O_BINARY;
	else
	{
	if((value&open_mode::text)==open_mode::none)
		mode = _O_BINARY;
	else
	{
		if constexpr(behavior==posix_open_mode_text_behavior::wide_text)
		{
#if defined(_O_WTEXT)
			mode |= _O_WTEXT;
#else
			mode |= _O_BINARY;
#endif
		}
		else if constexpr(behavior==posix_open_mode_text_behavior::u8_text)
		{
#if defined(_O_U8TEXT)
			mode |= _O_U8TEXT;
#else
			mode |= _O_BINARY;
#endif
		}
		else if constexpr(behavior==posix_open_mode_text_behavior::u16_text)
		{
#if defined(_O_U16TEXT)
			mode |= _O_U16TEXT;
#else
			mode |= _O_BINARY;
#endif
		}
		else
		{
#if defined(_O_TEXT)
			mode |= _O_TEXT;
#else
			mode |= _O_BINARY;
#endif
		}
	}
	}
	return calculate_posix_open_mode_for_win32_handle_impl(value,mode);
}
#endif


inline constexpr int calculate_posix_open_mode(open_mode value) noexcept
{
	int mode
	{
#ifdef O_NOFOLLOW
		O_NOFOLLOW
#endif
	};
	if((value&open_mode::follow)!=open_mode::none)
		mode = {};
	if((value&open_mode::inherit)==open_mode::none)
#ifdef O_CLOEXEC
		mode |= O_CLOEXEC;
#elif _O_NOINHERIT
		mode |= _O_NOINHERIT;
#endif
#ifdef O_BINARY
	if((value&open_mode::text)==open_mode::none)
		mode |= O_BINARY;
#endif
	if((value&open_mode::creat)!=open_mode::none)
		mode |= O_CREAT;
	if((value&open_mode::excl)!=open_mode::none)
		mode |= O_EXCL;
	if((value&open_mode::trunc)!=open_mode::none)
		mode |= O_TRUNC;
#ifdef O_DIRECT
	if((value&open_mode::direct)!=open_mode::none)
		mode |= O_DIRECT;
#endif
#ifdef O_SYNC
	if((value&open_mode::sync)!=open_mode::none)
		mode |= O_SYNC;
#endif
#ifdef O_TTY_INIT
	if((value&open_mode::tty_init)!=open_mode::none)
		mode |= O_TTY_INIT;
#endif
#ifdef O_NOCTTY
	if((value&open_mode::no_ctty)!=open_mode::none)
		mode |= O_NOCTTY;
#endif
#ifdef O_PATH
	if((value&open_mode::path)!=open_mode::none)
		mode |= O_PATH;
#endif
#ifdef O_NOATIME
	if((value&open_mode::no_atime)!=open_mode::none)
		mode |= O_NOATIME;
#endif
	if((value&open_mode::no_block)!=open_mode::none)
#ifdef O_NONBLOCK
		mode |= O_NONBLOCK;
#else
		return {};
#endif

	if((value&open_mode::temporary)!=open_mode::none)
	{
#if defined(O_TMPFILE)
		mode |= O_TMPFILE;
#elif defined(_O_TEMPORARY)
		mode |= _O_TEMPORARY;
#else
		return {};
#endif
	}
#ifdef _O_SEQUENTIAL
	if((value&open_mode::random_access)!=open_mode::none)
		mode |= _O_SEQUENTIAL;
	else
		mode |= _O_RANDOM;
#endif
#ifdef O_LARGEFILE
	mode |= O_LARGEFILE;
#endif

	if((value&open_mode::directory)!=open_mode::none)
#ifdef O_DIRECTORY
		mode |= O_DIRECTORY;
#else
		return {};
#endif

	using utype = typename std::underlying_type<open_mode>::type;
	constexpr auto supported_values{static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::app)|static_cast<utype>(open_mode::in)};
	switch(static_cast<utype>(value)&static_cast<utype>(supported_values))
	{
/*
https://en.cppreference.com/w/cpp/io/basic_filebuf/open

mode	openmode & ~ate	Action if file already exists	Action if file does not exist
"r"	in	Read from start	Failure to open
"w"	out, out|trunc	Destroy contents	Create new
"a"	app, out|app	Append to file	Create new
"r+"	out|in	Read from start	Error
"w+"	out|in|trunc	Destroy contents	Create new
"a+"	out|in|app, in|app	Write to end	Create new
"rb"	binary|in	Read from start	Failure to open
"wb"	binary|out, binary|out|trunc	Destroy contents	Create new
"ab"	binary|app, binary|out|app	Write to end	Create new
"r+b"	binary|out|in	Read from start	Error
"w+b"	binary|out|in|trunc	Destroy contents	Create new
"a+b"	binary|out|in|app, binary|in|app	Write to end	Create new

*/
//Action if file already exists;	Action if file does not exist;	c-style mode;	Explanation
//Read from start;	Failure to open;	"r";	Open a file for reading
	case static_cast<utype>(open_mode::in):
		return mode | O_RDONLY;
//Destroy contents;	Create new;	"w";	Create a file for writing
	case static_cast<utype>(open_mode::out):
		return mode | O_WRONLY | O_CREAT | O_TRUNC;
//Append to file;	Create new;	"a";	Append to a file
	case static_cast<utype>(open_mode::app):
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::app):
		return mode | O_WRONLY | O_CREAT | O_APPEND;
//Read from start;	Error;	"r+";		Open a file for read/write
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::in):
		return mode | O_RDWR;
//Write to end;	Create new;	"a+";	Open a file for read/write
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::in)|static_cast<utype>(open_mode::app):
	case static_cast<utype>(open_mode::in)|static_cast<utype>(open_mode::app):
		return mode | O_RDWR | O_CREAT | O_APPEND;
//Destroy contents;	Error;	"wx";	Create a file for writing
	default:
		return mode;
	}
}

}

struct posix_fs_dirent
{
	int fd{-1};
	char const* filename{};
	explicit constexpr posix_fs_dirent() = default;
	explicit constexpr posix_fs_dirent(int fdd,char const* fnm):fd(fdd),filename(fnm){}
};

struct posix_io_redirection
{
	int *pipe_fds{};
	int fd{-1};
	bool dev_null{};
};

struct posix_io_redirection_std:posix_io_redirection
{
	constexpr posix_io_redirection_std() noexcept=default;
	template<typename T>
	requires requires(T&& t)
	{
		{redirect(::std::forward<T>(t))}->std::same_as<posix_io_redirection>;
	}
	constexpr posix_io_redirection_std(T&& t) noexcept:posix_io_redirection(redirect(::std::forward<T>(t))){}
};

struct posix_process_io
{
	posix_io_redirection_std in;
	posix_io_redirection_std out;
	posix_io_redirection_std err;
};

struct posix_dev_null_t{};

inline constexpr posix_dev_null_t posix_dev_null() noexcept
{
	return {};
}

inline constexpr posix_io_redirection redirect(posix_dev_null_t) noexcept
{
	return {.dev_null=true};
}

#if (defined(_WIN32)&&!defined(__WINE__)) || defined(__CYGWIN__)
namespace details
{

/*
Warning! cygwin's _get_osfhandle has the same name as msvcrt or ucrt's name, but they are completely different functions. Also, it returns long, not std::intptr_t
*/
#if defined(__CYGWIN__)
#if (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
extern long cygwin_get_osfhandle(int fd) noexcept
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
__asm__("_get_osfhandle")
#else
__asm__("__get_osfhandle")
#endif
#else
__asm__("_get_osfhandle")
#endif
;
#endif

inline void* my_get_osfile_handle(int fd) noexcept
{
	if(fd==-1)
		return nullptr;
#if defined(__BIONIC__)
	return nullptr;
#elif defined(__CYGWIN__)
	long ret{cygwin_get_osfhandle(fd)};
	if(ret==-1)
		return nullptr;
	return reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<unsigned long>(ret)));
#else
	std::intptr_t ret{noexcept_call(_get_osfhandle,fd)};
	if(ret==-1)
		return nullptr;
	return reinterpret_cast<void*>(ret);
#endif
}

}
#endif

template<std::integral ch_type>
class basic_posix_io_observer
{
public:
	using char_type = ch_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
	using native_handle_type = int;
	native_handle_type fd=-1;
	constexpr native_handle_type native_handle() const noexcept
	{
		return fd;
	}
	explicit constexpr operator bool() const noexcept
	{
		return fd!=-1;
	}
#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) || defined(__CYGWIN__)
	template<win32_family family>
	explicit operator basic_win32_family_io_observer<family,char_type>() const noexcept
	{
		return {details::my_get_osfile_handle(fd)};
	}
	template<nt_family family>
	explicit operator basic_nt_family_io_observer<family,char_type>() const noexcept
	{
		return {details::my_get_osfile_handle(fd)};
	}
#endif
	constexpr native_handle_type release() noexcept
	{
		auto temp{fd};
		fd=-1;
		return temp;
	}
};

#if __cpp_lib_three_way_comparison >= 201907L

template<std::integral ch_type>
inline constexpr bool operator==(basic_posix_io_observer<ch_type> a,basic_posix_io_observer<ch_type> b) noexcept
{
	return a.fd==b.fd;
}

template<std::integral ch_type>
inline constexpr auto operator<=>(basic_posix_io_observer<ch_type> a,basic_posix_io_observer<ch_type> b) noexcept
{
	return a.fd<=>b.fd;
}

#endif

template<std::integral ch_type>
inline constexpr posix_at_entry at(basic_posix_io_observer<ch_type> piob) noexcept
{
	return posix_at_entry{piob.fd};
}

template<std::integral ch_type>
inline constexpr basic_posix_io_observer<ch_type> io_stream_ref_define(basic_posix_io_observer<ch_type> other) noexcept
{
	return other;
}

template<std::integral ch_type>
inline constexpr basic_posix_io_observer<char> io_bytes_stream_ref_define(basic_posix_io_observer<ch_type> other) noexcept
{
	return {other.handle};
}

#if !(defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && defined(AT_FDCWD)

inline constexpr posix_at_entry posix_at_fdcwd() noexcept
{
	return posix_at_entry(AT_FDCWD);
}

inline constexpr posix_at_entry at_fdcwd() noexcept
{
	return posix_at_entry(AT_FDCWD);
}

#endif


#if (!defined(__NEWLIB__) || defined(__CYGWIN__)) && (!defined(_WIN32) || defined(__WINE__))
namespace details
{

#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__)
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
#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__)
/*
https://github.com/Alexpux/mingw-w64/blob/master/mingw-w64-headers/crt/sys/stat.h

#define _S_IFDIR 0x4000
#define _S_IFCHR 0x2000
#define _S_IFIFO 0x1000
#define _S_IFREG 0x8000
#define	_S_IFBLK 0x3000
*/
	if((m&0xF000)==0x8000)
		return file_type::regular;
	else if((m&0xF000)==0x4000)
		return file_type::directory;
	else if((m&0xF000)==0x2000)
		return file_type::character;
	else if((m&0xF000)==0x3000)
		return file_type::block;
	else if((m&0xF000)==0x1000)
		return file_type::fifo;
	else
		return file_type::unknown;

#else
	if(S_ISREG(m))
		return file_type::regular;
	else if(S_ISDIR(m))
		return file_type::directory;
	else if(S_ISCHR(m))
		return file_type::character;
	else if(S_ISBLK(m))
		return file_type::block;
	else if(S_ISFIFO(m))
		return file_type::fifo;
#ifdef S_ISLNK
	else if(S_ISLNK(m))
		return file_type::symlink;
#endif
#ifdef S_ISSOCK
	else if(S_ISSOCK(m))
		return file_type::socket;
#endif
	else
		return file_type::unknown;
#endif
}

template<typename stat_model>
inline constexpr posix_file_status struct_stat_to_posix_file_status(stat_model& st) noexcept
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
#if ((defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__))||defined(__MSDOS__)
	131072,
	static_cast<::fast_io::uintfpos_t>(st.st_size/512),
	{st.st_atime,{}},{st.st_mtime,{}},{st.st_ctime,{}},{0,0},
#elif !defined(__CYGWIN__) && (defined(__BSD_VISIBLE) || defined(__DARWIN_C_LEVEL)) && !defined(_PICOLIBC__)
	static_cast<::fast_io::uintfpos_t>(st.st_blksize),
	static_cast<::fast_io::uintfpos_t>(st.st_blocks),
	timespec_to_unix_timestamp(st.st_atimespec),
	timespec_to_unix_timestamp(st.st_mtimespec),
	timespec_to_unix_timestamp(st.st_ctimespec),
#if defined(__DARWIN_C_LEVEL)
	{0,0}
#else
	timespec_to_unix_timestamp(st.st_birthtimespec)
#endif
	,
#else
	static_cast<::fast_io::uintfpos_t>(st.st_blksize),
	static_cast<::fast_io::uintfpos_t>(st.st_blocks),
	timespec_to_unix_timestamp(st.st_atim),
	timespec_to_unix_timestamp(st.st_mtim),
	timespec_to_unix_timestamp(st.st_ctim),
#if defined(__CYGWIN__)
timespec_to_unix_timestamp(st.st_birthtim)
#else
{0,0}
#endif
,
#endif
#if !defined(__CYGWIN__) && (defined(__BSD_VISIBLE) || defined(__DARWIN_C_LEVEL)) && !defined(_PICOLIBC__)
	st.st_flags,st.st_gen
#else
	0,0
#endif
};
}

inline posix_file_status fstat_impl(int fd)
{
#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__)
	struct __stat64 st;
#elif defined(__linux__) && !defined(__MLIBC_O_CLOEXEC)
	struct stat64 st;
#else
	struct stat st;
#endif
	if(
#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__)
#if (!defined(__MINGW32__) || __has_include(<_mingw_stat64.h>))
_fstat64
#else
_fstati64
#endif
#elif defined(__linux__) && !defined(__MLIBC_O_CLOEXEC)
fstat64
#else
fstat
#endif
(fd,__builtin_addressof(st))<0)
		throw_posix_error();
	return struct_stat_to_posix_file_status(st);
}

}

template<std::integral ch_type>
inline posix_file_status status(basic_posix_io_observer<ch_type> piob)
{
#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__)
	return status(static_cast<basic_win32_io_observer<ch_type>>(piob));
#else
	return details::fstat_impl(piob.fd);
#endif
}

#endif

#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__)
template<std::integral ch_type>
inline auto redirect_handle(basic_posix_io_observer<ch_type> h) noexcept
{
#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__))
	return my_get_osfile_handle(h.fd);
#else
	return h.fd;
#endif
}
#else


template<std::integral ch_type>
inline constexpr posix_io_redirection redirect(basic_posix_io_observer<ch_type> h) noexcept
{
	return {.fd=h.fd};
}

#endif

namespace details
{

#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__)

template<posix_open_mode_text_behavior behavior>
inline int open_fd_from_handle_impl(void* handle,open_mode md)
{
	int fd{::fast_io::noexcept_call(_open_osfhandle,reinterpret_cast<std::intptr_t>(handle),
		details::calculate_posix_open_mode_for_win32_handle<behavior>(md))};
	if(fd==-1)
		throw_posix_error();
	return fd;
}

template<std::integral ch_type>
inline int open_fd_from_handle(void* handle,open_mode md)
{
	if constexpr(is_ebcdic<ch_type>)
		return open_fd_from_handle_impl<posix_open_mode_text_behavior::always_binary>(handle,md);
	else if constexpr(std::same_as<ch_type,char>)
		return open_fd_from_handle_impl<posix_open_mode_text_behavior::text>(handle,md);
	else if constexpr(std::same_as<ch_type,char8_t>)
		return open_fd_from_handle_impl<posix_open_mode_text_behavior::text>(handle,md);
	else if constexpr(std::same_as<ch_type,wchar_t>)
		return open_fd_from_handle_impl<posix_open_mode_text_behavior::wide_text>(handle,md);
	else if constexpr(std::same_as<ch_type,char16_t>)
		return open_fd_from_handle_impl<posix_open_mode_text_behavior::u16_text>(handle,md);
	else
		return open_fd_from_handle_impl<posix_open_mode_text_behavior::always_binary>(handle,md);
}

#else
#if defined(__NEWLIB__)||defined(__MSDOS__) || defined(_PICOLIBC__)

template<bool always_terminate=false>
inline int my_posix_openat(int,char const*,int,mode_t)
{
	if constexpr(always_terminate)
		fast_terminate();
	else
		throw_posix_error(EINVAL);
}
#else
template<bool always_terminate=false>
inline int my_posix_openat(int dirfd,char const* pathname,int flags,mode_t mode)
{
	int fd{
#if defined(__linux__)
	system_call<__NR_openat,int>
#else
	::openat
#endif
	(dirfd,pathname,flags,mode)};
	system_call_throw_error<always_terminate>(fd);
	return fd;
}
#endif

#if defined(__CYGWIN__)

#if (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
extern int my_cygwin_attach_handle_to_fd(char const* name, int fd, void* handle, int bin, int access) noexcept
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
__asm__("cygwin_attach_handle_to_fd")
#else
__asm__("_cygwin_attach_handle_to_fd")
#endif
#else
__asm__("cygwin_attach_handle_to_fd")
#endif
;

inline constexpr unsigned calculate_win32_cygwin_open_mode(open_mode value)
{
	unsigned access{};
	if((value&open_mode::out)==open_mode::out)
		access|=0x80000000;
	if((value&open_mode::in)==open_mode::in)
		access|=0x40000000;
	return access;
}

inline int cygwin_create_fd_with_win32_handle(void* handle,open_mode mode)
{
	int fd{my_cygwin_attach_handle_to_fd(nullptr,-1,handle,true,static_cast<int>(calculate_win32_cygwin_open_mode(mode)))};
	if(fd==-1)
		throw_posix_error();
	return fd;
}

#endif

#ifdef __MSDOS__
extern unsigned int my_dos_creat(char const*,short unsigned,int*) noexcept __asm__("__dos_creat");
extern unsigned int my_dos_creatnew(char const*,short unsigned,int*) noexcept __asm__("__dos_creatnew");
extern unsigned int my_dos_open(char const*,short unsigned,int*) noexcept __asm__("__dos_open");
#endif
template<bool always_terminate=false>
inline int my_posix_open(char const* pathname,int flags,
#if __has_cpp_attribute(maybe_unused)
[[maybe_unused]]
#endif
mode_t mode)
{
#ifdef __MSDOS__
/*
Referenced from
https://dl.acm.org/doi/pdf/10.1145/70931.70935?casa_token=rWDy5JyhhkMAAAAA:BdkF0zbbWgurns3mU3yEJI2HnHXWhe6wyYGtKxjRewlEgLg6lk-cGGNLZTTdr3vUjtFg6Cnia2b4
An Example of Multiple Inheritance in C++: A Model of the Iostream Library
*/
	int fd{-1};
	unsigned int ret{};
	if(((flags&O_CREAT)==O_CREAT))
	{
		if((flags&O_EXCL)!=O_EXCL)
			ret=my_dos_creat(pathname,0,__builtin_addressof(fd));
		else
			ret=my_dos_creatnew(pathname,0,__builtin_addressof(fd));
	}
	else
		ret=my_dos_open(pathname,static_cast<short unsigned>(static_cast<unsigned>(flags)),__builtin_addressof(fd));
	if(ret)
	{
		if constexpr(always_terminate)
			fast_terminate();
		else
			throw_posix_error();
	}
	return fd;
#elif (defined(__NEWLIB__) && !defined(AT_FDCWD)) || defined(_PICOLIBC__)
	int fd{::open(pathname,flags,mode)};
	system_call_throw_error<always_terminate>(fd);
	return fd;
#else
	return my_posix_openat<always_terminate>(AT_FDCWD,pathname,flags,mode);
#endif
}


inline int my_posix_openat_file_internal_impl(int dirfd,char const* filepath,open_mode om,perms pm)
{
	return my_posix_openat(dirfd,filepath,details::calculate_posix_open_mode(om),static_cast<mode_t>(pm));
}

struct my_posix_at_open_paramter
{
	int dirfd{-1};
	int om{};
	mode_t pm{};
	inline int operator()(char const* filename) const
	{
		return my_posix_openat(dirfd,filename,om,pm);
	}
};

struct my_posix_open_paramter
{
	int om{};
	mode_t pm{};
	inline int operator()(char const* filename) const
	{
		return my_posix_open(filename,om,pm);
	}
};

#if defined(__MSDOS__) || (defined(__NEWLIB__) && !defined(AT_FDCWD)) || defined(_PICOLIBC__)

template<::fast_io::constructible_to_os_c_str T>
inline constexpr int posix_openat_file_impl(int,T const&,open_mode,perms)
{
	throw_posix_error(EINVAL);
	return -1;
}

#else
template<::fast_io::constructible_to_os_c_str T>
inline constexpr int posix_openat_file_impl(int dirfd,T const& t,open_mode om,perms pm)
{
	return ::fast_io::posix_api_common(t,my_posix_at_open_paramter{dirfd,::fast_io::details::calculate_posix_open_mode(om),static_cast<mode_t>(pm)});
}
#endif

template<::fast_io::constructible_to_os_c_str T>
inline constexpr int posix_open_file_impl(T const& t,open_mode om,perms pm)
{
#if defined(__MSDOS__) || (defined(__NEWLIB__) && !defined(AT_FDCWD)) || defined(_PICOLIBC__)
	return ::fast_io::posix_api_common(t,my_posix_open_paramter{::fast_io::details::calculate_posix_open_mode(om),static_cast<mode_t>(pm)});
#else
	return ::fast_io::posix_api_common(t,my_posix_at_open_paramter{AT_FDCWD,::fast_io::details::calculate_posix_open_mode(om),static_cast<mode_t>(pm)});
#endif
}

#endif

inline int my_open_posix_fd_temp_file()
{
#if (defined(_WIN32) && !defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__)
	::fast_io::basic_win32_file<char> wf(::fast_io::io_temp);
	int fd{::fast_io::noexcept_call(_open_osfhandle,reinterpret_cast<std::intptr_t>(wf.handle),_O_BINARY)};
	if(fd==-1)
		throw_posix_error();
	wf.release();
	return fd;
#elif defined(__CYGWIN__)
/*
This implementation is not correct since cygwin runtime changes the tmp environment and it causes issues.
*/
	::fast_io::basic_win32_file<char> wf(::fast_io::io_temp);
	constexpr int flag{static_cast<int>(0x80000000|0x40000000)};
	int fd{my_cygwin_attach_handle_to_fd(nullptr,-1,wf.handle,true,flag)};
	if(fd==-1)
		throw_posix_error();
	wf.release();
	return fd;
#elif defined(O_TMPFILE)&&defined(__linux__)
	int fd{system_call<__NR_openat,int>(AT_FDCWD,u8"/tmp",O_EXCL|O_RDWR|O_TMPFILE|O_APPEND|O_NOATIME,S_IRUSR | S_IWUSR)};
	system_call_throw_error(fd);
	return fd;
#else
	throw_posix_error(EINVAL);
#endif
}

}

struct
#if __has_cpp_attribute(clang::trivially_relocatable)
[[clang::trivially_relocatable]]
#endif
posix_file_factory
{
	using native_handle_type = int;
	int fd{-1};
	explicit constexpr posix_file_factory(int v) noexcept:fd(v){};
	posix_file_factory(posix_file_factory const&)=delete;
	posix_file_factory& operator=(posix_file_factory const&)=delete;
	~posix_file_factory()
	{
		if(fd!=-1)[[likely]]
			details::sys_close(fd);
	}
};

template<std::integral ch_type>
class basic_posix_file:public basic_posix_io_observer<ch_type>
{
public:
	using typename basic_posix_io_observer<ch_type>::char_type;
	using typename basic_posix_io_observer<ch_type>::input_char_type;
	using typename basic_posix_io_observer<ch_type>::output_char_type;
	using typename basic_posix_io_observer<ch_type>::native_handle_type;
	using file_factory_type = posix_file_factory;
	using basic_posix_io_observer<ch_type>::native_handle;
	constexpr basic_posix_file() noexcept = default;
	template<typename native_hd>
	requires std::same_as<native_handle_type,std::remove_cvref_t<native_hd>>
	explicit constexpr basic_posix_file(native_hd fd1) noexcept: basic_posix_io_observer<ch_type>{fd1}{}

	basic_posix_file(io_dup_t,basic_posix_io_observer<ch_type> piob):basic_posix_io_observer<ch_type>{details::sys_dup(piob.fd)}
	{}
	explicit constexpr basic_posix_file(posix_file_factory&& factory) noexcept: basic_posix_io_observer<ch_type>{factory.fd}
	{
		factory.fd=-1;
	}
	constexpr basic_posix_file(decltype(nullptr)) noexcept = delete;
#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__)
//windows specific. open posix file from win32/nt/zw file
	template<win32_family family>
	basic_posix_file(basic_win32_family_file<family,char_type>&& hd,open_mode m):
		basic_posix_io_observer<char_type>{details::open_fd_from_handle<ch_type>(hd.handle,m)}
	{
		hd.release();
	}
	template<nt_family family>
	basic_posix_file(basic_nt_family_file<family,char_type>&& hd,open_mode m):
		basic_posix_io_observer<char_type>{details::open_fd_from_handle<ch_type>(hd.handle,m)}
	{
		hd.release();
	}
	basic_posix_file(nt_fs_dirent fsdirent,open_mode om,perms pm=static_cast<perms>(436)):
		basic_posix_file(basic_win32_file<char_type>(fsdirent,om,pm),om){}
	template<::fast_io::constructible_to_os_c_str T>
	basic_posix_file(T const& file,open_mode om,perms pm=static_cast<perms>(436)):
		basic_posix_file(basic_win32_file<char_type>(file,om,pm),om)
	{}
	template<::fast_io::constructible_to_os_c_str T>
	basic_posix_file(nt_at_entry nate,T const& file,open_mode om,perms pm=static_cast<perms>(436)):
		basic_posix_file(basic_win32_file<char_type>(nate,file,om,pm),om)
	{}
	template<nt_family family>
	explicit constexpr basic_posix_file(io_construct_t,basic_nt_family_io_observer<family,char_type> hd,open_mode m) noexcept:
		basic_posix_io_observer<char_type>{details::open_fd_from_handle<ch_type>(hd.handle,m)}
	{}
	template<win32_family family>
	explicit constexpr basic_posix_file(io_construct_t,basic_win32_family_io_observer<family,char_type> hd,open_mode m) noexcept:
		basic_posix_io_observer<char_type>{details::open_fd_from_handle<ch_type>(hd.handle,m)}
	{}
#else

#if defined(__CYGWIN__)
	template<win32_family family>
	basic_posix_file(basic_win32_family_file<family,char_type>&& hd,open_mode m):
		basic_posix_io_observer<char_type>{details::cygwin_create_fd_with_win32_handle(hd.handle,m)}
	{
		hd.release();
	}
	template<nt_family family>
	basic_posix_file(basic_nt_family_file<family,char_type>&& hd,open_mode m):
		basic_posix_io_observer<char_type>{details::cygwin_create_fd_with_win32_handle(hd.handle,m)}
	{
		hd.release();
	}
#endif
	basic_posix_file(posix_fs_dirent fsdirent,open_mode om,perms pm=static_cast<perms>(436)):
		basic_posix_file(details::my_posix_openat_file_internal_impl(fsdirent.fd,fsdirent.filename,om,pm)){}
#if !defined(__wasi__) && __has_include(<sys/socket.h>) && __has_include(<netinet/in.h>)
	basic_posix_file(sock_family d,sock_type t,open_mode m,sock_protocol p):basic_posix_io_observer<char_type>{::fast_io::details::open_socket_impl(d,t,m,p)}{}
#endif

	template<::fast_io::constructible_to_os_c_str T>
	explicit basic_posix_file(T const& filename,open_mode om,perms pm=static_cast<perms>(436)):
			basic_posix_io_observer<char_type>{::fast_io::details::posix_open_file_impl(filename,om,pm)}
	{}

	template<::fast_io::constructible_to_os_c_str T>
	explicit basic_posix_file(posix_at_entry pate,T const& filename,open_mode om,perms pm=static_cast<perms>(436)):
			basic_posix_io_observer<char_type>{::fast_io::details::posix_openat_file_impl(pate.fd,filename,om,pm)}
	{}

#endif
	basic_posix_file(io_temp_t):basic_posix_io_observer<char_type>{::fast_io::details::my_open_posix_fd_temp_file()}{}

	constexpr basic_posix_file(basic_posix_io_observer<ch_type>) noexcept=delete;
	constexpr basic_posix_file& operator=(basic_posix_io_observer<ch_type>) noexcept=delete;

	basic_posix_file(basic_posix_file const& dp):basic_posix_io_observer<ch_type>{details::sys_dup(dp.fd)}
	{
	}
	basic_posix_file& operator=(basic_posix_file const& dp)
	{
		this->fd=details::sys_dup2(dp.fd,this->fd);
		return *this;
	}
	constexpr basic_posix_file(basic_posix_file&& __restrict b) noexcept : basic_posix_io_observer<ch_type>{b.fd}
	{
		b.fd = -1;
	}
	basic_posix_file& operator=(basic_posix_file&& __restrict b) noexcept
	{
		if(this->fd!=-1)[[likely]]
			details::sys_close(this->fd);
		this->fd=b.fd;
		b.fd = -1;
		return *this;
	}
	inline constexpr void reset(native_handle_type newfd=-1) noexcept
	{
		if(this->fd!=-1)[[likely]]
			details::sys_close(this->fd);
		this->fd=newfd;
	}
	void close()
	{
		if(this->fd!=-1)[[likely]]
			details::sys_close_throw_error(this->fd);
	}
	~basic_posix_file()
	{
		if(this->fd!=-1)[[likely]]
			details::sys_close(this->fd);
	}
};
#if (!defined(__NEWLIB__) || defined(__CYGWIN__))&&!defined(_PICOLIBC__)

namespace details
{

#if defined(__CYGWIN__)
#if (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if __has_cpp_attribute(__gnu__::__cdecl__)
[[__gnu__::__cdecl__]]
#endif
extern int ftruncate(int, off_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("ftruncate")
#else
__asm__("_ftruncate")
#endif
#else
__asm__("ftruncate")
#endif
#endif
;
#endif

inline void posix_truncate_impl(int fd,::fast_io::uintfpos_t size)
{
#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__)
#if (!defined(__MINGW32__) || __has_include(<_mingw_stat64.h>))
	if(size>static_cast<std::uint_least64_t>(std::numeric_limits<std::int_least64_t>::max()))
		throw_posix_error(EINVAL);
	auto err(noexcept_call(_chsize_s,fd,static_cast<std::int_least64_t>(size)));
	if(err)
		throw_posix_error(err);
#else
	if(size>std::numeric_limits<long>::max())
		throw_posix_error(EINVAL);
	auto err(noexcept_call(_chsize,fd,static_cast<long>(size)));
	if(err)
		throw_posix_error(err);
#endif

#elif defined(__linux__) && defined(__NR_ftruncate64)
	system_call_throw_error(system_call<__NR_ftruncate64,int>(fd,size));
#elif defined(__linux__) && defined(__NR_ftruncate)
	system_call_throw_error(system_call<__NR_ftruncate,int>(fd,size));
#else
	if(noexcept_call(ftruncate,fd,static_cast<off_t>(size))<0)
		throw_posix_error();
#endif
}

}

template<std::integral ch_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#endif
inline void truncate(basic_posix_io_observer<ch_type> h,::fast_io::uintfpos_t size)
{
	details::posix_truncate_impl(h.fd,size);
}
#endif

template<std::integral ch_type>
class basic_posix_pipe
{
public:
	using char_type = ch_type;
	basic_posix_file<ch_type> pipes[2];
	basic_posix_pipe()
	{
#if defined(__wasi__)
		throw_posix_error(ENOTSUP);
#else
		int a2[2]{-1,-1};
#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__)
		if(noexcept_call(_pipe,a2,131072u,_O_BINARY)==-1)
#else
		if(noexcept_call(::pipe,a2)==-1)
#endif
			throw_posix_error();
		pipes->fd=*a2;
		pipes[1].fd=a2[1];
#endif
	}
	constexpr auto& in() noexcept
	{
		return *pipes;
	}
	constexpr auto& out() noexcept
	{
		return pipes[1];
	}
};


template<std::integral ch_type>
inline constexpr basic_posix_io_observer<ch_type> input_stream_ref_define(basic_posix_pipe<ch_type>& pp) noexcept
{
	return {pp.in().fd};
}

template<std::integral ch_type>
inline constexpr basic_posix_io_observer<char> input_bytes_stream_ref_define(basic_posix_pipe<ch_type>& pp) noexcept
{
	return {pp.in().fd};
}

template<std::integral ch_type>
inline constexpr basic_posix_io_observer<ch_type> output_stream_ref_define(basic_posix_pipe<ch_type>& pp) noexcept
{
	return {pp.out().fd};
}

template<std::integral ch_type>
inline constexpr basic_posix_io_observer<char> output_bytes_stream_ref_define(basic_posix_pipe<ch_type>& pp) noexcept
{
	return {pp.out().fd};
}

#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__)
#if 0
template<std::integral ch_type>
inline ::fast_io::freestanding::array<int*,2> redirect_handle(basic_posix_pipe<ch_type>& h)
{
	return {__builtin_addressof(h.in().fd),
		__builtin_addressof(h.out().fd)};
}
#endif
#else
template<std::integral ch_type>
inline constexpr posix_io_redirection redirect(basic_posix_pipe<ch_type>& h) noexcept
{
	return {.pipe_fds=__builtin_addressof(h.in().fd)};
}
#endif

using posix_io_observer=basic_posix_io_observer<char>;
using posix_file=basic_posix_file<char>;
using posix_pipe=basic_posix_pipe<char>;

using wposix_io_observer=basic_posix_io_observer<wchar_t>;
using wposix_file=basic_posix_file<wchar_t>;
using wposix_pipe=basic_posix_pipe<wchar_t>;

using u8posix_io_observer=basic_posix_io_observer<char8_t>;
using u8posix_file=basic_posix_file<char8_t>;
using u8posix_pipe=basic_posix_pipe<char8_t>;

using u16posix_io_observer=basic_posix_io_observer<char16_t>;
using u16posix_file=basic_posix_file<char16_t>;
using u16posix_pipe=basic_posix_pipe<char16_t>;

using u32posix_io_observer=basic_posix_io_observer<char32_t>;
using u32posix_file=basic_posix_file<char32_t>;
using u32posix_pipe=basic_posix_pipe<char32_t>;

inline constexpr int posix_stdin_number{0};
inline constexpr int posix_stdout_number{1};
inline constexpr int posix_stderr_number{2};

template<std::integral char_type=char>
inline constexpr basic_posix_io_observer<char_type> posix_stdin()
{
	return {posix_stdin_number};
}
template<std::integral char_type=char>
inline constexpr basic_posix_io_observer<char_type> posix_stdout()
{
	return basic_posix_io_observer<char_type>{posix_stdout_number};
}
template<std::integral char_type=char>
inline constexpr basic_posix_io_observer<char_type> posix_stderr()
{
	return basic_posix_io_observer<char_type>{posix_stderr_number};
}


#if !defined(_WIN32) || defined(__WINE__)
template<std::integral char_type=char>
inline constexpr basic_posix_io_observer<char_type> native_stdin() noexcept
{
	return {posix_stdin_number};
}
template<std::integral char_type=char>
inline constexpr basic_posix_io_observer<char_type> native_stdout() noexcept
{
	return basic_posix_io_observer<char_type>{posix_stdout_number};
}
template<std::integral char_type=char>
inline constexpr basic_posix_io_observer<char_type> native_stderr() noexcept
{
	return basic_posix_io_observer<char_type>{posix_stderr_number};
}
#endif
}

#if defined(__linux__) && (defined(__NR_sendfile) || defined(__NR_sendfile64)) && 0
#include"linux_zerocopy.h"
#endif

#include"posix/impl.h"
