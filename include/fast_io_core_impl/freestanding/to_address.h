#pragma once

#if __STDC_HOSTED__==1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED==1)

#if defined(__GLIBCXX__)
#include<bits/ptr_traits.h>
#else
#include<memory>
#endif
namespace fast_io::freestanding
{
using ::std::pointer_traits;
using ::std::to_address;
}
#else
namespace fast_io::freestanding
{
template<typename>
struct pointer_traits;

template<typename T>
struct pointer_traits<T*>
{
	using pointer = T*;
	using element_type = T;
};

template<typename T>
inline constexpr T* to_address(T* p) noexcept
{
	static_assert(!std::is_function_v<T>);
	return p;
}

template<class T>
constexpr auto to_address(const T& p) noexcept
{
    if constexpr (requires{ pointer_traits<T>::to_address(p); }) {
        return pointer_traits<T>::to_address(p);
    } else {
        return to_address(p.operator->());
    }
}
}

#endif
