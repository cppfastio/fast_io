#pragma once

namespace fast_io
{

inline cross_code_cvt_t<::winrt::hstring::value_type> print_alias_define(io_alias_t,::winrt::hstring const& hstr) noexcept
{
	return {{hstr.c_str(),hstr.size()}};
}

inline ::winrt::hstring strlike_construct_define(io_strlike_type_t<wchar_t,::winrt::hstring>,wchar_t const* first,wchar_t const* last)
{
	std::size_t diff{static_cast<std::size_t>(last-first)};
	if constexpr(sizeof(std::uint_least32_t)<sizeof(std::size_t))
	{
		constexpr std::uint_least32_t max_sz{UINT_LEAST32_MAX/sizeof(wchar_t)};
		if(max_sz<diff)
			::fast_io::fast_terminate();
	}
	return ::winrt::hstring(first,static_cast<std::uint_least32_t>(diff));
}

template<typename... Args>
inline ::winrt::hstring wconcat_winrt_hstring(Args&& ...args)
{
	return ::fast_io::basic_general_concat<false,wchar_t,::winrt::hstring>(::std::forward<Args>(args)...);
}

template<typename... Args>
inline ::winrt::hstring wconcatln_winrt_hstring(Args&& ...args)
{
	return ::fast_io::basic_general_concat<true,wchar_t,::winrt::hstring>(::std::forward<Args>(args)...);
}

inline constexpr io_strlike_reference_wrapper<::winrt::hstring::value_type,::winrt::hstring> io_strlike_ref(::fast_io::io_alias_t,::winrt::hstring& hstr) noexcept
{
	return {__builtin_addressof(hstr)};
}

}
