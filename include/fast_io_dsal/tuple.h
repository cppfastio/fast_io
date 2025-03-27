#pragma once

#if !defined(__cplusplus)
#error "You must be using a C++ compiler"
#endif

#include "impl/misc/push_warnings.h"
#include "impl/misc/push_macros.h"
#include "impl/tuple.h"

namespace fast_io {

using ::fast_io::containers::tuple;
using ::fast_io::containers::get;
using ::fast_io::containers::is_tuple;
using ::fast_io::containers::forward_as_tuple;

}

#include "impl/misc/pop_macros.h"
#include "impl/misc/pop_warnings.h"
