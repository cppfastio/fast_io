#pragma once

namespace fast_io
{

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
	inline constexpr basic_scanner_context& operator++()
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
					if(ec!=parse_code::ok)
					{
						throw_parse_code(ec);
					}
					ibuffer_set_curr(handle,it);
				}
				else
				{
					if(curr_ptr==end_ptr)
					{
						auto code{scan_iterative_eof_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,context)};
						if(code!=parse_code::ok)
						{
							throw_parse_code(code);
						}
					}
					else
					{
						auto p{scan_iterative_next_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,context,curr_ptr,end_ptr)};
						if(p.code==parse_code::ok)
						{
							ibuffer_set_curr(handle,p);
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
							if(code!=parse_code::ok)
							{
								throw_parse_code(code);
							}
							eof=u;
							break;
						}
						continue;
					}
					auto [p,code]{scan_iterative_next_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,context,curr_ptr,end_ptr)};
					if(code==parse_code::ok)[[likely]]
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
			eof=scan_freestanding(handle,context);
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
#endif
}
