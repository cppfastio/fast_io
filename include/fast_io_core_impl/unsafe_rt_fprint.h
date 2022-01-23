#pragma once

namespace fast_io
{

/*
format string vulnerablities are hard to avoid when you deal with format string

We have to put restrictions on them and mark them as unsafe.
*/

template<std::integral ch_type>
struct basic_unsafe_rt_format_string_view
{
	using char_type = ch_type;
	char_type const* data_first_ptr{};
	char_type const* data_last_ptr{};
	inline explicit constexpr basic_unsafe_rt_format_string_view() noexcept = default;
	inline explicit constexpr basic_unsafe_rt_format_string_view(char_type const* dat,std::size_t le) noexcept:data_first_ptr{dat},data_last_ptr{dat+le}{}
	template<::fast_io::freestanding::contiguous_iterator Iter>
	requires (std::same_as<::fast_io::freestanding::iter_value_t<Iter>,char_type>)
	inline explicit constexpr basic_unsafe_rt_format_string_view(Iter first,Iter last):
		data_first_ptr{::fast_io::freestanding::to_address(first)},
		data_last_ptr{::fast_io::freestanding::to_address(last)}{}
#if __STDC_HOSTED__==1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED==1) && __cpp_lib_ranges >= 202106L
	template<::std::ranges::contiguous_range rg>
	requires (std::same_as<::std::ranges::range_value_t<std::remove_cvref_t<rg>>,char_type>&&!::std::is_array_v<rg>&&::std::is_lvalue_reference_v<rg&>)
	inline explicit constexpr basic_unsafe_rt_format_string_view(rg& r):data_first_ptr{::std::ranges::data(r)},data_last_ptr{::std::ranges::data(r)+::std::ranges::size(r)}{}
#endif
};

using unsafe_rt_format_string_view = basic_unsafe_rt_format_string_view<char>;
using wunsafe_rt_format_string_view = basic_unsafe_rt_format_string_view<wchar_t>;
using u8unsafe_rt_format_string_view = basic_unsafe_rt_format_string_view<char8_t>;
using u16unsafe_rt_format_string_view = basic_unsafe_rt_format_string_view<char16_t>;
using u32unsafe_rt_format_string_view = basic_unsafe_rt_format_string_view<char32_t>;

namespace details
{

template<typename T,typename U,typename... Args>
inline constexpr void print_para_at_pos(T out,std::size_t sz,U u,Args ...args)
{
	if(sz==0)
		decay::print_control(out,u);
	if constexpr(sizeof...(Args)!=0)
		print_para_at_pos(out,sz-1,args...);
}

enum class fprint_args_num_para_enum
{
one,less_than_11,other
};

template<fprint_args_num_para_enum ag>
struct fprint_args_num_para
{
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	std::conditional_t<ag==fprint_args_num_para_enum::one,empty,std::size_t> arg_num;
};

template<std::integral char_type,typename write_functor,typename functor,fprint_args_num_para_enum ag>
inline constexpr void unsafe_rt_fprint_impl(char_type* it,char_type* ed,write_functor write_func,functor func,
[[maybe_unused]] fprint_args_num_para<ag> arg)
{
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	std::size_t current_parameter{};
	std::size_t swap_parameters{};
	constexpr auto left_bracket{char_literal_v<u8'{',char_type>};
	constexpr auto right_bracket{char_literal_v<u8'}',char_type>};
	constexpr unsigned_char_type zeroch(static_cast<unsigned_char_type>(char_literal_v<u8'0',char_type>));
	std::size_t maximum_allowed_args;
	std::size_t args_num;
	if constexpr(ag==fprint_args_num_para_enum::one)
	{
		maximum_allowed_args=100;
		args_num=1;
	}
	else if constexpr(ag==fprint_args_num_para_enum::less_than_11)
	{
		maximum_allowed_args=100;
		args_num=arg.arg_num;
	}
	else
	{
		args_num=arg.arg_num;
		maximum_allowed_args=static_cast<std::size_t>(args_num*static_cast<std::size_t>(10));
	}
	for(;it!=ed;)
	{
		auto next_it{::fast_io::freestanding::find(it,ed,left_bracket)};
		std::ptrdiff_t diff{ed-next_it};
		if(diff<2)
			break;
		auto next_ch{next_it[1]};
		if(next_ch==left_bracket)
		{
			write_func(it,next_it+1);
			it=next_it+2;
			continue;
		}
		else if(next_ch==right_bracket)
		{
			if(current_parameter==args_num)
				write_func(it,next_it+2);
			else
			{
				write_func(it,next_it);
				if constexpr(ag==fprint_args_num_para_enum::one)
				{
					func();
				}
				else
				{
					func(current_parameter);
				}
				++current_parameter;
			}
			it=next_it+2;
			continue;
		}
		if(diff==2||swap_parameters==maximum_allowed_args)
			break;
		if constexpr(ag==fprint_args_num_para_enum::one)
		{
			if((next_ch!=zeroch)|(next_it[2]!=right_bracket))
				break;
			write_func(it,next_it);
			func();
			++swap_parameters;
			it=next_it+3;
		}
		else if constexpr(ag==fprint_args_num_para_enum::less_than_11)
		{
			unsigned_char_type const max_sz{static_cast<unsigned_char_type>(args_num)};
			if(next_it[2]!=right_bracket)
				break;
			unsigned_char_type rn(static_cast<unsigned_char_type>(next_ch)-zeroch);
			if(rn<max_sz)[[likely]]
			{
				write_func(it,next_it);
				func(static_cast<std::size_t>(rn));
				++swap_parameters;
				it=next_it+3;
			}
			else
				break;
		}
		else
		{
			std::size_t n;
			auto res{scan_int_contiguous_none_space_part_define_impl<10>(next_it+1,ed,n)};
			if((res.code!=parse_code::ok)|(res.iter==ed||*res.iter!=right_bracket))
				break;
			if(args_num<=n)
				break;
			write_func(it,next_it);
			func(static_cast<std::size_t>(n));
			++swap_parameters;
			it=res.iter+1;
		}
	}
	if(it!=ed)
	{
		write_func(it,ed);
	}
}

template<typename output>
struct unsafe_rt_fprint_parameter_write_func
{
	using char_type = typename output::char_type;
	output out;
	inline constexpr void operator()(char_type const* first,char_type const* last)
	{
		write(out,first,last);
	}
};

template<typename output,typename... Args>
requires (sizeof...(Args)!=0)
inline constexpr void unsafe_rt_fprint_freestanding_decay_impl(output out,
	typename output::char_type const* data_first_ptr,typename output::char_type const* data_last_ptr,
	Args ...args)
{
	if constexpr(sizeof...(Args)==1)
	{
		unsafe_rt_fprint_impl(data_first_ptr,data_last_ptr,
			unsafe_rt_fprint_parameter_write_func<output>{.out=out},[&]()
		{
			((details::decay::print_control(out,args)),...);
		},fprint_args_num_para<fprint_args_num_para_enum::one>{});
	}
	else
	{
		unsafe_rt_fprint_impl(data_first_ptr,data_last_ptr,unsafe_rt_fprint_parameter_write_func<output>{.out=out},
			[&](std::size_t pos)
		{
			print_para_at_pos(out,pos,args...);
		},fprint_args_num_para<sizeof...(Args)<11?fprint_args_num_para_enum::less_than_11:fprint_args_num_para_enum::other>{sizeof...(Args)});
	}
}

template<fprint_args_num_para_enum ag,std::integral char_type>
inline constexpr io_scatter_t* scatter_fprint_impl(
	char_type const* data_first_ptr,char_type const* data_last_ptr,
	io_scatter_t* scatters,
	io_scatter_t* new_scatters,fprint_args_num_para<ag> arg)
{
	if constexpr(ag==fprint_args_num_para_enum::one)
	{
		unsafe_rt_fprint_impl(data_first_ptr,data_last_ptr,
			[&new_scatters](char_type const* beg_ptr,char_type const* end_ptr)
		{
			*new_scatters={beg_ptr,(end_ptr-beg_ptr)*sizeof(char_type)};
			++new_scatters;
		},[&]()
		{
			*new_scatters=*scatters;
			++new_scatters;
		},arg);
	}
	else
	{
		unsafe_rt_fprint_impl(data_first_ptr,data_last_ptr,
			[&new_scatters](char_type const* beg_ptr,char_type const* end_ptr)
		{
			*new_scatters={beg_ptr,(end_ptr-beg_ptr)*sizeof(char_type)};
			++new_scatters;
		},[&](std::size_t pos)
		{
			*new_scatters=scatters[pos];
			++new_scatters;
		},arg);
	}
	return new_scatters;
}

inline constexpr std::size_t calculate_maxmium_allowed_args(std::size_t args_num) noexcept
{
	constexpr std::size_t hundred{100};
	constexpr std::size_t ten{10};
	std::size_t value{args_num*ten};
	if(value<hundred)
		value=hundred;
	std::size_t maximum_allowed_args{SIZE_MAX/3-args_num};
	if(value>=maximum_allowed_args)
		return maximum_allowed_args;
	else
		return value+args_num;
}

template<std::size_t args_num,output_stream output,typename ...Args>
inline constexpr void scatter_fprint_write(output out,
	typename output::char_type const* data_first_ptr,typename output::char_type const* data_last_ptr,
	io_scatter_t* scatters)
{
	constexpr std::size_t maximum_allowed_args{calculate_maxmium_allowed_args(args_num)};
	constexpr std::size_t n{maximum_allowed_args*2+1};
	io_scatter_t new_scatters[n];
	if constexpr(args_num==1)
	{
		auto res{scatter_fprint_impl(data_first_ptr,data_last_ptr,scatters,new_scatters,fprint_args_num_para<fprint_args_num_para_enum::one>{})};
		scatter_write(out,{new_scatters,res-new_scatters});
	}
	else
	{
		auto res{scatter_fprint_impl(data_first_ptr,data_last_ptr,scatters,new_scatters,
			fprint_args_num_para<args_num<11?
			fprint_args_num_para_enum::less_than_11:fprint_args_num_para_enum::other>{args_num})};
		scatter_write(out,{new_scatters,res-new_scatters});
	}
}


template<output_stream output,typename ...Args>
inline constexpr void unsafe_rt_fprint_fallback(output out,typename output::char_type const* data_first_ptr,typename output::char_type const* data_last_ptr,Args ...args)
{
	using char_type = typename output::char_type;
	if constexpr((scatter_output_stream<output>&&((scatter_printable<typename output::char_type,Args>||reserve_printable<typename output::char_type,Args>||dynamic_reserve_printable<typename output::char_type,Args>)&&...)))
	{
		constexpr std::size_t n{sizeof...(Args)};
		io_scatter_t scatters[n];
		if constexpr((scatter_printable<typename output::char_type,Args>&&...))
		{
			decay::scatter_print_recursive<typename output::char_type>(scatters,args...);
			scatter_fprint_write<sizeof...(Args)>(out,data_first_ptr,data_last_ptr,scatters);
		}
		else if constexpr(((scatter_printable<char_type,Args>||
			reserve_printable<char_type,Args>)&&...))
		{
			constexpr std::size_t m{decay::calculate_scatter_reserve_size<char_type,Args...>()};
			char_type array[m];
			decay::scatter_print_with_reserve_recursive(array,scatters,args...);
			scatter_fprint_write<sizeof...(Args)>(out,data_first_ptr,data_last_ptr,scatters);
		}
		else
		{
			constexpr std::size_t m{decay::calculate_scatter_reserve_size<char_type,Args...>()};
			char_type array[m];
			local_operator_new_array_ptr<char_type> new_ptr(decay::calculate_scatter_dynamic_reserve_size<char_type>(args...));
			decay::scatter_print_with_dynamic_reserve_recursive(scatters,array,new_ptr.ptr,args...);
			scatter_fprint_write<sizeof...(Args)>(out,data_first_ptr,data_last_ptr,scatters);
		}
	}
	else
	{
		temporary_buffer<output> buffer{.out=out};
		auto ref{io_ref(buffer)};
		unsafe_rt_fprint_freestanding_decay_impl(ref,data_first_ptr,data_last_ptr,args...);
		flush(buffer);
	}
}

template<typename T>
concept has_unsafe_rt_fprint_status_define = std::is_trivially_copyable_v<T>&&status_output_stream<T>&&requires(T t,typename T::char_type const* p)
{
	unsafe_rt_fprint_status_define(t,p,p);
};

}

template<typename output,typename... Args>
requires (print_freestanding_decay_okay_no_status<output,Args...>&&sizeof...(Args)==0)
inline constexpr void unsafe_rt_fprint_freestanding_decay_no_status(output out,
	typename output::char_type const* data_first_ptr,typename output::char_type const* data_last_ptr,
	Args ...args)
{
	if constexpr(mutex_stream<output>)
	{
		io_lock_guard lg{out};
		decltype(auto) dout{out.unlocked_handle()};
		unsafe_rt_fprint_freestanding_decay(io_ref(dout),data_first_ptr,data_last_ptr,args...);
	}
	else if constexpr(buffer_output_stream<output>)
	{
		details::unsafe_rt_fprint_freestanding_decay_impl(out,data_first_ptr,data_last_ptr,args...);
	}
	else
	{
		details::unsafe_rt_fprint_fallback(out,data_first_ptr,data_last_ptr,args...);
	}
}

template<typename output,typename... Args>
requires ((output_stream<output>||status_output_stream<output>)&&(std::is_trivially_copyable_v<output>&&(std::is_trivially_copyable_v<Args>&&...))&&(sizeof...(Args)!=0))
inline constexpr void unsafe_rt_fprint_freestanding_decay(output out,
	typename output::char_type const* data_first_ptr,typename output::char_type const* data_last_ptr,
	Args ...args)
{
	if constexpr(status_output_stream<output>&&::fast_io::details::has_unsafe_rt_fprint_status_define<output>)
	{
		unsafe_rt_fprint_status_define(out,data_first_ptr,data_last_ptr,args...);
	}
	else if constexpr(mutex_stream<output>)
	{
		io_lock_guard lg{out};
		decltype(auto) dout{out.unlocked_handle()};
		unsafe_rt_fprint_freestanding_decay(io_ref(dout),data_first_ptr,data_last_ptr,args...);
	}
	else if constexpr(buffer_output_stream<output>)
	{
		details::unsafe_rt_fprint_freestanding_decay_impl(out,data_first_ptr,data_last_ptr,args...);
	}
	else
	{
		details::unsafe_rt_fprint_fallback(out,data_first_ptr,data_last_ptr,args...);
	}
}

template<typename output,typename... Args>
inline constexpr void unsafe_rt_fprint(output&& out,::fast_io::basic_unsafe_rt_format_string_view<typename std::remove_cvref_t<output>::char_type> view,Args&& ...args)
{
	constexpr bool no_parameters{sizeof...(Args)!=0};
	if constexpr(no_parameters)
	{
	constexpr bool device_error{::fast_io::output_stream<std::remove_cvref_t<output>>||::fast_io::status_output_stream<std::remove_cvref_t<output>>};
	if constexpr(device_error)
	{
		constexpr bool type_error{::fast_io::print_freestanding_okay<std::remove_cvref_t<output>,Args...>};
		if constexpr(type_error)
		{
			unsafe_rt_fprint_freestanding_decay(io_ref(out),view.data_first_ptr,view.data_last_ptr,io_print_forward<typename std::remove_cvref_t<output>::char_type>(io_print_alias(args))...);
		}
		else
		{
static_assert(type_error,"some types are not printable for unsafe_rt_fprint");
		}
	}
	else
	{
static_assert(device_error,"type is neither output_stream nor status_output_stream");
	}
	}
	else
	{
static_assert(no_parameters,"we need at least one parameter. Oh! You want to print string?? hmhm. Just use print instead. Stop abusing format string.");
	}
}

}
