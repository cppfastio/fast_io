#pragma once

namespace fast_io::details
{

template<typename T,typename decot,std::integral char_type,::std::random_access_iterator Iter>
inline constexpr void iobuf_write_unhappy_decay_no_alloc_impl_deco(T t,decot deco,
	basic_io_buffer_pointers<char_type>& pointers,
	basic_io_buffer_pointers_no_curr<typename T::char_type>& external_buffer,
	Iter first,Iter last,std::size_t buffer_size)
{
	std::size_t const remain_space(pointers.buffer_end-pointers.buffer_curr);
	non_overlapped_copy_n(first,remain_space,pointers.buffer_curr);
	first+=remain_space;
	write_with_deco(t,deco,pointers.buffer_begin,pointers.buffer_end,
		external_buffer,buffer_size);
	pointers.buffer_curr=pointers.buffer_begin;
	std::size_t const new_remain_space(last-first);
	if(buffer_size<new_remain_space)
		write_with_deco(t,deco,first,last,external_buffer,buffer_size);
	else
		pointers.buffer_curr=non_overlapped_copy_n(first,new_remain_space,pointers.buffer_begin);

}
template<std::size_t buffer_size,typename T,typename decot,std::integral char_type,::std::random_access_iterator Iter>
inline constexpr void iobuf_write_unhappy_decay_impl_deco(
	T t,decot deco,
	basic_io_buffer_pointers<char_type>& pointers,
	basic_io_buffer_pointers_no_curr<typename T::char_type>& external_buffer,
	Iter first,Iter last)
{
	std::size_t const diff(static_cast<std::size_t>(last-first));
	if(pointers.buffer_begin==nullptr)
	{
		if(diff<buffer_size)
			iobuf_write_unhappy_nullptr_case_impl(pointers,first,last,buffer_size);
		else
			write_with_deco(t,deco,first,last,external_buffer,buffer_size);
		return;
	}
	iobuf_write_unhappy_decay_no_alloc_impl_deco(t,deco,
	pointers,
	external_buffer,
	first,last,buffer_size);
}

template<typename T,typename decot,std::integral char_type>
inline constexpr void iobuf_output_flush_impl_deco(T handle,decot deco,
	basic_io_buffer_pointers<char_type>& pointers,
	basic_io_buffer_pointers_no_curr<typename T::char_type>& external_buffer,std::size_t bfsz)
{
	if(pointers.buffer_curr==pointers.buffer_begin)
		return;
	write_with_deco(handle,deco,
	pointers.buffer_begin,
	pointers.buffer_curr,
	external_buffer,bfsz);
	pointers.buffer_curr=pointers.buffer_begin;
}

template<typename T,typename decot,std::integral char_type>
inline constexpr void iobuf_output_constant_flush_prepare_impl_deco(T handle,decot deco,
	basic_io_buffer_pointers<char_type>& pointers,
	basic_io_buffer_pointers_no_curr<typename T::char_type>& external_buffer,
	std::size_t bfsz)
{
	if(pointers.buffer_begin==nullptr)
	{
		iobuf_write_allocate_buffer_impl(pointers,bfsz);
	}
	else
	{
		iobuf_output_flush_impl_deco(handle,deco,pointers,external_buffer,bfsz);
	}
}

template<typename T,typename decot,std::integral char_type>
inline constexpr void iobuf_overflow_impl_deco(T handle,decot deco,
	basic_io_buffer_pointers<char_type>& pointers,
	basic_io_buffer_pointers_no_curr<typename T::char_type>& external_buffer,
	char_type ch,std::size_t bfsz)
{
	iobuf_output_constant_flush_prepare_impl_deco(handle,deco,pointers,external_buffer,bfsz);
	*pointers.buffer_curr=ch;
	++pointers.buffer_curr;
}

}