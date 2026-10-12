#pragma once

namespace fast_io
{

namespace details
{
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)

inline constexpr int calculate_posix_open_mode_for_win32_handle_impl(open_mode value, int mode) noexcept
{
	constexpr auto supported_values{open_mode::out | open_mode::app | open_mode::in};
	using utype = typename ::std::underlying_type<open_mode>::type;
	switch (static_cast<utype>(supported_values) & static_cast<utype>(value))
	{
		// Action if file already exists;	Action if file does not exist;	c-style mode;	Explanation
		// Read from start;	Failure to open;	"r";	Open a file for reading
	case static_cast<utype>(open_mode::in):
		return mode | O_RDONLY;
		// Destroy contents;	Create new;	"w";	Create a file for writing
	case static_cast<utype>(open_mode::out):
		// Read from start;	Error;	"r+";		Open a file for read/write
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::in):
		return mode;
		// Append to file;	Create new;	"a";	Append to a file
	case static_cast<utype>(open_mode::app):
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::app):
		return mode | O_APPEND;
		// Write to end;	Create new;	"a+";	Open a file for read/write
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::in) |
		static_cast<utype>(open_mode::app):
	case static_cast<utype>(open_mode::in) | static_cast<utype>(open_mode::app):
		return mode | O_APPEND;
		// Destroy contents;	Error;	"wx";	Create a file for writing
	default:
		return mode;
	}
}

enum class posix_open_mode_text_behavior
{
	always_binary = 0,
	text,
	wide_text,
	u8_text,
	u16_text
};

template <posix_open_mode_text_behavior behavior>
inline constexpr int calculate_posix_open_mode_for_win32_handle(open_mode value) noexcept
{
	int mode{};
	if constexpr (behavior == posix_open_mode_text_behavior::always_binary)
	{
		mode = _O_BINARY;
	}
	else
	{
		if ((value & open_mode::text) == open_mode::none)
		{
			mode = _O_BINARY;
		}
		else
		{
			if constexpr (behavior == posix_open_mode_text_behavior::wide_text)
			{
#if defined(_O_WTEXT)
				mode |= _O_WTEXT;
#else
				mode |= _O_BINARY;
#endif
			}
			else if constexpr (behavior == posix_open_mode_text_behavior::u8_text)
			{
#if defined(_O_U8TEXT)
				mode |= _O_U8TEXT;
#else
				mode |= _O_BINARY;
#endif
			}
			else if constexpr (behavior == posix_open_mode_text_behavior::u16_text)
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
	return calculate_posix_open_mode_for_win32_handle_impl(value, mode);
}
#endif

inline constexpr int calculate_posix_open_mode(open_mode value) noexcept
{
	int mode{
#ifdef O_NOFOLLOW
		O_NOFOLLOW
#endif
	};
	if ((value & open_mode::follow) != open_mode::none)
	{
		mode = {};
	}
	if ((value & open_mode::inherit) == open_mode::none)
#ifdef O_CLOEXEC
		mode |= O_CLOEXEC;
#elif defined(_O_NOINHERIT)
		mode |= _O_NOINHERIT;
#endif
#ifdef O_BINARY
	if ((value & open_mode::text) == open_mode::none)
	{
		mode |= O_BINARY;
	}
#endif
	if ((value & open_mode::creat) != open_mode::none)
	{
		mode |= O_CREAT;
	}
	if ((value & open_mode::excl) != open_mode::none)
	{
		mode |= O_EXCL;
	}
	if ((value & open_mode::trunc) != open_mode::none)
	{
		mode |= O_TRUNC;
	}
#ifdef O_DIRECT
	if ((value & open_mode::direct) != open_mode::none)
	{
		mode |= O_DIRECT;
	}
#endif
#ifdef O_SYNC
	if ((value & open_mode::sync) != open_mode::none)
	{
		mode |= O_SYNC;
	}
#endif
#ifdef O_DSYNC
	if ((value & open_mode::dsync) != open_mode::none)
	{
		mode |= O_DSYNC;
	}
#endif
#ifdef O_RSYNC
	if ((value & open_mode::rsync) != open_mode::none)
	{
		mode |= O_RSYNC;
	}
#endif
#ifdef O_TTY_INIT
	if ((value & open_mode::tty_init) != open_mode::none)
	{
		mode |= O_TTY_INIT;
	}
#endif
#ifdef O_NOCTTY
	if ((value & open_mode::no_ctty) != open_mode::none)
	{
		mode |= O_NOCTTY;
	}
#endif
#ifdef O_PATH
	if ((value & open_mode::path) != open_mode::none)
	{
		mode |= O_PATH;
	}
#endif
#ifdef O_NOATIME
	if ((value & open_mode::no_atime) != open_mode::none)
	{
		mode |= O_NOATIME;
	}
#endif
	if ((value & open_mode::no_block) != open_mode::none)
#ifdef O_NONBLOCK
		mode |= O_NONBLOCK;
#else
		return {};
#endif

	if ((value & open_mode::temporary) != open_mode::none)
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
	if ((value & open_mode::random_access) != open_mode::none)
	{
		mode |= _O_RANDOM;
	}
	else
	{
		mode |= _O_SEQUENTIAL;
	}
#endif
#ifdef O_LARGEFILE
	mode |= O_LARGEFILE;
#endif

	if ((value & open_mode::directory) != open_mode::none)
#ifdef O_DIRECTORY
		mode |= O_DIRECTORY;
#else
		return {};
#endif

	using utype = typename ::std::underlying_type<open_mode>::type;
	constexpr auto supported_values{static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::app) |
									static_cast<utype>(open_mode::in)};
	switch (static_cast<utype>(value) & static_cast<utype>(supported_values))
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
		// Action if file already exists;	Action if file does not exist;	c-style mode;	Explanation
		// Read from start;	Failure to open;	"r";	Open a file for reading
	case static_cast<utype>(open_mode::in):
		return mode | O_RDONLY;
		// Destroy contents;	Create new;	"w";	Create a file for writing
	case static_cast<utype>(open_mode::out):
		return mode | O_WRONLY | O_CREAT | O_TRUNC;
		// Append to file;	Create new;	"a";	Append to a file
	case static_cast<utype>(open_mode::app):
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::app):
		return mode | O_WRONLY | O_CREAT | O_APPEND;
		// Read from start;	Error;	"r+";		Open a file for read/write
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::in):
		return mode | O_RDWR;
		// Write to end;	Create new;	"a+";	Open a file for read/write
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::in) |
		static_cast<utype>(open_mode::app):
	case static_cast<utype>(open_mode::in) | static_cast<utype>(open_mode::app):
		return mode | O_RDWR | O_CREAT | O_APPEND;
		// Destroy contents;	Error;	"wx";	Create a file for writing
	default:
		return mode;
	}
}

} // namespace details

namespace details
{

#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)

template <posix_open_mode_text_behavior behavior>
inline int open_fd_from_handle_impl(void *handle, open_mode md) FAST_IO_HERBCEPTIONS_THROWS
{
	int fd{::fast_io::noexcept_call(_open_osfhandle, reinterpret_cast<::std::ptrdiff_t>(handle),
									details::calculate_posix_open_mode_for_win32_handle<behavior>(md))};
	if (fd == -1)
	{
		throw_posix_error();
	}
	return fd;
}

template <::std::integral ch_type>
inline int open_fd_from_handle(void *handle, open_mode md) FAST_IO_HERBCEPTIONS_THROWS
{
	if constexpr (is_ebcdic<ch_type>)
	{
		return open_fd_from_handle_impl<posix_open_mode_text_behavior::always_binary>(handle, md);
	}
	else if constexpr (::std::same_as<ch_type, char>)
	{
		return open_fd_from_handle_impl<posix_open_mode_text_behavior::text>(handle, md);
	}
	else if constexpr (::std::same_as<ch_type, char8_t>)
	{
		return open_fd_from_handle_impl<posix_open_mode_text_behavior::text>(handle, md);
	}
	else if constexpr (::std::same_as<ch_type, wchar_t>)
	{
		return open_fd_from_handle_impl<posix_open_mode_text_behavior::wide_text>(handle, md);
	}
	else if constexpr (::std::same_as<ch_type, char16_t>)
	{
		return open_fd_from_handle_impl<posix_open_mode_text_behavior::u16_text>(handle, md);
	}
	else
	{
		return open_fd_from_handle_impl<posix_open_mode_text_behavior::always_binary>(handle, md);
	}
}

#else
#if defined(__DARWIN_C_LEVEL) || defined(__MSDOS__)
extern int my_posix_open_noexcept(char const *pathname, int flags) noexcept __asm__("_open");
extern int my_posix_open_noexcept(char const *pathname, int flags, mode_t mode) noexcept __asm__("_open");
#else
extern int my_posix_open_noexcept(char const *pathname, int flags) noexcept __asm__("open");
extern int my_posix_open_noexcept(char const *pathname, int flags, mode_t mode) noexcept __asm__("open");
#endif

#if defined(__MSDOS__)

using dos_path_tlc_string = ::fast_io::containers::basic_string<char, ::fast_io::native_thread_local_allocator>;

template <typename... Args>
constexpr inline dos_path_tlc_string concat_dos_path_tlc_string(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char, dos_path_tlc_string>(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat dos_path_tlc_string");
		return {};
	}
}

struct my_dos_concat_tlc_path_common_result
{
	bool failed{};
	dos_path_tlc_string path{};
};

inline constexpr my_dos_concat_tlc_path_common_result my_dos_concat_tlc_path_common(int dirfd, char const *pathname) noexcept
{
	if (dirfd == -100)
	{
		return {false, dos_path_tlc_string{::fast_io::mnp::os_c_str(pathname)}};
	}
	else
	{
		auto fd_pathname_cstr{::fast_io::noexcept_call(::__get_fd_name, dirfd)};
		if (fd_pathname_cstr == nullptr) [[unlikely]]
		{
			return {true};
		}

		// check vaildity
		auto const sz{::fast_io::cstr_len(pathname)};

		if (sz > 255) [[unlikely]]
		{
			return {true};
		}

#if 0
		if (::fast_io::details::is_invalid_dos_pathname_with_size(pathname, sz)) [[unlikely]]
		{
			return {true};
		}
#endif

		// concat
		return {false, concat_dos_path_tlc_string(::fast_io::mnp::os_c_str(fd_pathname_cstr), ::fast_io::mnp::chvw('\\'), ::fast_io::mnp::os_c_str(pathname))};
	}
}

inline constexpr dos_path_tlc_string my_dos_concat_tlc_path(int dirfd, char const *pathname) FAST_IO_HERBCEPTIONS_THROWS
{
	auto [failed, path]{my_dos_concat_tlc_path_common(dirfd, pathname)};
	if (failed) [[unlikely]]
	{
		throw_posix_error(EINVAL);
	}
	return path;
}

inline int my_posix_openat(int dirfd, char const *pathname, int flags, mode_t mode) FAST_IO_HERBCEPTIONS_THROWS
{
	int fd{::fast_io::details::my_posix_open_noexcept(my_dos_concat_tlc_path(dirfd, pathname).c_str(), flags, mode)};
	if (fd == -1) [[unlikely]]
	{
		throw_posix_error();
	}
	return fd;
}

#elif (defined(__NEWLIB__) || defined(_PICOLIBC__)) && !defined(__CYGWIN__)

inline int my_posix_openat(int, char const *, int, mode_t) FAST_IO_HERBCEPTIONS_THROWS
{
	throw_posix_error(EINVAL);
}
#else

#if defined(__DARWIN_C_LEVEL) || defined(__MSDOS__)
extern int my_posix_openat_noexcept(int fd, char const *path, int aflag, ... /*mode_t mode*/) noexcept __asm__("_openat");
#else
extern int my_posix_openat_noexcept(int fd, char const *path, int aflag, ... /*mode_t mode*/) noexcept __asm__("openat");
#endif

inline int my_posix_openat(int dirfd, char const *pathname, int flags, mode_t mode) FAST_IO_HERBCEPTIONS_THROWS
{
	int fd{
#if defined(__linux__) && defined(__NR_openat)
		system_call<__NR_openat, int>
#else
		my_posix_openat_noexcept
#endif
		(dirfd, pathname, flags, mode)};

#if defined(__linux__) && defined(__NR_openat)
	system_call_throw_error(fd);
#else
	if (fd == -1) [[unlikely]]
	{
		throw_posix_error();
	}
#endif

	return fd;
}
#endif

#if defined(__CYGWIN__)

#if (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
extern int my_cygwin_attach_handle_to_fd(char const *name, int fd, void *handle, int bin, int access) noexcept
#if SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if defined(__GNUC__)
	__asm__("cygwin_attach_handle_to_fd")
#else
	__asm__("_cygwin_attach_handle_to_fd")
#endif
#else
	__asm__("cygwin_attach_handle_to_fd")
#endif
		;

inline constexpr unsigned calculate_win32_cygwin_open_mode(open_mode value) noexcept
{
	unsigned access{};
	if ((value & open_mode::out) == open_mode::out)
	{
		access |= 0x40000000 /*GENERIC_WRITE*/;
	}
	if ((value & open_mode::in) == open_mode::in)
	{
		access |= 0x80000000 /*GENERIC_READ*/;
	}
	return access;
}

inline int cygwin_create_fd_with_win32_handle(void *handle, open_mode mode)
	FAST_IO_HERBCEPTIONS_THROWS
{
	int fd{my_cygwin_attach_handle_to_fd(nullptr, -1, handle, true,
										 static_cast<int>(calculate_win32_cygwin_open_mode(mode)))};
	if (fd == -1)
	{
		throw_posix_error();
	}
	return fd;
}

#endif

#ifdef __MSDOS__
extern unsigned int my_dos_creat(char const *, short unsigned, int *) noexcept __asm__("__dos_creat");
extern unsigned int my_dos_creatnew(char const *, short unsigned, int *) noexcept __asm__("__dos_creatnew");
extern unsigned int my_dos_open(char const *, short unsigned, int *) noexcept __asm__("__dos_open");
extern unsigned int my_dos_setmode(int, int) noexcept __asm__("_setmode");
extern unsigned int my_dos_close(int) noexcept __asm__("__dos_close");
#endif

inline int my_posix_open(char const *pathname, int flags,
#if __has_cpp_attribute(maybe_unused)
						 [[maybe_unused]]
#endif
						 mode_t mode) FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(__MSDOS__) || (defined(__NEWLIB__) && !defined(AT_FDCWD)) || defined(_PICOLIBC__)
	int fd{::fast_io::details::my_posix_open_noexcept(pathname, flags, mode)};
	if (fd == -1) [[unlikely]]
	{
		throw_posix_error();
	}
	return fd;
#else
	return ::fast_io::details::my_posix_openat(AT_FDCWD, pathname, flags, mode);
#endif
}

inline int my_posix_openat_file_internal_impl(int dirfd, char const *filepath, open_mode om, perms pm) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::my_posix_openat(dirfd, filepath, ::fast_io::details::calculate_posix_open_mode(om), static_cast<mode_t>(pm));
}

struct my_posix_at_open_paramter
{
	int dirfd{-1};
	int om{};
	mode_t pm{};
	inline int operator()(char const *filename) const FAST_IO_HERBCEPTIONS_THROWS
	{
		return ::fast_io::details::my_posix_openat(dirfd, filename, om, pm);
	}
};

struct my_posix_open_paramter
{
	int om{};
	mode_t pm{};
	inline int operator()(char const *filename) const FAST_IO_HERBCEPTIONS_THROWS
	{
		return my_posix_open(filename, om, pm);
	}
};

#if ((defined(__NEWLIB__) && !defined(AT_FDCWD)) || defined(_PICOLIBC__)) && !defined(__CYGWIN__)

template <::fast_io::constructible_to_os_c_str T>
inline constexpr int posix_openat_file_impl(int, T const &, open_mode, perms)
	FAST_IO_HERBCEPTIONS_THROWS
{
	throw_posix_error(EINVAL);
	return -1;
}

#else
template <::fast_io::constructible_to_os_c_str T>
inline constexpr int posix_openat_file_impl(int dirfd, T const &t, open_mode om, perms pm)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::posix_api_common(
		t,
		my_posix_at_open_paramter{dirfd, ::fast_io::details::calculate_posix_open_mode(om), static_cast<mode_t>(pm)});
}
#endif

template <::fast_io::constructible_to_os_c_str T>
inline constexpr int posix_open_file_impl(T const &t, open_mode om, perms pm) FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(__MSDOS__) || (defined(__NEWLIB__) && !defined(AT_FDCWD)) || defined(_PICOLIBC__)
	return ::fast_io::posix_api_common(
		t, my_posix_open_paramter{::fast_io::details::calculate_posix_open_mode(om), static_cast<mode_t>(pm)});
#else
	return ::fast_io::posix_api_common(t, my_posix_at_open_paramter{AT_FDCWD,
																	::fast_io::details::calculate_posix_open_mode(om),
																	static_cast<mode_t>(pm)});
#endif
}

#endif

inline int my_open_posix_fd_temp_file()
	FAST_IO_HERBCEPTIONS_THROWS
{
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
	::fast_io::basic_win32_file<char> wf(::fast_io::io_temp);
	int fd{::fast_io::noexcept_call(_open_osfhandle, reinterpret_cast<::std::ptrdiff_t>(wf.handle), _O_BINARY)};
	if (fd == -1)
	{
		throw_posix_error();
	}
	wf.release();
	return fd;
#elif defined(__CYGWIN__)
	/*
	This implementation is not correct since cygwin runtime changes the tmp environment and it causes issues.
	*/
	::fast_io::basic_win32_file<char> wf(::fast_io::io_temp);
	constexpr int flag{static_cast<int>(0x80000000 | 0x40000000)};
	int fd{my_cygwin_attach_handle_to_fd(nullptr, -1, wf.handle, true, flag)};
	if (fd == -1)
	{
		throw_posix_error();
	}
	wf.release();
	return fd;
#elif defined(O_TMPFILE) && defined(__linux__) && defined(__NR_openat)
	int fd{system_call<__NR_openat, int>(AT_FDCWD, u8"/tmp", O_EXCL | O_RDWR | O_TMPFILE | O_APPEND | O_NOATIME,
										 S_IRUSR | S_IWUSR)};
	system_call_throw_error(fd);
	return fd;
#else
	throw_posix_error(EINVAL);
#endif
}

} // namespace details
} // namespace fast_io
