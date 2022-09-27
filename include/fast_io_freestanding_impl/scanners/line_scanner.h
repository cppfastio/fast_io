#pragma once

namespace fast_io
{

template<std::integral char_type>
struct basic_line_scanner_buffer
{
	struct buffer_type
	{
		char_type *begin_ptr{};
		char_type *curr_ptr{};
		char_type *end_ptr{};	
		constexpr buffer_type() noexcept:begin_ptr(nullptr),curr_ptr(nullptr),end_ptr(nullptr)
		{}
		buffer_type(buffer_type const&)=delete;
		buffer_type& operator=(buffer_type const&)=delete;
		constexpr buffer_type(buffer_type&& __restrict other) noexcept:begin_ptr(other.begin_ptr),curr_ptr(other.curr_ptr),end_ptr(other.end_ptr)
		{
			other.end_ptr=other.curr_ptr=other.begin_ptr=nullptr;
		}
		constexpr buffer_type& operator=(buffer_type&& __restrict other) noexcept
		{
			::fast_io::details::deallocate_iobuf_space<false,char_type>(this->begin_ptr,static_cast<std::size_t>(this->end_ptr-this->begin_ptr));
			this->begin_ptr=other.begin_ptr;
			this->curr_ptr=other.curr_ptr;
			this->end_ptr=other.end_ptr;
			other.end_ptr=other.curr_ptr=other.begin_ptr=nullptr;
		}
		constexpr ~buffer_type()
		{
			::fast_io::details::deallocate_iobuf_space<false,char_type>(this->begin_ptr,static_cast<std::size_t>(this->end_ptr-this->begin_ptr));
		}
	};
	buffer_type buffer;
	char_type const* view_begin_ptr{};
	char_type const* view_end_ptr{};
	bool inbuffer{};
	constexpr char_type const* begin() const noexcept
	{
		return view_begin_ptr;
	}
	constexpr char_type const* end() const noexcept
	{
		return view_end_ptr;
	}
};

template<std::integral char_type>
struct basic_line_scanner_contiguous_view
{
	char_type const* view_begin_ptr{};
	char_type const* view_end_ptr{};
	constexpr char_type const* begin() const noexcept
	{
		return view_begin_ptr;
	}
	constexpr char_type const* end() const noexcept
	{
		return view_end_ptr;
	}
};

namespace details
{

template<std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void copy_to_next_line_buffer_internal_impl(basic_line_scanner_buffer<char_type>& __restrict buf,char_type const* first,std::size_t sz) noexcept
{
	auto bg_ptr{buf.buffer.begin_ptr};
	std::size_t const elements{static_cast<std::size_t>(buf.buffer.curr_ptr-bg_ptr)};
	std::size_t const elementswithnewbuff{elements+sz};
	std::size_t const old_capacity{static_cast<std::size_t>(buf.buffer.end_ptr-bg_ptr)};
	constexpr std::size_t szmx{std::numeric_limits<std::size_t>::max()/sizeof(char_type)};
	constexpr std::size_t szmxhalf{szmx/static_cast<std::size_t>(2u)};
	std::size_t new_capacity;
	if(old_capacity>=szmxhalf)
	{
		new_capacity=szmx;
	}
	else
	{
		new_capacity=old_capacity;
		new_capacity<<=1u;
	}
	if(new_capacity<elementswithnewbuff)
	{
		new_capacity=elementswithnewbuff;
	}
	auto new_begin_ptr{::fast_io::details::allocate_iobuf_space<char_type>(new_capacity)};
	auto new_curr_ptr{non_overlapped_copy_n(bg_ptr,elements,new_begin_ptr)};
	new_curr_ptr=non_overlapped_copy_n(first,sz,new_curr_ptr);
	::fast_io::details::deallocate_iobuf_space<false,char_type>(bg_ptr,old_capacity);
	buf.buffer.begin_ptr=new_begin_ptr;
	buf.buffer.curr_ptr=new_curr_ptr;
	buf.buffer.end_ptr=new_begin_ptr+new_capacity;
}

template<std::integral char_type>
inline constexpr void copy_to_next_line_buffer_impl(basic_line_scanner_buffer<char_type>& __restrict buf,char_type const* first,char_type const* last) noexcept
{
	auto curr_ptr{buf.buffer.curr_ptr};
	std::size_t bfsz{static_cast<std::size_t>(buf.buffer.end_ptr-curr_ptr)};
	std::size_t diff{static_cast<std::size_t>(last-first)};
	if(bfsz<diff)[[unlikely]]
	{
		copy_to_next_line_buffer_internal_impl(buf,first,diff);
		return;
	}
	buf.buffer.curr_ptr=non_overlapped_copy_n(first,diff,curr_ptr);
}

template<std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr parse_result<char_type const*> scan_iterative_next_line_define_partial(basic_line_scanner_buffer<char_type>& __restrict buf,char_type const* first,char_type const* last) noexcept
{
	if(!buf.inbuffer)
	{
		buf.inbuffer=true;
		buf.buffer.curr_ptr=buf.buffer.begin_ptr;
	}
	copy_to_next_line_buffer_impl(buf,first,last);
	return {last,::fast_io::parse_code::partial};
}

template<std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr parse_result<char_type const*> scan_iterative_next_line_define_inbuffer(basic_line_scanner_buffer<char_type>& __restrict buf,char_type const* first,char_type const* it) noexcept
{
	copy_to_next_line_buffer_impl(buf,first,it);
	buf.view_begin_ptr=buf.buffer.begin_ptr;
	buf.view_end_ptr=buf.buffer.curr_ptr;
	return {it+1,::fast_io::parse_code::ok};
}

template<std::integral char_type>
inline constexpr parse_result<char_type const*> scan_iterative_next_line_define_impl(
	basic_line_scanner_buffer<char_type>& __restrict buf,char_type const* first,char_type const* last) noexcept
{
	auto it{::fast_io::find_lf(first,last)};
	if(it==last)[[unlikely]]
	{
		return scan_iterative_next_line_define_partial(buf,first,last);
	}
	if(buf.inbuffer)[[unlikely]]
	{
		return scan_iterative_next_line_define_inbuffer(buf,first,it);
	}
	buf.view_begin_ptr=first;
	buf.view_end_ptr=it;
	return {it+1,::fast_io::parse_code::ok};
}

template<std::integral char_type>
inline constexpr parse_code scan_iterative_eof_define_line_internal(basic_line_scanner_buffer<char_type>& __restrict buf) noexcept
{
	if(buf.inbuffer)
	{
		buf.view_begin_ptr=buf.buffer.begin_ptr;
		buf.view_end_ptr=buf.buffer.curr_ptr;
		return parse_code::ok;
	}
	else
	{
		buf.view_end_ptr=buf.view_begin_ptr=nullptr;
		return parse_code::end_of_file;
	}
}

template<std::integral char_type>
inline constexpr parse_result<char_type const*> scan_iterative_contiguous_line_define_impl(
	basic_line_scanner_contiguous_view<char_type>& __restrict buf,char_type const* first,char_type const* last) noexcept
{
	auto it{::fast_io::find_lf(first,last)};
	if(it==last)[[unlikely]]
	{
		if(first==it)
		{
			return {it,parse_code::end_of_file};
		}
		return {it,parse_code::ok};
	}
	buf.view_begin_ptr=first;
	buf.view_end_ptr=it;
	return {it+1,parse_code::ok};
}

}

template<std::integral char_type>
inline constexpr void scan_iterative_init_define(io_reserve_type_t<char_type,basic_line_scanner_buffer<char_type>>,
		basic_line_scanner_buffer<char_type>& __restrict buf) noexcept
{
	buf.inbuffer=false;	
}

template<std::integral char_type>
inline constexpr parse_result<char_type const*> scan_iterative_next_define(io_reserve_type_t<char_type,basic_line_scanner_buffer<char_type>>,
	basic_line_scanner_buffer<char_type>& __restrict buf,char_type const* first,char_type const* last) noexcept
{
	return ::fast_io::details::scan_iterative_next_line_define_impl(buf,first,last);
}

template<std::integral char_type>
inline constexpr parse_code scan_iterative_eof_define(io_reserve_type_t<char_type,basic_line_scanner_buffer<char_type>>,
	basic_line_scanner_buffer<char_type>& __restrict buf) noexcept
{
	return ::fast_io::details::scan_iterative_eof_define_line_internal(buf);
}

template<std::integral char_type>
inline constexpr parse_result<char_type const*> scan_iterative_contiguous_define(io_reserve_type_t<char_type,basic_line_scanner_contiguous_view<char_type>>,
	basic_line_scanner_contiguous_view<char_type>& __restrict buf,char_type const* first,char_type const* last) noexcept
{
	return ::fast_io::details::scan_iterative_contiguous_line_define_impl(buf,first,last);
}

template<input_stream input>
inline constexpr auto line_scanner(input&& in) noexcept(noexcept(io_ref(in)))
{
	using char_type = typename std::remove_cvref_t<input>::char_type;
	if constexpr(mutex_stream<input>)
	{
		return basic_scanner_context_mutex<decltype(io_ref(in)),basic_line_scanner_buffer<char_type>>(io_ref(in));
	}
	else if constexpr(contiguous_input_stream<input>)
	{
		return basic_scanner_context<decltype(io_ref(in)),basic_line_scanner_contiguous_view<char_type>>{io_ref(in)};
	}
	else
	{
		return basic_scanner_context<decltype(io_ref(in)),basic_line_scanner_buffer<char_type>>{io_ref(in)};
	}
}

}
