#pragma once

namespace fast_io
{

namespace details::io_buffer
{

template<typename allocator_type,std::integral char_type,::std::size_t bfsz,typename instmstype>
inline constexpr bool ibuffer_underflow_rl_impl(instmtype insm,
	basic_io_buffer_pointers<char_type>& ibuffer)
{
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type,char_type>;
	if(ibuffer.buffer_begin==nullptr)
		ibuffer.buffer_end=ibuffer.buffer_curr=ibuffer.buffer_begin=typed_allocator_type::allocate(bfsz);
	ibuffer.buffer_end=::fast_io::operations::read_some(t,ibuffer.buffer_begin,ibuffer.buffer_begin+bfsz);
	ibuffer.buffer_curr=ibuffer.buffer_begin;
	return ibuffer.buffer_begin!=ibuffer.buffer_end;
}

template<::std::integral char_type,
	typename instmtype>
inline constexpr char_type* read_some_underflow_impl(
	optstmtype instm,
	basic_io_buffer_pointers<char_type>& __restrict pointers,
	char_type *first,char_type *last)
{
	first=::fast_io::details::non_overlapped_copy(pointers.ibuffer.buffer_curr,pointers.ibuffer.buffer_end,first);
	if(pointers.begin_ptr==nullptr)
	{

	}
	return first;
}

}

}

