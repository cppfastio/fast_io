#pragma once

/* Referenced from

https://github.com/wbx-github/uclibc-ng/blob/master/libc/sysdeps/linux/common/bits/uClibc_stdio.h

*/

#if 0
#if defined __UCLIBC_HAS_THREADS__ && !defined __UCLIBC_IO_MUTEX
/* keep this in sync with uClibc_mutex.h */
# ifdef __USE_STDIO_FUTEXES__
#  include <bits/stdio-lock.h>
#  define __UCLIBC_IO_MUTEX(M) _IO_lock_t M
# else
#  include <bits/pthreadtypes.h>
#  define __UCLIBC_IO_MUTEX(M) pthread_mutex_t M
# endif /* __UCLIBC_HAS_THREADS_NATIVE__ */
#endif

struct __STDIO_FILE_STRUCT {
	unsigned short __modeflags;
	/* There could be a hole here, but modeflags is used most.*/
#ifdef __UCLIBC_HAS_WCHAR__
	unsigned char __ungot_width[2]; /* 0: current (building) char; 1: scanf */
	/* Move the following futher down to avoid problems with getc/putc
	 * macros breaking shared apps when wchar config support is changed. */
	/* wchar_t ungot[2]; */
#else  /* __UCLIBC_HAS_WCHAR__ */
	unsigned char __ungot[2];
#endif /* __UCLIBC_HAS_WCHAR__ */
	int __filedes;
#ifdef __STDIO_BUFFERS
	unsigned char *__bufstart;	/* pointer to buffer */
	unsigned char *__bufend;	/* pointer to 1 past end of buffer */
	unsigned char *__bufpos;
	unsigned char *__bufread; /* pointer to 1 past last buffered read char */

#ifdef __STDIO_GETC_MACRO
	unsigned char *__bufgetc_u;	/* 1 past last readable by getc_unlocked */
#endif /* __STDIO_GETC_MACRO */
#ifdef __STDIO_PUTC_MACRO
	unsigned char *__bufputc_u;	/* 1 past last writeable by putc_unlocked */
#endif /* __STDIO_PUTC_MACRO */

#endif /* __STDIO_BUFFERS */

#ifdef __STDIO_HAS_OPENLIST
	struct __STDIO_FILE_STRUCT *__nextopen;
#endif
#ifdef __UCLIBC_HAS_WCHAR__
	wchar_t __ungot[2];
#endif
#ifdef __STDIO_MBSTATE
	__mbstate_t __state;
#endif
#ifdef __UCLIBC_HAS_XLOCALE__
	void *__unused;				/* Placeholder for codeset binding. */
#endif
#ifdef __UCLIBC_HAS_THREADS__
	int __user_locking;
	__UCLIBC_IO_MUTEX(__lock);
#endif
/* Everything after this is unimplemented... and may be trashed. */
#if __STDIO_BUILTIN_BUF_SIZE > 0
	unsigned char __builtinbuf[__STDIO_BUILTIN_BUF_SIZE];
#endif /* __STDIO_BUILTIN_BUF_SIZE > 0 */
};
#endif

namespace fast_io
{

namespace details
{
extern int uclibc_fgetc_unlocked(FILE*) noexcept __asm__("__fgetc_unlocked");

extern int uclibc_fputc_unlocked(int,FILE*) noexcept __asm__("__fputc_unlocked");


inline bool uclibc_underflow_impl(FILE* fp)
{
	bool eof{uclibc_fgetc_unlocked(fp)==EOF};
	if(eof&&((fp->__modeflags&__FLAG_ERROR)==__FLAG_ERROR))
		throw_posix_error();
	fp->__bufpos=fp->__bufstart;
	return !eof;
}

inline void uclibc_overflow_impl(FILE* fp,char unsigned ch)
{
	if(uclibc_fputc_unlocked(static_cast<int>(ch),fp)==EOF)
		throw_posix_error();
}

inline void uclibc_set_curr_ptr_impl(FILE* fp,char unsigned* ptr) noexcept
{
	fp->__modeflags|=__FLAG_WRITING;
	fp->__bufpos=reinterpret_cast<char unsigned*>(ptr);
}

}

inline char* ibuffer_begin(c_io_observer_unlocked ciob) noexcept
{
	return reinterpret_cast<char*>(ciob.fp->__bufstart);
}

inline char* ibuffer_curr(c_io_observer_unlocked ciob) noexcept
{
	return reinterpret_cast<char*>(ciob.fp->__bufpos);
}

inline char* ibuffer_end(c_io_observer_unlocked ciob) noexcept
{
	return reinterpret_cast<char*>(ciob.fp->__bufread);
}

inline void ibuffer_set_curr(c_io_observer_unlocked ciob,char* ptr) noexcept
{
	ciob.fp->__bufpos=reinterpret_cast<char unsigned*>(ptr);
}

inline bool ibuffer_underflow(c_io_observer_unlocked ciob)
{
	return details::uclibc_underflow_impl(ciob.fp);
}


inline char* obuffer_begin(c_io_observer_unlocked ciob) noexcept
{
	return reinterpret_cast<char*>(ciob.fp->__bufstart);
}

inline char* obuffer_curr(c_io_observer_unlocked ciob) noexcept
{
	return reinterpret_cast<char*>(ciob.fp->__bufpos);
}

inline char* obuffer_end(c_io_observer_unlocked ciob) noexcept
{
	return reinterpret_cast<char*>(ciob.fp->__bufend);
}

inline void obuffer_set_curr(c_io_observer_unlocked ciob,char* ptr) noexcept
{
	details::uclibc_set_curr_ptr_impl(ciob.fp,reinterpret_cast<char unsigned*>(ptr));
}

inline void obuffer_overflow(c_io_observer_unlocked ciob,char ch)
{
	details::uclibc_overflow_impl(ciob.fp,static_cast<char unsigned>(ch));
}

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* ibuffer_begin(u8c_io_observer_unlocked ciob) noexcept
{
	return reinterpret_cast<char8_t*>(ciob.fp->__bufstart);
}

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* ibuffer_curr(u8c_io_observer_unlocked ciob) noexcept
{
	return reinterpret_cast<char8_t*>(ciob.fp->__bufpos);
}

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* ibuffer_end(u8c_io_observer_unlocked ciob) noexcept
{
	return reinterpret_cast<char8_t*>(ciob.fp->__bufread);
}

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline void ibuffer_set_curr(u8c_io_observer_unlocked ciob,char8_t* ptr) noexcept
{
	ciob.fp->__bufpos=reinterpret_cast<char unsigned*>(ptr);
}

inline bool ibuffer_underflow(u8c_io_observer_unlocked ciob)
{
	return details::uclibc_underflow_impl(ciob.fp);
}

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* obuffer_begin(u8c_io_observer_unlocked ciob) noexcept
{
	return reinterpret_cast<char8_t*>(ciob.fp->__bufstart);
}

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* obuffer_curr(u8c_io_observer_unlocked ciob) noexcept
{
	return reinterpret_cast<char8_t*>(ciob.fp->__bufpos);
}

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* obuffer_end(u8c_io_observer_unlocked ciob) noexcept
{
	return reinterpret_cast<char8_t*>(ciob.fp->__bufend);
}

inline void obuffer_set_curr(u8c_io_observer_unlocked ciob,char8_t* ptr) noexcept
{
	details::uclibc_set_curr_ptr_impl(ciob.fp,reinterpret_cast<char unsigned*>(ptr));
}

inline void obuffer_overflow(u8c_io_observer_unlocked ciob,char8_t ch)
{
	details::uclibc_overflow_impl(ciob.fp,static_cast<char unsigned>(ch));
}

}