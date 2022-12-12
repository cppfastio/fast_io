#pragma once
#undef min
#undef max
//fast_io_concept.h allows you define your device and type without compilation time penalty
#if !defined(__cplusplus)
#error "You must be using a C++ compiler"
#endif
#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(push)
#pragma warning( disable : 4514 )
#pragma warning( disable : 4623 )
#pragma warning( disable : 4626 )
#pragma warning( disable : 4668 )
#pragma warning( disable : 4820 )
#pragma warning( disable : 5027 )
#endif
#if !defined(__cpp_concepts)
#error "fast_io requires at least a C++20 standard compiler."
#else
#include<version>
#include<cstddef>
#include<type_traits>
#include<concepts>
#include"fast_io_core_impl/freestanding/addressof.h"
#include"fast_io_core_impl/concepts/impl.h"
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning (pop)
#endif
