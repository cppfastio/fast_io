#pragma once
#undef min
#undef max

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
#include "../fast_io_core_impl/freestanding/impl.h"
#include "../fast_io_core_impl/terminate.h"
#include "../fast_io_core_impl/intrinsics/msvc/impl.h"
#include "../fast_io_core_impl/allocation/impl.h"
#include "../fast_io_core_impl/asan_support.h"

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
} // namespace containers

namespace tlc
{
template <typename T, typename Alloc = ::fast_io::native_thread_local_allocator>
using vector = ::fast_io::containers::vector<T, Alloc>;
}

} // namespace fast_io

#endif

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#endif
