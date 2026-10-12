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
#include "../fast_io_core.h"
#include "impl/misc/push_warnings.h"
#include "impl/misc/push_macros.h"
#include "impl/freestanding.h"
#include "impl/common.h"
#include "impl/string_view.h"
#include "impl/cstring_view.h"
#include "impl/sized_string.h"

#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))


namespace fast_io
{

template <::std::unsigned_integral uinttype, ::std::integral chartype, typename allocator = ::fast_io::native_global_allocator>
using basic_sized_string = ::fast_io::containers::basic_sized_string<uinttype, chartype, allocator>;

template <::std::integral chartype, typename allocator = ::fast_io::native_global_allocator>
using basic_sized_string_zu32 = ::fast_io::containers::basic_sized_string<::fast_io::size32_t, chartype, allocator>;

using sized_string_zu32 = ::fast_io::basic_sized_string_zu32<char>;
using wsized_string_zu32 = ::fast_io::basic_sized_string_zu32<wchar_t>;
using u8sized_string_zu32 = ::fast_io::basic_sized_string_zu32<char8_t>;
using u16sized_string_zu32 = ::fast_io::basic_sized_string_zu32<char16_t>;
using u32sized_string_zu32 = ::fast_io::basic_sized_string_zu32<char32_t>;

namespace tlc
{

template <::std::unsigned_integral uinttype, ::std::integral chartype, typename allocator = ::fast_io::native_thread_local_allocator>
using basic_sized_string = ::fast_io::containers::basic_sized_string<uinttype, chartype, allocator>;

template <::std::integral chartype, typename allocator = ::fast_io::native_thread_local_allocator>
using basic_sized_string_zu32 = ::fast_io::containers::basic_sized_string<::fast_io::size32_t, chartype, allocator>;

using sized_string_zu32 = ::fast_io::tlc::basic_sized_string_zu32<char>;
using wsized_string_zu32 = ::fast_io::tlc::basic_sized_string_zu32<wchar_t>;
using u8sized_string_zu32 = ::fast_io::tlc::basic_sized_string_zu32<char8_t>;
using u16sized_string_zu32 = ::fast_io::tlc::basic_sized_string_zu32<char16_t>;
using u32sized_string_zu32 = ::fast_io::tlc::basic_sized_string_zu32<char32_t>;

} // namespace tlc

template <::std::unsigned_integral uinttype, ::std::integral char_type, typename... Args>
constexpr inline ::fast_io::basic_sized_string<uinttype, char_type> basic_concat_fast_io_sized(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::basic_general_concat<false, char_type, ::fast_io::basic_sized_string<uinttype, char_type>>(::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args))...))
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char_type>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char_type, ::fast_io::basic_sized_string<uinttype, char_type>>(::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::basic_sized_string");
		return {};
	}
}

template <::std::integral char_type, typename... Args>
constexpr inline ::fast_io::basic_sized_string_zu32<char_type> basic_concat_fast_io_sized_zu32(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::basic_general_concat<false, char_type, ::fast_io::basic_sized_string_zu32<char_type>>(::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args))...))
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char_type>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char_type, ::fast_io::basic_sized_string_zu32<char_type>>(::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::basic_sized_string_zu32<char_type>");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::sized_string_zu32 concat_fast_io_sized_zu32(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::basic_general_concat<false, char, ::fast_io::sized_string_zu32>(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args))...))
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char, ::fast_io::sized_string_zu32>(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::sized_string_zu32");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::wsized_string_zu32 wconcat_fast_io_sized_zu32(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::basic_general_concat<false, wchar_t, ::fast_io::wsized_string_zu32>(::fast_io::io_print_forward<wchar_t>(::fast_io::io_print_alias(args))...))
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<wchar_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, wchar_t, ::fast_io::wsized_string_zu32>(::fast_io::io_print_forward<wchar_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::wsized_string_zu32");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::u8sized_string_zu32 u8concat_fast_io_sized_zu32(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::basic_general_concat<false, char8_t, ::fast_io::u8sized_string_zu32>(::fast_io::io_print_forward<char8_t>(::fast_io::io_print_alias(args))...))
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char8_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char8_t, ::fast_io::u8sized_string_zu32>(::fast_io::io_print_forward<char8_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::u8sized_string_zu32");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::u16sized_string_zu32 u16concat_fast_io_sized_zu32(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::basic_general_concat<false, char16_t, ::fast_io::u16sized_string_zu32>(::fast_io::io_print_forward<char16_t>(::fast_io::io_print_alias(args))...))
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char16_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char16_t, ::fast_io::u16sized_string_zu32>(::fast_io::io_print_forward<char16_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::u16sized_string_zu32");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::u32sized_string_zu32 u32concat_fast_io_sized_zu32(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::basic_general_concat<false, char32_t, ::fast_io::u32sized_string_zu32>(::fast_io::io_print_forward<char32_t>(::fast_io::io_print_alias(args))...))
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char32_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char32_t, ::fast_io::u32sized_string_zu32>(::fast_io::io_print_forward<char32_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::u32sized_string_zu32");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::sized_string_zu32 concatln_fast_io_sized_zu32(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::basic_general_concat<true, char, ::fast_io::sized_string_zu32>(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args))...))
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<true, char, ::fast_io::sized_string_zu32>(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concatln ::fast_io::sized_string_zu32");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::wsized_string_zu32 wconcatln_fast_io_sized_zu32(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::basic_general_concat<true, wchar_t, ::fast_io::wsized_string_zu32>(::fast_io::io_print_forward<wchar_t>(::fast_io::io_print_alias(args))...))
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<wchar_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<true, wchar_t, ::fast_io::wsized_string_zu32>(::fast_io::io_print_forward<wchar_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concatln ::fast_io::wsized_string_zu32");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::u8sized_string_zu32 u8concatln_fast_io_sized_zu32(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::basic_general_concat<true, char8_t, ::fast_io::u8sized_string_zu32>(::fast_io::io_print_forward<char8_t>(::fast_io::io_print_alias(args))...))
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char8_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<true, char8_t, ::fast_io::u8sized_string_zu32>(::fast_io::io_print_forward<char8_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concatln ::fast_io::u8sized_string_zu32");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::u16sized_string_zu32 u16concatln_fast_io_sized_zu32(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::basic_general_concat<true, char16_t, ::fast_io::u16sized_string_zu32>(::fast_io::io_print_forward<char16_t>(::fast_io::io_print_alias(args))...))
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char16_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<true, char16_t, ::fast_io::u16sized_string_zu32>(::fast_io::io_print_forward<char16_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concatln ::fast_io::u16sized_string_zu32");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::u32sized_string_zu32 u32concatln_fast_io_sized_zu32(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::basic_general_concat<true, char32_t, ::fast_io::u32sized_string_zu32>(::fast_io::io_print_forward<char32_t>(::fast_io::io_print_alias(args))...))
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char32_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<true, char32_t, ::fast_io::u32sized_string_zu32>(::fast_io::io_print_forward<char32_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concatln ::fast_io::u32sized_string_zu32");
		return {};
	}
}

namespace tlc
{

template <::std::integral char_type, typename... Args>
constexpr inline ::fast_io::tlc::basic_sized_string_zu32<char_type> basic_concat_fast_io_sized_zu32_tlc(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::basic_general_concat<false, char_type, ::fast_io::tlc::basic_sized_string_zu32<char_type>>(::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args))...))
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char_type>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char_type, ::fast_io::tlc::basic_sized_string_zu32<char_type>>(::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::tlc::basic_sized_string_zu32<char_type>");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::tlc::sized_string_zu32 concat_fast_io_sized_zu32_tlc(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::basic_general_concat<false, char, ::fast_io::tlc::sized_string_zu32>(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args))...))
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char, ::fast_io::tlc::sized_string_zu32>(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::tlc::sized_string_zu32");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::tlc::wsized_string_zu32 wconcat_fast_io_sized_zu32_tlc(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::basic_general_concat<false, wchar_t, ::fast_io::tlc::wsized_string_zu32>(::fast_io::io_print_forward<wchar_t>(::fast_io::io_print_alias(args))...))
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<wchar_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, wchar_t, ::fast_io::tlc::wsized_string_zu32>(::fast_io::io_print_forward<wchar_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::tlc::wsized_string_zu32");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::tlc::u8sized_string_zu32 u8concat_fast_io_sized_zu32_tlc(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::basic_general_concat<false, char8_t, ::fast_io::tlc::u8sized_string_zu32>(::fast_io::io_print_forward<char8_t>(::fast_io::io_print_alias(args))...))
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char8_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char8_t, ::fast_io::tlc::u8sized_string_zu32>(::fast_io::io_print_forward<char8_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::tlc::u8sized_string_zu32");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::tlc::u16sized_string_zu32 u16concat_fast_io_sized_zu32_tlc(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::basic_general_concat<false, char16_t, ::fast_io::tlc::u16sized_string_zu32>(::fast_io::io_print_forward<char16_t>(::fast_io::io_print_alias(args))...))
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char16_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char16_t, ::fast_io::tlc::u16sized_string_zu32>(::fast_io::io_print_forward<char16_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::tlc::u16sized_string_zu32");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::tlc::u32sized_string_zu32 u32concat_fast_io_sized_zu32_tlc(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::basic_general_concat<false, char32_t, ::fast_io::tlc::u32sized_string_zu32>(::fast_io::io_print_forward<char32_t>(::fast_io::io_print_alias(args))...))
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char32_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char32_t, ::fast_io::tlc::u32sized_string_zu32>(::fast_io::io_print_forward<char32_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::tlc::u32sized_string_zu32");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::tlc::sized_string_zu32 concatln_fast_io_sized_zu32_tlc(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::basic_general_concat<true, char, ::fast_io::tlc::sized_string_zu32>(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args))...))
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<true, char, ::fast_io::tlc::sized_string_zu32>(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concatln ::fast_io::tlc::sized_string_zu32");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::tlc::wsized_string_zu32 wconcatln_fast_io_sized_zu32_tlc(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::basic_general_concat<true, wchar_t, ::fast_io::tlc::wsized_string_zu32>(::fast_io::io_print_forward<wchar_t>(::fast_io::io_print_alias(args))...))
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<wchar_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<true, wchar_t, ::fast_io::tlc::wsized_string_zu32>(::fast_io::io_print_forward<wchar_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concatln ::fast_io::tlc::wsized_string_zu32");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::tlc::u8sized_string_zu32 u8concatln_fast_io_sized_zu32_tlc(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::basic_general_concat<true, char8_t, ::fast_io::tlc::u8sized_string_zu32>(::fast_io::io_print_forward<char8_t>(::fast_io::io_print_alias(args))...))
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char8_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<true, char8_t, ::fast_io::tlc::u8sized_string_zu32>(::fast_io::io_print_forward<char8_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concatln ::fast_io::tlc::u8sized_string_zu32");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::tlc::u16sized_string_zu32 u16concatln_fast_io_sized_zu32_tlc(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::basic_general_concat<true, char16_t, ::fast_io::tlc::u16sized_string_zu32>(::fast_io::io_print_forward<char16_t>(::fast_io::io_print_alias(args))...))
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char16_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<true, char16_t, ::fast_io::tlc::u16sized_string_zu32>(::fast_io::io_print_forward<char16_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concatln ::fast_io::tlc::u16sized_string_zu32");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::tlc::u32sized_string_zu32 u32concatln_fast_io_sized_zu32_tlc(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::basic_general_concat<true, char32_t, ::fast_io::tlc::u32sized_string_zu32>(::fast_io::io_print_forward<char32_t>(::fast_io::io_print_alias(args))...))
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char32_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<true, char32_t, ::fast_io::tlc::u32sized_string_zu32>(::fast_io::io_print_forward<char32_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concatln ::fast_io::tlc::u32sized_string_zu32");
		return {};
	}
}

} // namespace tlc

} // namespace fast_io

#endif

#include "impl/misc/pop_macros.h"
#include "impl/misc/pop_warnings.h"
