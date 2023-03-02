#pragma once

namespace fast_io::details
{

template<::std::integral char_type,bool upper>
inline constexpr auto charliteralofnumber(char8_t v) noexcept
{
	if(v<10u)
	{
		return ::fast_io::char_literal_add<char_type>(v);
	}
	else if constexpr(::fast_io::details::is_ebcdic<char_type>)
	{
		if(v<19u)
		{
			return ::fast_io::char_literal_add<char_type,upper?u8'A':u8'a'>(static_cast<char8_t>(v-UINT8_C(10)));
		}
		else if(v<28u)
		{
			return ::fast_io::char_literal_add<char_type,upper?u8'J':u8'j'>(static_cast<char8_t>(v-UINT8_C(19)));
		}
		else
		{
			return ::fast_io::char_literal_add<char_type,upper?u8'S':u8's'>(static_cast<char8_t>(v-UINT8_C(28)));
		}
	}
	else
	{
		return ::fast_io::char_literal_add<char_type,upper?u8'A':u8'a'>(static_cast<char8_t>(v-UINT8_C(10)));
	}
}

template<::std::integral char_type,::std::size_t base,bool upper>
requires (2<=base&&base<=36)
inline constexpr auto generate_digits_table() noexcept
{
	constexpr char8_t char8base{static_cast<char8_t>(base)};
	constexpr
		::std::size_t n{base*base*2};
	constexpr
		::std::size_t base2{base<<1};
	::fast_io::freestanding::array<char_type,n> tb;
	for(char8_t i{};i!=char8base;++i)
	{
		::std::size_t const ibase{static_cast<::std::size_t>(i)*base2};
		auto ch0{::fast_io::details::charliteralofnumber<char_type,upper>(i)};
		for(char8_t j{};j!=char8base;++j)
		{
			::std::size_t const jsz{static_cast<::std::size_t>(j)};
			::std::size_t const ibpjsz{ibase+(jsz<<1)};
			tb[ibpjsz]=ch0;
			tb[ibpjsz+1]=::fast_io::details::charliteralofnumber<char_type,upper>(j);
		}
	}
	return tb;
}

template<::std::integral char_type,::std::size_t base,bool upper>
inline constexpr auto digits_table_impl{generate_digits_table<char_type,base,upper>()};

template<::std::integral char_type,::std::size_t base,bool upper>
inline constexpr auto& get_digits_table() noexcept
{
	constexpr bool up{(base<=10u)?false:upper};
	if constexpr(::fast_io::details::is_ebcdic<char_type>||
		(::std::same_as<char_type,wchar_t>&&::fast_io::details::wide_is_none_utf_endian))
	{
		return ::fast_io::details::digits_table_impl<char_type,base,up>;
	}
	else if constexpr(sizeof(char_type)==sizeof(char8_t))
	{
		return ::fast_io::details::digits_table_impl<char8_t,base,up>;
	}
	else if constexpr(sizeof(char_type)==sizeof(char16_t))
	{
		return ::fast_io::details::digits_table_impl<char16_t,base,up>;
	}
	else if constexpr(sizeof(char_type)==sizeof(char32_t))
	{
		return ::fast_io::details::digits_table_impl<char32_t,base,up>;
	}
	else
	{
		return ::fast_io::details::digits_table_impl<char_type,base,up>;
	}
}

template<::std::integral char_type,::std::size_t base,bool upper>
requires (2<=base&&base<=36)
inline constexpr auto const *digits_table{get_digits_table<char_type,base,upper>().data()};

}
