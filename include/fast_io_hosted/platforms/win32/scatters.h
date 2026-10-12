#pragma once

namespace fast_io::win32::details
{

template <typename T>
using win32_buffer_alloc_ptr = ::fast_io::details::buffer_alloc_arr_ptr<
	T,
	false,
	::fast_io::generic_allocator_adapter<::fast_io::win32_heapalloc_allocator,
										 ::fast_io::allocator_adapter_flags::throws_on_allocation_failure>>;

// WriteFile only takes a per-call LPOVERLAPPED, so chunk offset tracking lives in the
// thunk; these functors just carry it through the common loop.
struct win32_scatter_write_pwrite_chunk_impl
{
	::std::int_least64_t *pbyteoffset{};
	inline ::std::byte const *operator()(void *__restrict handle, ::std::byte const *first,
										 ::std::size_t count) const FAST_IO_HERBCEPTIONS_THROWS
	{
		return ::fast_io::win32::details::win32_write_pwrite_some_thunk(handle, first, count, pbyteoffset);
	}
};

// write-all variant: a chunk is only done when fully written; a call making no
// progress means the write stopped -> ERROR_IO_DEVICE inside the all thunk.
struct win32_scatter_write_all_pwrite_chunk_impl
{
	::std::int_least64_t *pbyteoffset{};
	inline void operator()(void *__restrict handle, ::std::byte const *first,
						   ::std::size_t count) const FAST_IO_HERBCEPTIONS_THROWS
	{
		::fast_io::win32::details::win32_write_pwrite_all_thunk(handle, first, count, pbyteoffset);
	}
};

inline ::fast_io::io_scatter_status_t win32_scatter_write_pwrite_some_bytes_overflow_define_impl(void *handle,
																								 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::std::int_least64_t *__restrict pbyteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::scatter_write_pwrite_some_bytes_common<
		::fast_io::win32::details::win32_buffer_alloc_ptr<::std::byte>>(
		handle, pscatters, n, ::fast_io::win32::details::win32_scatter_write_pwrite_chunk_impl{pbyteoffset});
}

inline ::fast_io::io_scatter_status_t win32_scatter_write_some_bytes_overflow_define_impl(void *handle,
																						  ::fast_io::io_scatter_t const *pscatters, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::details::win32_scatter_write_pwrite_some_bytes_overflow_define_impl(handle, pscatters, n, nullptr);
}

inline ::fast_io::io_scatter_status_t win32_scatter_pwrite_some_bytes_overflow_define_impl(void *handle,
																						   ::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::fast_io::intfpos_t byteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::int_least64_t offs{::fast_io::win32::nt::details::nt_calculate_offset_impl(byteoffset)};
	return ::fast_io::win32::details::win32_scatter_write_pwrite_some_bytes_overflow_define_impl(handle, pscatters, n, __builtin_addressof(offs));
}

inline void win32_scatter_write_pwrite_all_bytes_overflow_define_impl(void *handle,
																	  ::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::std::int_least64_t *__restrict pbyteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::details::scatter_write_pwrite_all_bytes_common<
		::fast_io::win32::details::win32_buffer_alloc_ptr<::std::byte>>(
		handle, pscatters, n, ::fast_io::win32::details::win32_scatter_write_all_pwrite_chunk_impl{pbyteoffset});
}

inline void win32_scatter_write_all_bytes_overflow_define_impl(void *handle,
															   ::fast_io::io_scatter_t const *pscatters, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::details::win32_scatter_write_pwrite_all_bytes_overflow_define_impl(handle, pscatters, n, nullptr);
}

inline void win32_scatter_pwrite_all_bytes_overflow_define_impl(void *handle,
																::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::fast_io::intfpos_t byteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::int_least64_t offs{::fast_io::win32::nt::details::nt_calculate_offset_impl(byteoffset)};
	::fast_io::win32::details::win32_scatter_write_pwrite_all_bytes_overflow_define_impl(handle, pscatters, n, __builtin_addressof(offs));
}

// read counterparts: a short chunk read means EOF, which the some common loop reports
// as a partial status and the all thunk turns into ERROR_HANDLE_EOF.
struct win32_scatter_read_pread_chunk_impl
{
	::std::int_least64_t *pbyteoffset{};
	inline ::std::byte *operator()(void *__restrict handle, ::std::byte *first,
								   ::std::size_t count) const FAST_IO_HERBCEPTIONS_THROWS
	{
		return ::fast_io::win32::details::win32_read_pread_some_thunk(handle, first, count, pbyteoffset);
	}
};

struct win32_scatter_read_all_pread_chunk_impl
{
	::std::int_least64_t *pbyteoffset{};
	inline void operator()(void *__restrict handle, ::std::byte *first,
						   ::std::size_t count) const FAST_IO_HERBCEPTIONS_THROWS
	{
		::fast_io::win32::details::win32_read_pread_all_thunk(handle, first, count, pbyteoffset);
	}
};

inline ::fast_io::io_scatter_status_t win32_scatter_read_pread_some_bytes_underflow_define_impl(void *handle,
																								::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::std::int_least64_t *__restrict pbyteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::scatter_read_pread_some_bytes_common<
		::fast_io::win32::details::win32_buffer_alloc_ptr<::std::byte>>(
		handle, pscatters, n, ::fast_io::win32::details::win32_scatter_read_pread_chunk_impl{pbyteoffset});
}

inline ::fast_io::io_scatter_status_t win32_scatter_read_some_bytes_underflow_define_impl(void *handle,
																						  ::fast_io::io_scatter_t const *pscatters, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::details::win32_scatter_read_pread_some_bytes_underflow_define_impl(handle, pscatters, n, nullptr);
}

inline ::fast_io::io_scatter_status_t win32_scatter_pread_some_bytes_underflow_define_impl(void *handle,
																						   ::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::fast_io::intfpos_t byteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::int_least64_t offs{::fast_io::win32::nt::details::nt_calculate_offset_impl(byteoffset)};
	return ::fast_io::win32::details::win32_scatter_read_pread_some_bytes_underflow_define_impl(handle, pscatters, n, __builtin_addressof(offs));
}

inline void win32_scatter_read_pread_all_bytes_underflow_define_impl(void *handle,
																	 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::std::int_least64_t *__restrict pbyteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::details::scatter_read_pread_all_bytes_common<
		::fast_io::win32::details::win32_buffer_alloc_ptr<::std::byte>>(
		handle, pscatters, n, ::fast_io::win32::details::win32_scatter_read_all_pread_chunk_impl{pbyteoffset});
}

inline void win32_scatter_read_all_bytes_underflow_define_impl(void *handle,
															   ::fast_io::io_scatter_t const *pscatters, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::details::win32_scatter_read_pread_all_bytes_underflow_define_impl(handle, pscatters, n, nullptr);
}

inline void win32_scatter_pread_all_bytes_underflow_define_impl(void *handle,
																::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::fast_io::intfpos_t byteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::int_least64_t offs{::fast_io::win32::nt::details::nt_calculate_offset_impl(byteoffset)};
	::fast_io::win32::details::win32_scatter_read_pread_all_bytes_underflow_define_impl(handle, pscatters, n, __builtin_addressof(offs));
}

} // namespace fast_io::win32::details

namespace fast_io
{

template <::fast_io::win32_family family, ::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_write_some_bytes_overflow_define(::fast_io::basic_win32_family_io_observer<family, char_type> wiob,
										 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::details::win32_scatter_write_some_bytes_overflow_define_impl(wiob.handle, pscatters, n);
}

template <::fast_io::win32_family family, ::std::integral char_type>
inline void
scatter_write_all_bytes_overflow_define(::fast_io::basic_win32_family_io_observer<family, char_type> wiob,
										::fast_io::io_scatter_t const *pscatters, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::details::win32_scatter_write_all_bytes_overflow_define_impl(wiob.handle, pscatters, n);
}

template <::fast_io::win32_family family, ::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_read_some_bytes_underflow_define(::fast_io::basic_win32_family_io_observer<family, char_type> wiob,
										 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::details::win32_scatter_read_some_bytes_underflow_define_impl(wiob.handle, pscatters, n);
}

template <::fast_io::win32_family family, ::std::integral char_type>
inline void
scatter_read_all_bytes_underflow_define(::fast_io::basic_win32_family_io_observer<family, char_type> wiob,
										::fast_io::io_scatter_t const *pscatters, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::details::win32_scatter_read_all_bytes_underflow_define_impl(wiob.handle, pscatters, n);
}

// Windows 9x does not support atomic p-series functions. When using operation::p-series functions,
// they will be emulated through seek and non-p-series functions.
#if !defined(_WIN32_WINDOWS)
template <::fast_io::win32_family family, ::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_pwrite_some_bytes_overflow_define(::fast_io::basic_win32_family_io_observer<family, char_type> wiob,
										  ::fast_io::io_scatter_t const *pscatters, ::std::size_t n,
										  ::fast_io::intfpos_t byteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::details::win32_scatter_pwrite_some_bytes_overflow_define_impl(wiob.handle, pscatters, n, byteoffset);
}

template <::fast_io::win32_family family, ::std::integral char_type>
inline void
scatter_pwrite_all_bytes_overflow_define(::fast_io::basic_win32_family_io_observer<family, char_type> wiob,
										 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::fast_io::intfpos_t byteoffset)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::details::win32_scatter_pwrite_all_bytes_overflow_define_impl(wiob.handle, pscatters, n, byteoffset);
}

template <::fast_io::win32_family family, ::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_pread_some_bytes_underflow_define(::fast_io::basic_win32_family_io_observer<family, char_type> wiob,
										  ::fast_io::io_scatter_t const *pscatters, ::std::size_t n,
										  ::fast_io::intfpos_t byteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::details::win32_scatter_pread_some_bytes_underflow_define_impl(wiob.handle, pscatters, n, byteoffset);
}

template <::fast_io::win32_family family, ::std::integral char_type>
inline void
scatter_pread_all_bytes_underflow_define(::fast_io::basic_win32_family_io_observer<family, char_type> wiob,
										 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::fast_io::intfpos_t byteoffset)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::details::win32_scatter_pread_all_bytes_underflow_define_impl(wiob.handle, pscatters, n, byteoffset);
}
#endif

} // namespace fast_io
