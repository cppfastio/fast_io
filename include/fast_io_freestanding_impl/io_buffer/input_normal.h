#pragma once

namespace fast_io::details
{

template<stream T,std::integral char_type>
inline constexpr bool ibuffer_underflow_rl_impl(T t,basic_io_buffer_pointers<char_type>& ibuffer,std::size_t bfsz)
{
	if(ibuffer.buffer_begin==nullptr)
		ibuffer.buffer_end=ibuffer.buffer_curr=ibuffer.buffer_begin=allocate_iobuf_space<char_type>(bfsz);
	ibuffer.buffer_end=read(t,ibuffer.buffer_begin,ibuffer.buffer_begin+bfsz);
	ibuffer.buffer_curr=ibuffer.buffer_begin;
	return ibuffer.buffer_begin!=ibuffer.buffer_end;
}

template<std::size_t bfsz,stream T,std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr bool ibuffer_underflow_impl(T t,basic_io_buffer_pointers<char_type>& ibuffer)
{
	return ibuffer_underflow_rl_impl(t,ibuffer,bfsz);
}

template<typename T,std::integral char_type,::std::random_access_iterator Iter>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr Iter iobuf_read_unhappy_decay_impl(T t,basic_io_buffer_pointers<char_type>& ibuffer,Iter first,Iter last,std::size_t buffer_size)
{
	std::size_t iter_diff(static_cast<std::size_t>(last-first));
	if(buffer_size<=iter_diff)
		return read(t,first,last);
	if(ibuffer.buffer_begin==nullptr)
	{
		ibuffer.buffer_end=ibuffer.buffer_begin=
		allocate_iobuf_space<char_type>(buffer_size);
	}
	ibuffer.buffer_end=read(t,ibuffer.buffer_begin,ibuffer.buffer_begin+buffer_size);
	ibuffer.buffer_curr=ibuffer.buffer_begin;
	std::size_t diff(static_cast<std::size_t>(ibuffer.buffer_end-ibuffer.buffer_begin));
	if(diff<iter_diff)
		iter_diff=diff;
	first=non_overlapped_copy_n(ibuffer.buffer_curr,iter_diff,first);
	ibuffer.buffer_curr+=iter_diff;
	return first;
}
}