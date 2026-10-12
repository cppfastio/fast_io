#pragma once

#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))
#include "defined_types.h"
#endif

// Opt-in deprecation warnings for string-literal misuse in the print
// family. -Wformat state is not observable: clang's __has_warning reports
// warning-name availability rather than the -W toggle, and GCC lacks
// __has_warning entirely, so these are strictly opt-in with
// -DFAST_IO_WARN_*=1 (the warnings live under -Wdeprecated-declarations and
// follow that flag as usual).
#ifndef FAST_IO_WARN_CONSECUTIVE_LITERALS
#define FAST_IO_WARN_CONSECUTIVE_LITERALS 0
#endif

#ifndef FAST_IO_WARN_TRAILING_LITERAL
#define FAST_IO_WARN_TRAILING_LITERAL 0
#endif

namespace fast_io
{

namespace details
{

// Whether T is a character-type C array (e.g. a string literal).
template <typename T>
inline constexpr bool io_arg_is_character_array_v =
	::std::is_array_v<::std::remove_cvref_t<T>> &&
	(::std::same_as<::std::remove_cv_t<::std::remove_extent_t<::std::remove_cvref_t<T>>>, char> ||
	 ::std::same_as<::std::remove_cv_t<::std::remove_extent_t<::std::remove_cvref_t<T>>>, wchar_t> ||
	 ::std::same_as<::std::remove_cv_t<::std::remove_extent_t<::std::remove_cvref_t<T>>>, char8_t> ||
	 ::std::same_as<::std::remove_cv_t<::std::remove_extent_t<::std::remove_cvref_t<T>>>, char16_t> ||
	 ::std::same_as<::std::remove_cv_t<::std::remove_extent_t<::std::remove_cvref_t<T>>>, char32_t>);

// Last element of a type list. Written as plain recursion rather than a
// fold or pack indexing, which GCC rejects in requires clauses.
template <typename... Args>
struct io_last_arg;

template <>
struct io_last_arg<>
{
	using type = void;
};

template <typename T>
struct io_last_arg<T>
{
	using type = T;
};

template <typename T, typename... Rest>
struct io_last_arg<T, Rest...> : io_last_arg<Rest...>
{
};

template <typename... Args>
using io_last_arg_t = typename io_last_arg<Args...>::type;

#if FAST_IO_WARN_CONSECUTIVE_LITERALS

// Emitted as a deprecation warning naming the 1-based position and the
// types of a pair of consecutive character-array arguments. Merge them into
// one literal: print("Hello" "World") instead of print("Hello", "World").
template <::std::size_t position, typename A, typename B>
[[deprecated("consecutive string literal arguments should be merged into one literal, e.g. "
			 "print(\"Hello\" \"World\") instead of print(\"Hello\", \"World\")")]]
inline constexpr void io_consecutive_string_literals_at_arguments() noexcept
{
}

// The recursion slides over adjacent pairs (A,B), (B,C), ... starting at
// argument `position` (1-based).
template <::std::size_t position, typename... Args>
struct io_print_warn_consecutive_literals
{
	static constexpr void run() noexcept
	{
	}
};

template <::std::size_t position, typename A, typename B, typename... Rest>
struct io_print_warn_consecutive_literals<position, A, B, Rest...>
{
	static constexpr void run() noexcept
	{
		if constexpr (::fast_io::details::io_arg_is_character_array_v<A> &&
					  ::fast_io::details::io_arg_is_character_array_v<B>)
		{
			::fast_io::details::io_consecutive_string_literals_at_arguments<position, A, B>();
		}
		::fast_io::details::io_print_warn_consecutive_literals<position + 1, B, Rest...>::run();
	}
};

#endif

#if FAST_IO_WARN_TRAILING_LITERAL

// Emitted as a deprecation warning naming the type of a trailing
// character-array argument of an "ln" print function: fold the newline into
// the literal and call print(..., "\n") instead.
template <typename T>
[[deprecated("the last argument of an \"ln\" print function should not be a string literal; "
			 "fold the newline into the literal")]]
inline constexpr void io_last_argument_is_a_string_literal() noexcept
{
}

#endif

template <typename... Args>
inline constexpr void io_print_check_no_consecutive_character_arrays() noexcept
{
#if FAST_IO_WARN_CONSECUTIVE_LITERALS
	::fast_io::details::io_print_warn_consecutive_literals<1, Args...>::run();
#endif
}

template <typename... Args>
inline constexpr void io_print_check_last_arg_not_character_array() noexcept
{
#if FAST_IO_WARN_TRAILING_LITERAL
	if constexpr (::fast_io::details::io_arg_is_character_array_v<::fast_io::details::io_last_arg_t<Args...>>)
	{
		::fast_io::details::io_last_argument_is_a_string_literal<::fast_io::details::io_last_arg_t<Args...>>();
	}
#endif
}

} // namespace details

inline namespace io
{

template <typename T, typename... Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void print(T &&t, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::details::io_print_may_throw<false, T, Args...>())
{
	::fast_io::details::io_print_check_no_consecutive_character_arrays<T, Args...>();
	constexpr bool device_and_type_ok{::fast_io::operations::defines::print_freestanding_okay<T, Args...>};
	if constexpr (device_and_type_ok)
	{
		using char_type = typename decltype(::fast_io::operations::output_stream_ref(t))::output_char_type;
		::fast_io::operations::decay::print_freestanding_decay<false>(
			::fast_io::operations::output_stream_ref(t),
			::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args))...);
	}
	else
	{
#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES)) &&                                     \
	__has_include(<stdio.h>)
		constexpr bool type_ok{::fast_io::operations::defines::print_freestanding_params_okay<char, Args...>};
		if constexpr (type_ok)
		{
			::fast_io::details::print_after_io_print_forward<false>(
				::fast_io::io_print_forward<char>(::fast_io::io_print_alias(t)),
				::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args))...);
		}
		else
		{
			// clang-format off
static_assert(type_ok, "some types are not printable for print on default C's stdout");
			// clang-format on
		}
#else
		constexpr bool device_ok{::fast_io::operations::defines::has_output_or_io_stream_ref_define<T>};
		// clang-format off
static_assert(device_ok, "freestanding environment must provide IO device for print");
static_assert(device_and_type_ok, "some types are not printable for print");
		// clang-format on
#endif
	}
}

template <typename T, typename... Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void println(T &&t, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::details::io_print_may_throw<true, T, Args...>())
{
	::fast_io::details::io_print_check_no_consecutive_character_arrays<T, Args...>();
	::fast_io::details::io_print_check_last_arg_not_character_array<T, Args...>();
	constexpr bool device_and_type_ok{::fast_io::operations::defines::print_freestanding_okay<T, Args...>};
	if constexpr (device_and_type_ok)
	{
		using char_type = typename decltype(::fast_io::operations::output_stream_ref(t))::output_char_type;
		::fast_io::operations::decay::print_freestanding_decay<true>(
			::fast_io::operations::output_stream_ref(t),
			::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args))...);
	}
	else
	{
#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES)) &&                                     \
	__has_include(<stdio.h>)
		constexpr bool type_ok{::fast_io::operations::defines::print_freestanding_params_okay<char, Args...>};
		if constexpr (type_ok)
		{
			::fast_io::details::print_after_io_print_forward<true>(
				::fast_io::io_print_forward<char>(::fast_io::io_print_alias(t)),
				::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args))...);
		}
		else
		{
			static_assert(type_ok, "some types are not printable for print on default C's stdout");
		}
#else
		constexpr bool device_ok{::fast_io::operations::defines::has_output_or_io_stream_ref_define<T>};
		// clang-format off
static_assert(device_ok, "freestanding environment must provide IO device for println");
static_assert(device_and_type_ok, "some types are not printable for println");
		// clang-format on
#endif
	}
}

template <typename T, typename... Args>
inline constexpr void perr(T &&t, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::details::io_perr_may_throw<false, T, Args...>())
{
	::fast_io::details::io_print_check_no_consecutive_character_arrays<T, Args...>();
	constexpr bool device_and_type_ok{::fast_io::operations::defines::print_freestanding_okay<T, Args...>};
	if constexpr (device_and_type_ok)
	{
		using char_type = typename decltype(::fast_io::operations::output_stream_ref(t))::output_char_type;
		::fast_io::operations::decay::print_freestanding_decay_cold<false>(
			::fast_io::operations::output_stream_ref(t),
			::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args))...);
	}
	else
	{
#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && !defined(_LIBCPP_FREESTANDING) && \
	  !defined(__AVR__)) ||                                                                                            \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))
		constexpr bool type_ok{::fast_io::operations::defines::print_freestanding_params_okay<char, Args...>};
		if constexpr (type_ok)
		{
			::fast_io::details::perr_after_io_print_forward<false>(
				fast_io::io_print_forward<char>(fast_io::io_print_alias(t)),
				fast_io::io_print_forward<char>(fast_io::io_print_alias(args))...);
		}
		else
		{
			// clang-format off
static_assert(type_ok, "some types are not printable for perr on native err");
			// clang-format on
		}
#else
		constexpr bool device_ok{::fast_io::operations::defines::has_output_or_io_stream_ref_define<T>};
		// clang-format off
static_assert(device_ok, "freestanding environment must provide IO device for perr");
static_assert(device_and_type_ok, "some types are not printable for perr");
		// clang-format on
#endif
	}
}

template <typename T, typename... Args>
inline constexpr void perrln(T &&t, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::details::io_perr_may_throw<true, T, Args...>())
{
	::fast_io::details::io_print_check_no_consecutive_character_arrays<T, Args...>();
	::fast_io::details::io_print_check_last_arg_not_character_array<T, Args...>();
	constexpr bool device_and_type_ok{::fast_io::operations::defines::print_freestanding_okay<T, Args...>};
	if constexpr (device_and_type_ok)
	{
		using char_type = typename decltype(::fast_io::operations::output_stream_ref(t))::output_char_type;
		::fast_io::operations::decay::print_freestanding_decay_cold<true>(
			::fast_io::operations::output_stream_ref(t),
			::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args))...);
	}
	else
	{
#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && !defined(_LIBCPP_FREESTANDING) && \
	  !defined(__AVR__)) ||                                                                                            \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))
		constexpr bool type_ok{::fast_io::operations::defines::print_freestanding_params_okay<char, Args...>};
		if constexpr (type_ok)
		{
			::fast_io::details::perr_after_io_print_forward<true>(
				fast_io::io_print_forward<char>(fast_io::io_print_alias(t)),
				fast_io::io_print_forward<char>(fast_io::io_print_alias(args))...);
		}
		else
		{
			// clang-format off
static_assert(type_ok, "some types are not printable for perrln on native err");
			// clang-format on
		}
#else
		constexpr bool device_ok{::fast_io::operations::defines::has_output_or_io_stream_ref_define<T>};
		// clang-format off
static_assert(device_ok, "freestanding environment must provide IO device for perrln");
static_assert(device_and_type_ok, "some types are not printable for perrln");
		// clang-format on
#endif
	}
}

template <typename... Args>
[[noreturn]] inline constexpr void panic(Args &&...args) noexcept
{
	::fast_io::details::io_print_check_no_consecutive_character_arrays<Args...>();
	if constexpr (sizeof...(Args) != 0)
	{
#if defined(__HERBCEPTIONS__) || defined(__cpp_exceptions)
		try
		{
#endif
			::fast_io::io::perr(::std::forward<Args>(args)...);
#if defined(__HERBCEPTIONS__) || defined(__cpp_exceptions)
		}
#endif
#ifdef __HERBCEPTIONS__
		catch throws(::std::error)
		{
		}
#elif defined(__cpp_exceptions)
		catch (...)
		{
		}
#endif
	}
	::fast_io::fast_terminate();
}

template <typename... Args>
[[noreturn]] inline constexpr void panicln(Args &&...args) noexcept
{
	::fast_io::details::io_print_check_no_consecutive_character_arrays<Args...>();
	::fast_io::details::io_print_check_last_arg_not_character_array<Args...>();
#if defined(__HERBCEPTIONS__) || defined(__cpp_exceptions)
	try
	{
#endif
		::fast_io::io::perrln(::std::forward<Args>(args)...);
#if defined(__HERBCEPTIONS__) || defined(__cpp_exceptions)
	}
#endif
#ifdef __HERBCEPTIONS__
	catch throws(::std::error)
	{
	}
#elif defined(__cpp_exceptions)
	catch (...)
	{
	}
#endif
	::fast_io::fast_terminate();
}

// Allow debug print
#if FAST_IO_DISABLE_DEBUG_PRINT == 0
// With debugging. We output to POSIX fd or Win32 Handle directly instead of C's stdout.
template <typename T, typename... Args>
inline constexpr void debug_print(T &&t, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::details::io_debug_print_may_throw<false, T, Args...>())
{
	::fast_io::details::io_print_check_no_consecutive_character_arrays<T, Args...>();
	constexpr bool device_and_type_ok{::fast_io::operations::defines::print_freestanding_okay<T, Args...>};
	if constexpr (device_and_type_ok)
	{
		using char_type = typename decltype(::fast_io::operations::output_stream_ref(t))::output_char_type;
		::fast_io::operations::decay::print_freestanding_decay<false>(
			::fast_io::operations::output_stream_ref(t),
			fast_io::io_print_forward<char_type>(fast_io::io_print_alias(args))...);
	}
	else
	{
#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))
		constexpr bool type_ok{::fast_io::operations::defines::print_freestanding_params_okay<char, Args...>};
		if constexpr (type_ok)
		{
			fast_io::details::debug_print_after_io_print_forward<false>(
				fast_io::io_print_forward<char>(fast_io::io_print_alias(t)),
				fast_io::io_print_forward<char>(fast_io::io_print_alias(args))...);
		}
		else
		{
			// clang-format off
static_assert(type_ok, "some types are not printable for debug_print on native out");
			// clang-format on
		}
#else
		constexpr bool device_ok{::fast_io::operations::defines::has_output_or_io_stream_ref_define<T>};
		// clang-format off
static_assert(device_ok, "freestanding environment must provide IO device for debug_print");
static_assert(device_and_type_ok, "some types are not printable for debug_print on native out");
		// clang-format on
#endif
	}
}

template <typename T, typename... Args>
inline constexpr void debug_println(T &&t, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::details::io_debug_print_may_throw<true, T, Args...>())
{
	::fast_io::details::io_print_check_no_consecutive_character_arrays<T, Args...>();
	::fast_io::details::io_print_check_last_arg_not_character_array<T, Args...>();
	constexpr bool device_and_type_ok{::fast_io::operations::defines::print_freestanding_okay<T, Args...>};
	if constexpr (device_and_type_ok)
	{
		using char_type = typename decltype(::fast_io::operations::output_stream_ref(t))::output_char_type;
		::fast_io::operations::decay::print_freestanding_decay<true>(
			::fast_io::operations::output_stream_ref(t),
			fast_io::io_print_forward<char_type>(fast_io::io_print_alias(args))...);
	}
	else
	{
#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))
		constexpr bool type_ok{::fast_io::operations::defines::print_freestanding_params_okay<char, Args...>};
		if constexpr (type_ok)
		{
			fast_io::details::debug_print_after_io_print_forward<true>(
				fast_io::io_print_forward<char>(fast_io::io_print_alias(t)),
				fast_io::io_print_forward<char>(fast_io::io_print_alias(args))...);
		}
		else
		{
			// clang-format off
static_assert(type_ok, "some types are not printable for debug_println on native out");
			// clang-format on
		}
#else
		constexpr bool device_ok{::fast_io::operations::defines::has_output_or_io_stream_ref_define<T>};
		// clang-format off
static_assert(device_ok, "freestanding environment must provide IO device for debug_println");
static_assert(device_and_type_ok, "some types are not printable for debug_println on native out");
		// clang-format on
#endif
	}
}

template <typename... Args>
	requires(sizeof...(Args) != 0)
inline constexpr void debug_perr(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::io::perr(::std::forward<Args>(args)...))
{
	::fast_io::details::io_print_check_no_consecutive_character_arrays<Args...>();
	::fast_io::io::perr(::std::forward<Args>(args)...);
}

template <typename... Args>
inline constexpr void debug_perrln(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::io::perrln(::std::forward<Args>(args)...))
{
	::fast_io::details::io_print_check_no_consecutive_character_arrays<Args...>();
	::fast_io::details::io_print_check_last_arg_not_character_array<Args...>();
	::fast_io::io::perrln(::std::forward<Args>(args)...);
}
#endif

template <typename input, typename... Args>
inline constexpr void scan(input &&in, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::details::io_scan_may_throw<input, Args...>())
{
	constexpr bool device_error{::fast_io::operations::defines::has_input_or_io_stream_ref_define<input>};
	if constexpr (device_error)
	{
		using char_type = typename decltype(::fast_io::operations::input_stream_ref(in))::input_char_type;
		if (!::fast_io::operations::decay::scan_some_freestanding_decay(
				::fast_io::operations::input_stream_ref(in),
				::fast_io::io_scan_forward<char_type>(::fast_io::io_scan_alias(args))...))
		{
			::fast_io::herbceptions::throws_parse_errc(::fast_io::freestanding::parse_errc::end_of_file);
		}
	}
	else
	{
#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES)) &&                                     \
	__has_include(<stdio.h>)
		::fast_io::details::scan_after_io_scan_forward(
			::fast_io::io_scan_forward<char>(::fast_io::io_scan_alias(in)),
			::fast_io::io_scan_forward<char>(::fast_io::io_scan_alias(args))...);
#else
		// clang-format off
static_assert(device_error, "freestanding environment must provide IO device");
		// clang-format on
#endif
	}
}

template <typename input, typename... Args>
[[nodiscard]] inline constexpr ::fast_io::scan_some_result_t scan_some(input &&in, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::details::io_scan_may_throw<input, Args...>())
{
	constexpr bool device_error{::fast_io::operations::defines::has_input_or_io_stream_ref_define<input>};
	if constexpr (device_error)
	{
		using char_type = typename decltype(::fast_io::operations::input_stream_ref(in))::input_char_type;
		return ::fast_io::operations::decay::scan_some_freestanding_decay(
			::fast_io::operations::input_stream_ref(in),
			::fast_io::io_scan_forward<char_type>(::fast_io::io_scan_alias(args))...);
	}
	else
	{
#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES)) &&                                     \
	__has_include(<stdio.h>)
		return ::fast_io::details::scan_some_after_io_scan_forward(
			::fast_io::io_scan_forward<char>(::fast_io::io_scan_alias(in)),
			::fast_io::io_scan_forward<char>(::fast_io::io_scan_alias(args))...);
#else
		// clang-format off
static_assert(device_error, "freestanding environment must provide IO device");
		// clang-format on
#endif
	}
}

} // namespace io

namespace iomnp
{
using namespace ::fast_io::io;
using namespace ::fast_io::mnp;
} // namespace iomnp

} // namespace fast_io
