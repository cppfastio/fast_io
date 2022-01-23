#pragma once

namespace fast_io::win32
{

/*
Referenced from ReactOS
https://doxygen.reactos.org/d2/d1b/sdk_2lib_2crt_2stdio_2file_8c_source.html
Referenced from MinGW-w64 CRT
https://github.com/Alexpux/mingw-w64/blob/d0d7f784833bbb0b2d279310ddc6afb52fe47a46/mingw-w64-crt/stdio/mingw_lock.c#L36
*/

[[gnu::dllimport]] extern void __cdecl _lock(int) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("_lock")
#else
asm("__lock")
#endif
#else
asm("_lock")
#endif
#endif
;

[[gnu::dllimport]] extern void __cdecl _unlock(int) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
asm("_unlock")
#else
asm("__unlock")
#endif
#else
asm("_unlock")
#endif
#endif
;


#if __has_cpp_attribute(gnu::const)
[[gnu::const]]
#endif
#if __has_cpp_attribute(gnu::always_inline)
[[gnu::always_inline]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline FILE* wincrt_iob_func() noexcept
{
#if defined(__iob_func)
	return __iob_func();
#else
#if (!defined(__MINGW32__) || __has_include(<_mingw_stat64.h>))
	return noexcept_call(__iob_func);
#else
	return _iob;
#endif
#endif
}

inline constexpr std::size_t msvcrt_iob_entries
{
#if defined(_IOB_ENTRIES)
_IOB_ENTRIES
#else
20
#endif
};

inline void my_msvcrt_lock_file(FILE* fp) noexcept
{
	auto iob{wincrt_iob_func()};
	std::size_t entry{static_cast<std::size_t>(fp-iob)};
	if(entry<msvcrt_iob_entries)
	{
		/*
		FILE lies in _iob[] so the lock lies in _locktable[]
https://github.com/Alexpux/mingw-w64/blob/d0d7f784833bbb0b2d279310ddc6afb52fe47a46/mingw-w64-crt/stdio/mingw_lock.c#L17
		#define _STREAM_LOCKS   16
		#define _IOLOCKED       0x8000
		*/
		_lock(static_cast<int>(16+entry));
		fp->_flag |= 0x8000;		//_IOLOCKED We set _IOLOCKED to indicate we locked the stream
	}
	else
		EnterCriticalSection(fp+1);
}

inline void my_msvcrt_unlock_file(FILE* fp) noexcept
{
	auto iob{wincrt_iob_func()};
	std::size_t entry{static_cast<std::size_t>(fp-iob)};
	if(entry<msvcrt_iob_entries)
	{
		fp->_flag &= ~0x8000;
		_unlock(static_cast<int>(16+entry));
	}
	else
		LeaveCriticalSection(fp+1);
}

/*
Referenced from ReactOS
https://doxygen.reactos.org/d2/d1b/sdk_2lib_2crt_2stdio_2file_8c_source.html
Line 3075
MSVCRT
https://github.com/changloong/msvcrt/blob/master/stdio/fwrite.c
*/

}