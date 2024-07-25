#pragma once

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(push)
#pragma warning(disable : 6308)
#endif

#include <crtdbg.h>

namespace fast_io
{

class wincrt_malloc_dbg_allocator
{
public:
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *allocate(::std::size_t n) noexcept
	{
		if (n == 0)
		{
			n = 1;
		}
		void *p = ::fast_io::noexcept_call(_malloc_dbg, n, 1, __FILE__, __LINE__);
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *reallocate(void *p, ::std::size_t n) noexcept
	{
		if (n == 0)
		{
			n = 1;
		}
		p = ::fast_io::noexcept_call(_realloc_dbg, p, n, 1, __FILE__, __LINE__);
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *allocate_zero(::std::size_t n) noexcept
	{
		if (n == 0)
		{
			n = 1;
		}
		void *p = ::fast_io::noexcept_call(_calloc_dbg, 1, n, 1, __FILE__, __LINE__);
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
	static inline void deallocate(void *p) noexcept
	{
		if (p == nullptr)
		{
			return;
		}
		::fast_io::noexcept_call(_free_dbg, p, 1);
	}

#if 0
	static inline allocation_least_result allocate_at_least(::std::size_t n) noexcept
	{
		auto p{::fast_io::wincrt_malloc_dbg_allocator::allocate(n)};
		return {p, ::fast_io::noexcept_call(_msize_dbg, p, 1)};
	}
	static inline allocation_least_result allocate_zero_at_least(::std::size_t n) noexcept
	{
		auto p{::fast_io::wincrt_malloc_dbg_allocator::allocate_zero(n)};
		return {p, ::fast_io::noexcept_call(_msize_dbg, p, 1)};
	}
	static inline allocation_least_result reallocate_at_least(void *oldp, ::std::size_t n) noexcept
	{
		auto p{::fast_io::wincrt_malloc_dbg_allocator::reallocate(oldp, n)};
		return {p, ::fast_io::noexcept_call(_msize_dbg, p, 1)};
	}
#endif
};

} // namespace fast_io

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#endif
