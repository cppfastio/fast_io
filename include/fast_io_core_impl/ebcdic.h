#pragma once

namespace fast_io::details
{

template<std::integral char_type>
inline constexpr bool exec_charset_is_ebcdic() noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<char_type>,char>)
		return static_cast<char8_t>('A')!=u8'A';
	else if constexpr(std::same_as<std::remove_cvref_t<char_type>,wchar_t>)
		return static_cast<char32_t>(L'A')!=U'A';
	else
		return false;
}

template<std::integral char_type>
inline constexpr bool is_ebcdic{exec_charset_is_ebcdic<char_type>()};

}