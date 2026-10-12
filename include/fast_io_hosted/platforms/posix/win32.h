#pragma once

namespace fast_io
{

namespace details
{

inline ::std::byte *posix_pread_bytes_impl(int fd, ::std::byte *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	// Here, functions from the operations module must be selected because Windows 9x may not provide the p-series functions.
	return ::fast_io::operations::pread_some_bytes(::fast_io::win32_io_observer{::fast_io::details::my_get_osfile_handle(fd)}, first, count, off);
}

inline void posix_pread_all_bytes_impl(int fd, ::std::byte *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	// Here, functions from the operations module must be selected because Windows 9x may not provide the p-series functions.
	::fast_io::operations::pread_all_bytes(::fast_io::win32_io_observer{::fast_io::details::my_get_osfile_handle(fd)}, first, count, off);
}

inline ::std::byte const *posix_pwrite_bytes_impl(int fd, ::std::byte const *first, ::std::size_t count,
												  ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	// Here, functions from the operations module must be selected because Windows 9x may not provide the p-series functions.
	return ::fast_io::operations::pwrite_some_bytes(::fast_io::win32_io_observer{::fast_io::details::my_get_osfile_handle(fd)}, first, count, off);
}

inline void posix_pwrite_all_bytes_impl(int fd, ::std::byte const *first, ::std::size_t count,
										::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	// Here, functions from the operations module must be selected because Windows 9x may not provide the p-series functions.
	::fast_io::operations::pwrite_all_bytes(::fast_io::win32_io_observer{::fast_io::details::my_get_osfile_handle(fd)}, first, count, off);
}

// Scatter I/O emulation buffers allocate through the throwing flavour of the thread
// local allocator so allocation failure propagates as an herbception like any other
// I/O error instead of terminating.
template <typename T>
using posix_scatter_buffer_alloc_ptr = ::fast_io::details::buffer_alloc_arr_ptr<
	T,
	false,
	::fast_io::generic_allocator_adapter<::fast_io::native_thread_local_allocator,
										 ::fast_io::allocator_adapter_flags::throws_on_allocation_failure>>;

// Non-positional scatter I/O goes through _read/_write (posix_read/write_bytes_impl)
// rather than win32_io_observer so the CRT fd's text mode is honored.
struct posix_scatter_read_chunk_impl
{
	int fd;
	inline ::std::byte *operator()(void *, ::std::byte *first, ::std::size_t count) const FAST_IO_HERBCEPTIONS_THROWS
	{
		return posix_read_bytes_impl(fd, first, count);
	}
};

struct posix_scatter_read_all_chunk_impl
{
	int fd;
	inline void operator()(void *, ::std::byte *first, ::std::size_t count) const FAST_IO_HERBCEPTIONS_THROWS
	{
		auto const e{first + count};
		while (first != e)
		{
			auto got{posix_read_bytes_impl(fd, first, static_cast<::std::size_t>(e - first))};
			if (got == first) [[unlikely]]
			{
				::fast_io::throw_posix_error(EIO);
			}
			first = got;
		}
	}
};

struct posix_scatter_write_chunk_impl
{
	int fd;
	inline ::std::byte const *operator()(void *, ::std::byte const *first, ::std::size_t count) const
		FAST_IO_HERBCEPTIONS_THROWS
	{
		return posix_write_bytes_impl(fd, first, count);
	}
};

struct posix_scatter_write_all_chunk_impl
{
	int fd;
	inline void operator()(void *, ::std::byte const *first, ::std::size_t count) const FAST_IO_HERBCEPTIONS_THROWS
	{
		auto const e{first + count};
		while (first != e)
		{
			auto written{posix_write_bytes_impl(fd, first, static_cast<::std::size_t>(e - first))};
			if (written == first) [[unlikely]]
			{
				::fast_io::throw_posix_error(EIO);
			}
			first = written;
		}
	}
};

inline ::fast_io::io_scatter_status_t posix_scatter_read_bytes_impl(int fd, ::fast_io::io_scatter_t const *pscatters,
																	::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::scatter_read_pread_some_bytes_common<::fast_io::details::posix_scatter_buffer_alloc_ptr<::std::byte>>(
		nullptr, pscatters, n, posix_scatter_read_chunk_impl{fd});
}

inline void posix_scatter_read_all_bytes_impl(int fd, ::fast_io::io_scatter_t const *pscatters, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::details::scatter_read_pread_all_bytes_common<::fast_io::details::posix_scatter_buffer_alloc_ptr<::std::byte>>(
		nullptr, pscatters, n, posix_scatter_read_all_chunk_impl{fd});
}

inline ::fast_io::io_scatter_status_t posix_scatter_pread_bytes_impl(int fd, ::fast_io::io_scatter_t const *pscatters,
																	 ::std::size_t n, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::operations::scatter_pread_some_bytes(
		::fast_io::win32_io_observer{::fast_io::details::my_get_osfile_handle(fd)}, pscatters, n, off);
}

inline void posix_scatter_pread_all_bytes_impl(int fd, ::fast_io::io_scatter_t const *pscatters, ::std::size_t n,
											   ::fast_io::intfpos_t off) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::operations::scatter_pread_all_bytes(
		::fast_io::win32_io_observer{::fast_io::details::my_get_osfile_handle(fd)}, pscatters, n, off);
}

inline ::fast_io::io_scatter_status_t posix_scatter_write_bytes_impl(int fd, ::fast_io::io_scatter_t const *pscatters,
																	 ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::scatter_write_pwrite_some_bytes_common<::fast_io::details::posix_scatter_buffer_alloc_ptr<::std::byte>>(
		nullptr, pscatters, n, posix_scatter_write_chunk_impl{fd});
}

inline void posix_scatter_write_all_bytes_impl(int fd, ::fast_io::io_scatter_t const *pscatters, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::details::scatter_write_pwrite_all_bytes_common<::fast_io::details::posix_scatter_buffer_alloc_ptr<::std::byte>>(
		nullptr, pscatters, n, posix_scatter_write_all_chunk_impl{fd});
}

inline ::fast_io::io_scatter_status_t posix_scatter_pwrite_bytes_impl(int fd, ::fast_io::io_scatter_t const *pscatters,
																	  ::std::size_t n, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::operations::scatter_pwrite_some_bytes(
		::fast_io::win32_io_observer{::fast_io::details::my_get_osfile_handle(fd)}, pscatters, n, off);
}

inline void posix_scatter_pwrite_all_bytes_impl(int fd, ::fast_io::io_scatter_t const *pscatters, ::std::size_t n,
												::fast_io::intfpos_t off) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::operations::scatter_pwrite_all_bytes(
		::fast_io::win32_io_observer{::fast_io::details::my_get_osfile_handle(fd)}, pscatters, n, off);
}

} // namespace details

template <::std::integral char_type>
inline ::std::byte *pread_some_bytes_underflow_define(::fast_io::basic_posix_io_observer<char_type> piob,
													  ::std::byte *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::posix_pread_bytes_impl(piob.fd, first, count, off);
}

template <::std::integral char_type>
inline void pread_all_bytes_underflow_define(::fast_io::basic_posix_io_observer<char_type> piob, ::std::byte *first, ::std::size_t count, ::fast_io::intfpos_t off) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::details::posix_pread_all_bytes_impl(piob.fd, first, count, off);
}

template <::std::integral char_type>
inline ::std::byte const *pwrite_some_bytes_overflow_define(::fast_io::basic_posix_io_observer<char_type> piob,
															::std::byte const *first, ::std::size_t count,
															::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::posix_pwrite_bytes_impl(piob.fd, first, count, off);
}

template <::std::integral char_type>
inline void pwrite_all_bytes_overflow_define(::fast_io::basic_posix_io_observer<char_type> piob,
											 ::std::byte const *first, ::std::size_t count,
											 ::fast_io::intfpos_t off) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::details::posix_pwrite_all_bytes_impl(piob.fd, first, count, off);
}

template <::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_read_some_bytes_underflow_define(::fast_io::basic_posix_io_observer<char_type> piob,
										 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::posix_scatter_read_bytes_impl(piob.fd, pscatters, n);
}

template <::std::integral char_type>
inline void scatter_read_all_bytes_underflow_define(::fast_io::basic_posix_io_observer<char_type> piob,
													::fast_io::io_scatter_t const *pscatters, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::details::posix_scatter_read_all_bytes_impl(piob.fd, pscatters, n);
}

template <::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_pread_some_bytes_underflow_define(::fast_io::basic_posix_io_observer<char_type> piob,
										  ::fast_io::io_scatter_t const *pscatters, ::std::size_t n,
										  ::fast_io::intfpos_t off) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::posix_scatter_pread_bytes_impl(piob.fd, pscatters, n, off);
}

template <::std::integral char_type>
inline void scatter_pread_all_bytes_underflow_define(::fast_io::basic_posix_io_observer<char_type> piob,
													 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n,
													 ::fast_io::intfpos_t off) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::details::posix_scatter_pread_all_bytes_impl(piob.fd, pscatters, n, off);
}

template <::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_write_some_bytes_overflow_define(::fast_io::basic_posix_io_observer<char_type> piob,
										 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::posix_scatter_write_bytes_impl(piob.fd, pscatters, n);
}

template <::std::integral char_type>
inline void scatter_write_all_bytes_overflow_define(::fast_io::basic_posix_io_observer<char_type> piob,
													::fast_io::io_scatter_t const *pscatters, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::details::posix_scatter_write_all_bytes_impl(piob.fd, pscatters, n);
}

template <::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_pwrite_some_bytes_overflow_define(::fast_io::basic_posix_io_observer<char_type> piob,
										  ::fast_io::io_scatter_t const *pscatters, ::std::size_t n,
										  ::fast_io::intfpos_t off) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::posix_scatter_pwrite_bytes_impl(piob.fd, pscatters, n, off);
}

template <::std::integral char_type>
inline void scatter_pwrite_all_bytes_overflow_define(::fast_io::basic_posix_io_observer<char_type> piob,
													 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n,
													 ::fast_io::intfpos_t off) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::details::posix_scatter_pwrite_all_bytes_impl(piob.fd, pscatters, n, off);
}

} // namespace fast_io
