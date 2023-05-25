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
	char_type *begin_ptr = typed_allocator_type::allocate(buffersize);
	char_type *curr_ptr = non_overlapped_copy(first,last,begin);
	pointers.begin_ptr=begin_ptr;
	pointers.curr_ptr=curr_ptr;
	pointers.end_ptr=begin_ptr+buffersize;
}

template<::std::integral char_type,
	typename optstmtype>
inline constexpr char_type const* write_some_nullptr_case(
	optstmtype optstm,
	basic_io_buffer_pointers<char_type>& __restrict pointers,
	char_type const *first,char_type const *last)
{
	basic_io_scatter_t<char_type> const scatters[2]
	{
	{pointers.begin_ptr,static_cast<::std::size_t>(pointers.curr_ptr-pointers.begin_ptr)},
	{first,static_cast<::std::size_t>(last-first)}
	};
	auto status{::fast_io::operations::decay::scatter_write_some(pointers,scatters,2)};
	auto position{status.position};
	if(position==2)
	{
		pointers.curr_ptr=pointers.begin_ptr;
		return last;
	}
	else if(position==1)
	{
		pointers.curr_ptr=pointers.begin_ptr;
		return first+status.position_in_scatter;
	}
	else
	{
		pointers.curr_ptr=::fast_io::freestanding::copy_n(pointers.curr_ptr-status.position_in_scatter,
			status.position_in_scatter,pointers.begin_ptr);
		return first;
	}
}

template<::std::integral char_type,
	typename allocator_type,
	::std::size_t buffersize,
	typename optstmtype>
inline constexpr char_type const* write_some_overflow_impl(
	optstmtype optstm,
	basic_io_buffer_pointers<char_type>& pointers,
	char_type const* first, char_type const* last)
{
	constexpr
		::std::size_t buffersize{io_buffer_type::buffer_size};
	std::size_t const diff{static_cast<std::size_t>(last-first)};
	if(pointers.buffer_begin==nullptr)
	{
		if(diff<buffer_size)
		{
			write_nullptr_case<char_type,allocator_type,buffersize>(pointers,first,last);
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
	optstmtype optstm,
	basic_io_buffer_pointers<char_type>& __restrict pointers,
	char_type const *first,char_type const *last)
{
	basic_io_scatter_t<char_type> const scatters[2]
	{
	{pointers.begin_ptr,static_cast<::std::size_t>(pointers.curr_ptr-pointers.begin_ptr)},
	{first,static_cast<::std::size_t>(last-first)}
	};
	::fast_io::operations::decay::scatter_write_all(pointers,scatters,2);
	pointers.curr_ptr=pointers.begin_ptr;
}

template<::std::integral char_type,
	typename allocator_type,
	::std::size_t buffersize,
	typename optstmtype>
inline constexpr void write_all_overflow_impl(
	optstmtype optstm,
	basic_io_buffer_pointers<char_type>& pointers,
	char_type const* first, char_type const* last)
{
	constexpr
		::std::size_t buffersize{io_buffer_type::buffer_size};
	std::size_t const diff{static_cast<std::size_t>(last-first)};
	if(pointers.buffer_begin==nullptr)
	{
		if(diff<buffer_size)
		{
			write_nullptr_case<char_type,allocator_type,buffersize>(pointers,first,last);
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
	if(pointers.begin_ptr==pointers.curr_ptr)
	{
		return;
	}
	::fast_io::operations::decay::write_all_decay(optstm,pointers.begin_ptr,pointers.curr_ptr);
	pointers.curr_ptr=pointers.begin_ptr;
}

}

template<typename io_buffer_type>
inline constexpr typename io_buffer_type::output_char_type const* write_some_overflow_define(
	basic_io_buffer_ref<io_buffer_type> ref,
	typename io_buffer_type::output_char_type const* first,
	typename io_buffer_type::output_char_type const* last)
{
	return ::fast_io::details::io_buffer::write_some_overflow_impl<
		typename io_buffer_type::output_char_type,
		typename io_buffer_type::allocator_type,
		io_buffer_type::buffer_size>(::fast_io::manipulators::output_stream_ref(ref.iobptr),first,last);
}

template<typename io_buffer_type>
inline constexpr void write_all_overflow_define(
	basic_io_buffer_ref<io_buffer_type> ref,
	typename io_buffer_type::output_char_type const* first,
	typename io_buffer_type::output_char_type const* last)
{
	return ::fast_io::details::io_buffer::write_all_overflow_impl<
		typename io_buffer_type::output_char_type,
		typename io_buffer_type::allocator_type,
		io_buffer_type::buffer_size>(::fast_io::manipulators::output_stream_ref(ref.iobptr),first,last);
}


template<typename io_buffer_type>
inline constexpr void output_stream_buffer_flush_define(
	basic_io_buffer_ref<io_buffer_type> ref)
{
	::fast_io::details::io_buffer::output_stream_buffer_flush_define_impl<
		typename io_buffer_type::output_char_type>(::fast_io::manipulators::output_stream_ref(ref.iobptr));
}

}
