#pragma once
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:6308)
#endif

namespace fast_io
{

class c_malloc_allocator
{
public:
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
	static inline void* allocate(::std::size_t n) noexcept
	{
		if(n==0)
		{
			n=1;
		}
		void* p =
#if defined(__has_builtin)
#if __has_builtin(__builtin_malloc)
			__builtin_malloc(n)
#else
			std::malloc(n)
#endif
#else
			std::malloc(n)
#endif
		;
		if(p==nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
	static inline void* reallocate(void* p,::std::size_t n) noexcept
	{
		if(n==0)
		{
			n=1;
		}
		::std::size_t const to_allocate{n};
		p=
#if defined(__has_builtin)
#if __has_builtin(__builtin_realloc)
			__builtin_realloc
#else
			::std::realloc
#endif
#else
			::std::realloc
#endif
		
		(p,to_allocate);
		if(p==nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
	static inline void* allocate_zero(::std::size_t n) noexcept
	{
		if(n==0)
		{
			n=1;
		}
		void* p =
#if defined(__has_builtin)
#if __has_builtin(__builtin_calloc)
			__builtin_calloc
#else
			::std::calloc
#endif
#else
			::std::calloc
#endif
			
			(1,n);
		if(p==nullptr)
		{
			::fast_io::fast_terminate();
		}
		return p;
	}
	static inline void deallocate(void* p) noexcept
	{
		if(p==nullptr)
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

}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
