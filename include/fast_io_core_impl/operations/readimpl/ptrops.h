#pragma once

namespace fast_io
{

namespace operations
{

template <typename instmtype, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr char_type *read_some(instmtype &&instm, char_type *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::readable<decltype(::fast_io::operations::input_stream_ref(instm))> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<decltype(::fast_io::operations::input_stream_ref(instm))>)
{
	return ::fast_io::details::read_some_impl(::fast_io::operations::input_stream_ref(instm), first, count);
}

template <typename instmtype, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void read_all(instmtype &&instm, char_type *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::readable<decltype(::fast_io::operations::input_stream_ref(instm))> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<decltype(::fast_io::operations::input_stream_ref(instm))>)
{
	return ::fast_io::details::read_all_impl(::fast_io::operations::input_stream_ref(instm), first, count);
}

template <typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::std::byte *read_some_bytes(instmtype &&instm, ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::bytes_readable<decltype(::fast_io::operations::input_stream_ref(instm))> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<decltype(::fast_io::operations::input_stream_ref(instm))>)
{
	return ::fast_io::details::read_some_bytes_impl(::fast_io::operations::input_stream_ref(instm), first, count);
}
template <typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void read_all_bytes(instmtype &&instm, ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::bytes_readable<decltype(::fast_io::operations::input_stream_ref(instm))> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<decltype(::fast_io::operations::input_stream_ref(instm))>)
{
	return ::fast_io::details::read_all_bytes_impl(::fast_io::operations::input_stream_ref(instm), first, count);
}

template <typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr io_scatter_status_t scatter_read_some_bytes(instmtype &&instm, io_scatter_t const *pscatter,
															 ::std::size_t len)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::bytes_readable<decltype(::fast_io::operations::input_stream_ref(instm))> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<decltype(::fast_io::operations::input_stream_ref(instm))>)
{
	return ::fast_io::details::scatter_read_some_bytes_impl(::fast_io::operations::input_stream_ref(instm), pscatter,
															len);
}

template <typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void scatter_read_all_bytes(instmtype &&instm, io_scatter_t const *pscatter, ::std::size_t len)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::bytes_readable<decltype(::fast_io::operations::input_stream_ref(instm))> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<decltype(::fast_io::operations::input_stream_ref(instm))>)
{
	::fast_io::details::scatter_read_all_bytes_impl(::fast_io::operations::input_stream_ref(instm), pscatter, len);
}

template <typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr io_scatter_status_t scatter_read_some(
	instmtype &&instm,
	basic_io_scatter_t<typename decltype(::fast_io::operations::input_stream_ref(instm))::input_char_type> const
		*pscatter,
	::std::size_t len)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::readable<decltype(::fast_io::operations::input_stream_ref(instm))> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<decltype(::fast_io::operations::input_stream_ref(instm))>)
{
	return ::fast_io::details::scatter_read_some_impl(::fast_io::operations::input_stream_ref(instm), pscatter, len);
}

template <typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void scatter_read_all(
	instmtype &&instm,
	basic_io_scatter_t<typename decltype(::fast_io::operations::input_stream_ref(instm))::input_char_type> const
		*pscatter,
	::std::size_t len)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::readable<decltype(::fast_io::operations::input_stream_ref(instm))> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<decltype(::fast_io::operations::input_stream_ref(instm))>)
{
	return ::fast_io::details::scatter_read_all_impl(::fast_io::operations::input_stream_ref(instm), pscatter, len);
}

template <typename instmtype, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr char_type *pread_some(instmtype &&instm, char_type *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::preadable<decltype(::fast_io::operations::input_stream_ref(instm))> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<decltype(::fast_io::operations::input_stream_ref(instm))>)
{
	return ::fast_io::details::pread_some_impl(::fast_io::operations::input_stream_ref(instm), first, count, off);
}

template <typename instmtype, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void pread_all(instmtype &&instm, char_type *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::preadable<decltype(::fast_io::operations::input_stream_ref(instm))> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<decltype(::fast_io::operations::input_stream_ref(instm))>)
{
	return ::fast_io::details::pread_all_impl(::fast_io::operations::input_stream_ref(instm), first, count, off);
}

template <typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::std::byte *pread_some_bytes(instmtype &&instm, ::std::byte *first, ::std::size_t count,
											   ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::bytes_preadable<decltype(::fast_io::operations::input_stream_ref(instm))> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<decltype(::fast_io::operations::input_stream_ref(instm))>)
{
	return ::fast_io::details::pread_some_bytes_impl(::fast_io::operations::input_stream_ref(instm), first, count, off);
}

template <typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void pread_all_bytes(instmtype &&instm, ::std::byte *first, ::std::size_t count,
									  ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::bytes_preadable<decltype(::fast_io::operations::input_stream_ref(instm))> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<decltype(::fast_io::operations::input_stream_ref(instm))>)
{
	return ::fast_io::details::pread_all_bytes_impl(::fast_io::operations::input_stream_ref(instm), first, count, off);
}

template <typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr io_scatter_status_t scatter_pread_some_bytes(instmtype &&instm, io_scatter_t const *pscatter,
															  ::std::size_t len, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::bytes_preadable<decltype(::fast_io::operations::input_stream_ref(instm))> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<decltype(::fast_io::operations::input_stream_ref(instm))>)
{
	return ::fast_io::details::scatter_pread_some_bytes_impl(::fast_io::operations::input_stream_ref(instm), pscatter,
															 len, off);
}

template <typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void scatter_pread_all_bytes(instmtype &&instm, io_scatter_t const *pscatter, ::std::size_t len,
											  ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::bytes_preadable<decltype(::fast_io::operations::input_stream_ref(instm))> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<decltype(::fast_io::operations::input_stream_ref(instm))>)
{
	::fast_io::details::scatter_pread_all_bytes_impl(::fast_io::operations::input_stream_ref(instm), pscatter, len,
													 off);
}

template <typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr io_scatter_status_t scatter_pread_some(
	instmtype &&instm,
	basic_io_scatter_t<typename decltype(::fast_io::operations::input_stream_ref(instm))::input_char_type> const
		*pscatter,
	::std::size_t len, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::preadable<decltype(::fast_io::operations::input_stream_ref(instm))> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<decltype(::fast_io::operations::input_stream_ref(instm))>)
{
	return ::fast_io::details::scatter_pread_some_impl(::fast_io::operations::input_stream_ref(instm), pscatter, len,
													   off);
}

template <typename instmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void scatter_pread_all(
	instmtype &&instm,
	basic_io_scatter_t<typename decltype(::fast_io::operations::input_stream_ref(instm))::input_char_type> const
		*pscatter,
	::std::size_t len, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::preadable<decltype(::fast_io::operations::input_stream_ref(instm))> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<decltype(::fast_io::operations::input_stream_ref(instm))>)
{
	return ::fast_io::details::scatter_pread_all_impl(::fast_io::operations::input_stream_ref(instm), pscatter, len,
													  off);
}

} // namespace operations

} // namespace fast_io
