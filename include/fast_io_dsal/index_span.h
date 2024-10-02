#pragma once

#if !defined(__cplusplus)
#error "You must be using a C++ compiler"
#endif

#if !defined(__cplusplus)
#error "You must be using a C++ compiler"
#endif

#include "impl/push_macros.h"
#include "impl/push_warnings.h"

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

#include "impl/pop_macros.h"
#include "impl/pop_warnings.h"
