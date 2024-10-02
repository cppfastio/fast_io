#pragma once

#if !defined(__cplusplus)
#error "You must be using a C++ compiler"
#endif

#if !defined(__cplusplus)
#error "You must be using a C++ compiler"
#endif

#include "impl/push_macros.h"
#include "impl/push_warnings.h"

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

#include "impl/pop_macros.h"
#include "impl/pop_warnings.h"
