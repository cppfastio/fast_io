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
#include "../fast_io_core_impl/freestanding/impl.h"
#include "../fast_io_core_impl/terminate.h"
#include "../fast_io_core_impl/intrinsics/msvc/impl.h"
#include "../fast_io_core_impl/allocation/impl.h"
#include "../fast_io_core_impl/asan_support.h"

#if defined(_MSC_VER) && !defined(__clang__)
#include <cstring>
#endif

#include "impl/freestanding.h"
#include "impl/common.h"
#include "impl/vector.h"

#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))

namespace fast_io
{

template <typename T, typename Alloc = ::fast_io::native_global_allocator>
using vector = ::fast_io::containers::vector<T, Alloc>;

namespace containers
{

template <::std::input_iterator InputIt>
vector(InputIt, InputIt) -> vector<typename ::std::iterator_traits<InputIt>::value_type, ::fast_io::native_global_allocator>;
#ifdef __cpp_lib_containers_ranges
template <::std::ranges::input_range R>
vector(::std::from_range_t, R &&) -> vector<::std::ranges::range_value_t<R>, ::fast_io::native_global_allocator>;
#endif


template <typename T, typename... U>
	requires(::std::constructible_from<T, U> && ...)
vector(T, U...) -> vector<T, ::fast_io::native_global_allocator>;

} // namespace containers

namespace tlc
{
template <typename T, typename Alloc = ::fast_io::native_thread_local_allocator>
using vector = ::fast_io::containers::vector<T, Alloc>;
} // namespace tlc

} // namespace fast_io

#endif

#include "impl/misc/pop_macros.h"
#include "impl/misc/pop_warnings.h"
