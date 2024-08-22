#pragma once

/*
Referenced from
https://github.com/google/sanitizers/wiki/AddressSanitizerManualPoisoning
*/

namespace fast_io::asan
{

enum class asan_status
{
	none,
	activate,
#if defined(__SANITIZE_ADDRESS__)
	current = activate
#elif defined(__has_feature)
#if __has_feature(address_sanitizer)
	current = activate
#else
	current = none
#endif
#else
	current = none
#endif
};

#if 0
#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__cdecl__) && !defined(__WINE__))
[[__gnu__::__cdecl__]]
#endif
extern void
#if (!__has_cpp_attribute(__gnu__::__cdecl__) && !defined(__WINE__)) && defined(_MSC_VER)
	__cdecl
#endif
	__asan_poison_memory_region(void const volatile *, ::std::size_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
	__asm__("__asan_poison_memory_region")
#endif
		;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__cdecl__) && !defined(__WINE__))
[[__gnu__::__cdecl__]]
#endif
extern void
#if (!__has_cpp_attribute(__gnu__::__cdecl__) && !defined(__WINE__)) && defined(_MSC_VER)
	__cdecl
#endif
	__asan_unpoison_memory_region(void const volatile *, ::std::size_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
	__asm__("__asan_unpoison_memory_region")
#endif
		;
#endif
} // namespace fast_io::asan
