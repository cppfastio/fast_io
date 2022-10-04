#pragma once

namespace fast_io
{

template<std::integral ch_type,typename traits_type>
inline constexpr cross_code_cvt_t<ch_type> print_alias_define(::fast_io::io_alias_t,::ATL::CStringT<ch_type, traits_type> const& atl_mfccstr) noexcept
{
	return {{static_cast<ch_type const*>(atl_mfccstr),static_cast<std::size_t>(atl_mfccstr.GetLength())}};
}


template<std::integral cross_ch_type,std::integral ch_type,typename traits_type>
inline ::ATL::CStringT<ch_type, traits_type> strlike_construct_define(io_strlike_type_t<cross_ch_type,::ATL::CStringT<ch_type, traits_type>>,cross_ch_type const* first,cross_ch_type const* last)
{
	std::size_t diff{static_cast<std::size_t>(last-first)};
	if constexpr(sizeof(std::uint_least32_t)<sizeof(std::size_t))
	{
		constexpr std::uint_least32_t max_sz{INT_LEAST32_MAX/sizeof(ch_type)};
		if(max_sz<diff)
			::fast_io::fast_terminate();
	}
	return ::ATL::CStringT<ch_type, traits_type>(first,static_cast<std::int_least32_t>(static_cast<std::uint_least32_t>(diff)));
}

template<typename... Args>
inline CString concat_atl_cstring(Args&& ...args)
{
	return ::fast_io::basic_general_concat<false,char,CString>(::std::forward<Args>(args)...);
}

template<typename... Args>
inline CString concatln_atl_cstring(Args&& ...args)
{
	return ::fast_io::basic_general_concat<true,char,CString>(::std::forward<Args>(args)...);
}

}
