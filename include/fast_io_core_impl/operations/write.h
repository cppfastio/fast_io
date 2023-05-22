#pragma once

namespace fast_io
{

namespace operations
{


template<::fast_io::output_stream outstmtype,::std::integral char_type>
requires requires(outstmtype&& outstm,char_type const *first)
{
	{::fast_io::details::write_some_impl(::fast_io::manipulators::output_stream_ref(outstm),first,first)}->::std::same_as<char_type const*>;
}
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
requires requires(outstmtype&& outstm,char_type const *first)
{
	::fast_io::details::write_all_impl(::fast_io::manipulators::output_stream_ref(outstm),first,first);
}
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
requires requires(outstmtype&& outstm,::std::byte const *first)
{
	{::fast_io::details::write_some_bytes_impl(::fast_io::manipulators::output_stream_ref(outstm),first,first)}->::std::same_as<::std::byte const*>;
}
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::std::byte const* write_some_bytes(outstmtype&& outstm,::std::byte const *first, ::std::byte *last)
{
	return ::fast_io::details::write_some_bytes_impl(::fast_io::manipulators::output_stream_ref(outstm),first,last);
}

template<::fast_io::output_stream outstmtype>
requires requires(outstmtype&& outstm,::std::byte const *first)
{
	::fast_io::details::write_all_bytes_impl(::fast_io::manipulators::output_stream_ref(outstm),first,first);
}
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void write_all_bytes(outstmtype&& outstm,::std::byte const *first, ::std::byte *last)
{
	return ::fast_io::details::write_all_bytes_impl(::fast_io::manipulators::output_stream_ref(outstm),first,last);
}

template<::fast_io::output_stream outstmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr io_scatter_status_t scatter_write_some_bytes(outstmtype&& outstm,
	io_scatter_t const* pscatter,::std::size_t len)
{
	return ::fast_io::details::scatter_write_some_bytes_impl(::fast_io::manipulators::output_stream_ref(outstm),pscatter,len);
}

template<::fast_io::output_stream outstmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
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


#if 0
template<::fast_io::output_stream outstmtype,::std::forward_iterator Iter>
requires requires(outstmtype&& outstm,Iter first, Iter last,intfpot_t off)
{
	{::fast_io::details::pwrite_some_impl(::fast_io::manipulators::output_stream_ref(outstm),first,last,off)}->::std::same_as<Iter>;
}
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr Iter pwrite_some(outstmtype&& outstm,Iter first,Iter last,intfpot_t off)
{
	return ::fast_io::details::pwrite_some_impl(::fast_io::manipulators::output_stream_ref(outstm),first,last,off);
}

template<::fast_io::output_stream outstmtype,::std::forward_iterator Iter>
requires requires(outstmtype&& outstm,Iter first, Iter last,intfpot_t off)
{
	::fast_io::details::pwrite_all_impl(::fast_io::manipulators::output_stream_ref(outstm),first,last,off);
}
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void pwrite_all(outstmtype&& outstm,Iter first,Iter last,intfpot_t off)
{
	::fast_io::details::pwrite_all_impl(::fast_io::manipulators::output_stream_ref(outstm),first,last,off);
}


template<::fast_io::output_stream outstmtype,::std::forward_iterator Iter>
requires requires(outstmtype&& outstm,Iter first, Iter last,intfpot_t off)
{
	{::fast_io::details::pwrite_some_bytes_impl(::fast_io::manipulators::output_stream_ref(outstm),first,last,off)}->::std::same_as<Iter>;
}
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr Iter pwrite_some_bytes(outstmtype&& outstm,Iter first,Iter last,intfpot_t off)
{
	return ::fast_io::details::pwrite_some_bytes_impl(::fast_io::manipulators::output_stream_ref(outstm),first,last,off);
}

template<::fast_io::output_stream outstmtype,::std::forward_iterator Iter>
requires requires(outstmtype&& outstm,Iter first, Iter last,intfpot_t off)
{
	::fast_io::details::pwrite_all_bytes_impl(::fast_io::manipulators::output_stream_ref(outstm),first,last,off);
}
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void pwrite_all_bytes(outstmtype&& outstm,Iter first,Iter last,intfpot_t off)
{
	::fast_io::details::pwrite_all_bytes_impl(::fast_io::manipulators::output_stream_ref(outstm),first,last,off);
}
#endif

}

}
