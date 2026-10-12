#pragma once

#if __has_include(<stdio.h>)
#include "c/impl.h"
#endif

namespace fast_io
{
#if !defined(__AVR__)

inline
#if defined(__WINE__) || !defined(_WIN32)
	constexpr
#endif
	native_io_observer
	in() noexcept
{
	return native_stdin();
}

inline
#if defined(__WINE__) || !defined(_WIN32)
	constexpr
#endif
	native_io_observer
	out() noexcept
{
	return native_stdout();
}

inline
#if defined(__WINE__) || !defined(_WIN32)
	constexpr
#endif
	native_io_observer
	err() noexcept
{
	return native_stderr();
}

inline
#if defined(__WINE__) || !defined(_WIN32)
	constexpr
#endif
	decltype(auto)
	u8in() noexcept
{
	return native_stdin<char8_t>();
}

inline
#if defined(__WINE__) || !defined(_WIN32)
	constexpr
#endif
	decltype(auto)
	u8out() noexcept
{
	return native_stdout<char8_t>();
}

inline
#if defined(__WINE__) || !defined(_WIN32)
	constexpr
#endif
	decltype(auto)
	u8err() noexcept
{
	return native_stderr<char8_t>();
}

using in_buf_type = basic_ibuf<native_io_observer>;
using out_buf_type = basic_obuf<native_io_observer>;

using u8in_buf_type = basic_ibuf<u8native_io_observer>;
using u8out_buf_type = basic_obuf<u8native_io_observer>;

using in_buf_type_lockable = basic_io_lockable<in_buf_type>;
using out_buf_type_lockable = basic_io_lockable<out_buf_type>;

using u8in_buf_type_lockable = basic_io_lockable<u8in_buf_type>;
using u8out_buf_type_lockable = basic_io_lockable<u8out_buf_type>;

#endif

namespace details
{

template <bool line, typename... Args>
inline constexpr void print_after_io_print_forward(Args... args)
#if __has_include(<stdio.h>)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::operations::decay::print_freestanding_decay<line>(c_stdout(), args...))
#else
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::operations::decay::print_freestanding_decay<line>(out(), args...))
#endif
{
#if __has_include(<stdio.h>)
	::fast_io::operations::decay::print_freestanding_decay<line>(c_stdout(), args...);
#else
	::fast_io::operations::decay::print_freestanding_decay<line>(out(), args...);
#endif
}

template <bool line, typename... Args>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void perr_after_io_print_forward(Args... args)
#if defined(__AVR__)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::operations::decay::print_freestanding_decay<line>(c_stderr(), args...))
#else
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::operations::decay::print_freestanding_decay<line>(err(), args...))
#endif
{
#if defined(__AVR__)
	::fast_io::operations::decay::print_freestanding_decay<line>(c_stderr(), args...);
#else
	::fast_io::operations::decay::print_freestanding_decay<line>(err(), args...);
#endif
}

template <bool line, typename... Args>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void debug_print_after_io_print_forward(Args... args)
#if defined(__AVR__)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::operations::decay::print_freestanding_decay<line>(c_stdout(), args...))
#else
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::operations::decay::print_freestanding_decay<line>(out(), args...))
#endif
{
#if defined(__AVR__)
	::fast_io::operations::decay::print_freestanding_decay<line>(c_stdout(), args...);
#else
	::fast_io::operations::decay::print_freestanding_decay<line>(out(), args...);
#endif
}

template <bool line, typename T, typename... Args>
inline consteval bool io_print_may_throw() noexcept
{
	if constexpr (::fast_io::operations::defines::print_freestanding_okay<T, Args...>)
	{
		return !noexcept(::fast_io::operations::decay::print_freestanding_decay<line>(
			::fast_io::operations::output_stream_ref(::std::declval<T>()),
			::fast_io::io_print_forward<typename decltype(::fast_io::operations::output_stream_ref(
				::std::declval<T>()))::output_char_type>(::fast_io::io_print_alias(::std::declval<Args>()))...));
	}
	else
	{
		return !noexcept(::fast_io::details::print_after_io_print_forward<line>(
			::fast_io::io_print_forward<char>(::fast_io::io_print_alias(::std::declval<T>())),
			::fast_io::io_print_forward<char>(::fast_io::io_print_alias(::std::declval<Args>()))...));
	}
}

template <bool line, typename T, typename... Args>
inline consteval bool io_perr_may_throw() noexcept
{
	if constexpr (::fast_io::operations::defines::print_freestanding_okay<T, Args...>)
	{
		return !noexcept(::fast_io::operations::decay::print_freestanding_decay_cold<line>(
			::fast_io::operations::output_stream_ref(::std::declval<T>()),
			::fast_io::io_print_forward<typename decltype(::fast_io::operations::output_stream_ref(
				::std::declval<T>()))::output_char_type>(::fast_io::io_print_alias(::std::declval<Args>()))...));
	}
	else
	{
		return !noexcept(::fast_io::details::perr_after_io_print_forward<line>(
			::fast_io::io_print_forward<char>(::fast_io::io_print_alias(::std::declval<T>())),
			::fast_io::io_print_forward<char>(::fast_io::io_print_alias(::std::declval<Args>()))...));
	}
}

template <bool line, typename T, typename... Args>
inline consteval bool io_debug_print_may_throw() noexcept
{
	if constexpr (::fast_io::operations::defines::print_freestanding_okay<T, Args...>)
	{
		return !noexcept(::fast_io::operations::decay::print_freestanding_decay<line>(
			::fast_io::operations::output_stream_ref(::std::declval<T>()),
			::fast_io::io_print_forward<typename decltype(::fast_io::operations::output_stream_ref(
				::std::declval<T>()))::output_char_type>(::fast_io::io_print_alias(::std::declval<Args>()))...));
	}
	else
	{
		return !noexcept(::fast_io::details::debug_print_after_io_print_forward<line>(
			::fast_io::io_print_forward<char>(::fast_io::io_print_alias(::std::declval<T>())),
			::fast_io::io_print_forward<char>(::fast_io::io_print_alias(::std::declval<Args>()))...));
	}
}

template <typename... Args>
inline constexpr void scan_after_io_scan_forward(Args... args)
#if __has_include(<stdio.h>)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::operations::decay::scan_some_freestanding_decay(c_stdin(), args...))
#endif
{
#if __has_include(<stdio.h>)
	if (!::fast_io::operations::decay::scan_some_freestanding_decay(c_stdin(), args...))
	{
		::fast_io::herbceptions::throws_parse_errc(::fast_io::freestanding::parse_errc::end_of_file);
	}
#endif
}

template <typename... Args>
[[nodiscard]] inline constexpr ::fast_io::scan_some_result_t scan_some_after_io_scan_forward(Args... args)
#if __has_include(<stdio.h>)
	FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(::fast_io::operations::decay::scan_some_freestanding_decay(c_stdin(), args...))
#else
	FAST_IO_HERBCEPTIONS_THROWS
#endif
{
#if __has_include(<stdio.h>)
	return ::fast_io::operations::decay::scan_some_freestanding_decay(c_stdin(), args...);
#else
	return {};
#endif
}

template <typename input, typename... Args>
inline consteval bool io_scan_may_throw() noexcept
{
	if constexpr (::fast_io::operations::defines::has_input_or_io_stream_ref_define<input>)
	{
		return !noexcept(::fast_io::operations::decay::scan_some_freestanding_decay(
			::fast_io::operations::input_stream_ref(::std::declval<input>()),
			::fast_io::io_scan_forward<typename decltype(::fast_io::operations::input_stream_ref(
				::std::declval<input>()))::input_char_type>(::fast_io::io_scan_alias(::std::declval<Args>()))...));
	}
	else
	{
		return !noexcept(::fast_io::details::scan_some_after_io_scan_forward(
			::fast_io::io_scan_forward<char>(::fast_io::io_scan_alias(::std::declval<input>())),
			::fast_io::io_scan_forward<char>(::fast_io::io_scan_alias(::std::declval<Args>()))...));
	}
}

} // namespace details

} // namespace fast_io
