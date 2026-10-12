#pragma once

#include "nt_preliminary_definition.h"

namespace fast_io
{

namespace details
{

inline void *nt_rtlallocate_heap_handle_common_impl(void *heaphandle, ::std::size_t n, ::std::uint_least32_t flag) noexcept
{
	if (n == 0)
	{
		n = 1;
	}
	return ::fast_io::win32::nt::RtlAllocateHeap(heaphandle, flag, n);
}

inline void *nt_rtlreallocate_heap_handle_common_impl(void *heaphandle, void *addr, ::std::size_t n, ::std::uint_least32_t flag) noexcept
{
	if (n == 0)
	{
		n = 1;
	}
	if (addr == nullptr)
		[[unlikely]]
	{
		return ::fast_io::details::nt_rtlallocate_heap_handle_common_impl(heaphandle, n, flag);
	}
	return ::fast_io::win32::nt::RtlReAllocateHeap(heaphandle, flag, addr, n);
}

inline void *nt_rtlallocate_heap_common_impl(::std::size_t n, ::std::uint_least32_t flag) noexcept
{
	return ::fast_io::details::nt_rtlallocate_heap_handle_common_impl(::fast_io::win32::nt::rtl_get_process_heap(), n, flag);
}

inline void *nt_rtlreallocate_heap_common_impl(void *addr, ::std::size_t n, ::std::uint_least32_t flag) noexcept
{
	return ::fast_io::details::nt_rtlreallocate_heap_handle_common_impl(::fast_io::win32::nt::rtl_get_process_heap(), addr, n, flag);
}

} // namespace details

class nt_rtlallocateheap_allocator
{
public:
#if __has_cpp_attribute(__gnu__::__malloc__)
	[[__gnu__::__malloc__]]
#endif
	static inline void *allocate_conditional_zero_die(::std::size_t n, bool zero) noexcept
	{
		auto p{::fast_io::details::nt_rtlallocate_heap_common_impl(n, zero ? 0x00000008u : 0u)};
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
#if __has_cpp_attribute(__gnu__::__malloc__)
	[[__gnu__::__malloc__]]
#endif
	static inline void *allocate_conditional_zero_try(::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS
	{
		auto p{::fast_io::details::nt_rtlallocate_heap_common_impl(n, zero ? 0x00000008u : 0u)};
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_nt_errc_with_value(0xC0000017u);
		}
		return p;
	}
	static inline void *reallocate_die(void *addr, ::std::size_t n) noexcept
	{
		auto p{::fast_io::details::nt_rtlreallocate_heap_common_impl(addr, n, 0u)};
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
	static inline void *reallocate_try(void *addr, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		auto p{::fast_io::details::nt_rtlreallocate_heap_common_impl(addr, n, 0u)};
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_nt_errc_with_value(0xC0000017u);
		}
		return p;
	}
	static inline void *reallocate_zero_die(void *addr, ::std::size_t n) noexcept
	{
		auto p{::fast_io::details::nt_rtlreallocate_heap_common_impl(addr, n, 0x00000008u)};
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
	static inline void *reallocate_zero_try(void *addr, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		auto p{::fast_io::details::nt_rtlreallocate_heap_common_impl(addr, n, 0x00000008u)};
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_nt_errc_with_value(0xC0000017u);
		}
		return p;
	}
	static inline void deallocate(void *addr) noexcept
	{
		if (addr == nullptr) [[unlikely]]
		{
			return;
		}
		::fast_io::win32::nt::RtlFreeHeap(::fast_io::win32::nt::rtl_get_process_heap(), 0u, addr);
	}
};

} // namespace fast_io
