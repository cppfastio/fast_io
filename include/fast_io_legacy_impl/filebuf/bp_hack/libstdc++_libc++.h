#pragma once


//Let's do great brutal hack to GCC's libstdc++ and LLVM's libc++ with memcpy
namespace fast_io
{

/*
https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/include/std/streambuf

*  This is based on _IO_FILE, just reordered to be more consistent,
*  and is intended to be the most minimal abstraction for an
*  internal buffer.
*  -  get == input == read
*  -  put == output == write

char_type* 		_M_in_beg;     ///< Start of get area.
char_type* 		_M_in_cur;     ///< Current read area.
char_type* 		_M_in_end;     ///< End of get area.
char_type* 		_M_out_beg;    ///< Start of put area.
char_type* 		_M_out_cur;    ///< Current put area.
char_type* 		_M_out_end;    ///< End of put area.

/// Current locale setting.
locale 			_M_buf_locale;


libc++
https://github.com/llvm-mirror/libcxx/blob/master/include/streambuf
    locale __loc_;
    char_type* __binp_;
    char_type* __ninp_;
    char_type* __einp_;
    char_type* __bout_;
    char_type* __nout_;
    char_type* __eout_;
*/
namespace details::streambuf_hack
{
template<std::size_t position,typename T>
requires (position<6)
inline typename T::char_type* hack_buffer_ptr(T* fb) noexcept
{
	constexpr std::size_t offset(
	sizeof(std::uintptr_t)	//vptr
#ifdef _LIBCPP_VERSION
	+sizeof(std::locale)
#endif
	+position*sizeof(std::uintptr_t));
	static_assert(offset+sizeof(std::uintptr_t)<=sizeof(T));

	typename T::char_type* value;
	::fast_io::details::my_memcpy(__builtin_addressof(value),
	reinterpret_cast<std::byte*>(fb)+offset,
	sizeof(std::uintptr_t));
	return value;
}
template<std::size_t position,typename T>
inline void hack_set_buffer_curr(T* fb,typename T::char_type* ptr) noexcept
{
	constexpr std::size_t offset(sizeof(std::uintptr_t)
#ifdef _LIBCPP_VERSION
	+sizeof(std::locale)
#endif
	+position*sizeof(std::uintptr_t));
	::fast_io::details::my_memcpy(reinterpret_cast<std::byte*>(fb)+offset,__builtin_addressof(ptr),sizeof(std::uintptr_t));
}
}



}