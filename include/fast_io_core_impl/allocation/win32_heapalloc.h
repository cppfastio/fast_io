#pragma once

namespace fast_io
{

namespace win32
{
#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
#if __has_cpp_attribute(__gnu__::__malloc__)
[[__gnu__::__malloc__]]
#endif
extern void*
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
HeapAlloc(void*,::std::uint_least32_t,::std::size_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("HeapAlloc@12")
#else
__asm__("_HeapAlloc@12")
#endif
#else
__asm__("HeapAlloc")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern int
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
HeapFree(void*,::std::uint_least32_t,void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("HeapFree@12")
#else
__asm__("_HeapFree@12")
#endif
#else
__asm__("HeapFree")
#endif
#endif
;

#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
#if __has_cpp_attribute(__gnu__::__const__)
[[__gnu__::__const__]]
#endif
extern void*
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
GetProcessHeap() noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("GetProcessHeap@0")
#else
__asm__("_GetProcessHeap@0")
#endif
#else
__asm__("GetProcessHeap")
#endif
#endif

;
#if defined(_MSC_VER) && !defined(__clang__)
__declspec(dllimport)
#elif (__has_cpp_attribute(__gnu__::__dllimport__)&&!defined(__WINE__))
[[__gnu__::__dllimport__]]
#endif
#if (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
[[__gnu__::__stdcall__]]
#endif
extern void*
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
HeapReAlloc(void*,::std::uint_least32_t,void*,::std::size_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("HeapReAlloc@16")
#else
__asm__("_HeapReAlloc@16")
#endif
#else
__asm__("HeapReAlloc")
#endif
#endif
;

}

namespace details
{
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
inline void* win32_heapalloc_common_impl(::std::size_t to_allocate,::std::uint_least32_t flag) noexcept
{
	if(to_allocate==0)
	{
		to_allocate=1;
	}
	auto p{::fast_io::win32::HeapAlloc(
		::fast_io::win32::GetProcessHeap(),
		flag,to_allocate)};
	if(p==nullptr)
	{
		::fast_io::fast_terminate();
	}
	return p;
}

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
inline void* win32_heaprealloc_common_impl(void* addr,::std::size_t n,::std::uint_least32_t flag) noexcept
{
		if(n==0)
		{
			n=1;
		}
		if(addr==nullptr)
	#if __has_cpp_attribute(unlikely)
		[[unlikely]]
	#endif
		{
			return win32_heapalloc_common_impl(n,flag);
		}
		auto p{::fast_io::win32::HeapReAlloc(
			::fast_io::win32::GetProcessHeap(),
			flag,addr,n)};
		if(p==nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
}

}

class win32_heapalloc_allocator
{
public:
#if __has_cpp_attribute(__gnu__::__malloc__)
[[__gnu__::__malloc__]]
#endif
	static inline void* allocate(::std::size_t n) noexcept
	{
		return ::fast_io::details::win32_heapalloc_common_impl(n,0u);
	}

#if __has_cpp_attribute(__gnu__::__malloc__)
[[__gnu__::__malloc__]]
#endif
	static inline void* allocate_zero(::std::size_t n) noexcept
	{
		return ::fast_io::details::win32_heapalloc_common_impl(n,0x00000008u);
	}
	static inline void* reallocate(void* addr,::std::size_t n) noexcept
	{
		return ::fast_io::details::win32_heaprealloc_common_impl(addr,n,0u);
	}
	static inline void* reallocate_zero(void* addr,::std::size_t n) noexcept
	{
		return ::fast_io::details::win32_heaprealloc_common_impl(addr,n,0x00000008u);
	}
	static inline void deallocate(void* addr) noexcept
	{
		if(addr==nullptr)
			return;
		::fast_io::win32::HeapFree(
			::fast_io::win32::GetProcessHeap(),
			0u,addr);
	}
};

}
