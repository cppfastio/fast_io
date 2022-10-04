#pragma once

namespace fast_io
{

template<std::integral src_char_type,
	encoding_scheme src_scheme=execution_charset_encoding_scheme<src_char_type>(),
	encoding_scheme dst_scheme=encoding_scheme::execution_charset>
struct basic_code_converter
{
	using char_type=src_char_type;
	inline static constexpr auto from_scheme = src_scheme;
	inline static constexpr auto to_scheme = dst_scheme;
	basic_mb_state<char_type,to_scheme> state;
};

using code_converter=basic_code_converter<char>;
using wcode_converter=basic_code_converter<wchar_t>;
using u8code_converter=basic_code_converter<char8_t>;
using u16code_converter=basic_code_converter<char16_t>;
using u32code_converter=basic_code_converter<char32_t>;

template<std::integral from_char_type,std::integral to_char_type,
encoding_scheme from_scheme,encoding_scheme to_scheme>
inline constexpr std::size_t deco_reserve_size(io_reserve_type_t<to_char_type,
basic_code_converter<from_char_type,from_scheme,to_scheme>>,
basic_code_converter<from_char_type,from_scheme,to_scheme>&,std::size_t from_size) noexcept
{
	return details::cal_decorated_reserve_size<sizeof(from_char_type),sizeof(to_char_type)>(from_size);
}

template<encoding_scheme from_scheme,encoding_scheme to_scheme,
::std::contiguous_iterator from_iter,::std::contiguous_iterator to_iter>
inline constexpr to_iter deco_reserve_define(io_reserve_type_t<::std::iter_value_t<to_iter>,
basic_code_converter<::std::iter_value_t<from_iter>,from_scheme,to_scheme>>,
basic_code_converter<::std::iter_value_t<from_iter>,from_scheme,to_scheme>& conv,from_iter src_first,from_iter src_last,to_iter dst) noexcept
{
	if constexpr(std::is_pointer_v<from_iter>&&std::is_pointer_v<to_iter>)
		return details::codecvt::general_code_cvt<from_scheme,to_scheme>(conv.state,src_first,src_last,dst);
	else if constexpr(std::is_pointer_v<to_iter>)
		return details::codecvt::general_code_cvt<from_scheme,to_scheme>(conv.state,
			::std::to_address(src_first),
			::std::to_address(src_last),
			dst);
	else
	{
		return dst+(details::codecvt::general_code_cvt<from_scheme,to_scheme>(conv.state,
		::std::to_address(src_first),
		::std::to_address(src_last),
		::std::to_address(dst))-::std::to_address(dst));
	}
}

}
