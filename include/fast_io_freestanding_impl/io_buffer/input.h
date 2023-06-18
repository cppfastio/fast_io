#pragma once

namespace fast_io
{

namespace details::io_buffer
{

template<typename allocator_type,std::integral char_type,typename instmtype>
inline constexpr bool ibuffer_underflow_rl_size_impl(instmtype insm,
	basic_io_buffer_pointers<char_type>& ibuffer,::std::size_t bfsz)
{
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type,char_type>;
	if(ibuffer.buffer_begin==nullptr)
		ibuffer.buffer_end=ibuffer.buffer_curr=ibuffer.buffer_begin=typed_allocator_type::allocate(bfsz);
	ibuffer.buffer_end=::fast_io::operations::decay::read_until_eof_decay(insm,ibuffer.buffer_begin,ibuffer.buffer_begin+bfsz);
	ibuffer.buffer_curr=ibuffer.buffer_begin;
	return ibuffer.buffer_begin!=ibuffer.buffer_end;
}

template<::std::size_t bfsz,std::integral char_type,typename allocator_type,typename instmtype>
inline constexpr bool ibuffer_underflow_rl_impl(instmtype insm,
	basic_io_buffer_pointers<char_type>& ibuffer)
{
	return ::fast_io::details::io_buffer::ibuffer_underflow_rl_size_impl<allocator_type>(insm,ibuffer,bfsz);
}

template<typename allocator_type,std::integral char_type,typename instmtype>
inline constexpr void ibuffer_minimum_size_underflow_all_prepare_rl_size_impl(instmtype insm,
	basic_io_buffer_pointers<char_type>& ibuffer,::std::size_t bfsz)
{
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type,char_type>;
	if(ibuffer.buffer_begin==nullptr)
		ibuffer.buffer_end=ibuffer.buffer_curr=ibuffer.buffer_begin=typed_allocator_type::allocate(bfsz);
	auto bg{ibuffer.buffer_begin};
	auto ed{bg+bfsz};
	::fast_io::operations::decay::read_all_decay(insm,bg,ed);
	ibuffer.buffer_curr=bg;
	ibuffer.buffer_end=ed;
}

template<::std::size_t bfsz,std::integral char_type,typename allocator_type,typename instmtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void ibuffer_minimum_size_underflow_all_prepare_impl(instmtype insm,
	basic_io_buffer_pointers<char_type>& ibuffer)
{
	::fast_io::details::io_buffer::ibuffer_minimum_size_underflow_all_prepare_rl_size_impl<allocator_type>(insm,ibuffer,bfsz);
}

template<typename allocator_type,::std::integral char_type,
	typename instmtype>
inline constexpr char_type* read_until_eof_underflow_size_impl(
	instmtype instm,
	basic_io_buffer_pointers<char_type>& __restrict pointers,
	char_type *first,char_type *last,::std::size_t bfsz)
{
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type,char_type>;
	first=::fast_io::details::non_overlapped_copy(pointers.buffer_curr,pointers.buffer_end,first);
	if(pointers.buffer_begin==nullptr)
	{
		pointers.buffer_end=pointers.buffer_curr=pointers.buffer_begin=typed_allocator_type::allocate(bfsz);
	}
	if constexpr(::fast_io::operations::decay::defines::has_any_of_read_bytes_operations<instmtype>)
	{
		::fast_io::io_scatter_t scatters[2]
		{
		{first,static_cast<::std::size_t>(
			reinterpret_cast<::std::byte const*>(last)-
			reinterpret_cast<::std::byte const*>(first))},
		{pointers.buffer_begin,bfsz*sizeof(char_type)}
		};
		auto [pos,scpos]{::fast_io::operations::decay::scatter_read_until_eof_bytes_decay(instm,scatters,2)};
		if(pos==2)
		{
			pointers.buffer_end=(pointers.buffer_curr=pointers.buffer_begin)+bfsz;
			return last;
		}
		else if(pos==1)
		{
			pointers.buffer_end=(pointers.buffer_curr=pointers.buffer_begin)+(scpos/sizeof(char_type));
			return last;
		}
		else
		{
			pointers.buffer_end=pointers.buffer_curr=pointers.buffer_begin;
			return first+scpos/sizeof(char_type);
		}
	}
	else
	{
		basic_io_scatter_t<char_type> scatters[2]
		{
		{first,static_cast<::std::size_t>(last-first)},
		{pointers.buffer_begin,bfsz}
		};
		auto [pos,scpos]{::fast_io::operations::decay::scatter_read_until_eof_decay(instm,scatters,2)};
		if(pos==2)
		{
			pointers.buffer_end=(pointers.buffer_curr=pointers.buffer_begin)+bfsz;
			return last;
		}
		else if(pos==1)
		{
			pointers.buffer_end=(pointers.buffer_curr=pointers.buffer_begin)+scpos;
			return last;
		}
		else
		{
			pointers.buffer_end=pointers.buffer_curr=pointers.buffer_begin;
			return first+scpos;
		}
	}
}

template<::std::size_t bfsz,typename allocatortype,::std::integral char_type,typename instmtype>
inline constexpr char_type* read_until_eof_underflow_impl(
	instmtype instm,
	basic_io_buffer_pointers<char_type>& __restrict pointers,
	char_type *first,char_type *last)
{
	return ::fast_io::details::io_buffer::read_until_eof_underflow_size_impl<allocatortype>(instm,pointers,first,last,bfsz);
}

}

template<typename io_buffer_type,::std::integral char_type>
inline constexpr char_type* read_until_eof_underflow_define(
	basic_io_buffer_ref<io_buffer_type> iobref,
	char_type *first,char_type *last)
{
	constexpr auto mode{io_buffer_type::traits_type::mode};
	if constexpr((mode&buffer_mode::out)==buffer_mode::out&&
		(mode&buffer_mode::tie)==buffer_mode::tie)
	{
		output_stream_buffer_flush_define(iobref);
	}
	return ::fast_io::details::io_buffer::read_until_eof_underflow_impl<
		io_buffer_type::traits_type::input_buffer_size,
		typename io_buffer_type::traits_type::allocator_type>(
		::fast_io::operations::input_stream_ref(iobref.iobptr->handle),
		iobref.iobptr->input_buffer,first,last);
}

template<typename io_buffer_type>
inline constexpr bool ibuffer_underflow(basic_io_buffer_ref<io_buffer_type> iobref)
{
	constexpr auto mode{io_buffer_type::traits_type::mode};
	if constexpr((mode&buffer_mode::out)==buffer_mode::out&&
		(mode&buffer_mode::tie)==buffer_mode::tie)
	{
		output_stream_buffer_flush_define(iobref);
	}
	return ::fast_io::details::io_buffer::ibuffer_underflow_rl_impl<
		io_buffer_type::traits_type::input_buffer_size,
		typename io_buffer_type::traits_type::input_char_type,
		typename io_buffer_type::traits_type::allocator_type>(
		::fast_io::operations::input_stream_ref(iobref.iobptr->handle),
		iobref.iobptr->input_buffer);
}

template<typename io_buffer_type>
inline constexpr auto ibuffer_begin(basic_io_buffer_ref<io_buffer_type> iobref) noexcept
{
	return iobref.iobptr->input_buffer.buffer_begin;
}

template<typename io_buffer_type>
inline constexpr auto ibuffer_curr(basic_io_buffer_ref<io_buffer_type> iobref) noexcept
{
	return iobref.iobptr->input_buffer.buffer_curr;
}

template<typename io_buffer_type>
inline constexpr auto ibuffer_end(basic_io_buffer_ref<io_buffer_type> iobref) noexcept
{
	return iobref.iobptr->input_buffer.buffer_end;
}

template<typename io_buffer_type>
inline constexpr void ibuffer_set_curr(basic_io_buffer_ref<io_buffer_type> iobref,
	typename basic_io_buffer_ref<io_buffer_type>::input_char_type* ptr) noexcept
{
	iobref.iobptr->input_buffer.buffer_curr = ptr;
}

template<::std::integral char_type,typename io_buffer_type>
requires (::std::same_as<char_type,typename basic_io_buffer_ref<io_buffer_type>::input_char_type>)
inline constexpr ::std::size_t ibuffer_minimum_size_define(::fast_io::io_reserve_type_t<char_type,
	basic_io_buffer_ref<io_buffer_type>>)
{
	return io_buffer_type::traits_type::input_buffer_size;
}

template<typename io_buffer_type>
inline constexpr void ibuffer_minimum_size_underflow_all_prepare_define(basic_io_buffer_ref<io_buffer_type> iobref)
{
	::fast_io::details::io_buffer::ibuffer_minimum_size_underflow_all_prepare_impl<
		io_buffer_type::traits_type::input_buffer_size,
		typename io_buffer_type::traits_type::input_char_type,
		typename io_buffer_type::traits_type::allocator_type>(
			::fast_io::operations::input_stream_ref(iobref.iobptr->handle),
			iobref.iobptr->input_buffer);

}

}
