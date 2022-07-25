#pragma once

namespace fast_io::freestanding
{

template<typename T>
struct is_trivially_relocatable
{
	inline static constexpr bool value =
#if defined(__clang__) && defined(__has_extension) && __has_cpp_attribute(clang::trivial_abi)
	__is_trivially_relocatable(T) ||
#endif
	::std::is_trivially_copyable_v<T>;
};

template<typename T>
inline constexpr bool is_trivially_relocatable_v = is_trivially_relocatable<T>::value;
}
