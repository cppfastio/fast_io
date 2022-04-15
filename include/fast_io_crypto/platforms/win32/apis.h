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

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif __has_cpp_attribute(gnu::dllimport)
[[gnu::dllimport]]
#endif
extern ::std::uint_least32_t __stdcall BCryptGetProperty(void*,char16_t const*,void*,::std::uint_least32_t,::std::uint_least32_t*,::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("BCryptGetProperty@24")
#else
__asm__("_BCryptGetProperty@24")
#endif
#else
__asm__("BCryptGetProperty")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif __has_cpp_attribute(gnu::dllimport)
[[gnu::dllimport]]
#endif
extern ::std::uint_least32_t __stdcall BCryptCreateHash(void*,void**,void*,::std::uint_least32_t,void*,::std::uint_least32_t,::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("BCryptCreateHash@28")
#else
__asm__("_BCryptCreateHash@28")
#endif
#else
__asm__("BCryptCreateHash")
#endif
#endif
;


#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif __has_cpp_attribute(gnu::dllimport)
[[gnu::dllimport]]
#endif
extern ::std::uint_least32_t __stdcall BCryptHashData(void*,void const*,::std::uint_least32_t,::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("BCryptHashData@16")
#else
__asm__("_BCryptHashData@16")
#endif
#else
__asm__("BCryptHashData")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif __has_cpp_attribute(gnu::dllimport)
[[gnu::dllimport]]
#endif
extern ::std::uint_least32_t __stdcall BCryptFinishHash(void*,void*,::std::uint_least32_t,::std::uint_least32_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("BCryptFinishHash@16")
#else
__asm__("_BCryptFinishHash@16")
#endif
#else
__asm__("BCryptFinishHash")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif __has_cpp_attribute(gnu::dllimport)
[[gnu::dllimport]]
#endif
extern ::std::uint_least32_t __stdcall BCryptDestroyHash(void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("BCryptDestroyHash@4")
#else
__asm__("_BCryptDestroyHash@4")
#endif
#else
__asm__("BCryptDestroyHash")
#endif
#endif
;

}
