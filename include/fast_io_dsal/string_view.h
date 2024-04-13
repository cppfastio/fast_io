#pragma once
#undef min
#undef max

#if !defined(__cplusplus)
#error "You must be using a C++ compiler"
#endif

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
#if __cpp_lib_three_way_comparison >= 201907L
#include <compare>
#endif
#include <algorithm>
#include "../fast_io_core.h"
#include "impl/common.h"
#include "impl/string_view.h"
#include "impl/cstring_view.h"

namespace fast_io
{

using ::fast_io::containers::null_terminated;
using ::fast_io::containers::null_terminated_t;

using string_view = ::fast_io::containers::basic_string_view<char>;
using wstring_view = ::fast_io::containers::basic_string_view<wchar_t>;
using u8string_view = ::fast_io::containers::basic_string_view<char8_t>;
using u16string_view = ::fast_io::containers::basic_string_view<char16_t>;
using u32string_view = ::fast_io::containers::basic_string_view<char32_t>;

using cstring_view = ::fast_io::containers::basic_cstring_view<char>;
using wcstring_view = ::fast_io::containers::basic_cstring_view<wchar_t>;
using u8cstring_view = ::fast_io::containers::basic_cstring_view<char8_t>;
using u16cstring_view = ::fast_io::containers::basic_cstring_view<char16_t>;
using u32cstring_view = ::fast_io::containers::basic_cstring_view<char32_t>;

namespace tlc
{
using string_view = ::fast_io::containers::basic_string_view<char>;
using wstring_view = ::fast_io::containers::basic_string_view<wchar_t>;
using u8string_view = ::fast_io::containers::basic_string_view<char8_t>;
using u16string_view = ::fast_io::containers::basic_string_view<char16_t>;
using u32string_view = ::fast_io::containers::basic_string_view<char32_t>;

using cstring_view = ::fast_io::containers::basic_cstring_view<char>;
using wcstring_view = ::fast_io::containers::basic_cstring_view<wchar_t>;
using u8cstring_view = ::fast_io::containers::basic_cstring_view<char8_t>;
using u16cstring_view = ::fast_io::containers::basic_cstring_view<char16_t>;
using u32cstring_view = ::fast_io::containers::basic_cstring_view<char32_t>;
} // namespace tlc

} // namespace fast_io

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#endif
