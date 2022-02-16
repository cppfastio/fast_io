#pragma once

namespace fast_io
{

template<std::integral char_type>
struct basic_scanner_line_buffer
{
	char_type *begin_ptr{};
	char_type *curr_ptr{};
	char_type *end_ptr{};
	char_type const* view_begin_ptr{};
	char_type const* view_end_ptr{};
	bool inbuffer{};
};
/*
namespace details
{

template<std::integral char_type>
inline constexpr parse_result<char_type const*> scan_iterative_next_line_define_impl(
	basic_line_scanner_buffer<char_type>& buf,char_type const* buffer_curr,char_type const* buffer_end) noexcept
{

}

}
*/

template<std::integral char_type,bool buffer>
inline constexpr void scan_iterative_init_define(io_reserve_type_t<char_type,basic_line_scanner_buffer<char_type,buffer>>,
		basic_line_scanner_buffer<char_type,buffer>& buf) noexcept
{
	buf.inbuffer=false;	
}

template<std::integral char_type>
inline constexpr parse_result<char_type const*> scan_iterative_next_define(io_reserve_type<char_type,basic_line_scanner_buffer<char_type>>,
	basic_line_scanner_buffer<char_type>& buf,char_type const* first,char_type const* last) noexcept
{
	for(;buffer_curr!=buffer_end&&*buffer_curr!=::fast_io::details::char_literal_v<u8'\n',char_type>;++buffer_curr);
	if(buffer_curr==buffer_end)
	{
		if(buf.inbuffer)
		{
			std::size_t const remain_buffer_size{static_cast<std::size_t>(buff.end_ptr-buff.curr_ptr)};
			std::size_t const diff{static_cast<std::size_t>(last-first)};
			if(remain_buffer_size<diff)
			{
				
			}
			non_overlapped_copy(buffer_curr,);
		}
		else
		{
			buf.
		}
		return {};
	}
	else
	{

	}
}

template<typename char_type,typename T>
concept iterative_scannable = ::std::integral<char_type>&&requires(T& t,char_type const* buffer_curr,char_type const* buffer_end)
{
	{scan_iterative_init_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,t)};
	{scan_iterative_next_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,t,buffer_curr,buffer_end)}->parse_result<char_type const*>;
	{scan_iterative_eof_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,t)}->fast_io::parse_code;
};

template<typename char_type,typename T>
concept iterative_contiguous_scannable = ::std::integral<char_type>&&requires(T& t,char_type const* buffer_curr,char_type const* buffer_end)
{
	{scan_iterative_contiguous_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,t,buffer_curr,buffer_end)}->parse_result<char_type const*>;
};

template<::fast_io::buffer_input_stream input,typename T>
requires std::is_trivially_copyable_v<input>
struct basic_scanner_context
{
	using input_handle_type = input;
	using char_type = typename std::remove_cvref_t<input>::char_type;
	using context_type = T;

#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	input_handle_type handle;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	context_type context;
	bool eof{};
	inline constexpr basic_scanner& operator++()
	{
		if constexpr(iterative_scannable<char_type,T>)
		{
			if constexpr(contiguous_input_stream<input_handle_type>)
			{
				auto curr_ptr{ibuffer_curr(handle)};
				auto end_ptr{ibuffer_end(handle)};
				if constexpr(iterative_contiguous_scannable<char_type,context_type>)
				{
					auto [it,ec]=scan_iterative_contiguous_define(io_reserve_type<char_type,context_type>,curr_ptr,end_ptr);
					if(ec!=parse_code::okay)
					{
						throw_parse_code(code);
					}
					ibuffer_set_curr(handle,it);
				}
				else
				{
					if(curr_ptr==end_ptr)
					{
						auto code{scan_iterative_eof_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,context)};
						if(code!=parse_code::okay)
						{
							throw_parse_code(code);
						}
					}
					else
					{
						auto p{scan_iterative_next_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,context,curr_ptr,end_ptr)};
						if(p.code==parse_code::okay)
						{
							ibuffer_set_curr(handle,p);
							break;
						}
					}
				}
			}
			else
			{
				for(;;)
				{
					auto curr_ptr{ibuffer_curr(handle)};
					auto end_ptr{ibuffer_end(handle)};
					if(curr_ptr==end_ptr)
					{
						bool u{ibuffer_underflow(handle)};
						if(!u)
						{
							auto code{scan_iterative_eof_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,context)};
							if(code!=parse_code::okay)
							{
								throw_parse_code(code);
							}
							eof=u;
							break;
						}
						continue;
					}
					auto [p,code]{scan_iterative_next_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,context,curr_ptr,end_ptr)};
					if(code==parse_code::okay)[[likely]]
					{
						ibuffer_set_curr(handle,p);
						break;
					}
					else if(code==parse_code::partial)
					{
						ibuffer_set_curr(handle,p);
						continue;
					}
					throw_parse_code(code);
				}
			}
		}
		else
		{
			eof=scan_freestanding(handle,p);
		}
		return *this;
	}
};
#if 0
template<::fast_io::buffer_input_stream input,typename T>
requires std::is_trivially_copyable_v<input>
struct basic_scanner_context_mutex
{

};
#endif
template<typename T>
inline constexpr auto basic_scanner(T&& input)
{
	using nocvreft = std::remove_cvref_t<T>;
	if constexpr(mutex_stream<T>)
	{

	}
	else
	{
		constexpr bool type_error{buffer_output_stream<nocvreft>};
		if constexpr(type_error)
		{
			using reftype = decltype(io_ref(input));
			return basic_scanner_context<reftype,nocvreft>(io_ref,input);
		}
		else
		{
static_assert(type_error,"input stream is not bufferred");
		}
	}
}

}
