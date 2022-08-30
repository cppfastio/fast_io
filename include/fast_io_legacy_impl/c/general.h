#pragma once
#if __has_include(<cwchar>)
#include<cwchar>
#elif __has_include(<wchar.h>)
#include<wchar.h>
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
template<std::integral char_type>
inline auto fgetc_unlocked_impl(FILE* fp) noexcept
{
	if constexpr(std::same_as<char_type,char>||std::same_as<char_type,char8_t>)
	{
#if (defined(_WIN32)&&!defined(__WINE__))
		return _fgetc_nolock(fp);
#elif (defined(__NEWLIB__) && __GNU_VISIBLE) || (!defined(__NEWLIB__)&&(_POSIX_C_SOURCE >= 199309L || _XOPEN_SOURCE || _POSIX_SOURCE || _BSD_SOURCE || _SVID_SOURCE))
		return fgetc_unlocked(fp);
#else
		return fgetc(fp);
#endif
	}
	else
	{
#if defined(_MSC_VER) || defined(_UCRT)  || __MSVCRT_VERSION__ >= 0x800
		return _fgetwc_nolock(fp);
#elif (defined(__NEWLIB__) && __GNU_VISIBLE) || (!defined(__NEWLIB__)&&(_POSIX_C_SOURCE >= 199309L || _XOPEN_SOURCE || _POSIX_SOURCE || _BSD_SOURCE || _SVID_SOURCE))
		return fgetwc_unlocked(fp);
#elif defined(__serenity__)
		return EOF;
#else
		return fgetwc(fp);
#endif
	}
}

template<std::integral char_type,std::integral int_type>
requires (std::same_as<char_type,char>||std::same_as<char_type,char8_t>||std::same_as<char_type,wchar_t>)
inline constexpr bool equals_to_eof_macro(int_type inv) noexcept
{
	if constexpr(std::same_as<char_type,char>||std::same_as<char_type,char8_t>)
		return inv==EOF;
	else
		return inv==WEOF;
}

template<std::integral char_type>
inline auto ungetc_unlocked_impl(char_type ch,FILE* fp) noexcept
{
	if constexpr(std::same_as<char_type,char>||std::same_as<char_type,char8_t>)
	{
#if defined(_MSC_VER)
		return _ungetc_nolock(ch,fp);
#elif (defined(__NEWLIB__) && __GNU_VISIBLE) || (!defined(__NEWLIB__)&&(_POSIX_C_SOURCE >= 199309L || _XOPEN_SOURCE || _POSIX_SOURCE || _BSD_SOURCE || _SVID_SOURCE))
		return ungetc_unlocked(ch,fp);
#else
		return ungetc(ch,fp);
#endif
	}
	else
	{
#if defined(_MSC_VER)
		return _ungetwc_nolock(ch,fp);
#elif (defined(__NEWLIB__) && __GNU_VISIBLE) || (!defined(__NEWLIB__)&&(_POSIX_C_SOURCE >= 199309L || _XOPEN_SOURCE || _POSIX_SOURCE || _BSD_SOURCE || _SVID_SOURCE))
		return ungetwc_unlocked(ch,fp);
#else
		return ungetwc(ch,fp);
#endif
	}
}
inline void ferror_throw_ex_impl(FILE* fp)
{
	if(
#if (defined(__NEWLIB__) && __GNU_VISIBLE) || (!defined(__NEWLIB__)&&(_POSIX_C_SOURCE >= 199309L || _XOPEN_SOURCE || _POSIX_SOURCE || _BSD_SOURCE || _SVID_SOURCE))
	ferror_unlocked(fp)
#else
	ferror(fp)
#endif
	)
		throw_posix_error();
}
}

template<std::integral char_type>
requires (std::same_as<char_type,char>
#if !defined(__serenity__)
||std::same_as<char_type,wchar_t>
#endif
||std::same_as<char_type,char8_t>)
inline std::pair<char_type,bool> try_get(basic_c_io_observer_unlocked<char_type> ciob)
{
	auto ret{details::fgetc_unlocked_impl<char_type>(ciob.fp)};
	if(details::equals_to_eof_macro<char_type>(ret))
	{
		details::ferror_throw_ex_impl(ciob.fp);
		return {0,false};
	}
	return {static_cast<char_type>(ret),true};
}

template<std::integral char_type>
requires (std::same_as<char_type,char>||std::same_as<char_type,wchar_t>||std::same_as<char_type,char8_t>)
inline void try_unget(basic_c_io_observer_unlocked<char_type> ciob,char_type ch) noexcept
{
	details::ungetc_unlocked_impl<char_type>(ch,ciob.fp);
}

}
