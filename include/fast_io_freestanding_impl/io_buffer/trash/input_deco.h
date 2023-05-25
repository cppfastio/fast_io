#pragma once

namespace fast_io::details
{

template<bool nsecure,stream T,typename decot,std::integral char_type>
inline constexpr bool ibuffer_underflow_rl_impl_deco(T t,decot deco,
	basic_io_buffer_pointers_with_cap<char_type>& ibuffer,
	basic_io_buffer_pointers_only_begin<typename T::char_type>& ibuffer_external,
	std::size_t bfsz)
{
	using external_char_type = typename T::char_type;
	if(ibuffer_external.buffer_begin==nullptr)
		ibuffer_external.buffer_begin=allocate_iobuf_space<external_char_type>(bfsz);
	auto buffer_begin{ibuffer_external.buffer_begin};
	auto buffer_end{buffer_begin+bfsz};
	auto readed=buffer_begin;
	for(;readed!=buffer_end;)
	{
		auto readed_after_this_round=read(t,readed,buffer_end);
		if(readed_after_this_round==readed)
		{
			if(readed==buffer_begin)
				return false;
			break;
		}
		readed=readed_after_this_round;
	}
	using decot_nocvref_t = std::remove_cvref_t<decot>;
	std::size_t readed_size{static_cast<std::size_t>(readed-buffer_begin)};
	std::size_t new_size{deco_reserve_size(io_reserve_type<char_type,decot_nocvref_t>,deco,readed_size)};
	std::size_t cap{static_cast<std::size_t>(ibuffer.buffer_cap-ibuffer.buffer_begin)};
	if(cap<new_size)
	{
		deallocate_iobuf_space<nsecure,char_type>(ibuffer.buffer_begin,cap);
		ibuffer.buffer_cap=ibuffer.buffer_end=ibuffer.buffer_curr=ibuffer.buffer_begin=nullptr;
		ibuffer.buffer_cap=(ibuffer.buffer_end=ibuffer.buffer_curr=ibuffer.buffer_begin=
		allocate_iobuf_space<char_type>(new_size))+new_size;
	}
	else
		ibuffer.buffer_end=ibuffer.buffer_curr=ibuffer.buffer_begin;
	using decot_nocvref_t = std::remove_cvref_t<decot>;
	ibuffer.buffer_end=deco_reserve_define(io_reserve_type<char_type,decot_nocvref_t>,deco,buffer_begin,readed,ibuffer.buffer_begin);
	if(ibuffer.buffer_begin==ibuffer.buffer_end)
		return false;
	return true;
}

template<bool nsecure,std::size_t bfsz,stream T,typename decot,std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr bool ibuffer_underflow_impl_deco(T t,decot deco,basic_io_buffer_pointers_with_cap<char_type>& ibuffer,
	basic_io_buffer_pointers_only_begin<typename T::char_type>& ibuffer_external)
{
#if 0
	if constexpr(maybe_noop_decorator<char_type,decot>)
	{

	}
#endif
	return ibuffer_underflow_rl_impl_deco<nsecure>(t,deco,ibuffer,ibuffer_external,bfsz);
}

template<bool nsecure,typename T,typename decot,std::integral char_type,::std::contiguous_iterator Iter>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr Iter iobuf_read_unhappy_decay_impl_deco(T t,decot deco,
	basic_io_buffer_pointers_with_cap<char_type>& ibuffer,
	basic_io_buffer_pointers_only_begin<typename T::char_type>& ibuffer_external,
	Iter first,Iter last,std::size_t bfsz)
{
	using external_char_type = typename T::char_type;
	using internal_char_type = char_type;
	if(ibuffer_external.buffer_begin==nullptr)
		ibuffer_external.buffer_begin=allocate_iobuf_space<external_char_type>(bfsz);
	auto buffer_begin{ibuffer_external.buffer_begin};
	auto buffer_end{buffer_begin+bfsz};
	for(;first!=last;)
	{
		auto read_this_round{buffer_begin};
		for(;read_this_round!=buffer_end;)
		{
			auto readed_after_this_round=read(t,read_this_round,buffer_end);
			if(readed_after_this_round==read_this_round)
			{
				if(read_this_round==buffer_begin)
					return first;
				break;
			}
			read_this_round=readed_after_this_round;
		}
		std::size_t readed_size{static_cast<std::size_t>(read_this_round-buffer_begin)};
		using decot_nocvref_t = std::remove_cvref_t<decot>;
		std::size_t new_size{deco_reserve_size(io_reserve_type<internal_char_type,decot_nocvref_t>,deco,readed_size)};
		std::size_t diff{static_cast<std::size_t>(last-first)};
		if(new_size<diff)
		{
			first=deco_reserve_define(io_reserve_type<internal_char_type,decot_nocvref_t>,deco,buffer_begin,read_this_round,first);
		}
		else
		{
			std::size_t cap{static_cast<std::size_t>(ibuffer.buffer_cap-ibuffer.buffer_begin)};
			if(cap<new_size)
			{
				deallocate_iobuf_space<nsecure,char_type>(ibuffer.buffer_begin,cap);
				ibuffer.buffer_cap=ibuffer.buffer_end=ibuffer.buffer_curr=ibuffer.buffer_begin=nullptr;
				ibuffer.buffer_cap=(ibuffer.buffer_end=ibuffer.buffer_curr=ibuffer.buffer_begin=
				allocate_iobuf_space<char_type>(new_size))+new_size;
			}
			else
				ibuffer.buffer_end=ibuffer.buffer_curr=ibuffer.buffer_begin;
			ibuffer.buffer_end=deco_reserve_define(io_reserve_type<internal_char_type,decot_nocvref_t>,deco,
				buffer_begin,read_this_round,ibuffer.buffer_begin);
			std::size_t need_to_copy{static_cast<std::size_t>(ibuffer.buffer_end-ibuffer.buffer_begin)};
			if(need_to_copy==0)
				return first;
			if(diff<need_to_copy)
				need_to_copy=diff;
			non_overlapped_copy_n(ibuffer.buffer_begin,need_to_copy,first);
			ibuffer.buffer_curr=ibuffer.buffer_begin+need_to_copy;
			first+=need_to_copy;
		}
	}
	return first;
}

}