#pragma once

namespace fast_io
{

namespace details::codecvt::gb18030
{

template<typename T>
requires (sizeof(T)==1)
inline constexpr std::size_t get_gb18030_invalid_code_units(T* p_dst) noexcept;

template<typename T>
requires (sizeof(T)==1)
inline constexpr std::size_t lookup_uni_to_gb18030(char32_t cdpt, T* p_dst) noexcept;

template<typename T>
requires (sizeof(T)==1)
inline constexpr std::size_t get_gb18030_code_units_unhappy(char32_t u32, T* p_dst) noexcept;

template<typename T>
requires (sizeof(T)==1)
inline constexpr std::size_t get_gb18030_code_units(char32_t cdpt, T* p_dst) noexcept;

template<typename T>
requires (sizeof(T)==1)
struct gb18030_advance_unchecked_result
{
	char32_t cdpt;
	char8_t adv;
};

inline constexpr char32_t lookup_gb18030_to_uni4_func(char32_t index) noexcept;

inline constexpr char32_t utf32cp_by_gb18030_index(char32_t index) noexcept;

template<typename T>
requires (sizeof(T)==1)
inline constexpr gb18030_advance_unchecked_result<T> gb18030_advance_unchecked(T const* src) noexcept;

template<typename T>
requires (sizeof(T)==1)
inline constexpr gb18030_advance_unchecked_result<T> gb18030_advance(T const* src,std::size_t sz) noexcept;
}

}
