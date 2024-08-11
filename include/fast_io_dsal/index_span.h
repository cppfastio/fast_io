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

#include "span.h"
#include "impl/index_span.h"

namespace fast_io
{

using ::fast_io::containers::index_unchecked;
using ::fast_io::containers::index_span;

namespace tlc
{
using ::fast_io::containers::index_span;
} // namespace tlc

} // namespace fast_io

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#endif
