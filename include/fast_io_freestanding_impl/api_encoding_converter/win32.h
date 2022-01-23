#pragma once

namespace fast_io
{

namespace details
{
template<std::integral char_type,typename Func>
requires (sizeof(char_type)!=sizeof(wchar_t))
inline auto nt_api_common_code_cvt_impl(char_type const* filename_c_str,std::size_t filename_c_str_len,Func callback)
{
	nt_api_encoding_converter converter(filename_c_str,filename_c_str_len);
	if constexpr(::fast_io::details::api_common_has_size_overload<wchar_t,Func>)
		return callback(converter.native_c_str(),converter.size());
	else
		return callback(converter.native_c_str());
}

}

template<typename T,typename Func>
requires (::fast_io::constructible_to_os_c_str<T>)
inline auto nt_api_common(T const& t,Func callback)
{
	using wchar_t_const_may_alias_ptr
#if __has_cpp_attribute(gnu::may_alias)
	[[gnu::may_alias]]
#endif
	= wchar_t const*;
	constexpr bool has_size_overload{::fast_io::details::api_common_has_size_overload<wchar_t,Func>};
	if constexpr(::std::is_array_v<T>)
	{
		using cstr_char_type = std::remove_extent_t<T>;
		auto p{t};
		if constexpr(sizeof(cstr_char_type)==sizeof(wchar_t))
		{
			if constexpr(has_size_overload)
				return callback(reinterpret_cast<wchar_t_const_may_alias_ptr>(p),::fast_io::details::cal_array_size(t));
			else
				return callback(reinterpret_cast<wchar_t_const_may_alias_ptr>(p));
		}
		else
		{
			return ::fast_io::details::nt_api_common_code_cvt_impl(p,::fast_io::details::cal_array_size(t),callback);
		}
	}
	else if constexpr(type_has_c_str_method<T>)
	{
		using cstr_char_type = std::remove_pointer_t<decltype(t.c_str())>;
		if constexpr(sizeof(cstr_char_type)==sizeof(wchar_t))
		{
			if constexpr(has_size_overload)
			{
#if __STDC_HOSTED__==1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED==1) && __has_include(<ranges>)
				if constexpr(::std::ranges::contiguous_range<std::remove_cvref_t<T>>)
				{
					return callback(reinterpret_cast<wchar_t_const_may_alias_ptr>(t.c_str()),::std::ranges::size(t));
				}
				else
#endif
				if constexpr(::fast_io::details::cxx_std_filesystem_pseudo_concept<std::remove_cvref_t<T>>)
				{
					auto const& native{t.native()};
					return callback(reinterpret_cast<wchar_t_const_may_alias_ptr>(native.c_str()),native.size());
				}
				else
				{
					auto ptr{t.c_str()};
					return callback(reinterpret_cast<wchar_t_const_may_alias_ptr>(ptr),::fast_io::cstr_len(ptr));
				}
			}
			else
			{
				return callback(reinterpret_cast<wchar_t_const_may_alias_ptr>(t.c_str()));
			}
		}
		else
#if __STDC_HOSTED__==1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED==1) && __has_include(<ranges>)
		if constexpr(::std::ranges::contiguous_range<std::remove_cvref_t<T>>)
		{
			return ::fast_io::details::nt_api_common_code_cvt_impl(::std::ranges::data(t),::std::ranges::size(t),callback);
		}
		else
#endif
		if constexpr(::fast_io::details::cxx_std_filesystem_pseudo_concept<std::remove_cvref_t<T>>)
		{
			auto const& native{t.native()};
			return ::fast_io::details::nt_api_common_code_cvt_impl(native.c_str(),native.size(),callback);
		}
		else
		{
			auto ptr{t.c_str()};
			return ::fast_io::details::nt_api_common_code_cvt_impl(ptr,::fast_io::cstr_len(ptr),callback);
		}
	}
	else	//types like std::basic_string_view, we must allocate new space to hold that type
	{
		using strvw_char_type = std::remove_pointer_t<decltype(t.data())>;
		if constexpr(sizeof(strvw_char_type)==sizeof(wchar_t))
		{
			return ::fast_io::details::api_common_copy_append0_strvw_case_impl<wchar_t>(reinterpret_cast<wchar_t_const_may_alias_ptr>(t.data()),t.length(),callback);
		}
		else
		{
			return ::fast_io::details::nt_api_common_code_cvt_impl(t.data(),t.length(),callback);
		}
	}
}

template<typename T,typename Func>
requires (::fast_io::constructible_to_os_c_str<T>)
inline auto win32_api_common_9xa(T const& t,Func callback)
{
	return posix_api_common(t,callback);
}

template<typename T,typename Func>
requires (::fast_io::constructible_to_os_c_str<T>)
inline auto win32_api_common(T const& t,Func callback)
{
	if constexpr(::fast_io::win32_family::native==win32_family::ansi_9x)
		return posix_api_common(t,callback);
	else
		return nt_api_common(t,callback);
}

template<win32_family family,typename T,typename Func>
requires (::fast_io::constructible_to_os_c_str<T>)
inline auto win32_family_api_common(T const& t,Func callback)
{
	if constexpr(family==win32_family::ansi_9x)
		return posix_api_common(t,callback);
	else
		return nt_api_common(t,callback);
}

}
