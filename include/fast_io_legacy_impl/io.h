#pragma once

#if ((__STDC_HOSTED__==1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED==1) && !defined(_LIBCPP_FREESTANDING)) || defined(FAST_IO_ENABLE_HOSTED_FEATURES))
#include"defined_types.h"
#endif

namespace fast_io::io
{

template<typename T,typename... Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void print(T&& t,Args&& ...args)
{
	constexpr bool device_error{::fast_io::operations::defines::has_output_or_io_stream_ref_define<T>};
	if constexpr(device_error)
	{
		using char_type = typename decltype(::fast_io::operations::output_stream_ref(t))::output_char_type;
		constexpr bool type_error{::fast_io::operations::decay::defines::print_freestanding_params_decay_okay<char_type,
			decltype(::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args)))...>};
		if constexpr(type_error)
		{
			::fast_io::operations::decay::print_freestanding_decay<false>(::fast_io::operations::output_stream_ref(t),
				::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args))...);
		}
		else
		{
static_assert(type_error,"some types are not printable for print");
		}
	}
	else
	{
#if ((__STDC_HOSTED__==1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED==1) && !defined(_LIBCPP_FREESTANDING)) || defined(FAST_IO_ENABLE_HOSTED_FEATURES)) && __has_include(<stdio.h>)
		constexpr bool type_error{::fast_io::operations::decay::defines::print_freestanding_params_decay_okay
		<char,decltype(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(t))),
		decltype(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args)))...>};
		if constexpr(type_error)
		{
			::fast_io::details::print_after_io_print_forward<false>(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(t)),::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args))...);
		}
		else
		{
static_assert(type_error,"some types are not printable for print on default C's stdout");
		}
#else
static_assert(device_error,"freestanding environment must provide IO device for print");
#endif
	}
}

template<typename T,typename... Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void println(T&& t,Args&& ...args)
{
	constexpr bool device_error{::fast_io::operations::defines::has_output_or_io_stream_ref_define<T>};
	if constexpr(device_error)
	{
		using char_type = typename decltype(::fast_io::operations::output_stream_ref(t))::output_char_type;
		constexpr bool type_error{::fast_io::operations::decay::defines::print_freestanding_params_decay_okay<char_type,
			decltype(::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args)))...>};
		if constexpr(type_error)
		{
			::fast_io::operations::decay::print_freestanding_decay<true>(::fast_io::operations::output_stream_ref(t),
				::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args))...);
		}
		else
		{
static_assert(type_error,"some types are not printable for println");
		}
	}
	else
	{
#if ((__STDC_HOSTED__==1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED==1) && !defined(_LIBCPP_FREESTANDING)) || defined(FAST_IO_ENABLE_HOSTED_FEATURES)) && __has_include(<stdio.h>)
		constexpr bool type_error{::fast_io::operations::decay::defines::print_freestanding_params_decay_okay
		<char,decltype(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(t))),
		decltype(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args)))...>};
		if constexpr(type_error)
		{
			::fast_io::details::print_after_io_print_forward<true>(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(t)),::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args))...);
		}
		else
		{
static_assert(type_error,"some types are not printable for print on default C's stdout");
		}
#else
static_assert(device_error,"freestanding environment must provide IO device for print");
#endif
	}
}

template<typename T,typename... Args>
inline constexpr void perr(T&& t,Args&&... args)
{
	constexpr bool device_error{::fast_io::operations::defines::has_output_or_io_stream_ref_define<T>};
	if constexpr(device_error)
	{
		using char_type = typename decltype(::fast_io::operations::output_stream_ref(t))::output_char_type;
		constexpr bool type_error{::fast_io::operations::decay::defines::print_freestanding_params_decay_okay<char_type,
			decltype(::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args)))...>};
		if constexpr(type_error)
		{
			::fast_io::operations::decay::print_freestanding_decay_cold<false>(::fast_io::operations::output_stream_ref(t),
				::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args))...);
		}
		else
		{
static_assert(type_error,"some types are not printable for perr");
		}
	}
	else
	{
#if ((__STDC_HOSTED__==1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED==1) && !defined(_LIBCPP_FREESTANDING) && !defined(__AVR__)) || defined(FAST_IO_ENABLE_HOSTED_FEATURES))
		constexpr bool type_error{::fast_io::operations::decay::defines::print_freestanding_params_decay_okay
		<char,decltype(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(t))),
		decltype(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args)))...>};
		if constexpr(type_error)
		{
			::fast_io::details::perr_after_io_print_forward<false>(fast_io::io_print_forward<char>(fast_io::io_print_alias(t)),fast_io::io_print_forward<char>(fast_io::io_print_alias(args))...);
		}
		else
		{
static_assert(type_error,"some types are not printable for perr on native err");
		}
#else
static_assert(device_error,"freestanding environment must provide IO device");
#endif
	}
}

template<typename T,typename... Args>
inline constexpr void perrln(T&& t,Args&&... args)
{
	constexpr bool device_error{::fast_io::operations::defines::has_output_or_io_stream_ref_define<T>};
	if constexpr(device_error)
	{
		using char_type = typename decltype(::fast_io::operations::output_stream_ref(t))::output_char_type;
		constexpr bool type_error{::fast_io::operations::decay::defines::print_freestanding_params_decay_okay<char_type,
			decltype(::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args)))...>};
		if constexpr(type_error)
		{
			::fast_io::operations::decay::print_freestanding_decay_cold<true>(::fast_io::operations::output_stream_ref(t),
				::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args))...);
		}
		else
		{
static_assert(type_error,"some types are not printable for perr");
		}
	}
	else
	{
#if ((__STDC_HOSTED__==1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED==1) && !defined(_LIBCPP_FREESTANDING) && !defined(__AVR__)) || defined(FAST_IO_ENABLE_HOSTED_FEATURES))
		constexpr bool type_error{::fast_io::operations::decay::defines::print_freestanding_params_decay_okay
		<char,decltype(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(t))),
		decltype(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args)))...>};
		if constexpr(type_error)
		{
			::fast_io::details::perr_after_io_print_forward<true>(fast_io::io_print_forward<char>(fast_io::io_print_alias(t)),fast_io::io_print_forward<char>(fast_io::io_print_alias(args))...);
		}
		else
		{
static_assert(type_error,"some types are not printable for perrln on native err");
		}
#else
static_assert(device_error,"freestanding environment must provide IO device");
#endif
	}
}

template<typename... Args>
[[noreturn]] inline constexpr void panic(Args&&... args) noexcept
{
	if constexpr(sizeof...(Args)!=0)
	{
#ifdef __cpp_exceptions
	try
	{
#endif
		::fast_io::io::perr(::std::forward<Args>(args)...);
#ifdef __cpp_exceptions
	}
	catch(...){}
#endif
	}
	::fast_io::fast_terminate();
}

template<typename... Args>
requires (sizeof...(Args)!=0)
[[noreturn]] inline constexpr void panicln(Args&&... args) noexcept
{
#ifdef __cpp_exceptions
	try
	{
#endif
		::fast_io::io::perrln(::std::forward<Args>(args)...);
#ifdef __cpp_exceptions
	}
	catch(...){}
#endif
	::fast_io::fast_terminate();
}

//Allow debug print
#ifndef FAST_IO_DISABLE_DEBUG_PRINT
//With debugging. We output to POSIX fd or Win32 Handle directly instead of C's stdout.
template<typename T,typename... Args>
inline constexpr void debug_print(T&& t,Args&& ...args)
{
	constexpr bool device_error{fast_io::output_stream<std::remove_cvref_t<T>>||fast_io::status_output_stream<std::remove_cvref_t<T>>};
	if constexpr(device_error)
	{
		using char_type = typename decltype(::fast_io::operations::output_stream_ref(t))::output_char_type;
		constexpr bool type_error{::fast_io::operations::decay::defines::print_freestanding_params_decay_okay<char_type,
			decltype(::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args)))...>};
		if constexpr(type_error)
		{
			::fast_io::operations::decay::print_freestanding_decay<false>(::fast_io::operations::output_stream_ref(t),
				fast_io::io_print_forward<char_type>(fast_io::io_print_alias(args))...);
		}
		else
		{
static_assert(type_error,"some types are not printable for debug_print");
		}
	}
	else
	{
#if ((__STDC_HOSTED__==1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED==1) && !defined(_LIBCPP_FREESTANDING)) || defined(FAST_IO_ENABLE_HOSTED_FEATURES))
		constexpr bool type_error{::fast_io::operations::decay::defines::print_freestanding_params_decay_okay
		<char,decltype(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(t))),
		decltype(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args)))...>};
		if constexpr(type_error)
		{
			fast_io::details::debug_print_after_io_print_forward<false>(fast_io::io_print_forward<char>(fast_io::io_print_alias(t)),fast_io::io_print_forward<char>(fast_io::io_print_alias(args))...);
		}
		else
		{
static_assert(type_error,"some types are not printable for debug_print on native out");
		}
#else
static_assert(device_error,"freestanding environment must provide IO device");
#endif
	}
}

template<typename T,typename... Args>
inline constexpr void debug_println(T&& t,Args&& ...args)
{
	constexpr bool device_error{fast_io::output_stream<std::remove_cvref_t<T>>||fast_io::status_output_stream<std::remove_cvref_t<T>>};
	if constexpr(device_error)
	{
		using char_type = typename decltype(::fast_io::operations::output_stream_ref(t))::output_char_type;
		constexpr bool type_error{::fast_io::operations::decay::defines::print_freestanding_params_decay_okay<char_type,
			decltype(::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(args)))...>};
		if constexpr(type_error)
		{
			::fast_io::operations::decay::print_freestanding_decay<true>(::fast_io::operations::output_stream_ref(t),
				fast_io::io_print_forward<char_type>(fast_io::io_print_alias(args))...);
		}
		else
		{
static_assert(type_error,"some types are not printable for debug_print");
		}
	}
	else
	{
#if ((__STDC_HOSTED__==1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED==1) && !defined(_LIBCPP_FREESTANDING)) || defined(FAST_IO_ENABLE_HOSTED_FEATURES))
		constexpr bool type_error{::fast_io::operations::decay::defines::print_freestanding_params_decay_okay
		<char,decltype(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(t))),
		decltype(::fast_io::io_print_forward<char>(::fast_io::io_print_alias(args)))...>};
		if constexpr(type_error)
		{
			fast_io::details::debug_print_after_io_print_forward<true>(fast_io::io_print_forward<char>(fast_io::io_print_alias(t)),fast_io::io_print_forward<char>(fast_io::io_print_alias(args))...);
		}
		else
		{
static_assert(type_error,"some types are not printable for debug_println on native out");
		}
#else
static_assert(device_error,"freestanding environment must provide IO device");
#endif
	}
}

template<typename... Args>
requires (sizeof...(Args)!=0)
inline constexpr void debug_perr(Args&&... args)
{
	::fast_io::io::perr(::std::forward<Args>(args)...);
}

template<typename... Args>
requires (sizeof...(Args)!=0)
inline constexpr void debug_perrln(Args&&... args)
{
	::fast_io::io::perrln(::std::forward<Args>(args)...);
}
#endif

template<bool report=false,typename input,typename... Args>
inline constexpr ::std::conditional_t<report,bool,void> scan(input&& in,Args&& ...args)
{
	constexpr bool device_error{::fast_io::operations::defines::has_input_or_io_stream_ref_define<input>};
	if constexpr(device_error)
	{
		using char_type = typename decltype(::fast_io::operations::input_stream_ref(in))::input_char_type;
		if constexpr(report)
		{
			return ::fast_io::operations::decay::scan_freestanding_decay(::fast_io::operations::input_stream_ref(in),
				::fast_io::io_scan_forward<char_type>(::fast_io::io_scan_alias(args))...);
		}
		else
		{
			if(!::fast_io::operations::decay::scan_freestanding_decay(::fast_io::operations::input_stream_ref(in),
				::fast_io::io_scan_forward<char_type>(::fast_io::io_scan_alias(args))...))
			{
				::fast_io::throw_parse_code(::fast_io::parse_code::end_of_file);
			}
		}
	}
	else
	{
#if ((__STDC_HOSTED__==1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED==1) && !defined(_LIBCPP_FREESTANDING)) || defined(FAST_IO_ENABLE_HOSTED_FEATURES)) && __has_include(<stdio.h>)
		return ::fast_io::details::scan_after_io_scan_forward<report>(::fast_io::io_scan_forward<char>(::fast_io::io_scan_alias(in)),::fast_io::io_scan_forward<char>(::fast_io::io_scan_alias(args))...);
#else
static_assert(device_error,"freestanding environment must provide IO device");
#endif
	}
}

}
