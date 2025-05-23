#pragma push_macro("erase")
#undef erase

#pragma push_macro("interface")
#undef interface

#pragma push_macro("max")
#undef max

#pragma push_macro("min")
#undef min

#pragma push_macro("move")
#undef move

#pragma push_macro("new")
#undef new

#pragma push_macro("refresh")
#undef refresh


#pragma push_macro("FAST_IO_DLLIMPORT")
#undef FAST_IO_DLLIMPORT
#if defined(_MSC_VER) && !defined(__clang__)
#define FAST_IO_DLLIMPORT __declspec(dllimport)
#elif __has_cpp_attribute(__gnu__::__dllimport__) && !defined(__WINE__) && !defined(__arm64ec__)
#define FAST_IO_DLLIMPORT [[__gnu__::__dllimport__]]
#else
#define FAST_IO_DLLIMPORT
#endif

#pragma push_macro("FAST_IO_DLL_DLLIMPORT")
#undef FAST_IO_DLL_DLLIMPORT
#if defined(_DLL) && !defined(__WINE__)
#define FAST_IO_DLL_DLLIMPORT FAST_IO_DLLIMPORT
#else
#define FAST_IO_DLL_DLLIMPORT
#endif

#pragma push_macro("FAST_IO_STDCALL")
#undef FAST_IO_STDCALL
#if defined(_MSC_VER) && (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
#define FAST_IO_STDCALL __stdcall
#elif (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
#define FAST_IO_STDCALL __attribute__((__stdcall__))
#else
#define FAST_IO_STDCALL
#endif

#pragma push_macro("FAST_IO_WINSTDCALL")
#undef FAST_IO_WINSTDCALL
#if defined(_MSC_VER) && (!__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
#define FAST_IO_WINSTDCALL __stdcall
#elif (__has_cpp_attribute(__gnu__::__stdcall__) && !defined(__WINE__))
#define FAST_IO_WINSTDCALL __attribute__((__stdcall__))
#else
#define FAST_IO_WINSTDCALL
#endif

#pragma push_macro("FAST_IO_WINSTDCALL_RENAME")
#undef FAST_IO_WINSTDCALL_RENAME
#if defined(__clang__) || defined(__GNUC__)
#if defined(_M_HYBRID)
#define FAST_IO_WINSTDCALL_RENAME(name, count) __asm__("#" #name "@" #count)
#elif defined(__arm64ec__)
#define FAST_IO_WINSTDCALL_RENAME(name, count) __asm__("#" #name)
#elif SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
#define FAST_IO_WINSTDCALL_RENAME(name, count) __asm__(#name "@" #count)
#else
#define FAST_IO_WINSTDCALL_RENAME(name, count) __asm__("_" #name "@" #count)
#endif
#else
#define FAST_IO_WINSTDCALL_RENAME(name, count) __asm__(#name)
#endif
#else
#define FAST_IO_WINSTDCALL_RENAME(name, count)
#endif

#pragma push_macro("FAST_IO_WINCDECL")
#undef FAST_IO_WINCDECL
#if defined(_MSC_VER) && (!__has_cpp_attribute(__gnu__::__cdecl__) && !defined(__WINE__))
#define FAST_IO_WINCDECL __cdecl
#elif (__has_cpp_attribute(__gnu__::__cdecl__) && !defined(__WINE__))
#define FAST_IO_WINCDECL __attribute__((__cdecl__))
#else
#define FAST_IO_WINCDECL
#endif

#pragma push_macro("FAST_IO_WINCDECL_RENAME")
#undef FAST_IO_WINCDECL_RENAME
#if defined(__clang__) || defined(__GNUC__)
#if defined(_M_HYBRID)
#define FAST_IO_WINCDECL_RENAME(name, count) __asm__("#" #name "@" #count)
#elif defined(__arm64ec__)
#define FAST_IO_WINCDECL_RENAME(name, count) __asm__("#" #name)
#elif SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
#define FAST_IO_WINCDECL_RENAME(name, count) __asm__(#name)
#else
#define FAST_IO_WINCDECL_RENAME(name, count) __asm__("_" #name)
#endif
#else
#define FAST_IO_WINCDECL_RENAME(name, count) __asm__(#name)
#endif
#else
#define FAST_IO_WINCDECL_RENAME(name, count)
#endif

#pragma push_macro("FAST_IO_WINFASTCALL")
#undef FAST_IO_WINFASTCALL
#if defined(_MSC_VER) && (!__has_cpp_attribute(__gnu__::__fastcall__) && !defined(__WINE__))
#define FAST_IO_WINFASTCALL __fastcall
#elif (__has_cpp_attribute(__gnu__::__fastcall__) && !defined(__WINE__))
#define FAST_IO_WINFASTCALL __attribute__((__fastcall__))
#else
#define FAST_IO_WINFASTCALL
#endif

#pragma push_macro("FAST_IO_WINFASTCALL_RENAME")
#undef FAST_IO_WINFASTCALL_RENAME
#if defined(__clang__) || defined(__GNUC__)
#if defined(_M_HYBRID)
#define FAST_IO_WINFASTCALL_RENAME(name, count) __asm__("#" #name "@" #count)
#elif defined(__arm64ec__)
#define FAST_IO_WINFASTCALL_RENAME(name, count) __asm__("#" #name)
#elif SIZE_MAX <= UINT_LEAST32_MAX && (defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
#define FAST_IO_WINFASTCALL_RENAME(name, count) __asm__("@" #name "@" #count)
#else
#define FAST_IO_WINFASTCALL_RENAME(name, count) __asm__("_@" #name "@" #count)
#endif
#else
#define FAST_IO_WINFASTCALL_RENAME(name, count) __asm__(#name)
#endif
#else
#define FAST_IO_WINFASTCALL_RENAME(name, count)
#endif

#pragma push_macro("FAST_IO_GNU_CONST")
#undef FAST_IO_GNU_CONST
#if __has_cpp_attribute(__gnu__::__const__)
#define FAST_IO_GNU_CONST [[__gnu__::__const__]]
#else
#define FAST_IO_GNU_CONST
#endif

#pragma push_macro("FAST_IO_GNU_ALWAYS_INLINE")
#undef FAST_IO_GNU_ALWAYS_INLINE
#if __has_cpp_attribute(__gnu__::__always_inline__)
#define FAST_IO_GNU_ALWAYS_INLINE [[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
#define FAST_IO_GNU_ALWAYS_INLINE [[msvc::forceinline]]
#else
#define FAST_IO_GNU_ALWAYS_INLINE
#endif

#pragma push_macro("FAST_IO_GNU_ARTIFICIAL")
#undef FAST_IO_GNU_ARTIFICIAL
#if __has_cpp_attribute(__gnu__::__artificial__)
#define FAST_IO_GNU_ARTIFICIAL [[__gnu__::__artificial__]]
#else
#define FAST_IO_GNU_ARTIFICIAL
#endif


#pragma push_macro("FAST_IO_GNU_ALWAYS_INLINE_ARTIFICIAL")
#undef FAST_IO_GNU_ALWAYS_INLINE_ARTIFICIAL
#define FAST_IO_GNU_ALWAYS_INLINE_ARTIFICIAL FAST_IO_GNU_ALWAYS_INLINE FAST_IO_GNU_ARTIFICIAL

#pragma push_macro("FAST_IO_GNU_ALWAYS_INLINE_ARTIFICIAL_CONST")
#undef FAST_IO_GNU_ALWAYS_INLINE_ARTIFICIAL_CONST
#define FAST_IO_GNU_ALWAYS_INLINE_ARTIFICIAL_CONST \
	FAST_IO_GNU_ALWAYS_INLINE [[nodiscard]] FAST_IO_GNU_ARTIFICIAL FAST_IO_GNU_CONST

#pragma push_macro("FAST_IO_GNU_MALLOC")
#undef FAST_IO_GNU_MALLOC
#if __has_cpp_attribute(__gnu__::__malloc__)
#define FAST_IO_GNU_MALLOC [[__gnu__::__malloc__]]
#else
#define FAST_IO_GNU_MALLOC
#endif

#pragma push_macro("FAST_IO_GNU_RETURNS_NONNULL")
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
#undef FAST_IO_GNU_RETURNS_NONNULL
#define FAST_IO_GNU_RETURNS_NONNULL [[__gnu__::__returns_nonnull__]]
#else
#define FAST_IO_GNU_RETURNS_NONNULL
#endif

#pragma push_macro("FAST_IO_ASSERT")
#undef FAST_IO_ASSERT
/*
Internal assert macros for fuzzing fast_io.
*/
#if defined(FAST_IO_DEBUG)
#if defined(_MSC_VER) && !defined(__clang__)
#define FAST_IO_ASSERT(x)                           \
	if (!__builtin_is_constant_evaluated() && !(x)) \
	::std::abort()
#else
#define FAST_IO_ASSERT(x)                           \
	if (!__builtin_is_constant_evaluated() && !(x)) \
	__builtin_trap()
#endif
#else
#define FAST_IO_ASSERT(x) ((void)0)
#endif

#pragma push_macro("FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE")
#if defined(__cpp_trivial_relocatability)
#undef FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
#define FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE trivially_relocatable_if_eligible
#else
#define FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
#endif

#pragma push_macro("FAST_IO_HAS_BUILTIN")
#undef FAST_IO_HAS_BUILTIN
#ifdef __has_builtin
# define FAST_IO_HAS_BUILTIN(...) __has_builtin(__VA_ARGS__)
#else
# define FAST_IO_HAS_BUILTIN(...) 0
#endif
