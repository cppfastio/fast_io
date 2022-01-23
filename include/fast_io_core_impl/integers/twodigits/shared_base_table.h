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
	if constexpr(is_ebcdic<char_type>)
	{
/*
http://www.astrodigital.org/digital/ebcdic.html
*/
		for(auto &e : vals)
			for(auto &e1 : e)
				if(e1<10)
					e1+=0xF0;
				else if(e1<19)
				{
					if constexpr(upper)
						e1+=0xC1-10;
					else
						e1+=0x81-10;
				}
				else if(e1<28)
				{
					if constexpr(upper)
						e1+=0xD1-19;
					else
						e1+=0x91-19;
				}
				else
				{
					if constexpr(upper)
						e1+=0xE2-28;
					else
						e1+=0xA2-28;
				}
	}
	else
	{
		for(auto &e : vals)
			for(auto &e1 : e)
				if(e1<10)
					e1+=0x30;
				else
				{
					if constexpr(upper)
						e1+=0x41-10;
					else
						e1+=0x61-10;
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
	else if constexpr(sizeof(char_type)==2)
		return shared_inline_constexpr_base_table_tb<char16_t,base,upper>;
	else if constexpr(sizeof(char_type)==4)
		return shared_inline_constexpr_base_table_tb<char32_t,base,upper>;
	else
		return shared_inline_constexpr_base_table_tb<char_type,base,upper>;
}

}
