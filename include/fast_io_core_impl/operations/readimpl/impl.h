#pragma once

#include"basis.h"
#include"scatter.h"
#include"scatterbytes.h"
#include"pbasis.h"
#include"scatterp.h"
#include"scatterpbytes.h"

namespace fast_io
{

namespace operations
{

template<::fast_io::input_stream instmtype,::std::integral char_type>
requires ::fast_io::details::read_write_can_allow_this_type<instmtype,char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr char_type* read_some(instmtype&& instm,char_type *first, char_type *last)
{
	return ::fast_io::details::read_some_impl(::fast_io::manipulators::input_stream_ref(instm),first,last);
}

template<::fast_io::input_stream instmtype,::std::integral char_type>
requires ::fast_io::details::read_write_can_allow_this_type<instmtype,char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void read_all(instmtype&& instm,char_type *first, char_type *last)
{
	return ::fast_io::details::read_all_impl(::fast_io::manipulators::input_stream_ref(instm),first,last);
}

template<::fast_io::input_stream instmtype>
requires ::fast_io::details::read_write_bytes_can_allowing<instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::std::byte* read_some_bytes(instmtype&& instm,::std::byte *first, ::std::byte *last)
{
	return ::fast_io::details::read_some_bytes_impl(::fast_io::manipulators::input_stream_ref(instm),first,last);
}

template<::fast_io::input_stream instmtype>
requires ::fast_io::details::read_write_bytes_can_allowing<instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void read_all_bytes(instmtype&& instm,::std::byte *first, ::std::byte *last)
{
	return ::fast_io::details::read_all_bytes_impl(::fast_io::manipulators::input_stream_ref(instm),first,last);
}

template<::fast_io::input_stream instmtype>
requires ::fast_io::details::read_write_bytes_can_allowing<instmtype>
inline constexpr io_scatter_status_t scatter_read_some_bytes(instmtype&& instm,
	io_scatter_t* pscatter,::std::size_t len)
{
	return ::fast_io::details::scatter_read_some_bytes_impl(::fast_io::manipulators::input_stream_ref(instm),pscatter,len);
}

template<::fast_io::input_stream instmtype>
requires ::fast_io::details::read_write_bytes_can_allowing<instmtype>
inline constexpr void scatter_read_all_bytes(instmtype&& instm,
	io_scatter_t* pscatter,::std::size_t len)
{
	::fast_io::details::scatter_read_all_bytes_impl(::fast_io::manipulators::input_stream_ref(instm),pscatter,len);
}

template<::fast_io::input_stream instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr io_scatter_status_t scatter_read_some(instmtype&& instm,
	basic_io_scatter_t<typename decltype(::fast_io::manipulators::input_stream_ref(instm))::input_char_type>* pscatter,::std::size_t len)
{
	return ::fast_io::details::scatter_read_some_impl(::fast_io::manipulators::input_stream_ref(instm),pscatter,len);
}

template<::fast_io::input_stream instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void scatter_read_all(instmtype&& instm,
	basic_io_scatter_t<typename decltype(::fast_io::manipulators::input_stream_ref(instm))::input_char_type>* pscatter,::std::size_t len)
{
	return ::fast_io::details::scatter_read_all_impl(::fast_io::manipulators::input_stream_ref(instm),pscatter,len);
}

template<::fast_io::input_stream instmtype,::std::integral char_type>
requires ::fast_io::details::read_write_can_allow_this_type<instmtype,char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr char_type* pread_some(instmtype&& instm,char_type *first, char_type *last,::fast_io::intfpos_t off)
{
	return ::fast_io::details::pread_some_impl(::fast_io::manipulators::input_stream_ref(instm),first,last,off);
}

template<::fast_io::input_stream instmtype,::std::integral char_type>
requires ::fast_io::details::read_write_can_allow_this_type<instmtype,char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void pread_all(instmtype&& instm,char_type *first, char_type *last,::fast_io::intfpos_t off)
{
	return ::fast_io::details::pread_all_impl(::fast_io::manipulators::input_stream_ref(instm),first,last,off);
}

template<::fast_io::input_stream instmtype>
requires ::fast_io::details::read_write_bytes_can_allowing<instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::std::byte* pread_some_bytes(instmtype&& instm,::std::byte *first, ::std::byte *last,::fast_io::intfpos_t off)
{
	return ::fast_io::details::pread_some_bytes_impl(::fast_io::manipulators::input_stream_ref(instm),first,last,off);
}

template<::fast_io::input_stream instmtype>
requires ::fast_io::details::read_write_bytes_can_allowing<instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void pread_all_bytes(instmtype&& instm,::std::byte *first, ::std::byte *last,::fast_io::intfpos_t off)
{
	return ::fast_io::details::pread_all_bytes_impl(::fast_io::manipulators::input_stream_ref(instm),first,last,off);
}

template<::fast_io::input_stream instmtype>
requires ::fast_io::details::read_write_bytes_can_allowing<instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr io_scatter_status_t scatter_pread_some_bytes(instmtype&& instm,
	io_scatter_t* pscatter,::std::size_t len,::fast_io::intfpos_t off)
{
	return ::fast_io::details::scatter_pread_some_bytes_impl(::fast_io::manipulators::input_stream_ref(instm),pscatter,len,off);
}

template<::fast_io::input_stream instmtype>
requires ::fast_io::details::read_write_bytes_can_allowing<instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void scatter_pread_all_bytes(instmtype&& instm,
	io_scatter_t* pscatter,::std::size_t len, ::fast_io::intfpos_t off)
{
	::fast_io::details::scatter_pread_all_bytes_impl(::fast_io::manipulators::input_stream_ref(instm),pscatter,len,off);
}

template<::fast_io::input_stream instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr io_scatter_status_t scatter_pread_some(instmtype&& instm,
	basic_io_scatter_t<typename decltype(::fast_io::manipulators::input_stream_ref(instm))::input_char_type>* pscatter,::std::size_t len,::fast_io::intfpos_t off)
{
	return ::fast_io::details::scatter_pread_some_impl(::fast_io::manipulators::input_stream_ref(instm),pscatter,len,off);
}

template<::fast_io::input_stream instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void scatter_pread_all(instmtype&& instm,
	basic_io_scatter_t<typename decltype(::fast_io::manipulators::input_stream_ref(instm))::input_char_type>* pscatter,::std::size_t len,::fast_io::intfpos_t off)
{
	return ::fast_io::details::scatter_pread_all_impl(::fast_io::manipulators::input_stream_ref(instm),pscatter,len,off);
}

/**
 * @brief reads a single character to the input stream.
 * @tparam instmtype The type of the input stream to read to, which should satisfy the input_stream concept.
 * @param instm The input stream to read to.
 * @param ch The character to read to the input stream.
 * @note This function is marked constexpr, allowing its invocation in constant expressions.
 */
template<::fast_io::input_stream instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void char_put(instmtype&& instm,
	typename decltype(::fast_io::manipulators::input_stream_ref(instm))::input_char_type ch)
{
	::fast_io::details::char_put_impl(::fast_io::manipulators::input_stream_ref(instm),ch);
}

}

}
