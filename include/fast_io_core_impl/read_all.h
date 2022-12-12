#pragma once

namespace fast_io
{
namespace details
{

template<bool throweh=true,::fast_io::input_stream input>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr ::std::conditional_t<throweh,void,bool> read_all_impl_decay_cold(input in,typename input::char_type* first,typename input::char_type* last)
{
	while(first!=last)
	{
		auto it{read(in,first,last)};
		if(it==first)[[unlikely]]
		{
			if constexpr(throweh)
			{
				throw_parse_code(parse_code::end_of_file);
			}
			else
			{
				return false;
			}
		}
		first=it;
	}
	if constexpr(!throweh)
	{
		return true;
	}
}

#if 0
template<typename char_type,::std::input_or_output_iterator output_iter>
inline output_iter type_punning_copy(char_type const* first,char_type const* last,output_iter result)
{
	using value_type = ::std::iter_value_t<output_iter>;
	if constexpr(sizeof(value_type)==0)
		return result;
	else if constexpr(sizeof(value_type)==sizeof(char_type))
	{
		for(;first!=last;)
		{
			if constexpr(std::same_as<char_type,value_type>)
				*result=*first;
			else
			{
				my_memcpy(__builtin_addressof(result),first,sizeof(value_type));
			}
			++first;
			++result;
		}
		return result;
	}
	else
	{
		static_assert(sizeof(char_type)==1);
		for(;first!=last;)
		{
			my_memcpy(__builtin_addressof(result),first,sizeof(value_type));
			first+=sizeof(value_type);
			++result;
		}
		return result;
	}
}

template<::fast_io::input_stream input,std::forward_iterator Iter>
inline constexpr void read_all_impl_none_contiguous(input in,Iter first,Iter last)
{
	using char_type = typename input::char_type;
	using iter_value_type = ::std::iter_value_t<Iter>;
	if constexpr(::fast_io::buffer_input_stream<input>)
	{
		auto to_read{::std::distance(first,last)};
		for(;to_read;)
		{
			auto curr{ibuffer_curr(in)};
			auto ed{ibuffer_end(in)};
			auto remains{ed-curr};
			if constexpr(sizeof(char_type)==sizeof(iter_value_type))
			{
				if(to_read<remains)
				{
					remains=to_read;
					to_read=0;
				}
				else
				{
					to_read-=remains;
				}
			}
			else
			{
				std::size_t real_to_read{static_cast<std::size_t>(to_read)*sizeof(iter_value_type)};
				if(real_to_read<remains)
				{
					remains=real_to_read;
					to_read=0;
				}
				else
				{
					to_read-=remains/;
				}
			}

			first=type_punning_copy(curr,curr+remains,first);
			ibuffer_set_curr(in,curr);
			if(curr)
		}
	}
	else
	{
		constexpr std::size_t buffer_size{512};
		char_type buffer[buffer_size];
		auto to_read{::std::distance(first,last)};
		for(;to_read;)
		{
			std::size_t read_this_round{buffer_size};
			if(to_read>=buffer_size)
			{

			}
		}
	}
}
#endif
template<::fast_io::input_stream input>
inline constexpr void read_all_impl_decay(input in,typename input::char_type* first,typename input::char_type* last)
{
	std::ptrdiff_t diff{last-first};
	if constexpr(buffer_input_stream<input>)
	{
		auto curr{ibuffer_curr(in)};
		auto end{ibuffer_end(in)};
		std::ptrdiff_t remain_space{end-curr};
		if(diff<remain_space)[[likely]]
		{
			non_overlapped_copy_n(curr,static_cast<std::size_t>(diff),first);
			curr+=diff;
			ibuffer_set_curr(in,curr);
		}
		else
			return read_all_impl_decay_cold(in,first,last);
	}
	else
	{
		return read_all_impl_decay_cold(in,first,last);
	}
}

template<::fast_io::input_stream input,std::input_or_output_iterator Iter>
requires std::same_as<typename input::char_type,char>||std::same_as<typename input::char_type,::std::iter_value_t<Iter>>
inline constexpr void read_all_impl(input in,Iter first,Iter last)
{
	using iter_char_type = ::std::iter_value_t<Iter>;
#if 0
	using char_type = typename input::char_type;
	if constexpr(::std::contiguous_iterator<Iter>)
	{
		if constexpr(std::same_as<typename input::char_type,iter_char_type>)
		{
			read_all_impl_decay(in,::std::to_address(first),::std::to_address(last));
		}
		else
		{
			read_all_impl_decay(in,reinterpret_cast<char*>(::std::to_address(first)),reinterpret_cast<char*>(::std::to_address(last)));
		}
	}
	else
	{
		for(;first!=last;++first)
		{
			auto addr{__builtin_addressof(*first)};
			read_all_impl(in,addr,addr+1);
		}
	}
#else
	static_assert(::std::contiguous_iterator<Iter>);
	if constexpr(std::same_as<typename input::char_type,iter_char_type>)
	{
		read_all_impl_decay(in,::std::to_address(first),::std::to_address(last));
	}
	else
	{
		read_all_impl_decay(in,reinterpret_cast<char*>(::std::to_address(first)),reinterpret_cast<char*>(::std::to_address(last)));
	}
#endif
}

}

template<typename input,::std::forward_iterator Iter>
inline constexpr void read_all(input&& in,Iter first,Iter last)
{
	::fast_io::details::read_all_impl(io_ref(in),first,last);
}

template<typename input,::std::ranges::forward_range rg>
inline constexpr void read_all(input&& in,rg&& r)
{
	::fast_io::details::read_all_impl(io_ref(in),::std::ranges::begin(r),::std::ranges::end(r));
}

}
