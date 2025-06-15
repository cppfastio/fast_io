#pragma once
#if __has_include(<cwchar>)
#include <cwchar>
#elif __has_include(<wchar.h>)
#include <wchar.h>
#endif
/*

Provide a general emulation of FILE*'s internal implementation with fgetc_unlocked and ungetc_unlocked.

It is probably slow. However, it is a general case when we have not supported hacks of libcs on your platform.

BTW, musl libc and BSD libc does not provide general buffering for wide characters. This should cover that gap.
*/

namespace fast_io
{

namespace details
{
template <::std::integral char_type>
inline auto fgetc_unlocked_impl(FILE *fp) noexcept
{
	if constexpr (::std::same_as<char_type, char> || ::std::same_as<char_type, char8_t>)
	{

#pragma push_macro("FAST_IO_FGETC_UNLOCKED")
#pragma push_macro("FAST_IO_FGETC")
#pragma push_macro("FAST_IO_PLATFORM_SUPPORTS_FGETC_UNLOCKED")
#pragma push_macro("FAST_IO_FGETC_UNLOCKED_IMPL")

#undef FAST_IO_FGETC_UNLOCKED
#undef FAST_IO_FGETC
#undef FAST_IO_PLATFORM_SUPPORTS_FGETC_UNLOCKED
#undef FAST_IO_FGETC_UNLOCKED_IMPL

#if (defined(_WIN32) && !defined(__WINE__))
#ifdef _fgetc_nolock
#define FAST_IO_FGETC_UNLOCKED(...) ::_fgetc_nolock(__VA_ARGS__)
#else
#define FAST_IO_FGETC_UNLOCKED(...) ::fast_io::noexcept_call(::_fgetc_nolock, __VA_ARGS__)
#endif
#elif defined(__BIONIC__)
#define FAST_IO_FGETC_UNLOCKED(...) ::fast_io::noexcept_call(::getc_unlocked, __VA_ARGS__)
#endif

#ifndef FAST_IO_FGETC_UNLOCKED
#if defined(__GLIBC__) || defined(__LLVM_LIBC_TYPES_FILE_H__)
#define FAST_IO_FGETC_UNLOCKED(...) ::fgetc_unlocked(__VA_ARGS__)
#elif defined(fgetc_unlocked)
#define FAST_IO_FGETC_UNLOCKED(...) fgetc_unlocked(__VA_ARGS__)
#else
#define FAST_IO_FGETC_UNLOCKED(...) ::fast_io::noexcept_call(::fgetc_unlocked, __VA_ARGS__)
#endif
#endif


#if defined(__GLIBC__) || defined(__LLVM_LIBC_TYPES_FILE_H__)
#define FAST_IO_FGETC(...) ::fgetc(__VA_ARGS__)
#elif defined(fgetc_unlocked)
#define FAST_IO_FGETC(...) fgetc(__VA_ARGS__)
#else
#define FAST_IO_FGETC(...) ::fast_io::noexcept_call(::fgetc, __VA_ARGS__)
#endif

#if defined(__BIONIC__)
#define FAST_IO_PLATFORM_SUPPORTS_FGETC_UNLOCKED 1
#elif defined(_WIN32) && !defined(__WINE__)
#define FAST_IO_PLATFORM_SUPPORTS_FGETC_UNLOCKED 1
#elif defined(__LLVM_LIBC_TYPES_FILE_H__)
#define FAST_IO_PLATFORM_SUPPORTS_FGETC_UNLOCKED 1
#elif defined(__NEWLIB__)
#if __GNU_VISIBLE
#define FAST_IO_PLATFORM_SUPPORTS_FGETC_UNLOCKED 1
#endif
#elif _POSIX_C_SOURCE >= 199309L || _XOPEN_SOURCE || _POSIX_SOURCE || _BSD_SOURCE || _SVID_SOURCE
#define FAST_IO_PLATFORM_SUPPORTS_FGETC_UNLOCKED 1
#endif

#if defined(FAST_IO_PLATFORM_SUPPORTS_FGETC_UNLOCKED)
#define FAST_IO_FGETC_UNLOCKED_IMPL(...) FAST_IO_FGETC_UNLOCKED(__VA_ARGS__)
#else
#define FAST_IO_FGETC_UNLOCKED_IMPL(...) FAST_IO_FGETC(__VA_ARGS__)
#endif

		return FAST_IO_FGETC_UNLOCKED_IMPL(fp);

#pragma pop_macro("FAST_IO_FGETC_UNLOCKED_IMPL")
#pragma pop_macro("FAST_IO_PLATFORM_SUPPORTS_FGETC_UNLOCKED")
#pragma pop_macro("FAST_IO_FGETC")
#pragma pop_macro("FAST_IO_FGETC_UNLOCKED")
	}
	else
	{
#pragma push_macro("FAST_IO_FGETWC_UNLOCKED")
#pragma push_macro("FAST_IO_FGETWC")
#pragma push_macro("FAST_IO_PLATFORM_SUPPORTS_FGETWC_UNLOCKED")
#pragma push_macro("FAST_IO_PLATFORM_NOT_SUPPORTS_FGETWC")
#pragma push_macro("FAST_IO_FGETWC_UNLOCKED_IMPL")

#undef FAST_IO_FGETWC_UNLOCKED
#undef FAST_IO_FGETWC
#undef FAST_IO_PLATFORM_SUPPORTS_FGETWC_UNLOCKED
#undef FAST_IO_PLATFORM_NOT_SUPPORTS_FGETWC
#undef FAST_IO_FGETWC_UNLOCKED_IMPL

#if (defined(_WIN32) && !defined(__WINE__))
#ifdef _fgetwc_nolock
#define FAST_IO_FGETWC_UNLOCKED(...) _fgetwc_nolock(__VA_ARGS__)
#else
#define FAST_IO_FGETWC_UNLOCKED(...) ::fast_io::noexcept_call(::_fgetwc_nolock, __VA_ARGS__)
#endif
#elif defined(__BIONIC__)
#define FAST_IO_FGETWC_UNLOCKED(...) ::fast_io::noexcept_call(::getc_unlocked, __VA_ARGS__)
#endif

#ifndef FAST_IO_FGETWC_UNLOCKED
#if defined(__GLIBC__) || defined(__LLVM_LIBC_TYPES_FILE_H__)
#define FAST_IO_FGETWC_UNLOCKED(...) ::fgetwc_unlocked(__VA_ARGS__)
#elif defined(fgetwc_unlocked)
#define FAST_IO_FGETWC_UNLOCKED(...) fgetwc_unlocked(__VA_ARGS__)
#else
#define FAST_IO_FGETWC_UNLOCKED(...) ::fast_io::noexcept_call(::fgetwc_unlocked, __VA_ARGS__)
#endif
#endif


#if defined(__GLIBC__) || defined(__LLVM_LIBC_TYPES_FILE_H__)
#define FAST_IO_FGETWC(...) ::fgetwc(__VA_ARGS__)
#elif defined(fgetwc_unlocked)
#define FAST_IO_FGETWC(...) fgetwc(__VA_ARGS__)
#else
#define FAST_IO_FGETWC(...) ::fast_io::noexcept_call(::fgetwc, __VA_ARGS__)
#endif

#if defined(__serenity__)
#define FAST_IO_PLATFORM_NOT_SUPPORTS_FGETWC 1
#elif defined(__BIONIC__)
#if defined(__BIONIC_AVAILABILITY_GUARD)
#if __BIONIC_AVAILABILITY_GUARD(28)
#define FAST_IO_PLATFORM_SUPPORTS_FGETWC_UNLOCKED 1
#endif
#endif
#elif defined(_WIN32) && !defined(__WINE__)
#if defined(_MSC_VER) || defined(_UCRT) || __MSVCRT_VERSION__ >= 0x800
#define FAST_IO_PLATFORM_SUPPORTS_FGETWC_UNLOCKED 1
#endif
#elif defined(__LLVM_LIBC_TYPES_FILE_H__)
#define FAST_IO_PLATFORM_SUPPORTS_FGETWC_UNLOCKED 1
#elif defined(__NEWLIB__)
#if __GNU_VISIBLE
#define FAST_IO_PLATFORM_SUPPORTS_FGETWC_UNLOCKED 1
#endif
#elif _POSIX_C_SOURCE >= 199309L || _XOPEN_SOURCE || _POSIX_SOURCE || _BSD_SOURCE || _SVID_SOURCE
#define FAST_IO_PLATFORM_SUPPORTS_FGETWC_UNLOCKED 1
#endif

#if defined(FAST_IO_PLATFORM_NOT_SUPPORTS_FGETWC)
#define FAST_IO_FGETWC_UNLOCKED_IMPL(...) EOF
#elif defined(FAST_IO_PLATFORM_SUPPORTS_FGETWC_UNLOCKED)
#define FAST_IO_FGETWC_UNLOCKED_IMPL(...) FAST_IO_FGETWC_UNLOCKED(__VA_ARGS__)
#else
#define FAST_IO_FGETWC_UNLOCKED_IMPL(...) FAST_IO_FGETWC(__VA_ARGS__)
#endif

		return FAST_IO_FGETWC_UNLOCKED_IMPL(fp);

#pragma pop_macro("FAST_IO_FGETWC_UNLOCKED_IMPL")
#pragma pop_macro("FAST_IO_PLATFORM_NOT_SUPPORTS_FGETWC")
#pragma pop_macro("FAST_IO_PLATFORM_SUPPORTS_FGETWC_UNLOCKED")
#pragma pop_macro("FAST_IO_FGETWC")
#pragma pop_macro("FAST_IO_FGETWC_UNLOCKED")
	}
}

template <::std::integral char_type, ::std::integral int_type>
	requires(::std::same_as<char_type, char> || ::std::same_as<char_type, char8_t> ||
			 ::std::same_as<char_type, wchar_t>)
inline constexpr bool equals_to_eof_macro(int_type inv) noexcept
{
	if constexpr (::std::same_as<char_type, char> || ::std::same_as<char_type, char8_t>)
	{
		return inv == EOF;
	}
	else
	{
		return inv == WEOF;
	}
}

template <::std::integral char_type>
inline auto ungetc_unlocked_impl(char_type ch, FILE *fp) noexcept
{


	if constexpr (::std::same_as<char_type, char> || ::std::same_as<char_type, char8_t>)
	{

#pragma push_macro("FAST_IO_UNGETC_UNLOCKED")
#pragma push_macro("FAST_IO_UNGETC")
#pragma push_macro("FAST_IO_PLATFORM_SUPPORTS_UNGETC_UNLOCKED")
#pragma push_macro("FAST_IO_UNGETC_UNLOCKED_IMPL")

#undef FAST_IO_UNGETC_UNLOCKED
#undef FAST_IO_UNGETC
#undef FAST_IO_PLATFORM_SUPPORTS_UNGETC_UNLOCKED
#undef FAST_IO_UNGETC_UNLOCKED_IMPL

#if (defined(_WIN32) && !defined(__WINE__))
#ifdef _ungetc_nolock
#define FAST_IO_UNGETC_UNLOCKED(...) _ungetc_nolock(__VA_ARGS__)
#else
#define FAST_IO_UNGETC_UNLOCKED(...) ::fast_io::noexcept_call(::_ungetc_nolock, __VA_ARGS__)
#endif
#endif

#ifndef FAST_IO_UNGETC_UNLOCKED
#if defined(__GLIBC__) || defined(__LLVM_LIBC_TYPES_FILE_H__)
#define FAST_IO_UNGETC_UNLOCKED(...) ::ungetc_unlocked(__VA_ARGS__)
#elif defined(ungetc_unlocked)
#define FAST_IO_UNGETC_UNLOCKED(...) ungetc_unlocked(__VA_ARGS__)
#else
#define FAST_IO_UNGETC_UNLOCKED(...) ::fast_io::noexcept_call(::ungetc_unlocked, __VA_ARGS__)
#endif
#endif


#if defined(__GLIBC__) || defined(__LLVM_LIBC_TYPES_FILE_H__)
#define FAST_IO_UNGETC(...) ::ungetc(__VA_ARGS__)
#elif defined(ungetc)
#define FAST_IO_UNGETC(...) ungetc(__VA_ARGS__)
#else
#define FAST_IO_UNGETC(...) ::fast_io::noexcept_call(::ungetc, __VA_ARGS__)
#endif

#if !defined(__BIONIC__) && defined(_MSC_VER)
#define FAST_IO_PLATFORM_SUPPORTS_UNGETC_UNLOCKED 1
#endif

#if defined(FAST_IO_PLATFORM_SUPPORTS_UNGETC_UNLOCKED)
#define FAST_IO_UNGETC_UNLOCKED_IMPL(...) FAST_IO_UNGETC_UNLOCKED(__VA_ARGS__)
#else
#define FAST_IO_UNGETC_UNLOCKED_IMPL(...) FAST_IO_UNGETC(__VA_ARGS__)
#endif

		return FAST_IO_UNGETC_UNLOCKED_IMPL(ch, fp);

#pragma pop_macro("FAST_IO_UNGETC_UNLOCKED_IMPL")
#pragma pop_macro("FAST_IO_PLATFORM_SUPPORTS_UNGETC_UNLOCKED")
#pragma pop_macro("FAST_IO_UNGETC")
#pragma pop_macro("FAST_IO_UNGETC_UNLOCKED")
	}
	else
	{
#pragma push_macro("FAST_IO_UNGETWC_UNLOCKED")
#pragma push_macro("FAST_IO_UNGETWC")
#pragma push_macro("FAST_IO_PLATFORM_SUPPORTS_UNGETWC_UNLOCKED")
#pragma push_macro("FAST_IO_PLATFORM_NOT_SUPPORTS_UNGETWC")
#pragma push_macro("FAST_IO_UNGETWC_UNLOCKED_IMPL")

#undef FAST_IO_UNGETWC_UNLOCKED
#undef FAST_IO_UNGETWC
#undef FAST_IO_PLATFORM_SUPPORTS_UNGETWC_UNLOCKED
#undef FAST_IO_PLATFORM_NOT_SUPPORTS_UNGETWC
#undef FAST_IO_UNGETWC_UNLOCKED_IMPL

#if (defined(_WIN32) && !defined(__WINE__))
#ifdef _ungetwc_nolock
#define FAST_IO_UNGETWC_UNLOCKED(...) ::_ungetwc_nolock(__VA_ARGS__)
#else
#define FAST_IO_UNGETWC_UNLOCKED(...) ::fast_io::noexcept_call(::_ungetwc_nolock, __VA_ARGS__)
#endif
#endif

#ifndef FAST_IO_UNGETWC_UNLOCKED
#if defined(__GLIBC__) || defined(__LLVM_LIBC_TYPES_FILE_H__)
#define FAST_IO_UNGETWC_UNLOCKED(...) ::ungetwc_unlocked(__VA_ARGS__)
#elif defined(ungetwc_unlocked)
#define FAST_IO_UNGETWC_UNLOCKED(...) ungetwc_unlocked(__VA_ARGS__)
#else
#define FAST_IO_UNGETWC_UNLOCKED(...) ::fast_io::noexcept_call(::ungetwc_unlocked, __VA_ARGS__)
#endif
#endif


#if defined(__GLIBC__) || defined(__LLVM_LIBC_TYPES_FILE_H__)
#define FAST_IO_UNGETWC(...) ::ungetwc(__VA_ARGS__)
#elif defined(ungetwc_unlocked)
#define FAST_IO_UNGETWC(...) ungetwc(__VA_ARGS__)
#else
#define FAST_IO_UNGETWC(...) ::fast_io::noexcept_call(::ungetwc, __VA_ARGS__)
#endif

#if !defined(__BIONIC__) && defined(_MSC_VER)
#define FAST_IO_PLATFORM_SUPPORTS_UNGETWC_UNLOCKED 1
#endif

#if defined(FAST_IO_PLATFORM_NOT_SUPPORTS_UNGETWC)
#define FAST_IO_UNGETWC_UNLOCKED_IMPL(...) WEOF
#elif defined(FAST_IO_PLATFORM_SUPPORTS_UNGETWC_UNLOCKED)
#define FAST_IO_UNGETWC_UNLOCKED_IMPL(...) FAST_IO_UNGETWC_UNLOCKED(__VA_ARGS__)
#else
#define FAST_IO_UNGETWC_UNLOCKED_IMPL(...) FAST_IO_UNGETWC(__VA_ARGS__)
#endif

		return FAST_IO_UNGETWC_UNLOCKED_IMPL(ch, fp);

#pragma pop_macro("FAST_IO_UNGETWC_UNLOCKED_IMPL")
#pragma pop_macro("FAST_IO_PLATFORM_NOT_SUPPORTS_UNGETWC")
#pragma pop_macro("FAST_IO_PLATFORM_SUPPORTS_UNGETWC_UNLOCKED")
#pragma pop_macro("FAST_IO_UNGETWC")
#pragma pop_macro("FAST_IO_UNGETWC_UNLOCKED")
	}
}
inline void ferror_throw_ex_impl(FILE *fp)
{
	if (
#if (defined(__NEWLIB__) && __GNU_VISIBLE) || (!defined(__NEWLIB__) && (_POSIX_C_SOURCE >= 199309L || _XOPEN_SOURCE || \
																		_POSIX_SOURCE || _BSD_SOURCE || _SVID_SOURCE))
		ferror_unlocked(fp)
#else
		ferror(fp)
#endif
	)
		throw_posix_error();
}
} // namespace details

template <::std::integral char_type>
	requires(::std::same_as<char_type, char>
#if !defined(__serenity__)
			 || ::std::same_as<char_type, wchar_t>
#endif
			 || ::std::same_as<char_type, char8_t>)
inline ::fast_io::try_get_result<char_type> try_get(::fast_io::basic_c_io_observer_unlocked<char_type> ciob)
{
	auto ret{::fast_io::details::fgetc_unlocked_impl<char_type>(ciob.fp)};
	if (::fast_io::details::equals_to_eof_macro<char_type>(ret))
	{
		::fast_io::details::ferror_throw_ex_impl(ciob.fp);
		return {0, false};
	}
	return {static_cast<char_type>(ret), true};
}

template <::std::integral char_type>
	requires(::std::same_as<char_type, char> || ::std::same_as<char_type, wchar_t> ||
			 ::std::same_as<char_type, char8_t>)
inline void try_unget(::fast_io::basic_c_io_observer_unlocked<char_type> ciob, char_type ch) noexcept
{
	::fast_io::details::ungetc_unlocked_impl<char_type>(ch, ciob.fp);
}

} // namespace fast_io
