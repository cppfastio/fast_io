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
#include <memory>
#include <initializer_list>
#include <bit>
#include <compare>
#include <algorithm>
#include <ranges>
#include <utility>
#include "impl/misc/push_macros.h"
#include "impl/misc/push_warnings.h"
#include "../fast_io_core.h"

#include "impl/freestanding.h"
#include "impl/common.h"
#include "string_view.h"
#include "impl/associative_string.h"
#include "impl/str_btree_set.h"
#include "impl/str_btree_map.h"

#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))

namespace fast_io
{

template <::std::integral T, typename M, typename Alloc = ::fast_io::native_global_allocator>
using basic_str_btree_map = ::fast_io::containers::basic_str_btree_map<T, M, Alloc>;

template <typename M>
using str_btree_map = ::fast_io::basic_str_btree_map<char, M>;
template <typename M>
using wstr_btree_map = ::fast_io::basic_str_btree_map<wchar_t, M>;
template <typename M>
using u8str_btree_map = ::fast_io::basic_str_btree_map<char8_t, M>;
template <typename M>
using u16str_btree_map = ::fast_io::basic_str_btree_map<char16_t, M>;
template <typename M>
using u32str_btree_map = ::fast_io::basic_str_btree_map<char32_t, M>;

using zu_str_btree_map = ::fast_io::basic_str_btree_map<char, ::std::size_t>;

namespace containers
{
} // namespace containers

namespace tlc
{
template <::std::integral T, typename M, typename Alloc = ::fast_io::native_thread_local_allocator>
using basic_str_btree_map = ::fast_io::containers::basic_str_btree_map<T, M, Alloc>;

template <typename M>
using str_btree_map = ::fast_io::tlc::basic_str_btree_map<char, M>;
template <typename M>
using wstr_btree_map = ::fast_io::tlc::basic_str_btree_map<wchar_t, M>;
template <typename M>
using u8str_btree_map = ::fast_io::tlc::basic_str_btree_map<char8_t, M>;
template <typename M>
using u16str_btree_map = ::fast_io::tlc::basic_str_btree_map<char16_t, M>;
template <typename M>
using u32str_btree_map = ::fast_io::tlc::basic_str_btree_map<char32_t, M>;

using zu_str_btree_map = ::fast_io::tlc::basic_str_btree_map<char, ::std::size_t>;
} // namespace tlc

} // namespace fast_io

#endif

#include "impl/misc/pop_macros.h"
#include "impl/misc/pop_warnings.h"
