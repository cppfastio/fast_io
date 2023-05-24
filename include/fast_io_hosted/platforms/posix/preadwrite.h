#pragma once

namespace fast_io
{

namespace details
{

#ifndef __wasi__
inline ::std::byte* posix_pread_bytes_impl(int fd,::std::byte *first,::std::byte *last,::fast_io::intfpos_t off)
{
	auto ret{::fast_io::noexcept_call(::pread,fd,first,static_cast<::std::size_t>(last-first),off)};
	if(ret<0)
	{
		::fast_io::throw_posix_error();
	}
	return first+ret;
}

inline ::std::byte const* posix_write_bytes_impl(int fd,::std::byte const *first,::std::byte const *last,::fast_io::intfpos_t off)
{
	auto ret{::fast_io::noexcept_call(::pwrite,fd,first,static_cast<::std::size_t>(last-first))};
	if(ret<0)
	{
		::fast_io::throw_posix_error();
	}
	return first+ret;
}
#endif

inline ::fast_io::io_scatter_status_t posix_scatter_pread_bytes_impl(int fd,
	::fast_io::io_scatter_t const *pscatter,::std::size_t n,::fast_io::intfpos_t off)
{
#if defined(__wasi__)
	using iovec_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= __wasi_iovec_t const*;
	::std::size_t ret;
	auto val{noexcept_call(__wasi_fd_read,
		reinterpret_cast<iovec_may_alias_const_ptr>(pscatter),
		static_cast<::std::size_t>(last-first),off,__builtin_addressof(ret))};
	if(val)
	{
		::fast_io::throw_posix_error(val);
	}
#else
	using iovec_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= struct iovec const*;

	auto ret{::fast_io::noexcept_call(::readv,
		reinterpret_cast<iovec_may_alias_const_ptr>(pscatter),
		static_cast<::std::size_t>(last-first),off)};
	if(ret<0)
	{
		::fast_io::throw_posix_error();
	}
#endif
	return scatter_size_to_status(static_cast<::std::size_t>(ret),pscatter,n);
}

inline ::fast_io::io_scatter_status_t posix_scatter_pwrite_bytes_impl(int fd,
	::fast_io::io_scatter_t const *pscatter,::std::size_t n,::fast_io::intfpos_t off)
{
#if defined(__wasi__)
	using iovec_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= __wasi_iovec_t const*;
	::std::size_t ret;
	auto val{noexcept_call(__wasi_fd_write,
		reinterpret_cast<iovec_may_alias_const_ptr>(pscatter),
		static_cast<::std::size_t>(last-first),off,__builtin_addressof(ret))};
	if(val)
	{
		::fast_io::throw_posix_error(val);
	}
#else
	using iovec_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= struct iovec const*;

	auto ret{::fast_io::noexcept_call(::writev,
		reinterpret_cast<iovec_may_alias_const_ptr>(pscatter),
		static_cast<::std::size_t>(last-first),off)};
	if(ret<0)
	{
		::fast_io::throw_posix_error();
	}
#endif
	return scatter_size_to_status(static_cast<::std::size_t>(ret),pscatter,n);
}

}

#ifndef __wasi__

template<::std::integral char_type>
inline ::std::byte* pread_some_bytes_underflow_define(
::fast_io::basic_posix_io_observer<char_type> piob,
::std::byte *first,::std::byte *last,::fast_io::intfpos_t off)
{
	return ::fast_io::details::posix_read_bytes_impl(piob.fd,first,last,off);
}

template<::std::integral char_type>
inline ::std::byte const* pwrite_some_bytes_overflow_define(
::fast_io::basic_posix_io_observer<char_type> piob,
::std::byte const* first,::std::byte const* last,::fast_io::intfpos_t off)
{
	return ::fast_io::details::posix_write_bytes_impl(piob.fd,first,last,off);
}

#endif

template<::std::integral char_type>
inline ::fast_io::io_scatter_status_t scatter_pread_some_bytes_underflow_define(
::fast_io::basic_posix_io_observer<char_type> piob,
::fast_io::io_scatter_t const *pscatters,::std::size_t n,::fast_io::uintfpos_t fpos)
{
	return ::fast_io::details::posix_scatter_pread_bytes_impl(piob.fd,pscatters,n,fpos);
}

template<::std::integral char_type>
inline ::fast_io::io_scatter_status_t scatter_pwrite_some_bytes_overflow_define(
::fast_io::basic_posix_io_observer<char_type> piob,
::fast_io::io_scatter_t const *pscatters,::std::size_t n,::fast_io::uintfpos_t fpos)
{
	return ::fast_io::details::posix_scatter_pwrite_bytes_impl(piob.fd,pscatters,n,fpos);
}

}
