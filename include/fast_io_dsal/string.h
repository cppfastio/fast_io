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
#include "impl/string.h"

#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))

namespace fast_io
{

template <::std::integral chartype, typename allocator = ::fast_io::native_global_allocator>
using basic_string = ::fast_io::containers::basic_string<chartype, allocator>;
using string = ::fast_io::containers::basic_string<char, ::fast_io::native_global_allocator>;
using wstring = ::fast_io::containers::basic_string<wchar_t, ::fast_io::native_global_allocator>;
using u8string = ::fast_io::containers::basic_string<char8_t, ::fast_io::native_global_allocator>;
using u16string = ::fast_io::containers::basic_string<char16_t, ::fast_io::native_global_allocator>;
using u32string = ::fast_io::containers::basic_string<char32_t, ::fast_io::native_global_allocator>;

template <::std::integral chartype, typename allocator = ::fast_io::native_global_allocator>
using basic_ostring_ref_fast_io = ::fast_io::containers::basic_ostring_ref_fast_io<chartype, allocator>;
using ostring_ref_fast_io = ::fast_io::containers::basic_ostring_ref_fast_io<char, ::fast_io::native_global_allocator>;
using wostring_ref_fast_io = ::fast_io::containers::basic_ostring_ref_fast_io<wchar_t, ::fast_io::native_global_allocator>;
using u8ostring_ref_fast_io = ::fast_io::containers::basic_ostring_ref_fast_io<char8_t, ::fast_io::native_global_allocator>;
using u16ostring_ref_fast_io = ::fast_io::containers::basic_ostring_ref_fast_io<char16_t, ::fast_io::native_global_allocator>;
using u32ostring_ref_fast_io = ::fast_io::containers::basic_ostring_ref_fast_io<char32_t, ::fast_io::native_global_allocator>;

namespace tlc
{

template <::std::integral chartype, typename allocator = ::fast_io::native_thread_local_allocator>
using basic_string = ::fast_io::containers::basic_string<chartype, allocator>;
using string = ::fast_io::containers::basic_string<char, ::fast_io::native_thread_local_allocator>;
using wstring = ::fast_io::containers::basic_string<wchar_t, ::fast_io::native_thread_local_allocator>;
using u8string = ::fast_io::containers::basic_string<char8_t, ::fast_io::native_thread_local_allocator>;
using u16string = ::fast_io::containers::basic_string<char16_t, ::fast_io::native_thread_local_allocator>;
using u32string = ::fast_io::containers::basic_string<char32_t, ::fast_io::native_thread_local_allocator>;

template <::std::integral chartype, typename allocator = ::fast_io::native_thread_local_allocator>
using basic_ostring_ref_fast_io_tlc = ::fast_io::containers::basic_ostring_ref_fast_io<chartype, allocator>;
using ostring_ref_fast_io_tlc = ::fast_io::containers::basic_ostring_ref_fast_io<char, ::fast_io::native_thread_local_allocator>;
using wostring_ref_fast_io_tlc = ::fast_io::containers::basic_ostring_ref_fast_io<wchar_t, ::fast_io::native_thread_local_allocator>;
using u8ostring_ref_fast_io_tlc = ::fast_io::containers::basic_ostring_ref_fast_io<char8_t, ::fast_io::native_thread_local_allocator>;
using u16ostring_ref_fast_io_tlc = ::fast_io::containers::basic_ostring_ref_fast_io<char16_t, ::fast_io::native_thread_local_allocator>;
using u32ostring_ref_fast_io_tlc = ::fast_io::containers::basic_ostring_ref_fast_io<char32_t, ::fast_io::native_thread_local_allocator>;

} // namespace tlc


template <::std::integral char_type, typename... Args>
constexpr inline ::fast_io::basic_string<char_type> basic_concat_fast_io(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char_type>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char_type, ::fast_io::basic_string<char_type>>(::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::basic_string");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::string concat_fast_io(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char, ::fast_io::string>(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::string");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::wstring wconcat_fast_io(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<wchar_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, wchar_t, ::fast_io::wstring>(::fast_io::io_print_forward<wchar_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::wstring");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::u8string u8concat_fast_io(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char8_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char8_t, ::fast_io::u8string>(::fast_io::io_print_forward<char8_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::u8string");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::u16string u16concat_fast_io(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char16_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char16_t, ::fast_io::u16string>(::fast_io::io_print_forward<char16_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::u16string");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::u32string u32concat_fast_io(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char32_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char32_t, ::fast_io::u32string>(::fast_io::io_print_forward<char32_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::u32string");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::string concatln_fast_io(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<true, char, ::fast_io::string>(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concatln ::fast_io::string");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::wstring wconcatln_fast_io(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<wchar_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<true, wchar_t, ::fast_io::wstring>(::fast_io::io_print_forward<wchar_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concatln ::fast_io::wstring");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::u8string u8concatln_fast_io(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char8_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<true, char8_t, ::fast_io::u8string>(::fast_io::io_print_forward<char8_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concatln ::fast_io::u8string");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::u16string u16concatln_fast_io(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char16_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<true, char16_t, ::fast_io::u16string>(::fast_io::io_print_forward<char16_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concatln ::fast_io::u16string");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::u32string u32concatln_fast_io(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char32_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<true, char32_t, ::fast_io::u32string>(::fast_io::io_print_forward<char32_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concatln ::fast_io::u32string");
		return {};
	}
}

namespace tlc
{

template <::std::integral char_type, typename... Args>
constexpr inline ::fast_io::tlc::basic_string<char_type> basic_concat_fast_io_tlc(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char_type>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char_type, ::fast_io::tlc::basic_string<char_type>>(::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::tlc::basic_string");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::tlc::string concat_fast_io_tlc(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char, ::fast_io::tlc::string>(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::tlc::string");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::tlc::wstring wconcat_fast_io_tlc(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<wchar_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, wchar_t, ::fast_io::tlc::wstring>(::fast_io::io_print_forward<wchar_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::tlc::wstring");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::tlc::u8string u8concat_fast_io_tlc(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char8_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char8_t, ::fast_io::tlc::u8string>(::fast_io::io_print_forward<char8_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::tlc::u8string");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::tlc::u16string u16concat_fast_io_tlc(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char16_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char16_t, ::fast_io::tlc::u16string>(::fast_io::io_print_forward<char16_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::tlc::u16string");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::tlc::u32string u32concat_fast_io_tlc(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char32_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char32_t, ::fast_io::tlc::u32string>(::fast_io::io_print_forward<char32_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::tlc::u32string");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::tlc::string concatln_fast_io_tlc(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<true, char, ::fast_io::tlc::string>(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concatln ::fast_io::tlc::string");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::tlc::wstring wconcatln_fast_io_tlc(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<wchar_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<true, wchar_t, ::fast_io::tlc::wstring>(::fast_io::io_print_forward<wchar_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concatln ::fast_io::tlc::wstring");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::tlc::u8string u8concatln_fast_io_tlc(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char8_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<true, char8_t, ::fast_io::tlc::u8string>(::fast_io::io_print_forward<char8_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concatln ::fast_io::tlc::u8string");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::tlc::u16string u16concatln_fast_io_tlc(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char16_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<true, char16_t, ::fast_io::tlc::u16string>(::fast_io::io_print_forward<char16_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concatln ::fast_io::tlc::u16string");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::tlc::u32string u32concatln_fast_io_tlc(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char32_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<true, char32_t, ::fast_io::tlc::u32string>(::fast_io::io_print_forward<char32_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concatln ::fast_io::tlc::u32string");
		return {};
	}
}

} // namespace tlc

} // namespace fast_io

#endif

#include "impl/misc/pop_macros.h"
#include "impl/misc/pop_warnings.h"
