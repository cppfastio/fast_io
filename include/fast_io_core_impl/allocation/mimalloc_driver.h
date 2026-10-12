#pragma once
#include <mimalloc.h>

namespace fast_io
{

class mimalloc_allocator
{
	/*
	no refernce counting. refernece counting allocator is extremely dangerous
	*/
public:
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *allocate_die(::std::size_t n) noexcept
	{
		void *p{::fast_io::noexcept_call(mi_malloc, n)};
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
		void *p{::fast_io::noexcept_call(mi_malloc, n)};
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return p;
	}
	static inline ::fast_io::allocation_least_result allocate_at_least_die(::std::size_t n) noexcept
	{
		void *p{::fast_io::noexcept_call(mi_malloc, n)};
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return {p, ::fast_io::noexcept_call(mi_malloc_usable_size, p)};
	}
	static inline ::fast_io::allocation_least_result allocate_at_least_try(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		void *p{::fast_io::noexcept_call(mi_malloc, n)};
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return {p, ::fast_io::noexcept_call(mi_malloc_usable_size, p)};
	}
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *allocate_aligned_die(::std::size_t alignment, ::std::size_t n) noexcept
	{
		void *p{::fast_io::noexcept_call(mi_malloc_aligned, n, alignment)};
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *allocate_aligned_try(::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		void *p{::fast_io::noexcept_call(mi_malloc_aligned, n, alignment)};
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return p;
	}
	static inline ::fast_io::allocation_least_result allocate_aligned_at_least_die(::std::size_t alignment, ::std::size_t n) noexcept
	{
		void *p{::fast_io::noexcept_call(mi_malloc_aligned, n, alignment)};
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return {p, ::fast_io::noexcept_call(mi_malloc_usable_size, p)};
	}
	static inline ::fast_io::allocation_least_result allocate_aligned_at_least_try(::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		void *p{::fast_io::noexcept_call(mi_malloc_aligned, n, alignment)};
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return {p, ::fast_io::noexcept_call(mi_malloc_usable_size, p)};
	}
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *allocate_zero_aligned_die(::std::size_t alignment, ::std::size_t n) noexcept
	{
		void *p{::fast_io::noexcept_call(mi_zalloc_aligned, n, alignment)};
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *allocate_zero_aligned_try(::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		void *p{::fast_io::noexcept_call(mi_zalloc_aligned, n, alignment)};
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return p;
	}
	static inline ::fast_io::allocation_least_result allocate_zero_aligned_at_least_die(::std::size_t alignment, ::std::size_t n) noexcept
	{
		void *p{::fast_io::noexcept_call(mi_zalloc_aligned, n, alignment)};
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return {p, ::fast_io::noexcept_call(mi_malloc_usable_size, p)};
	}
	static inline ::fast_io::allocation_least_result allocate_zero_aligned_at_least_try(::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		void *p{::fast_io::noexcept_call(mi_zalloc_aligned, n, alignment)};
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return {p, ::fast_io::noexcept_call(mi_malloc_usable_size, p)};
	}
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *reallocate_die(void *p, ::std::size_t n) noexcept
	{
		p = ::fast_io::noexcept_call(mi_realloc, p, n);
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
		p = ::fast_io::noexcept_call(mi_realloc, p, n);
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return p;
	}
	static inline ::fast_io::allocation_least_result reallocate_at_least_die(void *p, ::std::size_t n) noexcept
	{
		p = ::fast_io::noexcept_call(mi_realloc, p, n);
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return {p, ::fast_io::noexcept_call(mi_malloc_usable_size, p)};
	}
	static inline ::fast_io::allocation_least_result reallocate_at_least_try(void *p, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		p = ::fast_io::noexcept_call(mi_realloc, p, n);
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return {p, ::fast_io::noexcept_call(mi_malloc_usable_size, p)};
	}
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *reallocate_aligned_die(void *p, ::std::size_t alignment, ::std::size_t n) noexcept
	{
		p = ::fast_io::noexcept_call(mi_realloc_aligned, p, n, alignment);
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *reallocate_aligned_try(void *p, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		p = ::fast_io::noexcept_call(mi_realloc_aligned, p, n, alignment);
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return p;
	}
	static inline ::fast_io::allocation_least_result reallocate_aligned_at_least_die(void *p, ::std::size_t alignment, ::std::size_t n) noexcept
	{
		p = ::fast_io::noexcept_call(mi_realloc_aligned, p, n, alignment);
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return {p, ::fast_io::noexcept_call(mi_malloc_usable_size, p)};
	}
	static inline ::fast_io::allocation_least_result reallocate_aligned_at_least_try(void *p, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		p = ::fast_io::noexcept_call(mi_realloc_aligned, p, n, alignment);
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return {p, ::fast_io::noexcept_call(mi_malloc_usable_size, p)};
	}
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *reallocate_zero_die(void *p, ::std::size_t n) noexcept
	{
		p = ::fast_io::noexcept_call(mi_rezalloc, p, n);
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *reallocate_zero_try(void *p, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		p = ::fast_io::noexcept_call(mi_rezalloc, p, n);
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return p;
	}
	static inline ::fast_io::allocation_least_result reallocate_zero_at_least_die(void *p, ::std::size_t n) noexcept
	{
		p = ::fast_io::noexcept_call(mi_rezalloc, p, n);
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return {p, ::fast_io::noexcept_call(mi_malloc_usable_size, p)};
	}
	static inline ::fast_io::allocation_least_result reallocate_zero_at_least_try(void *p, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		p = ::fast_io::noexcept_call(mi_rezalloc, p, n);
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return {p, ::fast_io::noexcept_call(mi_malloc_usable_size, p)};
	}
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *reallocate_zero_aligned_die(void *p, ::std::size_t alignment, ::std::size_t n) noexcept
	{
		p = ::fast_io::noexcept_call(mi_rezalloc_aligned, p, n, alignment);
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *reallocate_zero_aligned_try(void *p, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		p = ::fast_io::noexcept_call(mi_rezalloc_aligned, p, n, alignment);
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return p;
	}
	static inline ::fast_io::allocation_least_result reallocate_zero_aligned_at_least_die(void *p, ::std::size_t alignment, ::std::size_t n) noexcept
	{
		p = ::fast_io::noexcept_call(mi_rezalloc_aligned, p, n, alignment);
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return {p, ::fast_io::noexcept_call(mi_malloc_usable_size, p)};
	}
	static inline ::fast_io::allocation_least_result reallocate_zero_aligned_at_least_try(void *p, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		p = ::fast_io::noexcept_call(mi_rezalloc_aligned, p, n, alignment);
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return {p, ::fast_io::noexcept_call(mi_malloc_usable_size, p)};
	}
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *allocate_zero_die(::std::size_t n) noexcept
	{
		void *p{::fast_io::noexcept_call(mi_zalloc, n)};
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
		void *p{::fast_io::noexcept_call(mi_zalloc, n)};
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return p;
	}
	static inline ::fast_io::allocation_least_result allocate_zero_at_least_die(::std::size_t n) noexcept
	{
		void *p{::fast_io::noexcept_call(mi_zalloc, n)};
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return {p, ::fast_io::noexcept_call(mi_malloc_usable_size, p)};
	}
	static inline ::fast_io::allocation_least_result allocate_zero_at_least_try(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		void *p{::fast_io::noexcept_call(mi_zalloc, n)};
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return {p, ::fast_io::noexcept_call(mi_malloc_usable_size, p)};
	}
	static inline void deallocate_n(void *p, ::std::size_t) noexcept
	{
		if (p == nullptr)
		{
			return;
		}
		::fast_io::noexcept_call(mi_free, p);
	}
	static inline void deallocate(void *p) noexcept
	{
		if (p == nullptr)
		{
			return;
		}
		::fast_io::noexcept_call(mi_free, p);
	}
	static inline void deallocate_aligned_n(void *p, ::std::size_t, ::std::size_t) noexcept
	{
		if (p == nullptr)
		{
			return;
		}
		::fast_io::noexcept_call(mi_free, p);
	}
	static inline void deallocate_aligned(void *p, ::std::size_t) noexcept
	{
		if (p == nullptr)
		{
			return;
		}
		::fast_io::noexcept_call(mi_free, p);
	}
};
} // namespace fast_io
