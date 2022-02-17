#pragma once

namespace fast_io
{

template<::fast_io::buffer_input_stream input,typename T>
requires std::is_trivially_copyable_v<input>
struct basic_scanner_context;

namespace details
{

template<::fast_io::buffer_input_stream input,typename T>
struct basic_scanner_context_iterator
{
	using input_handle_type = input;
	using char_type = typename std::remove_cvref_t<input>::char_type;
	using context_type = T;
	basic_scanner_context<input,T>* ptr{};
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	std::conditional_t<::fast_io::contiguous_input_stream<input>,::fast_io::details::empty,bool> last_is_eof{};
	inline constexpr context_type& operator*() const noexcept
	{
		return ptr->context;
	}
	inline constexpr basic_scanner_context_iterator& operator++()
	{
		auto& handle{this->ptr->handle};
		auto& context{this->ptr->context};
		if constexpr(iterative_scannable<char_type,T>)
		{
			if constexpr(contiguous_input_stream<input_handle_type>)
			{
				auto curr_ptr{ibuffer_curr(handle)};
				auto end_ptr{ibuffer_end(handle)};
				if constexpr(iterative_contiguous_scannable<char_type,context_type>)
				{
					auto [it,ec]=scan_iterative_contiguous_define(io_reserve_type<char_type,context_type>,curr_ptr,end_ptr);
					ibuffer_set_curr(handle,it);
					if(ec!=parse_code::ok)
					{
						throw_parse_code(ec);
					}
				}
				else
				{
					scan_iterative_init_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,context);
					auto [p,code]{scan_iterative_next_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,context,curr_ptr,end_ptr)};
					ibuffer_set_curr(handle,p);
					if(code!=parse_code::ok)[[unlikely]]
					{
						if(code!=parse_code::partial)
						{
							throw_parse_code(code);
						}
						auto eofcode{scan_iterative_eof_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,context)};
						if(eofcode!=parse_code::ok)
						{
							throw_parse_code(eofcode);	
						}
					}
				}
			}
			else
			{
				for(scan_iterative_init_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,context);;)
				{
					auto curr_ptr{ibuffer_curr(handle)};
					auto end_ptr{ibuffer_end(handle)};
					if(curr_ptr==end_ptr)
					{
						if(last_is_eof)
						{
							this->ptr=nullptr;
							break;
						}
						bool u{ibuffer_underflow(handle)};
						if(!u)
						{
							auto code{scan_iterative_eof_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,context)};
							if(code==parse_code::ok)
							{
								this->last_is_eof=true;
								break;
							}
							else
							{
								throw_parse_code(code);
							}
						}
						continue;
					}
					auto [p,code]{scan_iterative_next_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,context,curr_ptr,end_ptr)};
					ibuffer_set_curr(handle,p-curr_ptr+curr_ptr);
					if(code==parse_code::ok)[[likely]]
					{
						break;
					}
					else if(code==parse_code::partial)
					{
						continue;
					}
					throw_parse_code(code);
				}
			}
		}
		else
		{
			static_assert(iterative_scannable<char_type,T>);
			if(!scan_iterative_context_define(handle,context))[[unlikely]]
			{
				this->ptr=nullptr;	
			}
		}
		return *this;
	}
};

template<::fast_io::buffer_input_stream input,typename T>
inline constexpr bool operator==(basic_scanner_context_iterator<input,T> it,::std::default_sentinel_t) noexcept
{
	return it.ptr==nullptr;
}

template<::fast_io::buffer_input_stream input,typename T>
inline constexpr bool operator!=(basic_scanner_context_iterator<input,T> it,::std::default_sentinel_t) noexcept
{
	return it.ptr!=nullptr;
}

template<::fast_io::buffer_input_stream input,typename T>
inline constexpr bool operator==(::std::default_sentinel_t,basic_scanner_context_iterator<input,T> it) noexcept
{
	return it.ptr==nullptr;
}

template<::fast_io::buffer_input_stream input,typename T>
inline constexpr bool operator!=(::std::default_sentinel_t,basic_scanner_context_iterator<input,T> it) noexcept
{
	return it.ptr!=nullptr;
}

}

template<::fast_io::buffer_input_stream input,typename T>
requires std::is_trivially_copyable_v<input>
struct basic_scanner_context
{
	using input_handle_type = input;
	using char_type = typename std::remove_cvref_t<input>::char_type;
	using context_type = T;
	using iterator = ::fast_io::details::basic_scanner_context_iterator<input,T>;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	input_handle_type handle{};
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	context_type context{};
	inline constexpr iterator begin()
	{
		return ++iterator{this};
	}

	inline constexpr ::fast_io::freestanding::default_sentinel_t end() const noexcept
	{
		return {};
	}
};
#if 0
template<::fast_io::buffer_input_stream input,typename T>
requires std::is_trivially_copyable_v<input>
struct basic_scanner_context_mutex
{

};
#endif

template<::fast_io::buffer_input_stream input,typename T>
inline constexpr bool operator==(basic_scanner_context<input,T> const& ctx,::fast_io::freestanding::default_sentinel_t) noexcept
{
	return ctx.eof;
}

template<::fast_io::buffer_input_stream input,typename T>
inline constexpr bool operator!=(basic_scanner_context<input,T> const& ctx,::fast_io::freestanding::default_sentinel_t) noexcept
{
	return !ctx.eof;
}

template<::fast_io::buffer_input_stream input,typename T>
inline constexpr bool operator==(::fast_io::freestanding::default_sentinel_t,basic_scanner_context<input,T> const& ctx) noexcept
{
	return ctx.eof;
}

template<::fast_io::buffer_input_stream input,typename T>
inline constexpr bool operator!=(::fast_io::freestanding::default_sentinel_t,basic_scanner_context<input,T> const& ctx) noexcept
{
	return !ctx.eof;
}

#if 0
template<::fast_io::buffer_input_stream input,typename T>
requires std::is_trivially_copyable_v<input>
struct basic_scanner_context_mutex
{

};

template<typename T>
inline constexpr auto basic_scanner(T&& __restrict input)
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
