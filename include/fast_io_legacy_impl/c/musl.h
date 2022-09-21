#pragma once


namespace fast_io
{

namespace details::fp_hack
{


#ifdef __wasi__

struct fp_model {
	unsigned flags;
	unsigned char *rpos, *rend;
	int (*close)(FILE *);
	unsigned char *wend, *wpos;
#ifdef __wasilibc_unmodified_upstream // WASI doesn't need backwards-compatibility fields.
	unsigned char *mustbezero_1;
#endif
	unsigned char *wbase;
	size_t (*read)(FILE *, unsigned char *, size_t);
	size_t (*write)(FILE *, const unsigned char *, size_t);
	off_t (*seek)(FILE *, off_t, int);
	unsigned char *buf;
	size_t buf_size;
	FILE *prev, *next;
	int fd;
#ifdef __wasilibc_unmodified_upstream // WASI has no popen
	int pipe_pid;
#endif
#if defined(__wasilibc_unmodified_upstream) || defined(_REENTRANT)
	long lockcount;
#endif
	int mode;
#if defined(__wasilibc_unmodified_upstream) || defined(_REENTRANT)
	volatile int lock;
#endif
	int lbf;
	void *cookie;
	off_t off;
	char *getln_buf;
#ifdef __wasilibc_unmodified_upstream // WASI doesn't need backwards-compatibility fields.
	void *mustbezero_2;
#endif
	unsigned char *shend;
	off_t shlim, shcnt;
#if defined(__wasilibc_unmodified_upstream) || defined(_REENTRANT)
	FILE *prev_locked, *next_locked;
#endif
	void *locale;
};

#else
//https://github.com/EOSIO/musl/blob/master/src/internal/stdio_impl.h
struct fp_model
{
	unsigned flags;
	unsigned char *rpos, *rend;
	int (*close)(FILE *);
	unsigned char *wend, *wpos;
	unsigned char *mustbezero_1;
	unsigned char *wbase;
	size_t (*read)(FILE *, unsigned char *, size_t);
	size_t (*write)(FILE *, const unsigned char *, size_t);
	off_t (*seek)(FILE *, off_t, int);
	unsigned char *buf;
	size_t buf_size;
	FILE *prev, *next;
	int fd;
	int pipe_pid;
	long lockcount;
	short dummy3;
	signed char mode;
	signed char lbf;
	volatile int lock;
	volatile int waiters;
	void *cookie;
	off_t off;
	char *getln_buf;
	void *mustbezero_2;
	unsigned char *shend;
	off_t shlim, shcnt;
	FILE *prev_locked, *next_locked;
	void *locale;
};
#endif

template<std::size_t position>
requires (position<6)
constexpr std::size_t get_offset() noexcept
{
	if constexpr(position==0)
		return __builtin_offsetof(fp_model,buf);
	else if constexpr(position==1)
		return __builtin_offsetof(fp_model,rpos);
	else if constexpr(position==2)
		return __builtin_offsetof(fp_model,rend);
	else if constexpr(position==3)
		return __builtin_offsetof(fp_model,wbase);
	else if constexpr(position==4)
		return __builtin_offsetof(fp_model,wpos);
	else if constexpr(position==5)
		return __builtin_offsetof(fp_model,wend);
}

template<std::integral char_type,std::size_t position>
inline char_type* hack_fp_ptr(FILE* fp) noexcept
{
	constexpr std::size_t offset{get_offset<position>()};
	char_type* value;
	__builtin_memcpy(__builtin_addressof(value),reinterpret_cast<std::byte*>(fp)+offset,sizeof(char_type*));
	return value;	
}

template<std::size_t position,std::integral char_type>
inline void hack_fp_set_ptr(FILE* fp,char_type* ptr) noexcept
{
	constexpr std::size_t offset(get_offset<position>());
	__builtin_memcpy(reinterpret_cast<std::byte*>(fp)+offset,__builtin_addressof(ptr),sizeof(char_type*));
}

}

inline char* ibuffer_begin(c_io_observer_unlocked cio) noexcept
{
	return details::fp_hack::hack_fp_ptr<char,0>(cio.fp);
}

inline char* ibuffer_curr(c_io_observer_unlocked cio) noexcept
{
	return details::fp_hack::hack_fp_ptr<char,1>(cio.fp);
}

inline char* ibuffer_end(c_io_observer_unlocked cio) noexcept
{
	return details::fp_hack::hack_fp_ptr<char,2>(cio.fp);
}

inline void ibuffer_set_curr(c_io_observer_unlocked cio,char* ptr) noexcept
{
	details::fp_hack::hack_fp_set_ptr<1>(cio.fp,ptr);
}

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* ibuffer_begin(u8c_io_observer_unlocked cio) noexcept
{
	return details::fp_hack::hack_fp_ptr<char8_t,0>(cio.fp);
}

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* ibuffer_curr(u8c_io_observer_unlocked cio) noexcept
{
	return details::fp_hack::hack_fp_ptr<char8_t,1>(cio.fp);
}

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* ibuffer_end(u8c_io_observer_unlocked cio) noexcept
{
	return details::fp_hack::hack_fp_ptr<char8_t,2>(cio.fp);
}

inline void ibuffer_set_curr(u8c_io_observer_unlocked cio,
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
char8_t* ptr) noexcept
{
	details::fp_hack::hack_fp_set_ptr<1>(cio.fp,ptr);
}




namespace details::fp_hack
{
extern int libc_uflow (FILE *) noexcept __asm__("__uflow");

inline bool musl_fp_underflow_impl(FILE* fp)
{
	if(fp==stdin)
	{
		::fast_io::noexcept_call(fflush,stdout);
	}
	bool eof{libc_uflow(fp)!=EOF};
	if(!eof&&ferror_unlocked(fp))
		throw_posix_error();
	hack_fp_set_ptr<1>(fp,hack_fp_ptr<char,0>(fp));
	return eof;
}

}

inline bool ibuffer_underflow(c_io_observer_unlocked cio)
{
	return details::fp_hack::musl_fp_underflow_impl(cio.fp);
}

inline bool ibuffer_underflow(u8c_io_observer_unlocked cio)
{
	return details::fp_hack::musl_fp_underflow_impl(cio.fp);
}

inline char* obuffer_begin(c_io_observer_unlocked cio) noexcept
{
	return details::fp_hack::hack_fp_ptr<char,3>(cio.fp);
}

inline char* obuffer_curr(c_io_observer_unlocked cio) noexcept
{
	return details::fp_hack::hack_fp_ptr<char,4>(cio.fp);
}

inline char* obuffer_end(c_io_observer_unlocked cio) noexcept
{
	return details::fp_hack::hack_fp_ptr<char,5>(cio.fp);
}

inline void obuffer_set_curr(c_io_observer_unlocked cio,char* ptr) noexcept
{
	details::fp_hack::hack_fp_set_ptr<4>(cio.fp,ptr);
}

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* obuffer_begin(u8c_io_observer_unlocked cio) noexcept
{
	return details::fp_hack::hack_fp_ptr<char8_t,3>(cio.fp);
}

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* obuffer_curr(u8c_io_observer_unlocked cio) noexcept
{
	return details::fp_hack::hack_fp_ptr<char8_t,4>(cio.fp);
}

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* obuffer_end(u8c_io_observer_unlocked cio) noexcept
{
	return details::fp_hack::hack_fp_ptr<char8_t,5>(cio.fp);
}

inline void obuffer_set_curr(u8c_io_observer_unlocked cio,
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
char8_t* ptr) noexcept
{
	details::fp_hack::hack_fp_set_ptr<4>(cio.fp,ptr);
}

namespace details
{
extern int libc_overflow(_IO_FILE *, int) noexcept __asm__("__overflow");
}

inline void obuffer_overflow(c_io_observer_unlocked cio,char ch)
{
	if(details::libc_overflow(cio.fp,static_cast<int>(static_cast<unsigned char>(ch)))==EOF)[[unlikely]]
		throw_posix_error();
}

inline void obuffer_overflow(u8c_io_observer_unlocked cio,char8_t ch)
{
	if(details::libc_overflow(cio.fp,static_cast<int>(static_cast<unsigned char>(ch)))==EOF)[[unlikely]]
		throw_posix_error();
}

}
