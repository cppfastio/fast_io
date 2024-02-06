#pragma once

namespace fast_io
{

namespace details
{

template <typename outstmtype, typename outdecotype, ::std::integral output_char_type>
inline constexpr void decowrite_all_overflow_common_define_impl(outstmtype outstm, outdecotype outdeco,
																output_char_type const *first,
																output_char_type const *last,
																typename outstmtype::output_char_type *bufferfirst,
																typename outstmtype::output_char_type *bufferlast)
{
	while (first != last)
	{
		auto [it, bufferit] = outdeco.output_process_chars(first, last, bufferfirst, bufferlast);
		::fast_io::operations::decay::write_all_decay(outstm, bufferfirst, bufferit);
		first = it;
	}
}

template <typename allocator_type, ::std::size_t output_buffer_size, typename outstmtype, typename outdecotype,
		  ::std::integral output_char_type>
inline constexpr void decowrite_all_overflow_define_impl(outstmtype outstm, outdecotype outdeco,
														 output_char_type const *first, output_char_type const *last)
{
	::std::size_t diff{static_cast<::std::size_t>(last - first)};
	if (diff == 0)
	{
		return;
	}
	::fast_io::details::buffer_alloc_arr_ptr<typename outstmtype::output_char_type, false, allocator_type> arrptr(
		output_buffer_size);
	::fast_io::details::decowrite_all_overflow_common_define_impl(outstm, outdeco, first, last, arrptr.ptr,
																  arrptr.ptr + output_buffer_size);
}

template <typename allocator_type, typename outstmtype, typename outdecotype, ::std::integral output_char_type>
inline constexpr void
deco_scatter_write_all_overflow_common_define_impl(outstmtype outstm, outdecotype outdeco,
												   basic_io_scatter_t<output_char_type> const *scatters,
												   ::std::size_t n, ::std::size_t output_buffer_size)
{
	if (n == 0)
	{
		return;
	}
	::fast_io::details::buffer_alloc_arr_ptr<typename outstmtype::output_char_type, false, allocator_type> arrptr(
		output_buffer_size);
	for (::std::size_t i{}; i != n; ++i)
	{
		auto [base, sn] = scatters[i];
		::fast_io::details::decowrite_all_overflow_common_define_impl(outstm, outdeco, base, base + sn, arrptr.ptr,
																	  arrptr.ptr + output_buffer_size);
	}
}

template <typename allocator_type, ::std::size_t output_buffer_size, typename outstmtype, typename outdecotype,
		  ::std::integral output_char_type>
inline constexpr void deco_scatter_write_all_overflow_define_impl(outstmtype outstm, outdecotype outdeco,
																  basic_io_scatter_t<output_char_type> const *scatters,
																  ::std::size_t n)
{
	::fast_io::details::deco_scatter_write_all_overflow_common_define_impl<allocator_type>(outstm, outdeco, scatters, n,
																						   output_buffer_size);
}

} // namespace details

template <typename io_buffer_type>
inline constexpr void
write_all_overflow_define(basic_io_deco_filter_ref<io_buffer_type> filter,
						  typename basic_io_deco_filter_ref<io_buffer_type>::output_char_type const *first,
						  typename basic_io_deco_filter_ref<io_buffer_type>::output_char_type const *last)
{
	auto &idoref{*filter.idoptr};
	::fast_io::details::decowrite_all_overflow_define_impl<typename io_buffer_type::traits_type::allocator_type,
														   io_buffer_type::traits_type::output_buffer_size>(
		::fast_io::operations::output_stream_ref(idoref.handle),
		::fast_io::operations::refs::output_decorators_ref(idoref.decorators), first, last);
}

template <typename io_buffer_type>
inline constexpr void scatter_write_all_overflow_define(
	basic_io_deco_filter_ref<io_buffer_type> filter,
	basic_io_scatter_t<typename basic_io_deco_filter_ref<io_buffer_type>::output_char_type> const *scatters,
	::std::size_t n)
{
	auto &idoref{*filter.idoptr};
	::fast_io::details::deco_scatter_write_all_overflow_define_impl<
		typename io_buffer_type::traits_type::allocator_type, io_buffer_type::traits_type::output_buffer_size>(
		::fast_io::operations::output_stream_ref(idoref.handle),
		::fast_io::operations::refs::output_decorators_ref(idoref.decorators), scatters, n);
}

} // namespace fast_io
