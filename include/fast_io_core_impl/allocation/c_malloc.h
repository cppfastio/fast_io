#pragma once
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 6308)
#endif

#if __has_include(<malloc.h>)
#include <malloc.h>
#elif __has_include(<malloc_np.h>)
#include <malloc_np.h>
#endif

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
	static inline void *allocate(::std::size_t n) noexcept
	{
		if (n == 0)
		{
			n = 1;
		}
		void *p =
#if defined(__has_builtin)
#if __has_builtin(__builtin_malloc)
			__builtin_malloc(n)
#else
			::std::malloc(n)
#endif
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
	static inline void *reallocate(void *p, ::std::size_t n) noexcept
	{
		if (n == 0)
		{
			n = 1;
		}
		::std::size_t const to_allocate{n};
		p =
#if defined(__has_builtin)
#if __has_builtin(__builtin_realloc)
			__builtin_realloc
#else
			::std::realloc
#endif
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
	static inline void *allocate_zero(::std::size_t n) noexcept
	{
		if (n == 0)
		{
			n = 1;
		}
		void *p =
#if defined(__has_builtin)
#if __has_builtin(__builtin_calloc)
			__builtin_calloc
#else
			::std::calloc
#endif
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
#if (__has_include(<malloc.h>) || __has_include(<malloc_np.h>)) && !defined(__MSDOS__) && !defined(__LLVM_LIBC__)
	static inline allocation_least_result allocate_at_least(::std::size_t n) noexcept
	{
		auto p{::fast_io::c_malloc_allocator::allocate(n)};
		return {p, ::fast_io::details::c_malloc_usable_size_impl(p)};
	}
	static inline allocation_least_result allocate_zero_at_least(::std::size_t n) noexcept
	{
		auto p{::fast_io::c_malloc_allocator::allocate_zero(n)};
		return {p, ::fast_io::details::c_malloc_usable_size_impl(p)};
	}
	static inline allocation_least_result reallocate_at_least(void *oldp, ::std::size_t n) noexcept
	{
		auto p{::fast_io::c_malloc_allocator::reallocate(oldp, n)};
		return {p, ::fast_io::details::c_malloc_usable_size_impl(p)};
	}
#endif

#if defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__)
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline void *allocate_aligned(::std::size_t alignment, ::std::size_t n) noexcept
	{
		if (n == 0)
		{
			n = 1;
		}
		void *p;
		if (alignment <= __STDCPP_DEFAULT_NEW_ALIGNMENT__)
		{
			p =
#if defined(__has_builtin)
#if __has_builtin(__builtin_malloc)
				__builtin_malloc
#else
				::std::malloc
#endif
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
	static inline void *reallocate_aligned(void *p, ::std::size_t alignment, ::std::size_t n) noexcept
	{
		if (n == 0)
		{
			n = 1;
		}
		if (alignment <= __STDCPP_DEFAULT_NEW_ALIGNMENT__)
		{
			p =
#if defined(__has_builtin)
#if __has_builtin(__builtin_realloc)
				__builtin_realloc
#else
				::std::realloc
#endif
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
	static inline void deallocate_aligned(void *p, ::std::size_t alignment) noexcept
	{
		if (p == nullptr)
		{
			return;
		}
		if (alignment <= __STDCPP_DEFAULT_NEW_ALIGNMENT__)
		{
#if defined(__has_builtin)
#if __has_builtin(__builtin_free)
			__builtin_free
#else
			::std::free
#endif
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
#endif
	static inline void deallocate(void *p) noexcept
	{
		if (p == nullptr)
		{
			return;
		}
#if defined(__has_builtin)
#if __has_builtin(__builtin_free)
		__builtin_free
#else
		::std::free
#endif
#else
		::std::free
#endif

			(p);
	}
};

} // namespace fast_io

#ifdef _MSC_VER
#pragma warning(pop)
#endif
