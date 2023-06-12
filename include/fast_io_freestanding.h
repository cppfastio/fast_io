#pragma once

//fast_io_freestanding.h is usable when the underlining system implements dynamic memory allocations and exceptions
#if !defined(__cplusplus)
#error "You are not using a C++ compiler"
#endif

#if !defined(__cpp_concepts)
#error "fast_io requires at least C++20 standard compiler."
#else

#include"fast_io_core.h"

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(push)
#pragma warning( disable : 4514 )
#pragma warning( disable : 4554 )
#pragma warning( disable : 4668 )
#pragma warning( disable : 4820 )
#pragma warning( disable : 4710 )
#endif

#include"fast_io_freestanding_impl/exception.h"
//#include"fast_io_freestanding_impl/posix_error.h"
//compile floating point is slow since it requires algorithms like ryu
#ifndef FAST_IO_DISABLE_FLOATING_POINT
#include"fast_io_unit/floating/impl.h"
#endif
#include"fast_io_freestanding_impl/io_buffer/impl.h"
#include"fast_io_freestanding_impl/io_deco_filter/impl.h"
#include"fast_io_freestanding_impl/decorators/impl.h"
#include"fast_io_freestanding_impl/auto_indent.h"
#include"fast_io_freestanding_impl/serializations/impl.h"
#include"fast_io_freestanding_impl/space_reserve.h"
#include"fast_io_freestanding_impl/width.h"
#if 0
#include"fast_io_freestanding_impl/scanners/impl.h"
#endif
#include"fast_io_freestanding_impl/cond.h"

#if defined(_GLIBCXX_BITSET)
#include"fast_io_unit/bitset.h"
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#endif
#endif
