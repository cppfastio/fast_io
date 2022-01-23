#pragma once

/*
TO DO
*/

namespace fast_io::char_category
{

using charmax_t = char32_t;

inline constexpr bool is_iso14651_space(charmax_t ch) noexcept
{
	switch(ch)
	{
case 0x9:case 0xA:case 0xB:case 0xC:case 0xD:case 0x20:
case 0x1680:case 0x2000:case 0x2001:case 0x2002:case 0x2003:
case 0x2004:case 0x2005:case 0x2006:case 0x2008:case 0x2009:
case 0x200A:case 0x2028:case 0x2029:case 0x205F:case 0x3000:
		return true;
		default:return false;
	};
}
}