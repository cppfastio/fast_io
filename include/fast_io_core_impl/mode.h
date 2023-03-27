#pragma once

namespace fast_io
{

/*
https://github.com/gcc-mirror/gcc/blob/41d6b10e96a1de98e90a7c0378437c3255814b16/libstdc%2B%2B-v3/include/bits/fs_fwd.h#L73

Being binary compatible with libstdc++'s file_type
*/
enum class file_type : signed char
{
none = 0, not_found = -1, regular = 1, directory = 2, symlink = 3,
block = 4, character = 5, fifo = 6, socket = 7, unknown = 8, remote = 9
};

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,file_type>) noexcept
{
	return 20;
}


namespace details
{
template<std::integral char_type>
inline constexpr char_type* print_file_status_impl(char_type* it,file_type f) noexcept
{
	if constexpr(std::same_as<char_type,char>)
	{
		switch(f)
		{
		case file_type::none:
			return details::copy_string_literal("none",it); 
		case file_type::not_found:
			return details::copy_string_literal("not_found",it); 
		case file_type::regular:
			return details::copy_string_literal("regular",it); 
		case file_type::directory:
			return details::copy_string_literal("directory",it); 
		case file_type::symlink:
			return details::copy_string_literal("symlink",it);
		case file_type::block:
			return details::copy_string_literal("block",it);
		case file_type::character:
			return details::copy_string_literal("character",it);
		case file_type::fifo:
			return details::copy_string_literal("fifo",it);
		case file_type::socket:
			return details::copy_string_literal("socket",it);
		case file_type::remote:
			return details::copy_string_literal("remote",it);
		default:
			return details::copy_string_literal("unknown",it);
		}
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		switch(f)
		{
		case file_type::none:
			return details::copy_string_literal(L"none",it); 
		case file_type::not_found:
			return details::copy_string_literal(L"not_found",it); 
		case file_type::regular:
			return details::copy_string_literal(L"regular",it); 
		case file_type::directory:
			return details::copy_string_literal(L"directory",it); 
		case file_type::symlink:
			return details::copy_string_literal(L"symlink",it);
		case file_type::block:
			return details::copy_string_literal(L"block",it);
		case file_type::character:
			return details::copy_string_literal(L"character",it);
		case file_type::fifo:
			return details::copy_string_literal(L"fifo",it);
		case file_type::socket:
			return details::copy_string_literal(L"socket",it);
		case file_type::remote:
			return details::copy_string_literal(L"remote",it);
		default:
			return details::copy_string_literal(L"unknown",it);
		}
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		switch(f)
		{
		case file_type::none:
			return details::copy_string_literal(u"none",it); 
		case file_type::not_found:
			return details::copy_string_literal(u"not_found",it); 
		case file_type::regular:
			return details::copy_string_literal(u"regular",it); 
		case file_type::directory:
			return details::copy_string_literal(u"directory",it); 
		case file_type::symlink:
			return details::copy_string_literal(u"symlink",it);
		case file_type::block:
			return details::copy_string_literal(u"block",it);
		case file_type::character:
			return details::copy_string_literal(u"character",it);
		case file_type::fifo:
			return details::copy_string_literal(u"fifo",it);
		case file_type::socket:
			return details::copy_string_literal(u"socket",it);
		case file_type::remote:
			return details::copy_string_literal(u"remote",it);
		default:
			return details::copy_string_literal(u"unknown",it);
		}
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		switch(f)
		{
		case file_type::none:
			return details::copy_string_literal(U"none",it); 
		case file_type::not_found:
			return details::copy_string_literal(U"not_found",it); 
		case file_type::regular:
			return details::copy_string_literal(U"regular",it); 
		case file_type::directory:
			return details::copy_string_literal(U"directory",it); 
		case file_type::symlink:
			return details::copy_string_literal(U"symlink",it);
		case file_type::block:
			return details::copy_string_literal(U"block",it);
		case file_type::character:
			return details::copy_string_literal(U"character",it);
		case file_type::fifo:
			return details::copy_string_literal(U"fifo",it);
		case file_type::socket:
			return details::copy_string_literal(U"socket",it);
		case file_type::remote:
			return details::copy_string_literal(U"remote",it);
		default:
			return details::copy_string_literal(U"unknown",it);
		}
	}
	else
	{
		switch(f)
		{
		case file_type::none:
			return details::copy_string_literal(u8"none",it); 
		case file_type::not_found:
			return details::copy_string_literal(u8"not_found",it); 
		case file_type::regular:
			return details::copy_string_literal(u8"regular",it); 
		case file_type::directory:
			return details::copy_string_literal(u8"directory",it); 
		case file_type::symlink:
			return details::copy_string_literal(u8"symlink",it);
		case file_type::block:
			return details::copy_string_literal(u8"block",it);
		case file_type::character:
			return details::copy_string_literal(u8"character",it);
		case file_type::fifo:
			return details::copy_string_literal(u8"fifo",it);
		case file_type::socket:
			return details::copy_string_literal(u8"socket",it);
		case file_type::remote:
			return details::copy_string_literal(u8"remote",it);
		default:
			return details::copy_string_literal(u8"unknown",it);
		}
	}
}
}

template<std::integral char_type>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,file_type>,char_type* it,file_type f) noexcept
{
	return details::print_file_status_impl<char_type>(it,f);
}

enum class shut{in,out,io};

/*
https://www.unix.com/man-page/FreeBSD/2/clock_gettime/
https://linux.die.net/man/3/clock_gettime
*/

enum class posix_clock_id
{
realtime,
realtime_alarm,
realtime_coarse,
tai,
monotonic,
monotonic_coarse,
monotonic_raw,
monotonic_raw_approx,
boottime,
boottime_alarm,
uptime_raw,
uptime_raw_approx,
process_cputime_id,
thread_cputime_id
};

enum class open_mode:
	::std::uint_least64_t
{
none=0,
//	*indicates that the open mode has not been evaluated yet
app=static_cast<::std::uint_least64_t>(1)<<0,
//	*["a"]seek to the end of stream before each write.
archive=static_cast<::std::uint_least64_t>(1)<<1,
//	[Windows]FILE_ATTRIBUTE_ARCHIVE 0x20
case_insensitive=static_cast<::std::uint_least64_t>(1)<<2,
//	[Windows]NT OBJ_CASE_INSENSITIVE 0x00000040L
compressed=static_cast<::std::uint_least64_t>(1)<<3,
//	[Windows]FILE_ATTRIBUTE_COMPRESSED 0x800
creat=static_cast<::std::uint_least64_t>(1)<<4,
//	POSIX O_CREAT
direct=static_cast<::std::uint_least64_t>(1)<<5,
//	*POSIX O_DIRECT
directory=static_cast<::std::uint_least64_t>(1)<<6,
//	*POSIX O_DIRECTORY.
encrypted=static_cast<::std::uint_least64_t>(1)<<7,
//	[Windows]FILE_ATTRIBUTE_ENCRYPTED 0x4000
excl=static_cast<::std::uint_least64_t>(1)<<8,
noreplace=excl,
//	*["x"]POSIX O_EXCL
follow=static_cast<::std::uint_least64_t>(1)<<9,
//	*Inverse of POSIX O_NOFOLLOW.
hidden=static_cast<::std::uint_least64_t>(1)<<10,
//	[Windows]FILE_ATTRIBUTE_HIDDEN 0x2
in=static_cast<::std::uint_least64_t>(1)<<11,
//	*["r"]open for reading.
inherit=static_cast<::std::uint_least64_t>(1)<<12,
//	*Inverse of POSIX O_CLOEXEC.
no_atime=static_cast<::std::uint_least64_t>(1)<<13,
//	[Linux]O_NOATIME
no_block=static_cast<::std::uint_least64_t>(1)<<14,
//	*POSIX O_NONBLOCK
no_ctty=static_cast<::std::uint_least64_t>(1)<<15,
//	[Linux]O_NOCTTY
no_recall=static_cast<::std::uint_least64_t>(1)<<16,
//	[Windows]FILE_FLAG_OPEN_NO_RECALL 0x00100000
no_shared_read=static_cast<::std::uint_least64_t>(1)<<17,
//	[WINDOWS]!FILE_SHARE_READ 0x00000001
no_shared_write=static_cast<::std::uint_least64_t>(1)<<18,
//	[WINDOWS]!FILE_SHARE_WRITE 0x00000002
normal=static_cast<::std::uint_least64_t>(1)<<19,
//	[Windows]FILE_ATTRIBUTE_NORMAL 0x80
offline=static_cast<::std::uint_least64_t>(1)<<20,
//	[Windows]FILE_ATTRIBUTE_OFFLINE 0x1000
out=static_cast<::std::uint_least64_t>(1)<<21,
//	*["w"]open for writing.
path=static_cast<::std::uint_least64_t>(1)<<22,
//	[Linux]O_PATH
random_access=static_cast<::std::uint_least64_t>(1)<<23,
//	[Windows]FILE_FLAG_RANDOM_ACCESS 0x10000000
search=static_cast<::std::uint_least64_t>(1)<<24,
//	POSIX O_SEARCH
session_aware=static_cast<::std::uint_least64_t>(1)<<25,
//	[Windows]FILE_FLAG_SESSION_AWARE 0x00800000
shared_delete=static_cast<::std::uint_least64_t>(1)<<26,
//	[WINDOWS]FILE_SHARE_WRITE 0x00000004
sync=static_cast<::std::uint_least64_t>(1)<<27,
//	*POSIX O_SYNC
system=static_cast<::std::uint_least64_t>(1)<<28,
//	[Windows]FILE_ATTRIBUTE_SYSTEM 0x4
temporary=static_cast<::std::uint_least64_t>(1)<<29,
//	*[Linux]O_TMPFILE. [Windows]FILE_ATTRIBUTE_TEMPORARY 0x100 + FILE_FLAG_DELETE_ON_CLOSE 0x04000000
text=static_cast<::std::uint_least64_t>(1)<<30,
//	[inverse of "b"]open in text mode. native_file is always binary. The text mode is provided by the POSIX layer or C FILE layer. When native_file is win32_file or nt_file or zw_file or unsupported character type for C FILE* or C++ filebuf, throw invalid arguments eh.
trunc=static_cast<::std::uint_least64_t>(1)<<31,
//	*POSIX O_TRUNC
tty_init=static_cast<::std::uint_least64_t>(1)<<32,
//	POSIX O_TTY_INIT
};

constexpr open_mode operator&(open_mode x, open_mode y) noexcept
{
using utype = typename std::underlying_type<open_mode>::type;
return static_cast<open_mode>(static_cast<utype>(x) & static_cast<utype>(y));
}

constexpr open_mode operator|(open_mode x, open_mode y) noexcept
{
using utype = typename std::underlying_type<open_mode>::type;
return static_cast<open_mode>(static_cast<utype>(x) | static_cast<utype>(y));
}

constexpr open_mode operator^(open_mode x, open_mode y) noexcept
{
using utype = typename std::underlying_type<open_mode>::type;
return static_cast<open_mode>(static_cast<utype>(x) ^ static_cast<utype>(y));
}

constexpr open_mode operator~(open_mode x) noexcept
{
using utype = typename std::underlying_type<open_mode>::type;
return static_cast<open_mode>(~static_cast<utype>(x));
}

inline constexpr open_mode& operator&=(open_mode& x, open_mode y) noexcept{return x=x&y;}

inline constexpr open_mode& operator|=(open_mode& x, open_mode y) noexcept{return x=x|y;}

inline constexpr open_mode& operator^=(open_mode& x, open_mode y) noexcept{return x=x^y;}



inline constexpr open_mode c_supported(open_mode m) noexcept
{
using utype = typename std::underlying_type<open_mode>::type;
constexpr auto c_supported_values{static_cast<utype>(open_mode::text)|
	static_cast<utype>(open_mode::excl)|
	static_cast<utype>(open_mode::out)|
	static_cast<utype>(open_mode::app)|
	static_cast<utype>(open_mode::in)|
	static_cast<utype>(open_mode::trunc)};
return static_cast<open_mode>(static_cast<utype>(m)&c_supported_values);
}

inline constexpr char const* to_c_mode(open_mode m) noexcept
{
	using utype = typename std::underlying_type<open_mode>::type;
	switch(static_cast<utype>(c_supported(m)))
	{
//Action if file already exists;	Action if file does not exist;	c-style mode;	Explanation
//Read from start;	Failure to open;	"r";	Open a file for reading
	case static_cast<utype>(open_mode::in)|static_cast<utype>(open_mode::text):
		return "\x72";
//Destroy contents;	Create new;	"w";	Create a file for writing
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::text):
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::trunc)|static_cast<utype>(open_mode::text):
		return "\x77";
//Append to file;	Create new;	"a";	Append to a file
	case static_cast<utype>(open_mode::app)|static_cast<utype>(open_mode::text):
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::app)|static_cast<utype>(open_mode::text):
		return "\x61";
//Read from start;	Error;	"r+";		Open a file for read/write
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::in)|static_cast<utype>(open_mode::text):
		return "\x72\x2b";
//Destroy contents;	Create new;	"w+";	Create a file for read/write
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::in)|static_cast<utype>(open_mode::trunc)|static_cast<utype>(open_mode::text):
		return "\x77\x2b";
//Write to end;	Create new;	"a+";	Open a file for read/write
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::in)|static_cast<utype>(open_mode::app)|static_cast<utype>(open_mode::text):
	case static_cast<utype>(open_mode::in)|static_cast<utype>(open_mode::app)|static_cast<utype>(open_mode::text):
		return "\x61\x2b";
//Destroy contents;	Error;	"wx";	Create a file for writing
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::excl)|static_cast<utype>(open_mode::text):
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::trunc)|static_cast<utype>(open_mode::excl)|static_cast<utype>(open_mode::text):
		return "\x77\x78";
//Append to file;	Error;	"ax";	Append to a file
	case static_cast<utype>(open_mode::app)|static_cast<utype>(open_mode::excl)|static_cast<utype>(open_mode::text):
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::app)|static_cast<utype>(open_mode::excl)|static_cast<utype>(open_mode::text):
		return "\x61\x78";
//Destroy contents;	Error;	"w+x";	Create a file for read/write
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::in)|static_cast<utype>(open_mode::trunc)|static_cast<utype>(open_mode::excl)|static_cast<utype>(open_mode::text):
		return "\x77\x2b\x78";
//Write to end;	Error;	"a+x";	Open a file for read/write
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::in)|static_cast<utype>(open_mode::app)|static_cast<utype>(open_mode::excl)|static_cast<utype>(open_mode::text):
	case static_cast<utype>(open_mode::in)|static_cast<utype>(open_mode::app)|static_cast<utype>(open_mode::excl)|static_cast<utype>(open_mode::text):
		return "\x61\x2b\x78";
	break;
	
//binary support

//Action if file already exists;	Action if file does not exist;	c-style mode;	Explanation
//Read from start;	Failure to open;	"rb";	Open a file for reading
	case static_cast<utype>(open_mode::in):
		return "\x72\x62";
//Destroy contents;	Create new;	"wb";	Create a file for writing
	case static_cast<utype>(open_mode::out):
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::trunc):
		return "\x77\x62";
//Append to file;	Create new;	"ab";	Append to a file
	case static_cast<utype>(open_mode::app):
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::app):
		return "\x61\x62";
//Read from start;	Error;	"r+b";		Open a file for read/write
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::in):
		return "\x72\x2b\x62";
//Destroy contents;	Create new;	"w+b";	Create a file for read/write
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::in)|static_cast<utype>(open_mode::trunc):
		return "\x77\x2b\x62";
//Write to end;	Create new;	"a+b";	Open a file for read/write
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::in)|static_cast<utype>(open_mode::app):
	case static_cast<utype>(open_mode::in)|static_cast<utype>(open_mode::app):
		return "\x61\x2b\x62";
//Destroy contents;	Error;	"wxb";	Create a file for writing
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::excl):
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::trunc)|static_cast<utype>(open_mode::excl):
		return "\x77\x78\x62";
//Append to file;	Error;	"axb";	Append to a file
	case static_cast<utype>(open_mode::app)|static_cast<utype>(open_mode::excl):
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::app)|static_cast<utype>(open_mode::excl):
		return "\x61\x78\x62";
//Destroy contents;	Error;	"w+xb";	Create a file for read/write
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::in)|static_cast<utype>(open_mode::trunc)|static_cast<utype>(open_mode::excl):
		return "\x77\x2b\x78\x62";
//Write to end;	Error;	"a+xb";	Open a file for read/write
	case static_cast<utype>(open_mode::out)|static_cast<utype>(open_mode::in)|static_cast<utype>(open_mode::app)|static_cast<utype>(open_mode::excl):
	case static_cast<utype>(open_mode::in)|static_cast<utype>(open_mode::app)|static_cast<utype>(open_mode::excl):
		return "\x61\x2b\x78\x62";
	break;
	case 0:
		if((m&open_mode::directory)!=open_mode::none)
			return "\x72";
		[[fallthrough]];
	default:
		return "";
	}
}
enum class posix_wait_reason:unsigned char
{
none,if_exited,if_signaled,if_core_dump,if_core_signal,if_stopped
};

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,posix_wait_reason>) noexcept
{
	return 20;
}

template<std::integral char_type>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,posix_wait_reason>,char_type* iter,posix_wait_reason reason) noexcept
{
	if constexpr(std::same_as<char_type,char>)
	{
		switch(reason)
		{
		case posix_wait_reason::none:
			return details::copy_string_literal("none",iter);
		case posix_wait_reason::if_exited:
			return details::copy_string_literal("if_exited",iter);
		case posix_wait_reason::if_signaled:
			return details::copy_string_literal("if_signaled",iter);
		case posix_wait_reason::if_core_dump:
			return details::copy_string_literal("if_core_dump",iter);
		case posix_wait_reason::if_core_signal:
			return details::copy_string_literal("if_core_signal",iter);
		case posix_wait_reason::if_stopped:
			return details::copy_string_literal("if_stopped",iter);
		default:
			return details::copy_string_literal("unknown",iter);
		}
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		switch(reason)
		{
		case posix_wait_reason::none:
			return details::copy_string_literal(L"none",iter);
		case posix_wait_reason::if_exited:
			return details::copy_string_literal(L"if_exited",iter);
		case posix_wait_reason::if_signaled:
			return details::copy_string_literal(L"if_signaled",iter);
		case posix_wait_reason::if_core_dump:
			return details::copy_string_literal(L"if_core_dump",iter);
		case posix_wait_reason::if_core_signal:
			return details::copy_string_literal(L"if_core_signal",iter);
		case posix_wait_reason::if_stopped:
			return details::copy_string_literal(L"if_stopped",iter);
		default:
			return details::copy_string_literal(L"unknown",iter);
		}
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		switch(reason)
		{
		case posix_wait_reason::none:
			return details::copy_string_literal(u"none",iter);
		case posix_wait_reason::if_exited:
			return details::copy_string_literal(u"if_exited",iter);
		case posix_wait_reason::if_signaled:
			return details::copy_string_literal(u"if_signaled",iter);
		case posix_wait_reason::if_core_dump:
			return details::copy_string_literal(u"if_core_dump",iter);
		case posix_wait_reason::if_core_signal:
			return details::copy_string_literal(u"if_core_signal",iter);
		case posix_wait_reason::if_stopped:
			return details::copy_string_literal(u"if_stopped",iter);
		default:
			return details::copy_string_literal(u"unknown",iter);
		}
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		switch(reason)
		{
		case posix_wait_reason::none:
			return details::copy_string_literal(U"none",iter);
		case posix_wait_reason::if_exited:
			return details::copy_string_literal(U"if_exited",iter);
		case posix_wait_reason::if_signaled:
			return details::copy_string_literal(U"if_signaled",iter);
		case posix_wait_reason::if_core_dump:
			return details::copy_string_literal(U"if_core_dump",iter);
		case posix_wait_reason::if_core_signal:
			return details::copy_string_literal(U"if_core_signal",iter);
		case posix_wait_reason::if_stopped:
			return details::copy_string_literal(U"if_stopped",iter);
		default:
			return details::copy_string_literal(U"unknown",iter);
		}
	}
	else
	{
		switch(reason)
		{
		case posix_wait_reason::none:
			return details::copy_string_literal(u8"none",iter);
		case posix_wait_reason::if_exited:
			return details::copy_string_literal(u8"if_exited",iter);
		case posix_wait_reason::if_signaled:
			return details::copy_string_literal(u8"if_signaled",iter);
		case posix_wait_reason::if_core_dump:
			return details::copy_string_literal(u8"if_core_dump",iter);
		case posix_wait_reason::if_core_signal:
			return details::copy_string_literal(u8"if_core_signal",iter);
		case posix_wait_reason::if_stopped:
			return details::copy_string_literal(u8"if_stopped",iter);
		default:
			return details::copy_string_literal(u8"unknown",iter);
		}
	}
}



struct native_interface_t
{
explicit constexpr native_interface_t() noexcept=default;
};
inline constexpr native_interface_t native_interface{};

struct io_async_t
{
explicit constexpr io_async_t() noexcept=default;
};
inline constexpr io_async_t io_async{};

struct io_temp_t
{
explicit constexpr io_temp_t() noexcept=default;
};
inline constexpr io_temp_t io_temp{};

struct io_dup_t
{
explicit constexpr io_dup_t() noexcept=default;
};
inline constexpr io_dup_t io_dup{};

struct io_kernel_t
{
explicit constexpr io_kernel_t() noexcept=default;
};
inline constexpr io_kernel_t io_kernel{};

struct posix_at_entry
{
	int fd{-1};
	explicit constexpr posix_at_entry() noexcept=default;
	explicit constexpr posix_at_entry(int mfd) noexcept:fd(mfd){}
};

struct file_loader_extra_bytes
{
	::std::size_t n{};
	explicit constexpr file_loader_extra_bytes(::std::size_t nn) noexcept:n(nn) {}
};

}

