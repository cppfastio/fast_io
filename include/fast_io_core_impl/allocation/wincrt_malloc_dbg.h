#pragma once

#include <crtdbg.h>

namespace fast_io
{

class wincrt_malloc_dbg_allocator
{
public:
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *allocate_die(::std::size_t n) noexcept
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
	static inline void *allocate_try(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		if (n == 0)
		{
			n = 1;
		}
		void *p = ::fast_io::noexcept_call(_malloc_dbg, n, 1, __FILE__, __LINE__);
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return p;
	}
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *reallocate_die(void *p, ::std::size_t n) noexcept
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
	static inline void *reallocate_try(void *p, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		if (n == 0)
		{
			n = 1;
		}
		p = ::fast_io::noexcept_call(_realloc_dbg, p, n, 1, __FILE__, __LINE__);
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return p;
	}
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *allocate_zero_die(::std::size_t n) noexcept
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
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *allocate_zero_try(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		if (n == 0)
		{
			n = 1;
		}
		void *p = ::fast_io::noexcept_call(_calloc_dbg, 1, n, 1, __FILE__, __LINE__);
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return p;
	}
	static inline void deallocate(void *p) noexcept
	{
		if (p == nullptr) [[unlikely]]
		{
			return;
		}
		::fast_io::noexcept_call(_free_dbg, p, 1);
	}

#if 0
	static inline allocation_least_result allocate_at_least_die(::std::size_t n) noexcept
	{
		auto p{::fast_io::wincrt_malloc_dbg_allocator::allocate_die(n)};
		return {p, ::fast_io::noexcept_call(_msize_dbg, p, 1)};
	}
	static inline allocation_least_result allocate_at_least_try(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		auto p{::fast_io::wincrt_malloc_dbg_allocator::allocate_try(n)};
		return {p, ::fast_io::noexcept_call(_msize_dbg, p, 1)};
	}
	static inline allocation_least_result allocate_zero_at_least_die(::std::size_t n) noexcept
	{
		auto p{::fast_io::wincrt_malloc_dbg_allocator::allocate_zero_die(n)};
		return {p, ::fast_io::noexcept_call(_msize_dbg, p, 1)};
	}
	static inline allocation_least_result allocate_zero_at_least_try(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		auto p{::fast_io::wincrt_malloc_dbg_allocator::allocate_zero_try(n)};
		return {p, ::fast_io::noexcept_call(_msize_dbg, p, 1)};
	}
	static inline allocation_least_result reallocate_at_least_die(void *oldp, ::std::size_t n) noexcept
	{
		auto p{::fast_io::wincrt_malloc_dbg_allocator::reallocate_die(oldp, n)};
		return {p, ::fast_io::noexcept_call(_msize_dbg, p, 1)};
	}
	static inline allocation_least_result reallocate_at_least_try(void *oldp, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		auto p{::fast_io::wincrt_malloc_dbg_allocator::reallocate_try(oldp, n)};
		return {p, ::fast_io::noexcept_call(_msize_dbg, p, 1)};
	}
#endif
};

} // namespace fast_io
