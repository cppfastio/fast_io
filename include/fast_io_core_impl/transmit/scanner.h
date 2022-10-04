#pragma once

namespace fast_io
{

namespace details
{

template<::fast_io::input_stream input,typename context_type,typename sccontext>
inline constexpr void basic_scanner_context_next_common(input handle,context_type& context,sccontext& scnctx)
{
	using input_handle_type = input;
	using char_type = typename std::remove_cvref_t<input>::char_type;
	if constexpr(mutex_stream<input>)
	{
		basic_scanner_context_next_common(handle.unlocked_handle(),context,scnctx);
	}
	else
	{
		if constexpr(iterative_scannable<char_type,context_type>||iterative_contiguous_scannable<char_type,context_type>)
		{
			if constexpr(contiguous_input_stream<input_handle_type>)
			{
				auto curr_ptr{ibuffer_curr(handle)};
				auto end_ptr{ibuffer_end(handle)};
				if constexpr(iterative_contiguous_scannable<char_type,context_type>)
				{
					auto [it,ec]=scan_iterative_contiguous_define(io_reserve_type<char_type,std::remove_cvref_t<context_type>>,context,curr_ptr,end_ptr);
					ibuffer_set_curr(handle,it);
					if(ec!=parse_code::ok)
					{
						if(ec==parse_code::end_of_file)
						{
							scnctx.ptr=nullptr;
							return;
						}
						throw_parse_code(ec);
					}
				}
				else
				{
					scan_iterative_init_define(io_reserve_type<char_type,std::remove_cvref_t<context_type>>,context);
					auto [p,code]{scan_iterative_next_define(io_reserve_type<char_type,std::remove_cvref_t<context_type>>,context,curr_ptr,end_ptr)};
					ibuffer_set_curr(handle,p);
					if(code!=parse_code::ok)[[unlikely]]
					{
						if(code!=parse_code::partial)
						{
							throw_parse_code(code);
						}
						auto eofcode{scan_iterative_eof_define(io_reserve_type<char_type,std::remove_cvref_t<context_type>>,context)};
						if(eofcode!=parse_code::ok)
						{
							throw_parse_code(eofcode);	
						}
					}
				}
			}
			else
			{
				for(scan_iterative_init_define(io_reserve_type<char_type,std::remove_cvref_t<context_type>>,context);;)
				{
					auto curr_ptr{ibuffer_curr(handle)};
					auto end_ptr{ibuffer_end(handle)};
					if(curr_ptr==end_ptr)
					{
						if(scnctx.last_is_eof)
						{
							scnctx.ptr=nullptr;
							break;
						}
						bool u{ibuffer_underflow(handle)};
						if(!u)
						{
							auto code{scan_iterative_eof_define(io_reserve_type<char_type,std::remove_cvref_t<context_type>>,context)};
							if(code==parse_code::ok)
							{
								scnctx.last_is_eof=true;
								break;
							}
							else if(code==parse_code::end_of_file)
							{
								scnctx.ptr=nullptr;
								break;
							}
							else
							{
								throw_parse_code(code);
							}
						}
						continue;
					}
					auto [p,code]{scan_iterative_next_define(io_reserve_type<char_type,std::remove_cvref_t<context_type>>,context,curr_ptr,end_ptr)};
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
			static_assert(iterative_scannable<char_type,context_type>);
			if(!scan_iterative_context_define(handle,context))[[unlikely]]
			{
				scnctx.ptr=nullptr;	
			}
		}
	}
}

template<typename T>
struct basic_scanner_context_iterator
{
	using input_handle_type = typename T::input_handle_type;
	using char_type = typename T::char_type;
	using context_type = typename T::context_type;
	T* ptr{};
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	std::conditional_t<::fast_io::contiguous_input_stream<input_handle_type>,::fast_io::details::empty,bool> last_is_eof{};
	inline constexpr context_type& operator*() const noexcept
	{
		return ptr->context;
	}
	inline constexpr basic_scanner_context_iterator& operator++()
	{
		basic_scanner_context_next_common(ptr->handle,ptr->context,*this);
		return *this;
	}
};

template<typename T>
inline constexpr bool operator==(basic_scanner_context_iterator<T> it,::std::default_sentinel_t) noexcept
{
	return it.ptr==nullptr;
}

template<typename T>
inline constexpr bool operator!=(basic_scanner_context_iterator<T> it,::std::default_sentinel_t) noexcept
{
	return it.ptr!=nullptr;
}

template<typename T>
inline constexpr bool operator==(::std::default_sentinel_t,basic_scanner_context_iterator<T> it) noexcept
{
	return it.ptr==nullptr;
}

template<typename T>
inline constexpr bool operator!=(::std::default_sentinel_t,basic_scanner_context_iterator<T> it) noexcept
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
	using iterator = ::fast_io::details::basic_scanner_context_iterator<basic_scanner_context<input,T>>;
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
	inline constexpr ::std::default_sentinel_t end() const noexcept
	{
		return {};
	}
};

template<::fast_io::input_stream input,typename T>
requires (std::is_trivially_copyable_v<input>&&mutex_stream<input>)
struct basic_scanner_context_mutex
{
	using input_handle_type = input;
	using char_type = typename std::remove_cvref_t<input>::char_type;
	using context_type = T;
	using iterator = ::fast_io::details::basic_scanner_context_iterator<basic_scanner_context_mutex<input,T>>;
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
	explicit constexpr basic_scanner_context_mutex(input_handle_type h):handle(h)
	{
		handle.lock();
	}
#if __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	~basic_scanner_context_mutex()
	{
		handle.unlock();
	}
	basic_scanner_context_mutex(basic_scanner_context_mutex const&)=delete;
	basic_scanner_context_mutex& operator=(basic_scanner_context_mutex const&)=delete;
	inline constexpr iterator begin()
	{
		return ++iterator{this};
	}
	inline constexpr ::std::default_sentinel_t end() const noexcept
	{
		return {};
	}
};

}
