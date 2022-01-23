#pragma once

namespace fast_io
{

namespace scan_transmitter
{

inline constexpr auto single_sign(st_single([](std::integral auto ch)
{
	return (ch==u8'-')|(ch==u8'+');
}));

using single_sign_digit_t = decltype(single_sign);

inline constexpr auto single_dot(st_single([](std::integral auto ch)
{
	return ch==u8'.';
}));

using single_dot_t = decltype(single_dot);

inline constexpr auto single_comma(st_single([](std::integral auto ch)
{
	return ch==u8',';
}));

using single_comma_t = decltype(single_comma);
#ifndef _MSC_VER
template<char8_t base>
requires (2<=base&&base<=36)
inline constexpr auto until_none_digit(st_until([](std::integral auto ch)
{
	using unsigned_char_type = std::make_unsigned_t<std::remove_cvref_t<decltype(ch)>>;
	if constexpr(base<=10)
		return static_cast<unsigned_char_type>(base)<=
			static_cast<unsigned_char_type>(static_cast<unsigned_char_type>(ch)-
			static_cast<unsigned_char_type>(u8'0'));
	else
	{
		constexpr unsigned_char_type mns(base-10);
		unsigned_char_type const ch1(static_cast<unsigned_char_type>(ch)-static_cast<unsigned_char_type>(u8'0'));
		unsigned_char_type const ch2(static_cast<unsigned_char_type>(ch)-static_cast<unsigned_char_type>(u8'A'));
		unsigned_char_type const ch3(static_cast<unsigned_char_type>(ch)-static_cast<unsigned_char_type>(u8'a'));
		return (static_cast<unsigned_char_type>(10)<=ch1)&(mns<=ch2)&(mns<=ch3);
	}
}));

template<char8_t base>
using until_none_digit_t = decltype(until_none_digit<base>);
#endif
}

}