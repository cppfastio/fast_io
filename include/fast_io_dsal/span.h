#pragma once
#undef min
#undef max

#if !defined(__cplusplus)
#error "You must be using a C++ compiler"
#endif

#if !defined(__cplusplus)
#error "You must be using a C++ compiler"
#endif
#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(push)
#pragma warning(disable : 4464)
#pragma warning(disable : 4514)
#pragma warning(disable : 4623)
#pragma warning(disable : 4626)
#pragma warning(disable : 4668)
#pragma warning(disable : 4710)
#pragma warning(disable : 4820)
#pragma warning(disable : 5027)
#pragma warning(disable : 5045)
#include <cstring>
#endif

#include <version>
#include <type_traits>
#include <concepts>
#include <limits>
#include <cstdint>
#include <cstddef>
#include <new>
#include <initializer_list>
#include <bit>
#if __cpp_lib_three_way_comparison >= 201907L
#include <compare>
#endif
#include <algorithm>
#include "../fast_io_core.h"
#include "impl/common.h"
#include "impl/span.h"

namespace fast_io
{

using ::fast_io::containers::span;

namespace tlc
{
using ::fast_io::containers::span;
} // namespace tlc

} // namespace fast_io

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#endif
