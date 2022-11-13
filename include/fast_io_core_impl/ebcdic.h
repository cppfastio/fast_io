#pragma once

namespace fast_io::details
{

template<std::integral char_type>
inline constexpr bool exec_charset_is_ebcdic() noexcept
{
	if constexpr(std::same_as<std::remove_cvref_t<char_type>,char>)
	{
		return static_cast<char8_t>('A')!=u8'A';
	}
	else if constexpr(std::same_as<std::remove_cvref_t<char_type>,wchar_t>)
	{
		if constexpr(sizeof(wchar_t)==sizeof(char16_t))
		{
			constexpr char16_t value{static_cast<char16_t>(L'A')};
			constexpr char16_t swapped{::fast_io::byte_swap(value)};
			return value!=u'A'&&swapped!=u'A';
		}
		else if constexpr(sizeof(wchar_t)==sizeof(char32_t))
		{
			constexpr char32_t value{static_cast<char32_t>(L'A')};
			constexpr char32_t swapped{::fast_io::byte_swap(value)};
			return value!=U'A'&&swapped!=U'A';
		}
		else
		{
			return static_cast<char32_t>(L'A')!=U'A';
		}
	}
	else
	{
		return false;
	}
}

template<std::integral char_type>
inline constexpr bool is_ebcdic{exec_charset_is_ebcdic<char_type>()};

inline constexpr bool wexec_charset_is_utf_none_native_endian() noexcept
{
	if constexpr(sizeof(wchar_t)==sizeof(char16_t))
	{
		constexpr char16_t value{static_cast<char16_t>(L'A')};
		constexpr char16_t swapped{::fast_io::byte_swap(value)};
		return value!=u'A'&&swapped==u'A';
	}
	else
	{
		constexpr char32_t value{static_cast<char32_t>(L'A')};
		constexpr char32_t swapped{::fast_io::byte_swap(value)};
		return value!=U'A'&&swapped==U'A';
	}
}

inline constexpr bool wide_is_none_utf_endian{wexec_charset_is_utf_none_native_endian()};

}
