#pragma once

template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *handle_allocate_impl(handle_type handle, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(has_status && has_native_handle_allocate)
{
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L && __cpp_constexpr_dynamic_alloc >= 201907L
	if (__builtin_is_constant_evaluated())
#else
	if (false)
#endif
	{
		return static_cast<void *>(::fast_io::freestanding::allocator<::std::byte>{}.allocate(n));
	}
	else
	{
		if constexpr (::fast_io::details::has_handle_allocate_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::handle_allocate(handle, n);
		}
		else if constexpr (::fast_io::details::has_handle_allocate_at_least_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::handle_allocate_at_least(handle, n).ptr;
		}
		else
		{
			return handle_allocate_conditional_zero_impl<throwing>(handle, n, false);
		}
	}
}
static inline constexpr void *handle_allocate(handle_type handle, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (has_status && has_native_handle_allocate)) || (throws_on_allocation_failure && (has_status && has_native_handle_allocate_try)))
{
	return handle_allocate_impl<throws_on_allocation_failure>(handle, n);
}
static inline constexpr void *handle_allocate_die(handle_type handle, ::std::size_t n) noexcept
	requires(has_status && has_native_handle_allocate)
{
	return handle_allocate_impl<false>(handle, n);
}
static inline constexpr void *handle_allocate_try(handle_type handle, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(has_status && has_native_handle_allocate_try)
{
	return handle_allocate_impl<true>(handle, n);
}


template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *handle_allocate_zero_impl(handle_type handle, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(has_status && has_native_handle_allocate)
{
	if constexpr (::fast_io::details::has_handle_allocate_zero_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::handle_allocate_zero(handle, n);
	}
	else if constexpr (::fast_io::details::has_handle_allocate_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::handle_allocate_zero_at_least(handle, n).ptr;
	}
	else
	{
		return handle_allocate_conditional_zero_impl<throwing>(handle, n, true);
	}
}
static inline constexpr void *handle_allocate_zero(handle_type handle, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (has_status && has_native_handle_allocate)) || (throws_on_allocation_failure && (has_status && has_native_handle_allocate_try)))
{
	return handle_allocate_zero_impl<throws_on_allocation_failure>(handle, n);
}
static inline constexpr void *handle_allocate_zero_die(handle_type handle, ::std::size_t n) noexcept
	requires(has_status && has_native_handle_allocate)
{
	return handle_allocate_zero_impl<false>(handle, n);
}
static inline constexpr void *handle_allocate_zero_try(handle_type handle, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(has_status && has_native_handle_allocate_try)
{
	return handle_allocate_zero_impl<true>(handle, n);
}


static inline constexpr bool has_handle_reallocate{
	has_status && (::fast_io::details::has_handle_reallocate_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_zero_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_zero_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_zero_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_zero_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_conditional_zero_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_conditional_zero_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_conditional_zero_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_conditional_zero_at_least_impl<alloc>)};
static inline constexpr bool has_handle_reallocate_try{
	has_status && (::fast_io::details::has_handle_reallocate_try_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_try_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_zero_try_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_zero_try_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_at_least_try_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_at_least_try_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_zero_at_least_try_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_zero_at_least_try_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_conditional_zero_try_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_conditional_zero_try_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_conditional_zero_at_least_try_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_conditional_zero_at_least_try_impl<alloc>)};

template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *handle_reallocate_conditional_zero_impl(handle_type handle, void *p, ::std::size_t n, bool zero)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(has_status && has_handle_reallocate)
{
	if constexpr (::fast_io::details::has_handle_reallocate_conditional_zero_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::handle_reallocate_conditional_zero(handle, p, n, zero);
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_conditional_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::handle_reallocate_conditional_zero_at_least(handle, p, n, zero).ptr;
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_aligned_conditional_zero_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::handle_reallocate_aligned_conditional_zero(handle, p, default_alignment, n, zero);
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_aligned_conditional_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::handle_reallocate_aligned_conditional_zero_at_least(handle, p, default_alignment, n, zero).ptr;
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_mode_impl<alloc, throwing> ||
					   ::fast_io::details::has_handle_reallocate_at_least_mode_impl<alloc, throwing> ||
					   ::fast_io::details::has_handle_reallocate_aligned_mode_impl<alloc, throwing> ||
					   ::fast_io::details::has_handle_reallocate_aligned_at_least_mode_impl<alloc, throwing>)
	{
		if (zero)
		{
			if constexpr (::fast_io::details::has_handle_reallocate_zero_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::handle_reallocate_zero(handle, p, n);
			}
			else if constexpr (::fast_io::details::has_handle_reallocate_zero_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::handle_reallocate_zero_at_least(handle, p, n).ptr;
			}
			else if constexpr (::fast_io::details::has_handle_reallocate_aligned_zero_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::handle_reallocate_aligned_zero(handle, p, default_alignment, n);
			}
			else if constexpr (::fast_io::details::has_handle_reallocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::handle_reallocate_aligned_zero_at_least(handle, p, default_alignment, n).ptr;
			}
			else
			{
				::fast_io::fast_terminate();
			}
		}
		else
		{
			if constexpr (::fast_io::details::has_handle_reallocate_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::handle_reallocate(handle, p, n);
			}
			else if constexpr (::fast_io::details::has_handle_reallocate_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::handle_reallocate_at_least(handle, p, n).ptr;
			}
			else if constexpr (::fast_io::details::has_handle_reallocate_aligned_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::handle_reallocate_aligned(handle, p, default_alignment, n);
			}
			else
			{
				return dispatch<throwing>::handle_reallocate_aligned_at_least(handle, p, default_alignment, n).ptr;
			}
		}
	}
	else
	{
		if constexpr (::fast_io::details::has_handle_reallocate_zero_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::handle_reallocate_zero(handle, p, n);
		}
		else if constexpr (::fast_io::details::has_handle_reallocate_zero_at_least_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::handle_reallocate_zero_at_least(handle, p, n).ptr;
		}
		else if constexpr (::fast_io::details::has_handle_reallocate_aligned_zero_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::handle_reallocate_aligned_zero(handle, p, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_handle_reallocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::handle_reallocate_aligned_zero_at_least(handle, p, default_alignment, n).ptr;
		}
		else
		{
			::fast_io::fast_terminate();
		}
	}
}
static inline constexpr void *handle_reallocate_conditional_zero(handle_type handle, void *p, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (has_status && has_handle_reallocate)) || (throws_on_allocation_failure && (has_status && has_handle_reallocate_try)))
{
	return handle_reallocate_conditional_zero_impl<throws_on_allocation_failure>(handle, p, n, zero);
}
static inline constexpr void *handle_reallocate_conditional_zero_die(handle_type handle, void *p, ::std::size_t n, bool zero) noexcept
	requires(has_status && has_handle_reallocate)
{
	return handle_reallocate_conditional_zero_impl<false>(handle, p, n, zero);
}
static inline constexpr void *handle_reallocate_conditional_zero_try(handle_type handle, void *p, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS
	requires(has_status && has_handle_reallocate_try)
{
	return handle_reallocate_conditional_zero_impl<true>(handle, p, n, zero);
}


template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *handle_reallocate_impl(handle_type handle, void *p, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(has_status && has_handle_reallocate)
{
	if constexpr (::fast_io::details::has_handle_reallocate_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::handle_reallocate(handle, p, n);
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::handle_reallocate_at_least(handle, p, n).ptr;
	}
	else
	{
		return handle_reallocate_conditional_zero_impl<throwing>(handle, p, n, false);
	}
}
static inline constexpr void *handle_reallocate(handle_type handle, void *p, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (has_status && has_handle_reallocate)) || (throws_on_allocation_failure && (has_status && has_handle_reallocate_try)))
{
	return handle_reallocate_impl<throws_on_allocation_failure>(handle, p, n);
}
static inline constexpr void *handle_reallocate_die(handle_type handle, void *p, ::std::size_t n) noexcept
	requires(has_status && has_handle_reallocate)
{
	return handle_reallocate_impl<false>(handle, p, n);
}
static inline constexpr void *handle_reallocate_try(handle_type handle, void *p, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(has_status && has_handle_reallocate_try)
{
	return handle_reallocate_impl<true>(handle, p, n);
}


template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *handle_reallocate_zero_impl(handle_type handle, void *p, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(has_status && has_handle_reallocate)
{
	if constexpr (::fast_io::details::has_handle_reallocate_zero_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::handle_reallocate_zero(handle, p, n);
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::handle_reallocate_zero_at_least(handle, p, n).ptr;
	}
	else
	{
		return handle_reallocate_conditional_zero_impl<throwing>(handle, p, n, true);
	}
}
static inline constexpr void *handle_reallocate_zero(handle_type handle, void *p, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (has_status && has_handle_reallocate)) || (throws_on_allocation_failure && (has_status && has_handle_reallocate_try)))
{
	return handle_reallocate_zero_impl<throws_on_allocation_failure>(handle, p, n);
}
static inline constexpr void *handle_reallocate_zero_die(handle_type handle, void *p, ::std::size_t n) noexcept
	requires(has_status && has_handle_reallocate)
{
	return handle_reallocate_zero_impl<false>(handle, p, n);
}
static inline constexpr void *handle_reallocate_zero_try(handle_type handle, void *p, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(has_status && has_handle_reallocate_try)
{
	return handle_reallocate_zero_impl<true>(handle, p, n);
}


static inline constexpr bool has_handle_reallocate_n{
	has_status && (::fast_io::details::has_handle_reallocate_n_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_n_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_zero_n_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_zero_n_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_n_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_n_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_zero_n_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_zero_n_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_n_conditional_zero_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_n_conditional_zero_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_n_conditional_zero_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_n_conditional_zero_at_least_impl<alloc>)};
static inline constexpr bool has_handle_reallocate_n_try{
	has_status && (::fast_io::details::has_handle_reallocate_n_try_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_n_try_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_zero_n_try_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_zero_n_try_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_n_at_least_try_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_n_at_least_try_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_zero_n_at_least_try_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_zero_n_at_least_try_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_n_conditional_zero_try_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_n_conditional_zero_try_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_n_conditional_zero_at_least_try_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_n_conditional_zero_at_least_try_impl<alloc>)};

template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *handle_reallocate_n_conditional_zero_impl(handle_type handle, void *p, ::std::size_t oldn, ::std::size_t n, bool zero)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(has_status && has_handle_reallocate_n)
{
	if (p != nullptr && oldn == n)
	{
		return p;
	}
	if constexpr (::fast_io::details::has_handle_reallocate_n_conditional_zero_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::handle_reallocate_n_conditional_zero(handle, p, oldn, n, zero);
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_n_conditional_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::handle_reallocate_n_conditional_zero_at_least(handle, p, oldn, n, zero).ptr;
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_aligned_n_conditional_zero_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::handle_reallocate_aligned_n_conditional_zero(handle, p, oldn, default_alignment, n, zero);
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_aligned_n_conditional_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::handle_reallocate_aligned_n_conditional_zero_at_least(handle, p, oldn, default_alignment, n, zero).ptr;
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_n_mode_impl<alloc, throwing> ||
					   ::fast_io::details::has_handle_reallocate_n_at_least_mode_impl<alloc, throwing> ||
					   ::fast_io::details::has_handle_reallocate_aligned_n_mode_impl<alloc, throwing> ||
					   ::fast_io::details::has_handle_reallocate_aligned_n_at_least_mode_impl<alloc, throwing>)
	{
		if (zero)
		{
			if constexpr (::fast_io::details::has_handle_reallocate_zero_n_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::handle_reallocate_zero_n(handle, p, oldn, n);
			}
			else if constexpr (::fast_io::details::has_handle_reallocate_zero_n_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::handle_reallocate_zero_n_at_least(handle, p, oldn, n).ptr;
			}
			else if constexpr (::fast_io::details::has_handle_reallocate_aligned_zero_n_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::handle_reallocate_aligned_zero_n(handle, p, oldn, default_alignment, n);
			}
			else if constexpr (::fast_io::details::has_handle_reallocate_aligned_zero_n_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::handle_reallocate_aligned_zero_n_at_least(handle, p, oldn, default_alignment, n).ptr;
			}
			else
			{
				auto newptr{handle_reallocate_n_impl<throwing>(handle, p, oldn, n)};
				if (oldn < n)
				{
					::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(newptr) + oldn, n - oldn);
				}
				return newptr;
			}
		}
		else
		{
			if constexpr (::fast_io::details::has_handle_reallocate_n_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::handle_reallocate_n(handle, p, oldn, n);
			}
			else if constexpr (::fast_io::details::has_handle_reallocate_n_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::handle_reallocate_n_at_least(handle, p, oldn, n).ptr;
			}
			else if constexpr (::fast_io::details::has_handle_reallocate_aligned_n_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::handle_reallocate_aligned_n(handle, p, oldn, default_alignment, n);
			}
			else
			{
				return dispatch<throwing>::handle_reallocate_aligned_n_at_least(handle, p, oldn, default_alignment, n).ptr;
			}
		}
	}
	else
	{
		if constexpr (::fast_io::details::has_handle_reallocate_zero_n_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::handle_reallocate_zero_n(handle, p, oldn, n);
		}
		else if constexpr (::fast_io::details::has_handle_reallocate_zero_n_at_least_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::handle_reallocate_zero_n_at_least(handle, p, oldn, n).ptr;
		}
		else if constexpr (::fast_io::details::has_handle_reallocate_aligned_zero_n_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::handle_reallocate_aligned_zero_n(handle, p, oldn, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_handle_reallocate_aligned_zero_n_at_least_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::handle_reallocate_aligned_zero_n_at_least(handle, p, oldn, default_alignment, n).ptr;
		}
		else
		{
			auto newptr{handle_allocate_impl<throwing>(handle, n)};
			if (p != nullptr)
			{
				if (n)
				{
					::std::size_t copyn{oldn < n ? oldn : n};
					::fast_io::freestanding::nonoverlapped_bytes_copy_n(reinterpret_cast<::std::byte const *>(p), copyn, reinterpret_cast<::std::byte *>(newptr));
				}
				generic_allocator_adapter::handle_deallocate_n(handle, p, oldn);
			}
			if (zero && oldn < n)
			{
				::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(newptr) + oldn, n - oldn);
			}
			return newptr;
		}
	}
}
static inline constexpr void *handle_reallocate_n_conditional_zero(handle_type handle, void *p, ::std::size_t oldn, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (has_status && has_handle_reallocate_n)) || (throws_on_allocation_failure && (has_status && has_handle_reallocate_n_try)))
{
	return handle_reallocate_n_conditional_zero_impl<throws_on_allocation_failure>(handle, p, oldn, n, zero);
}
static inline constexpr void *handle_reallocate_n_conditional_zero_die(handle_type handle, void *p, ::std::size_t oldn, ::std::size_t n, bool zero) noexcept
	requires(has_status && has_handle_reallocate_n)
{
	return handle_reallocate_n_conditional_zero_impl<false>(handle, p, oldn, n, zero);
}
static inline constexpr void *handle_reallocate_n_conditional_zero_try(handle_type handle, void *p, ::std::size_t oldn, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS
	requires(has_status && has_handle_reallocate_n_try)
{
	return handle_reallocate_n_conditional_zero_impl<true>(handle, p, oldn, n, zero);
}


template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *handle_reallocate_n_impl(handle_type handle, void *p, ::std::size_t oldn, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(has_status && has_handle_reallocate_n)
{
	if (p != nullptr && oldn == n)
	{
		return p;
	}
	if constexpr (::fast_io::details::has_handle_reallocate_n_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::handle_reallocate_n(handle, p, oldn, n);
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_n_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::handle_reallocate_n_at_least(handle, p, oldn, n).ptr;
	}
	else
	{
		return handle_reallocate_n_conditional_zero_impl<throwing>(handle, p, oldn, n, false);
	}
}
static inline constexpr void *handle_reallocate_n(handle_type handle, void *p, ::std::size_t oldn, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (has_status && has_handle_reallocate_n)) || (throws_on_allocation_failure && (has_status && has_handle_reallocate_n_try)))
{
	return handle_reallocate_n_impl<throws_on_allocation_failure>(handle, p, oldn, n);
}
static inline constexpr void *handle_reallocate_n_die(handle_type handle, void *p, ::std::size_t oldn, ::std::size_t n) noexcept
	requires(has_status && has_handle_reallocate_n)
{
	return handle_reallocate_n_impl<false>(handle, p, oldn, n);
}
static inline constexpr void *handle_reallocate_n_try(handle_type handle, void *p, ::std::size_t oldn, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(has_status && has_handle_reallocate_n_try)
{
	return handle_reallocate_n_impl<true>(handle, p, oldn, n);
}


template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *handle_reallocate_zero_n_impl(handle_type handle, void *p, ::std::size_t oldn, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(has_status && has_handle_reallocate_n)
{
	if (p != nullptr && oldn == n)
	{
		return p;
	}
	if constexpr (::fast_io::details::has_handle_reallocate_zero_n_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::handle_reallocate_zero_n(handle, p, oldn, n);
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_zero_n_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::handle_reallocate_zero_n_at_least(handle, p, oldn, n).ptr;
	}
	else
	{
		return handle_reallocate_n_conditional_zero_impl<throwing>(handle, p, oldn, n, true);
	}
}
static inline constexpr void *handle_reallocate_zero_n(handle_type handle, void *p, ::std::size_t oldn, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (has_status && has_handle_reallocate_n)) || (throws_on_allocation_failure && (has_status && has_handle_reallocate_n_try)))
{
	return handle_reallocate_zero_n_impl<throws_on_allocation_failure>(handle, p, oldn, n);
}
static inline constexpr void *handle_reallocate_zero_n_die(handle_type handle, void *p, ::std::size_t oldn, ::std::size_t n) noexcept
	requires(has_status && has_handle_reallocate_n)
{
	return handle_reallocate_zero_n_impl<false>(handle, p, oldn, n);
}
static inline constexpr void *handle_reallocate_zero_n_try(handle_type handle, void *p, ::std::size_t oldn, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(has_status && has_handle_reallocate_n_try)
{
	return handle_reallocate_zero_n_impl<true>(handle, p, oldn, n);
}


static inline constexpr bool has_handle_deallocate{
	has_status && (::fast_io::details::has_handle_deallocate_impl<alloc> ||
				   ::fast_io::details::has_handle_deallocate_aligned_impl<alloc> ||
				   ::fast_io::details::has_handle_deallocate_n_impl<alloc> ||
				   ::fast_io::details::has_handle_deallocate_aligned_n_impl<alloc>)};

static inline void handle_deallocate(handle_type handle, void *p) noexcept
	requires(has_status && has_handle_deallocate && !secure_clear)
{
	if constexpr (::fast_io::details::has_handle_deallocate_impl<alloc>)
	{
		allocator_type::handle_deallocate(handle, p);
	}
	else if constexpr (::fast_io::details::has_handle_deallocate_aligned_impl<alloc>)
	{
		allocator_type::handle_deallocate_aligned(handle, p, default_alignment);
	}
	else if constexpr (::fast_io::details::has_handle_deallocate_n_impl<alloc>)
	{
		allocator_type::handle_deallocate_n(handle, p, 0);
	}
	else
	{
		allocator_type::handle_deallocate_aligned_n(handle, p, default_alignment, 0);
	}
}

static inline void handle_deallocate_n(handle_type handle, void *p, ::std::size_t n) noexcept
	requires(has_status && has_handle_deallocate)
{
	if constexpr (secure_clear)
	{
		if !consteval
		{
			if (p != nullptr)
			{
				::fast_io::freestanding::bytes_secure_clear_n(reinterpret_cast<::std::byte *>(p), n);
			}
		}
	}
	if constexpr (::fast_io::details::has_handle_deallocate_n_impl<alloc>)
	{
		allocator_type::handle_deallocate_n(handle, p, n);
	}
	else if constexpr (::fast_io::details::has_handle_deallocate_aligned_n_impl<alloc>)
	{
		allocator_type::handle_deallocate_aligned_n(handle, p, default_alignment, n);
	}
	else if constexpr (::fast_io::details::has_handle_deallocate_impl<alloc>)
	{
		allocator_type::handle_deallocate(handle, p);
	}
	else
	{
		allocator_type::handle_deallocate_aligned(handle, p, default_alignment);
	}
}

static inline void handle_deallocate_aligned(handle_type handle, void *p, ::std::size_t alignment) noexcept
	requires(has_status && has_handle_deallocate && !secure_clear)
{
	if constexpr (::fast_io::details::has_handle_deallocate_aligned_impl<alloc>)
	{
		allocator_type::handle_deallocate_aligned(handle, p, alignment);
	}
	else if constexpr (::fast_io::details::has_handle_deallocate_aligned_n_impl<alloc>)
	{
		allocator_type::handle_deallocate_aligned_n(handle, p, alignment, 0);
	}
	else if constexpr (::fast_io::details::has_handle_deallocate_n_impl<alloc>)
	{
		allocator_type::handle_deallocate_n(handle, p, 0);
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
		allocator_type::handle_deallocate(handle, p);
	}
}

static inline void handle_deallocate_aligned_n(handle_type handle, void *p, ::std::size_t alignment, ::std::size_t n) noexcept
	requires(has_status && has_handle_deallocate)
{
	if constexpr (secure_clear)
	{
		if !consteval
		{
			if (p != nullptr)
			{
				::fast_io::freestanding::bytes_secure_clear_n(reinterpret_cast<::std::byte *>(p), n);
			}
		}
	}
	if constexpr (::fast_io::details::has_handle_deallocate_aligned_n_impl<alloc>)
	{
		allocator_type::handle_deallocate_aligned_n(handle, p, alignment, n);
	}
	else if constexpr (::fast_io::details::has_handle_deallocate_aligned_impl<alloc>)
	{
		allocator_type::handle_deallocate_aligned(handle, p, alignment);
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
		if constexpr (::fast_io::details::has_handle_deallocate_n_impl<alloc>)
		{
			allocator_type::handle_deallocate_n(handle, p, n);
		}
		else if constexpr (::fast_io::details::has_handle_deallocate_impl<alloc>)
		{
			allocator_type::handle_deallocate(handle, p);
		}
		else
		{
			static_assert(::fast_io::details::has_handle_deallocate_n_impl<alloc>);
		}
	}
}
}
;
template <typename alloc, typename T,
		  ::fast_io::allocator_adapter_flags flags =
			  ::fast_io::details::adapter_flags_or_default<alloc>()>
class typed_generic_allocator_adapter
{
public:
	using allocator_adaptor = alloc;
	static inline constexpr ::fast_io::allocator_adapter_flags adapter_flags{flags};
	static inline constexpr bool throws_on_allocation_failure{
		(flags & ::fast_io::allocator_adapter_flags::throws_on_allocation_failure) != ::fast_io::allocator_adapter_flags::none};
	static inline constexpr bool throws_on_violations{
		(flags & ::fast_io::allocator_adapter_flags::throws_on_violations) != ::fast_io::allocator_adapter_flags::none};
	static inline constexpr bool throws_on_overflow{
		(flags & ::fast_io::allocator_adapter_flags::throws_on_overflow) != ::fast_io::allocator_adapter_flags::none};
	static inline constexpr bool prefer_big_allocation_on_stack{
		(flags & ::fast_io::allocator_adapter_flags::prefer_big_allocation_on_stack) != ::fast_io::allocator_adapter_flags::none};
	static inline constexpr bool secure_clear{
		(flags & ::fast_io::allocator_adapter_flags::secure_clear) != ::fast_io::allocator_adapter_flags::none};
	static inline constexpr bool has_status{allocator_adaptor::has_status};
	using handle_type = typename allocator_adaptor::handle_type;
	static inline constexpr ::std::size_t default_alignment{allocator_adaptor::default_alignment};
	static inline constexpr bool has_defaulted_alignment{alignof(T) <= default_alignment};

	template <bool throwing>
	using dispatch = ::fast_io::details::allocator_die_try_dispatch<allocator_adaptor, throwing>;

	static inline constexpr bool has_allocate_try{
		(has_defaulted_alignment && ::fast_io::details::has_allocate_try_impl<allocator_adaptor>) ||
		(!has_defaulted_alignment && ::fast_io::details::has_allocate_aligned_try_impl<allocator_adaptor>)};
	static inline constexpr bool has_allocate_at_least_try{
		(has_defaulted_alignment && ::fast_io::details::has_allocate_at_least_try_impl<allocator_adaptor>) ||
		(!has_defaulted_alignment && ::fast_io::details::has_allocate_aligned_at_least_try_impl<allocator_adaptor>)};
	static inline constexpr bool has_allocate_zero_try{
		(has_defaulted_alignment && ::fast_io::details::has_allocate_zero_try_impl<allocator_adaptor>) ||
		(!has_defaulted_alignment && ::fast_io::details::has_allocate_aligned_zero_try_impl<allocator_adaptor>)};
	static inline constexpr bool has_allocate_zero_at_least_try{
		(has_defaulted_alignment && ::fast_io::details::has_allocate_zero_at_least_try_impl<allocator_adaptor>) ||
		(!has_defaulted_alignment && ::fast_io::details::has_allocate_aligned_zero_at_least_try_impl<allocator_adaptor>)};
	static inline constexpr bool has_reallocate_try{
		(has_defaulted_alignment && ::fast_io::details::has_reallocate_try_impl<allocator_adaptor>) ||
		(!has_defaulted_alignment && ::fast_io::details::has_reallocate_aligned_try_impl<allocator_adaptor>)};
	static inline constexpr bool has_reallocate_at_least_try{
		(has_defaulted_alignment && ::fast_io::details::has_reallocate_at_least_try_impl<allocator_adaptor>) ||
		(!has_defaulted_alignment && ::fast_io::details::has_reallocate_aligned_at_least_try_impl<allocator_adaptor>)};
	static inline constexpr bool has_reallocate_zero_try{
		(has_defaulted_alignment && ::fast_io::details::has_reallocate_zero_try_impl<allocator_adaptor>) ||
		(!has_defaulted_alignment && ::fast_io::details::has_reallocate_aligned_zero_try_impl<allocator_adaptor>)};
	static inline constexpr bool has_reallocate_zero_at_least_try{
		(has_defaulted_alignment && ::fast_io::details::has_reallocate_zero_at_least_try_impl<allocator_adaptor>) ||
		(!has_defaulted_alignment && ::fast_io::details::has_reallocate_aligned_zero_at_least_try_impl<allocator_adaptor>)};
	static inline constexpr bool has_reallocate_n_try{
		(has_defaulted_alignment && ::fast_io::details::has_reallocate_n_try_impl<allocator_adaptor>) ||
		(!has_defaulted_alignment && ::fast_io::details::has_reallocate_aligned_n_try_impl<allocator_adaptor>)};
	static inline constexpr bool has_reallocate_n_at_least_try{
		(has_defaulted_alignment && ::fast_io::details::has_reallocate_n_at_least_try_impl<allocator_adaptor>) ||
		(!has_defaulted_alignment && ::fast_io::details::has_reallocate_aligned_n_at_least_try_impl<allocator_adaptor>)};
	static inline constexpr bool has_reallocate_zero_n_try{
		(has_defaulted_alignment && ::fast_io::details::has_reallocate_zero_n_try_impl<allocator_adaptor>) ||
		(!has_defaulted_alignment && ::fast_io::details::has_reallocate_aligned_zero_n_try_impl<allocator_adaptor>)};
	static inline constexpr bool has_reallocate_zero_n_at_least_try{
		(has_defaulted_alignment && ::fast_io::details::has_reallocate_zero_n_at_least_try_impl<allocator_adaptor>) ||
		(!has_defaulted_alignment && ::fast_io::details::has_reallocate_aligned_zero_n_at_least_try_impl<allocator_adaptor>)};

	template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline constexpr T *
	allocate_impl(::std::size_t n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
		requires(!has_status)
	{
#if __cpp_constexpr_dynamic_alloc >= 201907L
		if (__builtin_is_constant_evaluated())
		{
			return ::fast_io::freestanding::allocator<T>{}.allocate(n);
		}
#endif
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			if constexpr (throwing)
			{
				::fast_io::herbceptions::throws_errc(::std::errc::value_too_large);
			}
			else
			{
				::fast_io::fast_terminate();
			}
		}
		if constexpr (has_defaulted_alignment)
		{
			return static_cast<T *>(dispatch<throwing>::allocate(n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(dispatch<throwing>::allocate_aligned(alignof(T), n * sizeof(T)));
		}
	}

	static inline constexpr T *allocate(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
		requires((!throws_on_allocation_failure && !has_status) || (throws_on_allocation_failure && has_allocate_try))
	{
		return allocate_impl<throws_on_allocation_failure>(n);
	}
	static inline constexpr T *allocate_die(::std::size_t n) noexcept
		requires(!has_status)
	{
		return allocate_impl<false>(n);
	}
	static inline constexpr T *allocate_try(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
		requires(!has_status && has_allocate_try)
	{
		return allocate_impl<true>(n);
	}

	template <bool throwing>
	static inline constexpr basic_allocation_least_result<T *>
	allocate_at_least_impl(::std::size_t n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
		requires(!has_status)
	{
#if __cpp_constexpr_dynamic_alloc >= 201907L
		if (__builtin_is_constant_evaluated())
		{
			return {::fast_io::freestanding::allocator<T>{}.allocate(n), n};
		}
#endif
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			if constexpr (throwing)
			{
				::fast_io::herbceptions::throws_errc(::std::errc::value_too_large);
			}
			else
			{
				::fast_io::fast_terminate();
			}
		}
		if constexpr (has_defaulted_alignment)
		{
			auto newres{dispatch<throwing>::allocate_at_least(n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
		else
		{
			auto newres{dispatch<throwing>::allocate_aligned_at_least(alignof(T), n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
	}

	static inline constexpr basic_allocation_least_result<T *>
	allocate_at_least(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
		requires((!throws_on_allocation_failure && !has_status) || (throws_on_allocation_failure && has_allocate_at_least_try))
	{
		return allocate_at_least_impl<throws_on_allocation_failure>(n);
	}
	static inline constexpr basic_allocation_least_result<T *>
	allocate_at_least_die(::std::size_t n) noexcept
		requires(!has_status)
	{
		return allocate_at_least_impl<false>(n);
	}
	static inline constexpr basic_allocation_least_result<T *>
	allocate_at_least_try(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
		requires(!has_status && has_allocate_at_least_try)
	{
		return allocate_at_least_impl<true>(n);
	}

	template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline constexpr T *
	allocate_zero_impl(::std::size_t n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
		requires(!has_status)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			if constexpr (throwing)
			{
				::fast_io::herbceptions::throws_errc(::std::errc::value_too_large);
			}
			else
			{
				::fast_io::fast_terminate();
			}
		}
		if constexpr (has_defaulted_alignment)
		{
			return static_cast<T *>(dispatch<throwing>::allocate_zero(n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(dispatch<throwing>::allocate_aligned_zero(alignof(T), n * sizeof(T)));
		}
	}

	static inline constexpr T *allocate_zero(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
		requires((!throws_on_allocation_failure && !has_status) || (throws_on_allocation_failure && has_allocate_zero_try))
	{
		return allocate_zero_impl<throws_on_allocation_failure>(n);
	}
	static inline constexpr T *allocate_zero_die(::std::size_t n) noexcept
		requires(!has_status)
	{
		return allocate_zero_impl<false>(n);
	}
	static inline constexpr T *allocate_zero_try(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
		requires(!has_status && has_allocate_zero_try)
	{
		return allocate_zero_impl<true>(n);
	}

	template <bool throwing>
	static inline constexpr basic_allocation_least_result<T *>
	allocate_zero_at_least_impl(::std::size_t n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
		requires(!has_status)
	{
#if __cpp_constexpr_dynamic_alloc >= 201907L
		if (__builtin_is_constant_evaluated())
		{
			return {::fast_io::freestanding::allocator<T>{}.allocate(n), n};
		}
#endif
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			if constexpr (throwing)
			{
				::fast_io::herbceptions::throws_errc(::std::errc::value_too_large);
			}
			else
			{
				::fast_io::fast_terminate();
			}
		}
		if constexpr (has_defaulted_alignment)
		{
			auto newres{dispatch<throwing>::allocate_zero_at_least(n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
		else
		{
			auto newres{dispatch<throwing>::allocate_aligned_zero_at_least(alignof(T), n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
	}

	static inline constexpr basic_allocation_least_result<T *>
	allocate_zero_at_least(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
		requires((!throws_on_allocation_failure && !has_status) || (throws_on_allocation_failure && has_allocate_zero_at_least_try))
	{
		return allocate_zero_at_least_impl<throws_on_allocation_failure>(n);
	}
	static inline constexpr basic_allocation_least_result<T *>
	allocate_zero_at_least_die(::std::size_t n) noexcept
		requires(!has_status)
	{
		return allocate_zero_at_least_impl<false>(n);
	}
	static inline constexpr basic_allocation_least_result<T *>
	allocate_zero_at_least_try(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
		requires(!has_status && has_allocate_zero_at_least_try)
	{
		return allocate_zero_at_least_impl<true>(n);
	}

	static inline constexpr bool has_reallocate = allocator_adaptor::has_reallocate;

	template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline constexpr T *
	reallocate_impl(T *ptr, ::std::size_t n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
		requires(!has_status && has_reallocate)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			if constexpr (throwing)
			{
				::fast_io::herbceptions::throws_errc(::std::errc::value_too_large);
			}
			else
			{
				::fast_io::fast_terminate();
			}
		}
		if constexpr (has_defaulted_alignment)
		{
			return static_cast<T *>(dispatch<throwing>::reallocate(ptr, n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(dispatch<throwing>::reallocate_aligned(ptr, alignof(T), n * sizeof(T)));
		}
	}

	static inline constexpr T *reallocate(T *ptr, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
		requires((!throws_on_allocation_failure && !has_status && has_reallocate) ||
				 (throws_on_allocation_failure && has_reallocate_try))
	{
		return reallocate_impl<throws_on_allocation_failure>(ptr, n);
	}
	static inline constexpr T *reallocate_die(T *ptr, ::std::size_t n) noexcept
		requires(!has_status && has_reallocate)
	{
		return reallocate_impl<false>(ptr, n);
	}
	static inline constexpr T *reallocate_try(T *ptr, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
		requires(!has_status && has_reallocate_try)
	{
		return reallocate_impl<true>(ptr, n);
	}

	template <bool throwing>
	static inline constexpr basic_allocation_least_result<T *>
	reallocate_at_least_impl(T *ptr, ::std::size_t n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
		requires(!has_status && has_reallocate)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			if constexpr (throwing)
			{
				::fast_io::herbceptions::throws_errc(::std::errc::value_too_large);
			}
			else
			{
				::fast_io::fast_terminate();
			}
		}
		if constexpr (has_defaulted_alignment)
		{
			auto newres{dispatch<throwing>::reallocate_at_least(ptr, n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
		else
		{
			auto newres{dispatch<throwing>::reallocate_aligned_at_least(ptr, alignof(T), n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
	}

	static inline constexpr basic_allocation_least_result<T *>
	reallocate_at_least(T *ptr, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
		requires((!throws_on_allocation_failure && !has_status && has_reallocate) ||
				 (throws_on_allocation_failure && has_reallocate_at_least_try))
	{
		return reallocate_at_least_impl<throws_on_allocation_failure>(ptr, n);
	}
	static inline constexpr basic_allocation_least_result<T *>
	reallocate_at_least_die(T *ptr, ::std::size_t n) noexcept
		requires(!has_status && has_reallocate)
	{
		return reallocate_at_least_impl<false>(ptr, n);
	}
	static inline constexpr basic_allocation_least_result<T *>
	reallocate_at_least_try(T *ptr, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
		requires(!has_status && has_reallocate_at_least_try)
	{
		return reallocate_at_least_impl<true>(ptr, n);
	}

	static inline constexpr bool has_reallocate_zero = allocator_adaptor::has_reallocate_zero;

	template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline constexpr T *
	reallocate_zero_impl(T *ptr, ::std::size_t n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
		requires(!has_status && has_reallocate_zero)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			if constexpr (throwing)
			{
				::fast_io::herbceptions::throws_errc(::std::errc::value_too_large);
			}
			else
			{
				::fast_io::fast_terminate();
			}
		}
		if constexpr (has_defaulted_alignment)
		{
			return static_cast<T *>(dispatch<throwing>::reallocate_zero(ptr, n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(dispatch<throwing>::reallocate_aligned_zero(ptr, alignof(T), n * sizeof(T)));
		}
	}

	static inline constexpr T *reallocate_zero(T *ptr, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
		requires((!throws_on_allocation_failure && !has_status && has_reallocate_zero) ||
				 (throws_on_allocation_failure && has_reallocate_zero_try))
	{
		return reallocate_zero_impl<throws_on_allocation_failure>(ptr, n);
	}
	static inline constexpr T *reallocate_zero_die(T *ptr, ::std::size_t n) noexcept
		requires(!has_status && has_reallocate_zero)
	{
		return reallocate_zero_impl<false>(ptr, n);
	}
	static inline constexpr T *reallocate_zero_try(T *ptr, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
		requires(!has_status && has_reallocate_zero_try)
	{
		return reallocate_zero_impl<true>(ptr, n);
	}

	template <bool throwing>
	static inline constexpr basic_allocation_least_result<T *>
	reallocate_zero_at_least_impl(T *ptr, ::std::size_t n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
		requires(!has_status && has_reallocate_zero)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			if constexpr (throwing)
			{
				::fast_io::herbceptions::throws_errc(::std::errc::value_too_large);
			}
			else
			{
				::fast_io::fast_terminate();
			}
		}
		if constexpr (has_defaulted_alignment)
		{
			auto newres{dispatch<throwing>::reallocate_zero_at_least(ptr, n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
		else
		{
			auto newres{dispatch<throwing>::reallocate_aligned_zero_at_least(ptr, alignof(T), n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
	}

	static inline constexpr basic_allocation_least_result<T *>
	reallocate_zero_at_least(T *ptr, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
		requires((!throws_on_allocation_failure && !has_status && has_reallocate_zero) ||
				 (throws_on_allocation_failure && has_reallocate_zero_at_least_try))
	{
		return reallocate_zero_at_least_impl<throws_on_allocation_failure>(ptr, n);
	}
	static inline constexpr basic_allocation_least_result<T *>
	reallocate_zero_at_least_die(T *ptr, ::std::size_t n) noexcept
		requires(!has_status && has_reallocate_zero)
	{
		return reallocate_zero_at_least_impl<false>(ptr, n);
	}
	static inline constexpr basic_allocation_least_result<T *>
	reallocate_zero_at_least_try(T *ptr, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
		requires(!has_status && has_reallocate_zero_at_least_try)
	{
		return reallocate_zero_at_least_impl<true>(ptr, n);
	}

	template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline constexpr T *
	reallocate_n_impl(T *ptr, ::std::size_t oldn, ::std::size_t n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
		requires(!has_status)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			if constexpr (throwing)
			{
				::fast_io::herbceptions::throws_errc(::std::errc::value_too_large);
			}
			else
			{
				::fast_io::fast_terminate();
			}
		}
		if constexpr (has_defaulted_alignment)
		{
			return static_cast<T *>(dispatch<throwing>::reallocate_n(ptr, oldn * sizeof(T), n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(dispatch<throwing>::reallocate_aligned_n(ptr, oldn * sizeof(T), alignof(T), n * sizeof(T)));
		}
	}

	static inline constexpr T *reallocate_n(T *ptr, ::std::size_t oldn, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
		requires((!throws_on_allocation_failure && !has_status) || (throws_on_allocation_failure && has_reallocate_n_try))
	{
		return reallocate_n_impl<throws_on_allocation_failure>(ptr, oldn, n);
	}
	static inline constexpr T *reallocate_n_die(T *ptr, ::std::size_t oldn, ::std::size_t n) noexcept
		requires(!has_status)
	{
		return reallocate_n_impl<false>(ptr, oldn, n);
	}
	static inline constexpr T *reallocate_n_try(T *ptr, ::std::size_t oldn, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
		requires(!has_status && has_reallocate_n_try)
	{
		return reallocate_n_impl<true>(ptr, oldn, n);
	}

	template <bool throwing>
	static inline constexpr basic_allocation_least_result<T *>
	reallocate_n_at_least_impl(T *ptr, ::std::size_t oldn, ::std::size_t n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
		requires(!has_status)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			if constexpr (throwing)
			{
				::fast_io::herbceptions::throws_errc(::std::errc::value_too_large);
			}
			else
			{
				::fast_io::fast_terminate();
			}
		}
		if constexpr (has_defaulted_alignment)
		{
			auto newres{dispatch<throwing>::reallocate_n_at_least(ptr, oldn * sizeof(T), n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
		else
		{
			auto newres{dispatch<throwing>::reallocate_aligned_n_at_least(ptr, oldn * sizeof(T), alignof(T), n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
	}

	static inline constexpr basic_allocation_least_result<T *>
	reallocate_n_at_least(T *ptr, ::std::size_t oldn, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
		requires((!throws_on_allocation_failure && !has_status) || (throws_on_allocation_failure && has_reallocate_n_at_least_try))
	{
		return reallocate_n_at_least_impl<throws_on_allocation_failure>(ptr, oldn, n);
	}
	static inline constexpr basic_allocation_least_result<T *>
	reallocate_n_at_least_die(T *ptr, ::std::size_t oldn, ::std::size_t n) noexcept
		requires(!has_status)
	{
		return reallocate_n_at_least_impl<false>(ptr, oldn, n);
	}
	static inline constexpr basic_allocation_least_result<T *>
	reallocate_n_at_least_try(T *ptr, ::std::size_t oldn, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
		requires(!has_status && has_reallocate_n_at_least_try)
	{
		return reallocate_n_at_least_impl<true>(ptr, oldn, n);
	}

	template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline constexpr T *
	reallocate_zero_n_impl(T *ptr, ::std::size_t oldn, ::std::size_t n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
		requires(!has_status)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			if constexpr (throwing)
			{
				::fast_io::herbceptions::throws_errc(::std::errc::value_too_large);
			}
			else
			{
				::fast_io::fast_terminate();
			}
		}
		if constexpr (has_defaulted_alignment)
		{
			return static_cast<T *>(dispatch<throwing>::reallocate_zero_n(ptr, oldn * sizeof(T), n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(dispatch<throwing>::reallocate_aligned_zero_n(ptr, oldn * sizeof(T), alignof(T), n * sizeof(T)));
		}
	}

	static inline constexpr T *reallocate_zero_n(T *ptr, ::std::size_t oldn, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
		requires((!throws_on_allocation_failure && !has_status) || (throws_on_allocation_failure && has_reallocate_zero_n_try))
	{
		return reallocate_zero_n_impl<throws_on_allocation_failure>(ptr, oldn, n);
	}
	static inline constexpr T *reallocate_zero_n_die(T *ptr, ::std::size_t oldn, ::std::size_t n) noexcept
		requires(!has_status)
	{
		return reallocate_zero_n_impl<false>(ptr, oldn, n);
	}
	static inline constexpr T *reallocate_zero_n_try(T *ptr, ::std::size_t oldn, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
		requires(!has_status && has_reallocate_zero_n_try)
	{
		return reallocate_zero_n_impl<true>(ptr, oldn, n);
	}

	template <bool throwing>
	static inline constexpr basic_allocation_least_result<T *>
	reallocate_zero_n_at_least_impl(T *ptr, ::std::size_t oldn, ::std::size_t n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
		requires(!has_status)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			if constexpr (throwing)
			{
				::fast_io::herbceptions::throws_errc(::std::errc::value_too_large);
			}
			else
			{
				::fast_io::fast_terminate();
			}
		}
		if constexpr (has_defaulted_alignment)
		{
			auto newres{dispatch<throwing>::reallocate_zero_n_at_least(ptr, oldn * sizeof(T), n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
		else
		{
			auto newres{dispatch<throwing>::reallocate_aligned_zero_n_at_least(ptr, oldn * sizeof(T), alignof(T), n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
	}

	static inline constexpr basic_allocation_least_result<T *>
	reallocate_zero_n_at_least(T *ptr, ::std::size_t oldn, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
		requires((!throws_on_allocation_failure && !has_status) || (throws_on_allocation_failure && has_reallocate_zero_n_at_least_try))
	{
		return reallocate_zero_n_at_least_impl<throws_on_allocation_failure>(ptr, oldn, n);
	}
	static inline constexpr basic_allocation_least_result<T *>
	reallocate_zero_n_at_least_die(T *ptr, ::std::size_t oldn, ::std::size_t n) noexcept
		requires(!has_status)
	{
		return reallocate_zero_n_at_least_impl<false>(ptr, oldn, n);
	}
	static inline constexpr basic_allocation_least_result<T *>
	reallocate_zero_n_at_least_try(T *ptr, ::std::size_t oldn, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
		requires(!has_status && has_reallocate_zero_n_at_least_try)
	{
		return reallocate_zero_n_at_least_impl<true>(ptr, oldn, n);
	}

	static inline constexpr bool has_deallocate = allocator_adaptor::has_deallocate;

	static inline constexpr void
	deallocate(T *ptr) noexcept
		requires(!has_status && has_deallocate && !secure_clear)
	{
#if __cpp_constexpr_dynamic_alloc >= 201907L
		if (__builtin_is_constant_evaluated())
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
		if constexpr (has_defaulted_alignment)
		{
			return alloc::deallocate(ptr);
		}
		else
		{
			return alloc::deallocate_aligned(ptr, alignof(T));
		}
	}

	static inline constexpr void
	deallocate_n(T *ptr, ::std::size_t n) noexcept
		requires(!has_status)
	{
		if constexpr (secure_clear)
		{
			if !consteval
			{
				if (ptr != nullptr)
				{
					::fast_io::freestanding::bytes_secure_clear_n(reinterpret_cast<::std::byte *>(ptr), n * sizeof(T));
				}
			}
		}
#if __cpp_constexpr_dynamic_alloc >= 201907L
		if (__builtin_is_constant_evaluated())
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
		if constexpr (has_defaulted_alignment)
		{
			alloc::deallocate_n(ptr, n * sizeof(T));
		}
		else
		{
			alloc::deallocate_aligned_n(ptr, alignof(T), n * sizeof(T));
		}
	}

	static inline constexpr bool has_handle_allocate{has_status && allocator_adaptor::has_native_handle_allocate};
	static inline constexpr bool has_handle_allocate_try{has_status && allocator_adaptor::has_native_handle_allocate_try};

	template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline constexpr T *
	handle_allocate_impl(handle_type handle, ::std::size_t n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
		requires(has_status && has_defaulted_alignment)
	{
#if __cpp_constexpr_dynamic_alloc >= 201907L
		if (__builtin_is_constant_evaluated())
		{
			return ::fast_io::freestanding::allocator<T>{}.allocate(n);
		}
#endif
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			if constexpr (throwing)
			{
				::fast_io::herbceptions::throws_errc(::std::errc::value_too_large);
			}
			else
			{
				::fast_io::fast_terminate();
			}
		}
		return static_cast<T *>(dispatch<throwing>::handle_allocate(handle, n * sizeof(T)));
	}

	static inline constexpr T *handle_allocate(handle_type handle, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
		requires(has_defaulted_alignment &&
				 ((!throws_on_allocation_failure && has_handle_allocate) ||
				  (throws_on_allocation_failure && has_handle_allocate_try)))
	{
		return handle_allocate_impl<throws_on_allocation_failure>(handle, n);
	}
	static inline constexpr T *handle_allocate_die(handle_type handle, ::std::size_t n) noexcept
		requires(has_defaulted_alignment && has_handle_allocate)
	{
		return handle_allocate_impl<false>(handle, n);
	}
	static inline constexpr T *handle_allocate_try(handle_type handle, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
		requires(has_defaulted_alignment && has_handle_allocate_try)
	{
		return handle_allocate_impl<true>(handle, n);
	}

	template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline constexpr T *
	handle_allocate_zero_impl(handle_type handle, ::std::size_t n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
		requires(has_status && has_defaulted_alignment)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			if constexpr (throwing)
			{
				::fast_io::herbceptions::throws_errc(::std::errc::value_too_large);
			}
			else
			{
				::fast_io::fast_terminate();
			}
		}
		return static_cast<T *>(dispatch<throwing>::handle_allocate_zero(handle, n * sizeof(T)));
	}

	static inline constexpr T *handle_allocate_zero(handle_type handle, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
		requires(has_defaulted_alignment &&
				 ((!throws_on_allocation_failure && has_handle_allocate) ||
				  (throws_on_allocation_failure && has_handle_allocate_try)))
	{
		return handle_allocate_zero_impl<throws_on_allocation_failure>(handle, n);
	}
	static inline constexpr T *handle_allocate_zero_die(handle_type handle, ::std::size_t n) noexcept
		requires(has_defaulted_alignment && has_handle_allocate)
	{
		return handle_allocate_zero_impl<false>(handle, n);
	}
	static inline constexpr T *handle_allocate_zero_try(handle_type handle, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
		requires(has_defaulted_alignment && has_handle_allocate_try)
	{
		return handle_allocate_zero_impl<true>(handle, n);
	}

	template <bool throwing>
	static inline constexpr basic_allocation_least_result<T *>
	handle_allocate_at_least_impl(handle_type handle, ::std::size_t n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
		requires(has_status && has_defaulted_alignment)
	{
#if __cpp_constexpr_dynamic_alloc >= 201907L
		if (__builtin_is_constant_evaluated())
		{
			return {::fast_io::freestanding::allocator<T>{}.allocate(n), n};
		}
#endif
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			if constexpr (throwing)
			{
				::fast_io::herbceptions::throws_errc(::std::errc::value_too_large);
			}
			else
			{
				::fast_io::fast_terminate();
			}
		}
		if constexpr (::fast_io::details::has_handle_allocate_at_least_mode_impl<allocator_adaptor, throwing>)
		{
			auto newres{dispatch<throwing>::handle_allocate_at_least(handle, n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
		else
		{
			return {handle_allocate_impl<throwing>(handle, n), n};
		}
	}

	static inline constexpr basic_allocation_least_result<T *> handle_allocate_at_least(handle_type handle, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
		requires(has_defaulted_alignment &&
				 ((!throws_on_allocation_failure && has_handle_allocate) ||
				  (throws_on_allocation_failure && has_handle_allocate_try)))
	{
		return handle_allocate_at_least_impl<throws_on_allocation_failure>(handle, n);
	}
	static inline constexpr basic_allocation_least_result<T *> handle_allocate_at_least_die(handle_type handle, ::std::size_t n) noexcept
		requires(has_defaulted_alignment && has_handle_allocate)
	{
		return handle_allocate_at_least_impl<false>(handle, n);
	}
	static inline constexpr basic_allocation_least_result<T *> handle_allocate_at_least_try(handle_type handle, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
		requires(has_defaulted_alignment && has_handle_allocate_try)
	{
		return handle_allocate_at_least_impl<true>(handle, n);
	}

	static inline constexpr bool has_handle_deallocate{allocator_adaptor::has_handle_deallocate};

	static inline constexpr void
	handle_deallocate(handle_type handle, T *ptr) noexcept
		requires(has_status && has_defaulted_alignment && has_handle_deallocate && !secure_clear)
	{
#if __cpp_constexpr_dynamic_alloc >= 201907L
		if (__builtin_is_constant_evaluated())
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
		alloc::handle_deallocate(handle, ptr);
	}

	static inline constexpr void
	handle_deallocate_n(handle_type handle, T *ptr, ::std::size_t n) noexcept
		requires(has_status && has_handle_deallocate)
	{
		if constexpr (secure_clear)
		{
			if !consteval
			{
				if (ptr != nullptr)
				{
					::fast_io::freestanding::bytes_secure_clear_n(reinterpret_cast<::std::byte *>(ptr), n * sizeof(T));
				}
			}
		}
#if __cpp_constexpr_dynamic_alloc >= 201907L
		if (__builtin_is_constant_evaluated())
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
		alloc::handle_deallocate_n(handle, ptr, n * sizeof(T));
	}
