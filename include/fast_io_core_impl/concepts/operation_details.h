#pragma once

namespace fast_io::details
{
	
template<typename ch_type>
concept character = std::integral<ch_type>&&(std::same_as<ch_type,char>||std::same_as<ch_type,wchar_t>||std::same_as<ch_type,char16_t>||
std::same_as<ch_type,char8_t>||std::same_as<ch_type,char32_t>);

template<typename T>
concept c_str_pointer = std::same_as<T,char const*>||std::same_as<T,char8_t const*>||std::same_as<T,wchar_t const*>||
	std::same_as<T,char16_t const*>||std::same_as<T,char32_t const*>;

}

namespace fast_io
{

template<typename T>
concept type_has_c_str_method = requires(T&& t)
{
	{t.c_str()}->::fast_io::details::c_str_pointer;
};

template<typename T>
concept constructible_to_os_c_str = type_has_c_str_method<T>||(std::is_array_v<std::remove_reference_t<T>>)||requires(T&& t)
{
	{t.length()};
	{t.substr()};
	{t.data()};
};

namespace manipulators
{

template<std::integral ch_type>
struct basic_os_c_str
{
	using char_type = ch_type;
	char_type const* ptr{};
	inline constexpr char_type const* c_str() const noexcept
	{
		return ptr;
	}
};

template<std::integral char_type>
inline constexpr basic_os_c_str<char_type> os_c_str(char_type const* cstr) noexcept
{
	return {cstr};
}

inline constexpr void os_c_str(decltype(nullptr))=delete;

}

}
