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

template<::fast_io::output_stream outstmtype,::std::integral char_type>
requires ::fast_io::details::write_can_allow_this_type<outstmtype,char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr char_type const* write_some(outstmtype&& outstm,char_type const *first, char_type const *last)
{
	return ::fast_io::details::write_some_impl(::fast_io::manipulators::output_stream_ref(outstm),first,last);
}

template<::fast_io::output_stream outstmtype,::std::integral char_type>
requires ::fast_io::details::write_can_allow_this_type<outstmtype,char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void write_all(outstmtype&& outstm,char_type const *first, char_type const *last)
{
	return ::fast_io::details::write_all_impl(::fast_io::manipulators::output_stream_ref(outstm),first,last);
}

template<::fast_io::output_stream outstmtype>
requires ::fast_io::details::write_bytes_can_allowing<outstmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::std::byte const* write_some_bytes(outstmtype&& outstm,::std::byte const *first, ::std::byte const *last)
{
	return ::fast_io::details::write_some_bytes_impl(::fast_io::manipulators::output_stream_ref(outstm),first,last);
}

template<::fast_io::output_stream outstmtype>
requires ::fast_io::details::write_bytes_can_allowing<outstmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void write_all_bytes(outstmtype&& outstm,::std::byte const *first, ::std::byte const *last)
{
	return ::fast_io::details::write_all_bytes_impl(::fast_io::manipulators::output_stream_ref(outstm),first,last);
}

template<::fast_io::output_stream outstmtype>
requires ::fast_io::details::write_bytes_can_allowing<outstmtype>
inline constexpr io_scatter_status_t scatter_write_some_bytes(outstmtype&& outstm,
	io_scatter_t const* pscatter,::std::size_t len)
{
	return ::fast_io::details::scatter_write_some_bytes_impl(::fast_io::manipulators::output_stream_ref(outstm),pscatter,len);
}

template<::fast_io::output_stream outstmtype>
requires ::fast_io::details::write_bytes_can_allowing<outstmtype>
inline constexpr void scatter_write_all_bytes(outstmtype&& outstm,
	io_scatter_t const* pscatter,::std::size_t len)
{
	::fast_io::details::scatter_write_all_bytes_impl(::fast_io::manipulators::output_stream_ref(outstm),pscatter,len);
}

template<::fast_io::output_stream outstmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr io_scatter_status_t scatter_write_some(outstmtype&& outstm,
	basic_io_scatter_t<typename decltype(::fast_io::manipulators::output_stream_ref(outstm))::output_char_type> const* pscatter,::std::size_t len)
{
	return ::fast_io::details::scatter_write_some_impl(::fast_io::manipulators::output_stream_ref(outstm),pscatter,len);
}

template<::fast_io::output_stream outstmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void scatter_write_all(outstmtype&& outstm,
	basic_io_scatter_t<typename decltype(::fast_io::manipulators::output_stream_ref(outstm))::output_char_type> const* pscatter,::std::size_t len)
{
	return ::fast_io::details::scatter_write_all_impl(::fast_io::manipulators::output_stream_ref(outstm),pscatter,len);
}

template<::fast_io::output_stream outstmtype,::std::integral char_type>
requires ::fast_io::details::write_can_allow_this_type<outstmtype,char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr char_type const* pwrite_some(outstmtype&& outstm,char_type const *first, char_type const *last,::fast_io::intfpos_t off)
{
	return ::fast_io::details::pwrite_some_impl(::fast_io::manipulators::output_stream_ref(outstm),first,last,off);
}

template<::fast_io::output_stream outstmtype,::std::integral char_type>
requires ::fast_io::details::write_can_allow_this_type<outstmtype,char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void pwrite_all(outstmtype&& outstm,char_type const *first, char_type const *last,::fast_io::intfpos_t off)
{
	return ::fast_io::details::pwrite_all_impl(::fast_io::manipulators::output_stream_ref(outstm),first,last,off);
}

template<::fast_io::output_stream outstmtype>
requires ::fast_io::details::write_bytes_can_allowing<outstmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::std::byte const* pwrite_some_bytes(outstmtype&& outstm,::std::byte const *first, ::std::byte const *last,::fast_io::intfpos_t off)
{
	return ::fast_io::details::pwrite_some_bytes_impl(::fast_io::manipulators::output_stream_ref(outstm),first,last,off);
}

template<::fast_io::output_stream outstmtype>
requires ::fast_io::details::write_bytes_can_allowing<outstmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void pwrite_all_bytes(outstmtype&& outstm,::std::byte const *first, ::std::byte const *last,::fast_io::intfpos_t off)
{
	return ::fast_io::details::pwrite_all_bytes_impl(::fast_io::manipulators::output_stream_ref(outstm),first,last,off);
}

template<::fast_io::output_stream outstmtype>
requires ::fast_io::details::write_bytes_can_allowing<outstmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr io_scatter_status_t scatter_pwrite_some_bytes(outstmtype&& outstm,
	io_scatter_t const* pscatter,::std::size_t len,::fast_io::intfpos_t off)
{
	return ::fast_io::details::scatter_pwrite_some_bytes_impl(::fast_io::manipulators::output_stream_ref(outstm),pscatter,len,off);
}

template<::fast_io::output_stream outstmtype>
requires ::fast_io::details::write_bytes_can_allowing<outstmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void scatter_pwrite_all_bytes(outstmtype&& outstm,
	io_scatter_t const* pscatter,::std::size_t len, ::fast_io::intfpos_t off)
{
	::fast_io::details::scatter_pwrite_all_bytes_impl(::fast_io::manipulators::output_stream_ref(outstm),pscatter,len,off);
}

template<::fast_io::output_stream outstmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr io_scatter_status_t scatter_pwrite_some(outstmtype&& outstm,
	basic_io_scatter_t<typename decltype(::fast_io::manipulators::output_stream_ref(outstm))::output_char_type> const* pscatter,::std::size_t len,::fast_io::intfpos_t off)
{
	return ::fast_io::details::scatter_pwrite_some_impl(::fast_io::manipulators::output_stream_ref(outstm),pscatter,len,off);
}

template<::fast_io::output_stream outstmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void scatter_pwrite_all(outstmtype&& outstm,
	basic_io_scatter_t<typename decltype(::fast_io::manipulators::output_stream_ref(outstm))::output_char_type> const* pscatter,::std::size_t len,::fast_io::intfpos_t off)
{
	return ::fast_io::details::scatter_pwrite_all_impl(::fast_io::manipulators::output_stream_ref(outstm),pscatter,len,off);
}

/**
 * @brief Writes a single character to the output stream.
 * @tparam outstmtype The type of the output stream to write to, which should satisfy the output_stream concept.
 * @param outstm The output stream to write to.
 * @param ch The character to write to the output stream.
 * @note This function is marked constexpr, allowing its invocation in constant expressions.
 */
template<::fast_io::output_stream outstmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void char_put(outstmtype&& outstm,
	typename decltype(::fast_io::manipulators::output_stream_ref(outstm))::output_char_type ch)
{
	::fast_io::details::char_put_impl(::fast_io::manipulators::output_stream_ref(outstm),ch);
}

}

}
