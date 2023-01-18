#pragma once
/*
newlib and many other libcs (including MSVCRT and UCRT windows)'s FILE is very similar to BSD. Maybe they all come from BSD?

Anyway. All FILE implementations are trash tbh.
*/


namespace fast_io
{

namespace details
{

#if defined(__BIONIC__)
struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
sbuf {
	unsigned char *_base;
	int	_size;
};

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
sFILE {
	unsigned char *_p;	/* current position in (some) buffer */
	int	_r;		/* read space left for getc() */
	int	_w;		/* write space left for putc() */
	short	_flag;		/* flags, below; this FILE is free if 0 */
	short	_file;		/* fileno, if Unix descriptor, else -1 */
	struct	sbuf _bf;	/* the buffer (at least 1 byte, if !NULL) */
	int	_lbfsize;	/* 0 or -_bf._size, for inline putc */

	/* operations */
	void	*_cookie;	/* cookie passed to io functions */
	int	(*_close)(void *);
	int	(*_read)(void *, char *, int);
	fpos_t	(*_seek)(void *, fpos_t, int);
	int	(*_write)(void *, const char *, int);

	/* extension data, to avoid further ABI breakage */
	struct	sbuf _ext;
	/* data for long sequences of ungetc() */
	unsigned char *_up;	/* saved _p when _p is doing ungetc data */
	int	_ur;		/* saved _r when _r is counting ungetc data */

	/* tricks to meet minimum requirements even when malloc() fails */
	unsigned char _ubuf[3];	/* guarantee an ungetc() buffer */
	unsigned char _nbuf[1];	/* guarantee a getc() buffer */

	/* separate buffer for fgetln() when line crosses buffer boundary */
	struct	sbuf _lb;	/* buffer for fgetln() */

	/* Unix stdio files get aligned to block boundaries on fseek() */
	int	_blksize;	/* stat.st_blksize (may be != _bf._size) */
	fpos_t	_offset;	/* current lseek offset */
};

#endif

template<typename T,std::size_t num>
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline T* bsd_get_buffer_ptr_impl(FILE* __restrict fpp) noexcept
{
	static_assert(num<4);
#if defined(__MSDOS__)
	FILE* fp{fpp};
	if constexpr(num==0)
		return reinterpret_cast<T*>(fp->_base);
	else if constexpr(num==1)
		return reinterpret_cast<T*>(fp->_ptr);
	else
		return reinterpret_cast<T*>(fp->_ptr+fp->_cnt);
#else
#if defined(__BIONIC__)
	sFILE* fp{reinterpret_cast<sFILE*>(fpp)};
#else
	FILE* fp{fpp};
#endif
	if constexpr(num==0)
		return reinterpret_cast<T*>(fp->_bf._base);
	else if constexpr(num==1)
		return reinterpret_cast<T*>(fp->_p);
	else if constexpr(num==2)
		return reinterpret_cast<T*>(fp->_p+fp->_w);
	else
		return reinterpret_cast<T*>(fp->_p+fp->_r);
#endif
}


template<bool w,typename T>
inline void bsd_set_buffer_curr_ptr_impl(FILE* __restrict fpp,
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
T* ptr) noexcept
{
#if defined(__MSDOS__)
	FILE* fp{fpp};
	if constexpr(w)	//set dirty for output
		fp->_flag|=0x010000;
	fp->_cnt-=static_cast<int>(reinterpret_cast<char*>(ptr)-fp->_ptr);
	fp->_ptr=reinterpret_cast<char*>(ptr);
#else
#if defined(__BIONIC__)
	sFILE* fp{reinterpret_cast<sFILE*>(fpp)};
#else
	FILE* fp{fpp};
#endif
	if constexpr(w)
	{
		fp->_w-=static_cast<int>(reinterpret_cast<char unsigned*>(ptr)-fp->_p);
		fp->_p=reinterpret_cast<char unsigned*>(ptr);
	}
	else
	{
		fp->_r-=static_cast<int>(reinterpret_cast<char unsigned*>(ptr)-fp->_p);
		fp->_p=reinterpret_cast<char unsigned*>(ptr);
	}
#endif
}

#if defined(__BSD_VISIBLE) ||defined(__DARWIN_C_LEVEL)
extern int bsd_srget(FILE *) noexcept __asm__("__srget");
#elif defined(__MSDOS__)
extern int _filbuf(FILE *) noexcept __asm__("__filbuf");
extern int _flsbuf(int, FILE*) noexcept __asm__("__flsbuf");
#endif

inline bool bsd_underflow_impl(FILE* __restrict fp)
{
#if defined(__NEWLIB__)
//untested
#if defined(__CYGWIN__)
	bool eof{fgetc(fp)==EOF};
	if(eof&&((fp->_flags & __SERR)!=0))[[unlikely]]
		throw_posix_error();
	++fp->_r;
	--fp->_p;
	return !eof;
#else
	struct _reent rent{};
	bool eof{__sgetc_r(__builtin_addressof(rent),fp)!=EOF};
	if(!eof&&((fp->_flags & __SERR)!=0))[[unlikely]]
		throw_posix_error();
	++fp->_r;
	--fp->_p;
	return eof;
#endif
#elif defined(__BIONIC__)
	bool eof{getc_unlocked(fp)!=EOF};
	if(!eof&&ferror_unlocked(fp))[[unlikely]]
		throw_posix_error();
	sFILE* fpp{reinterpret_cast<sFILE*>(fp)};
	++fpp->_r;
	--fpp->_p;
	return eof;
#else
	bool eof{
#if defined(__DARWIN_C_LEVEL)
#if __DARWIN_C_LEVEL >= 199506L
	::fast_io::noexcept_call(::__srget,fp)
#else
	::fast_io::noexcept_call(::fgetc,fp)
#endif
#else
bsd_srget(fp)
#endif
!=EOF};
	if(!eof&&ferror_unlocked(fp))[[unlikely]]
		throw_posix_error();
	++fp->_r;
	--fp->_p;
	return eof;
#endif
}

inline void bsd_overflow(FILE* __restrict fp,char unsigned ch)
{
#if defined(__MSDOS__)
	fp->_flag|=0x010000;
	if(_flsbuf(static_cast<int>(static_cast<unsigned char>(ch)),fp)==EOF)[[unlikely]]
		throw_posix_error();
#elif defined(__NEWLIB__)
	struct _reent rent;
	if(__sputc_r(&rent,static_cast<int>(static_cast<char unsigned>(ch)),fp)==EOF)[[unlikely]]
		throw_posix_error(rent._errno);
#elif defined(__BIONIC__)
	if(putc_unlocked(static_cast<int>(static_cast<char unsigned>(ch)),fp)==EOF)[[unlikely]]
		throw_posix_error();
#else
	if(__sputc(static_cast<int>(static_cast<char unsigned>(ch)),fp)==EOF)[[unlikely]]
		throw_posix_error();
#endif
}

}

inline char* obuffer_begin(c_io_observer_unlocked cio) noexcept
{
	return details::bsd_get_buffer_ptr_impl<char,0>(cio.fp);
}

inline char* obuffer_curr(c_io_observer_unlocked cio) noexcept
{
	return details::bsd_get_buffer_ptr_impl<char,1>(cio.fp);
}

inline char* obuffer_end(c_io_observer_unlocked cio) noexcept
{
	return details::bsd_get_buffer_ptr_impl<char,2>(cio.fp);
}

inline void obuffer_set_curr(c_io_observer_unlocked cio,char* ptr) noexcept
{
	details::bsd_set_buffer_curr_ptr_impl<true>(cio.fp,ptr);
}

inline void obuffer_overflow(c_io_observer_unlocked cio,char ch)
{
	details::bsd_overflow(cio.fp,static_cast<char unsigned>(ch));
}

inline char* ibuffer_begin(c_io_observer_unlocked cio) noexcept
{
	return details::bsd_get_buffer_ptr_impl<char,0>(cio.fp);
}

inline char* ibuffer_curr(c_io_observer_unlocked cio) noexcept
{
	return details::bsd_get_buffer_ptr_impl<char,1>(cio.fp);
}

inline char* ibuffer_end(c_io_observer_unlocked cio) noexcept
{
	return details::bsd_get_buffer_ptr_impl<char,3>(cio.fp);
}

inline void ibuffer_set_curr(c_io_observer_unlocked cio,char* __restrict ptr) noexcept
{
	details::bsd_set_buffer_curr_ptr_impl<false>(cio.fp,ptr);
}

inline bool ibuffer_underflow(c_io_observer_unlocked cio)
{
	return details::bsd_underflow_impl(cio.fp);
}

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* obuffer_begin(u8c_io_observer_unlocked cio) noexcept
{
	return details::bsd_get_buffer_ptr_impl<char8_t,0>(cio.fp);
}

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* obuffer_curr(u8c_io_observer_unlocked cio) noexcept
{
	return details::bsd_get_buffer_ptr_impl<char8_t,1>(cio.fp);
}

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* obuffer_end(u8c_io_observer_unlocked cio) noexcept
{
	return details::bsd_get_buffer_ptr_impl<char8_t,2>(cio.fp);
}

inline void obuffer_set_curr(u8c_io_observer_unlocked cio,
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
char8_t* __restrict ptr) noexcept
{
	details::bsd_set_buffer_curr_ptr_impl<true>(cio.fp,ptr);
}

inline void obuffer_overflow(u8c_io_observer_unlocked cio,char8_t ch)
{
	details::bsd_overflow(cio.fp,static_cast<char unsigned>(ch));
}

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* ibuffer_begin(u8c_io_observer_unlocked cio) noexcept
{
	return details::bsd_get_buffer_ptr_impl<char8_t,0>(cio.fp);
}

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* ibuffer_curr(u8c_io_observer_unlocked cio) noexcept
{
	return details::bsd_get_buffer_ptr_impl<char8_t,1>(cio.fp);
}

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* ibuffer_end(u8c_io_observer_unlocked cio) noexcept
{
	return details::bsd_get_buffer_ptr_impl<char8_t,3>(cio.fp);
}

inline void ibuffer_set_curr(u8c_io_observer_unlocked cio,
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
char8_t* ptr) noexcept
{
	details::bsd_set_buffer_curr_ptr_impl<false>(cio.fp,ptr);
}

inline bool ibuffer_underflow(u8c_io_observer_unlocked cio)
{
	return details::bsd_underflow_impl(cio.fp);
}

}