#pragma once


namespace fast_io::operations
{

template <typename outstmtype, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::containers::span<char_type const> write_some_span(outstmtype &&outstm, ::fast_io::containers::span<char_type const> sp)
{
	auto first{sp.data()};
	auto last{first + sp.size()};
	return ::fast_io::containers::span<char_type const>(first, ::fast_io::details::write_some_impl(::fast_io::operations::output_stream_ref(outstm), first, last));
}

template <typename outstmtype, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void write_all_span(outstmtype &&outstm, ::fast_io::containers::span<char_type const> sp)
{
	auto first{sp.data()};
	auto last{first + sp.size()};
	::fast_io::details::write_some_impl(::fast_io::operations::output_stream_ref(outstm), first, last);
}

template <typename outstmtype, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::containers::span<::std::byte const> write_some_bytes_span(outstmtype &&outstm, ::fast_io::containers::span<::std::byte const> sp)
{
	auto first{sp.data()};
	auto last{first + sp.size()};
	return ::fast_io::containers::span<::std::byte const>(first, ::fast_io::details::write_some_bytes_impl(::fast_io::operations::output_stream_ref(outstm), first, last));
}

template <typename outstmtype, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void write_all_bytes_span(outstmtype &&outstm, ::fast_io::containers::span<::std::byte const> sp)
{
	auto first{sp.data()};
	auto last{first + sp.size()};
	::fast_io::details::write_all_bytes_impl(::fast_io::operations::output_stream_ref(outstm), first, last);
}

template <typename outstmtype>
inline constexpr io_scatter_status_t scatter_write_some_bytes_span(outstmtype &&outstm, ::fast_io::containers::span<::fast_io::io_scatter_t const> sp)
{
	using io_scatter_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= io_scatter_t const *;
	return ::fast_io::details::scatter_write_some_bytes_impl(::fast_io::operations::output_stream_ref(outstm),
															 reinterpret_cast<io_scatter_may_alias_const_ptr>(sp.data()),
															 sp.size());
}

template <typename outstmtype>
inline constexpr void scatter_write_all_bytes_span(outstmtype &&outstm, ::fast_io::containers::span<::fast_io::io_scatter_t const> sp)
{
	using io_scatter_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= io_scatter_t const *;
	::fast_io::details::scatter_write_all_bytes_impl(::fast_io::operations::output_stream_ref(outstm),
													 reinterpret_cast<io_scatter_may_alias_const_ptr>(sp.data()),
													 sp.size());
}

template <typename outstmtype>
inline constexpr io_scatter_status_t scatter_write_some_span(outstmtype &&outstm, ::fast_io::containers::span<::fast_io::basic_io_scatter_t<typename decltype(::fast_io::operations::output_stream_ref(outstm))::output_char_type> const> sp)
{
	using io_scatter_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= basic_io_scatter_t<typename decltype(::fast_io::operations::output_stream_ref(outstm))::output_char_type> const *;
	return ::fast_io::details::scatter_write_some_impl(::fast_io::operations::output_stream_ref(outstm),
													   reinterpret_cast<io_scatter_may_alias_const_ptr>(sp.data()),
													   sp.size());
}

template <typename outstmtype>
inline constexpr void scatter_write_all_span(outstmtype &&outstm, ::fast_io::containers::span<::fast_io::basic_io_scatter_t<typename decltype(::fast_io::operations::output_stream_ref(outstm))::output_char_type> const> sp)
{
	using io_scatter_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= basic_io_scatter_t<typename decltype(::fast_io::operations::output_stream_ref(outstm))::output_char_type> const *;
	::fast_io::details::scatter_write_all_impl(::fast_io::operations::output_stream_ref(outstm),
											   reinterpret_cast<io_scatter_may_alias_const_ptr>(sp.data()),
											   sp.size());
}

template <typename outstmtype, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::containers::span<char_type const> pwrite_some_span(outstmtype &&outstm, ::fast_io::containers::span<char_type const> sp, ::fast_io::intfpos_t off)
{
	auto first{sp.data()};
	auto last{first + sp.size()};
	return ::fast_io::containers::span<char_type const>(first, ::fast_io::details::pwrite_some_impl(::fast_io::operations::output_stream_ref(outstm), first, last, off));
}

template <typename outstmtype, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void pwrite_all_span(outstmtype &&outstm, ::fast_io::containers::span<char_type const> sp, ::fast_io::intfpos_t off)
{
	auto first{sp.data()};
	auto last{first + sp.size()};
	::fast_io::details::pwrite_all_impl(::fast_io::operations::output_stream_ref(outstm), first, last, off);
}

template <typename outstmtype, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::containers::span<::std::byte const> pwrite_some_bytes_span(outstmtype &&outstm, ::fast_io::containers::span<::std::byte const> sp, ::fast_io::intfpos_t off)
{
	auto first{sp.data()};
	auto last{first + sp.size()};
	return ::fast_io::containers::span<::std::byte const>(first, ::fast_io::details::pwrite_some_bytes_impl(::fast_io::operations::output_stream_ref(outstm), first, last, off));
}

template <typename outstmtype, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void pwrite_all_bytes_span(outstmtype &&outstm, ::fast_io::containers::span<::std::byte const> sp, ::fast_io::intfpos_t off)
{
	auto first{sp.data()};
	auto last{first + sp.size()};
	::fast_io::details::pwrite_all_bytes_impl(::fast_io::operations::output_stream_ref(outstm), first, last, off);
}

template <typename outstmtype>
inline constexpr io_scatter_status_t scatter_pwrite_some_bytes_span(outstmtype &&outstm, ::fast_io::containers::span<::fast_io::io_scatter_t const> sp, ::fast_io::intfpos_t off)
{

	return ::fast_io::details::scatter_pwrite_some_bytes_impl(::fast_io::operations::output_stream_ref(outstm),
															  sp.data(), sp.size(), off);
}

template <typename outstmtype>
inline constexpr void scatter_pwrite_all_bytes_span(outstmtype &&outstm, ::fast_io::containers::span<::fast_io::io_scatter_t const> sp, ::fast_io::intfpos_t off)
{
	::fast_io::details::scatter_pwrite_all_bytes_impl(::fast_io::operations::output_stream_ref(outstm),
													  sp.data(), sp.size(), off);
}

template <typename outstmtype>
inline constexpr io_scatter_status_t scatter_pwrite_some_span(outstmtype &&outstm, ::fast_io::containers::span<::fast_io::basic_io_scatter_t<typename decltype(::fast_io::operations::output_stream_ref(outstm))::output_char_type> const> sp, ::fast_io::intfpos_t off)
{
	return ::fast_io::details::scatter_pwrite_some_impl(::fast_io::operations::output_stream_ref(outstm),
														sp.data(), sp.size(), off);
}

template <typename outstmtype>
inline constexpr void scatter_pwrite_all_span(outstmtype &&outstm, ::fast_io::containers::span<::fast_io::basic_io_scatter_t<typename decltype(::fast_io::operations::output_stream_ref(outstm))::output_char_type> const> sp, ::fast_io::intfpos_t off)
{

	::fast_io::details::scatter_pwrite_all_impl(::fast_io::operations::output_stream_ref(outstm), sp.data(), sp.size(), off);
}

} // namespace fast_io::operations
