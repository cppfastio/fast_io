#pragma once

#include "../../../fast_io_dsal/impl/misc/push_macros.h"

namespace fast_io
{

namespace details
{

inline constexpr ::fast_io::wine_unix::flags_t calculate_wine_open_mode(open_mode value) noexcept
{
	::fast_io::wine_unix::flags_t mode{::fast_io::wine_unix::o_nofollow};
	if ((value & open_mode::follow) != open_mode::none)
	{
		mode = {};
	}
	if ((value & open_mode::inherit) == open_mode::none)
	{
		mode |= ::fast_io::wine_unix::o_cloexec;
	}
	if ((value & open_mode::creat) != open_mode::none)
	{
		mode |= ::fast_io::wine_unix::o_creat;
	}
	if ((value & open_mode::excl) != open_mode::none)
	{
		mode |= ::fast_io::wine_unix::o_excl;
	}
	if ((value & open_mode::trunc) != open_mode::none)
	{
		mode |= ::fast_io::wine_unix::o_trunc;
	}
	if ((value & open_mode::direct) != open_mode::none)
	{
		mode |= ::fast_io::wine_unix::o_direct;
	}
	if ((value & open_mode::sync) != open_mode::none)
	{
		mode |= ::fast_io::wine_unix::o_sync;
	}
	if ((value & open_mode::dsync) != open_mode::none)
	{
		mode |= ::fast_io::wine_unix::o_dsync;
	}
	if ((value & open_mode::no_ctty) != open_mode::none)
	{
		mode |= ::fast_io::wine_unix::o_noctty;
	}
	if ((value & open_mode::path) != open_mode::none)
	{
		mode |= ::fast_io::wine_unix::o_path;
	}
	if ((value & open_mode::no_atime) != open_mode::none)
	{
		mode |= ::fast_io::wine_unix::o_noatime;
	}
	if ((value & open_mode::no_block) != open_mode::none)
	{
		mode |= ::fast_io::wine_unix::o_nonblock;
	}
	if ((value & open_mode::temporary) != open_mode::none)
	{
		mode |= ::fast_io::wine_unix::o_tmpfile;
	}
	if ((value & open_mode::directory) != open_mode::none)
	{
		mode |= ::fast_io::wine_unix::o_directory;
	}
	mode |= ::fast_io::wine_unix::o_largefile;

	using utype = typename ::std::underlying_type<open_mode>::type;
	constexpr auto supported_values{static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::app) |
									static_cast<utype>(open_mode::in)};
	switch (static_cast<utype>(value) & static_cast<utype>(supported_values))
	{
		// Action if file already exists;	Action if file does not exist;	c-style mode;	Explanation
		// Read from start;	Failure to open;	"r";	Open a file for reading
	case static_cast<utype>(open_mode::in):
		return mode | ::fast_io::wine_unix::o_rdonly;
		// Destroy contents;	Create new;	"w";	Create a file for writing
	case static_cast<utype>(open_mode::out):
		return mode | ::fast_io::wine_unix::o_wronly | ::fast_io::wine_unix::o_creat | ::fast_io::wine_unix::o_trunc;
		// Append to file;	Create new;	"a";	Append to a file
	case static_cast<utype>(open_mode::app):
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::app):
		return mode | ::fast_io::wine_unix::o_wronly | ::fast_io::wine_unix::o_creat | ::fast_io::wine_unix::o_append;
		// Read from start;	Error;	"r+";		Open a file for read/write
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::in):
		return mode | ::fast_io::wine_unix::o_rdwr;
		// Write to end;	Create new;	"a+";	Open a file for read/write
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::in) |
		static_cast<utype>(open_mode::app):
	case static_cast<utype>(open_mode::in) | static_cast<utype>(open_mode::app):
		return mode | ::fast_io::wine_unix::o_rdwr | ::fast_io::wine_unix::o_creat | ::fast_io::wine_unix::o_append;
		// Destroy contents;	Error;	"wx";	Create a file for writing
	default:
		return mode;
	}
}

struct my_wine_at_open_parameter
{
	::fast_io::wine_host_fd_t host_dirfd{};
	::fast_io::wine_unix::flags_t flags{};
	::fast_io::wine_unix::mode_t mode{};
	inline ::fast_io::wine_host_fd_t operator()(char const *filename, ::std::size_t filenamelen) const
		FAST_IO_HERBCEPTIONS_THROWS
	{
		return ::fast_io::wine::wine_unix_openat(host_dirfd, filename, filenamelen, flags, mode);
	}
};

struct my_wine_open_parameter
{
	::fast_io::wine_unix::flags_t flags{};
	::fast_io::wine_unix::mode_t mode{};
	inline ::fast_io::wine_host_fd_t operator()(char const *filename, ::std::size_t filenamelen) const
		FAST_IO_HERBCEPTIONS_THROWS
	{
		return ::fast_io::wine::wine_unix_open(filename, filenamelen, flags, mode);
	}
};

/*
wineunix filenames are utf8 bytes like posix (the unixcall impl hands them to
openat, the nt impl converts to utf16), never utf16 like the win32 api — even
though the wine layer is compiled for a windows target.
*/
template <::fast_io::constructible_to_os_c_str T>
inline ::fast_io::wine_host_fd_t wine_openat_file_impl(::fast_io::wine_host_fd_t host_dirfd, T const &t, open_mode om, perms pm)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::posix_api_common(
		t, my_wine_at_open_parameter{host_dirfd, ::fast_io::details::calculate_wine_open_mode(om),
									 static_cast<::fast_io::wine_unix::mode_t>(pm)});
}

/* cwd-relative open: goes straight to wine_unix_open — no at_fdcwd fetch */
template <::fast_io::constructible_to_os_c_str T>
inline ::fast_io::wine_host_fd_t wine_open_file_impl(T const &t, open_mode om, perms pm) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::posix_api_common(
		t, my_wine_open_parameter{::fast_io::details::calculate_wine_open_mode(om),
								  static_cast<::fast_io::wine_unix::mode_t>(pm)});
}

} // namespace details

} // namespace fast_io

#include "../../../fast_io_dsal/impl/misc/pop_macros.h"
