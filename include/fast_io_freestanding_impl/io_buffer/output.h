#pragma once

namespace fast_io
{

namespace details::io_buffer
{

template <::std::integral char_type, typename allocator_type, ::std::size_t buffersize>
inline constexpr void write_nullptr_case(::fast_io::details::io_buffer::iobuffer_alloc_handle_t<allocator_type, char_type> allochdl,
										 basic_io_buffer_pointers<char_type> &__restrict pointers,
										 char_type const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::typed_generic_allocator_adapter<allocator_type, char_type>::throws_on_allocation_failure)
{
	char_type *buffer_begin = ::fast_io::details::io_buffer::iobuffer_allocate<char_type, allocator_type>(allochdl, buffersize);
	char_type *buffer_curr = non_overlapped_copy_n(first, count, buffer_begin);
	pointers.buffer_begin = buffer_begin;
	pointers.buffer_curr = buffer_curr;
	pointers.buffer_end = buffer_begin + buffersize;
}

template <::std::integral char_type, typename optstmtype>
inline constexpr char_type const *write_some_typical_case(optstmtype optstm,
														  basic_io_buffer_pointers<char_type> &__restrict pointers,
														  char_type const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<optstmtype>)
{
	if constexpr (::fast_io::operations::decay::defines::has_any_of_write_or_seek_pwrite_bytes_operations<optstmtype>)
	{
		io_scatter_t const scatters[2]{
			{pointers.buffer_begin,
			 static_cast<::std::size_t>(reinterpret_cast<::std::byte const *>(pointers.buffer_curr) -
										reinterpret_cast<::std::byte const *>(pointers.buffer_begin))},
			{first, count * sizeof(char_type)}};
		auto [position, scpos]{::fast_io::operations::decay::scatter_write_some_bytes_decay(optstm, scatters, 2)};
		if (position == 2)
		{
			pointers.buffer_curr = pointers.buffer_begin;
			return first + count;
		}
		else if (position == 1)
		{
			pointers.buffer_curr = pointers.buffer_begin;
			return first + scpos / sizeof(char_type);
		}
		else
		{
			scpos /= sizeof(char_type);
			pointers.buffer_curr =
				::fast_io::freestanding::copy_n(pointers.buffer_curr - scpos, scpos, pointers.buffer_begin);
			return first;
		}
	}
	else
	{
		basic_io_scatter_t<char_type> const scatters[2]{
			{pointers.buffer_begin, static_cast<::std::size_t>(pointers.buffer_curr - pointers.buffer_begin)},
			{first, count}};
		auto [position, scpos]{::fast_io::operations::decay::scatter_write_some_decay(optstm, scatters, 2)};
		if (position == 2)
		{
			pointers.buffer_curr = pointers.buffer_begin;
			return first + count;
		}
		else if (position == 1)
		{
			pointers.buffer_curr = pointers.buffer_begin;
			return first + scpos;
		}
		else
		{
			pointers.buffer_curr =
				::fast_io::freestanding::copy_n(pointers.buffer_curr - scpos, scpos, pointers.buffer_begin);
			return first;
		}
	}
}

template <::std::integral char_type, typename optstmtype>
inline constexpr void write_all_typical_case(optstmtype optstm,
											 basic_io_buffer_pointers<char_type> &__restrict pointers,
											 char_type const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<optstmtype>)
{
	if constexpr (::fast_io::operations::decay::defines::has_any_of_write_bytes_operations<optstmtype>)
	{
		io_scatter_t const scatters[2]{
			{pointers.buffer_begin,
			 static_cast<::std::size_t>(reinterpret_cast<::std::byte const *>(pointers.buffer_curr) -
										reinterpret_cast<::std::byte const *>(pointers.buffer_begin))},
			{first, count * sizeof(char_type)}};
		::fast_io::operations::decay::scatter_write_all_bytes_decay(optstm, scatters, 2);
	}
	else
	{
		basic_io_scatter_t<char_type> const scatters[2]{
			{pointers.buffer_begin, static_cast<::std::size_t>(pointers.buffer_curr - pointers.buffer_begin)},
			{first, count}};
		::fast_io::operations::decay::scatter_write_all_decay(optstm, scatters, 2);
	}
	pointers.buffer_curr = pointers.buffer_begin;
}

template <::std::integral char_type, typename allocator_type, ::std::size_t buffer_size, typename optstmtype>
inline constexpr char_type const *write_some_overflow_impl(::fast_io::details::io_buffer::iobuffer_alloc_handle_t<allocator_type, char_type> allochdl,
														   optstmtype optstm,
														   basic_io_buffer_pointers<char_type> &pointers,
														   char_type const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<optstmtype> ||
								   ::fast_io::typed_generic_allocator_adapter<allocator_type, char_type>::throws_on_allocation_failure)
{
	::std::size_t const diff{count};
	if (pointers.buffer_begin == nullptr)
	{
		if (diff < buffer_size)
		{
			write_nullptr_case<char_type, allocator_type, buffer_size>(allochdl, pointers, first, count);
			return first + count;
		}
		else
		{
			if constexpr (::fast_io::operations::decay::defines::
							  has_any_of_write_or_seek_pwrite_bytes_operations<optstmtype>)
			{
				::std::byte const *firstb{reinterpret_cast<::std::byte const *>(first)};
				return first +
					   (::fast_io::operations::decay::write_some_bytes_decay(
							optstm, firstb, count * sizeof(char_type)) -
						firstb) /
						   sizeof(char_type);
			}
			else
			{
				return ::fast_io::operations::decay::write_some_decay(optstm, first, count);
			}
		}
	}
	return write_some_typical_case<char_type>(optstm, pointers, first, count);
}

template <::std::integral char_type, typename optstmtype>
inline constexpr void write_all_nullptr_case(optstmtype optstm, basic_io_buffer_pointers<char_type> &__restrict pointers,
											 char_type const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<optstmtype>)
{
	if constexpr (::fast_io::operations::decay::defines::has_any_of_write_bytes_operations<optstmtype>)
	{
		io_scatter_t const scatters[2]{
			{reinterpret_cast<::std::byte const *>(pointers.buffer_begin ? pointers.buffer_begin : first),
			 static_cast<::std::size_t>(pointers.buffer_begin
											? (reinterpret_cast<::std::byte const *>(pointers.buffer_curr) -
											   reinterpret_cast<::std::byte const *>(pointers.buffer_begin))
											: 0u)},
			{reinterpret_cast<::std::byte const *>(first), count * sizeof(char_type)}};
		::fast_io::operations::decay::scatter_write_all_bytes_decay(optstm, scatters, 2);
	}
	else
	{
		basic_io_scatter_t<char_type> const scatters[2]{
			{(pointers.buffer_begin ? pointers.buffer_begin : first),
			 static_cast<::std::size_t>(pointers.buffer_begin ? (pointers.buffer_curr - pointers.buffer_begin) : 0u)},
			{first, count}};
		::fast_io::operations::decay::scatter_write_all_decay(optstm, scatters, 2);
	}
	pointers.buffer_curr = pointers.buffer_begin;
}

template <::std::integral char_type, typename allocator_type, ::std::size_t buffer_size, typename optstmtype>
inline constexpr void write_all_overflow_impl(::fast_io::details::io_buffer::iobuffer_alloc_handle_t<allocator_type, char_type> allochdl,
											  optstmtype optstm, basic_io_buffer_pointers<char_type> &pointers,
											  char_type const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<optstmtype> ||
								   ::fast_io::typed_generic_allocator_adapter<allocator_type, char_type>::throws_on_allocation_failure)
{
	::std::size_t const diff{count};
	if (pointers.buffer_begin == nullptr)
	{
		if (diff < buffer_size)
		{
			write_nullptr_case<char_type, allocator_type, buffer_size>(allochdl, pointers, first, count);
		}
		else
		{
			if constexpr (::fast_io::operations::decay::defines::
							  has_any_of_write_or_seek_pwrite_bytes_operations<optstmtype>)
			{
				::fast_io::io_scatter_t const scatter{
					first, count * sizeof(char_type)};
				::fast_io::operations::decay::scatter_write_all_bytes_decay(optstm,
																			__builtin_addressof(scatter), 1zu);
			}
			else
			{
				::fast_io::operations::decay::write_all_decay(optstm, first, count);
			}
		}
		return;
	}
	write_all_typical_case<char_type>(optstm, pointers, first, count);
}

template <::std::integral char_type, typename optstmtype>
inline constexpr void output_stream_buffer_flush_impl(optstmtype optstm, basic_io_buffer_pointers<char_type> &pointers)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<optstmtype>)
{
	if (pointers.buffer_begin == pointers.buffer_curr)
	{
		return;
	}
	if constexpr (::fast_io::operations::decay::defines::has_any_of_write_or_seek_pwrite_bytes_operations<optstmtype>)
	{
		::fast_io::io_scatter_t const scatter{
			pointers.buffer_begin,
			static_cast<::std::size_t>(pointers.buffer_curr - pointers.buffer_begin) * sizeof(char_type)};
		::fast_io::operations::decay::scatter_write_all_bytes_decay(optstm, __builtin_addressof(scatter), 1zu);
	}
	else
	{
		::fast_io::operations::decay::write_all_decay(optstm, pointers.buffer_begin,
													  static_cast<::std::size_t>(pointers.buffer_curr - pointers.buffer_begin));
	}
	pointers.buffer_curr = pointers.buffer_begin;
}

template <::std::integral char_type, typename allocator_type, ::std::size_t buffer_size, typename optstmtype>
inline constexpr void obuffer_minimum_size_flush_prepare_impl(::fast_io::details::io_buffer::iobuffer_alloc_handle_t<allocator_type, char_type> allochdl,
															  optstmtype optstm,
															  basic_io_buffer_pointers<char_type> &pointers)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<optstmtype> ||
								   ::fast_io::typed_generic_allocator_adapter<allocator_type, char_type>::throws_on_allocation_failure)
{
	if (pointers.buffer_begin == pointers.buffer_curr)
	{
		if (pointers.buffer_begin == nullptr)
		{
			pointers.buffer_end =
				((pointers.buffer_curr = pointers.buffer_begin =
					  ::fast_io::details::io_buffer::iobuffer_allocate<char_type, allocator_type>(allochdl, buffer_size)) +
				 buffer_size);
		}
	}
	else
	{
		if constexpr (::fast_io::operations::decay::defines::
						  has_any_of_write_or_seek_pwrite_bytes_operations<optstmtype>)
		{
			::fast_io::io_scatter_t const scatter{
				pointers.buffer_begin,
				static_cast<::std::size_t>(pointers.buffer_curr - pointers.buffer_begin) * sizeof(char_type)};
			::fast_io::operations::decay::scatter_write_all_bytes_decay(optstm, __builtin_addressof(scatter), 1zu);
		}
		else
		{
			::fast_io::operations::decay::write_all_decay(optstm, pointers.buffer_begin,
														  static_cast<::std::size_t>(pointers.buffer_curr - pointers.buffer_begin));
		}
		pointers.buffer_curr = pointers.buffer_begin;
	}
}

} // namespace details::io_buffer

template <typename io_buffer_type>
inline constexpr typename io_buffer_type::output_char_type const *
write_some_overflow_define(basic_io_buffer_ref<io_buffer_type> iobref,
						   typename io_buffer_type::output_char_type const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<decltype(::fast_io::operations::output_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))>)
{
	return ::fast_io::details::io_buffer::write_some_overflow_impl<typename io_buffer_type::output_char_type,
																   typename io_buffer_type::traits_type::allocator_type,
																   io_buffer_type::traits_type::output_buffer_size>(
		iobref.iobptr->allocator_handle, ::fast_io::operations::output_stream_ref(iobref.iobptr->handle),
		iobref.iobptr->output_buffer, first, count);
}

template <typename io_buffer_type>
inline constexpr void write_all_overflow_define(basic_io_buffer_ref<io_buffer_type> iobref,
												typename io_buffer_type::output_char_type const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<decltype(::fast_io::operations::output_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))>)
{
	return ::fast_io::details::io_buffer::write_all_overflow_impl<typename io_buffer_type::output_char_type,
																  typename io_buffer_type::traits_type::allocator_type,
																  io_buffer_type::traits_type::output_buffer_size>(
		iobref.iobptr->allocator_handle, ::fast_io::operations::output_stream_ref(iobref.iobptr->handle),
		iobref.iobptr->output_buffer, first, count);
}


template <typename io_buffer_type>
	requires(::fast_io::operations::decay::defines::pwritable<
			 decltype(::fast_io::operations::output_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))>)
inline constexpr typename io_buffer_type::output_char_type const *
pwrite_some_overflow_define(basic_io_buffer_ref<io_buffer_type> iobref,
							typename io_buffer_type::output_char_type const *first, ::std::size_t count,
							::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<decltype(::fast_io::operations::output_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))>)
{
	return ::fast_io::operations::decay::pwrite_some_decay(::fast_io::operations::output_stream_ref(iobref.iobptr->handle), first, count, off);
}

template <typename io_buffer_type>
	requires(::fast_io::operations::decay::defines::pwritable<
			 decltype(::fast_io::operations::output_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))>)
inline constexpr void pwrite_all_overflow_define(basic_io_buffer_ref<io_buffer_type> iobref,
												 typename io_buffer_type::output_char_type const *first, ::std::size_t count,
												 ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<decltype(::fast_io::operations::output_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))>)
{
	::fast_io::operations::decay::pwrite_all_decay(::fast_io::operations::output_stream_ref(iobref.iobptr->handle), first, count, off);
}

template <typename io_buffer_type>
	requires(::fast_io::operations::decay::defines::pwritable<
			 decltype(::fast_io::operations::output_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))>)
inline constexpr ::fast_io::io_scatter_status_t
scatter_pwrite_some_overflow_define(basic_io_buffer_ref<io_buffer_type> iobref,
									::fast_io::basic_io_scatter_t<typename io_buffer_type::output_char_type> pscatters,
									::std::size_t n, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<decltype(::fast_io::operations::output_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))>)
{
	return ::fast_io::operations::decay::scatter_pwrite_some_decay(::fast_io::operations::output_stream_ref(iobref.iobptr->handle), pscatters, n, off);
}

template <typename io_buffer_type>
	requires(::fast_io::operations::decay::defines::pwritable<
			 decltype(::fast_io::operations::output_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))>)
inline constexpr void scatter_pwrite_all_overflow_define(basic_io_buffer_ref<io_buffer_type> iobref,
														 ::fast_io::basic_io_scatter_t<typename io_buffer_type::output_char_type> pscatters,
														 ::std::size_t n, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<decltype(::fast_io::operations::output_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))>)
{
	::fast_io::operations::decay::scatter_pwrite_all_decay(::fast_io::operations::output_stream_ref(iobref.iobptr->handle), pscatters, n, off);
}

template <typename io_buffer_type>
inline constexpr void output_stream_buffer_flush_define(basic_io_buffer_ref<io_buffer_type> iobref)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<decltype(::fast_io::operations::output_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))>)
{
	::fast_io::details::io_buffer::output_stream_buffer_flush_impl<typename io_buffer_type::output_char_type>(
		::fast_io::operations::output_stream_ref(iobref.iobptr->handle), iobref.iobptr->output_buffer);
}

/*
 * output_stream_buffer_detach_define(iobref): hand the pending output
 * buffer allocation to the returned RAII guard so an async writer can
 * send those bytes zero-copy while the stream keeps buffering into a
 * lazily-allocated fresh buffer. Any buffered stream can participate in
 * zero-copy async print by providing this define; the guard must expose
 * pending_bytes() -> {first, count}.
 */
template <typename io_buffer_type>
	requires((io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::out) ==
			 ::fast_io::buffer_mode::out)
inline constexpr auto output_stream_buffer_detach_define(basic_io_buffer_ref<io_buffer_type> iobref) noexcept
{
	return iobref.iobptr->detach_output_buffer();
}

template <typename io_buffer_type>
inline constexpr auto obuffer_begin(basic_io_buffer_ref<io_buffer_type> iobref) noexcept
{
	return iobref.iobptr->output_buffer.buffer_begin;
}

template <typename io_buffer_type>
inline constexpr auto obuffer_curr(basic_io_buffer_ref<io_buffer_type> iobref) noexcept
{
	return iobref.iobptr->output_buffer.buffer_curr;
}

template <typename io_buffer_type>
inline constexpr auto obuffer_end(basic_io_buffer_ref<io_buffer_type> iobref) noexcept
{
	return iobref.iobptr->output_buffer.buffer_end;
}

template <typename io_buffer_type>
inline constexpr void obuffer_set_curr(basic_io_buffer_ref<io_buffer_type> iobref,
									   typename basic_io_buffer_ref<io_buffer_type>::output_char_type *ptr) noexcept
{
	iobref.iobptr->output_buffer.buffer_curr = ptr;
}

template <::std::integral char_type, typename io_buffer_type>
	requires(::std::same_as<char_type, typename basic_io_buffer_ref<io_buffer_type>::output_char_type>)
inline constexpr ::std::size_t
	obuffer_minimum_size_define(::fast_io::io_reserve_type_t<char_type, basic_io_buffer_ref<io_buffer_type>>)
{
	return io_buffer_type::traits_type::output_buffer_size;
}

template <typename io_buffer_type>
inline constexpr void obuffer_minimum_size_flush_prepare_define(basic_io_buffer_ref<io_buffer_type> iobref)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::output_stream_operations_nothrow<decltype(::fast_io::operations::output_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))>)
{
	::fast_io::details::io_buffer::obuffer_minimum_size_flush_prepare_impl<
		typename io_buffer_type::output_char_type, typename io_buffer_type::traits_type::allocator_type,
		io_buffer_type::traits_type::output_buffer_size>(
		iobref.iobptr->allocator_handle, ::fast_io::operations::output_stream_ref(iobref.iobptr->handle),
		iobref.iobptr->output_buffer);
}

template <typename io_buffer_type>
	requires((io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::out) == ::fast_io::buffer_mode::out &&
			 ::fast_io::operations::decay::defines::has_output_transmit_handle_define<
				 decltype(::fast_io::operations::output_stream_ref(
					 ::std::declval<typename io_buffer_type::handle_type &>()))>)
inline constexpr decltype(auto) output_transmit_handle_define(basic_io_buffer_ref<io_buffer_type> iobref) noexcept
{
	return output_transmit_handle_define(
		::fast_io::operations::output_stream_ref(iobref.iobptr->handle));
}

} // namespace fast_io
