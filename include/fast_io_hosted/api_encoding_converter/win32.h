#pragma once

namespace fast_io
{

namespace details
{
template<typename allocator_type,std::integral char_type,typename Func>
requires (sizeof(char_type)!=sizeof(char16_t))
inline auto nt_api_common_code_cvt_impl(char_type const* filename_c_str,std::size_t filename_c_str_len,Func callback)
{
	basic_nt_api_encoding_converter<allocator_type> converter(filename_c_str,filename_c_str_len);
	if constexpr(::fast_io::details::api_common_has_size_overload<char16_t,Func>)
		return callback(converter.native_c_str(),converter.size());
	else
		return callback(converter.native_c_str());
}

}

template<typename allocator_type = typename nt_api_encoding_converter::allocator_type,
	typename T,typename Func>
requires (::fast_io::constructible_to_os_c_str<T>)
inline auto nt_api_common(T const& t,Func callback)
{
	using char16_const_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= char16_t const*;
	constexpr bool has_size_overload{::fast_io::details::api_common_has_size_overload<char16_t,Func>};
	if constexpr(::std::is_array_v<T>)
	{
		using cstr_char_type = std::remove_extent_t<T>;
		auto p{t};
		if constexpr(sizeof(cstr_char_type)==sizeof(char16_t))
		{
			if constexpr(has_size_overload)
				return callback(reinterpret_cast<char16_const_may_alias_ptr>(p),::fast_io::details::cal_array_size(t));
			else
				return callback(reinterpret_cast<char16_const_may_alias_ptr>(p));
		}
		else
		{
			return ::fast_io::details::nt_api_common_code_cvt_impl<allocator_type>(p,::fast_io::details::cal_array_size(t),callback);
		}
	}
	else if constexpr(type_has_c_str_method<T>)
	{
		using cstr_char_type = std::remove_pointer_t<decltype(t.c_str())>;
		if constexpr(sizeof(cstr_char_type)==sizeof(char16_t))
		{
			if constexpr(has_size_overload)
			{
				if constexpr(::std::ranges::contiguous_range<std::remove_cvref_t<T>>)
				{
					return callback(reinterpret_cast<char16_const_may_alias_ptr>(t.c_str()),::std::ranges::size(t));
				}
				else if constexpr(::fast_io::details::cxx_std_filesystem_pseudo_concept<std::remove_cvref_t<T>>)
				{
					auto const& native{t.native()};
					return callback(reinterpret_cast<char16_const_may_alias_ptr>(native.c_str()),native.size());
				}
				else
				{
					auto ptr{t.c_str()};
					return callback(reinterpret_cast<char16_const_may_alias_ptr>(ptr),::fast_io::cstr_len(ptr));
				}
			}
			else
			{
				return callback(reinterpret_cast<char16_const_may_alias_ptr>(t.c_str()));
			}
		}
		else if constexpr(::std::ranges::contiguous_range<std::remove_cvref_t<T>>)
		{
			return ::fast_io::details::nt_api_common_code_cvt_impl<allocator_type>(::std::ranges::data(t),::std::ranges::size(t),callback);
		}
		else if constexpr(::fast_io::details::cxx_std_filesystem_pseudo_concept<std::remove_cvref_t<T>>)
		{
			auto const& native{t.native()};
			return ::fast_io::details::nt_api_common_code_cvt_impl<allocator_type>(native.c_str(),native.size(),callback);
		}
		else
		{
			auto ptr{t.c_str()};
			return ::fast_io::details::nt_api_common_code_cvt_impl<allocator_type>(ptr,::fast_io::cstr_len(ptr),callback);
		}
	}
	else	//types like std::basic_string_view, we must allocate new space to hold that type
	{
		using strvw_char_type = std::remove_pointer_t<decltype(t.data())>;
		if constexpr(sizeof(strvw_char_type)==sizeof(char16_t))
		{
			return ::fast_io::details::api_common_copy_append0_strvw_case_impl<allocator_type,char16_t>(reinterpret_cast<char16_const_may_alias_ptr>(t.data()),t.length(),callback);
		}
		else
		{
			return ::fast_io::details::nt_api_common_code_cvt_impl<allocator_type>(t.data(),t.length(),callback);
		}
	}
}

template<typename allocator_type = typename win32_api_encoding_converter::allocator_type,
	typename T,typename Func>
requires (::fast_io::constructible_to_os_c_str<T>)
inline auto win32_api_common_9xa(T const& t,Func callback)
{
	return posix_api_common<allocator_type>(t,callback);
}

template<typename allocator_type = typename win32_api_encoding_converter::allocator_type,
	typename T,typename Func>
requires (::fast_io::constructible_to_os_c_str<T>)
inline auto win32_api_common(T const& t,Func callback)
{
	if constexpr(::fast_io::win32_family::native==win32_family::ansi_9x)
		return posix_api_common<allocator_type>(t,callback);
	else
		return nt_api_common<allocator_type>(t,callback);
}

template<win32_family family,
	typename allocator_type = typename win32_api_encoding_converter::allocator_type,
	typename T,typename Func>
requires (::fast_io::constructible_to_os_c_str<T>)
inline auto win32_family_api_common(T const& t,Func callback)
{
	if constexpr(family==win32_family::ansi_9x)
		return posix_api_common<allocator_type>(t,callback);
	else
		return nt_api_common<allocator_type>(t,callback);
}

}
