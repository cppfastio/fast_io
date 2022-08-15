#pragma once

namespace fast_io
{
namespace mimalloc
{

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif __has_cpp_attribute(__gnu__::__dllimport__)
[[__gnu__::__dllimport__]]
#endif
#if __has_cpp_attribute(__gnu__::__malloc__)
[[__gnu__::__malloc__]]
#endif
extern void* __cdecl mi_malloc(::std::size_t size) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("mi_malloc")
#else
__asm__("_mi_malloc")
#endif
#else
__asm__("mi_malloc")
#endif
#endif
;
#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif __has_cpp_attribute(__gnu__::__dllimport__)
[[__gnu__::__dllimport__]]
#endif
extern void __cdecl mi_free(void* p) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("mi_free")
#else
__asm__("_mi_free")
#endif
#else
__asm__("mi_free")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif __has_cpp_attribute(__gnu__::__dllimport__)
[[__gnu__::__dllimport__]]
#endif
extern void* __cdecl mi_calloc(::std::size_t count,::std::size_t size) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("mi_calloc")
#else
__asm__("_mi_calloc")
#endif
#else
__asm__("mi_calloc")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif __has_cpp_attribute(__gnu__::__dllimport__)
[[__gnu__::__dllimport__]]
#endif
extern void* __cdecl mi_realloc(void* p, ::std::size_t newsize) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("mi_realloc")
#else
__asm__("_mi_realloc")
#endif
#else
__asm__("mi_realloc")
#endif
#endif
;

}

class mimalloc_allocator
{
public:
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
	static inline void* allocate(::std::size_t n) noexcept
	{
		void* p = ::fast_io::mimalloc::mi_malloc(n);
		if(p==nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
	static inline void* reallocate(void* p,::std::size_t n) noexcept
	{
		void* p = ::fast_io::mimalloc::mi_realloc(p,n);
		if(p==nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
	static inline void* allocate_zero(::std::size_t n) noexcept
	{
		void* p = ::fast_io::mimalloc::mi_calloc(1,n);
		if(p==nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
	static inline void deallocate(void* p) noexcept
	{
		::fast_io::mimalloc::mi_free(p);
	}
};

}