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
#include "../fast_io_core_impl/simd/cpu_flags.h"
#if __has_cpp_attribute(__gnu__::__vector_size__)
#include "../fast_io_core_impl/simd/gcc_clang.h"
#else
#include "../fast_io_core_impl/simd/generic_operations.h"
#include "../fast_io_core_impl/simd/generic.h"
#endif
#include "../fast_io_core_impl/simd/is_all_zeros.h"
#include "../fast_io_core_impl/simd/mask_countr.h"
#include "../fast_io_core.h"

#include "impl/freestanding.h"
#include "impl/common.h"
#include "string_view.h"
#include "impl/associative_string.h"
#include "../fast_io_crypto/hash/rapidhash.h"
#include "impl/swiss_table_common.h"
#include "impl/str_swiss_set.h"

#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))

namespace fast_io
{

template <::std::integral T, typename Hash = ::fast_io::native_swiss_hasher, typename Alloc = ::fast_io::native_global_allocator>
using basic_str_swiss_set = ::fast_io::containers::basic_str_swiss_set<T, Hash, Alloc>;

using str_swiss_set = ::fast_io::basic_str_swiss_set<char>;
using wstr_swiss_set = ::fast_io::basic_str_swiss_set<wchar_t>;
using u8str_swiss_set = ::fast_io::basic_str_swiss_set<char8_t>;
using u16str_swiss_set = ::fast_io::basic_str_swiss_set<char16_t>;
using u32str_swiss_set = ::fast_io::basic_str_swiss_set<char32_t>;

namespace tlc
{
template <typename T, typename Hash = ::fast_io::native_swiss_hasher, typename Alloc = ::fast_io::native_thread_local_allocator>
using basic_str_swiss_set = ::fast_io::containers::basic_str_swiss_set<T, Hash, Alloc>;

using str_swiss_set = ::fast_io::tlc::basic_str_swiss_set<char>;
using wstr_swiss_set = ::fast_io::tlc::basic_str_swiss_set<wchar_t>;
using u8str_swiss_set = ::fast_io::tlc::basic_str_swiss_set<char8_t>;
using u16str_swiss_set = ::fast_io::tlc::basic_str_swiss_set<char16_t>;
using u32str_swiss_set = ::fast_io::tlc::basic_str_swiss_set<char32_t>;
} // namespace tlc
} // namespace fast_io

#endif

#include "impl/misc/pop_macros.h"
#include "impl/misc/pop_warnings.h"
