#pragma once

#include"std_filesystem_pseudo.h"
#include"nt.h"
#include"posix.h"

#if defined(_WIN32) || defined(__CYGWIN__)
namespace fast_io
{
enum class win32_family
{
ansi_9x,
wide_nt,
#ifdef _WIN32_WINDOWS
native = ansi_9x
#else
native = wide_nt
#endif
};

#ifdef _WIN32_WINDOWS
using win32_api_encoding_converter = posix_api_encoding_converter;
#else
using win32_api_encoding_converter = nt_api_encoding_converter;
#endif

template<win32_family family>
using win32_family_api_encoding_converter=std::conditional_t<family==win32_family::ansi_9x,posix_api_encoding_converter,nt_api_encoding_converter>;
#if defined(__CYGWIN__) || defined(__WINE__)
using native_char_type = char;
#else
using native_char_type = std::conditional_t<win32_family::native==win32_family::ansi_9x,char,wchar_t>;
#endif

}
#include"win32.h"
#else
namespace fast_io
{
using native_char_type = char;
}
#endif
