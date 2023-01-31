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

template<typename T>
concept constructible_to_os_c_str_or_nullptr = constructible_to_os_c_str<T>||requires(T&& t)
{
	{t.is_nullptr()}->std::same_as<bool>;
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

template<::std::integral char_type>
inline constexpr basic_os_c_str<char_type> os_c_str(char_type const* cstr) noexcept
{
	return {cstr};
}

inline constexpr void os_c_str(decltype(nullptr))=delete;

template<std::integral ch_type>
struct basic_os_c_str_or_nullptr
{
	using char_type = ch_type;
	char_type const* ptr{};
	inline constexpr char_type const* c_str() const noexcept
	{
		return ptr;
	}
	inline constexpr bool is_nullptr() const noexcept
	{
		return ptr==nullptr;
	}
};

template<std::integral char_type>
inline constexpr basic_os_c_str_or_nullptr<char_type> os_c_str_or_nullptr(char_type const* cstr) noexcept
{
	return {cstr};
}

template<std::integral ch_type>
struct basic_os_c_str_with_known_size
{
	using char_type = ch_type;
	char_type const* ptr{};
	std::size_t n{};
	inline constexpr char_type const* c_str() const noexcept
	{
		return ptr;
	}
	inline constexpr std::size_t size() const noexcept
	{
		return n;
	}
	inline constexpr char_type const* data() const noexcept
	{
		return ptr;
	}
	inline constexpr char_type const* begin() const noexcept
	{
		return ptr;
	}
	inline constexpr char_type const* end() const noexcept
	{
		return ptr+n;
	}
};

template<std::integral char_type>
inline constexpr basic_os_c_str_with_known_size<char_type> os_c_str_with_known_size(char_type const* cstr,std::size_t n) noexcept
{
	return {cstr,n};
}

inline constexpr void os_c_str_with_known_size(decltype(nullptr),std::size_t)=delete;

template<std::integral ch_type>
struct basic_os_str_known_size_without_null_terminated
{
	using char_type = ch_type;
	char_type const* ptr{};
	std::size_t n{};
	inline constexpr std::size_t size() const noexcept
	{
		return n;
	}
	inline constexpr char_type const* data() const noexcept
	{
		return ptr;
	}
	inline constexpr char_type const* begin() const noexcept
	{
		return ptr;
	}
	inline constexpr char_type const* end() const noexcept
	{
		return ptr+n;
	}
};

template<std::integral char_type>
inline constexpr basic_os_str_known_size_without_null_terminated<char_type> os_str_known_size_without_null_terminated(char_type const* cstr,std::size_t n) noexcept
{
	return {cstr,n};
}

template<std::integral char_type>
inline constexpr basic_os_str_known_size_without_null_terminated<char_type> os_str_known_size_without_null_terminated(char_type const* cstr, char_type const* end) noexcept
{
	return {cstr,static_cast<::std::size_t>(end-cstr)};
}

inline constexpr void os_str_known_size_without_null_terminated(decltype(nullptr),std::size_t)=delete;

}

}
