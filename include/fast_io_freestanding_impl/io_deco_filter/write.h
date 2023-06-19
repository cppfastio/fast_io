#pragma once

namespace fast_io
{

namespace details
{

template<typename allocator_type,::std::size_t output_buffer_size,typename outstmtype,
			typename outdecotype,::std::integral output_char_type>
inline constexpr void decowrite_all_overflow_define_impl(
			outstmtype outstm,
			outdecotype outdeco,
			basic_io_buffer_pointers<output_char_type>& output_buffer,
			output_char_type const* first,
			output_char_type const* last)
{
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type,output_char_type>;
	if(output_buffer.buffer_begin==nullptr)
	{
		output_buffer.buffer_end=((output_buffer.buffer_curr=output_buffer.buffer_begin=
			typed_allocator_type::allocate(output_buffer_size))+output_buffer_size);
	}
	while(first!=last)
	{
		auto curr_ptr{output_buffer.buffer_curr},ed_ptr{output_buffer.buffer_end};
		auto [it,bufferit] = outdeco.output_process_chars(first,last,
			curr_ptr,ed_ptr);
		::fast_io::operations::decay::write_all_decay(
			::fast_io::operations::output_stream_ref(outstm),
			curr_ptr,bufferit);
		output_buffer.buffer_curr=output_buffer.buffer_begin;
		first=it;
	}
}

}

template<typename io_buffer_type>
inline constexpr void write_all_overflow_define(
			basic_io_deco_filter_ref<io_buffer_type> filter,
			typename io_buffer_type::output_char_type const* first,
			typename io_buffer_type::output_char_type const* last)
{
	auto &idoref{*filter.idoptr};
	using traits_type = typename io_buffer_type::traits_type;
	::fast_io::details::decowrite_all_overflow_define_impl<
		typename traits_type::allocator_type,
		traits_type::output_buffer_size>(
		::fast_io::operations::output_stream_ref(idoref.handle),
		::fast_io::operations::refs::output_decorators_ref(idoref.decorators),
		idoref.output_buffer,first,last);
}

}
