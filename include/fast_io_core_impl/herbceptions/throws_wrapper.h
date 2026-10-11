#pragma once

#if __has_include(<errno.h>)
#include <errno.h>
#endif

namespace fast_io::herbceptions
{

#if !defined(__HERBCEPTIONS__) && defined(FAST_IO_CPP_EXCEPTIONS)
namespace details
{
/* legacy-EH shim: errc-family throws degrade to a plain
   ::fast_io::error{domain, code} so catch handlers still see an
   error object instead of a terminate. domain values are arbitrary
   per-code-space constants; there is no registry or printing. */
inline constexpr ::std::size_t posix_domain_value{11138730069487200664ULL};
inline constexpr ::std::size_t parse_domain_value{17020429607921007685ULL};
#if defined(_WIN32) || defined(__CYGWIN__)
inline constexpr ::std::size_t nt_domain_value{17311375720795711021ULL};
inline constexpr ::std::size_t win32_domain_value{12911969238956088748ULL};
inline constexpr ::std::size_t wine_domain_value{14792643980012753601ULL};
inline constexpr ::std::size_t com_domain_value{11400714819323198485ULL};
#endif

inline constexpr ::std::size_t legacy_errc_domain(::std::errc) noexcept
{
	return posix_domain_value;
}
inline constexpr ::std::size_t legacy_errc_domain(::fast_io::freestanding::parse_errc) noexcept
{
	return parse_domain_value;
}
#if defined(_WIN32) || defined(__CYGWIN__)
inline constexpr ::std::size_t legacy_errc_domain(::fast_io::freestanding::nt_errc) noexcept
{
	return nt_domain_value;
}
inline constexpr ::std::size_t legacy_errc_domain(::fast_io::freestanding::win32_errc) noexcept
{
	return win32_domain_value;
}
inline constexpr ::std::size_t legacy_errc_domain(::fast_io::freestanding::wine_errc) noexcept
{
	return wine_domain_value;
}
inline constexpr ::std::size_t legacy_errc_domain(::fast_io::freestanding::com_errc) noexcept
{
	return com_domain_value;
}
#endif
} // namespace details
#endif

template <typename T>
	requires ::std::is_enum_v<T>
[[noreturn]] inline constexpr void throws_errc_generic([[maybe_unused]] T ec) FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(__HERBCEPTIONS__)
	throw throws ec;
#elif defined(FAST_IO_CPP_EXCEPTIONS)
	throw ::fast_io::error{::fast_io::herbceptions::details::legacy_errc_domain(ec),
						   static_cast<::std::size_t>(static_cast<::std::underlying_type_t<T>>(ec))};
#else
	::fast_io::fast_terminate();
#endif
}

[[noreturn]] inline constexpr void throws_errc([[maybe_unused]] ::std::errc ec) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_generic(ec);
}

[[noreturn]] inline constexpr void throws_errc_with_value([[maybe_unused]] int ec) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_generic(static_cast<::std::errc>(ec));
}

[[noreturn]] inline constexpr void throws_parse_errc([[maybe_unused]] ::fast_io::freestanding::parse_errc ec) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_generic(static_cast<::fast_io::freestanding::parse_errc>(ec));
}

#if defined(_WIN32) || defined(__CYGWIN__)
[[noreturn]] inline constexpr void throws_nt_errc([[maybe_unused]] ::fast_io::freestanding::nt_errc ec) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_generic(ec);
}
[[noreturn]] inline constexpr void throws_nt_errc_with_value([[maybe_unused]] ::std::uint_least32_t ec) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_generic(static_cast<::fast_io::freestanding::nt_errc>(ec));
}

[[noreturn]] inline constexpr void throws_win32_errc([[maybe_unused]] ::fast_io::freestanding::win32_errc ec) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_generic(ec);
}

[[noreturn]] inline constexpr void throws_win32_errc_with_value([[maybe_unused]] ::std::uint_least32_t ec) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_generic(static_cast<::fast_io::freestanding::win32_errc>(ec));
}

[[noreturn]] inline constexpr void throws_com_errc([[maybe_unused]] ::fast_io::freestanding::com_errc ec) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_generic(ec);
}

[[noreturn]] inline constexpr void throws_com_errc_with_value([[maybe_unused]] ::std::uint_least32_t ec) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_generic(static_cast<::fast_io::freestanding::com_errc>(ec));
}

[[noreturn]] inline constexpr void throws_wine_errc([[maybe_unused]] ::fast_io::freestanding::wine_errc ec) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_generic(ec);
}

[[noreturn]] inline constexpr void throws_wine_errc_with_value([[maybe_unused]] ::std::uint_least32_t ec) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_generic(static_cast<::fast_io::freestanding::wine_errc>(ec));
}

#endif
} // namespace fast_io::herbceptions

namespace fast_io
{

#ifdef errno
[[noreturn]] inline constexpr void throw_posix_error() FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_with_value(errno);
}
#endif

[[noreturn]] inline constexpr void throw_posix_error([[maybe_unused]] int err) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_with_value(err);
}

} // namespace fast_io
