#pragma once

namespace fast_io
{

template<typename io_buffer_type>
inline constexpr char_type const* write_some_overflow_define(
			basic_io_deco_filter_ref<T> filter,
			char_type const* first,char_type const* last)
{
	constexpr
		::std::size_t sz{io_buffer_type::traits_type::output_buffer_size};

	auto& ido{*filter.idoptr};
	
	auto& outstm{ido.handle};

	auto& outdeco{ido.decorators.output_process_chars};

	auto& output_buffer{ido.output_obuffer};

	if(output_buffer.buffer_begin==nullptr)
	{
		output_buffer.buffer_end=((output_buffer.buffer_curr=output_buffer.buffer_begin=
		ido)+sz);
	}

	while(first!=last)
	{
		auto curr_ptr{output_buffer.buffer_curr},ed_ptr{output_buffer.buffer_end};
		auto [it,bufferit] = ido.decorators.output_process_chars(first,last,
			curr_ptr,ed_ptr);
		first=it;
		auto ret{::fast_io::operations::decay::write_some_decay(
			::fast_io::manipulators::output_stream_ref(outstm),
			curr_ptr,ed_ptr)};
		if(ret!=ed_ptr)
		{
			output_buffer.buffer_curr=ret;
			break;
		}
		output_buffer.buffer_curr=output_buffer.buffer_begin;
	}
	return first;
}

template<typename io_buffer_type>
inline constexpr void write_all_overflow_define(
			basic_io_deco_filter_ref<T> filter,
			char_type const* first,char_type const* last)
{
	constexpr
		::std::size_t buffersize{io_buffer_type::traits_type::output_buffer_size};
	using allocator_type = typename io_buffer_type::traits_type::allocator_type;
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type,char_type>;
	auto& ido{*filter.idoptr};
	auto& outstm{ido.handle};
	auto& outdeco{ido.decorators.output_process_chars};
	auto& output_buffer{ido.output_buffer};
	if(output_buffer.buffer_begin==nullptr)
	{
		output_buffer.buffer_end=((output_buffer.buffer_curr=output_buffer.buffer_begin=
			typed_allocator_type::allocate(buffersize))+buffersize);
	}
	while(first!=last)
	{
		auto curr_ptr{output_buffer.buffer_curr},ed_ptr{output_buffer.buffer_end};
		auto [it,bufferit] = ido.decorators.output_process_chars(first,last,
			curr_ptr,ed_ptr);
		::fast_io::operations::decay::write_all_decay(
			::fast_io::manipulators::output_stream_ref(outstm),
			curr_ptr,ed_ptr);
		output_buffer.buffer_curr=output_buffer.buffer_begin;
		first=it;
	}
}

}
