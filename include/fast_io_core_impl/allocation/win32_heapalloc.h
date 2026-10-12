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
FAST_IO_DLLIMPORT FAST_IO_GNU_MALLOC extern void *FAST_IO_WINSTDCALL VirtualAlloc(void *, ::std::size_t, ::std::uint_least32_t, ::std::uint_least32_t) noexcept FAST_IO_WINSTDCALL_RENAME(VirtualAlloc, 16);
FAST_IO_DLLIMPORT extern int FAST_IO_WINSTDCALL VirtualProtect(void *, ::std::size_t, ::std::uint_least32_t, ::std::uint_least32_t *) noexcept FAST_IO_WINSTDCALL_RENAME(VirtualProtect, 16);
FAST_IO_DLLIMPORT extern int FAST_IO_WINSTDCALL VirtualFree(void *, ::std::size_t, ::std::uint_least32_t) noexcept FAST_IO_WINSTDCALL_RENAME(VirtualFree, 12);
FAST_IO_DLLIMPORT extern int FAST_IO_WINSTDCALL VirtualQuery(void const *, memory_basic_information *, ::std::size_t) noexcept FAST_IO_WINSTDCALL_RENAME(VirtualQuery, 12);
} // namespace win32

namespace details
{
inline void *win32_heapalloc_handle_common_impl(void *heaphandle, ::std::size_t n, ::std::uint_least32_t flag) noexcept
{
	if (n == 0)
	{
		n = 1;
	}
	return ::fast_io::win32::HeapAlloc(heaphandle, flag, n);
}

inline void *win32_heaprealloc_handle_common_impl(void *heaphandle, void *addr, ::std::size_t n, ::std::uint_least32_t flag) noexcept
{
	if (n == 0)
	{
		n = 1;
	}
	if (addr == nullptr)
		[[unlikely]]
	{
		return win32_heapalloc_handle_common_impl(heaphandle, n, flag);
	}
	return ::fast_io::win32::HeapReAlloc(heaphandle, flag, addr, n);
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

inline void *win32_heapalloc_common_impl(::std::size_t n, ::std::uint_least32_t flag) noexcept
{
	return ::fast_io::details::win32_heapalloc_handle_common_impl(::fast_io::details::win32_get_process_heap(), n, flag);
}

inline void *win32_heaprealloc_common_impl(void *addr, ::std::size_t n, ::std::uint_least32_t flag) noexcept
{
	return ::fast_io::details::win32_heaprealloc_handle_common_impl(::fast_io::details::win32_get_process_heap(), addr, n, flag);
}

} // namespace details

class win32_heapalloc_allocator
{
public:
#if __has_cpp_attribute(__gnu__::__malloc__)
	[[__gnu__::__malloc__]]
#endif
	static inline void *allocate_conditional_zero_die(::std::size_t n, bool zeroing) noexcept
	{
		auto p{::fast_io::details::win32_heapalloc_common_impl(n, zeroing ? 0x00000008u : 0u)};
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
#if __has_cpp_attribute(__gnu__::__malloc__)
	[[__gnu__::__malloc__]]
#endif
	static inline void *allocate_conditional_zero_try(::std::size_t n, bool zeroing) FAST_IO_HERBCEPTIONS_THROWS
	{
		auto p{::fast_io::details::win32_heapalloc_common_impl(n, zeroing ? 0x00000008u : 0u)};
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_win32_errc_with_value(8u);
		}
		return p;
	}

	static inline void *reallocate_die(void *addr, ::std::size_t n) noexcept
	{
		auto p{::fast_io::details::win32_heaprealloc_common_impl(addr, n, 0u)};
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
	static inline void *reallocate_try(void *addr, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		auto p{::fast_io::details::win32_heaprealloc_common_impl(addr, n, 0u)};
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_win32_errc_with_value(8u);
		}
		return p;
	}
	static inline void *reallocate_zero_die(void *addr, ::std::size_t n) noexcept
	{
		auto p{::fast_io::details::win32_heaprealloc_common_impl(addr, n, 0x00000008u)};
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
	static inline void *reallocate_zero_try(void *addr, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		auto p{::fast_io::details::win32_heaprealloc_common_impl(addr, n, 0x00000008u)};
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_win32_errc_with_value(8u);
		}
		return p;
	}
	static inline void *reallocate_conditional_zero_die(void *addr, ::std::size_t n, bool zeroing) noexcept
	{
		auto p{::fast_io::details::win32_heaprealloc_common_impl(addr, n, zeroing ? 0x00000008u : 0u)};
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
	static inline void *reallocate_conditional_zero_try(void *addr, ::std::size_t n, bool zeroing) FAST_IO_HERBCEPTIONS_THROWS
	{
		auto p{::fast_io::details::win32_heaprealloc_common_impl(addr, n, zeroing ? 0x00000008u : 0u)};
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_win32_errc_with_value(8u);
		}
		return p;
	}
	static inline void deallocate(void *addr) noexcept
	{
		if (addr == nullptr) [[unlikely]]
		{
			return;
		}
		::fast_io::win32::HeapFree(::fast_io::details::win32_get_process_heap(), 0u, addr);
	}
};

} // namespace fast_io
