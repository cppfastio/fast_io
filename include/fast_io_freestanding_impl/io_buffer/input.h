#pragma once

namespace fast_io
{

namespace details::io_buffer
{

template <typename allocator_type, ::std::integral char_type, typename instmtype>
inline constexpr bool ibuffer_underflow_rl_size_impl(::fast_io::details::io_buffer::iobuffer_alloc_handle_t<allocator_type, char_type> allochdl,
													 instmtype insm, basic_io_buffer_pointers<char_type> &ibuffer,
													 ::std::size_t bfsz)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype> ||
								   ::fast_io::typed_generic_allocator_adapter<allocator_type, char_type>::throws_on_allocation_failure)
{
	if (ibuffer.buffer_begin == nullptr)
	{
		ibuffer.buffer_end = ibuffer.buffer_curr = ibuffer.buffer_begin =
			::fast_io::details::io_buffer::iobuffer_allocate<char_type, allocator_type>(allochdl, bfsz);
	}
	ibuffer.buffer_end =
		::fast_io::operations::decay::read_some_decay(insm, ibuffer.buffer_begin, bfsz);
	ibuffer.buffer_curr = ibuffer.buffer_begin;
	return ibuffer.buffer_begin != ibuffer.buffer_end;
}

template <::std::size_t bfsz, ::std::integral char_type, typename allocator_type, typename instmtype>
inline constexpr bool ibuffer_underflow_rl_impl(::fast_io::details::io_buffer::iobuffer_alloc_handle_t<allocator_type, char_type> allochdl,
												instmtype insm, basic_io_buffer_pointers<char_type> &ibuffer)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype> ||
								   ::fast_io::typed_generic_allocator_adapter<allocator_type, char_type>::throws_on_allocation_failure)
{
	return ::fast_io::details::io_buffer::ibuffer_underflow_rl_size_impl<allocator_type>(allochdl, insm, ibuffer, bfsz);
}

template <typename allocator_type, ::std::integral char_type, typename instmtype>
inline constexpr void
ibuffer_minimum_size_underflow_all_prepare_rl_size_impl(::fast_io::details::io_buffer::iobuffer_alloc_handle_t<allocator_type, char_type> allochdl,
														instmtype insm, basic_io_buffer_pointers<char_type> &ibuffer,
														::std::size_t bfsz)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype> ||
								   ::fast_io::typed_generic_allocator_adapter<allocator_type, char_type>::throws_on_allocation_failure)
{
	if (ibuffer.buffer_begin == nullptr)
	{
		ibuffer.buffer_end = ibuffer.buffer_curr = ibuffer.buffer_begin =
			::fast_io::details::io_buffer::iobuffer_allocate<char_type, allocator_type>(allochdl, bfsz);
	}
	auto bg{ibuffer.buffer_begin};
	auto ed{bg + bfsz};
	::fast_io::operations::decay::read_all_decay(insm, bg, bfsz);
	ibuffer.buffer_curr = bg;
	ibuffer.buffer_end = ed;
}

template <::std::size_t bfsz, ::std::integral char_type, typename allocator_type, typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void ibuffer_minimum_size_underflow_all_prepare_impl(::fast_io::details::io_buffer::iobuffer_alloc_handle_t<allocator_type, char_type> allochdl,
																	  instmtype insm,
																	  basic_io_buffer_pointers<char_type> &ibuffer)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype> ||
								   ::fast_io::typed_generic_allocator_adapter<allocator_type, char_type>::throws_on_allocation_failure)
{
	::fast_io::details::io_buffer::ibuffer_minimum_size_underflow_all_prepare_rl_size_impl<allocator_type>(
		allochdl, insm, ibuffer, bfsz);
}

template <typename allocator_type, ::std::integral char_type, typename instmtype>
inline constexpr char_type *read_some_underflow_size_impl(::fast_io::details::io_buffer::iobuffer_alloc_handle_t<allocator_type, char_type> allochdl,
														  instmtype instm,
														  basic_io_buffer_pointers<char_type> &__restrict pointers,
														  char_type *first, ::std::size_t count, ::std::size_t bfsz)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype> ||
								   ::fast_io::typed_generic_allocator_adapter<allocator_type, char_type>::throws_on_allocation_failure)
{
	::std::size_t const buffered{static_cast<::std::size_t>(pointers.buffer_end - pointers.buffer_curr)};
	::std::size_t const take{buffered < count ? buffered : count};
	first = ::fast_io::details::non_overlapped_copy_n(pointers.buffer_curr, take, first);
	pointers.buffer_curr += take;
	if (take == count)
	{
		return first;
	}
	::std::size_t const remaining{count - take};
	if (pointers.buffer_begin == nullptr)
	{
		pointers.buffer_end = pointers.buffer_curr = pointers.buffer_begin =
			::fast_io::details::io_buffer::iobuffer_allocate<char_type, allocator_type>(allochdl, bfsz);
	}
	if constexpr (::fast_io::operations::decay::defines::has_any_of_read_bytes_operations<instmtype>)
	{
		::fast_io::io_scatter_t scatters[2]{
			{first, remaining * sizeof(char_type)},
			{pointers.buffer_begin, bfsz * sizeof(char_type)}};
		auto [pos, scpos]{::fast_io::operations::decay::scatter_read_some_bytes_decay(instm, scatters, 2)};
		if (pos == 2)
		{
			pointers.buffer_end = (pointers.buffer_curr = pointers.buffer_begin) + bfsz;
			return first + remaining;
		}
		else if (pos == 1)
		{
			pointers.buffer_end = (pointers.buffer_curr = pointers.buffer_begin) + (scpos / sizeof(char_type));
			return first + remaining;
		}
		else
		{
			pointers.buffer_end = pointers.buffer_curr = pointers.buffer_begin;
			return first + scpos / sizeof(char_type);
		}
	}
	else
	{
		basic_io_scatter_t<char_type> scatters[2]{{first, remaining},
												  {pointers.buffer_begin, bfsz}};
		auto [pos, scpos]{::fast_io::operations::decay::scatter_read_some_decay(instm, scatters, 2)};
		if (pos == 2)
		{
			pointers.buffer_end = (pointers.buffer_curr = pointers.buffer_begin) + bfsz;
			return first + remaining;
		}
		else if (pos == 1)
		{
			pointers.buffer_end = (pointers.buffer_curr = pointers.buffer_begin) + scpos;
			return first + remaining;
		}
		else
		{
			pointers.buffer_end = pointers.buffer_curr = pointers.buffer_begin;
			return first + scpos;
		}
	}
}

template <::std::size_t bfsz, typename allocatortype, ::std::integral char_type, typename instmtype>
inline constexpr char_type *read_some_underflow_impl(::fast_io::details::io_buffer::iobuffer_alloc_handle_t<allocatortype, char_type> allochdl,
													 instmtype instm,
													 basic_io_buffer_pointers<char_type> &__restrict pointers,
													 char_type *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::decay::defines::input_stream_operations_nothrow<instmtype> ||
								   ::fast_io::typed_generic_allocator_adapter<allocatortype, char_type>::throws_on_allocation_failure)
{
	return ::fast_io::details::io_buffer::read_some_underflow_size_impl<allocatortype>(allochdl, instm, pointers,
																					   first, count, bfsz);
}

} // namespace details::io_buffer

template <typename io_buffer_type, ::std::integral char_type>
inline constexpr char_type *read_some_underflow_define(basic_io_buffer_ref<io_buffer_type> iobref,
													   char_type *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		!::fast_io::operations::decay::defines::input_stream_operations_nothrow<
			decltype(::fast_io::operations::input_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))> ||
		((io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::out) == ::fast_io::buffer_mode::out &&
		 (io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::tie) == ::fast_io::buffer_mode::tie &&
		 ::fast_io::details::io_buffer_output_operations_may_throw<typename io_buffer_type::handle_type>))
{
	constexpr auto mode{io_buffer_type::traits_type::mode};
	if constexpr ((mode & buffer_mode::out) == buffer_mode::out && (mode & buffer_mode::tie) == buffer_mode::tie)
	{
		output_stream_buffer_flush_define(iobref);
	}
	return ::fast_io::details::io_buffer::read_some_underflow_impl<
		io_buffer_type::traits_type::input_buffer_size, typename io_buffer_type::traits_type::allocator_type>(
		iobref.iobptr->allocator_handle, ::fast_io::operations::input_stream_ref(iobref.iobptr->handle),
		iobref.iobptr->input_buffer, first, count);
}

template <typename io_buffer_type, ::std::integral char_type>
	requires(::fast_io::operations::decay::defines::preadable<
			 decltype(::fast_io::operations::input_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))>)
inline constexpr char_type *pread_some_underflow_define(basic_io_buffer_ref<io_buffer_type> iobref,
														char_type *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		!::fast_io::operations::decay::defines::input_stream_operations_nothrow<
			decltype(::fast_io::operations::input_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))> ||
		((io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::out) == ::fast_io::buffer_mode::out &&
		 (io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::tie) == ::fast_io::buffer_mode::tie &&
		 ::fast_io::details::io_buffer_output_operations_may_throw<typename io_buffer_type::handle_type>))
{
	constexpr auto mode{io_buffer_type::traits_type::mode};
	if constexpr ((mode & buffer_mode::out) == buffer_mode::out && (mode & buffer_mode::tie) == buffer_mode::tie)
	{
		output_stream_buffer_flush_define(iobref);
	}
	return ::fast_io::operations::decay::pread_some_decay(::fast_io::operations::input_stream_ref(iobref.iobptr->handle), first, count, off);
}

template <typename io_buffer_type, ::std::integral char_type>
	requires(::fast_io::operations::decay::defines::preadable<
			 decltype(::fast_io::operations::input_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))>)
inline constexpr void pread_all_underflow_define(basic_io_buffer_ref<io_buffer_type> iobref,
												 char_type *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		!::fast_io::operations::decay::defines::input_stream_operations_nothrow<
			decltype(::fast_io::operations::input_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))> ||
		((io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::out) == ::fast_io::buffer_mode::out &&
		 (io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::tie) == ::fast_io::buffer_mode::tie &&
		 ::fast_io::details::io_buffer_output_operations_may_throw<typename io_buffer_type::handle_type>))
{
	constexpr auto mode{io_buffer_type::traits_type::mode};
	if constexpr ((mode & buffer_mode::out) == buffer_mode::out && (mode & buffer_mode::tie) == buffer_mode::tie)
	{
		output_stream_buffer_flush_define(iobref);
	}
	return ::fast_io::operations::decay::pread_all_decay(::fast_io::operations::input_stream_ref(iobref.iobptr->handle), first, count, off);
}

template <typename io_buffer_type, ::std::integral char_type>
	requires(::fast_io::operations::decay::defines::preadable<
			 decltype(::fast_io::operations::input_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))>)
inline constexpr ::fast_io::io_scatter_status_t scatter_pread_some_underflow_define(basic_io_buffer_ref<io_buffer_type> iobref,
																					basic_io_scatter_t<char_type> const *pscatters, ::std::size_t n, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		!::fast_io::operations::decay::defines::input_stream_operations_nothrow<
			decltype(::fast_io::operations::input_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))> ||
		((io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::out) == ::fast_io::buffer_mode::out &&
		 (io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::tie) == ::fast_io::buffer_mode::tie &&
		 ::fast_io::details::io_buffer_output_operations_may_throw<typename io_buffer_type::handle_type>))
{
	constexpr auto mode{io_buffer_type::traits_type::mode};
	if constexpr ((mode & buffer_mode::out) == buffer_mode::out && (mode & buffer_mode::tie) == buffer_mode::tie)
	{
		output_stream_buffer_flush_define(iobref);
	}
	return ::fast_io::operations::decay::scatter_pread_some_decay(::fast_io::operations::input_stream_ref(iobref.iobptr->handle), pscatters, n, off);
}

template <typename io_buffer_type, ::std::integral char_type>
	requires(::fast_io::operations::decay::defines::preadable<
			 decltype(::fast_io::operations::input_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))>)
inline constexpr void scatter_pread_all_underflow_define(basic_io_buffer_ref<io_buffer_type> iobref,
														 basic_io_scatter_t<char_type> const *pscatters, ::std::size_t n, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		!::fast_io::operations::decay::defines::input_stream_operations_nothrow<
			decltype(::fast_io::operations::input_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))> ||
		((io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::out) == ::fast_io::buffer_mode::out &&
		 (io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::tie) == ::fast_io::buffer_mode::tie &&
		 ::fast_io::details::io_buffer_output_operations_may_throw<typename io_buffer_type::handle_type>))
{
	constexpr auto mode{io_buffer_type::traits_type::mode};
	if constexpr ((mode & buffer_mode::out) == buffer_mode::out && (mode & buffer_mode::tie) == buffer_mode::tie)
	{
		output_stream_buffer_flush_define(iobref);
	}
	::fast_io::operations::decay::scatter_pread_all_decay(::fast_io::operations::input_stream_ref(iobref.iobptr->handle), pscatters, n, off);
}

template <typename io_buffer_type, ::std::integral char_type>
	requires(::fast_io::operations::decay::defines::preadable<
			 decltype(::fast_io::operations::input_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))>)
inline constexpr char_type *pread_all_underflow_define(basic_io_buffer_ref<io_buffer_type> iobref,
													   char_type *first, ::std::size_t count, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		!::fast_io::operations::decay::defines::input_stream_operations_nothrow<
			decltype(::fast_io::operations::input_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))> ||
		((io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::out) == ::fast_io::buffer_mode::out &&
		 (io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::tie) == ::fast_io::buffer_mode::tie &&
		 ::fast_io::details::io_buffer_output_operations_may_throw<typename io_buffer_type::handle_type>))
{
	constexpr auto mode{io_buffer_type::traits_type::mode};
	if constexpr ((mode & buffer_mode::out) == buffer_mode::out && (mode & buffer_mode::tie) == buffer_mode::tie)
	{
		output_stream_buffer_flush_define(iobref);
	}
	return ::fast_io::operations::decay::pread_all_decay(::fast_io::operations::input_stream_ref(iobref.iobptr->handle), first, count, off);
}

template <typename io_buffer_type>
inline constexpr bool ibuffer_underflow(basic_io_buffer_ref<io_buffer_type> iobref)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		!::fast_io::operations::decay::defines::input_stream_operations_nothrow<
			decltype(::fast_io::operations::input_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))> ||
		((io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::out) == ::fast_io::buffer_mode::out &&
		 (io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::tie) == ::fast_io::buffer_mode::tie &&
		 ::fast_io::details::io_buffer_output_operations_may_throw<typename io_buffer_type::handle_type>))
{
	constexpr auto mode{io_buffer_type::traits_type::mode};
	if constexpr ((mode & buffer_mode::out) == buffer_mode::out && (mode & buffer_mode::tie) == buffer_mode::tie)
	{
		output_stream_buffer_flush_define(iobref);
	}
	return ::fast_io::details::io_buffer::ibuffer_underflow_rl_impl<
		io_buffer_type::traits_type::input_buffer_size, typename io_buffer_type::traits_type::input_char_type,
		typename io_buffer_type::traits_type::allocator_type>(
		iobref.iobptr->allocator_handle, ::fast_io::operations::input_stream_ref(iobref.iobptr->handle),
		iobref.iobptr->input_buffer);
}

template <typename io_buffer_type>
inline constexpr auto ibuffer_begin(basic_io_buffer_ref<io_buffer_type> iobref) noexcept
{
	return iobref.iobptr->input_buffer.buffer_begin;
}

template <typename io_buffer_type>
inline constexpr auto ibuffer_curr(basic_io_buffer_ref<io_buffer_type> iobref) noexcept
{
	return iobref.iobptr->input_buffer.buffer_curr;
}

template <typename io_buffer_type>
inline constexpr auto ibuffer_end(basic_io_buffer_ref<io_buffer_type> iobref) noexcept
{
	return iobref.iobptr->input_buffer.buffer_end;
}

template <typename io_buffer_type>
inline constexpr void ibuffer_set_curr(basic_io_buffer_ref<io_buffer_type> iobref,
									   typename basic_io_buffer_ref<io_buffer_type>::input_char_type *ptr) noexcept
{
	iobref.iobptr->input_buffer.buffer_curr = ptr;
}

template <::std::integral char_type, typename io_buffer_type>
	requires(::std::same_as<char_type, typename basic_io_buffer_ref<io_buffer_type>::input_char_type>)
inline constexpr ::std::size_t
	ibuffer_minimum_size_define(::fast_io::io_reserve_type_t<char_type, basic_io_buffer_ref<io_buffer_type>>)
{
	return io_buffer_type::traits_type::input_buffer_size;
}

template <typename io_buffer_type>
inline constexpr void ibuffer_minimum_size_underflow_all_prepare_define(basic_io_buffer_ref<io_buffer_type> iobref)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		!::fast_io::operations::decay::defines::input_stream_operations_nothrow<
			decltype(::fast_io::operations::input_stream_ref(::std::declval<typename io_buffer_type::handle_type &>()))> ||
		((io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::out) == ::fast_io::buffer_mode::out &&
		 (io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::tie) == ::fast_io::buffer_mode::tie &&
		 ::fast_io::details::io_buffer_output_operations_may_throw<typename io_buffer_type::handle_type>))
{
	::fast_io::details::io_buffer::ibuffer_minimum_size_underflow_all_prepare_impl<
		io_buffer_type::traits_type::input_buffer_size, typename io_buffer_type::traits_type::input_char_type,
		typename io_buffer_type::traits_type::allocator_type>(
		iobref.iobptr->allocator_handle, ::fast_io::operations::input_stream_ref(iobref.iobptr->handle),
		iobref.iobptr->input_buffer);
}

template <typename io_buffer_type>
	requires((io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::in) == ::fast_io::buffer_mode::in &&
			 ::fast_io::operations::decay::defines::has_input_transmit_handle_define<
				 decltype(::fast_io::operations::input_stream_ref(
					 ::std::declval<typename io_buffer_type::handle_type &>()))>)
inline constexpr decltype(auto) input_transmit_handle_define(basic_io_buffer_ref<io_buffer_type> iobref) noexcept
{
	return input_transmit_handle_define(
		::fast_io::operations::input_stream_ref(iobref.iobptr->handle));
}

} // namespace fast_io
