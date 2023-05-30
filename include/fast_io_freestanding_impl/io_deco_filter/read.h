#pragma once

namespace fast_io
{

namespace details
{

template<typename allocator_type,typename instmtype,
			typename indecotype,::std::integral input_char_type>
inline constexpr input_char_type* read_until_eof_underflow_define_sz_impl(
			instmtype instm,
			indecotype indeco,
			basic_io_buffer_pointers<input_char_type>& input_buffer,
			input_char_type* first,
			input_char_type* last,::std::size_t sz)
{
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type,input_char_type>;
	if(input_buffer.buffer_begin==nullptr)
	{
		input_buffer.buffer_end=((input_buffer.buffer_curr=input_buffer.buffer_begin=
			typed_allocator_type(sz))+sz);
	}
	while(first!=last)
	{
		auto curr_ptr{input_buffer.buffer_curr},ed_ptr{input_buffer.buffer_end};
		auto ret{::fast_io::operations::decay::read_until_eof_decay(
			instm,
			curr_ptr,ed_ptr)};
		if(ret==curr_ptr)
		{
			input_buffer.buffer_curr=input_buffer.buffer_begin;
			break;
		}
		auto [it,bufferit] = indeco.input_process_chars(curr_ptr,ed_ptr,
					first,last);
		first=it;
		if(ret==curr_ptr)
		{
			input_buffer.buffer_curr=ret;
			break;
		}
		input_buffer.buffer_curr=input_buffer.buffer_begin;
	}
	return first;
}

template<typename allocator_type,::std::size_t sz,typename instmtype,
			typename indecotype,::std::integral input_char_type>
inline constexpr input_char_type* read_until_eof_underflow_define_impl(
			instmtype instm,
			indecotype indeco,
			basic_io_buffer_pointers<input_char_type>& input_buffer,
			input_char_type* first,
			input_char_type* last)
{
	return read_until_eof_underflow_define_sz_impl<allocator_type>(instm,indeco,
			input_buffer,first,last,sz);
}

}

template<typename io_buffer_type>
inline constexpr typename io_buffer_type::input_char_type* read_until_eof_underflow_define(
			basic_io_deco_filter_ref<io_buffer_type> filter,
			typename io_buffer_type::input_char_type* first,
			typename io_buffer_type::input_char_type* last)
{
	auto &idoref{*filter.idoptr};
	return ::fast_io::details::read_until_eof_underflow_define_impl(
		::fast_io::manipulators::input_stream_ref(idoref.handle),
		::fast_io::operations::refs::input_decorators_ref(idoref.decorators),
		idoref.input_buffer,first,last);
}

}
