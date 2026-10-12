#pragma once

// fast_io_freestanding.h is usable when the underlining system implements dynamic memory allocations and exceptions
#if !defined(__cplusplus)
#error "You are not using a C++ compiler"
#endif

#if !defined(__cpp_concepts)
#error "fast_io requires at least a C++26 standard compiler."
#else

#include "fast_io_core.h"

#include "fast_io_dsal/impl/misc/push_warnings.h"
#include "fast_io_dsal/impl/misc/push_macros.h"

#include "fast_io_freestanding_impl/exception.h"
// #include"fast_io_freestanding_impl/posix_error.h"
// compile floating point is slow; the conversion machinery is separately compiled in fast_io.floating
#ifndef FAST_IO_DISABLE_FLOATING_POINT
#include "fast_io_unit/floating/impl.h"
#endif
#include "fast_io_freestanding_impl/io_buffer/impl.h"
#include "fast_io_freestanding_impl/auto_indent.h"
#include "fast_io_freestanding_impl/serializations/impl.h"
#include "fast_io_freestanding_impl/space_reserve.h"
#include "fast_io_freestanding_impl/width.h"
#if 0
#include "fast_io_freestanding_impl/scanners/impl.h"
#include "fast_io_freestanding_impl/cond.h"
#endif

#if defined(_GLIBCXX_BITSET)
#include "fast_io_unit/bitset.h"
#endif

#include "fast_io_dsal/impl/misc/pop_macros.h"
#include "fast_io_dsal/impl/misc/pop_warnings.h"

#endif
