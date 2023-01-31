#pragma once

namespace fast_io
{

template<std::integral ch_type,std::size_t buffer_size=4096u>
requires (buffer_size>=64u)
struct basic_http_header_buffer
{
	using char_type = ch_type;
	std::size_t header_length{};
	std::size_t http_request_end_location{};
	std::size_t http_status_code_start_location{};
	std::size_t http_status_code_end_location{};
	std::size_t http_status_reason_start_location{};
	std::size_t http_status_reason_end_location{};
	char_type buffer[buffer_size];
	inline static constexpr std::size_t size() noexcept
	{
		return buffer_size;
	}
	inline constexpr ::fast_io::manipulators::basic_os_str_known_size_without_null_terminated<char_type> request() const noexcept
	{
		return ::fast_io::manipulators::os_str_known_size_without_null_terminated<char_type>(buffer,buffer+http_request_end_location);
	}
	inline constexpr ::fast_io::manipulators::basic_os_str_known_size_without_null_terminated<char_type> code() const noexcept
	{
		return ::fast_io::manipulators::os_str_known_size_without_null_terminated<char_type>(buffer+http_status_code_start_location,buffer+http_status_code_end_location);
	}
	inline constexpr ::fast_io::manipulators::basic_os_str_known_size_without_null_terminated<char_type> reason() const noexcept
	{
		return ::fast_io::manipulators::os_str_known_size_without_null_terminated<char_type>(buffer+http_status_reason_start_location,buffer+http_status_reason_end_location);
	}
};

struct http_buffer_parse_context
{
	std::size_t state{};
};

template<std::integral char_type,std::size_t buffer_size>
inline constexpr auto scan_context_type(io_reserve_type_t<char_type,::fast_io::parameter<::fast_io::basic_http_header_buffer<char_type,buffer_size>&>>) noexcept
{
	return io_type_t<http_buffer_parse_context>{};
}

template<std::integral ch_type,std::size_t buffer_size>
inline constexpr basic_io_scatter_t<ch_type> print_alias_define(io_alias_t,basic_http_header_buffer<ch_type,buffer_size> const& b) noexcept
{
	return {b.buffer,b.header_length};
}

namespace details
{
template<std::integral ch_type,std::size_t buffer_size,::std::integral char_type>
inline bool try_copy_into_buffer(basic_http_header_buffer<ch_type,buffer_size>& b,char_type const* first,char_type const* last) noexcept
{
	std::size_t diff{static_cast<std::size_t>(last-first)};
	std::size_t remain_space{static_cast<std::size_t>(buffer_size-b.header_length)};
	if(remain_space<diff)
		return false;
	non_overlapped_copy_n(first,diff,b.buffer+b.header_length);
	b.header_length+=diff;
	return true;
}

template<std::integral ch_type,std::size_t buffer_size>
inline constexpr parse_code determine_http_header_location(basic_http_header_buffer<ch_type,buffer_size>& b) noexcept
{
	auto i{b.buffer},e{b.buffer+b.header_length};
	if(i!=e&&*i==char_literal_v<u8' ',ch_type>)
		return parse_code::invalid;
	{
	i=::fast_io::details::find_ch_impl<u8' ',false>(i,e);
	//for(;i!=e&&*i!=char_literal_v<u8' ',ch_type>;++i);
	if(i==e)
	{
		return parse_code::invalid;
	}
	b.http_request_end_location=static_cast<std::size_t>(i-b.buffer);
	}
	++i;
	i=::fast_io::details::find_ch_impl<u8' ',true>(i,e);
	//for(;i!=e&&*i==char_literal_v<u8' ',ch_type>;++i);
	if(i==e)
		return parse_code::invalid;
	b.http_status_code_start_location=static_cast<std::size_t>(i-b.buffer);
	{
	i=::fast_io::details::find_ch_impl<u8' ',false>(i,e);
	//for(;i!=e&&*i!=char_literal_v<u8' ',ch_type>;++i);
	if(i==e)
		return parse_code::invalid;
	b.http_status_code_end_location=static_cast<std::size_t>(i-b.buffer);
	}
	i=::fast_io::details::find_ch_impl<u8' ',true>(i,e);
	//for(;i!=e&&*i==char_literal_v<u8' ',ch_type>;++i);
	if(i==e)
		return parse_code::invalid;
	b.http_status_reason_start_location=static_cast<std::size_t>(i-b.buffer);
	i=::fast_io::details::find_ch_impl<u8'\r',false>(i,e);
	//for(;i!=e&&*i!=char_literal_v<u8'\r',ch_type>;++i);
	if(i==e)
		return parse_code::invalid;
	b.http_status_reason_end_location=static_cast<std::size_t>(i-b.buffer);
	return parse_code::ok;
}

template<::std::integral char_type,std::size_t buffer_size>
inline constexpr parse_result<char_type const*> http_header_scan_context_define_impl(std::size_t& state,char_type const* first1, char_type const* last,basic_http_header_buffer<char_type,buffer_size>& bufferref) noexcept
{
	auto first{first1};
	if(first==last)
	{
		return {first,parse_code::partial};
	}
	if(state!=0)
	{
		switch(state)
		{
		case 1:
		{
			if(*first!=char_literal_v<u8'\n',char_type>)
				break;
			++first;
			if(first==last)
			{
				state=2u;
				if(!::fast_io::details::try_copy_into_buffer(bufferref,first1,last))
					return {first,parse_code::invalid};
				return {first,parse_code::partial};
			}
			[[fallthrough]];
		}
		case 2:
		{
			if(*first!=char_literal_v<u8'\r',char_type>)
				break;
			++first;
			if(first==last)
			{
				state=3u;
				if(!::fast_io::details::try_copy_into_buffer(bufferref,first1,last))
					return {first,parse_code::invalid};
				return {first,parse_code::partial};
			}
			[[fallthrough]];
		}
		case 3:
		{
			if(*first!=char_literal_v<u8'\n',char_type>)
				break;
			++first;
			if(!::fast_io::details::try_copy_into_buffer(bufferref,first1,first))
				return {first,parse_code::invalid};
			return {first,::fast_io::details::determine_http_header_location(bufferref)};
		}
		}
	}
	state=0;
	for(;;)
	{
		first=::fast_io::details::find_ch_impl<u8'\r',false>(first,last);
		//for(;first!=last&&*first!=char_literal_v<u8'\r',char_type>;++first);
		if(first==last)
		{
			if(!::fast_io::details::try_copy_into_buffer(bufferref,first1,first))
				return {first,parse_code::invalid};
			state=0u;
			return {first,parse_code::partial};
		}
		++first;
		if(first==last)
		{
			if(!::fast_io::details::try_copy_into_buffer(bufferref,first1,first))
				return {first,parse_code::invalid};
			state=1u;
			return {first,parse_code::partial};
		}
		if(*first!=char_literal_v<u8'\n',char_type>)
		{
			return {first,parse_code::invalid};
		}
		++first;
		if(first==last)
		{
			if(!::fast_io::details::try_copy_into_buffer(bufferref,first1,first))
				return {first,parse_code::invalid};
			state=2u;
			return {first,parse_code::partial};
		}
		if(*first!=char_literal_v<u8'\r',char_type>)
		{
			continue;
		}
		++first;
		if(first==last)
		{
			if(!::fast_io::details::try_copy_into_buffer(bufferref,first1,first))
				return {first,parse_code::invalid};
			state=3u;
			return {first,parse_code::partial};
		}
		if(*first!=char_literal_v<u8'\n',char_type>)
		{
			return {first,parse_code::invalid};
		}
		++first;
		if(!::fast_io::details::try_copy_into_buffer(bufferref,first1,first))
			return {first,parse_code::invalid};
		return {first,::fast_io::details::determine_http_header_location(bufferref)};
	}
}

}

template<::std::integral char_type,std::size_t buffer_size>
inline constexpr parse_result<char_type const*> scan_context_define(io_reserve_type_t<char_type,::fast_io::parameter<basic_http_header_buffer<char_type,buffer_size>&>>,http_buffer_parse_context& statetp,char_type const* first1,char_type const* last,::fast_io::parameter<basic_http_header_buffer<char_type>&> t) noexcept
{
	return ::fast_io::details::http_header_scan_context_define_impl(statetp.state,first1,last,t.reference);
}

template<std::integral char_type,std::size_t buffer_size>
inline constexpr parse_code scan_context_eof_define(io_reserve_type_t<char_type,::fast_io::parameter<basic_http_header_buffer<char_type>&>>,http_buffer_parse_context,::fast_io::parameter<basic_http_header_buffer<char_type,buffer_size>&>) noexcept
{
	return parse_code::invalid;
}

using http_header_buffer = basic_http_header_buffer<char>;
using u8http_header_buffer = basic_http_header_buffer<char8_t>;
using u16http_header_buffer = basic_http_header_buffer<char16_t>;
using u32http_header_buffer = basic_http_header_buffer<char32_t>;
using whttp_header_buffer = basic_http_header_buffer<wchar_t>;

template<std::integral char_type>
struct basic_http_line_generator
{
	char_type const* current{};
	char_type const* last{};
	char_type const* key_end{};
	char_type const* value_start{};
	char_type const* value_end{};
};

template<std::integral char_type>
struct basic_http_line
{
	::fast_io::manipulators::basic_os_str_known_size_without_null_terminated<char_type> key,value;
};

template<std::integral char_type,std::size_t buffer_size>
inline constexpr basic_http_line_generator<char_type> line_generator(basic_http_header_buffer<char_type,buffer_size>& b) noexcept
{
	std::size_t end_loc{b.http_status_reason_end_location};
	char_type const* start{b.buffer+end_loc};
	return {start,b.buffer+b.header_length,start,start,start};
}

template<std::integral char_type>
inline constexpr basic_http_line_generator<char_type>& operator++(basic_http_line_generator<char_type>& b) noexcept
{
	auto const last{b.last};
	b.current=b.value_end;
	if(last-b.current<=4)
	{
		b.value_end=b.value_start=b.key_end=b.current=last;
		return b;
	}
	b.current = ::fast_io::details::find_ch_impl<u8'\n',false>(b.current,last);
//	for(;b.current!=last&&*b.current!=char_literal_v<u8'\n',char_type>;++b.current);
	if(b.current==last)
	{
		b.value_end=b.value_start=b.key_end=last;
		return b;
	}
	auto line_last{b.current};
	line_last = ::fast_io::details::find_ch_impl<u8'\r',false>(line_last,last);
//	for(;line_last!=last&&*line_last!=char_literal_v<u8'\r',char_type>;++line_last);
	++b.current;
	b.current = ::fast_io::details::find_ch_impl<u8' ',true>(b.current,line_last);
//	for(;b.current!=line_last&&*b.current==char_literal_v<u8' ',char_type>;++b.current);
	auto curr{b.current};
	curr = ::fast_io::details::find_ch_impl<u8':',false>(curr,line_last);
//	for(;curr!=line_last&&*curr!=char_literal_v<u8':',char_type>;++curr);
	b.key_end=curr;
	if(curr!=line_last)
		++curr;
	curr = ::fast_io::details::find_ch_impl<u8' ',true>(curr,line_last);
//	for(;curr!=line_last&&*curr==char_literal_v<u8' ',char_type>;++curr);
	b.value_start=curr;
	b.value_end=line_last;
	return b;
}

template<std::integral char_type>
inline constexpr basic_http_line_generator<char_type>& begin(basic_http_line_generator<char_type>& b) noexcept
{
	return ++b;
}

template<std::integral char_type>
inline constexpr ::std::default_sentinel_t end(basic_http_line_generator<char_type>&) noexcept
{
	return {};
}

template<std::integral char_type>
inline constexpr bool operator==(basic_http_line_generator<char_type>& b,::std::default_sentinel_t)
{
	return b.current==b.last;
}

template<std::integral char_type>
inline constexpr bool operator!=(basic_http_line_generator<char_type>& b,::std::default_sentinel_t)
{
	return b.current!=b.last;
}

template<std::integral char_type>
inline constexpr basic_http_line<char_type> operator*(basic_http_line_generator<char_type>& b) noexcept
{
	return {::fast_io::manipulators::os_str_known_size_without_null_terminated<char_type>(b.current,static_cast<std::size_t>(b.key_end-b.current)),::fast_io::manipulators::os_str_known_size_without_null_terminated<char_type>(b.value_start,static_cast<std::size_t>(b.value_end-b.value_start))};
}

}
