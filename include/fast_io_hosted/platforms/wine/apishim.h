#pragma once
/*
non-herbceptions shim for the wine layer — selected by api.h when
__HERBCEPTIONS__ is absent. implements the same fast_io::wine::wine_unix_*
surface apiherb.h declares as return_failure imports, on top of the dll's
*_returns_status api: status -> throw_wine_errc. call sites spell
wine::wine_unix_* once for both modes.
*/

namespace fast_io::wine
{

[[noreturn]] inline void throw_wine_errc(::std::uint_least32_t ec) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_wine_errc_with_value(ec);
}

inline ::std::ptrdiff_t wine_unix_host_fd_to_nt_handle(::fast_io::wine_host_fd_t host_fd) FAST_IO_HERBCEPTIONS_THROWS
{
	auto const ret{wine_unix_host_fd_to_nt_handle_returns_status(host_fd)};
	if (ret.status != 0)
	{
		throw_wine_errc(static_cast<::std::uint_least32_t>(ret.status));
	}
	return ret.handle;
}

inline ::fast_io::wine_host_fd_t wine_unix_nt_handle_to_host_fd_ref(::std::ptrdiff_t handle) FAST_IO_HERBCEPTIONS_THROWS
{
	auto const ret{wine_unix_nt_handle_to_host_fd_ref_returns_status(handle)};
	if (ret.status != 0)
	{
		throw_wine_errc(static_cast<::std::uint_least32_t>(ret.status));
	}
	return ret.host_fd;
}

inline ::fast_io::wine_host_fd_t wine_unix_openat(::fast_io::wine_host_fd_t host_dirfd, char const *filename,
												  ::std::size_t filenamelen, ::fast_io::wine_unix::flags_t flags,
												  ::fast_io::wine_unix::mode_t mode) FAST_IO_HERBCEPTIONS_THROWS
{
	auto const ret{wine_unix_openat_returns_status(host_dirfd, filename, filenamelen,
												   flags, mode)};
	if (ret.status != 0)
	{
		throw_wine_errc(static_cast<::std::uint_least32_t>(ret.status));
	}
	return ret.host_fd;
}

inline ::fast_io::wine_host_fd_t wine_unix_open(char const *filename,
												::std::size_t filenamelen, ::fast_io::wine_unix::flags_t flags,
												::fast_io::wine_unix::mode_t mode) FAST_IO_HERBCEPTIONS_THROWS
{
	auto const ret{wine_unix_open_returns_status(filename, filenamelen, flags, mode)};
	if (ret.status != 0)
	{
		throw_wine_errc(static_cast<::std::uint_least32_t>(ret.status));
	}
	return ret.host_fd;
}

inline ::fast_io::wine_unix::rwv_result_t wine_unix_writev(::fast_io::wine_host_fd_t host_fd,
														   ::fast_io::wine_unix::iovec_t const *iovs,
														   ::std::size_t iovsize) FAST_IO_HERBCEPTIONS_THROWS
{
	auto const ret{wine_unix_writev_returns_status(host_fd, iovs, iovsize)};
	if (ret.status != 0)
	{
		throw_wine_errc(static_cast<::std::uint_least32_t>(ret.status));
	}
	return {ret.baseindex, ret.index};
}

inline ::fast_io::wine_unix::rwv_result_t wine_unix_readv(::fast_io::wine_host_fd_t host_fd,
														  ::fast_io::wine_unix::iovec_t const *iovs,
														  ::std::size_t iovsize) FAST_IO_HERBCEPTIONS_THROWS
{
	auto const ret{wine_unix_readv_returns_status(host_fd, iovs, iovsize)};
	if (ret.status != 0)
	{
		throw_wine_errc(static_cast<::std::uint_least32_t>(ret.status));
	}
	return {ret.baseindex, ret.index};
}

inline ::fast_io::wine_unix::rwv_result_t wine_unix_pwritev(::fast_io::wine_host_fd_t host_fd,
															::fast_io::wine_unix::iovec_t const *iovs,
															::std::size_t iovsize,
															::fast_io::wine_unix::off_t offset) FAST_IO_HERBCEPTIONS_THROWS
{
	auto const ret{wine_unix_pwritev_returns_status(host_fd, iovs, iovsize, offset)};
	if (ret.status != 0)
	{
		throw_wine_errc(static_cast<::std::uint_least32_t>(ret.status));
	}
	return {ret.baseindex, ret.index};
}

inline ::fast_io::wine_unix::rwv_result_t wine_unix_preadv(::fast_io::wine_host_fd_t host_fd,
														   ::fast_io::wine_unix::iovec_t const *iovs,
														   ::std::size_t iovsize,
														   ::fast_io::wine_unix::off_t offset) FAST_IO_HERBCEPTIONS_THROWS
{
	auto const ret{wine_unix_preadv_returns_status(host_fd, iovs, iovsize, offset)};
	if (ret.status != 0)
	{
		throw_wine_errc(static_cast<::std::uint_least32_t>(ret.status));
	}
	return {ret.baseindex, ret.index};
}

inline ::fast_io::wine_unix::rw_result_t wine_unix_write(::fast_io::wine_host_fd_t host_fd, void const *buf,
														 ::std::size_t len) FAST_IO_HERBCEPTIONS_THROWS
{
	auto const ret{wine_unix_write_returns_status(host_fd, buf, len)};
	if (ret.status != 0)
	{
		throw_wine_errc(static_cast<::std::uint_least32_t>(ret.status));
	}
	return {ret.total};
}

inline ::fast_io::wine_unix::rw_result_t wine_unix_read(::fast_io::wine_host_fd_t host_fd, void *buf,
														::std::size_t len) FAST_IO_HERBCEPTIONS_THROWS
{
	auto const ret{wine_unix_read_returns_status(host_fd, buf, len)};
	if (ret.status != 0)
	{
		throw_wine_errc(static_cast<::std::uint_least32_t>(ret.status));
	}
	return {ret.total};
}

} // namespace fast_io::wine
