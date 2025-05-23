#pragma once
#if __has_include(<cstdio>)
#include <cstdio>
#elif __has_include(<stdio.h>)
#include <stdio.h>
#endif

#if defined(__MINGW32__) && !defined(_UCRT) && !defined(__BIONIC__)
#include "msvcrt_lock.h"
#endif

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
		return "\x77\x2b";

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
		return L"\x77\x2b";

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

inline FILE *my_c_file_open_impl(int fd, open_mode mode)
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
{
	my_c_io_flush_impl<c_family::unlocked>(fp);
}

#if defined(__AVR__)

[[noreturn]] inline void avr_libc_nosup_impl()
{
	throw_posix_error(EINVAL);
}

template <c_family family>
inline ::fast_io::intfpos_t my_c_io_seek_impl(FILE *, ::fast_io::intfpos_t, seekdir)
{
	avr_libc_nosup_impl();
}

#else
template <c_family family>
inline ::fast_io::intfpos_t my_c_io_seek_impl(FILE *fp, ::fast_io::intfpos_t offset, seekdir s)
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

template <c_family family, ::std::integral ch_type>
	requires(family == c_family::native || family == c_family::native_unlocked)
class basic_c_family_io_observer
{
public:
	using char_type = ch_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
	using native_handle_type = FILE *;
	native_handle_type fp{};
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return fp;
	}
	inline explicit constexpr operator bool() const noexcept
	{
		return fp;
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{fp};
		fp = nullptr;
		return temp;
	}
#if !defined(__AVR__)
	template <posix_family fam>
	inline explicit operator basic_posix_family_io_observer<fam, char_type>() const noexcept
	{
		return basic_posix_family_io_observer<fam, char_type>{details::my_fileno_impl<family>(fp)};
	}
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) || defined(__CYGWIN__)
	template <win32_family fam>
	inline explicit operator basic_win32_family_io_observer<fam, char_type>() const noexcept
	{
		return {details::my_fp_to_win32_handle_impl<family>(fp)};
	}
	template <nt_family fam>
	inline explicit operator basic_nt_family_io_observer<fam, char_type>() const noexcept
	{
		return {details::my_fp_to_win32_handle_impl<family>(fp)};
	}
#endif
#endif
	inline void lock() const noexcept
		requires(family == c_family::standard || family == c_family::emulated)
	{
#if (defined(_MSC_VER) || defined(_UCRT)) && !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__)
		if constexpr (false)
		{
			::fast_io::details::ucrt_lock_file(fp);
		}
		else
		{
			noexcept_call(_lock_file, fp);
		}
#elif (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__) && !defined(__WINE__)
		win32::my_msvcrt_lock_file(fp);
#elif !defined(__SINGLE_THREAD__)
#if defined(__NEWLIB__)
#if defined(__CYGWIN__)
		details::my_cygwin_flockfile(fp);
#elif !defined(__SINGLE_THREAD__)
//	_flockfile(fp);	//TO FIX undefined reference to `__cygwin_lock_lock' why?
#endif
#elif defined(__MSDOS__) || (defined(__wasi__) && !defined(__wasilibc_unmodified_upstream) && !defined(_REENTRANT)) || \
	defined(__AVR__) || defined(_PICOLIBC__)
#else
		noexcept_call(flockfile, fp);
#endif
#endif
	}
	inline void unlock() const noexcept
		requires(family == c_family::standard || family == c_family::emulated)
	{
#if (defined(_MSC_VER) || defined(_UCRT)) && !defined(__CYGWIN__) && !defined(__WINE__) && !defined(__BIONIC__)
		if constexpr (false)
		{
			::fast_io::details::ucrt_unlock_file(fp);
		}
		else
		{
			noexcept_call(_unlock_file, fp);
		}
#elif (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
		win32::my_msvcrt_unlock_file(fp);
#elif !defined(__SINGLE_THREAD__)
#if defined(__NEWLIB__)
#if defined(__CYGWIN__)
		details::my_cygwin_funlockfile(fp);
#elif !defined(__SINGLE_THREAD__)
//	_funlockfile(fp); //TO FIX
#endif
#elif defined(__MSDOS__) || (defined(__wasi__) && !defined(__wasilibc_unmodified_upstream) && !defined(_REENTRANT)) || \
	defined(__AVR__) || defined(_PICOLIBC__)
#else
		noexcept_call(funlockfile, fp);
#endif
#endif
	}
	inline constexpr basic_c_family_io_observer<c_family::native_unlocked, ch_type> unlocked_handle() const noexcept
		requires(family == c_family::standard)
	{
		return {fp};
	}
	inline constexpr basic_c_family_io_observer<c_family::native_unlocked, ch_type> unlocked_handle() const noexcept
		requires(family == c_family::emulated)
	{
		return {fp};
	}
};

template <c_family family, ::std::integral ch_type>
inline constexpr basic_c_family_io_observer<family, ch_type>
io_stream_ref_define(basic_c_family_io_observer<family, ch_type> other) noexcept
{
	return other;
}

template <c_family family, ::std::integral ch_type>
inline constexpr basic_c_family_io_observer<family, char>
io_bytes_stream_ref_define(basic_c_family_io_observer<family, ch_type> other) noexcept
{
	return {other.fp};
}

template <c_family family, ::std::integral ch_type>
	requires(family == c_family::standard || family == c_family::emulated)
inline constexpr basic_c_family_io_observer<family, ch_type>
io_stream_mutex_ref_define(basic_c_family_io_observer<family, ch_type> other) noexcept
{
	return other;
}

template <c_family family, ::std::integral ch_type>
	requires(family == c_family::standard || family == c_family::emulated)
inline constexpr basic_c_family_io_observer<c_family::native_unlocked, ch_type>
io_stream_unlocked_ref_define(basic_c_family_io_observer<family, ch_type> other) noexcept
{
	return {other.fp};
}

#if defined(__AVR__)
template <c_family family, ::std::integral ch_type>
inline constexpr posix_file_status status(basic_c_family_io_observer<family, ch_type> ciob)
{
	details::avr_libc_nosup_impl();
}

#else

template <c_family family, ::std::integral ch_type>
inline constexpr posix_at_entry at(basic_c_family_io_observer<family, ch_type> other) noexcept
{
	return posix_at_entry{details::my_fileno_impl<family>(other.fp)};
}

template <c_family family, ::std::integral ch_type>
	requires requires(basic_posix_io_observer<ch_type> piob) { status(piob); }
inline constexpr posix_file_status status(basic_c_family_io_observer<family, ch_type> ciob)
{
	return status(static_cast<basic_posix_io_observer<ch_type>>(ciob));
}

template <c_family family, ::std::integral ch_type>
inline ::std::size_t file_size(::fast_io::basic_c_family_io_observer<family, ch_type> ciob)
{
	return ::fast_io::details::posix_loader_get_file_size(details::my_fileno_impl<family>(ciob.fp));
}
#endif

template <c_family family, ::std::integral ch_type>
inline void io_stream_buffer_flush_define(basic_c_family_io_observer<family, ch_type> cfhd)
{
	details::my_c_io_flush_impl<family>(cfhd.fp);
}

template <c_family family, ::std::integral ch_type>
inline ::fast_io::intfpos_t io_stream_seek_bytes_define(basic_c_family_io_observer<family, ch_type> cfhd,
														::fast_io::intfpos_t offset, seekdir s)
{
	return details::my_c_io_seek_impl<family>(cfhd.fp, offset, s);
}

template <c_family family, ::std::integral ch_type>
inline constexpr bool operator==(basic_c_family_io_observer<family, ch_type> a,
								 basic_c_family_io_observer<family, ch_type> b) noexcept
{
	return a.fp == b.fp;
}

#if __cpp_impl_three_way_comparison >= 201907L
template <c_family family, ::std::integral ch_type>
inline constexpr auto operator<=>(basic_c_family_io_observer<family, ch_type> a,
								  basic_c_family_io_observer<family, ch_type> b) noexcept
{
	return a.fp <=> b.fp;
}
#endif

#if !defined(__AVR__)
template <c_family family, ::std::integral ch_type>
	requires requires(basic_c_family_io_observer<family, ch_type> h) {
		redirect_handle(static_cast<basic_posix_io_observer<ch_type>>(h));
	}
inline decltype(auto) redirect_handle(basic_c_family_io_observer<family, ch_type> h)
{
	return redirect_handle(static_cast<basic_posix_io_observer<ch_type>>(h));
}
#if 0
template<c_family family,::std::integral ch_type>
requires zero_copy_input_stream<basic_posix_io_observer<ch_type>>
inline decltype(auto) zero_copy_in_handle(basic_c_family_io_observer<family,ch_type> h)
{
	return zero_copy_in_handle(static_cast<basic_posix_io_observer<ch_type>>(h));
}

template<c_family family,::std::integral ch_type>
requires zero_copy_output_stream<basic_posix_io_observer<ch_type>>
inline decltype(auto) zero_copy_out_handle(basic_c_family_io_observer<family,ch_type> h)
{
	return zero_copy_out_handle(static_cast<basic_posix_io_observer<ch_type>>(h));
}
#endif
#endif

template <c_family family>
struct c_family_file_factory FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
{
	using native_handle_type = FILE *;
	FILE *fp{};
	inline explicit constexpr c_family_file_factory(FILE *fpp) noexcept
		: fp(fpp) {};
	inline c_family_file_factory(c_family_file_factory const &) = delete;
	inline c_family_file_factory &operator=(c_family_file_factory const &) = delete;
	inline ~c_family_file_factory()
	{
		if (fp) [[likely]]
		{
#if defined(__NEWLIB__) && !defined(__CYGWIN__)
			struct _reent ent{};
			noexcept_call(_fclose_r, __builtin_addressof(ent), this->fp);
#else
			details::my_fclose_impl<family>(this->fp);
#endif
		}
	}
};

template <c_family family, ::std::integral ch_type>
class basic_c_family_file FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE : public basic_c_family_io_observer<family, ch_type>
{
public:
	using char_type = ch_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
	using native_handle_type = FILE *;
	inline constexpr basic_c_family_file() noexcept = default;
	template <typename native_hd>
		requires ::std::same_as<native_handle_type, ::std::remove_cvref_t<native_hd>>
	inline explicit constexpr basic_c_family_file(native_hd ffp) noexcept
		: basic_c_family_io_observer<family, ch_type>{ffp}
	{
	}
	template <c_family family2>
	inline explicit constexpr basic_c_family_file(c_family_file_factory<family2> &&other) noexcept
		: basic_c_family_io_observer<family, ch_type>{other.fp}
	{
		other.fp = nullptr;
	}
	inline constexpr basic_c_family_file(basic_c_family_io_observer<family, ch_type>) noexcept = delete;
	inline constexpr basic_c_family_file &operator=(basic_c_family_io_observer<family, ch_type>) noexcept = delete;

	inline basic_c_family_file(basic_c_family_file const &) = delete;
	inline basic_c_family_file &operator=(basic_c_family_file const &) = delete;
	inline constexpr basic_c_family_file(decltype(nullptr)) noexcept = delete;
	inline constexpr basic_c_family_file(basic_c_family_file &&other) noexcept
		: basic_c_family_io_observer<family, ch_type>{other.fp}
	{
		other.fp = nullptr;
	}
	inline basic_c_family_file &operator=(basic_c_family_file &&other) noexcept
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		if (this->fp) [[likely]]
		{
#if defined(__NEWLIB__) && !defined(__CYGWIN__)
			struct _reent ent{};
			noexcept_call(_fclose_r, __builtin_addressof(ent), this->fp);
#else
			details::my_fclose_impl<family>(this->fp);
#endif
		}
		this->fp = other.fp;
		other.fp = nullptr;
		return *this;
	}
	inline void close()
	{
		if (this->fp == nullptr) [[unlikely]]
		{
			return;
		}
#if defined(__NEWLIB__) && !defined(__CYGWIN__)
		struct _reent ent{};
		int ret{noexcept_call(_fclose_r, __builtin_addressof(ent), this->fp)};
		this->fp = nullptr;
		if (ret == EOF)
		{
			throw_posix_error(ent._errno);
		}
#else
		int ret{details::my_fclose_impl<family>(this->fp)};
		this->fp = nullptr;
		if (ret == EOF)
		{
			throw_posix_error(errno);
		}
#endif
	}
	inline ~basic_c_family_file()
	{
		if (this->fp) [[likely]]
		{
#if defined(__NEWLIB__) && !defined(__CYGWIN__)
			struct _reent ent{};
			noexcept_call(_fclose_r, __builtin_addressof(ent), this->fp);
#else
			details::my_fclose_impl<family>(this->fp);
#endif
		}
	}
#if !defined(__AVR__)
	template <::fast_io::constructible_to_os_c_str T>
	inline basic_c_family_file(T const &file, open_mode om, perms pm = static_cast<perms>(436))
		: basic_c_family_file(basic_posix_file<char_type>(file, om, pm), om)
	{
	}
	template <posix_family pfamily>
	inline basic_c_family_file(basic_posix_family_file<pfamily, char_type> &&phd, open_mode om)
		: basic_c_family_io_observer<family, ch_type>{::fast_io::details::my_c_file_open_impl(phd.fd, om)}
	{
		phd.fd = -1;
	}
	template <posix_family pfamily>
	inline explicit constexpr basic_c_family_file(io_construct_t, basic_posix_family_io_observer<pfamily, ch_type> piob, open_mode om) noexcept
		: basic_c_family_io_observer<family, char_type>{::fast_io::details::my_c_file_open_impl(piob.fd, om)}
	{
	}
#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) || defined(__CYGWIN__)
	// windows specific. open posix file from win32 io handle
	template <win32_family wfamily>
	inline basic_c_family_file(basic_win32_family_file<wfamily, char_type> &&win32_handle, open_mode om)
		: basic_c_family_file(basic_posix_file<char_type>(::std::move(win32_handle), om), om)
	{
	}
	template <nt_family nfamily>
	inline basic_c_family_file(basic_nt_family_file<nfamily, char_type> &&nt_handle, open_mode om)
		: basic_c_family_file(basic_posix_file<char_type>(::std::move(nt_handle), om), om)
	{
	}
	inline basic_c_family_file(nt_fs_dirent ent, open_mode om, perms pm = static_cast<perms>(436))
		: basic_c_family_file(basic_posix_file<char_type>(ent, om, pm), om)
	{
	}
	inline basic_c_family_file(win32_9xa_fs_dirent ent, open_mode om, perms pm = static_cast<perms>(436))
		: basic_c_family_file(basic_posix_file<char_type>(ent, om, pm), om)
	{
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline basic_c_family_file(nt_at_entry nate, T const &file, open_mode om, perms pm = static_cast<perms>(436))
		: basic_c_family_file(basic_posix_file<char_type>(nate, file, om, pm), om)
	{
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline basic_c_family_file(win32_9xa_at_entry nate, T const &file, open_mode om, perms pm = static_cast<perms>(436))
		: basic_c_family_file(basic_posix_file<char_type>(nate, file, om, pm), om)
	{
	}
#else
	inline basic_c_family_file(posix_fs_dirent ent, open_mode om, perms pm = static_cast<perms>(436))
		: basic_c_family_file(basic_posix_file<char_type>(ent, om, pm), om)
	{
	}
	template <::fast_io::constructible_to_os_c_str T>
	inline basic_c_family_file(posix_at_entry nate, T const &file, open_mode om, perms pm = static_cast<perms>(436))
		: basic_c_family_file(basic_posix_file<char_type>(nate, file, om, pm), om)
	{
	}
#endif
#endif
	inline basic_c_family_file(io_temp_t)
		: basic_c_family_io_observer<family, ch_type>{::fast_io::details::my_c_open_tmp_file()}
	{
	}
};

template <::std::integral char_type>
using basic_c_io_observer_unlocked = basic_c_family_io_observer<c_family::native_unlocked, char_type>;
template <::std::integral char_type>
using basic_c_io_observer = basic_c_family_io_observer<c_family::native, char_type>;

template <::std::integral char_type>
using basic_c_file_unlocked = basic_c_family_file<c_family::native_unlocked, char_type>;
template <::std::integral char_type>
using basic_c_file = basic_c_family_file<c_family::native, char_type>;

using c_io_observer_unlocked = basic_c_io_observer_unlocked<char>;
using c_io_observer = basic_c_io_observer<char>;
using c_file = basic_c_file<char>;
using c_file_unlocked = basic_c_file_unlocked<char>;
using wc_io_observer_unlocked = basic_c_io_observer_unlocked<wchar_t>;
using wc_io_observer = basic_c_io_observer<wchar_t>;
using wc_file = basic_c_file<wchar_t>;
using wc_file_unlocked = basic_c_file_unlocked<wchar_t>;
using u8c_io_observer_unlocked = basic_c_io_observer_unlocked<char8_t>;
using u8c_io_observer = basic_c_io_observer<char8_t>;
using u8c_file = basic_c_file<char8_t>;
using u8c_file_unlocked = basic_c_file_unlocked<char8_t>;
using u16c_io_observer_unlocked = basic_c_io_observer_unlocked<char16_t>;
using u16c_io_observer = basic_c_io_observer<char16_t>;
using u16c_file = basic_c_file<char16_t>;
using u16c_file_unlocked = basic_c_file_unlocked<char16_t>;
using u32c_io_observer_unlocked = basic_c_io_observer_unlocked<char32_t>;
using u32c_io_observer = basic_c_io_observer<char32_t>;
using u32c_file = basic_c_file<char32_t>;
using u32c_file_unlocked = basic_c_file_unlocked<char32_t>;

using c_file_factory = c_family_file_factory<c_family::native>;
using c_file_factory_unlocked = c_family_file_factory<c_family::native_unlocked>;

namespace freestanding
{
template <c_family fm>
struct is_trivially_copyable_or_relocatable<c_family_file_factory<fm>>
{
	inline static constexpr bool value = true;
};

template <c_family fm, ::std::integral char_type>
struct is_zero_default_constructible<basic_c_family_io_observer<fm, char_type>>
{
	inline static constexpr bool value = true;
};

template <c_family fm, ::std::integral char_type>
struct is_trivially_copyable_or_relocatable<basic_c_family_file<fm, char_type>>
{
	inline static constexpr bool value = true;
};

template <c_family fm, ::std::integral char_type>
struct is_zero_default_constructible<basic_c_family_file<fm, char_type>>
{
	inline static constexpr bool value = true;
};

} // namespace freestanding

} // namespace fast_io

#if (defined(_WIN32) && !defined(__WINE__) && !defined(__BIONIC__)) && !defined(__CYGWIN__)
#include "wincrt.h"
#elif defined(__AVR__) || defined(_PICOLIBC__)
#include "avrlibc.h"
#include "macros_general.h"
#else
#if defined(__LLVM_LIBC_TYPES_FILE_H__)
#include "llvm.h"
#elif defined(__UCLIBC__)
#if defined(__STDIO_BUFFERS)
#include "uclibc.h"
#elif defined(FAST_IO_LIBC_CUSTOM_BUFFER_PTRS)
#include "custom.h"
#endif
#elif defined(__MLIBC_O_CLOEXEC)
#include "mlibc.h"
#elif defined(__GLIBC__)
#include "glibc.h"
#elif defined(__wasi__)
#include "musl.h"
#elif defined(__NEED___isoc_va_list) || defined(__musl__)
#include "musl.h"
#elif defined(__serenity__)
#include "serenity.h"
#elif defined(__BSD_VISIBLE) || defined(__DARWIN_C_LEVEL) || (defined(__NEWLIB__) && !defined(__CUSTOM_FILE_IO__)) || \
	defined(__BIONIC__) || defined(__MSDOS__)
#include "unix.h"
#elif defined(FAST_IO_LIBC_CUSTOM_BUFFER_PTRS)
#include "custom.h"
#endif

#if !defined(__MSDOS__)
#include "general.h"
#endif
#include "done.h"
#include "macros_general.h"
#endif

#include "preadwrite.h"
