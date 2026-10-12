#pragma once

#if __has_include(<malloc.h>)
#include <malloc.h>
#elif __has_include(<malloc_np.h>)
#include <malloc_np.h>
#endif
#include <cstdlib>

namespace fast_io
{

namespace details
{
#if (__has_include(<malloc.h>) || __has_include(<malloc_np.h>)) && !defined(__MSDOS__) && !defined(__LLVM_LIBC__)

inline ::std::size_t c_malloc_usable_size_impl(void *p) noexcept
{
#if defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__)
	return ::fast_io::noexcept_call(_msize, p);
#else
	return ::malloc_usable_size(p);
#endif
}

#endif

} // namespace details

class c_malloc_allocator
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
		void *p =
#if FAST_IO_HAS_BUILTIN(__builtin_malloc)
			__builtin_malloc(n)
#else
			::std::malloc(n)
#endif
			;
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
		void *p =
#if FAST_IO_HAS_BUILTIN(__builtin_malloc)
			__builtin_malloc(n)
#else
			::std::malloc(n)
#endif
			;
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
		::std::size_t const to_allocate{n};
		p =
#if FAST_IO_HAS_BUILTIN(__builtin_realloc)
			__builtin_realloc
#else
			::std::realloc
#endif

			(p, to_allocate);
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
		::std::size_t const to_allocate{n};
		p =
#if FAST_IO_HAS_BUILTIN(__builtin_realloc)
			__builtin_realloc
#else
			::std::realloc
#endif

			(p, to_allocate);
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
		void *p =
#if FAST_IO_HAS_BUILTIN(__builtin_calloc)
			__builtin_calloc
#else
			::std::calloc
#endif

			(1, n);
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
		void *p =
#if FAST_IO_HAS_BUILTIN(__builtin_calloc)
			__builtin_calloc
#else
			::std::calloc
#endif

			(1, n);
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return p;
	}
#if (__has_include(<malloc.h>) || __has_include(<malloc_np.h>)) && !defined(__MSDOS__) && !defined(__LLVM_LIBC__)
	static inline ::fast_io::allocation_least_result allocate_at_least_die(::std::size_t n) noexcept
	{
		auto p{::fast_io::c_malloc_allocator::allocate_die(n)};
		return {p, ::fast_io::details::c_malloc_usable_size_impl(p)};
	}
	static inline ::fast_io::allocation_least_result allocate_at_least_try(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		auto p{::fast_io::c_malloc_allocator::allocate_try(n)};
		return {p, ::fast_io::details::c_malloc_usable_size_impl(p)};
	}
	static inline ::fast_io::allocation_least_result allocate_zero_at_least_die(::std::size_t n) noexcept
	{
		auto p{::fast_io::c_malloc_allocator::allocate_zero_die(n)};
		return {p, ::fast_io::details::c_malloc_usable_size_impl(p)};
	}
	static inline ::fast_io::allocation_least_result allocate_zero_at_least_try(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		auto p{::fast_io::c_malloc_allocator::allocate_zero_try(n)};
		return {p, ::fast_io::details::c_malloc_usable_size_impl(p)};
	}
	static inline ::fast_io::allocation_least_result reallocate_at_least_die(void *oldp, ::std::size_t n) noexcept
	{
		auto p{::fast_io::c_malloc_allocator::reallocate_die(oldp, n)};
		return {p, ::fast_io::details::c_malloc_usable_size_impl(p)};
	}
	static inline ::fast_io::allocation_least_result reallocate_at_least_try(void *oldp, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		auto p{::fast_io::c_malloc_allocator::reallocate_try(oldp, n)};
		return {p, ::fast_io::details::c_malloc_usable_size_impl(p)};
	}
#endif

#if defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__)
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *allocate_aligned_die(::std::size_t alignment, ::std::size_t n) noexcept
	{
		if (n == 0)
		{
			n = 1;
		}
		void *p;
		if (alignment <= __STDCPP_DEFAULT_NEW_ALIGNMENT__)
		{
			p =
#if FAST_IO_HAS_BUILTIN(__builtin_malloc)
				__builtin_malloc
#else
				::std::malloc
#endif
				(n);
		}
		else
		{
			p = ::fast_io::noexcept_call(_aligned_malloc, n, alignment);
		}
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
		if (n == 0)
		{
			n = 1;
		}
		void *p;
		if (alignment <= __STDCPP_DEFAULT_NEW_ALIGNMENT__)
		{
			p =
#if FAST_IO_HAS_BUILTIN(__builtin_malloc)
				__builtin_malloc
#else
				::std::malloc
#endif
				(n);
		}
		else
		{
			p = ::fast_io::noexcept_call(_aligned_malloc, n, alignment);
		}
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return p;
	}
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *reallocate_aligned_die(void *p, ::std::size_t alignment, ::std::size_t n) noexcept
	{
		if (n == 0)
		{
			n = 1;
		}
		if (alignment <= __STDCPP_DEFAULT_NEW_ALIGNMENT__)
		{
			p =
#if FAST_IO_HAS_BUILTIN(__builtin_realloc)
				__builtin_realloc
#else
				::std::realloc
#endif

				(p, n);
		}
		else
		{
			p = ::fast_io::noexcept_call(_aligned_realloc, p, n, alignment);
		}
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
		if (n == 0)
		{
			n = 1;
		}
		if (alignment <= __STDCPP_DEFAULT_NEW_ALIGNMENT__)
		{
			p =
#if FAST_IO_HAS_BUILTIN(__builtin_realloc)
				__builtin_realloc
#else
				::std::realloc
#endif

				(p, n);
		}
		else
		{
			p = ::fast_io::noexcept_call(_aligned_realloc, p, n, alignment);
		}
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return p;
	}
	static inline void deallocate_aligned(void *p, ::std::size_t alignment) noexcept
	{
		if (p == nullptr)
		{
			return;
		}
		if (alignment <= __STDCPP_DEFAULT_NEW_ALIGNMENT__)
		{
#if FAST_IO_HAS_BUILTIN(__builtin_free)
			__builtin_free
#else
			::std::free
#endif
				(p);
		}
		else
		{
			::fast_io::noexcept_call(_aligned_free, p);
		}
	}
#elif !defined(__MSDOS__) && 0
	static inline void *allocate_aligned_die(::std::size_t alignment, ::std::size_t n) noexcept
	{
		if (n == 0)
		{
			n = 1;
		}
		void *p =
#if FAST_IO_HAS_BUILTIN(__builtin_aligned_alloc)
			__builtin_aligned_alloc(alignment, n)
#else
			::std::aligned_alloc(alignment, n)
#endif
			;
		if (p == nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
	static inline void *allocate_aligned_try(::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		if (n == 0)
		{
			n = 1;
		}
		void *p =
#if FAST_IO_HAS_BUILTIN(__builtin_aligned_alloc)
			__builtin_aligned_alloc(alignment, n)
#else
			::std::aligned_alloc(alignment, n)
#endif
			;
		if (p == nullptr)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::not_enough_memory);
		}
		return p;
	}
	static inline void deallocate_aligned(void *p, ::std::size_t) noexcept
	{
		deallocate(p);
	}
#endif
	static inline void deallocate(void *p) noexcept
	{
		if (p == nullptr)
		{
			return;
		}

#if FAST_IO_HAS_BUILTIN(__builtin_free)
		__builtin_free
#else
		::std::free
#endif

			(p);
	}
};

} // namespace fast_io
