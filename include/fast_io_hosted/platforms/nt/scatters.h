#pragma once

namespace fast_io::win32::nt::details
{

template <typename T>
using nt_buffer_alloc_ptr = ::fast_io::details::buffer_alloc_arr_ptr<
	T,
	false,
	::fast_io::generic_allocator_adapter<
#if defined(__WINE__) && !defined(__CYGWIN__)
		::fast_io::native_thread_local_allocator
#else
		::fast_io::nt_rtlallocateheap_allocator
#endif
		,
		::fast_io::allocator_adapter_flags::throws_on_allocation_failure>>;


// The kernel only takes a per-call ByteOffset; it cannot advance an offset across the
// chunked syscalls this emulation issues, so the offset accumulation and the pre-write
// overflow check live here. Negative ByteOffset values (-1/-2 special positions) pass through.
template <::fast_io::nt_family family>
struct nt_scatter_write_pwrite_chunk_impl
{
	::std::int_least64_t *pbyteoffset{};
	inline ::std::byte const *operator()(void *__restrict handle, ::std::byte const *first,
										 ::std::size_t count) const FAST_IO_HERBCEPTIONS_THROWS
	{
		return ::fast_io::nt::details::nt_write_pwrite_some_thunk<family>(handle, first, count, pbyteoffset);
	}
};

// write-all variant: a chunk is only done when fully written; a syscall round making no
// progress means the write stopped -> STATUS_IO_DEVICE_ERROR.
template <::fast_io::nt_family family>
struct nt_scatter_write_all_pwrite_chunk_impl
{
	::std::int_least64_t *pbyteoffset{};
	inline void operator()(void *__restrict handle, ::std::byte const *first,
						   ::std::size_t count) const FAST_IO_HERBCEPTIONS_THROWS
	{
		::fast_io::nt::details::nt_write_pwrite_all_thunk<family>(handle, first, count, pbyteoffset);
	}
};

template <::fast_io::nt_family family>
inline ::fast_io::io_scatter_status_t nt_scatter_write_pwrite_some_bytes_overflow_define_impl(void *handle,
																							  ::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::std::int_least64_t *__restrict pbyteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::scatter_write_pwrite_some_bytes_common<
		::fast_io::win32::nt::details::nt_buffer_alloc_ptr<::std::byte>>(
		handle, pscatters, n, ::fast_io::nt::details::nt_scatter_write_pwrite_chunk_impl<family>{pbyteoffset});
}

template <::fast_io::nt_family family>
inline ::fast_io::io_scatter_status_t nt_scatter_write_some_bytes_overflow_define_impl(void *handle,
																					   ::fast_io::io_scatter_t const *pscatters, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::nt::details::nt_scatter_write_pwrite_some_bytes_overflow_define_impl<family>(handle, pscatters, n, nullptr);
}
template <::fast_io::nt_family family>
inline ::fast_io::io_scatter_status_t nt_scatter_pwrite_some_bytes_overflow_define_impl(void *handle,
																						::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::fast_io::intfpos_t byteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::int_least64_t offs{::fast_io::nt::details::nt_calculate_offset_impl(byteoffset)};
	return ::fast_io::nt::details::nt_scatter_write_pwrite_some_bytes_overflow_define_impl<family>(handle, pscatters, n, __builtin_addressof(offs));
}

template <::fast_io::nt_family family>
inline void nt_scatter_write_pwrite_all_bytes_overflow_define_impl(void *handle,
																   ::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::std::int_least64_t *__restrict pbyteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::scatter_write_pwrite_all_bytes_common<
		::fast_io::win32::nt::details::nt_buffer_alloc_ptr<::std::byte>>(
		handle, pscatters, n, ::fast_io::nt::details::nt_scatter_write_all_pwrite_chunk_impl<family>{pbyteoffset});
}

template <::fast_io::nt_family family>
inline void nt_scatter_write_all_bytes_overflow_define_impl(void *handle,
															::fast_io::io_scatter_t const *pscatters, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::nt::details::nt_scatter_write_pwrite_all_bytes_overflow_define_impl<family>(handle, pscatters, n, nullptr);
}

template <::fast_io::nt_family family>
inline void nt_scatter_pwrite_all_bytes_overflow_define_impl(void *handle,
															 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::fast_io::intfpos_t byteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::int_least64_t offs{::fast_io::nt::details::nt_calculate_offset_impl(byteoffset)};
	return ::fast_io::nt::details::nt_scatter_write_pwrite_all_bytes_overflow_define_impl<family>(handle, pscatters, n, __builtin_addressof(offs));
}

// read counterpart of nt_scatter_write_pwrite_chunk_impl: offset tracking is identical,
// and a short read means EOF, which the common loop reports as a partial status.
template <::fast_io::nt_family family>
struct nt_scatter_read_pread_chunk_impl
{
	::std::int_least64_t *pbyteoffset{};
	inline ::std::byte *operator()(void *__restrict handle, ::std::byte *first,
								   ::std::size_t count) const FAST_IO_HERBCEPTIONS_THROWS
	{
		return ::fast_io::nt::details::nt_read_pread_some_thunk<family>(handle, first, count, pbyteoffset);
	}
};

template <::fast_io::nt_family family>
inline ::fast_io::io_scatter_status_t nt_scatter_read_pread_some_bytes_underflow_define_impl(void *handle,
																							 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::std::int_least64_t *__restrict pbyteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::scatter_read_pread_some_bytes_common<
		::fast_io::win32::nt::details::nt_buffer_alloc_ptr<::std::byte>>(
		handle, pscatters, n, ::fast_io::nt::details::nt_scatter_read_pread_chunk_impl<family>{pbyteoffset});
}

template <::fast_io::nt_family family>
inline ::fast_io::io_scatter_status_t nt_scatter_read_some_bytes_underflow_define_impl(void *handle,
																					   ::fast_io::io_scatter_t const *pscatters, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::nt::details::nt_scatter_read_pread_some_bytes_underflow_define_impl<family>(handle, pscatters, n, nullptr);
}

template <::fast_io::nt_family family>
inline ::fast_io::io_scatter_status_t nt_scatter_pread_some_bytes_underflow_define_impl(void *handle,
																						::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::fast_io::intfpos_t byteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::int_least64_t offs{::fast_io::nt::details::nt_calculate_offset_impl(byteoffset)};
	return ::fast_io::nt::details::nt_scatter_read_pread_some_bytes_underflow_define_impl<family>(handle, pscatters, n, __builtin_addressof(offs));
}

// read-all variant: a chunk is only done when fully filled; EOF before that throws
// end_of_file inside nt_read_pread_all_thunk.
template <::fast_io::nt_family family>
struct nt_scatter_read_all_pread_chunk_impl
{
	::std::int_least64_t *pbyteoffset{};
	inline void operator()(void *__restrict handle, ::std::byte *first,
						   ::std::size_t count) const FAST_IO_HERBCEPTIONS_THROWS
	{
		::fast_io::nt::details::nt_read_pread_all_thunk<family>(handle, first, count, pbyteoffset);
	}
};

template <::fast_io::nt_family family>
inline void nt_scatter_read_pread_all_bytes_underflow_define_impl(void *handle,
																  ::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::std::int_least64_t *__restrict pbyteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::details::scatter_read_pread_all_bytes_common<
		::fast_io::win32::nt::details::nt_buffer_alloc_ptr<::std::byte>>(
		handle, pscatters, n, ::fast_io::nt::details::nt_scatter_read_all_pread_chunk_impl<family>{pbyteoffset});
}

template <::fast_io::nt_family family>
inline void nt_scatter_read_all_bytes_underflow_define_impl(void *handle,
															::fast_io::io_scatter_t const *pscatters, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::nt::details::nt_scatter_read_pread_all_bytes_underflow_define_impl<family>(handle, pscatters, n, nullptr);
}

template <::fast_io::nt_family family>
inline void nt_scatter_pread_all_bytes_underflow_define_impl(void *handle,
															 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::fast_io::intfpos_t byteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::int_least64_t offs{::fast_io::nt::details::nt_calculate_offset_impl(byteoffset)};
	::fast_io::nt::details::nt_scatter_read_pread_all_bytes_underflow_define_impl<family>(handle, pscatters, n, __builtin_addressof(offs));
}

} // namespace fast_io::win32::nt::details

namespace fast_io
{

template <::fast_io::nt_family family, ::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_write_some_bytes_overflow_define(::fast_io::basic_nt_family_io_observer<family, char_type> niob,
										 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::nt::details::nt_scatter_write_some_bytes_overflow_define_impl<family>(niob.handle, pscatters, n);
}

template <::fast_io::nt_family family, ::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_pwrite_some_bytes_overflow_define(::fast_io::basic_nt_family_io_observer<family, char_type> niob,
										  ::fast_io::io_scatter_t const *pscatters, ::std::size_t n,
										  ::fast_io::intfpos_t byteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::nt::details::nt_scatter_pwrite_some_bytes_overflow_define_impl<family>(niob.handle, pscatters, n, byteoffset);
}

template <::fast_io::nt_family family, ::std::integral char_type>
inline void
scatter_write_all_bytes_overflow_define(::fast_io::basic_nt_family_io_observer<family, char_type> niob,
										::fast_io::io_scatter_t const *pscatters, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::nt::details::nt_scatter_write_all_bytes_overflow_define_impl<family>(niob.handle, pscatters, n);
}

template <::fast_io::nt_family family, ::std::integral char_type>
inline void
scatter_pwrite_all_bytes_overflow_define(::fast_io::basic_nt_family_io_observer<family, char_type> niob,
										 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::fast_io::intfpos_t byteoffset)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::nt::details::nt_scatter_pwrite_all_bytes_overflow_define_impl<family>(niob.handle, pscatters, n, byteoffset);
}

template <::fast_io::nt_family family, ::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_read_some_bytes_underflow_define(::fast_io::basic_nt_family_io_observer<family, char_type> niob,
										 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::nt::details::nt_scatter_read_some_bytes_underflow_define_impl<family>(niob.handle, pscatters, n);
}

template <::fast_io::nt_family family, ::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_pread_some_bytes_underflow_define(::fast_io::basic_nt_family_io_observer<family, char_type> niob,
										  ::fast_io::io_scatter_t const *pscatters, ::std::size_t n,
										  ::fast_io::intfpos_t byteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::nt::details::nt_scatter_pread_some_bytes_underflow_define_impl<family>(niob.handle, pscatters, n, byteoffset);
}

template <::fast_io::nt_family family, ::std::integral char_type>
inline void
scatter_read_all_bytes_underflow_define(::fast_io::basic_nt_family_io_observer<family, char_type> niob,
										::fast_io::io_scatter_t const *pscatters, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::nt::details::nt_scatter_read_all_bytes_underflow_define_impl<family>(niob.handle, pscatters, n);
}

template <::fast_io::nt_family family, ::std::integral char_type>
inline void
scatter_pread_all_bytes_underflow_define(::fast_io::basic_nt_family_io_observer<family, char_type> niob,
										 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::fast_io::intfpos_t byteoffset)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::nt::details::nt_scatter_pread_all_bytes_underflow_define_impl<family>(niob.handle, pscatters, n, byteoffset);
}

} // namespace fast_io
