#pragma once

namespace fast_io
{

namespace details
{

inline constexpr ::std::size_t posix_iovec_max_number{
// readv/writev reject iovcnt > IOV_MAX (1024 on Linux); loop over the scatters in chunks
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

inline ::fast_io::io_scatter_status_t posix_scatter_read_bytes_impl(int fd, ::fast_io::io_scatter_t const *pscatter,
																	::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::size_t this_n{::fast_io::details::posix_iovec_max_number < n ? ::fast_io::details::posix_iovec_max_number : n};
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
	return ::fast_io::scatter_size_to_status(static_cast<::std::size_t>(ret), pscatter, this_n);
}

inline ::std::size_t posix_scatter_write_bytes_n_impl(int fd, ::fast_io::io_scatter_t const *pscatter,
													  ::std::size_t this_n) FAST_IO_HERBCEPTIONS_THROWS
{
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
	return static_cast<::std::size_t>(ret);
}

inline ::fast_io::io_scatter_status_t posix_scatter_write_bytes_impl(int fd, ::fast_io::io_scatter_t const *pscatter,
																	 ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::size_t this_n{::fast_io::details::posix_iovec_max_number < n ? ::fast_io::details::posix_iovec_max_number : n};
	return ::fast_io::scatter_size_to_status(::fast_io::details::posix_scatter_write_bytes_n_impl(fd, pscatter, this_n), pscatter, this_n);
}

#if ((!defined(_WIN32) && !defined(__NEWLIB__)) || defined(__WINE__) || defined(__CYGWIN__)) && !defined(__wasi__) && defined(MSG_NOSIGNAL)
inline ::fast_io::io_scatter_status_t posix_socket_scatter_write_bytes_impl(int fd, ::fast_io::io_scatter_t const *pscatter,
																			::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::size_t retsz FAST_IO_INDETERMINATE;
	::std::size_t this_n{::fast_io::details::posix_iovec_max_number < n ? ::fast_io::details::posix_iovec_max_number : n};
	using iovec_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= struct iovec *;
	struct msghdr msg{};
	msg.msg_iov = reinterpret_cast<iovec_may_alias_const_ptr>(const_cast<::fast_io::io_scatter_t *>(pscatter));
	msg.msg_iovlen = this_n;
#if defined(__linux__) && defined(__NR_sendmsg)
	auto ret{system_call<__NR_sendmsg, ::std::ptrdiff_t>(fd, ::std::addressof(msg), MSG_NOSIGNAL)};
	if (::fast_io::linux_system_call_fails(ret)) [[unlikely]]
	{
		::fast_io::linux_system_call_throw_error(ret);
	}
#else
	auto ret{::fast_io::noexcept_call(::sendmsg, fd, ::std::addressof(msg),
									  static_cast<int>(this_n))};
	if (ret == -1) [[unlikely]]
	{
		::fast_io::throw_posix_error();
	}
#endif
	retsz = static_cast<::std::size_t>(ret);
	return ::fast_io::scatter_size_to_status(retsz, pscatter, this_n);
}
#endif
} // namespace details

template <::fast_io::posix_family fam, ::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_read_some_bytes_underflow_define(::fast_io::basic_posix_family_io_observer<fam, char_type> piob,
										 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::posix_scatter_read_bytes_impl(piob.fd, pscatters, n);
}

template <::fast_io::posix_family fam, ::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_write_some_bytes_overflow_define(::fast_io::basic_posix_family_io_observer<fam, char_type> piob,
										 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
#if ((!defined(_WIN32) && !defined(__NEWLIB__)) || defined(__WINE__) || defined(__CYGWIN__)) && !defined(__wasi__) && defined(MSG_NOSIGNAL)
	if constexpr (fam == ::fast_io::posix_family::sock)
	{
		return ::fast_io::details::posix_socket_scatter_write_bytes_impl(piob.fd, pscatters, n);
	}
	else
#endif
	{
		return ::fast_io::details::posix_scatter_write_bytes_impl(piob.fd, pscatters, n);
	}
}

} // namespace fast_io
