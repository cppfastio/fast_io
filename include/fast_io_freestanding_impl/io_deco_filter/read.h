#pragma once

namespace fast_io
{

template<typename io_buffer_type>
inline constexpr char_type* read_until_eof_underflow_define(
			basic_io_deco_filter_ref<T> filter,
			char_type* first,char_type* last)
{
	constexpr
		::std::size_t sz{io_buffer_type::traits_type::input_buffer_size};
	auto& ido{*filter.idoptr};
	auto& instm{ido.handle};
	auto& indeco{ido.decorators.input_process_chars};
	auto& input_buffer{ido.input_buffer};
	if(input_buffer.buffer_begin==nullptr)
	{
		input_buffer.buffer_end=((input_buffer.buffer_curr=input_buffer.buffer_begin=ido)+sz);
	}
	while(first!=last)
	{
		auto curr_ptr{input_buffer.buffer_curr},ed_ptr{input_buffer.buffer_end};
		auto ret{::fast_io::operations::decay::read_until_eof_decay(
			::fast_io::manipulators::input_stream_ref(instm),
			curr_ptr,ed_ptr)};
		if(ret==curr_ptr)
		{
			input_buffer.buffer_curr=input_buffer.buffer_begin;
			break;
		}
		auto [it,bufferit] = ido.decorators.input_process_chars(curr_ptr,ed_ptr,
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

}
