#pragma once

namespace fast_io
{

namespace details
{
inline ::std::byte* posix_pread_bytes_impl(int fd,::std::byte *first,::std::byte *last,::fast_io::intfpos_t off);
inline ::std::byte const* posix_pwrite_bytes_impl(int fd,::std::byte const *first,::std::byte const *last,::fast_io::intfpos_t off);
inline ::fast_io::io_scatter_status_t posix_scatter_pread_bytes_impl(int fd,
	::fast_io::io_scatter_t const *pscatter,::std::size_t n,::fast_io::intfpos_t off);
inline ::fast_io::io_scatter_status_t posix_scatter_pwrite_bytes_impl(int fd,
	::fast_io::io_scatter_t const *pscatter,::std::size_t n,::fast_io::intfpos_t off);

template<::fast_io::c_family family>
inline ::std::byte* c_pread_bytes_impl(FILE *fp,::std::byte *first,::std::byte *last,::fast_io::intfpos_t off)
{
	return posix_pread_bytes_impl(::fast_io::details::my_fileno_impl<family>(fp),first,last,off);
}

template<::fast_io::c_family family>
inline ::std::byte const* c_pwrite_bytes_impl(FILE *fp,::std::byte const *first,::std::byte const *last,::fast_io::intfpos_t off)
{
	return posix_pwrite_bytes_impl(::fast_io::details::my_fileno_impl<family>(fp),first,last,off);
}

template<::fast_io::c_family family>
inline ::fast_io::io_scatter_status_t c_scatter_pread_bytes_impl(FILE *fp,
	::fast_io::io_scatter_t const *pscatter,::std::size_t n,::fast_io::intfpos_t off)
{
	return posix_scatter_pread_bytes_impl(::fast_io::details::my_fileno_impl<family>(fp),pscatter,n,off);
}

template<::fast_io::c_family family>
inline ::std::byte const* c_scatter_pwrite_bytes_impl(FILE *fp,
	::fast_io::io_scatter_t const *pscatter,::std::size_t n,::fast_io::intfpos_t off)
{
	return posix_scatter_pwrite_bytes_impl(::fast_io::details::my_fileno_impl<family>(fp),pscatter,n,off);
}

}

template<::fast_io::c_family family,::std::integral char_type>
requires ::fast_io::operations::decay::defines::has_pread_some_bytes_underflow_define<::fast_io::basic_posix_io_observer<char_type>>
inline ::std::byte* pread_some_bytes_underflow_define(
::fast_io::basic_c_family_io_observer<family,char_type> piob,
::std::byte *first,::std::byte *last,::fast_io::intfpos_t off)
{
	return ::fast_io::details::c_pread_bytes_impl<family>(piob.fp,first,last,off);
}

template<::fast_io::c_family family,::std::integral char_type>
requires ::fast_io::operations::decay::defines::has_pwrite_some_bytes_overflow_define<::fast_io::basic_posix_io_observer<char_type>>
inline ::std::byte const* pwrite_some_bytes_overflow_define(
::fast_io::basic_c_family_io_observer<family,char_type> piob,
::std::byte const* first,::std::byte const* last,::fast_io::intfpos_t off)
{
	return ::fast_io::details::c_pwrite_bytes_impl<family>(piob.fp,first,last,off);
}

template<::fast_io::c_family family,::std::integral char_type>
requires ::fast_io::operations::decay::defines::has_scatter_pread_some_bytes_underflow_define<::fast_io::basic_posix_io_observer<char_type>>
inline ::fast_io::io_scatter_status_t scatter_pread_some_bytes_underflow_define(
::fast_io::basic_c_family_io_observer<family,char_type> piob,
::fast_io::io_scatter_t const *pscatters,::std::size_t n,::fast_io::intfpos_t fpos)
{
	return ::fast_io::details::c_scatter_pread_bytes_impl<family>(piob.fp,pscatters,n,fpos);
}

template<::fast_io::c_family family,::std::integral char_type>
requires ::fast_io::operations::decay::defines::has_scatter_pwrite_some_bytes_overflow_define<::fast_io::basic_posix_io_observer<char_type>>
inline ::fast_io::io_scatter_status_t scatter_pwrite_some_bytes_overflow_define(
::fast_io::basic_c_family_io_observer<family,char_type> piob,
::fast_io::io_scatter_t const *pscatters,::std::size_t n,::fast_io::intfpos_t fpos)
{
	return ::fast_io::details::c_scatter_pwrite_bytes_impl<family>(piob.fp,pscatters,n,fpos);
}

}
