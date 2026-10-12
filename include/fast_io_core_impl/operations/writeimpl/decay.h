#pragma once

namespace fast_io::operations::decay
{

template <typename outstmtype>
inline constexpr typename outstmtype::output_char_type const *
write_some_decay(outstmtype outsm, typename outstmtype::output_char_type const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires(::fast_io::operations::decay::defines::writable<outstmtype> || ::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
{
	return ::fast_io::details::write_some_impl(outsm, first, count);
}

template <typename outstmtype>
inline constexpr void write_all_decay(outstmtype outsm, typename outstmtype::output_char_type const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires(::fast_io::operations::decay::defines::writable<outstmtype> || ::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
{
	::fast_io::details::write_all_impl(outsm, first, count);
}

template <typename outstmtype>
inline constexpr ::std::byte const *write_some_bytes_decay(outstmtype outsm, ::std::byte const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires(::fast_io::operations::decay::defines::bytes_writable<outstmtype> || ::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
{
	return ::fast_io::details::write_some_bytes_impl(outsm, first, count);
}

template <typename outstmtype>
inline constexpr void write_all_bytes_decay(outstmtype outsm, ::std::byte const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires(::fast_io::operations::decay::defines::bytes_writable<outstmtype> || ::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
{
	::fast_io::details::write_all_bytes_impl(outsm, first, count);
}

template <typename outstmtype>
inline constexpr io_scatter_status_t
scatter_write_some_decay(outstmtype outsm, basic_io_scatter_t<typename outstmtype::output_char_type> const *pscatters,
						 ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires(::fast_io::operations::decay::defines::writable<outstmtype> || ::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
{
	return ::fast_io::details::scatter_write_some_impl(outsm, pscatters, n);
}

template <typename outstmtype>
inline constexpr io_scatter_status_t scatter_write_some_bytes_decay(outstmtype outsm, io_scatter_t const *pscatters,
																	::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires(::fast_io::operations::decay::defines::bytes_writable<outstmtype> || ::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
{
	return ::fast_io::details::scatter_write_some_bytes_impl(outsm, pscatters, n);
}

template <typename outstmtype>
inline constexpr void
scatter_write_all_decay(outstmtype outsm, basic_io_scatter_t<typename outstmtype::output_char_type> const *pscatters,
						::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires(::fast_io::operations::decay::defines::writable<outstmtype> || ::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
{
	::fast_io::details::scatter_write_all_impl(outsm, pscatters, n);
}

template <typename outstmtype>
inline constexpr void scatter_write_all_bytes_decay(outstmtype outsm, io_scatter_t const *pscatters, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires(::fast_io::operations::decay::defines::bytes_writable<outstmtype> || ::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
{
	::fast_io::details::scatter_write_all_bytes_impl(outsm, pscatters, n);
}

template <typename outstmtype>
inline constexpr typename outstmtype::output_char_type const *
pwrite_some_decay(outstmtype outsm, typename outstmtype::output_char_type const *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires(::fast_io::operations::decay::defines::pwritable<outstmtype> || ::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
{
	return ::fast_io::details::pwrite_some_impl(outsm, first, count, off);
}

template <typename outstmtype>
inline constexpr void pwrite_all_decay(outstmtype outsm, typename outstmtype::output_char_type const *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires(::fast_io::operations::decay::defines::pwritable<outstmtype> || ::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
{
	::fast_io::details::pwrite_all_impl(outsm, first, count, off);
}

template <typename outstmtype>
inline constexpr ::std::byte const *pwrite_some_bytes_decay(outstmtype outsm, ::std::byte const *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires(::fast_io::operations::decay::defines::bytes_pwritable<outstmtype> || ::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
{
	return ::fast_io::details::pwrite_some_bytes_impl(outsm, first, count, off);
}

template <typename outstmtype>
inline constexpr void pwrite_all_bytes_decay(outstmtype outsm, ::std::byte const *first, ::std::size_t count,
											 ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires(::fast_io::operations::decay::defines::bytes_pwritable<outstmtype> || ::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
{
	::fast_io::details::pwrite_all_bytes_impl(outsm, first, count, off);
}

template <typename outstmtype>
inline constexpr io_scatter_status_t
scatter_pwrite_some_decay(outstmtype outsm, basic_io_scatter_t<typename outstmtype::output_char_type> const *pscatters,
						  ::std::size_t n, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires(::fast_io::operations::decay::defines::pwritable<outstmtype> || ::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
{
	return ::fast_io::details::scatter_pwrite_some_impl(outsm, pscatters, n, off);
}

template <typename outstmtype>
inline constexpr io_scatter_status_t scatter_pwrite_some_bytes_decay(outstmtype outsm, io_scatter_t const *pscatters,
																	 ::std::size_t n, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires(::fast_io::operations::decay::defines::bytes_pwritable<outstmtype> || ::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
{
	return ::fast_io::details::scatter_pwrite_some_bytes_impl(outsm, pscatters, n, off);
}

template <typename outstmtype>
inline constexpr void
scatter_pwrite_all_decay(outstmtype outsm, basic_io_scatter_t<typename outstmtype::output_char_type> const *pscatters,
						 ::std::size_t n, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires(::fast_io::operations::decay::defines::pwritable<outstmtype> || ::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
{
	::fast_io::details::scatter_pwrite_all_impl(outsm, pscatters, n, off);
}

template <typename outstmtype>
inline constexpr void scatter_pwrite_all_bytes_decay(outstmtype outsm, io_scatter_t const *pscatters, ::std::size_t n,
													 ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires(::fast_io::operations::decay::defines::bytes_pwritable<outstmtype> || ::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
{
	::fast_io::details::scatter_pwrite_all_bytes_impl(outsm, pscatters, n, off);
}

template <typename outstmtype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void char_put_decay(outstmtype outstm, typename outstmtype::output_char_type ch)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<outstmtype>)
	requires((::fast_io::operations::decay::defines::writable<outstmtype> || ::fast_io::operations::decay::defines::has_output_stream_char_put_overflow_define<outstmtype>) || ::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<outstmtype>)
{
	::fast_io::details::char_put_impl(outstm, ch);
}

} // namespace fast_io::operations::decay