#pragma once

namespace fast_io
{
/*
MSDOS should not contain any issues with multiple write calls
*/

namespace details
{

inline io_scatter_status_t posix_scatter_read_impl_with_normal_read(int fd, io_scatter_t const *scatters,
																	::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS
{
	for (::std::size_t i{}; i != n; ++i)
	{
		auto e{scatters[i]};
		auto ret{noexcept_call(::read, fd, const_cast<void *>(e.base), e.len)};
		if (ret == -1)
		{
			throw_posix_error();
		}
		::std::size_t tsize{static_cast<::std::size_t>(ret)};
		if (tsize != e.len)
		{
			return {i, tsize};
		}
	}
	return {n, 0};
}

inline io_scatter_status_t posix_scatter_write_impl_with_normal_write(int fd, io_scatter_t const *scatters,
																	  ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS
{
	for (::std::size_t i{}; i != n; ++i)
	{
		auto e{scatters[i]};
		auto ret{noexcept_call(::write, fd, e.base, e.len)};
		if (ret == -1)
		{
			throw_posix_error();
		}
		::std::size_t tsize{static_cast<::std::size_t>(ret)};
		if (tsize != e.len)
		{
			return {i, tsize};
		}
	}
	return {n, 0};
}

} // namespace details

template <::std::integral char_type>
inline io_scatter_status_t scatter_read_some_bytes_underflow_define(basic_posix_io_observer<char_type> piob,
																	io_scatter_t const *pscatters, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return details::posix_scatter_read_impl_with_normal_read(piob.fd, pscatters, n);
}

template <::std::integral char_type>
inline io_scatter_status_t scatter_write_some_bytes_overflow_define(basic_posix_io_observer<char_type> piob,
																	io_scatter_t const *pscatters, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return details::posix_scatter_write_impl_with_normal_write(piob.fd, pscatters, n);
}

} // namespace fast_io
