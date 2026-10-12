#pragma once

namespace fast_io::operations::decay
{

template <typename instmtype>
inline constexpr typename instmtype::input_char_type *
read_some_decay(instmtype insm, typename instmtype::input_char_type *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::readable<instmtype> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
{
	return ::fast_io::details::read_some_impl(insm, first, count);
}

template <typename instmtype>
inline constexpr void read_all_decay(instmtype insm, typename instmtype::input_char_type *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::readable<instmtype> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
{
	::fast_io::details::read_all_impl(insm, first, count);
}

template <typename instmtype>
inline constexpr ::std::byte *read_some_bytes_decay(instmtype insm, ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::bytes_readable<instmtype> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
{
	return ::fast_io::details::read_some_bytes_impl(insm, first, count);
}

template <typename instmtype>
inline constexpr void read_all_bytes_decay(instmtype insm, ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::bytes_readable<instmtype> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
{
	::fast_io::details::read_all_bytes_impl(insm, first, count);
}

template <typename instmtype>
inline constexpr io_scatter_status_t
scatter_read_some_decay(instmtype insm, basic_io_scatter_t<typename instmtype::input_char_type> const *pscatters,
						::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::readable<instmtype> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
{
	return ::fast_io::details::scatter_read_some_impl(insm, pscatters, n);
}

template <typename instmtype>
inline constexpr io_scatter_status_t scatter_read_some_bytes_decay(instmtype insm, io_scatter_t const *pscatters,
																   ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::bytes_readable<instmtype> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
{
	return ::fast_io::details::scatter_read_some_bytes_impl(insm, pscatters, n);
}

template <typename instmtype>
inline constexpr void scatter_read_all_decay(instmtype insm,
											 basic_io_scatter_t<typename instmtype::input_char_type> const *pscatters,
											 ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::readable<instmtype> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
{
	::fast_io::details::scatter_read_all_impl(insm, pscatters, n);
}

template <typename instmtype>
inline constexpr void scatter_read_all_bytes_decay(instmtype insm, io_scatter_t const *pscatters, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::bytes_readable<instmtype> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
{
	::fast_io::details::scatter_read_all_bytes_impl(insm, pscatters, n);
}

template <typename instmtype>
inline constexpr typename instmtype::input_char_type *
pread_some_decay(instmtype insm, typename instmtype::input_char_type *first, ::std::size_t count,
				 ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::preadable<instmtype> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
{
	return ::fast_io::details::pread_some_impl(insm, first, count, off);
}

template <typename instmtype>
inline constexpr void pread_all_decay(instmtype insm, typename instmtype::input_char_type *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::preadable<instmtype> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
{
	::fast_io::details::pread_all_impl(insm, first, count, off);
}

template <typename instmtype>
inline constexpr ::std::byte *pread_some_bytes_decay(instmtype insm, ::std::byte *first, ::std::size_t count,
													 ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::bytes_preadable<instmtype> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
{
	return ::fast_io::details::pread_some_bytes_impl(insm, first, count, off);
}

template <typename instmtype>
inline constexpr void pread_all_bytes_decay(instmtype insm, ::std::byte *first, ::std::size_t count,
											::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::bytes_preadable<instmtype> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
{
	::fast_io::details::pread_all_bytes_impl(insm, first, count, off);
}

template <typename instmtype>
inline constexpr io_scatter_status_t
scatter_pread_some_decay(instmtype insm, basic_io_scatter_t<typename instmtype::input_char_type> const *pscatters,
						 ::std::size_t n, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::preadable<instmtype> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
{
	return ::fast_io::details::scatter_pread_some_impl(insm, pscatters, n, off);
}

template <typename instmtype>
inline constexpr io_scatter_status_t scatter_pread_some_bytes_decay(instmtype insm, io_scatter_t const *pscatters,
																	::std::size_t n, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::bytes_preadable<instmtype> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
{
	return ::fast_io::details::scatter_pread_some_bytes_impl(insm, pscatters, n, off);
}

template <typename instmtype>
inline constexpr void scatter_pread_all_decay(instmtype insm,
											  basic_io_scatter_t<typename instmtype::input_char_type> const *pscatters,
											  ::std::size_t n, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::preadable<instmtype> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
{
	::fast_io::details::scatter_pread_all_impl(insm, pscatters, n, off);
}

template <typename instmtype>
inline constexpr void scatter_pread_all_bytes_decay(instmtype insm, io_scatter_t const *pscatters, ::std::size_t n,
													::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype>)
	requires(::fast_io::operations::decay::defines::bytes_preadable<instmtype> || ::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<instmtype>)
{
	::fast_io::details::scatter_pread_all_bytes_impl(insm, pscatters, n, off);
}

} // namespace fast_io::operations::decay