#pragma once

/*
Referenced from libstdc++
https://github.com/gcc-mirror/gcc/blob/master/libstdc++-v3/include/bits/move.h
*/
/*
C++ freestanding does not provide std::addressof, std::move and std::forward. We need to define them by ourselves.
*/

namespace fast_io::freestanding
{

template<typename T>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
[[nodiscard]] inline constexpr T* addressof(T& r) noexcept
{
	return __builtin_addressof(r);//death to compilers which do not support __builtin_addressof
}

template<typename T>
T const* addressof(T const&&) = delete;

template<typename T>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
[[nodiscard]] inline constexpr T&& forward(std::remove_reference_t<T>& t) noexcept
{
	return static_cast<T&&>(t);
}

template<typename T>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
[[nodiscard]] inline constexpr typename std::remove_reference<T>::type&& move(T&& t) noexcept
{
	return static_cast<typename std::remove_reference<T>::type&&>(t);
}

}

