#pragma once

namespace fast_io
{
namespace details
{
template<::std::integral char_type>
inline constexpr char_type* print_reserve_parse_code_impl(char_type* iter,parse_code code) noexcept
{
	if constexpr(std::same_as<char_type,char>)
	{
	switch(code)
	{
	case parse_code::ok:
		return copy_string_literal("ok",iter);
	case parse_code::end_of_file:
		return copy_string_literal("end_of_file",iter);
	case parse_code::partial:
		return copy_string_literal("partial",iter);
	case parse_code::invalid:
		return copy_string_literal("invalid",iter);
	case parse_code::overflow:
		return copy_string_literal("overflow",iter);
	default:
		return copy_string_literal("unknown",iter);
	}
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
	switch(code)
	{
	case parse_code::ok:
		return copy_string_literal(L"ok",iter);
	case parse_code::end_of_file:
		return copy_string_literal(L"end_of_file",iter);
	case parse_code::partial:
		return copy_string_literal(L"partial",iter);
	case parse_code::invalid:
		return copy_string_literal(L"invalid",iter);
	case parse_code::overflow:
		return copy_string_literal(L"overflow",iter);
	default:
		return copy_string_literal(L"unknown",iter);
	}
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
	switch(code)
	{
	case parse_code::ok:
		return copy_string_literal(u"ok",iter);
	case parse_code::end_of_file:
		return copy_string_literal(u"end_of_file",iter);
	case parse_code::partial:
		return copy_string_literal(u"partial",iter);
	case parse_code::invalid:
		return copy_string_literal(u"invalid",iter);
	case parse_code::overflow:
		return copy_string_literal(u"overflow",iter);
	default:
		return copy_string_literal(u"unknown",iter);
	}
	}
	else
	{
	switch(code)
	{
	case parse_code::ok:
		return copy_string_literal(u8"ok",iter);
	case parse_code::end_of_file:
		return copy_string_literal(u8"end_of_file",iter);
	case parse_code::partial:
		return copy_string_literal(u8"partial",iter);
	case parse_code::invalid:
		return copy_string_literal(u8"invalid",iter);
	case parse_code::overflow:
		return copy_string_literal(u8"overflow",iter);
	default:
		return copy_string_literal(u8"unknown",iter);
	}
	}
}

}

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,parse_code>) noexcept
{
	return 16;
}

template<::std::integral char_type>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,parse_code>, char_type* iter,parse_code code) noexcept
{
	return details::print_reserve_parse_code_impl(iter,code);
}

inline constexpr std::uintptr_t domain_define(error_type_t<parse_code>) noexcept
{
	if constexpr(sizeof(std::uintptr_t)<=sizeof(std::uint_least16_t))
		return 19339u;
	else if constexpr(sizeof(std::uintptr_t)<=sizeof(std::uint_least32_t))
		return 2666707100u;
	else
		return 17020429607921007685ULL;
}

inline constexpr std::uintptr_t parse_domain_value{domain_define(error_type<parse_code>)};

[[noreturn]] inline void throw_parse_code([[maybe_unused]] parse_code code)
{
#ifdef __cpp_exceptions
#if defined(_MSC_VER) && (!defined(_HAS_EXCEPTIONS) || _HAS_EXCEPTIONS == 0)
	fast_terminate();
#else
	throw ::fast_io::error{parse_domain_value,static_cast<std::uintptr_t>(static_cast<char8_t>(code))};
#endif
#else
	fast_terminate();
#endif
}

namespace details
{

inline constexpr bool parse_code_equivalent_impl(std::uintptr_t domain,std::uintptr_t code,std::uintptr_t e) noexcept
{
	return domain==parse_domain_value&&code==e;
}

}

inline constexpr bool equivalent_define(error_type_t<parse_code>,error e,parse_code code) noexcept
{
	return ::fast_io::details::parse_code_equivalent_impl(e.domain,e.code,static_cast<std::uintptr_t>(static_cast<char8_t>(code)));
}

}
