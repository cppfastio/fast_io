#pragma once

namespace fast_io
{

template<typename T>
class basic_io_buffer_ref
{
public:
	using io_buffer_type = T;
	using handle_type = typename io_buffer_type::handle_type;
	using input_char_type = typename io_buffer_type::input_char_type;
	using output_char_type = typename io_buffer_type::output_char_type;
	using allocator_type = typename io_buffer_type::allocator_type;
	using native_handle_type = io_buffer_type*;
	native_handle_type iobptr{};
};

template<typename handletype,
	::std::integral input_char_type,
	::std::integral output_char_type,
	buffer_mode mode,
	typename allocatortp,
	std::size_t bfs>
requires ((mode&buffer_mode::out)==buffer_mode::out&&(mode&buffer_mode::in)==buffer_mode::in)
inline constexpr basic_io_buffer_ref<basic_io_buffer<handletype,input_char_type,output_char_type,mode,allocatortp,bfs>>
	io_stream_ref_define(basic_io_buffer<handletype,input_char_type,output_char_type,mode,decoratorstypr,allocatortp,bfs> &r)
{
	return {__builtin_addressof(r)};
}

template<typename handletype,
	::std::integral input_char_type,
	::std::integral output_char_type,
	buffer_mode mode,
	typename allocatortp,
	std::size_t bfs>
requires ((mode&buffer_mode::out)==buffer_mode::out)
inline constexpr basic_io_buffer_ref<basic_io_buffer<handletype,input_char_type,output_char_type,mode,decoratorstypr,allocatortp,bfs>> output_stream_ref_define(basic_io_buffer<handletype,mde,decoratorstypr,allocatortp,bfs> &r)
{
	return {__builtin_addressof(r)};
}

template<typename handletype,
	::std::integral input_char_type,
	::std::integral output_char_type,
	buffer_mode mode,
	typename allocatortp,
	std::size_t bfs>
requires ((mode&buffer_mode::in)==buffer_mode::in)
inline constexpr basic_io_buffer_ref<basic_io_buffer<handletype,
	input_char_type,
	output_char_type,mode,allocatortp,bfs>> input_stream_ref_define(
	basic_io_buffer<handletype,input_char_type,
	output_char_type,mode,allocatortp,bfs> &r)
{
	return {__builtin_addressof(r)};
}

}
