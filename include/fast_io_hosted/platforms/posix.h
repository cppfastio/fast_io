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
inline constexpr basic_posix_io_observer<ch_type> io_value_handle(basic_posix_io_observer<ch_type> other) noexcept
{
	return other;
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

namespace details
{

inline std::size_t posix_read_impl(int fd,void* address,std::size_t bytes_to_read)
{
#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__)
	if constexpr(4<sizeof(std::size_t))
		if(static_cast<std::size_t>(INT_LEAST32_MAX)<bytes_to_read)
			bytes_to_read=static_cast<std::size_t>(INT_LEAST32_MAX);
#endif
	auto read_bytes(
#if defined(__linux__)
		system_call<__NR_read,std::ptrdiff_t>(
#elif (defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__)) || defined(__MSDOS__)
		::fast_io::noexcept_call(::_read,
#else
		::fast_io::noexcept_call(::read,
#endif
	fd,address,
#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__)
	static_cast<std::uint_least32_t>(bytes_to_read)
#else
	bytes_to_read
#endif
	));
	system_call_throw_error(read_bytes);
	return static_cast<std::size_t>(read_bytes);
}

#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__)

inline io_scatter_status_t posix_scatter_read_impl(int fd,io_scatters_t sp)
{
	std::size_t total_size{};
	for(std::size_t i{};i!=sp.len;++i)
	{
		std::size_t pos_in{posix_read_impl(fd,const_cast<void*>(sp.base[i].base),sp.base[i].len)};
		total_size+=pos_in;
		if(pos_in<sp.base[i].len)[[unlikely]]
			return {total_size,i,pos_in};
	}
	return {total_size,sp.len,0};
}

inline std::uint_least32_t posix_write_simple_impl(int fd,void const* address,std::size_t bytes_to_write)
{
	auto ret{noexcept_call(_write,fd,address,static_cast<std::uint_least32_t>(bytes_to_write))};
	if(ret==-1)
		throw_posix_error();
	return static_cast<std::uint_least32_t>(ret);
}

inline std::size_t posix_write_nolock_impl(int fd,void const* address,std::size_t to_write)
{
	if constexpr(4<sizeof(std::size_t))		//above the size of std::uint_least32_t, unfortunately, we cannot guarantee the atomicity of syscall
	{
		std::size_t written{};
		for(;to_write;)
		{
			std::uint_least32_t to_write_this_round{INT_LEAST32_MAX};
			if(to_write<static_cast<std::size_t>(INT_LEAST32_MAX))
				to_write_this_round=static_cast<std::uint_least32_t>(to_write);
			std::uint_least32_t number_of_bytes_written{posix_write_simple_impl(fd,address,to_write_this_round)};
			written+=number_of_bytes_written;
			if(number_of_bytes_written<to_write_this_round)
				break;
			to_write-=to_write_this_round;
		}
		return written;
	}
	else
		return posix_write_simple_impl(fd,address,to_write);
}

inline std::size_t posix_write_lock_impl(int fd,void const* address,std::size_t to_write)
{
	return posix_write_nolock_impl(fd,address,to_write);
}

inline io_scatter_status_t posix_scatter_write_impl(int fd,io_scatters_t sp)
{
	std::size_t total_size{};
	for(std::size_t i{};i!=sp.len;++i)
	{
		std::size_t written{posix_write_nolock_impl(fd,sp.base[i].base,sp.base[i].len)};
		total_size+=written;
		if(sp.base[i].len<written)[[unlikely]]
			return {total_size,i,written};
	}
	return {total_size,sp.len,0};
}
#endif

inline std::size_t posix_write_impl(int fd,void const* address,std::size_t to_write)
{
#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__)
	if constexpr(4<sizeof(std::size_t))
		return posix_write_nolock_impl(fd,address,to_write);
	else
		return posix_write_simple_impl(fd,address,to_write);
#elif defined(__wasi__)
	__wasi_ciovec_t iov{.buf = reinterpret_cast<char unsigned const*>(address), .buf_len = to_write};
	std::size_t bytes_written;
	auto ern{noexcept_call(__wasi_fd_write,fd, __builtin_addressof(iov), 1u, __builtin_addressof(bytes_written))};
	if(ern)
		throw_posix_error(ern);
	return bytes_written;
#else
	auto write_bytes(
#if defined(__linux__)
		system_call<__NR_write,std::ptrdiff_t>(
#elif (defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__)) || defined(__MSDOS__)
		::fast_io::noexcept_call(::_write,
#else
		::fast_io::noexcept_call(::write,
#endif
	fd,address,to_write));
	system_call_throw_error(write_bytes);
	return static_cast<std::size_t>(write_bytes);
#endif
}

inline std::uintmax_t posix_seek_impl(int fd,std::intmax_t offset,seekdir s)
{
#if defined(__MSDOS__)
	if constexpr(sizeof(off_t)<sizeof(std::intmax_t))
	{
		if(offset<static_cast<std::intmax_t>(std::numeric_limits<off_t>::min())||offset>static_cast<std::intmax_t>(std::numeric_limits<off_t>::max()))
			throw_posix_error(EINVAL);
	}
	auto ret{noexcept_call(::lseek,fd,static_cast<off_t>(offset),static_cast<int>(s))};
	if(ret==-1)
		throw_posix_error();
	return static_cast<std::uintmax_t>(static_cast<my_make_unsigned_t<off_t>>(ret));
#elif defined(__linux__)
#if defined(__NR_llseek)
	if constexpr(sizeof(off_t)<=sizeof(std::int_least32_t))
	{
		std::uint_least64_t result{};
		std::uint_least64_t offset64_val{static_cast<std::uint_least64_t>(static_cast<std::uintmax_t>(offset))};
		auto ret{system_call<__NR_llseek,int>(fd,static_cast<std::uint_least32_t>(offset>>32u),static_cast<std::uint_least32_t>(offset),
			__builtin_addressof(result),static_cast<int>(s))};
		system_call_throw_error(ret);
		return static_cast<std::uintmax_t>(static_cast<std::uint_least64_t>(result));
	}
	else
#endif
	{
		if constexpr(sizeof(off_t)<=sizeof(std::int_least32_t))
		{
			if(offset<static_cast<std::intmax_t>(std::numeric_limits<off_t>::min())||offset>static_cast<std::intmax_t>(std::numeric_limits<off_t>::max()))
				throw_posix_error(EOVERFLOW);
		}
		auto ret{system_call<__NR_lseek,std::ptrdiff_t>(fd,offset,static_cast<int>(s))};
		system_call_throw_error(ret);
		return static_cast<std::uintmax_t>(static_cast<std::uint_least64_t>(ret));
	}
#else
	if constexpr(sizeof(off_t)<=sizeof(std::int_least32_t))
	{
		if(offset<static_cast<std::intmax_t>(std::numeric_limits<off_t>::min())||offset>static_cast<std::intmax_t>(std::numeric_limits<off_t>::max()))
			throw_posix_error(EOVERFLOW);
	}
	auto ret(
#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__)
		::_lseeki64
#else
		::lseek
#endif
		(fd,static_cast<off_t>(offset),static_cast<int>(s)));
	system_call_throw_error(ret);
	return static_cast<std::uintmax_t>(static_cast<std::uint_least64_t>(ret));
#endif
}

inline bool posix_is_character_device(int fd) noexcept
{
#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__))
	return noexcept_call(::_isatty,fd)!=0;
#else
	return noexcept_call(::isatty,fd)!=0;
#endif
}
#ifdef __MSDOS__
extern void dos_clrscr() noexcept __asm__("_clrscr");
#endif
inline void posix_clear_screen_main([[maybe_unused]] int fd)
{
#ifdef __MSDOS__
	dos_clrscr();
#else
	constexpr std::size_t sequence_len{2};
	constexpr char8_t const clear_screen_control[2] {u8'\033',u8'c'};
	posix_write_impl(fd,clear_screen_control,sequence_len);
#endif
}

inline void posix_clear_screen_impl(int fd)
{
#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__)
	::fast_io::win32::details::win32_clear_screen_impl(my_get_osfile_handle(fd));
#else
	if(!posix_is_character_device(fd))
		return;
	posix_clear_screen_main(fd);
#endif
}

inline void posix_flush_impl(int fd)
{
#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__)
	if(noexcept_call(_commit,fd)==-1)
		throw_posix_error();
#elif defined(__linux__) && defined(__NR_fdatasync)
	system_call_throw_error(system_call<__NR_fdatasync,int>(fd));
#elif defined(__linux__) && defined(__NR_fsync)
	system_call_throw_error(system_call<__NR_fsync,int>(fd));
#else
	if(noexcept_call(fsync,fd)==-1)
		throw_posix_error();
#endif
}

inline void posix_data_sync_flags_impl(int fd,data_sync_flags)
{
//todo
	posix_flush_impl(fd);
}

}

template<std::integral ch_type,::std::contiguous_iterator Iter>
[[nodiscard]] inline Iter read(basic_posix_io_observer<ch_type> h,Iter begin,Iter end)
{
	return begin+details::posix_read_impl(h.fd,::std::to_address(begin),static_cast<std::size_t>(end-begin)*sizeof(*begin))/sizeof(*begin);
}
template<std::integral ch_type,::std::contiguous_iterator Iter>
inline Iter write(basic_posix_io_observer<ch_type> h,Iter cbegin,Iter cend)
{
	return cbegin+details::posix_write_impl(h.fd,::std::to_address(cbegin),static_cast<std::size_t>(cend-cbegin)*sizeof(*cbegin))/sizeof(*cbegin);
}

template<std::integral ch_type>
inline std::uintmax_t seek(basic_posix_io_observer<ch_type> h,std::intmax_t i=0,seekdir s=seekdir::cur)
{
	return details::posix_seek_impl(h.fd,i,s);
}

template<std::integral ch_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#endif
inline void flush(basic_posix_io_observer<ch_type> piob)
{
	details::posix_flush_impl(piob.fd);
}

template<std::integral ch_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#endif
inline void data_sync(basic_posix_io_observer<ch_type> piob,data_sync_flags flags)
{
	details::posix_data_sync_flags_impl(piob.fd,flags);
}
#if defined(_WIN32)&& !defined(__WINE__) && !defined(__CYGINW__)

template<std::integral ch_type>
inline constexpr auto file_lock(basic_posix_io_observer<ch_type> piob) noexcept
{
	return file_lock(::fast_io::basic_win32_io_observer<ch_type>{::fast_io::details::my_get_osfile_handle(piob.fd)});
}

#elif defined(F_RDLCK) && defined(F_WRLCK) && defined(F_UNLCK) && !defined(__wasi__)

template<std::integral ch_type>
inline constexpr posix_file_lock file_lock(basic_posix_io_observer<ch_type> piob) noexcept
{
	return {piob.fd};
}

#else

template<std::integral ch_type>
inline constexpr nop_file_lock file_lock(basic_posix_io_observer<ch_type>) noexcept
{
	return {};
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
	return {static_cast<std::uintmax_t>(st.st_dev),
	static_cast<std::uintmax_t>(st.st_ino),
	st_mode_to_perms(st.st_mode),
	st_mode_to_file_type(st.st_mode),
	static_cast<std::uintmax_t>(st.st_nlink),
	static_cast<std::uintmax_t>(st.st_uid),
	static_cast<std::uintmax_t>(st.st_gid),
	static_cast<std::uintmax_t>(st.st_rdev),
	static_cast<std::uintmax_t>(st.st_size),
#if ((defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__))||defined(__MSDOS__)
	131072,
	static_cast<std::uintmax_t>(st.st_size/512),
	{st.st_atime,{}},{st.st_mtime,{}},{st.st_ctime,{}},{0,0},
#elif !defined(__CYGWIN__) && (defined(__BSD_VISIBLE) || defined(__DARWIN_C_LEVEL)) && !defined(_PICOLIBC__) && !defined(__linux__)
	static_cast<std::uintmax_t>(st.st_blksize),
	static_cast<std::uintmax_t>(st.st_blocks),
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
	static_cast<std::uintmax_t>(st.st_blksize),
	static_cast<std::uintmax_t>(st.st_blocks),
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
#if !defined(__CYGWIN__) && (defined(__BSD_VISIBLE) || defined(__DARWIN_C_LEVEL)) && !defined(_PICOLIBC__) && !defined(__linux__)
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

#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__)
template<std::integral ch_type,typename... Args>
requires io_controllable<basic_win32_io_observer<ch_type>,Args...>
inline decltype(auto) io_control(basic_posix_io_observer<ch_type> h,Args&& ...args)
{
	return io_control(static_cast<basic_win32_io_observer<ch_type>>(h),::std::forward<Args>(args)...);
}
#else

namespace posix
{
extern int ioctl(int, long unsigned, ...) noexcept __asm__("ioctl");
}


template<std::integral ch_type,typename... Args>
requires requires(basic_posix_io_observer<ch_type> h,Args&& ...args)
{
	::fast_io::posix::ioctl(h.fd,::std::forward<Args>(args)...);
}
inline void io_control(basic_posix_io_observer<ch_type> h,Args&& ...args)
{
#if defined(__linux__) && defined(__NR_ioctl)
	system_call_throw_error(system_call<__NR_ioctl,int>(h.fd,::std::forward<Args>(args)...));
#else
	if(::fast_io::posix::ioctl(h.fd,::std::forward<Args>(args)...)==-1)
		throw_posix_error();
#endif
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

inline void posix_truncate_impl(int fd,std::uintmax_t size)
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
inline void truncate(basic_posix_io_observer<ch_type> h,std::uintmax_t size)
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

template<std::integral ch_type,::std::contiguous_iterator Iter>
inline Iter read(basic_posix_pipe<ch_type>& h,Iter begin,Iter end)
{
	return read(h.in(),begin,end);
}
template<std::integral ch_type,::std::contiguous_iterator Iter>
inline Iter write(basic_posix_pipe<ch_type>& h,Iter begin,Iter end)
{
	return write(h.out(),begin,end);
}

template<std::integral ch_type>
inline void flush(basic_posix_pipe<ch_type>& pp)
{
	details::posix_flush_impl(pp.out().fd);
	details::posix_flush_impl(pp.in().fd);
}

#if (defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__CYGWIN__)
template<std::integral ch_type>
inline ::fast_io::freestanding::array<int*,2> redirect_handle(basic_posix_pipe<ch_type>& h)
{
	return {__builtin_addressof(h.in().fd),
		__builtin_addressof(h.out().fd)};
}
#else
template<std::integral ch_type>
inline constexpr posix_io_redirection redirect(basic_posix_pipe<ch_type>& h) noexcept
{
	return {.pipe_fds=__builtin_addressof(h.in().fd)};
}
#endif

template<std::integral ch_type>
inline bool is_character_device(basic_posix_io_observer<ch_type> piob) noexcept
{
	return details::posix_is_character_device(piob.fd);
}

template<std::integral ch_type>
inline void clear_screen(basic_posix_io_observer<ch_type> piob)
{
	details::posix_clear_screen_impl(piob.fd);
}


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

#if defined(__CYGWIN__) || (!(defined(_WIN32)&&!defined(__WINE__)&&!defined(__BIONIC__)) && !defined(__MSDOS__) && !defined(_PICOLIBC__))
namespace details
{

#if defined(__NEWLIB__) && !defined(__CYGWIN__)
extern std::ptrdiff_t newlib_writev(int fd, struct iovec const* iov, int iovcnt) __asm__("writev");
extern std::ptrdiff_t newlib_readv(int fd, struct iovec const* iov, int iovcnt) __asm__("readv");
#endif

inline std::size_t posix_scatter_read_size_impl(int fd,io_scatters_t sp)
{
#if defined(__linux__)
	static_assert(sizeof(unsigned long)==sizeof(std::size_t));
	auto val{system_call<__NR_readv,std::ptrdiff_t>(static_cast<unsigned int>(fd),sp.base,sp.len)};
	system_call_throw_error(val);
	return static_cast<std::size_t>(val);
#elif defined(__wasi__)
	using iovec_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= __wasi_iovec_t const*;
	std::size_t val{};
	auto err{noexcept_call(__wasi_fd_read,fd,
		reinterpret_cast<iovec_may_alias_const_ptr>(sp.base),sp.len,
		__builtin_addressof(val))};
	if(err)
		throw_posix_error(err);
	return val;
#else
	std::size_t sz{sp.len};
	if(static_cast<std::size_t>(std::numeric_limits<int>::max())<sz)
		sz=static_cast<std::size_t>(std::numeric_limits<int>::max());
	using iovec_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= iovec const*;
	auto ptr{reinterpret_cast<iovec_may_alias_const_ptr>(sp.base)};
#if defined(__NEWLIB__) && !defined(__CYGWIN__)
	std::ptrdiff_t val{newlib_readv(fd,ptr,static_cast<int>(sz))};
#else
	std::ptrdiff_t val{noexcept_call(::readv,fd,ptr,static_cast<int>(sz))};
#endif
	if(val<0)
		throw_posix_error();
	return static_cast<std::size_t>(val);
#endif
}

inline constexpr io_scatter_status_t scatter_size_to_status(std::size_t sz,io_scatters_t sp) noexcept
{
	std::size_t total{sz};
	for(std::size_t i{};i!=sp.len;++i)
	{
		if(total<sp.base[i].len)[[unlikely]]
			return {sz,i,total};
		total-=sp.base[i].len;
	}
	return {sz,sp.len,0};
}

#if defined(__wasi__)

#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline io_scatter_status_t wasmtime_bug_posix_scatter_write_cold(int fd,io_scatters_t sp)
{
	std::size_t total{sp.base->len};
	auto i{sp.base+1};
	auto e{sp.base+sp.len};
	for(;i!=e;++i)
	{
		std::size_t val{};
		__wasi_ciovec_t iovec{.buf = reinterpret_cast<char unsigned const*>(i->base), .buf_len = i->len};
		auto err{noexcept_call(__wasi_fd_write,fd,__builtin_addressof(iovec),1u,__builtin_addressof(val))};
		if(err)
			throw_posix_error(err);
		total+=val;
		if(val!=i->len)
			return {total,static_cast<std::size_t>(val),val};
	}
	return {total,sp.len,0};
}

struct wasmtime_bug_code
{
	std::size_t val;
	bool failed;
};

inline std::size_t wasmtime_bug_code_write_normal(int fd,io_scatters_t sp)
{
	using iovec_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= __wasi_ciovec_t const*;
	std::size_t val{};
	auto err{noexcept_call(__wasi_fd_write,fd,reinterpret_cast<iovec_may_alias_const_ptr>(sp.base),
		sp.len,__builtin_addressof(val))};
	if(err)
		throw_posix_error(err);
	return val;
}

inline io_scatter_status_t posix_scatter_wasmtime_bug_write_size_impl(int fd,io_scatters_t sp)
{
	auto val{wasmtime_bug_code_write_normal(fd,sp)};
	if(1<sp.len&&val==sp.base->len)[[unlikely]]
		return wasmtime_bug_posix_scatter_write_cold(fd,sp);
	return scatter_size_to_status(val,sp);
}
#else

inline std::size_t posix_scatter_write_size_impl(int fd,io_scatters_t sp)
{
#if defined(__linux__)
	static_assert(sizeof(unsigned long)==sizeof(std::size_t));
	auto val{system_call<__NR_writev,std::ptrdiff_t>(static_cast<unsigned int>(fd),sp.base,sp.len)};
	system_call_throw_error(val);
	return static_cast<std::size_t>(val);
#else
	std::size_t sz{sp.len};
	if(static_cast<std::size_t>(std::numeric_limits<int>::max())<sz)
		sz=static_cast<std::size_t>(std::numeric_limits<int>::max());
	using iovec_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= iovec const*;
	auto ptr{reinterpret_cast<iovec_may_alias_const_ptr>(sp.base)};
#if defined(__NEWLIB__) && !defined(__CYGWIN__)
	std::ptrdiff_t val{newlib_writev(fd,ptr,static_cast<int>(sz))};
#else
	std::ptrdiff_t val{noexcept_call(::writev,fd,ptr,static_cast<int>(sz))};
#endif
	if(val<0)
		throw_posix_error();
	return static_cast<std::size_t>(val);
#endif
}
#endif


inline io_scatter_status_t posix_scatter_write_impl(int fd,io_scatters_t sp)
{
#ifdef __wasi__
	return posix_scatter_wasmtime_bug_write_size_impl(fd,sp);
#else
	return scatter_size_to_status(posix_scatter_write_size_impl(fd,sp),sp);
#endif
}

[[nodiscard]] inline io_scatter_status_t posix_scatter_read_impl(int fd,io_scatters_t sp)
{
	return scatter_size_to_status(posix_scatter_read_size_impl(fd,sp),sp);
}

}

#endif
#endif

#if defined(__CYGWIN__) || ((!defined(__NEWLIB__)|| defined(FAST_IO_USE_NEWLIB_CUSTOM_WRITEV)) && !(defined(__BIONIC__)&&defined(_WIN32)) && !defined(__MSDOS__) && !defined(_PICOLIBC__))

template<std::integral ch_type>
[[nodiscard]] inline io_scatter_status_t scatter_read(basic_posix_io_observer<ch_type> h,io_scatters_t sp)
{
	return details::posix_scatter_read_impl(h.fd,sp);
}

template<std::integral ch_type>
inline io_scatter_status_t scatter_write(basic_posix_io_observer<ch_type> h,io_scatters_t sp)
{
	return details::posix_scatter_write_impl(h.fd,sp);
}

template<std::integral ch_type>
inline io_scatter_status_t scatter_read(basic_posix_pipe<ch_type>& h,io_scatters_t sp)
{
	return details::posix_scatter_read_impl(h.in().fd,sp);
}

template<std::integral ch_type>
inline io_scatter_status_t scatter_write(basic_posix_pipe<ch_type>& h,io_scatters_t sp)
{
	return details::posix_scatter_write_impl(h.out().fd,sp);
}

#endif

#if _XOPEN_SOURCE >= 500 ||  _POSIX_C_SOURCE >= 200809L

namespace details
{

inline std::size_t posix_pread_impl(int fd,void* address,std::size_t bytes_to_read,std::intmax_t offset)
{
	if constexpr(sizeof(std::intmax_t)>sizeof(off_t))
	{
		if(static_cast<std::intmax_t>(std::numeric_limits<off_t>::max())<offset)
			throw_posix_error(EINVAL);
	}
	auto read_bytes(
#if defined(__linux__)
		system_call<__NR_pread64,std::ptrdiff_t>
#else
		::pread
#endif
	(fd,address,bytes_to_read,static_cast<off_t>(offset)));
	system_call_throw_error(read_bytes);
	return static_cast<std::size_t>(read_bytes);
}

inline std::size_t posix_pwrite_impl(int fd,void const* address,std::size_t bytes_to_write,std::intmax_t offset)
{
	if constexpr(sizeof(std::intmax_t)>sizeof(off_t))
	{
		if(static_cast<std::intmax_t>(std::numeric_limits<off_t>::max())<offset)
			throw_posix_error(EINVAL);
	}
	auto written_bytes(
#if defined(__linux__)
		system_call<__NR_pwrite64,std::ptrdiff_t>
#else
		::pwrite
#endif
	(fd,address,bytes_to_write,static_cast<off_t>(offset)));
	system_call_throw_error(written_bytes);
	return static_cast<std::size_t>(written_bytes);
}
#if !defined(__NEWLIB__) && (__BSD_VISIBLE || _BSD_SOURCE || _DEFAULT_SOURCE)
inline std::size_t posix_scatter_pread_size_impl(int fd,io_scatters_t sp,std::intmax_t offset)
{
	if constexpr(sizeof(std::intmax_t)>sizeof(off_t))
	{
		if(static_cast<std::intmax_t>(std::numeric_limits<off_t>::max())<offset)
			throw_posix_error(EINVAL);
	}
#if defined(__linux__)
//	static_assert(sizeof(unsigned long)==sizeof(std::size_t));
	auto val{system_call<__NR_preadv,std::ptrdiff_t>(static_cast<unsigned int>(fd),sp.base,sp.len,static_cast<off_t>(offset))};
	system_call_throw_error(val);
	return static_cast<std::size_t>(val);
#else
	using iovec_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= iovec const*;
	std::size_t sz{sp.len};
	if(static_cast<std::size_t>(std::numeric_limits<int>::max())<sz)
		sz=static_cast<std::size_t>(std::numeric_limits<int>::max());
	auto ptr{reinterpret_cast<iovec_may_alias_const_ptr>(sp.base)};
	std::ptrdiff_t val{noexcept_call(::preadv,fd,ptr,static_cast<int>(sz),static_cast<off_t>(offset))};
	if(val<0)
		throw_posix_error();
	return static_cast<std::size_t>(val);
#endif
}

inline std::size_t posix_scatter_pwrite_size_impl(int fd,io_scatters_t sp,std::intmax_t offset)
{
	if constexpr(sizeof(std::intmax_t)>sizeof(off_t))
	{
		if(static_cast<std::intmax_t>(std::numeric_limits<off_t>::max())<offset)
			throw_posix_error(EINVAL);
	}
#if defined(__linux__)
//	static_assert(sizeof(unsigned long)==sizeof(std::size_t));
	auto val{system_call<__NR_pwritev,std::ptrdiff_t>(static_cast<unsigned int>(fd),sp.base,sp.len,static_cast<off_t>(offset))};
	system_call_throw_error(val);
	return static_cast<std::size_t>(val);
#else
	using iovec_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= iovec const*;
	std::size_t sz{sp.len};
	if(static_cast<std::size_t>(std::numeric_limits<int>::max())<sz)
		sz=static_cast<std::size_t>(std::numeric_limits<int>::max());
	auto ptr{reinterpret_cast<iovec_may_alias_const_ptr>(sp.base)};
	std::ptrdiff_t val{noexcept_call(::pwritev,fd,ptr,static_cast<int>(sz),static_cast<off_t>(offset))};
	if(val<0)
		throw_posix_error();
	return static_cast<std::size_t>(val);
#endif
}

inline io_scatter_status_t posix_scatter_pwrite_impl(int fd,io_scatters_t sp,std::intmax_t offset)
{
	return scatter_size_to_status(posix_scatter_pwrite_size_impl(fd,sp,offset),sp);
}

[[nodiscard]] inline io_scatter_status_t posix_scatter_pread_impl(int fd,io_scatters_t sp,std::intmax_t offset)
{
	return scatter_size_to_status(posix_scatter_pread_size_impl(fd,sp,offset),sp);
}
#endif
}

template<std::integral char_type,::std::contiguous_iterator Iter>
inline constexpr Iter pread(basic_posix_io_observer<char_type> piob,Iter begin,Iter end,std::intmax_t offset)
{
	return begin+details::posix_pread_impl(piob.fd,::std::to_address(begin),(end-begin)*sizeof(*begin),offset)/sizeof(*begin);
}

template<std::integral char_type,::std::contiguous_iterator Iter>
inline constexpr Iter pwrite(basic_posix_io_observer<char_type> piob,Iter begin,Iter end,std::intmax_t offset)
{
	return begin+details::posix_pwrite_impl(piob.fd,::std::to_address(begin),(end-begin)*sizeof(*begin),offset)/sizeof(*begin);
}
#if !defined(__NEWLIB__) && (__BSD_VISIBLE || _BSD_SOURCE || _DEFAULT_SOURCE)
template<std::integral ch_type>
[[nodiscard]] inline io_scatter_status_t scatter_pread(basic_posix_io_observer<ch_type> piob,io_scatters_t sp,std::intmax_t offset)
{
	return details::posix_scatter_pread_impl(piob.fd,sp,offset);
}

template<std::integral ch_type>
inline io_scatter_status_t scatter_pwrite(basic_posix_io_observer<ch_type> piob,io_scatters_t sp,std::intmax_t offset)
{
	return details::posix_scatter_pwrite_impl(piob.fd,sp,offset);
}
#endif
#endif
#if 0
namespace freestanding
{

template<std::integral char_type>
struct is_trivially_relocatable<basic_posix_file<fm,char_type>>
{
	inline static constexpr bool value = true;
};

}
#endif
}

#if defined(__MSDOS__) || (defined(__BIONIC__)&&defined(_WIN32)) || ((defined(__NEWLIB__)||defined(_PICOLIBC__))&&!defined(FAST_IO_USE_NEWLIB_CUSTOM_WRITEV)&&!defined(__CYGWIN__))
#include"msdos.h"
#endif

#if defined(__linux__) && (defined(__NR_sendfile) || defined(__NR_sendfile64))
#include"linux_zerocopy.h"
#endif
