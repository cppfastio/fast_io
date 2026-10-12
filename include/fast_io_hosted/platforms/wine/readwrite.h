#pragma once

namespace fast_io
{

template <::fast_io::wine_family family, ::std::integral char_type>
inline ::std::byte const *write_some_bytes_overflow_define(::fast_io::basic_wine_family_io_observer<family, char_type> wiob,
														   ::std::byte const *first, ::std::size_t count) FAST_IO_HERBCEPTIONS_THROWS
{
	auto const done{::fast_io::wine::wine_unix_write(wiob.host_fd, first,
													 count)
						.total};
	return first + done;
}

template <::fast_io::wine_family family, ::std::integral char_type>
inline ::std::byte *read_some_bytes_underflow_define(::fast_io::basic_wine_family_io_observer<family, char_type> wiob,
													 ::std::byte *first, ::std::size_t count) FAST_IO_HERBCEPTIONS_THROWS
{
	auto const done{::fast_io::wine::wine_unix_read(wiob.host_fd, first,
													count)
						.total};
	return first + done;
}

template <::fast_io::wine_family family, ::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_write_some_bytes_overflow_define(::fast_io::basic_wine_family_io_observer<family, char_type> wiob,
										 ::fast_io::io_scatter_t const *pscatter,
										 ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	auto ret{::fast_io::wine::wine_unix_writev(
		wiob.host_fd, reinterpret_cast<::fast_io::details::wine_iovec_may_alias_const_ptr>(pscatter), n)};
	return {ret.baseindex, ret.index};
}

template <::fast_io::wine_family family, ::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_read_some_bytes_underflow_define(::fast_io::basic_wine_family_io_observer<family, char_type> wiob,
										 ::fast_io::io_scatter_t const *pscatter,
										 ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	auto ret{::fast_io::wine::wine_unix_readv(
		wiob.host_fd, reinterpret_cast<::fast_io::details::wine_iovec_may_alias_const_ptr>(pscatter), n)};
	return {ret.baseindex, ret.index};
}

template <::fast_io::wine_family family, ::std::integral char_type>
inline ::std::byte const *pwrite_some_bytes_overflow_define(::fast_io::basic_wine_family_io_observer<family, char_type> wiob,
															::std::byte const *first, ::std::size_t count,
															::fast_io::intfpos_t off) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::wine_unix::iovec_t iov{first, count};
	auto ret{::fast_io::wine::wine_unix_pwritev(wiob.host_fd, __builtin_addressof(iov), 1,
												static_cast<::fast_io::wine_unix::off_t>(off))};
	return first + (ret.baseindex ? count : ret.index);
}

template <::fast_io::wine_family family, ::std::integral char_type>
inline ::std::byte *pread_some_bytes_underflow_define(::fast_io::basic_wine_family_io_observer<family, char_type> wiob,
													  ::std::byte *first, ::std::size_t count,
													  ::fast_io::intfpos_t off) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::wine_unix::iovec_t iov{first, count};
	auto ret{::fast_io::wine::wine_unix_preadv(wiob.host_fd, __builtin_addressof(iov), 1,
											   static_cast<::fast_io::wine_unix::off_t>(off))};
	return first + (ret.baseindex ? count : ret.index);
}

template <::fast_io::wine_family family, ::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_pwrite_some_bytes_overflow_define(::fast_io::basic_wine_family_io_observer<family, char_type> wiob,
										  ::fast_io::io_scatter_t const *pscatter, ::std::size_t n,
										  ::fast_io::intfpos_t off) FAST_IO_HERBCEPTIONS_THROWS
{
	auto ret{::fast_io::wine::wine_unix_pwritev(
		wiob.host_fd, reinterpret_cast<::fast_io::details::wine_iovec_may_alias_const_ptr>(pscatter), n,
		static_cast<::fast_io::wine_unix::off_t>(off))};
	return {ret.baseindex, ret.index};
}

template <::fast_io::wine_family family, ::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_pread_some_bytes_underflow_define(::fast_io::basic_wine_family_io_observer<family, char_type> wiob,
										  ::fast_io::io_scatter_t const *pscatter, ::std::size_t n,
										  ::fast_io::intfpos_t off) FAST_IO_HERBCEPTIONS_THROWS
{
	auto ret{::fast_io::wine::wine_unix_preadv(
		wiob.host_fd, reinterpret_cast<::fast_io::details::wine_iovec_may_alias_const_ptr>(pscatter), n,
		static_cast<::fast_io::wine_unix::off_t>(off))};
	return {ret.baseindex, ret.index};
}

} // namespace fast_io
