#pragma once

namespace fast_io
{

inline constexpr char* ibuffer_begin(c_io_observer_unlocked cio) noexcept
{
	return cio.fp->_IO_read_base;
}

inline constexpr char* ibuffer_curr(c_io_observer_unlocked cio) noexcept
{
	return cio.fp->_IO_read_ptr;
}

inline constexpr char* ibuffer_end(c_io_observer_unlocked cio) noexcept
{
	return cio.fp->_IO_read_end;
}

inline constexpr void ibuffer_set_curr(c_io_observer_unlocked cio,char* ptr) noexcept
{
	cio.fp->_IO_read_ptr=ptr;
}

extern int glibc_underflow (FILE*) noexcept __asm__("__underflow");
inline bool ibuffer_underflow(c_io_observer_unlocked cio) noexcept
{
	cio.fp->_IO_read_ptr=cio.fp->_IO_read_end;
	return glibc_underflow(cio.fp)!=EOF;
}

inline constexpr char* obuffer_begin(c_io_observer_unlocked cio) noexcept
{
	return cio.fp->_IO_write_base;
}

inline constexpr char* obuffer_curr(c_io_observer_unlocked cio) noexcept
{
	return cio.fp->_IO_write_ptr;
}

inline constexpr char* obuffer_end(c_io_observer_unlocked cio) noexcept
{
	return cio.fp->_IO_write_end;
}

inline constexpr void obuffer_set_curr(c_io_observer_unlocked cio,char* ptr) noexcept
{
	cio.fp->_IO_write_ptr=ptr;
}

extern int glibc_overflow (FILE *,int) noexcept __asm__("__overflow");

inline void obuffer_overflow(c_io_observer_unlocked cio,char ch)
{
	if(glibc_overflow(cio.fp,static_cast<int>(static_cast<unsigned char>(ch)))==EOF)[[unlikely]]
		throw_posix_error();
}


[[__gnu__::__may_alias__]] inline char8_t* ibuffer_begin(u8c_io_observer_unlocked cio) noexcept
{
	return bit_cast<char8_t*>(cio.fp->_IO_read_base);
}

[[__gnu__::__may_alias__]] inline char8_t* ibuffer_curr(u8c_io_observer_unlocked cio) noexcept
{
	return bit_cast<char8_t*>(cio.fp->_IO_read_ptr);
}

[[__gnu__::__may_alias__]] inline char8_t* ibuffer_end(u8c_io_observer_unlocked cio) noexcept
{
	return bit_cast<char8_t*>(cio.fp->_IO_read_end);
}

inline void ibuffer_set_curr(u8c_io_observer_unlocked cio,char8_t* ptr) noexcept
{
	cio.fp->_IO_read_ptr=bit_cast<char*>(ptr);
}

inline bool ibuffer_underflow(u8c_io_observer_unlocked cio) noexcept
{
	cio.fp->_IO_read_ptr=cio.fp->_IO_read_end;
	return glibc_underflow(cio.fp)!=EOF;
}

[[__gnu__::__may_alias__]] inline char8_t* obuffer_begin(u8c_io_observer_unlocked cio) noexcept
{
	return bit_cast<char8_t*>(cio.fp->_IO_write_base);
}

[[__gnu__::__may_alias__]] inline char8_t* obuffer_curr(u8c_io_observer_unlocked cio) noexcept
{
	return bit_cast<char8_t*>(cio.fp->_IO_write_ptr);
}

[[__gnu__::__may_alias__]] inline char8_t* obuffer_end(u8c_io_observer_unlocked cio) noexcept
{
	return bit_cast<char8_t*>(cio.fp->_IO_write_end);
}

inline void obuffer_set_curr(u8c_io_observer_unlocked cio,[[__gnu__::__may_alias__]] char8_t* ptr) noexcept
{
	cio.fp->_IO_write_ptr=bit_cast<char*>(ptr);
}

inline void obuffer_overflow(u8c_io_observer_unlocked cio,char8_t ch)
{
	if(glibc_overflow(cio.fp,static_cast<int>(ch))==EOF)[[unlikely]]
		throw_posix_error();
}

namespace details::fp_wide_hack
{
/*
https://github.com/lattera/glibc/blob/master/libio/bits/types/struct_FILE.h
*/
inline std::byte* hack_wide_data(FILE* fp) noexcept
{
	constexpr std::size_t off{sizeof(__off64_t)+2*sizeof(std::uintptr_t)};
	std::byte* value;
	::fast_io::details::my_memcpy(__builtin_addressof(value),reinterpret_cast<std::byte*>(__builtin_addressof(fp->_lock))+off,sizeof(std::byte*));
	return value;
}

template<std::size_t position,std::integral char_type>
requires (sizeof(char_type)==sizeof(char32_t))
inline char_type* hack_wp(FILE* fp) noexcept
{
	constexpr std::size_t off{position*sizeof(uintptr_t)};
	char_type* value;
	::fast_io::details::my_memcpy(__builtin_addressof(value),hack_wide_data(fp)+off,sizeof(wchar_t*));
	return value;
}
template<std::size_t position,std::integral char_type>
requires (sizeof(char_type)==sizeof(char32_t))
inline void hack_wpset(FILE* fp,char_type* ptr) noexcept
{
	constexpr std::size_t off{position*sizeof(uintptr_t)};
	::fast_io::details::my_memcpy(hack_wide_data(fp)+off,__builtin_addressof(ptr),sizeof(wchar_t*));
}
}
#if WCHAR_MAX == UINT_LEAST32_MAX
//wchar_t supports

inline wchar_t* ibuffer_begin(wc_io_observer_unlocked cio) noexcept
{
	return details::fp_wide_hack::hack_wp<2,wchar_t>(cio.fp);
}

inline wchar_t* ibuffer_curr(wc_io_observer_unlocked cio) noexcept
{
	return details::fp_wide_hack::hack_wp<0,wchar_t>(cio.fp);
}

inline wchar_t* ibuffer_end(wc_io_observer_unlocked cio) noexcept
{
	return details::fp_wide_hack::hack_wp<1,wchar_t>(cio.fp);
}

inline void ibuffer_set_curr(wc_io_observer_unlocked cio,wchar_t* ptr) noexcept
{
	details::fp_wide_hack::hack_wpset<0,wchar_t>(cio.fp,ptr);
}
#endif

namespace details
{
extern std::uint_least32_t glibc_wunderflow (FILE *) noexcept __asm__("__wunderflow");
}
#if WCHAR_MAX == UINT_LEAST32_MAX
inline bool ibuffer_underflow(wc_io_observer_unlocked cio) noexcept
{
	ibuffer_set_curr(cio,ibuffer_end(cio));
	return details::glibc_wunderflow(cio.fp)!=static_cast<std::uint_least32_t>(0xffffffffu);
}
inline wchar_t* obuffer_begin(wc_io_observer_unlocked cio) noexcept
{
	return details::fp_wide_hack::hack_wp<3,wchar_t>(cio.fp);
}

inline wchar_t* obuffer_curr(wc_io_observer_unlocked cio) noexcept
{
	return details::fp_wide_hack::hack_wp<4,wchar_t>(cio.fp);
}

inline wchar_t* obuffer_end(wc_io_observer_unlocked cio) noexcept
{
	return details::fp_wide_hack::hack_wp<5,wchar_t>(cio.fp);
}

inline void obuffer_set_curr(wc_io_observer_unlocked cio,wchar_t* ptr) noexcept
{
	ibuffer_set_curr(cio,ibuffer_end(cio));
	details::fp_wide_hack::hack_wpset<4,wchar_t>(cio.fp,ptr);
}
#endif
namespace details
{
extern std::uint_least32_t glibc_woverflow (FILE *,std::uint_least32_t) noexcept __asm__("__woverflow");
}

inline void obuffer_overflow(wc_io_observer_unlocked cio,wchar_t ch)
{
	if(details::glibc_woverflow(cio.fp,static_cast<std::uint_least32_t>(ch))==static_cast<std::uint_least32_t>(0xffffffffu))[[unlikely]]
		throw_posix_error();
}


[[__gnu__::__may_alias__]] inline char32_t* ibuffer_begin(u32c_io_observer_unlocked cio) noexcept
{
	return details::fp_wide_hack::hack_wp<2,char32_t>(cio.fp);
}

[[__gnu__::__may_alias__]] inline char32_t* ibuffer_curr(u32c_io_observer_unlocked cio) noexcept
{
	return details::fp_wide_hack::hack_wp<0,char32_t>(cio.fp);
}

[[__gnu__::__may_alias__]] inline char32_t* ibuffer_end(u32c_io_observer_unlocked cio) noexcept
{
	return details::fp_wide_hack::hack_wp<1,char32_t>(cio.fp);
}

inline void ibuffer_set_curr(u32c_io_observer_unlocked cio,[[__gnu__::__may_alias__]] char32_t* ptr) noexcept
{
	details::fp_wide_hack::hack_wpset<0,char32_t>(cio.fp,ptr);
}

inline bool ibuffer_underflow(u32c_io_observer_unlocked cio) noexcept
{
	ibuffer_set_curr(cio,ibuffer_end(cio));
	return details::glibc_wunderflow(cio.fp)!=static_cast<std::uint_least32_t>(0xffffffffu);
}

[[__gnu__::__may_alias__]] inline char32_t* obuffer_begin(u32c_io_observer_unlocked cio) noexcept
{
	return details::fp_wide_hack::hack_wp<3,char32_t>(cio.fp);
}

[[__gnu__::__may_alias__]] inline char32_t* obuffer_curr(u32c_io_observer_unlocked cio) noexcept
{
	return details::fp_wide_hack::hack_wp<4,char32_t>(cio.fp);
}

[[__gnu__::__may_alias__]] inline char32_t* obuffer_end(u32c_io_observer_unlocked cio) noexcept
{
	return details::fp_wide_hack::hack_wp<5,char32_t>(cio.fp);
}

inline void obuffer_set_curr(u32c_io_observer_unlocked cio,[[__gnu__::__may_alias__]] char32_t* ptr) noexcept
{
	details::fp_wide_hack::hack_wpset<4,char32_t>(cio.fp,ptr);
}

inline void obuffer_overflow(u32c_io_observer_unlocked cio,char32_t ch)
{
	if(details::glibc_woverflow(cio.fp,static_cast<std::uint_least32_t>(ch))==static_cast<std::uint_least32_t>(0xffffffffu))[[unlikely]]
		throw_posix_error();
}

namespace details
{
extern int glibc_flbf(FILE* fp) noexcept __asm__("__flbf");
}

template<std::integral ch_type>
inline bool obuffer_is_line_buffering(basic_c_io_observer_unlocked<ch_type> ciou) noexcept
{
	return details::glibc_flbf(ciou.fp);
}

}
