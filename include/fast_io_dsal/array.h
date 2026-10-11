#pragma once

#if !defined(__cplusplus)
#error "You must be using a C++ compiler"
#endif

#include "../fast_io_core.h"
#include "impl/misc/push_macros.h"
#include "impl/misc/push_warnings.h"
#include "impl/array.h"

namespace fast_io
{

template <typename T, ::std::size_t N>
using array = ::fast_io::containers::array<T, N, ::fast_io::allocator_adapter_flags::none>;

template <::std::size_t N>
using zu_array = ::fast_io::containers::array<::std::size_t, N>;

namespace tlc
{
template <typename T, ::std::size_t N>
using array = ::fast_io::containers::array<T, N, ::fast_io::allocator_adapter_flags::none>;

template <::std::size_t N>
using zu_array = ::fast_io::containers::array<::std::size_t, N>;

using ::fast_io::containers::to_array;
} // namespace tlc

using ::fast_io::containers::to_array;

} // namespace fast_io

#include "impl/misc/pop_macros.h"
#include "impl/misc/pop_warnings.h"
