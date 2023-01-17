#pragma once

namespace fast_io
{


inline constexpr std::uintptr_t domain_define(error_type_t<::fast_io::freestanding::errc>) noexcept
{
	if constexpr(sizeof(std::uintptr_t)<=sizeof(std::uint_least16_t))
		return 52386u;
	else if constexpr(sizeof(std::uintptr_t)<=sizeof(std::uint_least32_t))
		return 1105291838u;
	else
		return 11138730069487200664ULL;
}

inline constexpr std::uintptr_t posix_domain_value{domain_define(error_type<::fast_io::freestanding::errc>)};

[[noreturn]] inline void throw_posix_error()
{
#ifdef __cpp_exceptions
#if defined(_MSC_VER) && (!defined(_HAS_EXCEPTIONS) || _HAS_EXCEPTIONS == 0)
	fast_terminate();
#else
	throw ::fast_io::error{posix_domain_value,static_cast<std::uintptr_t>(static_cast<unsigned>(errno))};
#endif
#else
	fast_terminate();
#endif
}
[[noreturn]] inline void throw_posix_error([[maybe_unused]] int err)
{
#ifdef __cpp_exceptions
#if defined(_MSC_VER) && (!defined(_HAS_EXCEPTIONS) || _HAS_EXCEPTIONS == 0)
	fast_terminate();
#else
	throw ::fast_io::error{posix_domain_value,static_cast<std::uintptr_t>(static_cast<unsigned>(err))};
#endif
#else
	fast_terminate();
#endif
}

template<std::integral char_type>
inline constexpr basic_io_scatter_t<char_type> status_io_print_forward(io_alias_type_t<char_type>,::fast_io::freestanding::errc perr) noexcept
{
	return get_posix_errno_scatter<char_type>(static_cast<int>(perr));
}

#if !((defined(_WIN32)&&!defined(__WINE__)) || defined(__CYGWIN__))

namespace details
{
inline constexpr bool posix_code_equivalent_impl(std::uintptr_t domain,std::uintptr_t code,int e) noexcept
{
	if(posix_domain_value!=domain)
		return false;
	else
	{
		using common_type = std::common_type_t<std::uintptr_t,unsigned>;
		return code==static_cast<common_type>(static_cast<unsigned>(e));
	}
}

inline constexpr int to_posix_code_impl(std::uintptr_t domain,std::uintptr_t code) noexcept
{
	if(posix_domain_value!=domain)
	{
#if defined(EOTHER)
		return EOTHER;
#else
		return EINVAL;
#endif
	}
	else
	{
		return static_cast<int>(static_cast<unsigned>(code));
	}
}

}

inline constexpr bool equivalent_define(error_type_t<::fast_io::freestanding::errc>,::fast_io::error e,::fast_io::freestanding::errc p)
{
	return ::fast_io::details::posix_code_equivalent_impl(e.domain,e.code,static_cast<int>(p));
}

inline constexpr ::fast_io::freestanding::errc to_code_define(error_type_t<::fast_io::freestanding::errc>,error e) noexcept
{
	return ::fast_io::freestanding::errc{::fast_io::details::to_posix_code_impl(e.domain,e.code)};
}

using native_code = ::fast_io::freestanding::errc;

inline constexpr ::fast_io::freestanding::errc print_alias_define(::fast_io::io_alias_t,::fast_io::error e) noexcept
{
	return to_code_define(error_type<::fast_io::freestanding::errc>,e);
}

#endif

}
