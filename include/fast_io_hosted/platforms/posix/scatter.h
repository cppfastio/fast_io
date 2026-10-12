#pragma once

namespace fast_io
{

namespace details
{

inline ::fast_io::io_scatter_status_t posix_scatter_read_bytes_impl(int fd, ::fast_io::io_scatter_t const *pscatter,
																	::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	// readv/writev reject iovcnt > IOV_MAX (1024 on Linux); loop over the scatters in chunks
	constexpr ::std::size_t iovec_max_number{
#if defined(__wasi__)
		// __wasi_fd_read/__wasi_fd_write are not readv(2); no iovcnt limit
		::std::numeric_limits<::std::size_t>::max()
#elif defined(IOV_MAX)
		IOV_MAX
#elif defined(__linux__)
		1024
#else
		::std::numeric_limits<::std::size_t>::max()
#endif
	};
	::std::size_t position{};
	while (n)
	{
		::std::size_t this_n{iovec_max_number < n ? iovec_max_number : n};
#if defined(__linux__) && defined(__NR_readv)
		auto ret{system_call<__NR_readv, ::std::ptrdiff_t>(fd, pscatter, this_n)};
		::fast_io::linux_system_call_throw_error(ret);
#elif defined(__wasi__)
		using iovec_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= __wasi_iovec_t const *;
		::std::size_t ret;
		auto val{noexcept_call(::__wasi_fd_read, fd, reinterpret_cast<iovec_may_alias_const_ptr>(pscatter), this_n,
							   __builtin_addressof(ret))};
		if (val)
		{
			::fast_io::throw_posix_error(val);
		}
#else
		using iovec_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= struct iovec const *;

		auto ret{::fast_io::noexcept_call(::readv, fd, reinterpret_cast<iovec_may_alias_const_ptr>(pscatter),
										  static_cast<int>(this_n))};
		if (ret == -1)
		{
			::fast_io::throw_posix_error();
		}
#endif
		auto status{scatter_size_to_status(static_cast<::std::size_t>(ret), pscatter, this_n)};
		position += status.position;
		if (status.position != this_n || status.position_in_scatter != 0)
		{
			return {position, status.position_in_scatter};
		}
		pscatter += this_n;
		n -= this_n;
	}
	return {position, 0};
}

inline ::fast_io::io_scatter_status_t posix_scatter_write_bytes_impl(int fd, ::fast_io::io_scatter_t const *pscatter,
																	 ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	// readv/writev reject iovcnt > IOV_MAX (1024 on Linux); loop over the scatters in chunks
	constexpr ::std::size_t iovec_max_number{
#if defined(__wasi__)
		// __wasi_fd_read/__wasi_fd_write are not readv(2); no iovcnt limit
		::std::numeric_limits<::std::size_t>::max()
#elif defined(IOV_MAX)
		IOV_MAX
#elif defined(__linux__)
		1024
#else
		::std::numeric_limits<::std::size_t>::max()
#endif
	};
	::std::size_t position{};
	while (n)
	{
		::std::size_t this_n{iovec_max_number < n ? iovec_max_number : n};
#if defined(__linux__) && defined(__NR_writev)
		auto ret{system_call<__NR_writev, ::std::ptrdiff_t>(fd, pscatter, this_n)};
		::fast_io::linux_system_call_throw_error(ret);
#elif defined(__wasi__)
		using iovec_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= __wasi_ciovec_t const *;
		::std::size_t ret;
		auto val{noexcept_call(::__wasi_fd_write, fd, reinterpret_cast<iovec_may_alias_const_ptr>(pscatter), this_n,
							   __builtin_addressof(ret))};
		if (val)
		{
			::fast_io::throw_posix_error(val);
		}
#else
		using iovec_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= struct iovec const *;

		auto ret{::fast_io::noexcept_call(::writev, fd, reinterpret_cast<iovec_may_alias_const_ptr>(pscatter),
										  static_cast<int>(this_n))};
		if (ret == -1)
		{
			::fast_io::throw_posix_error();
		}
#endif
		auto status{scatter_size_to_status(static_cast<::std::size_t>(ret), pscatter, this_n)};
		position += status.position;
		if (status.position != this_n || status.position_in_scatter != 0)
		{
			return {position, status.position_in_scatter};
		}
		pscatter += this_n;
		n -= this_n;
	}
	return {position, 0};
}

} // namespace details

template <::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_read_some_bytes_underflow_define(::fast_io::basic_posix_io_observer<char_type> piob,
										 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::posix_scatter_read_bytes_impl(piob.fd, pscatters, n);
}

template <::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_write_some_bytes_overflow_define(::fast_io::basic_posix_io_observer<char_type> piob,
										 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::posix_scatter_write_bytes_impl(piob.fd, pscatters, n);
}

} // namespace fast_io
