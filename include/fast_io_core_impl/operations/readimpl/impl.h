#pragma once

#include"basis.h"
#include"scatter.h"
#include"scatterbytes.h"
#include"pbasis.h"
#include"scatterp.h"
#include"scatterpbytes.h"
#include"until_eof.h"
#include"puntil_eof.h"
#include"scatter_until_eof.h"
#include"scatter_until_eofp.h"
#include"decay.h"

namespace fast_io
{

namespace operations
{

template<typename instmtype,::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr char_type* read_some(instmtype&& instm,char_type *first, char_type *last)
{
	return ::fast_io::details::read_some_impl(::fast_io::operations::input_stream_ref(instm),first,last);
}

template<typename instmtype,::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr char_type* read_until_eof(instmtype&& instm,char_type *first, char_type *last)
{
	return ::fast_io::operations::decay::read_until_eof_decay(::fast_io::operations::input_stream_ref(instm),first,last);
}

template<typename instmtype,::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void read_all(instmtype&& instm,char_type *first, char_type *last)
{
	return ::fast_io::details::read_all_impl(::fast_io::operations::input_stream_ref(instm),first,last);
}

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::std::byte* read_some_bytes(instmtype&& instm,::std::byte *first, ::std::byte *last)
{
	return ::fast_io::details::read_some_bytes_impl(::fast_io::operations::input_stream_ref(instm),first,last);
}

template<typename instmtype,::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::std::byte* read_until_eof_bytes(instmtype&& instm,::std::byte *first, ::std::byte *last)
{
	return ::fast_io::operations::decay::read_until_eof_bytes_decay(::fast_io::operations::input_stream_ref(instm),first,last);
}

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void read_all_bytes(instmtype&& instm,::std::byte *first, ::std::byte *last)
{
	return ::fast_io::details::read_all_bytes_impl(::fast_io::operations::input_stream_ref(instm),first,last);
}

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr io_scatter_status_t scatter_read_some_bytes(instmtype&& instm,
	io_scatter_t* pscatter,::std::size_t len)
{
	return ::fast_io::details::scatter_read_some_bytes_impl(::fast_io::operations::input_stream_ref(instm),pscatter,len);
}

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr io_scatter_status_t scatter_read_until_eof_bytes(instmtype&& instm,
	io_scatter_t* pscatter,::std::size_t len)
{
	return ::fast_io::operations::decay::scatter_read_until_eof_bytes_decay(::fast_io::operations::input_stream_ref(instm),pscatter,len);
}

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void scatter_read_all_bytes(instmtype&& instm,
	io_scatter_t* pscatter,::std::size_t len)
{
	::fast_io::details::scatter_read_all_bytes_impl(::fast_io::operations::input_stream_ref(instm),pscatter,len);
}

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr io_scatter_status_t scatter_read_some(instmtype&& instm,
	basic_io_scatter_t<typename decltype(::fast_io::operations::input_stream_ref(instm))::input_char_type> const *pscatter,::std::size_t len)
{
	return ::fast_io::details::scatter_read_some_impl(::fast_io::operations::input_stream_ref(instm),pscatter,len);
}

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void scatter_read_all(instmtype&& instm,
	basic_io_scatter_t<typename decltype(::fast_io::operations::input_stream_ref(instm))::input_char_type> const *pscatter,::std::size_t len)
{
	return ::fast_io::details::scatter_read_all_impl(::fast_io::operations::input_stream_ref(instm),pscatter,len);
}

template<typename instmtype,::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr char_type* pread_some(instmtype&& instm,char_type *first, char_type *last,::fast_io::intfpos_t off)
{
	return ::fast_io::details::pread_some_impl(::fast_io::operations::input_stream_ref(instm),first,last,off);
}

template<typename instmtype,::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void pread_all(instmtype&& instm,char_type *first, char_type *last,::fast_io::intfpos_t off)
{
	return ::fast_io::details::pread_all_impl(::fast_io::operations::input_stream_ref(instm),first,last,off);
}

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::std::byte* pread_some_bytes(instmtype&& instm,::std::byte *first, ::std::byte *last,::fast_io::intfpos_t off)
{
	return ::fast_io::details::pread_some_bytes_impl(::fast_io::operations::input_stream_ref(instm),first,last,off);
}

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void pread_all_bytes(instmtype&& instm,::std::byte *first, ::std::byte *last,::fast_io::intfpos_t off)
{
	return ::fast_io::details::pread_all_bytes_impl(::fast_io::operations::input_stream_ref(instm),first,last,off);
}

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr io_scatter_status_t scatter_pread_some_bytes(instmtype&& instm,
	io_scatter_t* pscatter,::std::size_t len,::fast_io::intfpos_t off)
{
	return ::fast_io::details::scatter_pread_some_bytes_impl(::fast_io::operations::input_stream_ref(instm),pscatter,len,off);
}

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void scatter_pread_all_bytes(instmtype&& instm,
	io_scatter_t* pscatter,::std::size_t len, ::fast_io::intfpos_t off)
{
	::fast_io::details::scatter_pread_all_bytes_impl(::fast_io::operations::input_stream_ref(instm),pscatter,len,off);
}

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr io_scatter_status_t scatter_pread_some(instmtype&& instm,
	basic_io_scatter_t<typename decltype(::fast_io::operations::input_stream_ref(instm))::input_char_type> const *pscatter,::std::size_t len,::fast_io::intfpos_t off)
{
	return ::fast_io::details::scatter_pread_some_impl(::fast_io::operations::input_stream_ref(instm),pscatter,len,off);
}

template<typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void scatter_pread_all(instmtype&& instm,
	basic_io_scatter_t<typename decltype(::fast_io::operations::input_stream_ref(instm))::input_char_type> const *pscatter,::std::size_t len,::fast_io::intfpos_t off)
{
	return ::fast_io::details::scatter_pread_all_impl(::fast_io::operations::input_stream_ref(instm),pscatter,len,off);
}

}

}
