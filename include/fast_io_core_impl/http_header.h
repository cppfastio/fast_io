#pragma once

namespace fast_io
{

template <::std::integral ch_type, ::std::size_t buffer_size = 4096u>
	requires(buffer_size >= 64u)
struct basic_http_header_buffer
{
	using char_type = ch_type;
	::std::size_t header_length{};
	::std::size_t http_request_end_location{};
	::std::size_t http_status_code_start_location{};
	::std::size_t http_status_code_end_location{};
	::std::size_t http_status_reason_start_location{};
	::std::size_t http_status_reason_end_location{};
	char_type buffer[buffer_size];
	inline static constexpr ::std::size_t size() noexcept
	{
		return buffer_size;
	}
	inline constexpr ::fast_io::manipulators::basic_os_str_known_size_without_null_terminated<char_type>
	request() const noexcept
	{
		return ::fast_io::manipulators::os_str_known_size_without_null_terminated<char_type>(
			buffer, buffer + http_request_end_location);
	}
	inline constexpr ::fast_io::manipulators::basic_os_str_known_size_without_null_terminated<char_type>
	code() const noexcept
	{
		return ::fast_io::manipulators::os_str_known_size_without_null_terminated<char_type>(
			buffer + http_status_code_start_location, buffer + http_status_code_end_location);
	}
	inline constexpr ::fast_io::manipulators::basic_os_str_known_size_without_null_terminated<char_type>
	reason() const noexcept
	{
		return ::fast_io::manipulators::os_str_known_size_without_null_terminated<char_type>(
			buffer + http_status_reason_start_location, buffer + http_status_reason_end_location);
	}
};

struct http_buffer_parse_context
{
	::std::size_t state{};
};

template <::std::integral char_type, ::std::size_t buffer_size>
inline constexpr auto scan_context_type(
	io_reserve_type_t<char_type,
					  ::fast_io::parameter<::fast_io::basic_http_header_buffer<char_type, buffer_size> &>>) noexcept
{
	return io_type_t<http_buffer_parse_context>{};
}

template <::std::integral ch_type, ::std::size_t buffer_size>
inline constexpr basic_io_scatter_t<ch_type>
print_alias_define(io_alias_t, basic_http_header_buffer<ch_type, buffer_size> const &b) noexcept
{
	return {b.buffer, b.header_length};
}

namespace details
{
template <::std::integral ch_type, ::std::size_t buffer_size, ::std::integral char_type>
inline bool try_copy_into_buffer(basic_http_header_buffer<ch_type, buffer_size> &b, char_type const *first,
								 char_type const *last) noexcept
{
	::std::size_t diff{static_cast<::std::size_t>(last - first)};
	::std::size_t remain_space{static_cast<::std::size_t>(buffer_size - b.header_length)};
	if (remain_space < diff)
	{
		return false;
	}
	non_overlapped_copy_n(first, diff, b.buffer + b.header_length);
	b.header_length += diff;
	return true;
}

template <::std::integral ch_type, ::std::size_t buffer_size>
inline constexpr ::fast_io::freestanding::parse_errc determine_http_header_location(basic_http_header_buffer<ch_type, buffer_size> &b) noexcept
{
	auto i{b.buffer}, e{b.buffer + b.header_length};
	if (i != e && *i == char_literal_v<u8' ', ch_type>)
	{
		return ::fast_io::freestanding::parse_errc::invalid;
	}
	{
		i = ::fast_io::details::find_ch_impl<u8' ', false>(i, e);
		// for(;i!=e&&*i!=char_literal_v<u8' ',ch_type>;++i);
		if (i == e)
		{
			return ::fast_io::freestanding::parse_errc::invalid;
		}
		b.http_request_end_location = static_cast<::std::size_t>(i - b.buffer);
	}
	++i;
	i = ::fast_io::details::find_ch_impl<u8' ', true>(i, e);
	// for(;i!=e&&*i==char_literal_v<u8' ',ch_type>;++i);
	if (i == e)
	{
		return ::fast_io::freestanding::parse_errc::invalid;
	}
	b.http_status_code_start_location = static_cast<::std::size_t>(i - b.buffer);
	{
		i = ::fast_io::details::find_ch_impl<u8' ', false>(i, e);
		// for(;i!=e&&*i!=char_literal_v<u8' ',ch_type>;++i);
		if (i == e)
		{
			return ::fast_io::freestanding::parse_errc::invalid;
		}
		b.http_status_code_end_location = static_cast<::std::size_t>(i - b.buffer);
	}
	i = ::fast_io::details::find_ch_impl<u8' ', true>(i, e);
	// for(;i!=e&&*i==char_literal_v<u8' ',ch_type>;++i);
	if (i == e)
	{
		return ::fast_io::freestanding::parse_errc::invalid;
	}
	b.http_status_reason_start_location = static_cast<::std::size_t>(i - b.buffer);
	i = ::fast_io::details::find_ch_impl<u8'\r', false>(i, e);
	// for(;i!=e&&*i!=char_literal_v<u8'\r',ch_type>;++i);
	if (i == e)
	{
		return ::fast_io::freestanding::parse_errc::invalid;
	}
	b.http_status_reason_end_location = static_cast<::std::size_t>(i - b.buffer);
	return ::fast_io::freestanding::parse_errc::ok;
}

template <::std::integral char_type, ::std::size_t buffer_size>
inline constexpr parse_result<char_type const *>
http_header_scan_context_define_impl(::std::size_t &state, char_type const *first1, char_type const *last,
									 basic_http_header_buffer<char_type, buffer_size> &bufferref) noexcept
{
	auto first{first1};
	if (first == last)
	{
		return {first, ::fast_io::freestanding::parse_errc::partial};
	}
	if (state != 0)
	{
		switch (state)
		{
		case 1:
		{
			if (*first != char_literal_v<u8'\n', char_type>)
			{
				break;
			}
			++first;
			if (first == last)
			{
				state = 2u;
				if (!::fast_io::details::try_copy_into_buffer(bufferref, first1, last))
				{
					return {first, ::fast_io::freestanding::parse_errc::invalid};
				}
				return {first, ::fast_io::freestanding::parse_errc::partial};
			}
			[[fallthrough]];
		}
		case 2:
		{
			if (*first != char_literal_v<u8'\r', char_type>)
			{
				break;
			}
			++first;
			if (first == last)
			{
				state = 3u;
				if (!::fast_io::details::try_copy_into_buffer(bufferref, first1, last))
				{
					return {first, ::fast_io::freestanding::parse_errc::invalid};
				}
				return {first, ::fast_io::freestanding::parse_errc::partial};
			}
			[[fallthrough]];
		}
		case 3:
		{
			if (*first != char_literal_v<u8'\n', char_type>)
			{
				break;
			}
			++first;
			if (!::fast_io::details::try_copy_into_buffer(bufferref, first1, first))
			{
				return {first, ::fast_io::freestanding::parse_errc::invalid};
			}
			return {first, ::fast_io::details::determine_http_header_location(bufferref)};
		}
		}
	}
	state = 0;
	for (;;)
	{
		first = ::fast_io::details::find_ch_impl<u8'\r', false>(first, last);
		// for(;first!=last&&*first!=char_literal_v<u8'\r',char_type>;++first);
		if (first == last)
		{
			if (!::fast_io::details::try_copy_into_buffer(bufferref, first1, first))
			{
				return {first, ::fast_io::freestanding::parse_errc::invalid};
			}
			state = 0u;
			return {first, ::fast_io::freestanding::parse_errc::partial};
		}
		++first;
		if (first == last)
		{
			if (!::fast_io::details::try_copy_into_buffer(bufferref, first1, first))
			{
				return {first, ::fast_io::freestanding::parse_errc::invalid};
			}
			state = 1u;
			return {first, ::fast_io::freestanding::parse_errc::partial};
		}
		if (*first != char_literal_v<u8'\n', char_type>)
		{
			return {first, ::fast_io::freestanding::parse_errc::invalid};
		}
		++first;
		if (first == last)
		{
			if (!::fast_io::details::try_copy_into_buffer(bufferref, first1, first))
			{
				return {first, ::fast_io::freestanding::parse_errc::invalid};
			}
			state = 2u;
			return {first, ::fast_io::freestanding::parse_errc::partial};
		}
		if (*first != char_literal_v<u8'\r', char_type>)
		{
			continue;
		}
		++first;
		if (first == last)
		{
			if (!::fast_io::details::try_copy_into_buffer(bufferref, first1, first))
			{
				return {first, ::fast_io::freestanding::parse_errc::invalid};
			}
			state = 3u;
			return {first, ::fast_io::freestanding::parse_errc::partial};
		}
		if (*first != char_literal_v<u8'\n', char_type>)
		{
			return {first, ::fast_io::freestanding::parse_errc::invalid};
		}
		++first;
		if (!::fast_io::details::try_copy_into_buffer(bufferref, first1, first))
		{
			return {first, ::fast_io::freestanding::parse_errc::invalid};
		}
		return {first, ::fast_io::details::determine_http_header_location(bufferref)};
	}
}

} // namespace details

template <::std::integral char_type, ::std::size_t buffer_size>
inline constexpr parse_result<char_type const *> scan_context_define(
	io_reserve_type_t<char_type, ::fast_io::parameter<basic_http_header_buffer<char_type, buffer_size> &>>,
	http_buffer_parse_context &statetp, char_type const *first1, char_type const *last,
	::fast_io::parameter<basic_http_header_buffer<char_type> &> t) noexcept
{
	return ::fast_io::details::http_header_scan_context_define_impl(statetp.state, first1, last, t.reference);
}

template <::std::integral char_type, ::std::size_t buffer_size>
inline constexpr ::fast_io::freestanding::parse_errc
scan_context_eof_define(io_reserve_type_t<char_type, ::fast_io::parameter<basic_http_header_buffer<char_type> &>>,
						http_buffer_parse_context,
						::fast_io::parameter<basic_http_header_buffer<char_type, buffer_size> &>) noexcept
{
	return ::fast_io::freestanding::parse_errc::invalid;
}

using http_header_buffer = basic_http_header_buffer<char>;
using u8http_header_buffer = basic_http_header_buffer<char8_t>;
using u16http_header_buffer = basic_http_header_buffer<char16_t>;
using u32http_header_buffer = basic_http_header_buffer<char32_t>;
using whttp_header_buffer = basic_http_header_buffer<wchar_t>;

template <::std::integral char_type>
struct basic_http_line_generator
{
	char_type const *current{};
	char_type const *last{};
	char_type const *key_end{};
	char_type const *value_start{};
	char_type const *value_end{};
};

template <::std::integral char_type>
struct basic_http_line
{
	::fast_io::manipulators::basic_os_str_known_size_without_null_terminated<char_type> key, value;
};

template <::std::integral char_type, ::std::size_t buffer_size>
inline constexpr basic_http_line_generator<char_type>
line_generator(basic_http_header_buffer<char_type, buffer_size> const &b) noexcept
{
	::std::size_t end_loc{b.http_status_reason_end_location};
	char_type const *start{b.buffer + end_loc};
	return {start, b.buffer + b.header_length, start, start, start};
}

template <::std::integral char_type>
inline constexpr basic_http_line_generator<char_type> &operator++(basic_http_line_generator<char_type> &b) noexcept
{
	auto const last{b.last};
	b.current = b.value_end;
	if (last - b.current <= 4)
	{
		b.value_end = b.value_start = b.key_end = b.current = last;
		return b;
	}
	b.current = ::fast_io::details::find_ch_impl<u8'\n', false>(b.current, last);
	//	for(;b.current!=last&&*b.current!=char_literal_v<u8'\n',char_type>;++b.current);
	if (b.current == last)
	{
		b.value_end = b.value_start = b.key_end = last;
		return b;
	}
	auto line_last{b.current};
	line_last = ::fast_io::details::find_ch_impl<u8'\r', false>(line_last, last);
	//	for(;line_last!=last&&*line_last!=char_literal_v<u8'\r',char_type>;++line_last);
	++b.current;
	b.current = ::fast_io::details::find_ch_impl<u8' ', true>(b.current, line_last);
	//	for(;b.current!=line_last&&*b.current==char_literal_v<u8' ',char_type>;++b.current);
	auto curr{b.current};
	curr = ::fast_io::details::find_ch_impl<u8':', false>(curr, line_last);
	//	for(;curr!=line_last&&*curr!=char_literal_v<u8':',char_type>;++curr);
	b.key_end = curr;
	if (curr != line_last)
	{
		++curr;
	}
	curr = ::fast_io::details::find_ch_impl<u8' ', true>(curr, line_last);
	//	for(;curr!=line_last&&*curr==char_literal_v<u8' ',char_type>;++curr);
	b.value_start = curr;
	b.value_end = line_last;
	return b;
}

template <::std::integral char_type>
inline constexpr basic_http_line_generator<char_type> &begin(basic_http_line_generator<char_type> &b) noexcept
{
	return ++b;
}

template <::std::integral char_type>
inline constexpr ::std::default_sentinel_t end(basic_http_line_generator<char_type> &) noexcept
{
	return {};
}

template <::std::integral char_type>
inline constexpr bool operator==(basic_http_line_generator<char_type> &b, ::std::default_sentinel_t) noexcept
{
	return b.current == b.last;
}

template <::std::integral char_type>
inline constexpr bool operator!=(basic_http_line_generator<char_type> &b, ::std::default_sentinel_t) noexcept
{
	return b.current != b.last;
}

template <::std::integral char_type>
inline constexpr basic_http_line<char_type> operator*(basic_http_line_generator<char_type> &b) noexcept
{
	return {::fast_io::manipulators::os_str_known_size_without_null_terminated<char_type>(
				b.current, static_cast<::std::size_t>(b.key_end - b.current)),
			::fast_io::manipulators::os_str_known_size_without_null_terminated<char_type>(
				b.value_start, static_cast<::std::size_t>(b.value_end - b.value_start))};
}

namespace manipulators::scan_skippers
{

enum class line_skipper_tag
{
	lf,
	crlf,
	cr,
#if defined(_WIN32) || defined(__CYGWIN__)
	platform = crlf
#else
	platform = lf
#endif
};

template <line_skipper_tag tag>
struct line_skipper
{
	using manip_tag = manip_tag_t;
	inline static constexpr line_skipper_tag tag_value{tag};
};

/*
Consumes exactly one line terminator from an input stream.
	crlf: "\r\n" or a lone "\n"		lf: "\n"	cr: "\r"
	platform: crlf on _WIN32/__CYGWIN__, lf elsewhere
Used for protocol framing such as HTTP chunked bodies. Also printable:
prints the terminator to an output stream.
*/
inline constexpr line_skipper<line_skipper_tag::lf> lf() noexcept
{
	return {};
}
inline constexpr line_skipper<line_skipper_tag::crlf> crlf() noexcept
{
	return {};
}
inline constexpr line_skipper<line_skipper_tag::cr> cr() noexcept
{
	return {};
}
inline constexpr line_skipper<line_skipper_tag::platform> line_skipper_platform() noexcept
{
	return {};
}

} // namespace manipulators::scan_skippers

template <::std::integral char_type, manipulators::scan_skippers::line_skipper_tag tag>
inline constexpr ::std::size_t
print_reserve_size(io_reserve_type_t<char_type, manipulators::scan_skippers::line_skipper<tag>>) noexcept
{
	return 2;
}

template <::std::integral char_type, manipulators::scan_skippers::line_skipper_tag tag>
inline constexpr char_type *
print_reserve_define(io_reserve_type_t<char_type, manipulators::scan_skippers::line_skipper<tag>>, char_type *iter,
					 manipulators::scan_skippers::line_skipper<tag>) noexcept
{
	if constexpr (tag == manipulators::scan_skippers::line_skipper_tag::lf)
	{
		*iter = char_literal_v<u8'\n', char_type>;
		return iter + 1;
	}
	else if constexpr (tag == manipulators::scan_skippers::line_skipper_tag::cr)
	{
		*iter = char_literal_v<u8'\r', char_type>;
		return iter + 1;
	}
	else
	{
		*iter = char_literal_v<u8'\r', char_type>;
		iter[1] = char_literal_v<u8'\n', char_type>;
		return iter + 2;
	}
}

namespace details
{

struct line_skipper_scan_context
{
	::std::uint_least8_t matched{};
};

template <::fast_io::manipulators::scan_skippers::line_skipper_tag tag, ::std::integral char_type>
inline constexpr parse_result<char_type const *>
line_skipper_scan_context_define_impl(line_skipper_scan_context &ctx, char_type const *first,
									  char_type const *last) noexcept
{
	for (; first != last; ++first)
	{
		if constexpr (tag == ::fast_io::manipulators::scan_skippers::line_skipper_tag::lf)
		{
			if (*first != char_literal_v<u8'\n', char_type>)
			{
				return {first, ::fast_io::freestanding::parse_errc::invalid};
			}
			return {first + 1, ::fast_io::freestanding::parse_errc::ok};
		}
		else if constexpr (tag == ::fast_io::manipulators::scan_skippers::line_skipper_tag::cr)
		{
			if (*first != char_literal_v<u8'\r', char_type>)
			{
				return {first, ::fast_io::freestanding::parse_errc::invalid};
			}
			return {first + 1, ::fast_io::freestanding::parse_errc::ok};
		}
		else
		{
			if (ctx.matched == 0)
			{
				if (*first == char_literal_v<u8'\n', char_type>)
				{
					return {first + 1, ::fast_io::freestanding::parse_errc::ok};
				}
				if (*first != char_literal_v<u8'\r', char_type>)
				{
					return {first, ::fast_io::freestanding::parse_errc::invalid};
				}
				ctx.matched = 1;
			}
			else
			{
				if (*first != char_literal_v<u8'\n', char_type>)
				{
					return {first, ::fast_io::freestanding::parse_errc::invalid};
				}
				return {first + 1, ::fast_io::freestanding::parse_errc::ok};
			}
		}
	}
	return {first, ::fast_io::freestanding::parse_errc::partial};
}

} // namespace details

template <::std::integral char_type, manipulators::scan_skippers::line_skipper_tag tag>
inline constexpr io_type_t<details::line_skipper_scan_context>
scan_context_type(io_reserve_type_t<char_type, manipulators::scan_skippers::line_skipper<tag>>) noexcept
{
	return {};
}

template <::std::integral char_type, manipulators::scan_skippers::line_skipper_tag tag>
inline constexpr parse_result<char_type const *>
scan_contiguous_define(io_reserve_type_t<char_type, manipulators::scan_skippers::line_skipper<tag>>,
					   char_type const *begin, char_type const *end,
					   manipulators::scan_skippers::line_skipper<tag>) noexcept
{
	constexpr bool is_crlf{tag == manipulators::scan_skippers::line_skipper_tag::crlf};
	if (begin != end)
	{
		if constexpr (is_crlf)
		{
			if (*begin == char_literal_v<u8'\n', char_type>)
			{
				return {begin + 1, ::fast_io::freestanding::parse_errc::ok};
			}
		}
		char_type const expected{(tag == manipulators::scan_skippers::line_skipper_tag::lf)
									 ? char_literal_v<u8'\n', char_type>
									 : char_literal_v<u8'\r', char_type>};
		if (*begin != expected)
		{
			return {begin, ::fast_io::freestanding::parse_errc::invalid};
		}
		if constexpr (!is_crlf)
		{
			return {begin + 1, ::fast_io::freestanding::parse_errc::ok};
		}
		else if (end - begin >= 2)
		{
			if (begin[1] != char_literal_v<u8'\n', char_type>)
			{
				return {begin + 1, ::fast_io::freestanding::parse_errc::invalid};
			}
			return {begin + 2, ::fast_io::freestanding::parse_errc::ok};
		}
	}
	/*
	Ran out of input before the line terminator completed. Return end so
	the caller falls back to the context path which re-examines the same chars.
	*/
	return {end, ::fast_io::freestanding::parse_errc::partial};
}

template <::std::integral char_type, manipulators::scan_skippers::line_skipper_tag tag>
inline constexpr parse_result<char_type const *>
scan_context_define(io_reserve_type_t<char_type, manipulators::scan_skippers::line_skipper<tag>>,
					details::line_skipper_scan_context &ctx, char_type const *begin, char_type const *end,
					manipulators::scan_skippers::line_skipper<tag>) noexcept
{
	return details::line_skipper_scan_context_define_impl<tag>(ctx, begin, end);
}

template <::std::integral char_type, manipulators::scan_skippers::line_skipper_tag tag>
inline constexpr ::fast_io::freestanding::parse_errc
scan_context_eof_define(io_reserve_type_t<char_type, manipulators::scan_skippers::line_skipper<tag>>,
						details::line_skipper_scan_context &ctx,
						manipulators::scan_skippers::line_skipper<tag>) noexcept
{
	/* EOF with nothing consumed is plain end_of_file; EOF between '\r' and '\n' is truncated input */
	return ctx.matched == 0 ? ::fast_io::freestanding::parse_errc::end_of_file
							: ::fast_io::freestanding::parse_errc::invalid;
}

} // namespace fast_io
