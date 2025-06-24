#pragma once

namespace fast_io
{

namespace win32
{
struct memory_basic_information
{
	void *BaseAddress;
	void *AllocationBase;
	::std::uint_least32_t AllocationProtect;
#if UINT_LEAST64_MAX <= SIZE_MAX
	::std::uint_least16_t PartitionId;
#endif
	::std::size_t RegionSize;
	::std::uint_least32_t State;
	::std::uint_least32_t Protect;
	::std::uint_least32_t Type;
};

FAST_IO_DLLIMPORT FAST_IO_GNU_MALLOC extern void *FAST_IO_WINSTDCALL HeapAlloc(void *, ::std::uint_least32_t, ::std::size_t) noexcept FAST_IO_WINSTDCALL_RENAME(HeapAlloc, 12);
FAST_IO_DLLIMPORT extern int FAST_IO_WINSTDCALL HeapFree(void *, ::std::uint_least32_t, void *) noexcept FAST_IO_WINSTDCALL_RENAME(HeapFree, 12);
FAST_IO_DLLIMPORT FAST_IO_GNU_CONST extern void *FAST_IO_WINSTDCALL GetProcessHeap() noexcept FAST_IO_WINSTDCALL_RENAME(GetProcessHeap, 0);
FAST_IO_DLLIMPORT extern void *FAST_IO_WINSTDCALL HeapReAlloc(void *, ::std::uint_least32_t, void *, ::std::size_t) noexcept FAST_IO_WINSTDCALL_RENAME(HeapReAlloc, 16);
FAST_IO_DLLIMPORT extern ::std::size_t FAST_IO_WINSTDCALL HeapSize(void *, ::std::uint_least32_t, void const *) noexcept FAST_IO_WINSTDCALL_RENAME(HeapSize, 12);
FAST_IO_DLLIMPORT FAST_IO_GNU_MALLOC extern void *FAST_IO_WINSTDCALL VirtualAlloc(void *, ::std::size_t, ::std::uint_least32_t, ::std::uint_least32_t) noexcept FAST_IO_WINSTDCALL_RENAME(VirtualAlloc, 16);
FAST_IO_DLLIMPORT extern int FAST_IO_WINSTDCALL VirtualProtect(void *, ::std::size_t, ::std::uint_least32_t, ::std::uint_least32_t *) noexcept FAST_IO_WINSTDCALL_RENAME(VirtualProtect, 16);
FAST_IO_DLLIMPORT extern int FAST_IO_WINSTDCALL VirtualFree(void *, ::std::size_t, ::std::uint_least32_t) noexcept FAST_IO_WINSTDCALL_RENAME(VirtualFree, 12);
FAST_IO_DLLIMPORT extern int FAST_IO_WINSTDCALL VirtualQuery(void const *, memory_basic_information *, ::std::size_t) noexcept FAST_IO_WINSTDCALL_RENAME(VirtualQuery, 12);
} // namespace win32

namespace details
{
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
inline void *win32_heapalloc_handle_common_impl(void *heaphandle, ::std::size_t n, ::std::uint_least32_t flag) noexcept
{
	if (n == 0)
	{
		n = 1;
	}
	auto p{::fast_io::win32::HeapAlloc(heaphandle, flag, n)};
	if (p == nullptr)
	{
		::fast_io::fast_terminate();
	}
	return p;
}

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
inline void *win32_heaprealloc_handle_common_impl(void *heaphandle, void *addr, ::std::size_t n, ::std::uint_least32_t flag) noexcept
{
	if (n == 0)
	{
		n = 1;
	}
	if (addr == nullptr)
#if __has_cpp_attribute(unlikely)
		[[unlikely]]
#endif
	{
		return win32_heapalloc_handle_common_impl(heaphandle, n, flag);
	}
	auto p{::fast_io::win32::HeapReAlloc(heaphandle, flag, addr, n)};
	if (p == nullptr)
	{
		::fast_io::fast_terminate();
	}
	return p;
}

#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
[[nodiscard]]
#if __has_cpp_attribute(__gnu__::__artificial__)
[[__gnu__::__artificial__]]
#endif
#if __has_cpp_attribute(__gnu__::__const__)
[[__gnu__::__const__]]
#endif
inline void *win32_get_process_heap() noexcept
{
	constexpr bool intrinsicssupported{
#if (defined(__GNUC__) || defined(__clang__) || defined(_MSC_VER)) &&                     \
	(defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_AMD64) || \
	 defined(__aarch64__) || defined(__arm64ec__) || defined(_M_ARM64) || defined(_M_ARM64EC))
		true
#endif
	};
	if constexpr (intrinsicssupported)
	{
		return ::fast_io::win32::nt::rtl_get_process_heap();
	}
	else
	{
		return ::fast_io::win32::GetProcessHeap();
	}
}

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
inline void *win32_heapalloc_common_impl(::std::size_t n, ::std::uint_least32_t flag) noexcept
{
	return ::fast_io::details::win32_heapalloc_handle_common_impl(::fast_io::details::win32_get_process_heap(), n, flag);
}

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
inline void *win32_heaprealloc_common_impl(void *addr, ::std::size_t n, ::std::uint_least32_t flag) noexcept
{
	return ::fast_io::details::win32_heaprealloc_handle_common_impl(::fast_io::details::win32_get_process_heap(), addr, n, flag);
}

inline ::fast_io::allocation_least_result win32_heapalloc_least_common_impl(::std::size_t n, ::std::uint_least32_t flag) noexcept
{
	auto processheap{::fast_io::details::win32_get_process_heap()};
	auto ptr{::fast_io::details::win32_heapalloc_handle_common_impl(processheap, n, flag)};
	return {ptr, ::fast_io::win32::HeapSize(processheap, 0, ptr)};
}

inline ::fast_io::allocation_least_result win32_heaprealloc_least_common_impl(void *addr, ::std::size_t n, ::std::uint_least32_t flag) noexcept
{
	auto processheap{::fast_io::details::win32_get_process_heap()};
	auto ptr{::fast_io::details::win32_heaprealloc_handle_common_impl(processheap, addr, n, flag)};
	return {ptr, ::fast_io::win32::HeapSize(processheap, 0, ptr)};
}

} // namespace details

class win32_heapalloc_allocator
{
public:
#if __has_cpp_attribute(__gnu__::__malloc__)
	[[__gnu__::__malloc__]]
#endif
	static inline void *allocate(::std::size_t n) noexcept
	{
		return ::fast_io::details::win32_heapalloc_common_impl(n, 0u);
	}

#if __has_cpp_attribute(__gnu__::__malloc__)
	[[__gnu__::__malloc__]]
#endif
	static inline void *allocate_zero(::std::size_t n) noexcept
	{
		return ::fast_io::details::win32_heapalloc_common_impl(n, 0x00000008u);
	}
	static inline void *reallocate(void *addr, ::std::size_t n) noexcept
	{
		return ::fast_io::details::win32_heaprealloc_common_impl(addr, n, 0u);
	}
	static inline void *reallocate_zero(void *addr, ::std::size_t n) noexcept
	{
		return ::fast_io::details::win32_heaprealloc_common_impl(addr, n, 0x00000008u);
	}
	static inline void deallocate(void *addr) noexcept
	{
		if (addr == nullptr) [[unlikely]]
		{
			return;
		}
		::fast_io::win32::HeapFree(::fast_io::details::win32_get_process_heap(), 0u, addr);
	}
#if 0
	static inline ::fast_io::allocation_least_result allocate_at_least(::std::size_t n) noexcept
	{
		return ::fast_io::details::win32_heapalloc_least_common_impl(n, 0u);
	}
	static inline ::fast_io::allocation_least_result allocate_zero_at_least(::std::size_t n) noexcept
	{
		return ::fast_io::details::win32_heapalloc_least_common_impl(n, 0x00000008u);
	}
	static inline ::fast_io::allocation_least_result reallocate_at_least(void *addr, ::std::size_t n) noexcept
	{
		return ::fast_io::details::win32_heaprealloc_least_common_impl(addr, n, 0u);
	}
	static inline ::fast_io::allocation_least_result reallocate_zero_at_least(void *addr, ::std::size_t n) noexcept
	{
		return ::fast_io::details::win32_heaprealloc_least_common_impl(addr, n, 0x00000008u);
	}
#endif
};

} // namespace fast_io
