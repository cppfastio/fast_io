#pragma once

namespace fast_io
{

namespace details
{

template<::std::integral char_type,typename traits_type>
inline ::std::byte* filebuf_pread_bytes_impl(::std::basic_filebuf<char_type,traits_type>* fb,
	::std::byte *first,::std::byte *last,::fast_io::intfpos_t off)
{
	return ::fast_io::details::c_pread_bytes_impl<::fast_io::c_family::native_unlocked>(
		::fast_io::details::streambuf_hack::fp_hack(fb),
		first,last,off);
}

template<std::integral char_type,typename traits_type>
inline ::std::byte const* filebuf_pwrite_bytes_impl(
	::std::basic_filebuf<char_type,traits_type>* fb,
	::std::byte const *first,::std::byte const *last,::fast_io::intfpos_t off)
{
	return ::fast_io::details::c_pwrite_bytes_impl<::fast_io::c_family::native_unlocked>(
		::fast_io::details::streambuf_hack::fp_hack(fb),
		first,last,off);
}

template<std::integral char_type,typename traits_type>
inline ::fast_io::io_scatter_status_t filebuf_scatter_pread_bytes_impl(
	::std::basic_filebuf<char_type,traits_type>* fb,
	::fast_io::io_scatter_t const *pscatter,::std::size_t n,::fast_io::intfpos_t off)
{
	return ::fast_io::details::c_pwrite_bytes_impl<::fast_io::c_family::native_unlocked>(
		::fast_io::details::streambuf_hack::fp_hack(fb),pscatter,n,off);
}

template<std::integral char_type,typename traits_type>
inline ::std::byte const* filebuf_scatter_pwrite_bytes_impl(
	::std::basic_filebuf<char_type,traits_type>* fb,
	::fast_io::io_scatter_t const *pscatter,::std::size_t n,::fast_io::intfpos_t off)
{
	return ::fast_io::details::c_pwrite_bytes_impl<::fast_io::c_family::native_unlocked>(
		::fast_io::details::streambuf_hack::fp_hack(fb),pscatter,n,off);
}

}

template<std::integral char_type,typename traits_type>
requires ::fast_io::operations::decay::defines::has_pread_some_bytes_underflow_define<
	::fast_io::basic_c_io_observer_unlocked<char_type>>
inline ::std::byte* pread_some_bytes_underflow_define(
::fast_io::basic_filebuf_io_observer<char_type,traits_type> fiob,
::std::byte *first,::std::byte *last,::fast_io::intfpos_t off)
{
	return ::fast_io::details::filebuf_pread_bytes_impl(fiob.fb,first,last,off);
}

template<std::integral char_type,typename traits_type>
requires ::fast_io::operations::decay::defines::has_pwrite_some_bytes_overflow_define<
	::fast_io::basic_c_io_observer_unlocked<char_type>>
inline ::std::byte const* pwrite_some_bytes_overflow_define(
::fast_io::basic_filebuf_io_observer<char_type,traits_type> fiob,
::std::byte const* first,::std::byte const* last,::fast_io::intfpos_t off)
{
	return ::fast_io::details::filebuf_pwrite_bytes_impl(fiob.fb,first,last,off);
}

template<std::integral char_type,typename traits_type>
requires ::fast_io::operations::decay::defines::has_scatter_pread_some_bytes_underflow_define<
	::fast_io::basic_c_io_observer_unlocked<char_type>>
inline ::fast_io::io_scatter_status_t scatter_pread_some_bytes_underflow_define(
::fast_io::basic_filebuf_io_observer<char_type,traits_type> fiob,
::fast_io::io_scatter_t const *pscatters,::std::size_t n,::fast_io::intfpos_t fpos)
{
	return ::fast_io::details::filebuf_scatter_pread_bytes_impl(fiob.fb,pscatters,n,fpos);
}

template<std::integral char_type,typename traits_type>
requires ::fast_io::operations::decay::defines::has_scatter_pwrite_some_bytes_overflow_define<
	::fast_io::basic_c_io_observer_unlocked<char_type>>
inline ::fast_io::io_scatter_status_t scatter_pwrite_some_bytes_overflow_define(
::fast_io::basic_filebuf_io_observer<char_type,traits_type> fiob,
::fast_io::io_scatter_t const *pscatters,::std::size_t n,::fast_io::intfpos_t fpos)
{
	return ::fast_io::details::filebuf_scatter_pwrite_bytes_impl(fiob.fb,pscatters,n,fpos);
}

}
