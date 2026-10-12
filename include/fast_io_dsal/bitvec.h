#pragma once

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
#include <compare>
#include <algorithm>
#include "impl/misc/push_macros.h"
#include "impl/misc/push_warnings.h"
#include "../fast_io_core.h"

#if defined(_MSC_VER) && !defined(__clang__)
#include <cstring>
#endif

#include "impl/freestanding.h"
#include "impl/common.h"
#include "impl/bitvec.h"

#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))

namespace fast_io
{

using bitvec = ::fast_io::containers::bitvec<::fast_io::native_global_allocator>;

namespace tlc
{
using bitvec = ::fast_io::containers::bitvec<::fast_io::native_thread_local_allocator>;
} // namespace tlc

} // namespace fast_io

#endif

#include "impl/misc/pop_macros.h"
#include "impl/misc/pop_warnings.h"
