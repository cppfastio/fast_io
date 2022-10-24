#pragma once
//std::string is not usable in freestanding environments and cause problems for this library
//This is a very simple stuff for internal use

namespace fast_io
{

template<std::integral ch_type>
class dynamic_io_buffer
{
public:
	using char_type = ch_type;
	using pointer = char_type*;
	pointer buffer_begin=nullptr,buffer_curr=nullptr,buffer_end=nullptr;
	constexpr dynamic_io_buffer()=default;
	dynamic_io_buffer(dynamic_io_buffer const&)=delete;
	dynamic_io_buffer& operator=(dynamic_io_buffer const&)=delete;
#if __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	~dynamic_io_buffer()
	{
		details::deallocate_iobuf_space<false,char_type>(buffer_begin,buffer_end-buffer_begin);
	}
};

template<std::integral char_type>
[[nodiscard]] constexpr char_type* obuffer_begin(dynamic_io_buffer<char_type>& ob) noexcept
{
	return ob.buffer_begin;
}

template<std::integral char_type>
[[nodiscard]] constexpr char_type* obuffer_curr(dynamic_io_buffer<char_type>& ob) noexcept
{
	return ob.buffer_curr;
}

template<std::integral char_type>
[[nodiscard]] constexpr char_type* obuffer_end(dynamic_io_buffer<char_type>& ob) noexcept
{
	return ob.buffer_end;
}

template<std::integral char_type>
inline constexpr void obuffer_set_curr(dynamic_io_buffer<char_type>& ob,char_type* ptr) noexcept
{
	ob.buffer_curr=ptr;
}

namespace details
{

template<std::integral char_type>
inline constexpr void dynamic_io_buffer_oreallocate_impl(dynamic_io_buffer<char_type>& ob,std::size_t size) noexcept
{
	auto new_space{allocate_iobuf_space<char_type>(size)};
	std::size_t ptr_diff{static_cast<std::size_t>(ob.buffer_curr-ob.buffer_begin)};
	non_overlapped_copy_n(ob.buffer_begin,ptr_diff,new_space);
	deallocate_iobuf_space<false,char_type>(ob.buffer_begin,ob.buffer_end-ob.buffer_begin);
	ob.buffer_begin=new_space;
	ob.buffer_curr=new_space+ptr_diff;
	ob.buffer_end=new_space+size;
}

template<std::integral char_type>
inline constexpr void dynamic_io_buffer_grow_with_new_size(dynamic_io_buffer<char_type>& ob,std::size_t new_size) noexcept
{
	std::size_t new_capacity{static_cast<std::size_t>(ob.buffer_end-ob.buffer_begin)};
	constexpr std::size_t cap_max{SIZE_MAX/2}; 
	if(new_capacity>cap_max)
		new_capacity=SIZE_MAX;
	else
		new_capacity*=2;
	if(new_capacity<new_size)
		new_capacity=new_size;
	dynamic_io_buffer_oreallocate_impl(ob,new_capacity);
}

template<std::integral char_type>
inline constexpr void dynamic_io_buffer_overflow_impl(dynamic_io_buffer<char_type>& ob,char_type ch) noexcept
{
	std::size_t new_capacity{static_cast<std::size_t>(ob.buffer_end-ob.buffer_begin)};
	constexpr std::size_t cap_max{SIZE_MAX/2}; 
	if(new_capacity>cap_max)
		new_capacity=SIZE_MAX;
	else
		new_capacity*=2;
	dynamic_io_buffer_oreallocate_impl(ob,new_capacity);
	*ob.buffer_curr=ch;
	++ob.buffer_curr;
}

template<std::integral char_type,::std::forward_iterator Iter>
inline constexpr void dynamic_io_buffer_write_impl_unhappy_iter(dynamic_io_buffer<char_type>& ob,
	Iter first,std::size_t diff) noexcept
{
	dynamic_io_buffer_grow_with_new_size(ob,ob.buffer_end-ob.buffer_begin+diff);
	ob.buffer_curr=non_overlapped_copy_n(first,diff,ob.buffer_curr);
}

template<::std::forward_iterator Iter>
inline constexpr void dynamic_io_buffer_write_impl_unhappy(dynamic_io_buffer<::std::iter_value_t<Iter>>& ob,
	Iter first,std::size_t diff) noexcept
{
	if constexpr(::std::contiguous_iterator<Iter>)
	{
		auto const * ptr{::std::to_address(first)};
		dynamic_io_buffer_write_impl_unhappy_iter(ob,ptr,diff);
	}
	else
		dynamic_io_buffer_write_impl_unhappy_iter(ob,first,diff);
}

}
template<std::integral ch_type>
inline constexpr void oreserve(dynamic_io_buffer<ch_type>& ob,std::size_t new_capacity)  noexcept
{
	if(static_cast<std::size_t>(ob.buffer_end-ob.buffer_begin)<=new_capacity)
		return;
	details::dynamic_io_buffer_oreallocate_impl(ob,new_capacity);
}

template<std::integral ch_type>
inline constexpr void oshrink_to_fit(dynamic_io_buffer<ch_type>& ob)  noexcept
{
	if(ob.buffer_curr==ob.buffer_end)
		return;
	details::dynamic_io_buffer_oreallocate_impl(ob,ob.buffer_curr-ob.buffer_begin);
}

template<std::integral ch_type>
inline constexpr void obuffer_overflow(dynamic_io_buffer<ch_type>& ob,ch_type ch) noexcept
{
	details::dynamic_io_buffer_overflow_impl(ob,ch);
}

template<std::integral ch_type,::std::forward_iterator Iter>
requires ((std::same_as<ch_type,char>&&::std::contiguous_iterator<Iter>)||
	std::same_as<ch_type,::std::iter_value_t<Iter>>)
inline constexpr void write(dynamic_io_buffer<ch_type>& ob,Iter first,Iter last) noexcept
{
	if constexpr(!std::same_as<ch_type,::std::iter_value_t<Iter>>)
	{
		write(ob,
		reinterpret_cast<char const*>(::std::to_address(first)),
		reinterpret_cast<char const*>(::std::to_address(last)));
	}
	else if constexpr(::std::contiguous_iterator<Iter>&&!std::is_pointer_v<std::remove_cvref_t<Iter>>)
	{
		write(ob,::std::to_address(first),::std::to_address(last));
	}
	else
	{
		std::size_t diff{static_cast<std::size_t>(last-first)};
		std::size_t remain_space{static_cast<std::size_t>(ob.buffer_end-ob.buffer_curr)};
		if(remain_space<diff)
		{
			details::dynamic_io_buffer_write_impl_unhappy(ob,first,diff);
			return;
		}
		ob.buffer_curr=details::non_overlapped_copy(first,last,ob.buffer_begin);
	}
}

}
