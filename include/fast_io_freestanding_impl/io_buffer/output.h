#pragma once

namespace fast_io
{

namespace details::io_buffer
{

template<::std::integral char_type,
	typename allocator_type,
	::std::size_t buffersize>
inline constexpr void write_nullptr_case(
	basic_io_buffer_pointers<char_type>& __restrict pointers,
	char_type const *first,char_type const *last)
{
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type,char_type>;
	char_type *buffer_begin = typed_allocator_type::allocate(buffersize);
	char_type *buffer_curr = non_overlapped_copy(first,last,buffer_begin);
	pointers.buffer_begin=buffer_begin;
	pointers.buffer_curr=buffer_curr;
	pointers.buffer_end=buffer_begin+buffersize;
}

template<::std::integral char_type,
	typename optstmtype>
inline constexpr char_type const* write_some_typical_case(
	optstmtype optstm,
	basic_io_buffer_pointers<char_type>& __restrict pointers,
	char_type const *first,char_type const *last)
{
	if constexpr(::fast_io::operations::decay::defines::has_any_of_write_or_seek_pwrite_bytes_operations<optstmtype>)
	{
		io_scatter_t const scatters[2]
		{
		{pointers.buffer_begin,static_cast<::std::size_t>(reinterpret_cast<::std::byte const*>(pointers.buffer_curr)-
			reinterpret_cast<::std::byte const*>(pointers.buffer_begin))},
		{first,static_cast<::std::size_t>(reinterpret_cast<::std::byte const*>(last)-
			reinterpret_cast<::std::byte const*>(first))}
		};
		auto [position,scpos]{::fast_io::operations::decay::scatter_write_some_bytes_decay(optstm,scatters,2)};
		if(position==2)
		{
			pointers.buffer_curr=pointers.buffer_begin;
			return last;
		}
		else if(position==1)
		{
			pointers.buffer_curr=pointers.buffer_begin;
			return first+scpos/sizeof(char_type);
		}
		else
		{
			scpos/=sizeof(char_type);
			pointers.buffer_curr=::fast_io::freestanding::copy_n(pointers.buffer_curr-scpos,
				scpos,pointers.buffer_begin);
			return first;
		}
	}
	else
	{
		basic_io_scatter_t<char_type> const scatters[2]
		{
		{pointers.buffer_begin,static_cast<::std::size_t>(pointers.buffer_curr-pointers.buffer_begin)},
		{first,static_cast<::std::size_t>(last-first)}
		};
		auto [position,scpos]{::fast_io::operations::decay::scatter_write_some_decay(optstm,scatters,2)};
		if(position==2)
		{
			pointers.buffer_curr=pointers.buffer_begin;
			return last;
		}
		else if(position==1)
		{
			pointers.buffer_curr=pointers.buffer_begin;
			return first+scpos;
		}
		else
		{
			pointers.buffer_curr=::fast_io::freestanding::copy_n(pointers.buffer_curr-scpos,
				scpos,pointers.buffer_begin);
			return first;
		}
	}
}

template<::std::integral char_type,
	typename optstmtype>
inline constexpr void write_all_typical_case(
	optstmtype optstm,
	basic_io_buffer_pointers<char_type>& __restrict pointers,
	char_type const *first,char_type const *last)
{
	if constexpr(::fast_io::operations::decay::defines::has_any_of_write_bytes_operations<optstmtype>)
	{
		io_scatter_t const scatters[2]
		{
		{pointers.buffer_begin,static_cast<::std::size_t>(reinterpret_cast<::std::byte const*>(pointers.buffer_curr)-
			reinterpret_cast<::std::byte const*>(pointers.buffer_begin))},
		{first,static_cast<::std::size_t>(reinterpret_cast<::std::byte const*>(last)-
			reinterpret_cast<::std::byte const*>(first))}
		};
		::fast_io::operations::decay::scatter_write_all_bytes_decay(optstm,scatters,2);
	}
	else
	{
		basic_io_scatter_t<char_type> const scatters[2]
		{
		{pointers.buffer_begin,static_cast<::std::size_t>(pointers.buffer_curr-pointers.buffer_begin)},
		{first,static_cast<::std::size_t>(last-first)}
		};
		::fast_io::operations::decay::scatter_write_all_decay(optstm,scatters,2);
	}
	pointers.buffer_curr=pointers.buffer_begin;
}

template<::std::integral char_type,
	typename allocator_type,
	::std::size_t buffer_size,
	typename optstmtype>
inline constexpr char_type const* write_some_overflow_impl(
	optstmtype optstm,
	basic_io_buffer_pointers<char_type>& pointers,
	char_type const* first, char_type const* last)
{
	std::size_t const diff{static_cast<std::size_t>(last-first)};
	if(pointers.buffer_begin==nullptr)
	{
		if(diff<buffer_size)
		{
			write_nullptr_case<char_type,allocator_type,buffer_size>(pointers,first,last);
			return last;
		}
		else
		{
			return ::fast_io::operations::decay::write_some_decay(optstm,first,last);
		}
	}
	return write_some_typical_case<char_type>(optstm,pointers,first,last);
}

template<::std::integral char_type,
	typename optstmtype>
inline constexpr void write_all_nullptr_case(
	basic_io_buffer_pointers<char_type>& __restrict pointers,
	char_type const *first,char_type const *last)
{
	basic_io_scatter_t<char_type> const scatters[2]
	{
	{(pointers.buffer_begin?pointers.buffer_begin:first),static_cast<::std::size_t>(pointers.buffer_curr-pointers.buffer_begin)},
	{first,static_cast<::std::size_t>(last-first)}
	};
	::fast_io::operations::decay::scatter_write_all_decay(pointers,scatters,2);
	pointers.buffer_curr=pointers.buffer_begin;
}

template<::std::integral char_type,
	typename allocator_type,
	::std::size_t buffer_size,
	typename optstmtype>
inline constexpr void write_all_overflow_impl(
	optstmtype optstm,
	basic_io_buffer_pointers<char_type>& pointers,
	char_type const* first, char_type const* last)
{
	std::size_t const diff{static_cast<std::size_t>(last-first)};
	if(pointers.buffer_begin==nullptr)
	{
		if(diff<buffer_size)
		{
			write_nullptr_case<char_type,allocator_type,buffer_size>(pointers,first,last);
		}
		else
		{
			::fast_io::operations::decay::write_all_decay(optstm,first,last);
		}
		return;
	}
	write_all_typical_case<char_type>(optstm,pointers,first,last);
}

template<::std::integral char_type,typename optstmtype>
inline constexpr void output_stream_buffer_flush_impl(optstmtype optstm,
	basic_io_buffer_pointers<char_type>& pointers)
{
	if(pointers.buffer_begin==pointers.buffer_curr)
	{
		return;
	}
	::fast_io::operations::decay::write_all_decay(optstm,pointers.buffer_begin,pointers.buffer_curr);
	pointers.buffer_curr=pointers.buffer_begin;
}

template<::std::integral char_type,
	typename allocator_type,
	::std::size_t buffer_size,
	typename optstmtype>
inline constexpr void obuffer_minimum_size_flush_prepare_impl(optstmtype optstm,
	basic_io_buffer_pointers<char_type>& pointers)
{
	if(pointers.buffer_begin==pointers.buffer_curr)
	{
		if(pointers.buffer_begin==nullptr)
		{
			using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type,char_type>;
			pointers.buffer_end=((pointers.buffer_curr=
				pointers.buffer_begin=typed_allocator_type::allocate(buffer_size))+buffer_size);
		}
	}
	else
	{
		::fast_io::operations::decay::write_all_decay(optstm,pointers.buffer_begin,pointers.buffer_curr);
		pointers.buffer_curr=pointers.buffer_begin;
	}
}


}

template<typename io_buffer_type>
inline constexpr typename io_buffer_type::output_char_type const* write_some_overflow_define(
	basic_io_buffer_ref<io_buffer_type> iobref,
	typename io_buffer_type::output_char_type const* first,
	typename io_buffer_type::output_char_type const* last)
{
	return ::fast_io::details::io_buffer::write_some_overflow_impl<
		typename io_buffer_type::output_char_type,
		typename io_buffer_type::traits_type::allocator_type,
		io_buffer_type::traits_type::output_buffer_size>(::fast_io::operations::output_stream_ref(iobref.iobptr->handle),
			iobref.iobptr->output_buffer,first,last);
}

template<typename io_buffer_type>
inline constexpr void write_all_overflow_define(
	basic_io_buffer_ref<io_buffer_type> iobref,
	typename io_buffer_type::output_char_type const* first,
	typename io_buffer_type::output_char_type const* last)
{
	return ::fast_io::details::io_buffer::write_all_overflow_impl<
		typename io_buffer_type::output_char_type,
		typename io_buffer_type::traits_type::allocator_type,
		io_buffer_type::traits_type::output_buffer_size>(::fast_io::operations::output_stream_ref(iobref.iobptr->handle),
			iobref.iobptr->output_buffer,first,last);
}

template<typename io_buffer_type>
inline constexpr void output_stream_buffer_flush_define(
	basic_io_buffer_ref<io_buffer_type> iobref)
{
	::fast_io::details::io_buffer::output_stream_buffer_flush_impl<
		typename io_buffer_type::output_char_type>(::fast_io::operations::output_stream_ref(iobref.iobptr->handle),
		iobref.iobptr->output_buffer);
}



template<typename io_buffer_type>
inline constexpr auto obuffer_begin(basic_io_buffer_ref<io_buffer_type> iobref) noexcept
{
	return iobref.iobptr->output_buffer.buffer_begin;
}

template<typename io_buffer_type>
inline constexpr auto obuffer_curr(basic_io_buffer_ref<io_buffer_type> iobref) noexcept
{
	return iobref.iobptr->output_buffer.buffer_curr;
}

template<typename io_buffer_type>
inline constexpr auto obuffer_end(basic_io_buffer_ref<io_buffer_type> iobref) noexcept
{
	return iobref.iobptr->output_buffer.buffer_end;
}

template<typename io_buffer_type>
inline constexpr void obuffer_set_curr(basic_io_buffer_ref<io_buffer_type> iobref,
	typename basic_io_buffer_ref<io_buffer_type>::output_char_type* ptr) noexcept
{
	iobref.iobptr->output_buffer.buffer_curr = ptr;
}

template<::std::integral char_type,typename io_buffer_type>
requires (::std::same_as<char_type,typename basic_io_buffer_ref<io_buffer_type>::output_char_type>)
inline constexpr ::std::size_t obuffer_minimum_size_define(::fast_io::io_reserve_type_t<char_type,basic_io_buffer_ref<io_buffer_type>>)
{
	return io_buffer_type::traits_type::output_buffer_size;
}

template<typename io_buffer_type>
inline constexpr void obuffer_minimum_size_flush_prepare_define(basic_io_buffer_ref<io_buffer_type> iobref)
{
	::fast_io::details::io_buffer::obuffer_minimum_size_flush_prepare_impl<
		typename io_buffer_type::output_char_type,
		typename io_buffer_type::traits_type::allocator_type,
		io_buffer_type::traits_type::output_buffer_size>(
			::fast_io::operations::output_stream_ref(iobref.iobptr->handle),
			iobref.iobptr->output_buffer);

}

}
