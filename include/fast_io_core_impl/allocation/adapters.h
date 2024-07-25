#pragma once

// To make a constexpr allocator, we need ::std::allocator. Because only new expression and
// ::std::allocator<T>::allocate are allowed in constexpr functions. See https://github.com/microsoft/STL/issues/1532
// https://github.com/microsoft/STL/issues/4002 gcc and clang provide constexpr new, but still won't compile.
// ::std::allocator<T> is NOT freestanding.

#pragma push_macro("new")
#undef new

namespace fast_io
{

namespace details
{

#include "has_methods_detect.h"

template <typename alloc>
concept has_default_alignment_impl = requires(::std::size_t n) { alloc::default_alignment; };

template <typename alloc>
inline constexpr ::std::size_t calculate_default_alignment() noexcept
{
	if constexpr (has_default_alignment_impl<alloc>)
	{
		return alloc::default_alignment;
	}
	else
	{
		return __STDCPP_DEFAULT_NEW_ALIGNMENT__;
	}
}

inline constexpr ::std::size_t allocator_compute_aligned_total_size_impl(::std::size_t alignment, ::std::size_t n) noexcept
{
	constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max()},
		sizeofptr{sizeof(void *)},
		mxmptr{mxn - sizeofptr};
	if (alignment < sizeofptr)
	{
		alignment = sizeofptr;
	}
	if (alignment > mxmptr)
	{
		::fast_io::fast_terminate();
	}
	::std::size_t total_extra_space{alignment + sizeofptr};
	::std::size_t upperlimit{static_cast<::std::size_t>(mxn - total_extra_space)};
	if (n > upperlimit)
	{
		::fast_io::fast_terminate();
	}
	return n + total_extra_space;
}

inline void *allocator_adjust_ptr_to_aligned_impl(void *p, ::std::size_t alignment) noexcept
{
	void *aligned_ptr{reinterpret_cast<void *>((reinterpret_cast<::std::size_t>(p) + alignment) & (0 - alignment))};
	reinterpret_cast<void **>(aligned_ptr)[-1] = p;
	return aligned_ptr;
}

template <typename, bool>
inline constexpr void *allocator_pointer_aligned_impl(::std::size_t, ::std::size_t) noexcept;

template <typename, bool>
inline constexpr ::fast_io::allocation_least_result allocator_pointer_aligned_at_least_impl(::std::size_t, ::std::size_t) noexcept;

template <typename alloc, bool>
inline constexpr void *status_allocator_pointer_aligned_impl(typename alloc::handle_type, ::std::size_t, ::std::size_t) noexcept;

template <typename alloc, bool>
inline constexpr ::fast_io::allocation_least_result status_allocator_pointer_aligned_at_least_impl(typename alloc::handle_type, ::std::size_t, ::std::size_t) noexcept;

} // namespace details

#if 0
#include "allocator_adapter_flags.h"
#endif

template <typename alloc>
class generic_allocator_adapter
{
public:
	using allocator_type = alloc;
	static inline constexpr bool has_status{::fast_io::details::has_non_empty_handle_type<allocator_type>};
	template <typename T, bool = false>
	struct has
	{
		using type = allocator_type; // any meaningless type other than void
	};
	template <typename T>
	struct has<T, true>
	{
		using type = typename T::handle_type;
	};
	using handle_type = typename has<allocator_type, has_status>::type;
	static inline constexpr ::std::size_t default_alignment{::fast_io::details::calculate_default_alignment<allocator_type>()};
	static inline constexpr bool has_native_allocate{
		!has_status && (::fast_io::details::has_allocate_at_least_impl<alloc> ||
						::fast_io::details::has_allocate_aligned_at_least_impl<alloc> ||
						::fast_io::details::has_allocate_zero_at_least_impl<alloc> ||
						::fast_io::details::has_allocate_aligned_zero_at_least_impl<alloc> ||
						::fast_io::details::has_allocate_impl<alloc> ||
						::fast_io::details::has_allocate_aligned_impl<alloc> ||
						::fast_io::details::has_allocate_zero_impl<alloc> ||
						::fast_io::details::has_allocate_aligned_zero_impl<alloc>)};
	static inline

		void *
		allocate(::std::size_t n) noexcept
		requires(!has_status)
	{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			return ::operator new(n);
		}
		else
#endif
		{
			if constexpr (::fast_io::details::has_allocate_impl<alloc>)
			{
				return allocator_type::allocate(n);
			}
			else if constexpr (::fast_io::details::has_allocate_aligned_at_least_impl<alloc>)
			{
				return allocator_type::allocate_at_least(n).ptr;
			}
			else if constexpr (::fast_io::details::has_allocate_aligned_impl<alloc>)
			{
				return allocator_type::allocate_aligned(default_alignment, n);
			}
			else if constexpr (::fast_io::details::has_allocate_zero_impl<alloc>)
			{
				return allocator_type::allocate_zero(n);
			}
			else if constexpr (::fast_io::details::has_allocate_aligned_at_least_impl<alloc>)
			{
				return allocator_type::allocate_zero_at_least(n).ptr;
			}
			else if constexpr (::fast_io::details::has_allocate_aligned_zero_impl<alloc>)
			{
				return allocator_type::allocate_aligned_zero(default_alignment, n);
			}
			else
			{
				fast_io::fast_terminate();
			}
		}
	}

	static inline void *allocate_zero(::std::size_t n) noexcept
		requires(!has_status)
	{
		if constexpr (::fast_io::details::has_allocate_zero_impl<alloc>)
		{
			return allocator_type::allocate_zero(n);
		}
		else if constexpr (::fast_io::details::has_allocate_zero_at_least_impl<alloc>)
		{
			return allocator_type::allocate_zero_at_least(n).ptr;
		}
		else if constexpr (::fast_io::details::has_allocate_aligned_zero_impl<alloc>)
		{
			return allocator_type::allocate_aligned_zero(default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_allocate_aligned_zero_at_least_impl<alloc>)
		{
			return allocator_type::allocate_aligned_zero_at_least(default_alignment, n).ptr;
		}
		else
		{
			auto p{generic_allocator_adapter::allocate(n)};
			::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(p), n);
			return p;
		}
	}

	static inline constexpr bool has_reallocate = (::fast_io::details::has_reallocate_impl<alloc> ||
												   ::fast_io::details::has_reallocate_at_least_impl<alloc> ||
												   ::fast_io::details::has_reallocate_aligned_impl<alloc> ||
												   ::fast_io::details::has_reallocate_aligned_at_least_impl<alloc> ||
												   ::fast_io::details::has_reallocate_zero_impl<alloc> ||
												   ::fast_io::details::has_reallocate_zero_at_least_impl<alloc> ||
												   ::fast_io::details::has_reallocate_aligned_zero_impl<alloc> ||
												   ::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>);
	static inline void *reallocate(void *p, ::std::size_t n) noexcept
		requires(!has_status && has_reallocate)
	{
		if constexpr (::fast_io::details::has_reallocate_impl<alloc>)
		{
			return allocator_type::reallocate(p, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_at_least(p, n).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_impl<alloc>)
		{
			return allocator_type::reallocate_aligned(p, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_at_least(p, default_alignment, n).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_zero_impl<alloc>)
		{
			return allocator_type::reallocate_zero(p, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_zero_at_least(p, n).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero(p, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_at_least(p, default_alignment, n).ptr;
		}
	}
	static inline constexpr bool has_reallocate_zero = (::fast_io::details::has_reallocate_zero_impl<alloc> ||
														::fast_io::details::has_reallocate_zero_at_least_impl<alloc> ||
														::fast_io::details::has_reallocate_aligned_zero_impl<alloc> ||
														::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>);
	static inline void *reallocate_zero(void *p, ::std::size_t n) noexcept
		requires(!has_status && has_reallocate_zero)
	{
		if constexpr (::fast_io::details::has_reallocate_zero_impl<alloc>)
		{
			return allocator_type::reallocate_zero(p, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_zero_at_least(p, n).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero(p, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_at_least(p, default_alignment, n).ptr;
		}
	}
	static inline void *reallocate_n(void *p, ::std::size_t oldn, ::std::size_t n) noexcept
		requires(!has_status)
	{
		if constexpr (::fast_io::details::has_reallocate_n_impl<alloc>)
		{
			return allocator_type::reallocate_n(p, oldn, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_n_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_n_at_least(p, oldn, n).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_n_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_n(p, oldn, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_n_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_n_at_least(p, oldn, default_alignment, n).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_zero_n_impl<alloc>)
		{
			return allocator_type::reallocate_zero_n(p, oldn, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_zero_n_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_zero_n_at_least(p, oldn, default_alignment, n).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_n(p, oldn, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_n_at_least(p, oldn, default_alignment, n).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_impl<alloc>)
		{
			return allocator_type::reallocate(p, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_at_least(p, n).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_n(p, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_n_at_least(p, default_alignment, n).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_zero_impl<alloc>)
		{
			return allocator_type::reallocate_zero(p, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_zero_at_least(p, n).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_n(p, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_n_at_least(p, default_alignment, n).ptr;
		}
		else
		{
			auto newptr{generic_allocator_adapter::allocate(n)};
			if (p != nullptr && n)
			{
				if (oldn < n)
				{
					n = oldn;
				}
				::fast_io::freestanding::nonoverlapped_bytes_copy_n(reinterpret_cast<::std::byte const *>(p), n, reinterpret_cast<::std::byte *>(newptr));
				generic_allocator_adapter::deallocate_n(p, oldn);
			}
			return newptr;
		}
	}
	static inline void *reallocate_zero_n(void *p, ::std::size_t oldn, ::std::size_t n) noexcept
		requires(!has_status)
	{
		if constexpr (::fast_io::details::has_reallocate_zero_n_impl<alloc>)
		{
			return allocator_type::reallocate_zero_n(p, oldn, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_zero_n_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_zero_n_at_least(p, oldn, n).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_n(p, oldn, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_n_at_least(p, oldn, default_alignment, n).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_zero_impl<alloc>)
		{
			return allocator_type::reallocate_zero(p, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_zero_at_least(p, n).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero(p, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_at_least(p, default_alignment, n).ptr;
		}
		else
		{
			auto newptr{generic_allocator_adapter::reallocate_n(p, oldn, n)};
			if (oldn < n)
			{
				::std::size_t const to_zero_bytes{static_cast<::std::size_t>(n - oldn)};
				::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(newptr) + oldn, to_zero_bytes);
			}
			return newptr;
		}
	}

	static inline constexpr bool has_deallocate = (::fast_io::details::has_deallocate_impl<alloc> ||
												   ::fast_io::details::has_deallocate_aligned_impl<alloc>);
	static inline void deallocate(void *p) noexcept
		requires(!has_status && has_deallocate)
	{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			::operator delete(p);
		}
		else
#endif
		{
			if constexpr (::fast_io::details::has_deallocate_impl<alloc>)
			{
				allocator_type::deallocate(p);
			}
			else if constexpr (::fast_io::details::has_deallocate_aligned_impl<alloc>)
			{
				allocator_type::deallocate_aligned(p, default_alignment);
			}
		}
	}
	static inline void deallocate_n(void *p, ::std::size_t n) noexcept
		requires(!has_status)
	{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			::operator delete(p);
		}
		else
#endif
		{
			if constexpr (::fast_io::details::has_deallocate_n_impl<alloc>)
			{
				allocator_type::deallocate_n(p, n);
			}
			else if constexpr (::fast_io::details::has_deallocate_aligned_n_impl<alloc>)
			{
				allocator_type::deallocate_aligned_n(p, default_alignment, n);
			}
			else if constexpr (::fast_io::details::has_deallocate_impl<alloc>)
			{
				allocator_type::deallocate(p);
			}
			else if constexpr (::fast_io::details::has_deallocate_aligned_impl<alloc>)
			{
				allocator_type::deallocate_aligned(p, default_alignment);
			}
			else
			{
				fast_io::fast_terminate();
			}
		}
	}

	static inline

		void *
		allocate_aligned(::std::size_t alignment, ::std::size_t n) noexcept
		requires(!has_status)
	{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			return ::operator new(n);
		}
		else
#endif
		{
			return ::fast_io::details::allocator_pointer_aligned_impl<alloc, false>(alignment, n);
		}
	}
	static inline

		void *
		allocate_aligned_zero(::std::size_t alignment, ::std::size_t n) noexcept
		requires(!has_status)
	{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			return ::operator new(n); // this is problematic. No way to clean it up at compile time.
		}
		else
#endif
		{
			return ::fast_io::details::allocator_pointer_aligned_impl<alloc, true>(alignment, n);
		}
	}
	static inline constexpr bool has_native_allocate_at_least{
		!has_status && (::fast_io::details::has_allocate_at_least_impl<alloc> ||
						::fast_io::details::has_allocate_aligned_at_least_impl<alloc> ||
						::fast_io::details::has_allocate_zero_at_least_impl<alloc> ||
						::fast_io::details::has_allocate_aligned_zero_at_least_impl<alloc>)};
	static inline constexpr allocation_least_result
	allocate_at_least(::std::size_t n) noexcept
		requires(!has_status)
	{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			return {::operator new(n), n};
		}
		else
#endif
		{
			if constexpr (::fast_io::details::has_allocate_at_least_impl<alloc>)
			{
				return allocator_type::allocate_at_least(n);
			}
			else if constexpr (::fast_io::details::has_allocate_aligned_at_least_impl<alloc>)
			{
				return allocator_type::allocate_aligned_at_least(n, default_alignment);
			}
			else if constexpr (::fast_io::details::has_allocate_zero_at_least_impl<alloc>)
			{
				return allocator_type::allocate_zero_at_least(n);
			}
			else if constexpr (::fast_io::details::has_allocate_aligned_zero_at_least_impl<alloc>)
			{
				return allocator_type::allocate_aligned_zero_at_least(n, default_alignment);
			}
			else
			{
				return {generic_allocator_adapter::allocate(n), n};
			}
		}
	}
	static inline constexpr allocation_least_result
	allocate_zero_at_least(::std::size_t n) noexcept
		requires(!has_status)
	{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			return {::operator new(n), n};
		}
		else
#endif
		{
			if constexpr (::fast_io::details::has_allocate_zero_at_least_impl<alloc>)
			{
				return allocator_type::allocate_zero_at_least(n);
			}
			else if constexpr (::fast_io::details::has_allocate_aligned_zero_at_least_impl<alloc>)
			{
				return allocator_type::allocate_aligned_zero_at_least(n, default_alignment);
			}
			else if constexpr (::fast_io::details::has_allocate_at_least_impl<alloc> ||
							   ::fast_io::details::has_allocate_aligned_at_least_impl<alloc>)
			{
				auto temp{generic_allocator_adapter::allocate_at_least(n)};
				auto ptr{temp.ptr};
				auto sz{temp.count};
				::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(ptr), sz);
				return temp;
			}
			else
			{
				return {generic_allocator_adapter::allocate_zero(n), n};
			}
		}
	}

	static inline constexpr allocation_least_result
	allocate_aligned_at_least(::std::size_t alignment, ::std::size_t n) noexcept
		requires(!has_status)
	{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			return {::operator new(n), n};
		}
		else
#endif
		{
			if constexpr (::fast_io::details::has_allocate_aligned_at_least_impl<alloc>)
			{
				return allocator_type::allocate_aligned_at_least(alignment, n);
			}
			else if constexpr (::fast_io::details::has_allocate_aligned_zero_at_least_impl<alloc>)
			{
				return allocator_type::allocate_aligned_zero_at_least(alignment, n);
			}
			else if constexpr (::fast_io::details::has_allocate_at_least_impl<alloc> ||
							   ::fast_io::details::has_allocate_zero_at_least_impl<alloc>)
			{
				return ::fast_io::details::allocator_pointer_aligned_at_least_impl<alloc, false>(alignment, n);
			}
			else
			{
				return {generic_allocator_adapter::allocate_aligned(alignment, n), n};
			}
		}
	}

	static inline constexpr allocation_least_result
	allocate_aligned_zero_at_least(::std::size_t alignment, ::std::size_t n) noexcept
		requires(!has_status)
	{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			return {::operator new(n), n};
		}
		else
#endif
		{
			if constexpr (::fast_io::details::has_allocate_aligned_zero_at_least_impl<alloc>)
			{
				return allocator_type::allocate_aligned_zero_at_least(alignment, n);
			}
			else if constexpr (::fast_io::details::has_allocate_aligned_at_least_impl<alloc>)
			{
				auto temp{allocator_type::allocate_aligned_at_least(alignment, n)};
				::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(temp.ptr), temp.count);
				return temp;
			}
			else if constexpr (::fast_io::details::has_allocate_at_least_impl<alloc> ||
							   ::fast_io::details::has_allocate_zero_at_least_impl<alloc>)
			{
				return ::fast_io::details::allocator_pointer_aligned_at_least_impl<alloc, true>(alignment, n);
			}
			else
			{
				return {generic_allocator_adapter::allocate_aligned(alignment, n), n};
			}
		}
	}

	static inline constexpr bool has_reallocate_aligned = (::fast_io::details::has_reallocate_aligned_impl<alloc> ||
														   ::fast_io::details::has_reallocate_aligned_at_least_impl<alloc> ||
														   ::fast_io::details::has_reallocate_aligned_zero_impl<alloc> ||
														   ::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>);
	static inline

		void *
		reallocate_aligned(void *p, ::std::size_t alignment, ::std::size_t n) noexcept
		requires(!has_status && has_reallocate_aligned)
	{
		if constexpr (::fast_io::details::has_reallocate_aligned_impl<alloc>)
		{
			return allocator_type::reallocate_aligned(p, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_at_least(p, alignment, n).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero(p, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_at_least(p, alignment, n).ptr;
		}
	}
	static inline constexpr bool has_reallocate_aligned_zero = (::fast_io::details::has_reallocate_aligned_zero_impl<alloc> ||
																::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>);
	static inline

		void *
		reallocate_aligned_zero(void *p, ::std::size_t alignment, ::std::size_t n) noexcept
		requires(!has_status && has_reallocate_aligned_zero)
	{
		if constexpr (::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero(p, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero(p, alignment, n).ptr;
		}
	}
	static inline

		void *
		reallocate_aligned_n(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n) noexcept
		requires(!has_status)
	{
		if constexpr (::fast_io::details::has_reallocate_aligned_n_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_n(p, oldn, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_n_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_n_at_least(p, oldn, alignment, n).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_impl<alloc>)
		{
			return allocator_type::reallocate_aligned(p, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_at_least(p, alignment, n).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_n(p, oldn, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_n_at_least(p, oldn, alignment, n).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero(p, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_n(p, alignment, n).ptr;
		}
		else
		{
			if constexpr (::fast_io::details::has_reallocate_n_impl<alloc> ||
						  ::fast_io::details::has_reallocate_zero_n_impl<alloc> ||
						  ::fast_io::details::has_reallocate_impl<alloc> ||
						  ::fast_io::details::has_reallocate_zero_impl<alloc>)
			{
				if (alignment <= default_alignment)
				{
					return generic_allocator_adapter::reallocate_n(p, oldn, n);
				}
			}
			auto newptr{::fast_io::details::allocator_pointer_aligned_impl<alloc, false>(alignment, n)};
			if (p != nullptr && n)
			{
				if (oldn < n)
				{
					n = oldn;
				}
				::fast_io::freestanding::nonoverlapped_bytes_copy_n(reinterpret_cast<::std::byte const *>(p), n, reinterpret_cast<::std::byte *>(newptr));
				generic_allocator_adapter::deallocate_aligned_n(p, alignment, oldn);
			}
			return newptr;
		}
	}
	static inline

		void *
		reallocate_aligned_zero_n(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n) noexcept
		requires(!has_status)
	{
		if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_n(p, oldn, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_n_at_least(p, oldn, alignment, n).ptr;
		}
		else
		{
			auto newptr{generic_allocator_adapter::reallocate_aligned_n(p, oldn, alignment, n)};
			if (oldn < n)
			{
				::std::size_t const to_zero_bytes{static_cast<::std::size_t>(n - oldn)};
				::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(newptr) + oldn, to_zero_bytes);
			}
			return newptr;
		}
	}

	static inline constexpr bool has_native_reallocate_at_least = (has_reallocate &&
																   (::fast_io::details::has_reallocate_aligned_at_least_impl<alloc> ||
																	::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>));
	static inline

		::fast_io::allocation_least_result
		reallocate_at_least(void *p, ::std::size_t n) noexcept
		requires(!has_status && has_reallocate)
	{
		if constexpr (::fast_io::details::has_reallocate_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_at_least(p, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_at_least(p, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_zero_at_least(p, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_at_least(p, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_impl<alloc>)
		{
			return {allocator_type::reallocate(p, n), n};
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_impl<alloc>)
		{
			return {allocator_type::reallocate_aligned(p, default_alignment, n), n};
		}
		else if constexpr (::fast_io::details::has_reallocate_zero_impl<alloc>)
		{
			return {allocator_type::reallocate_zero(p, n), n};
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
		{
			return {allocator_type::reallocate_aligned_zero(p, default_alignment, n), n};
		}
	}

	static inline constexpr bool has_native_reallocate_zero_at_least = (has_reallocate_zero &&
																		(::fast_io::details::has_reallocate_zero_at_least_impl<alloc> ||
																		 ::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>));
	static inline

		::fast_io::allocation_least_result
		reallocate_zero_at_least(void *p, ::std::size_t n) noexcept
		requires(!has_status && has_reallocate)
	{
		if constexpr (::fast_io::details::has_reallocate_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_zero_at_least(p, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_at_least(p, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_zero_impl<alloc>)
		{
			return {allocator_type::reallocate_zero(p, n), n};
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_impl<alloc>)
		{
			return {allocator_type::reallocate_aligned(p, default_alignment, n), n};
		}
	}

	static inline

		::fast_io::allocation_least_result
		reallocate_n_at_least(void *p, ::std::size_t oldn, ::std::size_t n) noexcept
		requires(!has_status)
	{
		if constexpr (::fast_io::details::has_reallocate_n_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_n_at_least(p, oldn, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_n_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_n_at_least(p, oldn, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_zero_n_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_zero_n_at_least(p, oldn, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_n_at_least(p, oldn, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_at_least(p, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_at_least(p, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_zero_at_least(p, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_at_least(p, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_n_impl<alloc>)
		{
			return {allocator_type::reallocate_n(p, oldn, n), n};
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_n_impl<alloc>)
		{
			return {allocator_type::reallocate_aligned_n(p, oldn, default_alignment, n), n};
		}
		else if constexpr (::fast_io::details::has_reallocate_zero_n_impl<alloc>)
		{
			return {allocator_type::reallocate_zero_n(p, oldn, n), n};
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_impl<alloc>)
		{
			return {allocator_type::reallocate_aligned_zero_n(p, oldn, default_alignment, n), n};
		}
		else if constexpr (::fast_io::details::has_reallocate_impl<alloc>)
		{
			return {allocator_type::reallocate(p, n), n};
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_impl<alloc>)
		{
			return {allocator_type::reallocate_aligned(p, default_alignment, n), n};
		}
		else if constexpr (::fast_io::details::has_reallocate_zero_impl<alloc>)
		{
			return {allocator_type::reallocate_zero(p, n), n};
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
		{
			return {allocator_type::reallocate_aligned_zero(p, default_alignment, n), n};
		}
		else
		{
			auto newres{generic_allocator_adapter::allocate_at_least(n)};
			auto newptr{newres.ptr};
			if (p != nullptr && n)
			{
				if (oldn < n)
				{
					n = oldn;
				}
				::fast_io::freestanding::nonoverlapped_bytes_copy_n(reinterpret_cast<::std::byte const *>(p), n, reinterpret_cast<::std::byte *>(newptr));
				generic_allocator_adapter::deallocate_n(p, oldn);
			}
			return newres;
		}
	}

	static inline

		::fast_io::allocation_least_result
		reallocate_zero_n_at_least(void *p, ::std::size_t oldn, ::std::size_t n) noexcept
		requires(!has_status)
	{
		if constexpr (::fast_io::details::has_reallocate_zero_n_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_zero_n_at_least(p, oldn, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_n_at_least(p, oldn, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_zero_at_least(p, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_at_least(p, default_alignment, n);
		}
		else
		{
			auto newres{generic_allocator_adapter::reallocate_n_at_least(p, oldn, n)};
			auto newptr{newres.ptr};
			n = newres.count;
			if (oldn < n)
			{
				::std::size_t const to_zero_bytes{static_cast<::std::size_t>(n - oldn)};
				::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(newptr) + oldn, to_zero_bytes);
			}
			return newres;
		}
	}

	static inline constexpr bool has_native_reallocate_aligned_at_least = (has_reallocate_aligned &&
																		   (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc> ||
																			::fast_io::details::has_reallocate_aligned_at_least_impl<alloc>));

	static inline ::fast_io::allocation_least_result
	reallocate_aligned_at_least(void *p, ::std::size_t alignment, ::std::size_t n) noexcept
		requires(!has_status && has_reallocate_aligned_zero)
	{
		if constexpr (::fast_io::details::has_reallocate_aligned_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_at_least(p, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_at_least(p, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_impl<alloc>)
		{
			return {allocator_type::reallocate_aligned(p, alignment, n), n};
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
		{
			return {allocator_type::reallocate_aligned_zero(p, alignment, n), n};
		}
	}

	static inline constexpr bool has_native_reallocate_aligned_zero_at_least = (has_reallocate_aligned_zero &&
																				::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>);

	static inline ::fast_io::allocation_least_result
	reallocate_aligned_zero_at_least(void *p, ::std::size_t alignment, ::std::size_t n) noexcept
		requires(!has_status && has_reallocate_aligned_zero)
	{
		if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_at_least(p, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
		{
			return {allocator_type::reallocate_aligned_zero(p, alignment, n), n};
		}
	}

	static inline constexpr bool has_native_reallocate_aligned_n_at_least = (has_reallocate_aligned &&
																			 (::fast_io::details::has_reallocate_aligned_n_at_least_impl<alloc> ||
																			  ::fast_io::details::has_reallocate_aligned_zero_n_at_least_impl<alloc>));
	static inline ::fast_io::allocation_least_result
	reallocate_aligned_n_at_least(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n) noexcept
		requires(!has_status)
	{
		if constexpr (::fast_io::details::has_reallocate_aligned_n_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_n_at_least(p, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_at_least(p, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_n_at_least(p, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_at_least(p, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_n(p, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_impl<alloc>)
		{
			return allocator_type::reallocate_aligned(p, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_n(p, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero(p, alignment, n);
		}
		else
		{
			if constexpr (
				::fast_io::details::has_reallocate_at_least_impl<alloc> ||
				::fast_io::details::has_reallocate_zero_at_least_impl<alloc> ||
				::fast_io::details::has_reallocate_n_at_least_impl<alloc> ||
				::fast_io::details::has_reallocate_zero_n_at_least_impl<alloc>)
			{
				if (alignment <= default_alignment)
				{
					return generic_allocator_adapter::reallocate_n_at_least(p, oldn, n);
				}
			}
			auto newres{::fast_io::details::allocator_pointer_aligned_at_least_impl<alloc, false>(alignment, n)};
			auto newptr{newres.ptr};
			if (p != nullptr && n)
			{
				if (oldn < n)
				{
					n = oldn;
				}
				::fast_io::freestanding::nonoverlapped_bytes_copy_n(reinterpret_cast<::std::byte const *>(p), n, reinterpret_cast<::std::byte *>(newptr));
				generic_allocator_adapter::deallocate_aligned_n(p, alignment, oldn);
			}
			return newres;
		}
	}

	static inline constexpr bool has_native_reallocate_aligned_zero_n_at_least = (has_reallocate_aligned_zero && ::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>);

	static inline ::fast_io::allocation_least_result reallocate_aligned_zero_n_at_least(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n) noexcept
		requires(!has_status)
	{
		if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_n_at_least(p, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_impl<alloc>)
		{
			return {allocator_type::reallocate_aligned_zero_n(p, alignment, n), n};
		}
		else
		{
			auto newres = generic_allocator_adapter::reallocate_aligned_n_at_least(p, oldn, alignment, n);
			auto newptr{newres.ptr};
			n = newres.count;
			if (oldn < n)
			{
				::std::size_t const to_zero_bytes{static_cast<::std::size_t>(n - oldn)};
				::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(newptr) + oldn, to_zero_bytes);
			}
			return newres;
		}
	}

	static inline constexpr bool has_deallocate_aligned = (::fast_io::details::has_deallocate_aligned_impl<alloc> ||
														   ::fast_io::details::has_deallocate_impl<alloc>);
	static inline void deallocate_aligned(void *p, ::std::size_t alignment) noexcept
		requires(!has_status && has_deallocate_aligned)
	{
		if constexpr (::fast_io::details::has_deallocate_aligned_impl<alloc>)
		{
			allocator_type::deallocate_aligned(p, alignment);
		}
		else
		{
			if (p == nullptr)
			{
				return;
			}
			if (default_alignment < alignment)
			{
				p = reinterpret_cast<void **>(p)[-1];
			}
			allocator_type::deallocate(p);
		}
	}
	static inline void deallocate_aligned_n(void *p, ::std::size_t alignment, ::std::size_t n) noexcept
		requires(!has_status)
	{
		if constexpr (::fast_io::details::has_deallocate_aligned_n_impl<alloc>)
		{
			allocator_type::deallocate_aligned_n(p, alignment, n);
		}
		else if constexpr (::fast_io::details::has_deallocate_aligned_impl<alloc>)
		{
			allocator_type::deallocate_aligned(p, alignment);
		}
		else
		{
			if (p == nullptr)
			{
				return;
			}
			if (default_alignment < alignment)
			{
				auto start{reinterpret_cast<void **>(p)[-1]};
				n += static_cast<::std::size_t>(reinterpret_cast<char unsigned *>(p) - reinterpret_cast<char unsigned *>(start));
				p = start;
			}
			if constexpr (::fast_io::details::has_deallocate_impl<alloc>)
			{
				allocator_type::deallocate(p);
			}
			else
			{
				allocator_type::deallocate_n(p, n);
			}
		}
	}
};

template <typename alloc, typename T>
class typed_generic_allocator_adapter
{
public:
	using allocator_adaptor = alloc;
	static inline constexpr bool has_status{allocator_adaptor::has_status};
	using handle_type = typename allocator_adaptor::handle_type;
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif

		T *
		allocate(::std::size_t n) noexcept
		requires(!has_status)
	{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			return ::fast_io::freestanding::allocator<T>{}.allocate(n);
		}
#endif
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::allocate(n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::allocate_aligned(n * sizeof(T), alignof(T)));
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif

		basic_allocation_least_result<T *>
		allocate_at_least(::std::size_t n) noexcept
		requires(!has_status)
	{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			return {::fast_io::freestanding::allocator<T>{}.allocate(n), n};
		}
#endif
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			auto newres{alloc::allocate_at_least(n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
		else
		{
			auto newres{alloc::allocate_aligned_at_least(n * sizeof(T), alignof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif

		T *
		allocate_zero(::std::size_t n) noexcept
		requires(!has_status)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::allocate_zero(n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::allocate_zero_aligned(n * sizeof(T), alignof(T)));
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif

		basic_allocation_least_result<T *>
		allocate_zero_at_least(::std::size_t n) noexcept
		requires(!has_status)
	{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			return ::fast_io::freestanding::allocator<T>{}.allocate(n);
		}
#endif
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			auto newres{alloc::allocate_zero_at_least(n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
		else
		{
			auto newres{alloc::allocate_zero_aligned_at_least(n * sizeof(T), alignof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
	}

	static inline constexpr bool has_reallocate = allocator_adaptor::has_reallocate;

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif

		T *
		reallocate(T *ptr, ::std::size_t n) noexcept
		requires(!has_status && has_reallocate)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::reallocate(ptr, n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::reallocate_aligned(ptr, n * sizeof(T), alignof(T)));
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif

		basic_allocation_least_result<T *>
		reallocate_at_least(T *ptr, ::std::size_t n) noexcept
		requires(!has_status && has_reallocate)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			auto newres{alloc::reallocate_zero_at_least(ptr, n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
		else
		{
			auto newres{alloc::reallocate_aligned_zero_at_least(ptr, n * sizeof(T), alignof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
	}

	static inline constexpr bool has_reallocate_zero = allocator_adaptor::has_reallocate_zero;
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif

		T *
		reallocate_zero(T *ptr, ::std::size_t n) noexcept
		requires(!has_status && has_reallocate_zero)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::reallocate_zero(ptr, n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::reallocate_aligned_zero(ptr, n * sizeof(T), alignof(T)));
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif

		basic_allocation_least_result<T *>
		reallocate_zero_at_least(T *ptr, ::std::size_t n) noexcept
		requires(!has_status && has_reallocate_zero)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			auto newres{alloc::reallocate_zero_at_least(ptr, n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
		else
		{
			auto newres{alloc::reallocate_aligned_zero_at_least(ptr, alignof(T), n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif

		T *
		reallocate_n(T *ptr, ::std::size_t oldn, ::std::size_t n) noexcept
		requires(!has_status)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::reallocate_n(ptr, oldn * sizeof(T), n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::reallocate_aligned_n(ptr, oldn * sizeof(T), alignof(T), n * sizeof(T)));
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif

		basic_allocation_least_result<T *>
		reallocate_n_at_least(T *ptr, ::std::size_t oldn, ::std::size_t n) noexcept
		requires(!has_status)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			auto newres{alloc::reallocate_n_at_least(ptr, oldn * sizeof(T), n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
		else
		{
			auto newres{alloc::reallocate_aligned_n_at_least(ptr, oldn * sizeof(T), alignof(T), n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif

		T *
		reallocate_zero_n(T *ptr, ::std::size_t oldn, ::std::size_t n) noexcept
		requires(!has_status)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::reallocate_zero_n(ptr, oldn * sizeof(T), n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::reallocate_aligned_zero_n(ptr, oldn * sizeof(T), alignof(T), n * sizeof(T)));
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif

		basic_allocation_least_result<T *>
		reallocate_zero_n_at_least(T *ptr, ::std::size_t oldn, ::std::size_t n) noexcept
		requires(!has_status)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			auto newres{alloc::reallocate_zero_n_at_least(ptr, oldn * sizeof(T), n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
		else
		{
			auto newres{alloc::reallocate_aligned_zero_n_at_least(ptr, oldn * sizeof(T), alignof(T), n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
	}

	static inline constexpr bool has_deallocate = allocator_adaptor::has_deallocate;
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
		void
		deallocate(T *ptr) noexcept
		requires(!has_status && has_deallocate)
	{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			if (ptr)
			{
				return ::fast_io::freestanding::allocator<T>{}.deallocate(ptr, 1);
			}
			else
			{
				return;
			}
		}
#endif
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return alloc::deallocate(ptr);
		}
		else
		{
			return alloc::deallocate_aligned(ptr, alignof(T));
		}
	}
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
		void
		deallocate_n(T *ptr, ::std::size_t n) noexcept
		requires(!has_status)
	{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			if (ptr)
			{
				return ::fast_io::freestanding::allocator<T>{}.deallocate(ptr, n);
			}
			else
			{
				return;
			}
		}
#endif
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			alloc::deallocate_n(ptr, n * sizeof(T));
		}
		else
		{
			alloc::deallocate_aligned_n(ptr, alignof(T), n * sizeof(T));
		}
	}
#if 0
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
 

		T *
		handle_allocate(handle_type handle, ::std::size_t n) noexcept
		requires(has_status)
	{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			if (n)
			{
				return ::fast_io::freestanding::allocator<T>{}.allocate(n);
			}
			else
			{
				return nullptr;
			}
		}
#endif
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::handle_allocate(handle, n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::handle_allocate_aligned(handle, n * sizeof(T), alignof(T)));
		}
	}
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
 

		T *
		handle_allocate_zero(handle_type handle, ::std::size_t n) noexcept
		requires(has_status)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::handle_allocate_zero(handle, n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::handle_allocate_zero_aligned(handle, n * sizeof(T), alignof(T)));
		}
	}

	static inline constexpr bool has_handle_reallocate = allocator_adaptor::has_handle_reallocate;
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
 

		T *
		handle_reallocate(handle_type handle, T *ptr, ::std::size_t n) noexcept
		requires(has_status && has_handle_reallocate)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::handle_reallocate(handle, ptr, n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::handle_reallocate_aligned(handle, ptr, n * sizeof(T), alignof(T)));
		}
	}
	static inline constexpr bool has_handle_reallocate_zero = allocator_adaptor::has_handle_reallocate_zero;
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
 

		T *
		handle_reallocate_zero(handle_type handle, T *ptr, ::std::size_t n) noexcept
		requires(has_status && has_handle_reallocate)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::handle_reallocate_zero(handle, ptr, n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::handle_reallocate_aligned_zero(handle, ptr, n * sizeof(T), alignof(T)));
		}
	}
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
 

		T *
		handle_reallocate_n(handle_type handle, T *ptr, ::std::size_t oldn, ::std::size_t n) noexcept
		requires(has_status)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::handle_reallocate_n(handle, ptr, oldn * sizeof(T), n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::handle_reallocate_aligned_n(handle, ptr, oldn * sizeof(T), alignof(T), n * sizeof(T)));
		}
	}
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
 

		T *
		handle_reallocate_zero_n(handle_type handle, T *ptr, ::std::size_t oldn, ::std::size_t n) noexcept
		requires(has_status)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::handle_reallocate_zero_n(handle, ptr, oldn * sizeof(T), n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::handle_reallocate_aligned_zero_n(handle, ptr * sizeof(T), oldn, n * sizeof(T), alignof(T)));
		}
	}

	static inline constexpr bool has_handle_deallocate = allocator_adaptor::has_handle_deallocate;
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
		void
		handle_deallocate(handle_type handle, T *ptr) noexcept
		requires(has_status && has_handle_deallocate)
	{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			if (ptr)
			{
				return ::fast_io::freestanding::allocator<T>{}.deallocate(ptr, 1);
			}
			else
			{
				return;
			}
		}
#endif
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return alloc::handle_deallocate(handle, ptr);
		}
		else
		{
			return alloc::handle_deallocate_aligned(handle, ptr, alignof(T));
		}
	}
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
		void
		handle_deallocate_n(handle_type handle, T *ptr, ::std::size_t n) noexcept
		requires(has_status)
	{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			if (ptr)
			{
				return ::fast_io::freestanding::allocator<T>{}.deallocate(ptr, n);
			}
			else
			{
				return;
			}
		}
#endif
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			alloc::handle_deallocate_n(handle, ptr, n * sizeof(T));
		}
		else
		{
			alloc::handle_deallocate_aligned_n(handle, ptr, alignof(T), n * sizeof(T));
		}
	}
#endif
};

namespace details
{

template <typename alloc, bool zero>
inline constexpr void *allocator_pointer_aligned_impl(::std::size_t alignment, ::std::size_t n) noexcept
{
	if constexpr (!::fast_io::generic_allocator_adapter<alloc>::has_native_allocate)
	{
		::fast_io::fast_terminate();
	}
	else
	{
		constexpr ::std::size_t defaultalignment{::fast_io::details::calculate_default_alignment<alloc>()};
		bool const alignedadjustment{defaultalignment < alignment};
		if (alignedadjustment)
		{
			n = ::fast_io::details::allocator_compute_aligned_total_size_impl(alignment, n);
		}
		void *p;
		if constexpr (zero)
		{
			p = ::fast_io::generic_allocator_adapter<alloc>::allocate_zero(n);
		}
		else
		{
			p = ::fast_io::generic_allocator_adapter<alloc>::allocate(n);
		}
		if (alignedadjustment)
		{
			p = ::fast_io::details::allocator_adjust_ptr_to_aligned_impl(p, alignment);
		}
		return p;
	}
}

template <typename alloc, bool zero>
inline constexpr ::fast_io::allocation_least_result allocator_pointer_aligned_at_least_impl(::std::size_t alignment, ::std::size_t n) noexcept
{
	if constexpr (!::fast_io::generic_allocator_adapter<alloc>::has_native_allocate)
	{
		::fast_io::fast_terminate();
	}
	else
	{
		constexpr ::std::size_t defaultalignment{::fast_io::details::calculate_default_alignment<alloc>()};
		bool const alignedadjustment{defaultalignment < alignment};
		if (alignedadjustment)
		{
			n = ::fast_io::details::allocator_compute_aligned_total_size_impl(alignment, n);
		}
		::fast_io::allocation_least_result res;
		if constexpr (zero)
		{
			res = ::fast_io::generic_allocator_adapter<alloc>::allocate_zero_at_least(n);
		}
		else
		{
			res = ::fast_io::generic_allocator_adapter<alloc>::allocate_at_least(n);
		}
		if (alignedadjustment)
		{
			auto resptr{res.ptr};
			auto aligned_ptr = ::fast_io::details::allocator_adjust_ptr_to_aligned_impl(resptr, alignment);
			res = {aligned_ptr, res.count - static_cast<::std::size_t>(reinterpret_cast<char unsigned *>(aligned_ptr) - reinterpret_cast<char unsigned *>(resptr))};
		}
		return res;
	}
}
#if 0
template <typename alloc, bool zero>
inline constexpr void *status_allocator_pointer_aligned_impl(typename alloc::handle_type handle, ::std::size_t alignment, ::std::size_t n) noexcept
{
	if constexpr (!::fast_io::generic_allocator_adapter<alloc>::has_native_handle_allocate)
	{
		::fast_io::fast_terminate();
	}
	else
	{
		constexpr ::std::size_t defaultalignment{::fast_io::details::calculate_default_alignment<alloc>()};
		bool const alignedadjustment{defaultalignment < alignment};
		if (alignedadjustment)
		{
			n = ::fast_io::details::allocator_compute_aligned_total_size_impl(alignment, n);
		}
		void *p;
		if constexpr (zero)
		{
			p = ::fast_io::generic_allocator_adapter<alloc>::handle_allocate_zero(handle, n);
		}
		else
		{
			p = ::fast_io::generic_allocator_adapter<alloc>::handle_allocate(handle, n);
		}
		if (alignedadjustment)
		{
			p = ::fast_io::details::allocator_adjust_ptr_to_aligned_impl(p,alignment);
		}
		return p;
	}
}

template <typename alloc, bool zero>
inline constexpr ::fast_io::allocation_least_result status_allocator_pointer_aligned_impl(typename alloc::handle_type handle, ::std::size_t alignment, ::std::size_t n) noexcept
{
	if constexpr (!::fast_io::generic_allocator_adapter<alloc>::has_native_handle_allocate)
	{
		::fast_io::fast_terminate();
	}
	else
	{
		constexpr ::std::size_t defaultalignment{::fast_io::details::calculate_default_alignment<alloc>()};
		bool const alignedadjustment{defaultalignment < alignment};
		if (alignedadjustment)
		{
			n = ::fast_io::details::allocator_compute_aligned_total_size_impl(alignment, n);
		}
		::fast_io::allocation_least_result res;
		if constexpr (zero)
		{
			res = ::fast_io::generic_allocator_adapter<alloc>::handle_allocate_zero_at_least(handle, n);
		}
		else
		{
			res = ::fast_io::generic_allocator_adapter<alloc>::handle_allocate_at_least(handle, n);
		}
		if (alignedadjustment)
		{
			auto resptr{res.ptr};
			auto aligned_ptr = ::fast_io::details::allocator_adjust_ptr_to_aligned_impl(resptr,alignment);
			res = {aligned_ptr,res.count-static_cast<::std::size_t>(reinterpret_cast<char unsigned*>(aligned_ptr)-reinterpret_cast<char unsigned*>(resptr))};
		}
		return res;
	}
}
#endif
} // namespace details

} // namespace fast_io

#pragma pop_macro("new")
