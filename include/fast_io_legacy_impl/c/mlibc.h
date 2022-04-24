#pragma once

/*
Referenced from:
https://github.com/managarm/mlibc/blob/master/options/ansi/include/stdio.h
*/

namespace fast_io
{
namespace details::mlibc_hack
{

inline constexpr char* hack_ibuffer_begin_impl(FILE* fp) noexcept
{
	return fp->__buffer_ptr;
}

inline constexpr char* hack_ibuffer_curr_impl(FILE* fp) noexcept
{
	return fp->__buffer_ptr+fp->__offset;
}

inline constexpr char* hack_ibuffer_end_impl(FILE* fp) noexcept
{
	return fp->__buffer_ptr+fp->__valid_limit;
}

inline constexpr void hack_ibuffer_set_curr_impl(FILE* fp,char* ptr) noexcept
{
	std::size_t new_size{static_cast<std::size_t>(ptr-fp->__buffer_ptr)};
	fp->__offset=fp->__io_offset=new_size;
}

inline constexpr char* hack_obuffer_begin_impl(FILE* fp) noexcept
{
	return fp->__buffer_ptr;
}

inline constexpr char* hack_obuffer_curr_impl(FILE* fp) noexcept
{
	return fp->__buffer_ptr+fp->__offset;
}

inline constexpr char* hack_obuffer_end_impl(FILE* fp) noexcept
{
	if(fp->__buffer_ptr)[[likely]]
		return fp->__buffer_ptr+fp->__io_offset;
	else
		return nullptr;
}

inline constexpr void hack_obuffer_set_curr_impl(FILE* fp,char* ptr) noexcept
{
	std::size_t new_size{static_cast<std::size_t>(ptr-fp->__buffer_ptr)};
	if(fp->__dirty_begin==fp->__io_offset)
	{
		fp->__dirty_begin=new_size;
		fp->__offset=new_size;
	}
	fp->__io_offset=new_size;
	fp->__valid_limit=new_size;
	fp->__dirty_end=new_size;
}

inline void hack_overflow_impl(FILE* fp,char unsigned ch)
{
	if(noexcept_call(putc_unlocked,ch,fp)==EOF)
		throw_posix_error();
}

inline bool hack_underflow_impl(FILE* fp)
{
	int ch{noexcept_call(getc_unlocked,fp)};
	if(ch==EOF)
	{
		if(noexcept_call(ferror_unlocked,fp))
			throw_posix_error();
		return false;
	}
	fp->__offset=fp->__io_offset=0;
	return true;
}

}

inline constexpr char* obuffer_begin(c_io_observer_unlocked ciob) noexcept
{
	return details::mlibc_hack::hack_obuffer_begin_impl(ciob.fp);
}

inline constexpr char* obuffer_curr(c_io_observer_unlocked ciob) noexcept
{
	return details::mlibc_hack::hack_obuffer_curr_impl(ciob.fp);
}

inline constexpr char* obuffer_end(c_io_observer_unlocked ciob) noexcept
{
	return details::mlibc_hack::hack_obuffer_end_impl(ciob.fp);
}

inline constexpr void obuffer_set_curr(c_io_observer_unlocked ciob,char* ptr) noexcept
{
	details::mlibc_hack::hack_obuffer_set_curr_impl(ciob.fp,ptr);
}

inline void obuffer_overflow(c_io_observer_unlocked ciob,char ch)
{
	details::mlibc_hack::hack_overflow_impl(ciob.fp,static_cast<char unsigned>(ch));
}

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* obuffer_begin(u8c_io_observer_unlocked ciob) noexcept
{
	return reinterpret_cast<char8_t*>(details::mlibc_hack::hack_obuffer_begin_impl(ciob.fp));
}

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* obuffer_curr(u8c_io_observer_unlocked ciob) noexcept
{
	return reinterpret_cast<char8_t*>(details::mlibc_hack::hack_obuffer_curr_impl(ciob.fp));
}

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* obuffer_end(u8c_io_observer_unlocked ciob) noexcept
{
	return reinterpret_cast<char8_t*>(details::mlibc_hack::hack_obuffer_end_impl(ciob.fp));
}

inline void obuffer_set_curr(u8c_io_observer_unlocked ciob,char8_t* ptr) noexcept
{
	details::mlibc_hack::hack_obuffer_set_curr_impl(ciob.fp,reinterpret_cast<char*>(ptr));
}

inline void obuffer_overflow(u8c_io_observer_unlocked ciob,char8_t ch)
{
	details::mlibc_hack::hack_overflow_impl(ciob.fp,static_cast<char unsigned>(ch));
}



inline constexpr char* ibuffer_begin(c_io_observer_unlocked ciob) noexcept
{
	return details::mlibc_hack::hack_ibuffer_begin_impl(ciob.fp);
}

inline constexpr char* ibuffer_curr(c_io_observer_unlocked ciob) noexcept
{
	return details::mlibc_hack::hack_ibuffer_curr_impl(ciob.fp);
}

inline constexpr char* ibuffer_end(c_io_observer_unlocked ciob) noexcept
{
	return details::mlibc_hack::hack_ibuffer_end_impl(ciob.fp);
}

inline constexpr void ibuffer_set_curr(c_io_observer_unlocked ciob,char* ptr) noexcept
{
	details::mlibc_hack::hack_ibuffer_set_curr_impl(ciob.fp,ptr);
}

inline bool ibuffer_underflow(c_io_observer_unlocked ciob)
{
	return details::mlibc_hack::hack_underflow_impl(ciob.fp);
}

#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* ibuffer_begin(u8c_io_observer_unlocked ciob) noexcept
{
	return reinterpret_cast<char8_t*>(details::mlibc_hack::hack_ibuffer_begin_impl(ciob.fp));
}
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* ibuffer_curr(u8c_io_observer_unlocked ciob) noexcept
{
	return reinterpret_cast<char8_t*>(details::mlibc_hack::hack_ibuffer_curr_impl(ciob.fp));
}
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
inline char8_t* ibuffer_end(u8c_io_observer_unlocked ciob) noexcept
{
	return reinterpret_cast<char8_t*>(details::mlibc_hack::hack_ibuffer_end_impl(ciob.fp));
}

inline bool ibuffer_underflow(u8c_io_observer_unlocked ciob)
{
	return details::mlibc_hack::hack_underflow_impl(ciob.fp);
}

inline void ibuffer_set_curr(u8c_io_observer_unlocked ciob,char8_t* ptr) noexcept
{
	details::mlibc_hack::hack_ibuffer_set_curr_impl(ciob.fp,reinterpret_cast<char*>(ptr));
}

}
