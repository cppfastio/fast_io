#pragma once

namespace fast_io
{

namespace details
{

template<typename allocator_type,typename instmtype,
			typename indecotype,::std::integral input_char_type>
inline constexpr input_char_type* decoread_until_eof_underflow_define_sz_impl(
			instmtype instm,
			indecotype indeco,
			basic_io_buffer_pointers<input_char_type>& input_buffer,
			input_char_type* first,
			input_char_type* last,::std::size_t sz)
{
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type,input_char_type>;
	auto bufbg{input_buffer.buffer_begin};
	if(bufbg==nullptr)
	{
		input_buffer.buffer_end=input_buffer.buffer_curr=input_buffer.buffer_begin=
			bufbg=typed_allocator_type::allocate(sz);
	}
	auto bufcur{input_buffer.buffer_curr};
	auto bufed{input_buffer.buffer_end};
	for(;;)
	{
		if(bufcur!=bufed)
		{
			auto [bufferit,it] = indeco.input_process_chars(bufcur,bufed,first,last);
			input_buffer.buffer_curr = bufferit-bufcur+bufcur;
			first=it;
			if(first==last)
			{
				return first;
			}
		}
		auto ret{::fast_io::operations::decay::read_until_eof_decay(
			instm,
			bufbg,bufbg+sz)};
		input_buffer.buffer_curr=bufcur=bufbg;
		input_buffer.buffer_end=bufed=ret;
		if(ret==bufbg)
		{
			return first;
		}
	}
}

template<typename allocator_type,::std::size_t sz,typename instmtype,
			typename indecotype,::std::integral input_char_type>
inline constexpr input_char_type* decoread_until_eof_underflow_define_impl(
			instmtype instm,
			indecotype indeco,
			basic_io_buffer_pointers<input_char_type>& input_buffer,
			input_char_type* first,
			input_char_type* last)
{
	return ::fast_io::details::decoread_until_eof_underflow_define_sz_impl<allocator_type>(instm,indeco,
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
	using traits_type = typename io_buffer_type::traits_type;
	return ::fast_io::details::decoread_until_eof_underflow_define_impl<
		typename traits_type::allocator_type,
		traits_type::input_buffer_size>(
		::fast_io::manipulators::input_stream_ref(idoref.handle),
		::fast_io::operations::refs::input_decorators_ref(idoref.decorators),
		idoref.input_buffer,first,last);
}

}
