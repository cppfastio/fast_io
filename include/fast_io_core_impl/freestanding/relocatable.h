#pragma once

namespace fast_io::freestanding
{

template <typename T>
struct is_trivially_copyable_or_relocatable
{
	inline static constexpr bool value = ::std::is_trivially_copyable_v<T>
#if defined(__cpp_lib_trivially_relocatable)
										 || ::std::is_trivially_copyable_or_relocatable_v<T>
#endif
		;
};

template <typename T>
inline constexpr bool is_trivially_copyable_or_relocatable_v = is_trivially_copyable_or_relocatable<T>::value;

template <typename T>
struct is_zero_default_constructible
{
	inline static constexpr bool value = ::std::is_scalar_v<::std::remove_all_extents_t<T>> || ::std::is_empty_v<T>;
};

template <typename T>
inline constexpr bool is_zero_default_constructible_v = is_zero_default_constructible<T>::value;

} // namespace fast_io::freestanding

namespace fast_io
{

struct for_overwrite_t
{
	inline explicit constexpr for_overwrite_t() noexcept = default;
};

inline constexpr for_overwrite_t for_overwrite{};

} // namespace fast_io
