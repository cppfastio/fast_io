#pragma once


namespace fast_io
{

namespace details
{

template<typename alloc>
concept has_allocate_zero_impl = requires(::std::size_t n)
{
	{alloc::allocate_zero(n)}->::std::same_as<void*>;
};

template<typename alloc>
concept has_allocate_impl = requires(::std::size_t n)
{
	{alloc::allocate(n)}->::std::same_as<void*>;
};

template<typename alloc>
concept has_aligned_deallocate_impl = requires(void* ptr,::std::size_t alignment)
{
	{alloc::deallocate_aligned(ptr,alignment)}->::std::same_as<void*>;
};

template<typename alloc>
concept has_allocate_zero_aligned_impl = requires(::std::size_t n)
{
	{alloc::allocate_zero_aligned(n,n)}->::std::same_as<void*>;
};

template<typename alloc>
concept has_allocate_aligned_impl = requires(::std::size_t n)
{
	{alloc::allocate_aligned(n,n)}->::std::same_as<void*>;
};

template<typename alloc>
concept has_reallocate_aligned_impl = requires(void* ptr,::std::size_t n)
{
	{alloc::reallocate_aligned(ptr,n,n)}->::std::same_as<void*>;
};

template<typename alloc>
concept has_reallocate_impl = requires(void* ptr,::std::size_t n)
{
	{alloc::reallocate(ptr,n)}->::std::same_as<void*>;
};

template<typename alloc>
concept is_my_allocator_type_impl = ::std::is_empty_v<alloc>&&(has_allocate_zero_impl<alloc>||has_allocate_impl<alloc>
	||has_allocate_zero_aligned_impl<alloc>||has_allocate_aligned_impl<alloc>);

template<typename alloc>
concept has_deallocate_impl = is_my_allocator_type_impl<alloc>||requires(void* ptr)
{
	{alloc::deallocate(ptr)}->::std::same_as<void*>;
};

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

}

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
			else if constexpr(::fast_io::details::has_allocate_aligned_impl<alloc>)
			{
				return allocator_type::allocate_aligned(n,default_alignment);
			}
			else if constexpr(::fast_io::details::has_allocate_zero_aligned_impl<alloc>)
			{
				return allocator_type::allocate_aligned_zero(n,default_alignment);
			}
			else
			{
				fast_io::fast_terminate();
			}
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	void deallocate(void* p) noexcept
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
			if constexpr(::fast_io::details::has_allocate_impl<alloc>)
			{
				allocator_type::deallocate(p);
			}
		}
	}
	static inline void* reallocate(void* p,::std::size_t n) noexcept
	{
		if constexpr(::fast_io::details::has_reallocate_impl<alloc>)
		{
			return allocator_type::reallocate(p,n);
		}
		else
		{
			auto newptr{generic_allocator_adapter::allocate(n)};
			if(p!=nullptr&&n)
			{
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
				generic_allocator_adapter::deallocate(p);
			}
			return newptr;
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
			if constexpr(::fast_io::details::has_allocate_aligned_impl<alloc>)
			{
				return allocator_type::allocate_aligned(alignment,n);
			}
			else if constexpr(::fast_io::details::has_allocate_zero_aligned_impl<alloc>)
			{
				return allocator_type::allocate_zero_aligned(alignment,n);
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
				if constexpr(::fast_io::details::has_allocate_impl<alloc>)
				{
					p=allocator_type::allocate(to_allocate);
				}
				else
				{
					p=allocator_type::allocate_zero(to_allocate);
				}
				void* aligned_ptr{reinterpret_cast<void*>((reinterpret_cast<::std::size_t>(p)+alignment)&(-alignment))};
				reinterpret_cast<void**>(aligned_ptr)[-1] = p;
				return aligned_ptr;
			}
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	void deallocate_aligned(void* p,::std::size_t alignment) noexcept
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
			if constexpr(::fast_io::details::has_allocate_aligned_impl<alloc>)
			{
				return allocator_type::deallocate_aligned(p,alignment);
			}
			else
			{
				if(p==nullptr)
				{
					return;
				}
				if constexpr(::fast_io::details::has_deallocate_impl<alloc>)
				{
					allocator_type::deallocate(reinterpret_cast<void**>(p)[-1]);
				}
			}
		}
	}

	static inline void* reallocate_aligned(void* p,::std::size_t alignment,::std::size_t n) noexcept
	{
		if constexpr(::fast_io::details::has_reallocate_aligned_impl<alloc>)
		{
			return allocator_type::reallocate_aligned(p,alignment,n);
		}
		else
		{
			auto newptr{generic_allocator_adapter::allocate_aligned(alignment,n)};
			if(p!=nullptr&&n)
			{
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
				generic_allocator_adapter::deallocate_aligned(p,alignment);
			}
			return newptr;
		}
	}

	static inline void* allocate_zero_aligned(::std::size_t alignment,::std::size_t n) noexcept
	{
		if constexpr(::fast_io::details::has_allocate_zero_aligned_impl<alloc>)
		{
			return allocator_type::allocate_zero_aligned(alignment,n);
		}
		else
		{
			auto p{generic_allocator_adapter::allocate_aligned(alignment,n)};
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
};

template<typename alloc,typename T>
class typed_generic_allocator_adapter
{
public:
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

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	void deallocate(T* ptr) noexcept
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
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	void* reallocate(T* ptr,::std::size_t n) noexcept requires(::std::is_trivially_copyable_v<T>)
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
};

}
