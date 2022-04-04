#pragma once

namespace fast_io
{

namespace details::decay
{

template<std::integral char_type,typename T,typename... Args>
inline constexpr std::size_t lc_calculate_scatter_dynamic_reserve_size_with_scatter(basic_lc_all<char_type> const* all,[[maybe_unused]] T t,Args... args)
{
	if constexpr((!(lc_dynamic_reserve_printable<char_type,T>||lc_scatter_printable<char_type,T>)&&((!(lc_dynamic_reserve_printable<char_type,Args>||lc_scatter_printable<char_type,Args>))&&...)))
		return calculate_scatter_dynamic_reserve_size_with_scatter(t,args...);
	else if constexpr(lc_dynamic_reserve_printable<char_type,T>)
	{
		std::size_t res{print_reserve_size(all,t)};
		if constexpr(sizeof...(Args)==0)
			return res;
		else
			return ::fast_io::details::intrinsics::add_or_overflow_die(res,lc_calculate_scatter_dynamic_reserve_size_with_scatter<char_type>(all,args...));
	}
	else if constexpr(lc_scatter_printable<char_type,T>)
	{
		std::size_t res{print_scatter_define(all,t).len};
		if constexpr(sizeof...(Args)==0)
			return res;
		else
			return ::fast_io::details::intrinsics::add_or_overflow_die(res,lc_calculate_scatter_dynamic_reserve_size_with_scatter<char_type>(all,args...));
	}
	else if constexpr(dynamic_reserve_printable<char_type,T>)
	{
		std::size_t res{print_reserve_size(io_reserve_type<char_type,T>,t)};
		if constexpr(sizeof...(Args)==0)
			return res;
		else
			return ::fast_io::details::intrinsics::add_or_overflow_die(res,lc_calculate_scatter_dynamic_reserve_size_with_scatter<char_type>(all,args...));
	}
	else if constexpr(scatter_printable<char_type,T>)
	{
		std::size_t res{print_scatter_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,t).len};
		if constexpr(sizeof...(Args)==0)
			return res;
		else
			return ::fast_io::details::intrinsics::add_or_overflow_die(res,lc_calculate_scatter_dynamic_reserve_size_with_scatter<char_type>(all,args...));
	}
	else
	{
		if constexpr(sizeof...(Args)==0)
			return 0;
		else
			return lc_calculate_scatter_dynamic_reserve_size_with_scatter<char_type>(all,args...);
	}
}

template<bool line,typename ptr_type,std::integral char_type,typename T,typename... Args>
inline constexpr ptr_type lc_print_reserve_define_chain_scatter_impl(basic_lc_all<char_type> const* all,ptr_type p,T t,Args ...args)
{
	if constexpr((!(lc_dynamic_reserve_printable<char_type,T>||lc_scatter_printable<char_type,T>)&&((!(lc_dynamic_reserve_printable<char_type,Args>||lc_scatter_printable<char_type,Args>))&&...)))
		return print_reserve_define_chain_scatter_impl<line>(p,t,args...);
	else if constexpr(lc_dynamic_reserve_printable<char_type,T>)
		p = print_reserve_define(all,p,t);
	else if constexpr(lc_scatter_printable<char_type,T>)
	{
		basic_io_scatter_t<char_type> sc{print_scatter_define(all,t)};
		p = non_overlapped_copy_n(sc.base,sc.len,p);
	}
	else if constexpr(dynamic_reserve_printable<char_type,T>||reserve_printable<char_type,T>)
		p = print_reserve_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,p,t);
	else
	{
		basic_io_scatter_t<char_type> sc{print_scatter_define(io_reserve_type<char_type,std::remove_cvref_t<T>>,t)};
		p = non_overlapped_copy_n(sc.base,sc.len,p);
	}
	if constexpr(sizeof...(Args)==0)
	{
		if constexpr(line)
		{
			*p=char_literal_v<u8'\n',char_type>;
			++p;
		}
		return p;
	}
	else
		return lc_print_reserve_define_chain_scatter_impl<line>(all,p,args...);
}

template<std::integral ch_type,typename T>
inline constexpr basic_io_scatter_t<ch_type> lc_print_scatter_define_extract_one(basic_lc_all<typename T::char_type> const* all,T t)
{
	return print_scatter_define(all,t);
}

template<bool line,typename T,typename ...Args>
inline constexpr T lc_concat_decay_impl(basic_lc_all<typename T::value_type> const* all,Args ...args)
{
	using ch_type = typename T::value_type;
	using ostring_ref_type =  basic_ostring_ref<ch_type,typename T::traits_type,typename T::allocator_type>;
	if constexpr(((!(lc_dynamic_reserve_printable<ch_type,Args>||lc_scatter_printable<ch_type,Args>||lc_printable<ostring_ref_type,Args>))&&...))
	{
		return ::fast_io::details::decay::basic_general_concat_phase1_decay_impl<line,ch_type,T>(args...);
	}
	else if constexpr(sizeof...(Args)==0)
	{
		if constexpr(line)
		{
			return T(1,char_literal_v<u8'\n',ch_type>);
		}
		else
		{
			return {};
		}
	}
	else if constexpr(((reserve_printable<ch_type,Args>||scatter_printable<ch_type,Args>||dynamic_reserve_printable<ch_type,Args>||lc_scatter_printable<ch_type,Args>||lc_dynamic_reserve_printable<ch_type,Args>)&&...))
	{
		constexpr std::size_t sz_with_line{static_cast<std::size_t>(line)};
		if constexpr((!line)&&sizeof...(args)==1&&(lc_scatter_printable<ch_type,Args>&&...))
		{
			basic_io_scatter_t<ch_type> scatter{lc_print_scatter_define_extract_one<ch_type>(all,args...)};
			return T(scatter.base,scatter.len);
		}
		else
		{
			std::size_t total_size{::fast_io::details::intrinsics::add_or_overflow_die(sz_with_line,lc_calculate_scatter_dynamic_reserve_size_with_scatter<ch_type>(all,args...))};
			T str;
			str.reserve(total_size);
			set_basic_string_ptr(str,lc_print_reserve_define_chain_scatter_impl<line>(all,str.data(),args...));
			return str;
		}
	}
	else
	{
		T str;
		basic_ostring_ref<ch_type,typename T::traits_type,typename T::allocator_type> ref{__builtin_addressof(str)};
		lc_print_status_define_further_decay<line>(all,ref,args...);
		return str;
	}
}

template<typename T>
concept l10ntypes_impl = requires(T& loc)
{
	{loc.loc} -> std::same_as<::fast_io::lc_locale>;
};

}

template<typename T,typename ...Args>
inline constexpr T basic_lc_concat_decay(basic_lc_all<typename T::value_type> const* all,Args ...args)
{
	return ::fast_io::details::decay::lc_concat_decay_impl<false,T>(all,args...);
}
template<typename T,typename ...Args>
inline constexpr T basic_lc_concatln_decay(basic_lc_all<typename T::value_type> const* all,Args ...args)
{
	return ::fast_io::details::decay::lc_concat_decay_impl<true,T>(all,args...);
}

template<typename T,typename ...Args>
inline constexpr T basic_lc_concat(basic_lc_all<typename T::value_type> const* all,Args ...args)
{
	return ::fast_io::details::decay::lc_concat_decay_impl<false,T>(all,io_print_forward<typename T::value_type>(io_print_alias(args))...);
}

template<typename T,typename ...Args>
inline constexpr T basic_lc_concatln(basic_lc_all<typename T::value_type> const* all,Args ...args)
{
	return ::fast_io::details::decay::lc_concat_decay_impl<true,T>(all,io_print_forward<typename T::value_type>(io_print_alias(args))...);
}

template<std::integral ch_type,typename ...Args>
inline
#if __cpp_lib_constexpr_string >= 201907L
constexpr
#endif
std::basic_string<ch_type> lc_concat(basic_lc_all<ch_type> const* all,Args ...args)
{
	return ::fast_io::details::decay::lc_concat_decay_impl<false,std::basic_string<ch_type>>(all,io_print_forward<ch_type>(io_print_alias(args))...);
}

template<std::integral ch_type,typename ...Args>
inline
#if __cpp_lib_constexpr_string >= 201907L
constexpr
#endif
std::basic_string<ch_type> lc_concatln(basic_lc_all<ch_type> const* all,Args ...args)
{
	return ::fast_io::details::decay::lc_concat_decay_impl<true,ch_type>(all,io_print_forward<ch_type>(io_print_alias(args))...);
}

template<::fast_io::details::decay::l10ntypes_impl T,typename ...Args>
inline
#if __cpp_lib_constexpr_string >= 201907L
constexpr
#endif
std::string lc_concat(T& loc,Args&& ...args)
{
	return ::fast_io::details::decay::lc_concat_decay_impl<false,std::string>(loc.loc.all,io_print_forward<char>(io_print_alias(args))...);
}

template<::fast_io::details::decay::l10ntypes_impl T,typename ...Args>
inline
#if __cpp_lib_constexpr_string >= 201907L
constexpr
#endif
std::string lc_concatln(T& loc,Args&& ...args)
{
	return ::fast_io::details::decay::lc_concat_decay_impl<true,std::string>(loc.loc.all,io_print_forward<char>(io_print_alias(args))...);
}

template<::fast_io::details::decay::l10ntypes_impl T,typename ...Args>
inline
#if __cpp_lib_constexpr_string >= 201907L
constexpr
#endif
std::basic_string<wchar_t> wlc_concat(T& loc,Args&& ...args)
{
	return ::fast_io::details::decay::lc_concat_decay_impl<false,std::basic_string<wchar_t>>(loc.loc.wall,io_print_forward<wchar_t>(io_print_alias(args))...);
}

template<::fast_io::details::decay::l10ntypes_impl T,typename ...Args>
inline
#if __cpp_lib_constexpr_string >= 201907L
constexpr
#endif
std::basic_string<wchar_t> wlc_concatln(T& loc,Args&& ...args)
{
	return ::fast_io::details::decay::lc_concat_decay_impl<true,std::basic_string<wchar_t>>(loc.loc.wall,io_print_forward<wchar_t>(io_print_alias(args))...);
}

template<::fast_io::details::decay::l10ntypes_impl T,typename ...Args>
inline
#if __cpp_lib_constexpr_string >= 201907L
constexpr
#endif
std::u8string u8lc_concat(T& loc,Args&& ...args)
{
	return ::fast_io::details::decay::lc_concat_decay_impl<false,std::u8string>(loc.loc.u8all,io_print_forward<char8_t>(io_print_alias(args))...);
}

template<::fast_io::details::decay::l10ntypes_impl T,typename ...Args>
inline
#if __cpp_lib_constexpr_string >= 201907L
constexpr
#endif
std::u8string u8lc_concatln(T& loc,Args&& ...args)
{
	return ::fast_io::details::decay::lc_concat_decay_impl<true,std::u8string>(loc.loc.u8all,io_print_forward<char8_t>(io_print_alias(args))...);
}

template<::fast_io::details::decay::l10ntypes_impl T,typename ...Args>
inline
#if __cpp_lib_constexpr_string >= 201907L
constexpr
#endif
std::u16string u16lc_concat(T& loc,Args&& ...args)
{
	return ::fast_io::details::decay::lc_concat_decay_impl<false,std::u16string>(loc.loc.u16all,io_print_forward<char16_t>(io_print_alias(args))...);
}

template<::fast_io::details::decay::l10ntypes_impl T,typename ...Args>
inline
#if __cpp_lib_constexpr_string >= 201907L
constexpr
#endif
std::u16string u16lc_concatln(T& loc,Args&& ...args)
{
	return ::fast_io::details::decay::lc_concat_decay_impl<true,std::u16string>(loc.loc.u16all,io_print_forward<char16_t>(io_print_alias(args))...);
}

template<::fast_io::details::decay::l10ntypes_impl T,typename ...Args>
inline
#if __cpp_lib_constexpr_string >= 201907L
constexpr
#endif
std::u32string u32lc_concat(T& loc,Args&& ...args)
{
	return ::fast_io::details::decay::lc_concat_decay_impl<false,std::u32string>(loc.loc.u32all,io_print_forward<char32_t>(io_print_alias(args))...);
}

template<::fast_io::details::decay::l10ntypes_impl T,typename ...Args>
inline
#if __cpp_lib_constexpr_string >= 201907L
constexpr
#endif
std::u32string u32lc_concatln(T& loc,Args&& ...args)
{
	return ::fast_io::details::decay::lc_concat_decay_impl<true,std::u32string>(loc.loc.u32all,io_print_forward<char32_t>(io_print_alias(args))...);
}

}
