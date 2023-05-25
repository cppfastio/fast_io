#pragma once

namespace fast_io::details
{


template<typename char_type,typename Iter>
concept allow_iobuf_punning = ::std::contiguous_iterator<Iter>&&
//temporary only allow contiguous_iterator before we finish this part
(std::same_as<char_type,::std::iter_value_t<Iter>>||
(std::same_as<char_type,char>&&::std::contiguous_iterator<Iter>));

template<std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void iobuf_write_allocate_buffer_impl(basic_io_buffer_pointers<char_type>& obuffer,std::size_t buffer_size)
{
	obuffer.buffer_end=(obuffer.buffer_curr=obuffer.buffer_begin=
	allocate_iobuf_space<char_type>(buffer_size))+buffer_size;
}

template<std::integral char_type,::std::random_access_iterator Iter>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void iobuf_write_unhappy_nullptr_case_impl(basic_io_buffer_pointers<char_type>& obuffer,Iter first,Iter last,std::size_t buffer_size)
{
	iobuf_write_allocate_buffer_impl(obuffer,buffer_size);
	obuffer.buffer_curr=non_overlapped_copy(first,last,obuffer.buffer_curr);
}

template<typename T,std::integral char_type,::std::random_access_iterator Iter>
inline constexpr void iobuf_write_unhappy_decay_no_alloc_impl(T t,basic_io_buffer_pointers<char_type>& pointers,Iter first,Iter last,std::size_t buffer_size)
{
	if constexpr(false)
	{
		std::size_t const remain_space{static_cast<std::size_t>(pointers.buffer_end-pointers.buffer_curr)};
		non_overlapped_copy_n(first,remain_space,pointers.buffer_curr);
		first+=remain_space;
		write(t,pointers.buffer_begin,pointers.buffer_end);
		pointers.buffer_curr=pointers.buffer_begin;
		std::size_t const new_remain_space{static_cast<std::size_t>(last-first)};
		if(buffer_size<new_remain_space)
		{
			write(t,first,last);
		}
		else
			pointers.buffer_curr=non_overlapped_copy_n(first,new_remain_space,pointers.buffer_begin);
	}
	else
	{
		if constexpr(scatter_output_stream<T>)
		{
			io_scatter_t scatters[2]{
				{pointers.buffer_begin,
				static_cast<std::size_t>(pointers.buffer_curr-pointers.buffer_begin)*sizeof(char_type)},
				{
				first,
				static_cast<std::size_t>(last-first)*sizeof(char_type)
				}};
			scatter_write(t,io_scatters_t{scatters,2});
		}
		else
		{
			write(t,pointers.buffer_begin,pointers.buffer_curr);
			write(t,first,last);
		}
		pointers.buffer_curr=pointers.buffer_begin;
	}
}

template<std::size_t buffer_size,typename T,std::integral char_type,::std::random_access_iterator Iter>
inline constexpr void iobuf_write_unhappy_decay_impl(T t,basic_io_buffer_pointers<char_type>& pointers,Iter first,Iter last)
{
	std::size_t const diff{static_cast<std::size_t>(last-first)};
	if(pointers.buffer_begin==nullptr)
	{
		if(diff<buffer_size)
			iobuf_write_unhappy_nullptr_case_impl(pointers,first,last,buffer_size);
		else
			write(t,first,last);
		return;
	}
	iobuf_write_unhappy_decay_no_alloc_impl(t,pointers,first,last,buffer_size);
}

template<typename T,std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void iobuf_output_flush_impl(T handle,
	basic_io_buffer_pointers<char_type>& pointers)
{
	if(pointers.buffer_curr==pointers.buffer_begin)
		return;
	write(handle,pointers.buffer_begin,pointers.buffer_curr);
	pointers.buffer_curr=pointers.buffer_begin;
}

template<typename T,std::integral char_type>
inline constexpr void iobuf_output_constant_flush_prepare_impl(T handle,
	basic_io_buffer_pointers<char_type>& pointers,std::size_t buffer_size)
{
	if(pointers.buffer_begin==nullptr)
	{
		iobuf_write_allocate_buffer_impl(pointers,buffer_size);
	}
	else
	{
		iobuf_output_flush_impl(handle,pointers);
	}
}

template<typename T,std::integral char_type>
inline constexpr void iobuf_overflow_impl(T handle,
	basic_io_buffer_pointers<char_type>& pointers,char_type ch,std::size_t bfsz)
{
	iobuf_output_constant_flush_prepare_impl(handle,pointers,bfsz);
	*pointers.buffer_curr=ch;
	++pointers.buffer_curr;
}

}
