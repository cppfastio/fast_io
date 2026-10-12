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
#include "impl/sized_vector.h"

#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))

namespace fast_io
{

template <::std::unsigned_integral uinttype, typename T, typename Alloc = ::fast_io::native_global_allocator>
using sized_vector = ::fast_io::containers::sized_vector<uinttype, T, Alloc>;

template <typename T, typename Alloc = ::fast_io::native_global_allocator>
using sized_vector_zu32 = ::fast_io::containers::sized_vector<::fast_io::size32_t, T, Alloc>;

using zu32_sized_vector_zu32 = ::fast_io::containers::sized_vector<::fast_io::size32_t, ::fast_io::size32_t, ::fast_io::native_global_allocator>;

namespace containers
{

template <::std::input_iterator InputIt>
sized_vector(InputIt, InputIt) -> sized_vector<::fast_io::size32_t, typename ::std::iterator_traits<InputIt>::value_type, ::fast_io::native_global_allocator>;
#ifdef __cpp_lib_containers_ranges
template <::std::ranges::input_range R>
sized_vector(::std::from_range_t, R &&) -> sized_vector<::fast_io::size32_t, ::std::ranges::range_value_t<R>, ::fast_io::native_global_allocator>;
#endif

template <typename T, typename... U>
	requires(::std::constructible_from<T, U> && ...)
sized_vector(T, U...) -> sized_vector<::fast_io::size32_t, T, ::fast_io::native_global_allocator>;

} // namespace containers

namespace tlc
{
template <::std::unsigned_integral uinttype, typename T, typename Alloc = ::fast_io::native_thread_local_allocator>
using sized_vector = ::fast_io::containers::sized_vector<uinttype, T, Alloc>;

template <typename T, typename Alloc = ::fast_io::native_thread_local_allocator>
using sized_vector_zu32 = ::fast_io::containers::sized_vector<::fast_io::size32_t, T, Alloc>;

using zu32_sized_vector_zu32 = ::fast_io::containers::sized_vector<::fast_io::size32_t, ::fast_io::size32_t, ::fast_io::native_thread_local_allocator>;
} // namespace tlc

} // namespace fast_io

#endif

#include "impl/misc/pop_macros.h"
#include "impl/misc/pop_warnings.h"
