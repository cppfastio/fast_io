#pragma once

namespace fast_io::freestanding
{

template<typename T>
struct is_trivially_relocatable
{
	inline static constexpr bool value = ::std::is_trivially_copyable_v<T>;
};

template<typename T>
inline constexpr bool is_trivially_relocatable_v = is_trivially_relocatable<T>::value;

template<typename T>
struct is_zero_default_constructible
{
	inline static constexpr bool value = ::std::is_scalar_v<T>||::std::is_empty_v<T>;
};

template<typename T>
inline constexpr bool is_zero_default_constructible_v = is_zero_default_constructible<T>::value;

}

namespace fast_io
{

struct for_overwrite_t
{
explicit constexpr for_overwrite_t() noexcept = default;
};

inline constexpr for_overwrite_t for_overwrite{};

}

