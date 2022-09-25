#pragma once


namespace fast_io
{

namespace details
{

#include"has_methods_detect.h"

template<typename alloc>
concept has_default_alignment_impl = requires(::std::size_t n)
{
	alloc::default_alignment;
};

template<typename alloc>
inline constexpr ::std::size_t calculate_default_alignment() noexcept
{
	if constexpr(has_default_alignment_impl<alloc>)
	{
		return alloc::default_alignment;
	}
	else
	{
		return __STDCPP_DEFAULT_NEW_ALIGNMENT__;
	}
}

template<typename alloc,bool zero>
inline constexpr void* allocator_pointer_aligned_impl(::std::size_t alignment,::std::size_t n) noexcept
{
	if constexpr(!has_allocate_impl<alloc>&&!has_allocate_zero_impl<alloc>)
	{
		::fast_io::fast_terminate();
	}
	else if constexpr(!zero&&!has_allocate_impl<alloc>)
	{
		return allocator_pointer_aligned_impl<alloc,true>(alignment,n);
	}
	else if constexpr(zero&&!has_allocate_zero_impl<alloc>)
	{
		void *aligned_ptr{allocator_pointer_aligned_impl<alloc,false>(alignment,n)};
#if defined(__has_builtin)
#if __has_builtin(__builtin_memset)
		__builtin_memset
#else
		std::memset
#endif
#else
		std::memset
#endif
		(aligned_ptr,0,n);
		return aligned_ptr;
	}
	else
	{
		constexpr
			::std::size_t mxn{::std::numeric_limits<::std::size_t>::max()},
				sizeofptr{sizeof(void*)},
				mxmptr{mxn-sizeofptr};
		if (alignment < sizeofptr)
			alignment = sizeofptr;
		if(alignment>mxmptr)
		{
			::fast_io::fast_terminate();
		}
		::std::size_t total_extra_space{alignment+sizeofptr};
		::std::size_t upperlimit{static_cast<std::size_t>(mxn-total_extra_space)};
		if(n>upperlimit)
		{
			::fast_io::fast_terminate();
		}
		::std::size_t to_allocate{n+total_extra_space};
		void* p;
		if constexpr(zero)
		{
			if constexpr(has_allocate_zero_impl<alloc>)
			{
				p=alloc::allocate_zero(to_allocate);
			}
			else
			{
				p=alloc::allocate(to_allocate);
			}
		}
		else
		{
			if constexpr(has_allocate_impl<alloc>)
			{
				p=alloc::allocate(to_allocate);
			}
			else
			{
				p=alloc::allocate_zero(to_allocate);
			}
		}
		void* aligned_ptr{reinterpret_cast<void*>((reinterpret_cast<::std::size_t>(p)+alignment)&(-alignment))};
		reinterpret_cast<void**>(aligned_ptr)[-1] = p;
		return aligned_ptr;
	}
}

}

#if 0
#include"allocator_adapter_flags.h"
#endif

template<typename alloc>
class generic_allocator_adapter
{
public:
	using allocator_type = alloc;
	static inline constexpr ::std::size_t default_alignment{
		::fast_io::details::calculate_default_alignment<allocator_type>()
	};

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	void* allocate(::std::size_t n) noexcept
	{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if(__builtin_is_constant_evaluated())
#endif
		{
			return ::operator new(n);
		}
		else
#endif
		{
			if constexpr(::fast_io::details::has_allocate_impl<alloc>)
			{
				return allocator_type::allocate(n);
			}
			else if constexpr(::fast_io::details::has_allocate_zero_impl<alloc>)
			{
				return allocator_type::allocate_zero(n);
			}
			else
			{
				fast_io::fast_terminate();
			}
		}
	}

	static inline void* allocate_zero(::std::size_t n) noexcept
	{
		if constexpr(::fast_io::details::has_allocate_zero_impl<alloc>)
		{
			return allocator_type::allocate_zero(n);
		}
		else
		{
			auto p{generic_allocator_adapter::allocate(n)};
#if defined(__has_builtin)
#if __has_builtin(__builtin_memset)
			__builtin_memset(p,0,n);
#else
			std::memset(p,0,n);
#endif
#else
			std::memset(p,0,n);
#endif
			return p;
		}
	}

	static inline constexpr bool has_reallocate = ::fast_io::details::has_reallocate_impl<alloc>;

	static inline void* reallocate(void* p,::std::size_t n) noexcept
		requires(has_reallocate)
	{
		return allocator_type::reallocate(p,n);
	}

	static inline void* reallocate_n(void* p,::std::size_t oldn,::std::size_t n) noexcept
	{
		if constexpr(::fast_io::details::has_reallocate_n_impl<alloc>)
		{
			return allocator_type::reallocate_n(p,oldn,n);
		}
		else
		{
			auto newptr{generic_allocator_adapter::allocate(n)};
			if(p!=nullptr&&n)
			{
				if(oldn<n)
				{
					n=oldn;
				}
#if defined(__has_builtin)
#if __has_builtin(__builtin_memcpy)
				__builtin_memcpy
#else
				std::memcpy
#endif
#else
				std::memcpy
#endif
				(newptr,p,n);
				generic_allocator_adapter::deallocate_n(p,oldn);
			}
			return newptr;
		}
	}
	static inline constexpr bool has_reallocate_zero = ::fast_io::details::has_reallocate_zero_impl<alloc>;

	static inline void* reallocate_zero(void* p,::std::size_t n) noexcept
		requires(has_reallocate_zero)
	{
		return allocator_type::reallocate_zero(p,n);
	}

	static inline void* reallocate_zero_n(void* p,::std::size_t oldn,::std::size_t n) noexcept
	{
		if constexpr(::fast_io::details::has_reallocate_zero_n_impl<alloc>)
		{
			return allocator_type::reallocate_zero_n(p,oldn,n);
		}
		else if constexpr(::fast_io::details::has_reallocate_zero_impl<alloc>)
		{
			return allocator_type::reallocate_zero(p,n);
		}
		else
		{
			auto newptr{generic_allocator_adapter::reallocate_n(p,oldn,n)};
			if(oldn<n)
			{
				::std::size_t const to_zero_bytes{static_cast<std::size_t>(n-oldn)};
#if defined(__has_builtin)
#if __has_builtin(__builtin_memset)
				__builtin_memset
#else
				std::memset
#endif
#else
				std::memset
#endif
				(reinterpret_cast<char*>(newptr)+oldn,0,to_zero_bytes);
			}
			return newptr;
		}
	}

	static inline constexpr bool has_deallocate = ::fast_io::details::has_deallocate_impl<alloc>;

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	void deallocate(void* p) noexcept requires(has_deallocate)
	{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if(__builtin_is_constant_evaluated())
#endif
		{
			::operator delete(p);
		}
		else
#endif
		{
			allocator_type::deallocate(p);
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	void deallocate_n(void* p,::std::size_t n) noexcept
	{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if(__builtin_is_constant_evaluated())
#endif
		{
			::operator delete(p);
		}
		else
#endif
		{
			if constexpr(::fast_io::details::has_deallocate_n_impl<alloc>)
			{
				allocator_type::deallocate_n(p,n);
			}
			else if constexpr(::fast_io::details::has_deallocate_impl<alloc>)
			{
				allocator_type::deallocate(p);
			}
		}
	}


	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	void* allocate_aligned(::std::size_t alignment,::std::size_t n) noexcept
	{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if(__builtin_is_constant_evaluated())
#endif
		{
			return ::operator new(n);
		}
		else
#endif
		{
			return ::fast_io::details::allocator_pointer_aligned_impl<alloc,false>(alignment,n);
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	void* allocate_aligned_zero(::std::size_t alignment,::std::size_t n) noexcept
	{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if(__builtin_is_constant_evaluated())
#endif
		{
			return ::operator new(n);//this is problematic. No way to clean it up at compile time.
		}
		else
#endif
		{
			return ::fast_io::details::allocator_pointer_aligned_impl<alloc,true>(alignment,n);
		}
	}


	static inline constexpr bool has_reallocate_aligned = ::fast_io::details::has_reallocate_aligned_impl<alloc>;

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	void* reallocate_aligned(void* p,::std::size_t alignment,::std::size_t n) noexcept
		requires(has_reallocate_aligned)
	{
		return allocator_type::reallocate_aligned(p,alignment,n);
	}

	static inline constexpr bool has_reallocate_aligned_zero =
		::fast_io::details::has_reallocate_aligned_zero_impl<alloc>;

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	void* reallocate_aligned_zero(void* p,::std::size_t alignment,::std::size_t n) noexcept
		requires(has_reallocate_aligned_zero)
	{
		return allocator_type::reallocate_aligned_zero(p,alignment,n);		
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	void* reallocate_aligned_n(void* p,::std::size_t oldn,::std::size_t alignment,::std::size_t n) noexcept
	{
		if constexpr(::fast_io::details::has_reallocate_aligned_n_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_n(p,oldn,alignment,n);
		}
		else if constexpr(::fast_io::details::has_reallocate_aligned_impl<alloc>)
		{
			return allocator_type::reallocate_aligned(p,alignment,n);
		}
		else if constexpr(::fast_io::details::has_reallocate_aligned_zero_n_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_n(p,oldn,alignment,n);
		}
		else if constexpr(::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero(p,alignment,n);
		}
		else
		{
			auto newptr{::fast_io::details::allocator_pointer_aligned_impl<alloc,false>(alignment,n)};
			if(p!=nullptr&&n)
			{
				if(oldn<n)
				{
					n=oldn;
				}
#if defined(__has_builtin)
#if __has_builtin(__builtin_memcpy)
				__builtin_memcpy
#else
				std::memcpy
#endif
#else
				std::memcpy
#endif
				(newptr,p,n);
				generic_allocator_adapter::deallocate_aligned_n(p,alignment,oldn);
			}
			return newptr;
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	void* reallocate_aligned_zero_n(void* p,::std::size_t oldn,::std::size_t alignment,::std::size_t n) noexcept
	{
		if constexpr(::fast_io::details::has_reallocate_aligned_zero_n_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_n(p,oldn,alignment,n);
		}
		else
		{
			auto newptr{reallocate_aligned_n(p,oldn,alignment,n)};
#if defined(__has_builtin)
#if __has_builtin(__builtin_memset)
			__builtin_memset(newptr,0,n);
#else
			std::memset(newptr,0,n);
#endif
#else
			std::memset(newptr,0,n);
#endif
			return newptr;
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	void deallocate_aligned_n(void* p,::std::size_t alignment,::std::size_t n) noexcept
	{
		if constexpr(::fast_io::details::has_deallocate_aligned_n_impl<alloc>)
		{
			allocator_type::deallocate_aligned_n(p,alignment,n);
		}
		else if constexpr(::fast_io::details::has_deallocate_aligned_impl<alloc>)
		{
			allocator_type::deallocate_aligned(p,alignment);
		}
		else
		{
			if(p==nullptr)
			{
				return;
			}
			::std::size_t const to_deallocate{sizeof(void*)+alignment+n};
			allocator_type::deallocate(reinterpret_cast<void**>(p)[-1],to_deallocate);
		}
	}
	static inline constexpr bool has_deallocate_aligned = ::fast_io::details::has_deallocate_aligned_impl<alloc>;
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	void deallocate_aligned(void* p,::std::size_t alignment) noexcept requires(has_deallocate_aligned)
	{
		allocator_type::deallocate_aligned(p,alignment);
	}
};

template<typename alloc,typename T>
class typed_generic_allocator_adapter
{
public:
	using allocator_adapter = alloc;
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	T* allocate(::std::size_t n) noexcept
	{
		constexpr
			::std::size_t mxn{::std::numeric_limits<::std::size_t>::max()/sizeof(T)};
		if(n>mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr(alignof(T)<=alloc::default_alignment)
		{
			return static_cast<T*>(alloc::allocate(n*sizeof(T)));
		}
		else
		{
			return static_cast<T*>(alloc::allocate_aligned(n*sizeof(T),alignof(T)));
		}
	}
	static inline constexpr bool has_deallocate = allocator_adapter::has_deallocate;

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	void deallocate(T* ptr) noexcept requires(has_deallocate)
	{
		if constexpr(alignof(T)<=alloc::default_alignment)
		{
			return alloc::deallocate(ptr);
		}
		else
		{
			return alloc::deallocate_aligned(ptr,alignof(T));
		}
	}
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	void deallocate_n(T* ptr,::std::size_t n) noexcept
	{
		if constexpr(alignof(T)<=alloc::default_alignment)
		{
			alloc::deallocate_n(ptr,n);
		}
		else
		{
			alloc::deallocate_aligned_n(ptr,alignof(T),n);
		}
	}
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	T* allocate_zero(::std::size_t n) noexcept
	{
		constexpr
			::std::size_t mxn{::std::numeric_limits<::std::size_t>::max()/sizeof(T)};
		if(n>mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr(alignof(T)<=alloc::default_alignment)
		{
			return static_cast<T*>(alloc::allocate_zero(n*sizeof(T)));
		}
		else
		{
			return static_cast<T*>(alloc::allocate_zero_aligned(n*sizeof(T),alignof(T)));
		}
	}

	static inline constexpr bool has_reallocate = allocator_adapter::has_reallocate;
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	void* reallocate(T* ptr,::std::size_t n) noexcept requires(has_reallocate)
	{
		constexpr
			::std::size_t mxn{::std::numeric_limits<::std::size_t>::max()/sizeof(T)};
		if(n>mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr(alignof(T)<=alloc::default_alignment)
		{
			return static_cast<T*>(alloc::reallocate(ptr,n*sizeof(T)));
		}
		else
		{
			return static_cast<T*>(alloc::reallocate_aligned(ptr,n*sizeof(T),alignof(T)));
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	void* reallocate_n(T* ptr,::std::size_t oldn,::std::size_t n) noexcept
	{
		constexpr
			::std::size_t mxn{::std::numeric_limits<::std::size_t>::max()/sizeof(T)};
		if(n>mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr(alignof(T)<=alloc::default_alignment)
		{
			return static_cast<T*>(alloc::reallocate_n(ptr,oldn,n*sizeof(T)));
		}
		else
		{
			return static_cast<T*>(alloc::reallocate_aligned_n(ptr,oldn,n*sizeof(T),alignof(T)));
		}
	}

	static inline constexpr bool has_reallocate_zero = allocator_adapter::has_reallocate_zero;
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	void* reallocate_zero(T* ptr,::std::size_t n) noexcept requires(has_reallocate)
	{
		constexpr
			::std::size_t mxn{::std::numeric_limits<::std::size_t>::max()/sizeof(T)};
		if(n>mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr(alignof(T)<=alloc::default_alignment)
		{
			return static_cast<T*>(alloc::reallocate_zero(ptr,n*sizeof(T)));
		}
		else
		{
			return static_cast<T*>(alloc::reallocate_aligned_zero(ptr,n*sizeof(T),alignof(T)));
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	void* reallocate_zero_n(T* ptr,::std::size_t oldn,::std::size_t n) noexcept
	{
		constexpr
			::std::size_t mxn{::std::numeric_limits<::std::size_t>::max()/sizeof(T)};
		if(n>mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr(alignof(T)<=alloc::default_alignment)
		{
			return static_cast<T*>(alloc::reallocate_zero_n(ptr,oldn,n*sizeof(T)));
		}
		else
		{
			return static_cast<T*>(alloc::reallocate_aligned_zero_n(ptr,oldn,n*sizeof(T),alignof(T)));
		}
	}
};

}
