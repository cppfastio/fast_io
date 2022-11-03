#pragma once

#include<array>

inline constexpr std::size_t space_size{1024};
inline constexpr std::size_t N(100000);

template<std::integral char_type>
inline constexpr auto generate_filled_chars() noexcept
{
	std::array<char_type,space_size> arr;
	if constexpr(std::same_as<char_type,char>)
	{
		arr.fill('\n');
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		arr.fill(L'\n');
	}
	else
	{
		arr.fill(u8'\n');
	}
	return arr;
}

template<std::integral char_type>
inline constexpr auto filled_chars{generate_filled_chars<char_type>()};

