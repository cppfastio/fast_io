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

#include "impl/freestanding.h"
#include "impl/common.h"
#include "impl/list.h"

#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))

namespace fast_io
{

template <typename T, typename Alloc = ::fast_io::native_global_allocator>
using list = ::fast_io::containers::list<T, Alloc>;

namespace containers
{

#if 0
template <::std::input_iterator InputIt>
list(InputIt, InputIt) -> list<typename ::std::iterator_traits<InputIt>::value_type, ::fast_io::native_global_allocator>;

#ifdef __cpp_lib_containers_ranges
template <::std::ranges::input_range R>
list(::std::from_range_t, R &&) -> list<::std::ranges::range_value_t<R>, ::fast_io::native_global_allocator>;
#endif
#endif
} // namespace containers

namespace tlc
{
template <typename T, typename Alloc = ::fast_io::native_thread_local_allocator>
using list = ::fast_io::containers::list<T, Alloc>;
}

} // namespace fast_io

#endif

#include "impl/misc/pop_macros.h"
#include "impl/misc/pop_warnings.h"
