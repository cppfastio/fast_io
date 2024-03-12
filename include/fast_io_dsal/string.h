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
#include "../fast_io_core.h"
#include "impl/common.h"
#include "impl/string.h"

#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))

namespace fast_io
{

using string = ::fast_io::containers::basic_string<char, ::fast_io::native_global_allocator>;
using wstring = ::fast_io::containers::basic_string<wchar_t, ::fast_io::native_global_allocator>;
using u8string = ::fast_io::containers::basic_string<char8_t, ::fast_io::native_global_allocator>;
using u16string = ::fast_io::containers::basic_string<char16_t, ::fast_io::native_global_allocator>;
using u32string = ::fast_io::containers::basic_string<char32_t, ::fast_io::native_global_allocator>;

namespace tlc
{

using string = ::fast_io::containers::basic_string<char, ::fast_io::native_thread_local_allocator>;
using wstring = ::fast_io::containers::basic_string<wchar_t, ::fast_io::native_thread_local_allocator>;
using u8string = ::fast_io::containers::basic_string<char8_t, ::fast_io::native_thread_local_allocator>;
using u16string = ::fast_io::containers::basic_string<char16_t, ::fast_io::native_thread_local_allocator>;
using u32string = ::fast_io::containers::basic_string<char32_t, ::fast_io::native_thread_local_allocator>;

} // namespace tlc

template <typename... Args>
constexpr inline ::fast_io::string concat(Args &&...args)
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
constexpr inline ::fast_io::wstring wconcat(Args &&...args)
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
constexpr inline ::fast_io::u8string u8concat(Args &&...args)
{
	constexpr bool type_error{::fast_io::operations::defines::print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char8_t>, Args...>};
	if constexpr (type_error)
	{
		return ::fast_io::basic_general_concat<false, char8_t, ::fast_io::u8string>(::fast_io::io_print_forward<char8_t>(::fast_io::io_print_alias(args))...);
	}
	else
	{
		static_assert(type_error, "some types are not printable, so we cannot concat ::fast_io::FFFstring");
		return {};
	}
}

template <typename... Args>
constexpr inline ::fast_io::u16string u16concat(Args &&...args)
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
constexpr inline ::fast_io::u32string u32concat(Args &&...args)
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
constexpr inline ::fast_io::string concatln(Args &&...args)
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
constexpr inline ::fast_io::wstring wconcatln(Args &&...args)
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
constexpr inline ::fast_io::u8string u8concatln(Args &&...args)
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
constexpr inline ::fast_io::u16string u16concatln(Args &&...args)
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
constexpr inline ::fast_io::u32string u32concatln(Args &&...args)
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

template <typename... Args>
constexpr inline ::fast_io::tlc::string concat_tlc(Args &&...args)
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
constexpr inline ::fast_io::tlc::wstring wconcat_tlc(Args &&...args)
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
constexpr inline ::fast_io::tlc::u8string u8concat_tlc(Args &&...args)
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
constexpr inline ::fast_io::tlc::u16string u16concat_tlc(Args &&...args)
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
constexpr inline ::fast_io::tlc::u32string u32concat_tlc(Args &&...args)
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
constexpr inline ::fast_io::tlc::string concatln_tlc(Args &&...args)
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
constexpr inline ::fast_io::tlc::wstring wconcatln_tlc(Args &&...args)
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
constexpr inline ::fast_io::tlc::u8string u8concatln_tlc(Args &&...args)
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
constexpr inline ::fast_io::tlc::u16string u16concatln_tlc(Args &&...args)
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
constexpr inline ::fast_io::tlc::u32string u32concatln_tlc(Args &&...args)
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

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#endif
