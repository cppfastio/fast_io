#pragma once

namespace fast_io
{

template<typename ...Args>
[[nodiscard]] inline
#if __cpp_lib_constexpr_string >= 201907L
constexpr
#endif
std::string concat(Args&& ...args)
{
	constexpr bool type_error{print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char>,Args...>};
	if constexpr(type_error)
	{
		return details::decay::basic_general_concat_phase1_decay_impl<false,char,std::string>(io_print_forward<char>(io_print_alias(args))...);
	}
	else
	{
static_assert(type_error,"some types are not printable, so we cannot concat std::string");
		return {};
	}
}

template<typename ...Args>
[[nodiscard]] inline
#if __cpp_lib_constexpr_string >= 201907L
constexpr
#endif
std::basic_string<wchar_t> wconcat(Args&& ...args)
{
	constexpr bool type_error{print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<wchar_t>,Args...>};
	if constexpr(type_error)
	{
		return details::decay::basic_general_concat_phase1_decay_impl<false,wchar_t,std::basic_string<wchar_t>>(io_print_forward<wchar_t>(io_print_alias(args))...);
	}
	else
	{
static_assert(type_error,"some types are not printable, so we cannot concat std::wstring");
		return {};
	}
}

template<typename ...Args>
[[nodiscard]] inline
#if __cpp_lib_constexpr_string >= 201907L
constexpr
#endif
std::u8string u8concat(Args&& ...args)
{
	constexpr bool type_error{print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char8_t>,Args...>};
	if constexpr(type_error)
	{
		return details::decay::basic_general_concat_phase1_decay_impl<false,char8_t,std::u8string>(io_print_forward<char8_t>(io_print_alias(args))...);
	}
	else
	{
static_assert(type_error,"some types are not printable, so we cannot concat std::u8string");
		return {};
	}
}

template<typename ...Args>
[[nodiscard]] inline
#if __cpp_lib_constexpr_string >= 201907L
constexpr
#endif
std::u16string u16concat(Args&& ...args)
{
	constexpr bool type_error{print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char16_t>,Args...>};
	if constexpr(type_error)
	{
		return details::decay::basic_general_concat_phase1_decay_impl<false,char16_t,std::u16string>(io_print_forward<char16_t>(io_print_alias(args))...);
	}
	else
	{
static_assert(type_error,"some types are not printable, so we cannot concat std::u16string");
		return {};
	}
}

template<typename ...Args>
[[nodiscard]] inline
#if __cpp_lib_constexpr_string >= 201907L
constexpr
#endif
std::u32string u32concat(Args&& ...args)
{
	constexpr bool type_error{print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char32_t>,Args...>};
	if constexpr(type_error)
	{
		return details::decay::basic_general_concat_phase1_decay_impl<false,char32_t,std::u32string>(io_print_forward<char32_t>(io_print_alias(args))...);
	}
	else
	{
static_assert(type_error,"some types are not printable, so we cannot concat std::u32string");
		return {};
	}
}

template<typename ...Args>
[[nodiscard]] inline
#if __cpp_lib_constexpr_string >= 201907L
constexpr
#endif
std::basic_string<char> concatln(Args&& ...args)
{
	constexpr bool type_error{print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char>,Args...>};
	if constexpr(type_error)
	{
		return details::decay::basic_general_concat_phase1_decay_impl<true,char,std::basic_string<char>>(io_print_forward<char>(io_print_alias(args))...);
	}
	else
	{
static_assert(type_error,"some types are not printable, so we cannot concatln std::wstring");
		return {};
	}
}

template<typename ...Args>
[[nodiscard]] inline
#if __cpp_lib_constexpr_string >= 201907L
constexpr
#endif
std::basic_string<wchar_t> wconcatln(Args&& ...args)
{
	constexpr bool type_error{print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<wchar_t>,Args...>};
	if constexpr(type_error)
	{
		return details::decay::basic_general_concat_phase1_decay_impl<true,wchar_t,std::basic_string<wchar_t>>(io_print_forward<wchar_t>(io_print_alias(args))...);
	}
	else
	{
static_assert(type_error,"some types are not printable, so we cannot concatln std::wstring");
		return {};
	}
}

template<typename ...Args>
[[nodiscard]] inline
#if __cpp_lib_constexpr_string >= 201907L
constexpr
#endif
std::u8string u8concatln(Args&& ...args)
{
	constexpr bool type_error{print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char8_t>,Args...>};
	if constexpr(type_error)
	{
		return details::decay::basic_general_concat_phase1_decay_impl<true,char8_t,std::u8string>(io_print_forward<char8_t>(io_print_alias(args))...);
	}
	else
	{
static_assert(type_error,"some types are not printable, so we cannot concatln std::u8string");
		return {};
	}
}

template<typename ...Args>
[[nodiscard]] inline
#if __cpp_lib_constexpr_string >= 201907L
constexpr
#endif
std::u16string u16concatln(Args&& ...args)
{
	constexpr bool type_error{print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char16_t>,Args...>};
	if constexpr(type_error)
	{
		return details::decay::basic_general_concat_phase1_decay_impl<true,char16_t,std::u16string>(io_print_forward<char16_t>(io_print_alias(args))...);
	}
	else
	{
static_assert(type_error,"some types are not printable, so we cannot concatln std::u16string");
		return {};
	}
}

template<typename ...Args>
[[nodiscard]] inline
#if __cpp_lib_constexpr_string >= 201907L
constexpr
#endif
std::u32string u32concatln(Args&& ...args)
{
	constexpr bool type_error{print_freestanding_okay<::fast_io::details::dummy_buffer_output_stream<char32_t>,Args...>};
	if constexpr(type_error)
	{
		return details::decay::basic_general_concat_phase1_decay_impl<true,char32_t,std::u32string>(io_print_forward<char32_t>(io_print_alias(args))...);
	}
	else
	{
static_assert(type_error,"some types are not printable, so we cannot concatln std::u32string");
		return {};
	}
}

}
