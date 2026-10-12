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
#include "impl/str_ranked_btree_set.h"

#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))

namespace fast_io
{

template <::std::integral T, typename Alloc = ::fast_io::native_global_allocator>
using basic_str_ranked_btree_set = ::fast_io::containers::basic_str_ranked_btree_set<T, Alloc>;

using str_ranked_btree_set = ::fast_io::basic_str_ranked_btree_set<char>;
using wstr_ranked_btree_set = ::fast_io::basic_str_ranked_btree_set<wchar_t>;
using u8str_ranked_btree_set = ::fast_io::basic_str_ranked_btree_set<char8_t>;
using u16str_ranked_btree_set = ::fast_io::basic_str_ranked_btree_set<char16_t>;
using u32str_ranked_btree_set = ::fast_io::basic_str_ranked_btree_set<char32_t>;

namespace containers
{
} // namespace containers

namespace tlc
{
template <typename T, typename Alloc = ::fast_io::native_thread_local_allocator>
using basic_str_ranked_btree_set = ::fast_io::containers::basic_str_ranked_btree_set<T, Alloc>;

using str_ranked_btree_set = ::fast_io::tlc::basic_str_ranked_btree_set<char>;
using wstr_ranked_btree_set = ::fast_io::tlc::basic_str_ranked_btree_set<wchar_t>;
using u8str_ranked_btree_set = ::fast_io::tlc::basic_str_ranked_btree_set<char8_t>;
using u16str_ranked_btree_set = ::fast_io::tlc::basic_str_ranked_btree_set<char16_t>;
using u32str_ranked_btree_set = ::fast_io::tlc::basic_str_ranked_btree_set<char32_t>;
} // namespace tlc

} // namespace fast_io

#endif

#include "impl/misc/pop_macros.h"
#include "impl/misc/pop_warnings.h"
