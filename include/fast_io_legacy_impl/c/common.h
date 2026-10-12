#pragma once

namespace fast_io
{

inline constexpr open_mode native_c_supported(open_mode m) noexcept
{
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
	using utype = typename ::std::underlying_type<open_mode>::type;
	constexpr auto c_supported_values{static_cast<utype>(open_mode::text) | static_cast<utype>(open_mode::out) |
									  static_cast<utype>(open_mode::app) | static_cast<utype>(open_mode::in) |
									  static_cast<utype>(open_mode::trunc)};
	return static_cast<open_mode>(static_cast<utype>(m) & c_supported_values);
#else
	return c_supported(m);
#endif
}
inline constexpr
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__) && !defined(_WIN32_WINDOWS)
	wchar_t const *
#else
	char const *
#endif
	to_native_c_mode(open_mode m) noexcept
{
/*
https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/fdopen-wfdopen?view=vs-2019
From microsoft's document. _fdopen only supports

"r"	Opens for reading. If the file does not exist or cannot be found, the fopen call fails.
"w"	Opens an empty file for writing. If the given file exists, its contents are destroyed.
"a"	Opens for writing at the end of the file (appending). Creates the file if it does not exist.
"r+"	Opens for both reading and writing. The file must exist.
"w+"	Opens an empty file for both reading and writing. If the file exists, its contents are destroyed.
"a+"	Opens for reading and appending. Creates the file if it does not exist.

"x" will throw EINVAL which does not satisfy POSIX, C11 and C++17 standard.
*/
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
	using utype = typename ::std::underlying_type<open_mode>::type;
#ifdef _WIN32_WINDOWS
	switch (static_cast<utype>(native_c_supported(m)))
	{
		// Action if file already exists;	Action if file does not exist;	c-style mode;	Explanation
		// Read from start;	Failure to open;	"r";	Open a file for reading
	case static_cast<utype>(open_mode::in) | static_cast<utype>(open_mode::text):
		return "\x72";
		// Destroy contents;	Create new;	"w";	Create a file for writing
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::text):
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::trunc) |
		static_cast<utype>(open_mode::text):
		return "\x77";
		// Append to file;	Create new;	"a";	Append to a file
	case static_cast<utype>(open_mode::app) | static_cast<utype>(open_mode::text):
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::app) |
		static_cast<utype>(open_mode::text):
		return "\x61";
		// Read from start;	Error;	"r+";		Open a file for read/write
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::in) |
		static_cast<utype>(open_mode::text):
		return "\x72\x2b";
		// Destroy contents;	Create new;	"w+";	Create a file for read/write
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::in) |
		static_cast<utype>(open_mode::trunc) | static_cast<utype>(open_mode::text):
		return "\x77\x2b";
		// Write to end;	Create new;	"a+";	Open a file for read/write
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::in) |
		static_cast<utype>(open_mode::app) | static_cast<utype>(open_mode::text):
	case static_cast<utype>(open_mode::in) | static_cast<utype>(open_mode::app) |
		static_cast<utype>(open_mode::text):
		return "\x61\x2b";

		// binary support

		// Action if file already exists;	Action if file does not exist;	c-style mode;	Explanation
		// Read from start;	Failure to open;	"rb";	Open a file for reading
	case static_cast<utype>(open_mode::in):
		return "\x72\x62";
		// Destroy contents;	Create new;	"wb";	Create a file for writing
	case static_cast<utype>(open_mode::out):
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::trunc):
		return "\x77\x62";
		// Append to file;	Create new;	"ab";	Append to a file
	case static_cast<utype>(open_mode::app):
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::app):
		return "\x61\x62";
		// Read from start;	Error;	"r+b";		Open a file for read/write
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::in):
		return "\x72\x2b\x62";
		// Destroy contents;	Create new;	"w+b";	Create a file for read/write
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::in) |
		static_cast<utype>(open_mode::trunc):
		return "\x77\x2b\x62";
		// Write to end;	Create new;	"a+b";	Open a file for read/write
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::in) |
		static_cast<utype>(open_mode::app):
	case static_cast<utype>(open_mode::in) | static_cast<utype>(open_mode::app):
		return "\x61\x2b\x62";
	case 0:
		if ((m & open_mode::directory) != open_mode::none)
		{
			return "\x72";
		}
		[[fallthrough]];
	default:
		return "";
	}
#else
	switch (static_cast<utype>(native_c_supported(m)))
	{
		// Action if file already exists;	Action if file does not exist;	c-style mode;	Explanation
		// Read from start;	Failure to open;	"r";	Open a file for reading
	case static_cast<utype>(open_mode::in) | static_cast<utype>(open_mode::text):
		return L"\x72";
		// Destroy contents;	Create new;	"w";	Create a file for writing
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::text):
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::trunc) |
		static_cast<utype>(open_mode::text):
		return L"\x77";
		// Append to file;	Create new;	"a";	Append to a file
	case static_cast<utype>(open_mode::app) | static_cast<utype>(open_mode::text):
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::app) |
		static_cast<utype>(open_mode::text):
		return L"\x61";
		// Read from start;	Error;	"r+";		Open a file for read/write
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::in) |
		static_cast<utype>(open_mode::text):
		return L"\x72\x2b";
		// Destroy contents;	Create new;	"w+";	Create a file for read/write
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::in) |
		static_cast<utype>(open_mode::trunc) | static_cast<utype>(open_mode::text):
		return L"\x77\x2b";
		// Write to end;	Create new;	"a+";	Open a file for read/write
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::in) |
		static_cast<utype>(open_mode::app) | static_cast<utype>(open_mode::text):
	case static_cast<utype>(open_mode::in) | static_cast<utype>(open_mode::app) |
		static_cast<utype>(open_mode::text):
		return L"\x61\x2b";

		// binary support

		// Action if file already exists;	Action if file does not exist;	c-style mode;	Explanation
		// Read from start;	Failure to open;	"rb";	Open a file for reading
	case static_cast<utype>(open_mode::in):
		return L"\x72\x62";
		// Destroy contents;	Create new;	"wb";	Create a file for writing
	case static_cast<utype>(open_mode::out):
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::trunc):
		return L"\x77\x62";
		// Append to file;	Create new;	"ab";	Append to a file
	case static_cast<utype>(open_mode::app):
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::app):
		return L"\x61\x62";
		// Read from start;	Error;	"r+b";		Open a file for read/write
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::in):
		return L"\x72\x2b\x62";
		// Destroy contents;	Create new;	"w+b";	Create a file for read/write
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::in) |
		static_cast<utype>(open_mode::trunc):
		return L"\x77\x2b\x62";
		// Write to end;	Create new;	"a+b";	Open a file for read/write
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::in) |
		static_cast<utype>(open_mode::app):
	case static_cast<utype>(open_mode::in) | static_cast<utype>(open_mode::app):
		return L"\x61\x2b\x62";
	case 0:
		if ((m & open_mode::directory) != open_mode::none)
		{
			return L"\x72";
		}
		[[fallthrough]];
	default:
		return L"";
	}
#endif
#else
	return to_c_mode(m);
#endif
}

namespace details
{

#if defined(__MSDOS__)
extern int fileno(FILE *) noexcept __asm__("_fileno");
extern FILE *fdopen(int, char const *) noexcept __asm__("_fdopen");
#elif defined(__CYGWIN__)
FAST_IO_DLLIMPORT extern int FAST_IO_WINCDECL fileno(FILE *) noexcept FAST_IO_WINCDECL_RENAME(fileno, 4);
FAST_IO_DLLIMPORT extern FILE *FAST_IO_WINCDECL fdopen(int, char const *) noexcept FAST_IO_WINCDECL_RENAME(fdopen, 8);
FAST_IO_DLLIMPORT extern void FAST_IO_WINCDECL my_cygwin_pthread_mutex_lock(void *) noexcept FAST_IO_WINCDECL_RENAME(pthread_mutex_lock, 4);
FAST_IO_DLLIMPORT extern void FAST_IO_WINCDECL my_cygwin_pthread_mutex_unlock(void *) noexcept FAST_IO_WINCDECL_RENAME(pthread_mutex_unlock, 4);

inline void my_cygwin_flockfile(FILE *fp) noexcept
{
	if (!((fp->_flags) & __SSTR))
	{
		my_cygwin_pthread_mutex_lock(fp->_lock);
	}
}
inline void my_cygwin_funlockfile(FILE *fp) noexcept
{
	if (!((fp->_flags) & __SSTR))
	{
		my_cygwin_pthread_mutex_unlock(fp->_lock);
	}
}
#endif

#if (defined(_MSC_VER) || defined(_UCRT)) && !defined(__WINE__) && !defined(__CYGWIN__)
inline void ucrt_lock_file(FILE *fp) noexcept;
inline void ucrt_unlock_file(FILE *fp) noexcept;
#endif

} // namespace details

enum class c_family : ::std::uint_fast8_t
{
	standard,
	unlocked,
	emulated,
	emulated_unlocked,
	native =
#if defined(__AVR__) || defined(_PICOLIBC__)
		emulated_unlocked
#elif defined(__MSDOS__)
		unlocked
#else
		standard
#endif
		,
	native_unlocked =
#if defined(__AVR__) || defined(_PICOLIBC__)
		emulated_unlocked
#else
		unlocked
#endif
};

enum class c_io_device_environment : ::std::uint_fast8_t
{
	file,
	custom,
	native =
#if defined(__AVR__)
		custom
#else
		file
#endif
};

struct c_io_device_open_t
{
	inline explicit constexpr c_io_device_open_t() noexcept = default;
};
inline constexpr c_io_device_open_t c_io_device_open{};

namespace details
{

template <c_family family>
inline int my_fileno_impl(FILE *fp) noexcept
{
	if (fp == nullptr)
	{
		return -1;
	}
	if constexpr (family == c_family::standard)
	{
		return
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
			noexcept_call(_fileno, fp)
#elif defined(__NEWLIB__)
			fp->_file
#else
			noexcept_call(fileno, fp)
#endif
				;
	}
	else
	{
		return
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
			noexcept_call(_fileno, fp)
#elif defined(__NEWLIB__) || defined(__DARWIN_C_LEVEL)
			fp->_file
#elif (defined(__MISC_VISIBLE) || defined(__USE_MISC)) && !defined(_PICOLIBC__)
			noexcept_call(fileno_unlocked, fp)
#else
			noexcept_call(fileno, fp)
#endif
				;
	}
}

inline int fp_to_fd(FILE *fp) noexcept
{
	return my_fileno_impl<c_family::standard>(fp);
}

inline int fp_unlocked_to_fd(FILE *fp) noexcept
{
	return my_fileno_impl<c_family::unlocked>(fp);
}

#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
template <c_family family>
inline void *my_fp_to_win32_handle_impl(FILE *fp) noexcept
{
	return my_get_osfile_handle(my_fileno_impl<family>(fp));
}

#if defined(FAST_IO_HAS_WINE_UNIX)
template <c_family family>
inline ::fast_io::wine_host_fd_t my_fp_to_wine_host_fd_impl(FILE *fp) FAST_IO_HERBCEPTIONS_THROWS
{
	return my_fd_to_wine_host_fd_impl(my_fileno_impl<family>(fp));
}
#endif
#endif

template <c_family family>
inline int my_fclose_impl(FILE *fp) noexcept
{
	if constexpr (family == c_family::standard || family == c_family::emulated)
	{
#if FAST_IO_HAS_BUILTIN(__builtin_fclose)
		return __builtin_fclose(fp);
#else
		return fclose(fp);
#endif
	}
	else
	{
#if defined(_MSC_VER) || defined(_UCRT)
		return noexcept_call(_fclose_nolock, fp);
#else
#if FAST_IO_HAS_BUILTIN(__builtin_fclose)
		return __builtin_fclose(fp);
#else
		return fclose(fp);
#endif
#endif
	}
}

#if !defined(__NEWLIB__) || defined(__CYGWIN__)

#if defined(__GLIBC__)
extern size_t glibc_fbufsize(FILE *stream) noexcept __asm__("__fbufsize");
#endif

inline FILE *my_fdopen(int fd, char const *mode) noexcept
{
	auto fp{
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
		noexcept_call(_fdopen, fd, mode)
#elif defined(__MSDOS__) || defined(__CYGWIN__)
		fdopen(fd, mode)
#else
		noexcept_call(fdopen, fd, mode)
#endif
	};
	/*
	WSL since microsoft sets the fstat block size incorrectly, leading to massive IO slow down for glibc.
	i woud like to add bandaids to fix it. Not working. setvbuf is a noop

	For fstream for GNU libstdc++. We should no longer construct FILE* by ourself if user is not using c_file to
	construct filebuf_file.
	*/
	return fp;
}

#endif

inline FILE *my_c_file_open_impl(int fd, open_mode mode) FAST_IO_HERBCEPTIONS_THROWS
{
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__) && !defined(_WIN32_WINDOWS)

	/*
	Reference implementation from ReactOS shows that _fdopen will call MultiByteToWideChar(CP_ACP,
	MB_PRECOMPOSED,str,len,wstr,len); which is not thread-safe and we might get screwed by locale on NT kernel. Avoid it
	and call _wfdopen instead. https://doxygen.reactos.org/d2/d1b/sdk_2lib_2crt_2stdio_2file_8c_source.html
	*/

	wchar_t const *cmode{to_native_c_mode(mode)};
#if defined(__MINGW32__) && !__has_include(<_mingw_stat64.h>)
	auto fp{noexcept_call(_wfdopen, fd, const_cast<wchar_t *>(cmode))};
#else
	auto fp{noexcept_call(_wfdopen, fd, cmode)};
#endif
	if (fp == nullptr)
	{
		throw_posix_error();
	}
#else
	char const *cmode{to_native_c_mode(mode)};
#if defined(__NEWLIB__) && !defined(__CYGWIN__)
	struct _reent ent{};
	auto fp{noexcept_call(_fdopen_r, __builtin_addressof(ent), fd, cmode)};
	if (fp == nullptr)
	{
		throw_posix_error(ent._errno);
	}
#else
	auto fp{my_fdopen(fd, cmode)};
	if (fp == nullptr)
	{
		throw_posix_error();
	}
#endif
#endif
	return fp;
}
#if defined(__NEWLIB__)
inline void my_c_io_newlib_flush_impl(FILE *fp)
	FAST_IO_HERBCEPTIONS_THROWS
{
	struct _reent ent{};
	if (noexcept_call(_fflush_r, __builtin_addressof(ent), fp))
	{
		throw_posix_error(ent._errno);
	}
}
#endif

template <c_family family>
inline void my_c_io_flush_impl(FILE *fp)
	FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(__NEWLIB__) && !defined(__CYGWIN__)
	my_c_io_newlib_flush_impl(fp);
#else
	if constexpr (family == c_family::standard)
	{
#if FAST_IO_HAS_BUILTIN(__builtin_fflush)
		if (__builtin_fflush(fp))
#else
		if (fflush(fp))
#endif
			throw_posix_error();
	}
	else
	{
#if (defined(_MSC_VER) || defined(_UCRT)) && !defined(__BIONIC__)
		if (noexcept_call(_fflush_nolock, fp))
		{
			throw_posix_error();
		}
#elif defined(__MISC_VISIBLE) && !defined(__NEWLIB__) && !defined(_PICOLIBC__)
		if (noexcept_call(fflush_unlocked, fp))
		{
			throw_posix_error();
		}
#else
		return my_c_io_flush_impl<c_family::standard>(fp);
#endif
	}
#endif
}

inline void c_flush_unlocked_impl(FILE *fp)
	FAST_IO_HERBCEPTIONS_THROWS
{
	my_c_io_flush_impl<c_family::unlocked>(fp);
}

#if defined(__AVR__)

[[noreturn]] inline void avr_libc_nosup_impl()
	FAST_IO_HERBCEPTIONS_THROWS
{
	throw_posix_error(EINVAL);
}

template <c_family family>
inline ::fast_io::intfpos_t my_c_io_seek_impl(FILE *, ::fast_io::intfpos_t, seekdir)
	FAST_IO_HERBCEPTIONS_THROWS
{
	avr_libc_nosup_impl();
}

#else
template <c_family family>
inline ::fast_io::intfpos_t my_c_io_seek_impl(FILE *fp, ::fast_io::intfpos_t offset, seekdir s)
	FAST_IO_HERBCEPTIONS_THROWS
{

	/*
	We avoid standard C functions since they cannot deal with large file on 32 bits platforms

	Reference:

	https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/fseek-nolock-fseeki64-nolock?view=vs-2019

	https://www.gnu.org/software/libc/manual/html_node/File-Positioning.html

	*/
	if constexpr (family == c_family::unlocked || family == c_family::emulated_unlocked)
	{
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
#if defined(_MSC_VER) || defined(_UCRT) || __MSVCRT_VERSION__ >= 0x800
		if (noexcept_call(_fseeki64_nolock, fp, offset, static_cast<int>(s)))
		{
			throw_posix_error();
		}
		auto val{noexcept_call(_ftelli64_nolock, fp)};
		if (val < 0)
		{
			throw_posix_error();
		}
		return val;
#else
		if (noexcept_call(fseeko64, fp, offset, static_cast<int>(s)))
		{
			throw_posix_error();
		}
		auto val{noexcept_call(ftello64, fp)};
		if (val < 0)
		{
			throw_posix_error();
		}
		return static_cast<::fast_io::intfpos_t>(val);
#endif
#else
		return my_c_io_seek_impl<c_family::standard>(fp, offset, s);
#endif
	}
	else
	{

#if defined(__NEWLIB__) && !defined(__CYGWIN__)
		struct _reent ent{};
		if (noexcept_call(_fseeko_r, __builtin_addressof(ent), fp, static_cast<_off_t>(offset), static_cast<int>(s)))
		{
			throw_posix_error(ent._errno);
		}
		ent = {};
		auto val{noexcept_call(_ftell_r, __builtin_addressof(ent), fp)};
		if (val < 0)
		{
			throw_posix_error(ent._errno);
		}
		return val;
#elif defined(__MSDOS__) || defined(__CYGWIN__) || defined(_PICOLIBC__) || \
	(defined(__MINGW32__) && !__has_include(<_mingw_stat64.h>))
		if constexpr (sizeof(long) < sizeof(::fast_io::intfpos_t))
		{
			if (offset < static_cast<::fast_io::intfpos_t>(::std::numeric_limits<long>::min()) ||
				offset > static_cast<::fast_io::intfpos_t>(::std::numeric_limits<long>::max()))
			{
				throw_posix_error(EINVAL);
			}
		}
		if (noexcept_call(::fseek, fp, static_cast<long>(offset), static_cast<int>(s)))
		{
			throw_posix_error();
		}
		auto val{noexcept_call(::ftell, fp)};
		if (val < 0)
		{
			throw_posix_error();
		}
		return static_cast<::fast_io::intfpos_t>(static_cast<long unsigned>(val));
#else
		if (
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
			_fseeki64(fp, offset, static_cast<int>(s))
#elif defined(__USE_LARGEFILE64)
			noexcept_call(fseeko64, fp, offset, static_cast<int>(s))
#elif FAST_IO_HAS_BUILTIN(__builtin_fseeko)
			__builtin_fseeko(fp, offset, static_cast<int>(s))
#else
			fseeko(fp, offset, static_cast<int>(s))
#endif
		)
			throw_posix_error();
		auto val{
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
			noexcept_call(_ftelli64, fp)
#elif defined(__USE_LARGEFILE64)
			noexcept_call(ftello64, fp)
#elif FAST_IO_HAS_BUILTIN(__builtin_ftello)
			__builtin_ftello(fp)
#else
			ftello(fp)
#endif
		};
		if (val < 0)
		{
			throw_posix_error();
		}
		return static_cast<::fast_io::intfpos_t>(val);
#endif
	}
}
#endif

inline FILE *my_c_open_tmp_file()
	FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(__AVR__) || defined(_PICOLIBC__)
	throw_posix_error(EINVAL);
#else
	::fast_io::posix_file pf(io_temp);
	auto fp{::fast_io::details::my_c_file_open_impl(pf.fd, ::fast_io::open_mode::in | ::fast_io::open_mode::out)};
	pf.release();
	return fp;
#endif
}

} // namespace details
} // namespace fast_io
