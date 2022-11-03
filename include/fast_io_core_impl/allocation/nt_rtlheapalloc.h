#pragma once

#if defined(_MSC_VER) && !defined(_KERNEL_MODE) && !defined(_WIN32_WINDOWS)
#pragma comment(lib,"ntdll.lib")
#endif

namespace fast_io
{

namespace nt
{
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
RtlAllocateHeap(void*,::std::uint_least32_t,::std::size_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RtlAllocateHeap@12")
#else
__asm__("_RtlAllocateHeap@12")
#endif
#else
__asm__("RtlAllocateHeap")
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
extern char unsigned
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
RtlFreeHeap(void*,::std::uint_least32_t,void*) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RtlFreeHeap@12")
#else
__asm__("_RtlFreeHeap@12")
#endif
#else
__asm__("RtlFreeHeap")
#endif
#endif
;

struct peb_ldr_data;
struct rtl_user_process_parameters;
using pps_post_process_init_routine = void (
#if defined(_MSC_VER) && (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
__stdcall
#elif (__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__))
__attribute__((__stdcall__))
#endif
*)(void)noexcept;

struct peb
{
char unsigned InheritedAddressSpace;
char unsigned ReadImageFileExecOptions;
char unsigned BeingDebugged;
char unsigned SpareBool;
void* Mutant;
void* ImageBaseAddress;
peb_ldr_data* Ldr;
rtl_user_process_parameters* ProcessParameters;//PRTL_USER_PROCESS_PARAMETERS
void* SubSystemData;
void* ProcessHeap;
void* FastPebLock;
void* FastPebLockRoutine;
void* FastPebUnlockRoutine;
::std::uint_least32_t Reserved6;
void* Reserved7;
::std::uint_least32_t Reserved8;
::std::uint_least32_t AtlThunkSListPtr32;
void* Reserved9[45];
char unsigned Reserved10[96];
pps_post_process_init_routine PostProcessInitRoutine;
char unsigned Reserved11[128];
void* Reserved12[1];
::std::uint_least32_t SessionId;
};

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
extern peb*
#if (!__has_cpp_attribute(__gnu__::__stdcall__)&&!defined(__WINE__)) && defined(_MSC_VER)
__stdcall
#endif
RtlGetCurrentPeb() noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RtlGetCurrentPeb@0")
#else
__asm__("_RtlGetCurrentPeb@0")
#endif
#else
__asm__("RtlGetCurrentPeb")
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
RtlReAllocateHeap(void*,::std::uint_least32_t,void*,::std::size_t) noexcept
#if defined(__clang__) || defined(__GNUC__)
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
#if !defined(__clang__)
__asm__("RtlReAllocateHeap@16")
#else
__asm__("_RtlReAllocateHeap@16")
#endif
#else
__asm__("RtlReAllocateHeap")
#endif
#endif
;

#if __has_cpp_attribute(__gnu__::__const__)
[[__gnu__::__const__]]
#endif
inline void* rtl_get_process_heap() noexcept
{
	return ::fast_io::nt::RtlGetCurrentPeb()->ProcessHeap;
}

}

namespace details
{

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
inline void* nt_rtlallocate_heap_common_impl(::std::size_t to_allocate,::std::uint_least32_t flag) noexcept
{
	if(to_allocate==0)
	{
		to_allocate=1;
	}
	auto p{::fast_io::nt::RtlAllocateHeap(
		::fast_io::nt::rtl_get_process_heap(),
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
inline void* nt_rtlreallocate_heap_common_impl(void* addr,::std::size_t n,::std::uint_least32_t flag) noexcept
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
		return nt_rtlallocate_heap_common_impl(n,flag);
	}
	auto p{::fast_io::nt::RtlReAllocateHeap(
		::fast_io::nt::rtl_get_process_heap(),
		flag,addr,n)};
	if(p==nullptr)
	{
		::fast_io::fast_terminate();
	}
	return p;
}
}

class nt_rtlallocateheap_allocator
{
public:
#if __has_cpp_attribute(__gnu__::__malloc__)
[[__gnu__::__malloc__]]
#endif
	static inline void* allocate(::std::size_t n) noexcept
	{
		return ::fast_io::details::nt_rtlallocate_heap_common_impl(n,0u);
	}

#if __has_cpp_attribute(__gnu__::__malloc__)
[[__gnu__::__malloc__]]
#endif
	static inline void* allocate_zero(::std::size_t n) noexcept
	{
		return ::fast_io::details::nt_rtlallocate_heap_common_impl(n,0x00000008u);
	}
	static inline void* reallocate(void* addr,::std::size_t n) noexcept
	{
		return ::fast_io::details::nt_rtlreallocate_heap_common_impl(addr,n,0u);
	}
	static inline void* reallocate_zero(void* addr,::std::size_t n) noexcept
	{
		return ::fast_io::details::nt_rtlreallocate_heap_common_impl(addr,n,0x00000008u);
	}
	static inline void deallocate(void* addr) noexcept
	{
		if(addr==nullptr)
			return;
		::fast_io::nt::RtlFreeHeap(
			::fast_io::nt::rtl_get_process_heap(),
			0u,addr);
	}
};

}
