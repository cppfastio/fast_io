#pragma once


namespace fast_io::operations
{

template <typename outstmtype, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::containers::span<char_type> read_some_span(outstmtype &&outstm, ::fast_io::containers::span<char_type> sp)
{
	auto first{sp.data()};
	auto last{first + sp.size()};
	return ::fast_io::containers::span<char_type>(first, ::fast_io::details::read_some_impl(::fast_io::operations::input_stream_ref(outstm), first, last));
}

template <typename outstmtype, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void read_all_span(outstmtype &&outstm, ::fast_io::containers::span<char_type> sp)
{
	auto first{sp.data()};
	auto last{first + sp.size()};
	::fast_io::details::read_some_impl(::fast_io::operations::input_stream_ref(outstm), first, last);
}

template <typename outstmtype, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::containers::span<::std::byte const> read_some_bytes_span(outstmtype &&outstm, ::fast_io::containers::span<::std::byte const> sp)
{
	auto first{sp.data()};
	auto last{first + sp.size()};
	return ::fast_io::containers::span<::std::byte const>(first, ::fast_io::details::read_some_bytes_impl(::fast_io::operations::input_stream_ref(outstm), first, last));
}

template <typename outstmtype, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void read_all_bytes_span(outstmtype &&outstm, ::fast_io::containers::span<::std::byte const> sp)
{
	auto first{sp.data()};
	auto last{first + sp.size()};
	::fast_io::details::read_all_bytes_impl(::fast_io::operations::input_stream_ref(outstm), first, last);
}

template <typename outstmtype>
inline constexpr io_scatter_status_t scatter_read_some_bytes_span(outstmtype &&outstm, ::fast_io::containers::span<::fast_io::io_scatter_t const> sp)
{
	using io_scatter_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= io_scatter_t const *;
	return ::fast_io::details::scatter_read_some_bytes_impl(::fast_io::operations::input_stream_ref(outstm),
															 reinterpret_cast<io_scatter_may_alias_const_ptr>(sp.data()),
															 sp.size());
}

template <typename outstmtype>
inline constexpr void scatter_read_all_bytes_span(outstmtype &&outstm, ::fast_io::containers::span<::fast_io::io_scatter_t const> sp)
{
	using io_scatter_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= io_scatter_t const *;
	::fast_io::details::scatter_read_all_bytes_impl(::fast_io::operations::input_stream_ref(outstm),
													 reinterpret_cast<io_scatter_may_alias_const_ptr>(sp.data()),
													 sp.size());
}

template <typename outstmtype>
inline constexpr io_scatter_status_t scatter_read_some_span(outstmtype &&outstm, ::fast_io::containers::span<::fast_io::basic_io_scatter_t<typename decltype(::fast_io::operations::input_stream_ref(outstm))::input_char_type> const> sp)
{
	using io_scatter_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= basic_io_scatter_t<typename decltype(::fast_io::operations::input_stream_ref(outstm))::input_char_type> const *;
	return ::fast_io::details::scatter_read_some_impl(::fast_io::operations::input_stream_ref(outstm),
													   reinterpret_cast<io_scatter_may_alias_const_ptr>(sp.data()),
													   sp.size());
}

template <typename outstmtype>
inline constexpr void scatter_read_all_span(outstmtype &&outstm, ::fast_io::containers::span<::fast_io::basic_io_scatter_t<typename decltype(::fast_io::operations::input_stream_ref(outstm))::input_char_type> const> sp)
{
	using io_scatter_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= basic_io_scatter_t<typename decltype(::fast_io::operations::input_stream_ref(outstm))::input_char_type> const *;
	::fast_io::details::scatter_read_all_impl(::fast_io::operations::input_stream_ref(outstm),
											   reinterpret_cast<io_scatter_may_alias_const_ptr>(sp.data()),
											   sp.size());
}

template <typename outstmtype, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::containers::span<char_type> pread_some_span(outstmtype &&outstm, ::fast_io::containers::span<char_type> sp, ::fast_io::intfpos_t off)
{
	auto first{sp.data()};
	auto last{first + sp.size()};
	return ::fast_io::containers::span<char_type>(first, ::fast_io::details::pread_some_impl(::fast_io::operations::input_stream_ref(outstm), first, last, off));
}

template <typename outstmtype, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void pread_all_span(outstmtype &&outstm, ::fast_io::containers::span<char_type> sp, ::fast_io::intfpos_t off)
{
	auto first{sp.data()};
	auto last{first + sp.size()};
	::fast_io::details::pread_all_impl(::fast_io::operations::input_stream_ref(outstm), first, last, off);
}

template <typename outstmtype, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::containers::span<::std::byte const> pread_some_bytes_span(outstmtype &&outstm, ::fast_io::containers::span<::std::byte const> sp, ::fast_io::intfpos_t off)
{
	auto first{sp.data()};
	auto last{first + sp.size()};
	return ::fast_io::containers::span<::std::byte const>(first, ::fast_io::details::pread_some_bytes_impl(::fast_io::operations::input_stream_ref(outstm), first, last, off));
}

template <typename outstmtype, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void pread_all_bytes_span(outstmtype &&outstm, ::fast_io::containers::span<::std::byte const> sp, ::fast_io::intfpos_t off)
{
	auto first{sp.data()};
	auto last{first + sp.size()};
	::fast_io::details::pread_all_bytes_impl(::fast_io::operations::input_stream_ref(outstm), first, last, off);
}

template <typename outstmtype>
inline constexpr io_scatter_status_t scatter_pread_some_bytes_span(outstmtype &&outstm, ::fast_io::containers::span<::fast_io::io_scatter_t const> sp, ::fast_io::intfpos_t off)
{

	return ::fast_io::details::scatter_pread_some_bytes_impl(::fast_io::operations::input_stream_ref(outstm),
															  sp.data(), sp.size(), off);
}

template <typename outstmtype>
inline constexpr void scatter_pread_all_bytes_span(outstmtype &&outstm, ::fast_io::containers::span<::fast_io::io_scatter_t const> sp, ::fast_io::intfpos_t off)
{
	::fast_io::details::scatter_pread_all_bytes_impl(::fast_io::operations::input_stream_ref(outstm),
													  sp.data(), sp.size(), off);
}

template <typename outstmtype>
inline constexpr io_scatter_status_t scatter_pread_some_span(outstmtype &&outstm, ::fast_io::containers::span<::fast_io::basic_io_scatter_t<typename decltype(::fast_io::operations::input_stream_ref(outstm))::input_char_type> const> sp, ::fast_io::intfpos_t off)
{
	return ::fast_io::details::scatter_pread_some_impl(::fast_io::operations::input_stream_ref(outstm),
														sp.data(), sp.size(), off);
}

template <typename outstmtype>
inline constexpr void scatter_pread_all_span(outstmtype &&outstm, ::fast_io::containers::span<::fast_io::basic_io_scatter_t<typename decltype(::fast_io::operations::input_stream_ref(outstm))::input_char_type> const> sp, ::fast_io::intfpos_t off)
{

	::fast_io::details::scatter_pread_all_impl(::fast_io::operations::input_stream_ref(outstm), sp.data(), sp.size(), off);
}

} // namespace fast_io::operations
