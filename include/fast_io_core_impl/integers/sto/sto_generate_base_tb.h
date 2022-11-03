#pragma once

namespace fast_io
{

namespace details
{

template<bool ebcdic,char8_t base>
requires (2<=base&&base<=36)
inline constexpr bool char_digit_to_literal_impl(char8_t& ch) noexcept
{
	using unsigned_char_type = ::std::make_unsigned_t<char8_t>;
	constexpr unsigned_char_type base_char_type(base);
	if constexpr(base<=10)
	{
		if constexpr(ebcdic)
			ch-=static_cast<unsigned_char_type>(240);
		else
			ch-=static_cast<unsigned_char_type>(u8'0');
		return base_char_type<=ch;
	}
	else
	{
		if constexpr(ebcdic)
		{

			if constexpr(base<=19)
			{
				constexpr unsigned_char_type mns{base-10};
				unsigned_char_type ch2(ch);
				ch2-=0xC1;
				unsigned_char_type ch3(ch);
				ch3-=0x81;
				ch-=0xF0;
				if(ch2<mns)
					ch=ch2+static_cast<unsigned_char_type>(10);
				else if(ch3<mns)
					ch=ch3+static_cast<unsigned_char_type>(10);
				else if(10<=ch)
					return true;
				return false;
			}
			else if constexpr(base<=28)
			{
				constexpr unsigned_char_type mns{base-19};
				unsigned_char_type ch2(ch);
				ch2-=0xC1;
				unsigned_char_type ch3(ch);
				ch3-=0x81;
				unsigned_char_type ch4(ch);
				ch2-=0xD1;
				unsigned_char_type ch5(ch);
				ch3-=0x91;
				ch-=0xF0;
				if(ch4<mns)
					ch=ch4+static_cast<unsigned_char_type>(19);
				else if(ch5<mns)
					ch=ch5+static_cast<unsigned_char_type>(19);
				else if(ch2<9)
					ch=ch2+static_cast<unsigned_char_type>(10);
				else if(ch3<9)
					ch=ch3+static_cast<unsigned_char_type>(10);
				else if(10<=ch)
					return true;
				return false;
			}
			else
			{
				constexpr unsigned_char_type mns{base-27};
				unsigned_char_type ch2(ch);
				ch2-=0xC1;
				unsigned_char_type ch3(ch);
				ch3-=0x81;
				unsigned_char_type ch4(ch);
				ch4-=0xD1;
				unsigned_char_type ch5(ch);
				ch5-=0x91;
				unsigned_char_type ch6(ch);
				ch6-=0xE2;
				unsigned_char_type ch7(ch);
				ch7-=0xA2;
				ch-=0xF0;
				if(ch6<mns)
					ch=ch6+static_cast<unsigned_char_type>(27);
				else if(ch7<mns)
					ch=ch7+static_cast<unsigned_char_type>(27);
				else if(ch4<9)
					ch=ch4+static_cast<unsigned_char_type>(19);
				else if(ch5<9)
					ch=ch5+static_cast<unsigned_char_type>(19);
				else if(ch2<9)
					ch=ch2+static_cast<unsigned_char_type>(10);
				else if(ch3<9)
					ch=ch3+static_cast<unsigned_char_type>(10);
				else if(10<=ch)
					return true;
				return false;
			}
		}
		else
		{
			constexpr unsigned_char_type mns{base-10};
			unsigned_char_type ch2(ch);
			ch2-=u8'A';
			unsigned_char_type ch3(ch);
			ch3-=u8'a';
			ch-=u8'0';
			if(ch2<mns)
				ch=ch2+static_cast<unsigned_char_type>(10);
			else if(ch3<mns)
				ch=ch3+static_cast<unsigned_char_type>(10);
			else if(10<=ch)
				return true;
			return false;
		}
	}
}

template<bool isebcdic,bool partial,char8_t base>
inline constexpr std::size_t calculate_max_available_sto_tb_size() noexcept
{
	constexpr char8_t mx{std::numeric_limits<char8_t>::max()};
	std::size_t n{static_cast<std::size_t>(mx)+1};
	if constexpr(partial)
	{
		for(;n;--n)
		{
			char8_t ch{static_cast<char8_t>(n-1)};
			if(!char_digit_to_literal_impl<isebcdic,base>(ch))
			{
				break;
			}
		}
		if(n==0)
		{
			::fast_io::fast_terminate();
		}
		return n;
	}
	else
	{
		return mx;
	}
}

template<bool isebcdic,bool partial,char8_t base>
inline constexpr auto generate_sto_base_tb() noexcept
{
	constexpr char8_t mx{std::numeric_limits<char8_t>::max()};

	::fast_io::freestanding::array<char8_t,calculate_max_available_sto_tb_size<isebcdic,partial,base>()> buffer;
	for(char16_t i{};i!=buffer.size();++i)
	{
		char8_t ch{static_cast<char8_t>(i)};
		if(char_digit_to_literal_impl<isebcdic,base>(ch))
		{
			ch=mx;
		}
		buffer[i]=ch;
	}
	return buffer;
}

template<bool isebcdic,bool partial,char8_t base>
inline constexpr auto sto_base_tb{generate_sto_base_tb<isebcdic,partial,base>()};

}

}
