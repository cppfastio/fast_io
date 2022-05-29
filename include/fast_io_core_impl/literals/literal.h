#pragma once

namespace fast_io
{

template<std::integral ch_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ch_type char_literal(char8_t ch) noexcept
{
#if 'A' != u8'A' || L'A' !=u8'A'
#include"none_ascii.h"
#endif
	{
		using unsigned_t = ::std::make_unsigned_t<ch_type>;
		return static_cast<ch_type>(static_cast<unsigned_t>(ch));
	}
}
template<char8_t ch,std::integral ch_type>
inline constexpr ch_type char_literal_v{char_literal<ch_type>(ch)};

}
