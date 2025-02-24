#pragma once

#if !defined(__cplusplus)
#error "You must be using a C++ compiler"
#endif

#if !defined(__cplusplus)
#error "You must be using a C++ compiler"
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
#include "../fast_io_core.h"
#include "impl/misc/push_macros.h"
#include "impl/misc/push_warnings.h"
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

#include "impl/misc/pop_macros.h"
#include "impl/misc/pop_warnings.h"
