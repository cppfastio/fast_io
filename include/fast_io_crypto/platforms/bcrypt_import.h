#pragma once

namespace fast_io::win32
{

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif __has_cpp_attribute(gnu::dllimport)
[[gnu::dllimport]]
#endif
extern ::std::uint_least32_t __stdcall BCryptOpenAlgorithmProvider(void**,char16_t const*,char16_t const*,::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("BCryptOpenAlgorithmProvider@16")
#else
__asm__("_BCryptOpenAlgorithmProvider@16")
#endif
#else
__asm__("BCryptOpenAlgorithmProvider")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif __has_cpp_attribute(gnu::dllimport)
[[gnu::dllimport]]
#endif
extern ::std::uint_least32_t __stdcall BCryptCloseAlgorithmProvider(void*,::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("BCryptCloseAlgorithmProvider@8")
#else
__asm__("_BCryptCloseAlgorithmProvider@8")
#endif
#else
__asm__("BCryptCloseAlgorithmProvider")
#endif
#endif
;

}
