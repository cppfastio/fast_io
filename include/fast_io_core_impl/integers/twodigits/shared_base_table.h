#pragma once

namespace fast_io::details
{

template<std::integral char_type,std::size_t base,bool upper>
inline constexpr auto cal_content() noexcept
{
	constexpr std::size_t chars{2};
	constexpr std::size_t pw{base*base};
	::fast_io::freestanding::array<::fast_io::freestanding::array<char_type,chars>,pw> vals{};
	for(std::size_t i(1);i<pw;++i)
	{
		auto& val(vals[i]);
		val=vals[i-1];
		std::size_t j(chars);
		for(;j--;)
		{
			if(val[j]==base-1)
				val[j]=0;
			else
				break;
		}
		++val[j];
	}
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	for(auto &e : vals)
	{
		for(auto &e1 : e)
		{
			unsigned_char_type val{};
			if constexpr(::fast_io::details::is_ebcdic<char_type>)
			{
/*
http://www.astrodigital.org/digital/ebcdic.html
*/
				if(e1<10)
					val=0xF0;
				else if(e1<19)
				{
					if constexpr(upper)
						val=0xC1-10;
					else
						val=0x81-10;
				}
				else if(e1<28)
				{
					if constexpr(upper)
						val=0xD1-19;
					else
						val=0x91-19;
				}
				else
				{
					if constexpr(upper)
						val=0xE2-28;
					else
						val=0xA2-28;
				}
			}
			else
			{
				if(e1<10)
					val=0x30;
				else
				{
					if constexpr(upper)
						val=0x41-10;
					else
						val=0x61-10;
				}
			}
			e1 = static_cast<char_type>(static_cast<unsigned_char_type>(static_cast<unsigned_char_type>(e1)+val));
		}
	}
	if constexpr(::std::same_as<char_type,wchar_t>&&wide_is_none_utf_endian)
	{
		for(auto &e : vals)
			for(auto &e1 : e)
			{
				e1=static_cast<wchar_t>(::fast_io::byte_swap(static_cast<unsigned_char_type>(e1)));
			}
	}
	return vals;
}

template<std::integral char_type,std::size_t base,bool upper>
inline constexpr auto shared_inline_constexpr_base_table_tb{cal_content<char_type,base,upper>()};

template<std::integral char_type,std::size_t base,bool upper>
inline constexpr auto& get_shared_inline_constexpr_base_table() noexcept
{
	if constexpr(is_ebcdic<char_type>)
		return shared_inline_constexpr_base_table_tb<char_type,base,upper>;
	else if constexpr(sizeof(char_type)==1)
		return shared_inline_constexpr_base_table_tb<char8_t,base,upper>;
	else if constexpr(sizeof(char_type)==2&&(!::std::same_as<char_type,wchar_t>||!wide_is_none_utf_endian))
		return shared_inline_constexpr_base_table_tb<char16_t,base,upper>;
	else if constexpr(sizeof(char_type)==4&&(!::std::same_as<char_type,wchar_t>||!wide_is_none_utf_endian))
		return shared_inline_constexpr_base_table_tb<char32_t,base,upper>;
	else
		return shared_inline_constexpr_base_table_tb<char_type,base,upper>;
}

}
