#pragma once

static inline constexpr bool has_native_allocate{
	!has_status && (::fast_io::details::has_allocate_at_least_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_at_least_impl<alloc> ||
					::fast_io::details::has_allocate_zero_at_least_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_zero_at_least_impl<alloc> ||
					::fast_io::details::has_allocate_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_impl<alloc> ||
					::fast_io::details::has_allocate_zero_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_zero_impl<alloc> ||
					::fast_io::details::has_allocate_conditional_zero_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_conditional_zero_impl<alloc> ||
					::fast_io::details::has_allocate_conditional_zero_at_least_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_conditional_zero_at_least_impl<alloc>)};
static inline constexpr bool has_native_allocate_try{
	!has_status && (::fast_io::details::has_allocate_at_least_try_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_at_least_try_impl<alloc> ||
					::fast_io::details::has_allocate_zero_at_least_try_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_zero_at_least_try_impl<alloc> ||
					::fast_io::details::has_allocate_try_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_try_impl<alloc> ||
					::fast_io::details::has_allocate_zero_try_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_zero_try_impl<alloc> ||
					::fast_io::details::has_allocate_conditional_zero_try_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_conditional_zero_try_impl<alloc> ||
					::fast_io::details::has_allocate_conditional_zero_at_least_try_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_conditional_zero_at_least_try_impl<alloc>)};

template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *allocate_conditional_zero_impl(::std::size_t n, bool zero)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status)
{
	FAST_IO_IF_CONSTEVAL
	{
		auto p{
#if FAST_IO_HAS_BUILTIN(__builtin_operator_new)
			__builtin_operator_new(n)
#else
			::operator new(n)
#endif
		};
		if (zero)
		{
			::fast_io::freestanding::bytes_clear_n(static_cast<::std::byte *>(p), n);
		}
		return p;
	}
	else
	{
		if constexpr (::fast_io::details::has_allocate_conditional_zero_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::allocate_conditional_zero(n, zero);
		}
		else if constexpr (::fast_io::details::has_allocate_conditional_zero_at_least_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::allocate_conditional_zero_at_least(n, zero).ptr;
		}
		else if constexpr (::fast_io::details::has_allocate_aligned_conditional_zero_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::allocate_aligned_conditional_zero(default_alignment, n, zero);
		}
		else if constexpr (::fast_io::details::has_allocate_aligned_conditional_zero_at_least_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::allocate_aligned_conditional_zero_at_least(default_alignment, n, zero).ptr;
		}
		else
		{
			constexpr bool has_none_zero_ops{::fast_io::details::native_allocate_has_none_zero_ops<alloc, throwing>};
			constexpr bool has_zero_ops{::fast_io::details::native_allocate_has_zero_ops<alloc, throwing>};
			if constexpr (!has_none_zero_ops && !has_zero_ops)
			{
				::fast_io::fast_terminate();
#if __has_cpp_attribute(unreachable)
				[[unreachable]];
#endif
			}
			else if constexpr (!has_none_zero_ops && has_zero_ops)
			{
				if constexpr (::fast_io::details::has_allocate_zero_mode_impl<alloc, throwing>)
				{
					return dispatch<throwing>::allocate_zero(n);
				}
				else if constexpr (::fast_io::details::has_allocate_zero_at_least_mode_impl<alloc, throwing>)
				{
					return dispatch<throwing>::allocate_zero_at_least(n).ptr;
				}
				else if constexpr (::fast_io::details::has_allocate_aligned_zero_mode_impl<alloc, throwing>)
				{
					return dispatch<throwing>::allocate_aligned_zero(default_alignment, n);
				}
				else if constexpr (::fast_io::details::has_allocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
				{
					return dispatch<throwing>::allocate_aligned_zero_at_least(default_alignment, n).ptr;
				}
			}
			else if constexpr (has_none_zero_ops && !has_zero_ops)
			{
				void *ptr;
				if constexpr (::fast_io::details::has_allocate_mode_impl<alloc, throwing>)
				{
					ptr = dispatch<throwing>::allocate(n);
				}
				else if constexpr (::fast_io::details::has_allocate_at_least_mode_impl<alloc, throwing>)
				{
					ptr = dispatch<throwing>::allocate_at_least(n).ptr;
				}
				else if constexpr (::fast_io::details::has_allocate_aligned_mode_impl<alloc, throwing>)
				{
					ptr = dispatch<throwing>::allocate_aligned(default_alignment, n);
				}
				else if constexpr (::fast_io::details::has_allocate_aligned_at_least_mode_impl<alloc, throwing>)
				{
					ptr = dispatch<throwing>::allocate_aligned_at_least(default_alignment, n).ptr;
				}
				if (zero)
				{
					::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(ptr), n);
				}
				return ptr;
			}
			else
			{
				void *ptr;
				if (zero)
				{
					if constexpr (::fast_io::details::has_allocate_zero_mode_impl<alloc, throwing>)
					{
						ptr = dispatch<throwing>::allocate_zero(n);
					}
					else if constexpr (::fast_io::details::has_allocate_zero_at_least_mode_impl<alloc, throwing>)
					{
						ptr = dispatch<throwing>::allocate_zero_at_least(n).ptr;
					}
					else if constexpr (::fast_io::details::has_allocate_aligned_zero_mode_impl<alloc, throwing>)
					{
						ptr = dispatch<throwing>::allocate_aligned_zero(default_alignment, n);
					}
					else if constexpr (::fast_io::details::has_allocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
					{
						ptr = dispatch<throwing>::allocate_aligned_zero_at_least(default_alignment, n).ptr;
					}
					else
					{
						::fast_io::fast_terminate();
					}
				}
				else
				{
					if constexpr (::fast_io::details::has_allocate_mode_impl<alloc, throwing>)
					{
						ptr = dispatch<throwing>::allocate(n);
					}
					else if constexpr (::fast_io::details::has_allocate_at_least_mode_impl<alloc, throwing>)
					{
						ptr = dispatch<throwing>::allocate_at_least(n).ptr;
					}
					else if constexpr (::fast_io::details::has_allocate_aligned_mode_impl<alloc, throwing>)
					{
						ptr = dispatch<throwing>::allocate_aligned(default_alignment, n);
					}
					else if constexpr (::fast_io::details::has_allocate_aligned_at_least_mode_impl<alloc, throwing>)
					{
						ptr = dispatch<throwing>::allocate_aligned_at_least(default_alignment, n).ptr;
					}
					else
					{
						::fast_io::fast_terminate();
					}
				}
				return ptr;
			}
		}
	}
}
static inline constexpr void *allocate_conditional_zero(::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status)) || (throws_on_allocation_failure && (!has_status && has_native_allocate_try)))
{
	return allocate_conditional_zero_impl<throws_on_allocation_failure>(n, zero);
}
static inline constexpr void *allocate_conditional_zero_die(::std::size_t n, bool zero) noexcept
	requires(!has_status)
{
	return allocate_conditional_zero_impl<false>(n, zero);
}
static inline constexpr void *allocate_conditional_zero_try(::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_native_allocate_try)
{
	return allocate_conditional_zero_impl<true>(n, zero);
}

template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline constexpr void *
allocate_impl(::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status)
{
#if __cpp_constexpr_dynamic_alloc >= 201907L
	FAST_IO_IF_CONSTEVAL
	{
		return allocate_conditional_zero_impl<throwing>(n, false);
	}
	else
#endif
	{
		if constexpr (::fast_io::details::has_allocate_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::allocate(n);
		}
		else
		{
			return allocate_conditional_zero_impl<throwing>(n, false);
		}
	}
}
static inline constexpr void *allocate(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status)) || (throws_on_allocation_failure && (!has_status && has_native_allocate_try)))
{
	return allocate_impl<throws_on_allocation_failure>(n);
}
static inline constexpr void *allocate_die(::std::size_t n) noexcept
	requires(!has_status)
{
	return allocate_impl<false>(n);
}
static inline constexpr void *allocate_try(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_native_allocate_try)
{
	return allocate_impl<true>(n);
}


template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *allocate_zero_impl(::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status)
{
#if __cpp_constexpr_dynamic_alloc >= 201907L
	FAST_IO_IF_CONSTEVAL
	{
		return allocate_conditional_zero_impl<throwing>(n, true);
	}
	else
#endif
	{
		if constexpr (::fast_io::details::has_allocate_zero_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::allocate_zero(n);
		}
		else
		{
			return allocate_conditional_zero_impl<throwing>(n, true);
		}
	}
}
static inline constexpr void *allocate_zero(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status)) || (throws_on_allocation_failure && (!has_status && has_native_allocate_try)))
{
	return allocate_zero_impl<throws_on_allocation_failure>(n);
}
static inline constexpr void *allocate_zero_die(::std::size_t n) noexcept
	requires(!has_status)
{
	return allocate_zero_impl<false>(n);
}
static inline constexpr void *allocate_zero_try(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_native_allocate_try)
{
	return allocate_zero_impl<true>(n);
}


static inline constexpr bool has_reallocate = (::fast_io::details::has_reallocate_impl<alloc> ||
											   ::fast_io::details::has_reallocate_at_least_impl<alloc> ||
											   ::fast_io::details::has_reallocate_aligned_impl<alloc> ||
											   ::fast_io::details::has_reallocate_aligned_at_least_impl<alloc> ||
											   ::fast_io::details::has_reallocate_zero_impl<alloc> ||
											   ::fast_io::details::has_reallocate_zero_at_least_impl<alloc> ||
											   ::fast_io::details::has_reallocate_aligned_zero_impl<alloc> ||
											   ::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc> ||
											   ::fast_io::details::has_reallocate_conditional_zero_impl<alloc> ||
											   ::fast_io::details::has_reallocate_aligned_conditional_zero_impl<alloc> ||
											   ::fast_io::details::has_reallocate_conditional_zero_at_least_impl<alloc> ||
											   ::fast_io::details::has_reallocate_aligned_conditional_zero_at_least_impl<alloc>);
static inline constexpr bool has_reallocate_try = (::fast_io::details::has_reallocate_try_impl<alloc> ||
												   ::fast_io::details::has_reallocate_at_least_try_impl<alloc> ||
												   ::fast_io::details::has_reallocate_aligned_try_impl<alloc> ||
												   ::fast_io::details::has_reallocate_aligned_at_least_try_impl<alloc> ||
												   ::fast_io::details::has_reallocate_zero_try_impl<alloc> ||
												   ::fast_io::details::has_reallocate_zero_at_least_try_impl<alloc> ||
												   ::fast_io::details::has_reallocate_aligned_zero_try_impl<alloc> ||
												   ::fast_io::details::has_reallocate_aligned_zero_at_least_try_impl<alloc> ||
												   ::fast_io::details::has_reallocate_conditional_zero_try_impl<alloc> ||
												   ::fast_io::details::has_reallocate_aligned_conditional_zero_try_impl<alloc> ||
												   ::fast_io::details::has_reallocate_conditional_zero_at_least_try_impl<alloc> ||
												   ::fast_io::details::has_reallocate_aligned_conditional_zero_at_least_try_impl<alloc>);

template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *reallocate_conditional_zero_impl(void *p, ::std::size_t n, bool zero)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status && has_reallocate)
{
	if constexpr (::fast_io::details::has_reallocate_conditional_zero_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_conditional_zero(p, n, zero);
	}
	else if constexpr (::fast_io::details::has_reallocate_conditional_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_conditional_zero_at_least(p, n, zero).ptr;
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_conditional_zero_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_conditional_zero(p, default_alignment, n, zero);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_conditional_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_conditional_zero_at_least(p, default_alignment, n, zero).ptr;
	}
	else if constexpr (::fast_io::details::has_reallocate_mode_impl<alloc, throwing> ||
					   ::fast_io::details::has_reallocate_at_least_mode_impl<alloc, throwing> ||
					   ::fast_io::details::has_reallocate_aligned_mode_impl<alloc, throwing> ||
					   ::fast_io::details::has_reallocate_aligned_at_least_mode_impl<alloc, throwing>)
	{
		// Non-zero APIs exist - need runtime branch
		if (zero)
		{
			if constexpr (::fast_io::details::has_reallocate_zero_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_zero(p, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_zero_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_zero_at_least(p, n).ptr;
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_zero_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_aligned_zero(p, default_alignment, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_aligned_zero_at_least(p, default_alignment, n).ptr;
			}
			else
			{
				::fast_io::fast_terminate();
			}
		}
		else
		{
			if constexpr (::fast_io::details::has_reallocate_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate(p, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_at_least(p, n).ptr;
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_aligned(p, default_alignment, n);
			}
			else
			{
				return dispatch<throwing>::reallocate_aligned_at_least(p, default_alignment, n).ptr;
			}
		}
	}
	else
	{
		// Only zero APIs exist - use zero API for both cases
		if constexpr (::fast_io::details::has_reallocate_zero_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::reallocate_zero(p, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_zero_at_least_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::reallocate_zero_at_least(p, n).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::reallocate_aligned_zero(p, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::reallocate_aligned_zero_at_least(p, default_alignment, n).ptr;
		}
		else
		{
			::fast_io::fast_terminate();
		}
	}
}
static inline constexpr void *reallocate_conditional_zero(void *p, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status && has_reallocate)) || (throws_on_allocation_failure && (!has_status && has_reallocate_try)))
{
	return reallocate_conditional_zero_impl<throws_on_allocation_failure>(p, n, zero);
}
static inline constexpr void *reallocate_conditional_zero_die(void *p, ::std::size_t n, bool zero) noexcept
	requires(!has_status && has_reallocate)
{
	return reallocate_conditional_zero_impl<false>(p, n, zero);
}
static inline constexpr void *reallocate_conditional_zero_try(void *p, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_reallocate_try)
{
	return reallocate_conditional_zero_impl<true>(p, n, zero);
}


template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *reallocate_impl(void *p, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status && has_reallocate)
{
	if constexpr (::fast_io::details::has_reallocate_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate(p, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_at_least(p, n).ptr;
	}
	else
	{
		return reallocate_conditional_zero_impl<throwing>(p, n, false);
	}
}
static inline constexpr void *reallocate(void *p, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status && has_reallocate)) || (throws_on_allocation_failure && (!has_status && has_reallocate_try)))
{
	return reallocate_impl<throws_on_allocation_failure>(p, n);
}
static inline constexpr void *reallocate_die(void *p, ::std::size_t n) noexcept
	requires(!has_status && has_reallocate)
{
	return reallocate_impl<false>(p, n);
}
static inline constexpr void *reallocate_try(void *p, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_reallocate_try)
{
	return reallocate_impl<true>(p, n);
}


static inline constexpr bool has_reallocate_zero = (::fast_io::details::has_reallocate_zero_impl<alloc> ||
													::fast_io::details::has_reallocate_zero_at_least_impl<alloc> ||
													::fast_io::details::has_reallocate_aligned_zero_impl<alloc> ||
													::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>);
static inline constexpr bool has_reallocate_zero_try = (::fast_io::details::has_reallocate_zero_try_impl<alloc> ||
														::fast_io::details::has_reallocate_zero_at_least_try_impl<alloc> ||
														::fast_io::details::has_reallocate_aligned_zero_try_impl<alloc> ||
														::fast_io::details::has_reallocate_aligned_zero_at_least_try_impl<alloc>);


template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *reallocate_zero_impl(void *p, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status && has_reallocate_zero)
{
	if constexpr (::fast_io::details::has_reallocate_zero_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_zero(p, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_zero_at_least(p, n).ptr;
	}
	else
	{
		return reallocate_conditional_zero_impl<throwing>(p, n, true);
	}
}
static inline constexpr void *reallocate_zero(void *p, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status && has_reallocate_zero)) || (throws_on_allocation_failure && (!has_status && has_reallocate_zero_try)))
{
	return reallocate_zero_impl<throws_on_allocation_failure>(p, n);
}
static inline constexpr void *reallocate_zero_die(void *p, ::std::size_t n) noexcept
	requires(!has_status && has_reallocate_zero)
{
	return reallocate_zero_impl<false>(p, n);
}
static inline constexpr void *reallocate_zero_try(void *p, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_reallocate_zero_try)
{
	return reallocate_zero_impl<true>(p, n);
}


template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *reallocate_n_conditional_zero_impl(void *p, ::std::size_t oldn, ::std::size_t n, bool zero)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status)
{
	if (p != nullptr && oldn == n)
	{
		return p;
	}
	FAST_IO_IF_CONSTEVAL
	{
		auto newptr{::operator new(n)};
		if (p != nullptr)
		{
			if (n)
			{
				::std::size_t copyn{oldn < n ? oldn : n};
				::fast_io::freestanding::nonoverlapped_bytes_copy_n(reinterpret_cast<::std::byte const *>(p), copyn, reinterpret_cast<::std::byte *>(newptr));
			}
			::operator delete(p);
		}
		if (zero && oldn < n)
		{
			::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(newptr) + oldn, n - oldn);
		}
		return newptr;
	}
	else
	{
		if constexpr (::fast_io::details::has_reallocate_n_conditional_zero_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::reallocate_n_conditional_zero(p, oldn, n, zero);
		}
		else if constexpr (::fast_io::details::has_reallocate_n_conditional_zero_at_least_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::reallocate_n_conditional_zero_at_least(p, oldn, n, zero).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_n_conditional_zero_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::reallocate_aligned_n_conditional_zero(p, oldn, default_alignment, n, zero);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_n_conditional_zero_at_least_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::reallocate_aligned_n_conditional_zero_at_least(p, oldn, default_alignment, n, zero).ptr;
		}
		else if (zero)
		{
			if constexpr (::fast_io::details::has_reallocate_zero_n_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_zero_n(p, oldn, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_zero_n_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_zero_n_at_least(p, oldn, n).ptr;
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_aligned_zero_n(p, oldn, default_alignment, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_aligned_zero_n_at_least(p, oldn, default_alignment, n).ptr;
			}
			else
			{
				auto newptr{reallocate_n_impl<throwing>(p, oldn, n)};
				if (oldn < n)
				{
					::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(newptr) + oldn, n - oldn);
				}
				return newptr;
			}
		}
		else
		{
			if constexpr (::fast_io::details::has_reallocate_n_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_n(p, oldn, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_n_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_n_at_least(p, oldn, n).ptr;
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_n_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_aligned_n(p, oldn, default_alignment, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_n_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_aligned_n_at_least(p, oldn, default_alignment, n).ptr;
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_aligned_zero_n(p, oldn, default_alignment, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_aligned_zero_n_at_least(p, oldn, default_alignment, n).ptr;
			}
			else if constexpr (::fast_io::details::has_reallocate_zero_n_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_zero_n(p, oldn, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_zero_n_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_zero_n_at_least(p, oldn, n).ptr;
			}
			else
			{
				auto newptr{allocate_impl<throwing>(n)};
				if (p != nullptr)
				{
					if (n)
					{
						::std::size_t copyn{oldn < n ? oldn : n};
						::fast_io::freestanding::nonoverlapped_bytes_copy_n(reinterpret_cast<::std::byte const *>(p), copyn, reinterpret_cast<::std::byte *>(newptr));
					}
					generic_allocator_adapter::deallocate_n(p, oldn);
				}
				return newptr;
			}
		}
	}
}
static inline constexpr void *reallocate_n_conditional_zero(void *p, ::std::size_t oldn, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status)) || (throws_on_allocation_failure && (!has_status && has_native_allocate_try)))
{
	return reallocate_n_conditional_zero_impl<throws_on_allocation_failure>(p, oldn, n, zero);
}
static inline constexpr void *reallocate_n_conditional_zero_die(void *p, ::std::size_t oldn, ::std::size_t n, bool zero) noexcept
	requires(!has_status)
{
	return reallocate_n_conditional_zero_impl<false>(p, oldn, n, zero);
}
static inline constexpr void *reallocate_n_conditional_zero_try(void *p, ::std::size_t oldn, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_native_allocate_try)
{
	return reallocate_n_conditional_zero_impl<true>(p, oldn, n, zero);
}


template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *reallocate_n_impl(void *p, ::std::size_t oldn, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status)
{
	if (p != nullptr && oldn == n)
	{
		return p;
	}
	if constexpr (::fast_io::details::has_reallocate_n_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_n(p, oldn, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_n_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_n_at_least(p, oldn, n).ptr;
	}
	else
	{
		return reallocate_n_conditional_zero_impl<throwing>(p, oldn, n, false);
	}
}
static inline constexpr void *reallocate_n(void *p, ::std::size_t oldn, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status)) || (throws_on_allocation_failure && (!has_status && has_native_allocate_try)))
{
	return reallocate_n_impl<throws_on_allocation_failure>(p, oldn, n);
}
static inline constexpr void *reallocate_n_die(void *p, ::std::size_t oldn, ::std::size_t n) noexcept
	requires(!has_status)
{
	return reallocate_n_impl<false>(p, oldn, n);
}
static inline constexpr void *reallocate_n_try(void *p, ::std::size_t oldn, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_native_allocate_try)
{
	return reallocate_n_impl<true>(p, oldn, n);
}


template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *reallocate_zero_n_impl(void *p, ::std::size_t oldn, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status)
{
	if (p != nullptr && oldn == n)
	{
		return p;
	}
	if constexpr (::fast_io::details::has_reallocate_zero_n_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_zero_n(p, oldn, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_zero_n_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_zero_n_at_least(p, oldn, n).ptr;
	}
	else
	{
		return reallocate_n_conditional_zero_impl<throwing>(p, oldn, n, true);
	}
}
static inline constexpr void *reallocate_zero_n(void *p, ::std::size_t oldn, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status)) || (throws_on_allocation_failure && (!has_status && has_native_allocate_try)))
{
	return reallocate_zero_n_impl<throws_on_allocation_failure>(p, oldn, n);
}
static inline constexpr void *reallocate_zero_n_die(void *p, ::std::size_t oldn, ::std::size_t n) noexcept
	requires(!has_status)
{
	return reallocate_zero_n_impl<false>(p, oldn, n);
}
static inline constexpr void *reallocate_zero_n_try(void *p, ::std::size_t oldn, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_native_allocate_try)
{
	return reallocate_zero_n_impl<true>(p, oldn, n);
}


static inline constexpr bool has_deallocate = (::fast_io::details::has_deallocate_impl<alloc> ||
											   ::fast_io::details::has_deallocate_aligned_impl<alloc>);
static inline constexpr void deallocate(void *p) noexcept
	requires(!has_status && has_deallocate && !secure_clear)
{
#if __cpp_constexpr_dynamic_alloc >= 201907L
	if (__builtin_is_constant_evaluated())
	{
#if FAST_IO_HAS_BUILTIN(__builtin_operator_delete)
		__builtin_operator_delete(p);
#else
		::operator delete(p);
#endif
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

static inline constexpr void deallocate_n(void *p, ::std::size_t n) noexcept
	requires(!has_status)
{
	if constexpr (secure_clear)
	{
		FAST_IO_IF_NOT_CONSTEVAL
		{
			if (p != nullptr)
			{
				::fast_io::freestanding::bytes_secure_clear_n(reinterpret_cast<::std::byte *>(p), n);
			}
		}
	}
#if __cpp_constexpr_dynamic_alloc >= 201907L
	if (__builtin_is_constant_evaluated())
	{
#if FAST_IO_HAS_BUILTIN(__builtin_operator_delete)
		__builtin_operator_delete(p);
#else
		::operator delete(p);
#endif
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
			static_assert(::fast_io::details::has_deallocate_n_impl<alloc>);
		}
	}
}

template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline constexpr void *
allocate_aligned_conditional_zero_impl(::std::size_t alignment, ::std::size_t n, bool zero)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status)
{
	FAST_IO_IF_CONSTEVAL
	{
		auto p{
#if FAST_IO_HAS_BUILTIN(__builtin_operator_new)
			__builtin_operator_new(n)
#else
			::operator new(n)
#endif
		};
		if (zero)
		{
			::fast_io::freestanding::bytes_clear_n(static_cast<::std::byte *>(p), n);
		}
		return p;
	}
	if constexpr (::fast_io::details::has_allocate_aligned_conditional_zero_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::allocate_aligned_conditional_zero(alignment, n, zero);
	}
	else if constexpr (::fast_io::details::has_allocate_aligned_conditional_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::allocate_aligned_conditional_zero_at_least(alignment, n, zero).ptr;
	}
	else if constexpr (::fast_io::details::native_allocate_aligned_has_ops<alloc, throwing>)
	{
		constexpr bool has_none_zero_ops{::fast_io::details::native_allocate_aligned_has_none_zero_ops<alloc, throwing>};
		constexpr bool has_zero_ops{::fast_io::details::native_allocate_aligned_has_zero_ops<alloc, throwing>};
		if constexpr (!has_none_zero_ops && has_zero_ops)
		{
			if constexpr (::fast_io::details::has_allocate_aligned_zero_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::allocate_aligned_zero(alignment, n);
			}
			else
			{
				return dispatch<throwing>::allocate_aligned_zero_at_least(alignment, n).ptr;
			}
		}
		else if constexpr (has_none_zero_ops && !has_zero_ops)
		{
			void *ptr;
			if constexpr (::fast_io::details::has_allocate_aligned_mode_impl<alloc, throwing>)
			{
				ptr = dispatch<throwing>::allocate_aligned(alignment, n);
			}
			else
			{
				ptr = dispatch<throwing>::allocate_aligned_at_least(alignment, n).ptr;
			}
			if (zero)
			{
				::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(ptr), n);
			}
			return ptr;
		}
		else if constexpr (has_none_zero_ops && has_zero_ops)
		{
			void *ptr;
			if (zero)
			{
				if constexpr (::fast_io::details::has_allocate_aligned_zero_mode_impl<alloc, throwing>)
				{
					ptr = dispatch<throwing>::allocate_aligned_zero(alignment, n);
				}
				else
				{
					ptr = dispatch<throwing>::allocate_aligned_at_least(alignment, n).ptr;
				}
			}
			else
			{
				if constexpr (::fast_io::details::has_allocate_aligned_mode_impl<alloc, throwing>)
				{
					ptr = dispatch<throwing>::allocate_aligned_zero(alignment, n);
				}
				else
				{
					ptr = dispatch<throwing>::allocate_aligned_at_least(alignment, n).ptr;
				}
			}
			return ptr;
		}
		else
		{
			::fast_io::fast_terminate();
		}
	}
	else
	{
		return ::fast_io::details::allocator_pointer_aligned_impl<alloc, throwing>(alignment, n, zero);
	}
}
static inline constexpr void *allocate_aligned_conditional_zero(::std::size_t alignment, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status)) || (throws_on_allocation_failure && (!has_status && has_native_allocate_try)))
{
	return allocate_aligned_conditional_zero_impl<throws_on_allocation_failure>(alignment, n, zero);
}
static inline constexpr void *allocate_aligned_conditional_zero_die(::std::size_t alignment, ::std::size_t n, bool zero) noexcept
	requires(!has_status)
{
	return allocate_aligned_conditional_zero_impl<false>(alignment, n, zero);
}
static inline constexpr void *allocate_aligned_conditional_zero_try(::std::size_t alignment, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_native_allocate_try)
{
	return allocate_aligned_conditional_zero_impl<true>(alignment, n, zero);
}


template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline constexpr void *
allocate_aligned_impl(::std::size_t alignment, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status)
{
	FAST_IO_IF_CONSTEVAL
	{
		return
#if FAST_IO_HAS_BUILTIN(__builtin_operator_new)
			__builtin_operator_new(n)
#else
			::operator new(n)
#endif
				;
	}
	if constexpr (::fast_io::details::has_allocate_aligned_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::allocate_aligned(alignment, n);
	}
	else
	{
		return allocate_aligned_conditional_zero_impl<throwing>(alignment, n, false);
	}
}
static inline constexpr void *allocate_aligned(::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status)) || (throws_on_allocation_failure && (!has_status && has_native_allocate_try)))
{
	return allocate_aligned_impl<throws_on_allocation_failure>(alignment, n);
}
static inline constexpr void *allocate_aligned_die(::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status)
{
	return allocate_aligned_impl<false>(alignment, n);
}
static inline constexpr void *allocate_aligned_try(::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_native_allocate_try)
{
	return allocate_aligned_impl<true>(alignment, n);
}


template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline constexpr void *
allocate_aligned_zero_impl(::std::size_t alignment, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status)
{
	FAST_IO_IF_CONSTEVAL
	{
		return
#if FAST_IO_HAS_BUILTIN(__builtin_operator_new)
			__builtin_operator_new(n)
#else
			::operator new(n)
#endif
				;
	}
	if constexpr (::fast_io::details::has_allocate_aligned_zero_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::allocate_aligned_zero(alignment, n);
	}
	else
	{
		return allocate_aligned_conditional_zero_impl<throwing>(alignment, n, true);
	}
}
static inline constexpr void *allocate_aligned_zero(::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status)) || (throws_on_allocation_failure && (!has_status && has_native_allocate_try)))
{
	return allocate_aligned_zero_impl<throws_on_allocation_failure>(alignment, n);
}
static inline constexpr void *allocate_aligned_zero_die(::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status)
{
	return allocate_aligned_zero_impl<false>(alignment, n);
}
static inline constexpr void *allocate_aligned_zero_try(::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_native_allocate_try)
{
	return allocate_aligned_zero_impl<true>(alignment, n);
}


static inline constexpr bool has_native_allocate_at_least{
	!has_status && (::fast_io::details::has_allocate_at_least_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_at_least_impl<alloc> ||
					::fast_io::details::has_allocate_zero_at_least_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_zero_at_least_impl<alloc>)};
static inline constexpr bool has_native_allocate_at_least_try{
	!has_status && (::fast_io::details::has_allocate_at_least_try_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_at_least_try_impl<alloc> ||
					::fast_io::details::has_allocate_zero_at_least_try_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_zero_at_least_try_impl<alloc>)};

template <bool throwing>
static inline constexpr allocation_least_result
allocate_at_least_impl(::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status)
{
	FAST_IO_IF_CONSTEVAL
	{
		return {
#if FAST_IO_HAS_BUILTIN(__builtin_operator_new)
			__builtin_operator_new(n)
#else
			::operator new(n)
#endif
				,
			n};
	}
	else
	{
		if constexpr (::fast_io::details::has_allocate_at_least_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::allocate_at_least(n);
		}
		else
		{
			return {allocate_impl<throwing>(n), n};
		}
	}
}
static inline constexpr ::fast_io::allocation_least_result allocate_at_least(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status)) || (throws_on_allocation_failure && (!has_status && has_native_allocate_try)))
{
	return allocate_at_least_impl<throws_on_allocation_failure>(n);
}
static inline constexpr ::fast_io::allocation_least_result allocate_at_least_die(::std::size_t n) noexcept
	requires(!has_status)
{
	return allocate_at_least_impl<false>(n);
}
static inline constexpr ::fast_io::allocation_least_result allocate_at_least_try(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_native_allocate_try)
{
	return allocate_at_least_impl<true>(n);
}


template <bool throwing>
static inline constexpr allocation_least_result
allocate_zero_at_least_impl(::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status)
{
	FAST_IO_IF_CONSTEVAL
	{
		return {
#if FAST_IO_HAS_BUILTIN(__builtin_operator_new)
			__builtin_operator_new(n)
#else
			::operator new(n)
#endif
				,
			n};
	}
	else
	{
		if constexpr (::fast_io::details::has_allocate_zero_at_least_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::allocate_zero_at_least(n);
		}
		else
		{
			return allocate_conditional_zero_at_least_impl<throwing>(n, true);
		}
	}
}
static inline constexpr ::fast_io::allocation_least_result allocate_zero_at_least(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status)) || (throws_on_allocation_failure && (!has_status && has_native_allocate_try)))
{
	return allocate_zero_at_least_impl<throws_on_allocation_failure>(n);
}
static inline constexpr ::fast_io::allocation_least_result allocate_zero_at_least_die(::std::size_t n) noexcept
	requires(!has_status)
{
	return allocate_zero_at_least_impl<false>(n);
}
static inline constexpr ::fast_io::allocation_least_result allocate_zero_at_least_try(::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_native_allocate_try)
{
	return allocate_zero_at_least_impl<true>(n);
}


template <bool throwing>
static inline constexpr allocation_least_result
allocate_conditional_zero_at_least_impl(::std::size_t n, bool zero)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status)
{
	FAST_IO_IF_CONSTEVAL
	{
		auto p{::operator new(n)};
		if (zero)
		{
			::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(p), n);
		}
		return {p, n};
	}
	else
	{
		if constexpr (::fast_io::details::has_allocate_conditional_zero_at_least_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::allocate_conditional_zero_at_least(n, zero);
		}
		if constexpr (::fast_io::details::has_allocate_conditional_zero_mode_impl<alloc, throwing>)
		{
			return {dispatch<throwing>::allocate_conditional_zero(n, zero), n};
		}
		else if constexpr (::fast_io::details::has_allocate_aligned_conditional_zero_at_least_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::allocate_aligned_conditional_zero_at_least(default_alignment, n, zero);
		}
		else if constexpr (::fast_io::details::has_allocate_aligned_conditional_zero_mode_impl<alloc, throwing>)
		{
			return {dispatch<throwing>::allocate_aligned_conditional_zero(default_alignment, n, zero), n};
		}
		else
		{
			constexpr bool has_none_zero_ops{::fast_io::details::native_allocate_has_none_zero_ops<alloc, throwing>};
			constexpr bool has_zero_ops{::fast_io::details::native_allocate_has_zero_ops<alloc, throwing>};
			if constexpr (!has_none_zero_ops && !has_zero_ops)
			{
				::fast_io::fast_terminate();
#if __has_cpp_attribute(unreachable)
				[[unreachable]];
#endif
			}
			else if constexpr (!has_none_zero_ops && has_zero_ops)
			{
				if constexpr (::fast_io::details::has_allocate_zero_at_least_mode_impl<alloc, throwing>)
				{
					return dispatch<throwing>::allocate_zero_at_least(n);
				}
				else if constexpr (::fast_io::details::has_allocate_zero_mode_impl<alloc, throwing>)
				{
					return {dispatch<throwing>::allocate_zero(n), n};
				}
				else if constexpr (::fast_io::details::has_allocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
				{
					return dispatch<throwing>::allocate_aligned_zero_at_least(default_alignment, n);
				}
				else if constexpr (::fast_io::details::has_allocate_aligned_zero_mode_impl<alloc, throwing>)
				{
					return {dispatch<throwing>::allocate_aligned_zero(default_alignment, n), n};
				}
			}
			else if constexpr (has_none_zero_ops && !has_zero_ops)
			{
				::fast_io::allocation_least_result res;
				if constexpr (::fast_io::details::has_allocate_at_least_mode_impl<alloc, throwing>)
				{
					res = dispatch<throwing>::allocate_at_least(n);
				}
				else if constexpr (::fast_io::details::has_allocate_mode_impl<alloc, throwing>)
				{
					res = {dispatch<throwing>::allocate(n), n};
				}
				else if constexpr (::fast_io::details::has_allocate_aligned_at_least_mode_impl<alloc, throwing>)
				{
					res = dispatch<throwing>::allocate_aligned_at_least(default_alignment, n);
				}
				else if constexpr (::fast_io::details::has_allocate_aligned_mode_impl<alloc, throwing>)
				{
					res = {dispatch<throwing>::allocate_aligned(default_alignment, n), n};
				}
				if (zero)
				{
					::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(res.ptr), res.count);
				}
				return res;
			}
			else
			{
				::fast_io::allocation_least_result res;
				if (zero)
				{
					if constexpr (::fast_io::details::has_allocate_at_least_mode_impl<alloc, throwing>)
					{
						res = dispatch<throwing>::allocate_at_least(n);
					}
					else if constexpr (::fast_io::details::has_allocate_mode_impl<alloc, throwing>)
					{
						res = {dispatch<throwing>::allocate(n), n};
					}
					else if constexpr (::fast_io::details::has_allocate_aligned_at_least_mode_impl<alloc, throwing>)
					{
						res = dispatch<throwing>::allocate_aligned_at_least(default_alignment, n);
					}
					else if constexpr (::fast_io::details::has_allocate_aligned_mode_impl<alloc, throwing>)
					{
						res = {dispatch<throwing>::allocate_aligned(default_alignment, n), n};
					}
					else
					{
						::fast_io::fast_terminate();
					}
				}
				else
				{
					if constexpr (::fast_io::details::has_allocate_zero_at_least_mode_impl<alloc, throwing>)
					{
						res = dispatch<throwing>::allocate_zero_at_least(n);
					}
					else if constexpr (::fast_io::details::has_allocate_zero_mode_impl<alloc, throwing>)
					{
						res = {dispatch<throwing>::allocate_zero(n), n};
					}
					else if constexpr (::fast_io::details::has_allocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
					{
						res = dispatch<throwing>::allocate_aligned_zero_at_least(default_alignment, n);
					}
					else if constexpr (::fast_io::details::has_allocate_aligned_zero_mode_impl<alloc, throwing>)
					{
						res = {dispatch<throwing>::allocate_aligned_zero(default_alignment, n), n};
					}
					else
					{
						::fast_io::fast_terminate();
					}
				}
				return res;
			}
		}
	}
}
static inline constexpr ::fast_io::allocation_least_result allocate_conditional_zero_at_least(::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status)) || (throws_on_allocation_failure && (!has_status && has_native_allocate_try)))
{
	return allocate_conditional_zero_at_least_impl<throws_on_allocation_failure>(n, zero);
}
static inline constexpr ::fast_io::allocation_least_result allocate_conditional_zero_at_least_die(::std::size_t n, bool zero) noexcept
	requires(!has_status)
{
	return allocate_conditional_zero_at_least_impl<false>(n, zero);
}
static inline constexpr ::fast_io::allocation_least_result allocate_conditional_zero_at_least_try(::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_native_allocate_try)
{
	return allocate_conditional_zero_at_least_impl<true>(n, zero);
}

template <bool throwing>
static inline constexpr allocation_least_result
allocate_aligned_at_least_impl(::std::size_t alignment, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status)
{
	FAST_IO_IF_CONSTEVAL
	{
		return allocate_aligned_conditional_zero_at_least_impl<throwing>(alignment, n, false);
	}
	else
	{
		if constexpr (::fast_io::details::has_allocate_aligned_at_least_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::allocate_aligned_at_least(alignment, n);
		}
		else
		{
			return allocate_aligned_conditional_zero_at_least_impl<throwing>(alignment, n, false);
		}
	}
}
static inline constexpr ::fast_io::allocation_least_result allocate_aligned_at_least(::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status)) || (throws_on_allocation_failure && (!has_status && has_native_allocate_try)))
{
	return allocate_aligned_at_least_impl<throws_on_allocation_failure>(alignment, n);
}
static inline constexpr ::fast_io::allocation_least_result allocate_aligned_at_least_die(::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status)
{
	return allocate_aligned_at_least_impl<false>(alignment, n);
}
static inline constexpr ::fast_io::allocation_least_result allocate_aligned_at_least_try(::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_native_allocate_try)
{
	return allocate_aligned_at_least_impl<true>(alignment, n);
}


template <bool throwing>
static inline constexpr allocation_least_result
allocate_aligned_zero_at_least_impl(::std::size_t alignment, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status)
{
	FAST_IO_IF_CONSTEVAL
	{
		return allocate_aligned_conditional_zero_at_least_impl<throwing>(alignment, n, true);
	}
	else
	{
		if constexpr (::fast_io::details::has_allocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::allocate_aligned_zero_at_least(alignment, n);
		}
		else
		{
			return allocate_aligned_conditional_zero_at_least_impl<throwing>(alignment, n, true);
		}
	}
}
static inline constexpr ::fast_io::allocation_least_result allocate_aligned_zero_at_least(::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status)) || (throws_on_allocation_failure && (!has_status && has_native_allocate_try)))
{
	return allocate_aligned_zero_at_least_impl<throws_on_allocation_failure>(alignment, n);
}
static inline constexpr ::fast_io::allocation_least_result allocate_aligned_zero_at_least_die(::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status)
{
	return allocate_aligned_zero_at_least_impl<false>(alignment, n);
}
static inline constexpr ::fast_io::allocation_least_result allocate_aligned_zero_at_least_try(::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_native_allocate_try)
{
	return allocate_aligned_zero_at_least_impl<true>(alignment, n);
}

template <bool throwing>
static inline constexpr allocation_least_result
allocate_aligned_conditional_zero_at_least_impl(::std::size_t alignment, ::std::size_t n, bool zero)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status)
{
	FAST_IO_IF_CONSTEVAL
	{
		auto p{::operator new(n)};
		if (zero)
		{
			::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(p), n);
		}
		return {p, n};
	}
	if constexpr (::fast_io::details::has_allocate_aligned_conditional_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::allocate_aligned_conditional_zero_at_least(alignment, n, zero);
	}
	else if constexpr (::fast_io::details::has_allocate_aligned_conditional_zero_mode_impl<alloc, throwing>)
	{
		return {dispatch<throwing>::allocate_aligned_conditional_zero(alignment, n, zero), n};
	}
	else if constexpr (::fast_io::details::native_allocate_aligned_has_ops<alloc, throwing>)
	{
		constexpr bool has_none_zero_ops{::fast_io::details::native_allocate_aligned_has_none_zero_ops<alloc, throwing>};
		constexpr bool has_zero_ops{::fast_io::details::native_allocate_aligned_has_zero_ops<alloc, throwing>};
		if constexpr (!has_none_zero_ops && has_zero_ops)
		{
			if constexpr (::fast_io::details::has_allocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::allocate_aligned_zero_at_least(alignment, n);
			}
			else
			{
				return {dispatch<throwing>::allocate_aligned_zero(alignment, n), n};
			}
		}
		else if constexpr (has_none_zero_ops && !has_zero_ops)
		{
			::fast_io::allocation_least_result res;
			if constexpr (::fast_io::details::has_allocate_aligned_at_least_mode_impl<alloc, throwing>)
			{
				res = dispatch<throwing>::allocate_aligned_at_least(alignment, n);
			}
			else
			{
				res = {dispatch<throwing>::allocate_aligned(alignment, n), n};
			}
			if (zero)
			{
				::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(res.ptr), res.count);
			}
			return res;
		}
		else if constexpr (has_none_zero_ops && has_zero_ops)
		{
			::fast_io::allocation_least_result res;
			if (zero)
			{
				if constexpr (::fast_io::details::has_allocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
				{
					res = dispatch<throwing>::allocate_aligned_zero_at_least(alignment, n);
				}
				else
				{
					res = {dispatch<throwing>::allocate_aligned_zero(alignment, n), n};
				}
			}
			else
			{
				if constexpr (::fast_io::details::has_allocate_aligned_at_least_mode_impl<alloc, throwing>)
				{
					res = dispatch<throwing>::allocate_aligned_at_least(alignment, n);
				}
				else
				{
					res = {dispatch<throwing>::allocate_aligned(alignment, n), n};
				}
			}
			return res;
		}
		else
		{
			::fast_io::fast_terminate();
		}
	}
	else
	{
		return ::fast_io::details::allocator_pointer_aligned_at_least_impl<alloc, throwing>(default_alignment, n, zero);
	}
}
static inline constexpr ::fast_io::allocation_least_result allocate_aligned_conditional_zero_at_least(::std::size_t alignment, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status)) || (throws_on_allocation_failure && (!has_status && has_native_allocate_try)))
{
	return allocate_aligned_conditional_zero_at_least_impl<throws_on_allocation_failure>(alignment, n, zero);
}
static inline constexpr ::fast_io::allocation_least_result allocate_aligned_conditional_zero_at_least_die(::std::size_t alignment, ::std::size_t n, bool zero) noexcept
	requires(!has_status)
{
	return allocate_aligned_conditional_zero_at_least_impl<false>(alignment, n, zero);
}
static inline constexpr ::fast_io::allocation_least_result allocate_aligned_conditional_zero_at_least_try(::std::size_t alignment, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_native_allocate_try)
{
	return allocate_aligned_conditional_zero_at_least_impl<true>(alignment, n, zero);
}


static inline constexpr bool has_reallocate_aligned = (::fast_io::details::has_reallocate_aligned_impl<alloc> ||
													   ::fast_io::details::has_reallocate_aligned_at_least_impl<alloc> ||
													   ::fast_io::details::has_reallocate_aligned_zero_impl<alloc> ||
													   ::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc> ||
													   ::fast_io::details::has_reallocate_aligned_conditional_zero_impl<alloc> ||
													   ::fast_io::details::has_reallocate_aligned_conditional_zero_at_least_impl<alloc>);
static inline constexpr bool has_reallocate_aligned_try = (::fast_io::details::has_reallocate_aligned_try_impl<alloc> ||
														   ::fast_io::details::has_reallocate_aligned_at_least_try_impl<alloc> ||
														   ::fast_io::details::has_reallocate_aligned_zero_try_impl<alloc> ||
														   ::fast_io::details::has_reallocate_aligned_zero_at_least_try_impl<alloc> ||
														   ::fast_io::details::has_reallocate_aligned_conditional_zero_try_impl<alloc> ||
														   ::fast_io::details::has_reallocate_aligned_conditional_zero_at_least_try_impl<alloc>);

template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *
reallocate_aligned_conditional_zero_impl(void *p, ::std::size_t alignment, ::std::size_t n, bool zero)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status && has_reallocate_aligned)
{
	if constexpr (::fast_io::details::has_reallocate_aligned_conditional_zero_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_conditional_zero(p, alignment, n, zero);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_conditional_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_conditional_zero_at_least(p, alignment, n, zero).ptr;
	}
	else if (zero)
	{
		if constexpr (::fast_io::details::has_reallocate_aligned_zero_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::reallocate_aligned_zero(p, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::reallocate_aligned_zero_at_least(p, alignment, n).ptr;
		}
		else
		{
			::fast_io::fast_terminate();
		}
	}
	else
	{
		if constexpr (::fast_io::details::has_reallocate_aligned_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::reallocate_aligned(p, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_at_least_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::reallocate_aligned_at_least(p, alignment, n).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::reallocate_aligned_zero(p, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::reallocate_aligned_zero_at_least(p, alignment, n).ptr;
		}
		else
		{
			::fast_io::fast_terminate();
		}
	}
}
static inline constexpr void *reallocate_aligned_conditional_zero(void *p, ::std::size_t alignment, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status && has_reallocate_aligned)) || (throws_on_allocation_failure && (!has_status && has_reallocate_aligned_try)))
{
	return reallocate_aligned_conditional_zero_impl<throws_on_allocation_failure>(p, alignment, n, zero);
}
static inline constexpr void *reallocate_aligned_conditional_zero_die(void *p, ::std::size_t alignment, ::std::size_t n, bool zero) noexcept
	requires(!has_status && has_reallocate_aligned)
{
	return reallocate_aligned_conditional_zero_impl<false>(p, alignment, n, zero);
}
static inline constexpr void *reallocate_aligned_conditional_zero_try(void *p, ::std::size_t alignment, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_reallocate_aligned_try)
{
	return reallocate_aligned_conditional_zero_impl<true>(p, alignment, n, zero);
}


template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *
reallocate_aligned_impl(void *p, ::std::size_t alignment, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status && has_reallocate_aligned)
{
	if constexpr (::fast_io::details::has_reallocate_aligned_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned(p, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_at_least(p, alignment, n).ptr;
	}
	else
	{
		return reallocate_aligned_conditional_zero_impl<throwing>(p, alignment, n, false);
	}
}
static inline constexpr void *reallocate_aligned(void *p, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status && has_reallocate_aligned)) || (throws_on_allocation_failure && (!has_status && has_reallocate_aligned_try)))
{
	return reallocate_aligned_impl<throws_on_allocation_failure>(p, alignment, n);
}
static inline constexpr void *reallocate_aligned_die(void *p, ::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status && has_reallocate_aligned)
{
	return reallocate_aligned_impl<false>(p, alignment, n);
}
static inline constexpr void *reallocate_aligned_try(void *p, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_reallocate_aligned_try)
{
	return reallocate_aligned_impl<true>(p, alignment, n);
}


static inline constexpr bool has_reallocate_aligned_zero = (::fast_io::details::has_reallocate_aligned_zero_impl<alloc> ||
															::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>);
static inline constexpr bool has_reallocate_aligned_zero_try = (::fast_io::details::has_reallocate_aligned_zero_try_impl<alloc> ||
																::fast_io::details::has_reallocate_aligned_zero_at_least_try_impl<alloc>);
template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *
reallocate_aligned_zero_impl(void *p, ::std::size_t alignment, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status && has_reallocate_aligned_zero)
{
	if constexpr (::fast_io::details::has_reallocate_aligned_zero_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_zero(p, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_zero_at_least(p, alignment, n).ptr;
	}
	else
	{
		return reallocate_aligned_conditional_zero_impl<throwing>(p, alignment, n, true);
	}
}
static inline constexpr void *reallocate_aligned_zero(void *p, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status && has_reallocate_aligned_zero)) || (throws_on_allocation_failure && (!has_status && has_reallocate_aligned_zero_try)))
{
	return reallocate_aligned_zero_impl<throws_on_allocation_failure>(p, alignment, n);
}
static inline constexpr void *reallocate_aligned_zero_die(void *p, ::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status && has_reallocate_aligned_zero)
{
	return reallocate_aligned_zero_impl<false>(p, alignment, n);
}
static inline constexpr void *reallocate_aligned_zero_try(void *p, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_reallocate_aligned_zero_try)
{
	return reallocate_aligned_zero_impl<true>(p, alignment, n);
}


template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *
reallocate_aligned_n_conditional_zero_impl(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n, bool zero)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status)
{
	if (p != nullptr && oldn == n)
	{
		return p;
	}
	FAST_IO_IF_CONSTEVAL
	{
		auto newptr{::operator new(n)};
		if (p != nullptr)
		{
			if (n)
			{
				::std::size_t copyn{oldn < n ? oldn : n};
				::fast_io::freestanding::nonoverlapped_bytes_copy_n(reinterpret_cast<::std::byte const *>(p), copyn, reinterpret_cast<::std::byte *>(newptr));
			}
			::operator delete(p);
		}
		if (zero && oldn < n)
		{
			::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(newptr) + oldn, n - oldn);
		}
		return newptr;
	}
	else
	{
		if constexpr (::fast_io::details::has_reallocate_aligned_n_conditional_zero_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::reallocate_aligned_n_conditional_zero(p, oldn, alignment, n, zero);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_n_conditional_zero_at_least_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::reallocate_aligned_n_conditional_zero_at_least(p, oldn, alignment, n, zero).ptr;
		}
		else if (zero)
		{
			if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_aligned_zero_n(p, oldn, alignment, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_aligned_zero_n_at_least(p, oldn, alignment, n).ptr;
			}
			else
			{
				auto newptr{reallocate_aligned_n_impl<throwing>(p, oldn, alignment, n)};
				if (oldn < n)
				{
					::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(newptr) + oldn, n - oldn);
				}
				return newptr;
			}
		}
		else
		{
			if constexpr (::fast_io::details::has_reallocate_aligned_n_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_aligned_n(p, oldn, alignment, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_n_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_aligned_n_at_least(p, oldn, alignment, n).ptr;
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_aligned(p, alignment, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_aligned_at_least(p, alignment, n).ptr;
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_aligned_zero_n(p, oldn, alignment, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_aligned_zero_n_at_least(p, oldn, alignment, n).ptr;
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_zero_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_aligned_zero(p, alignment, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::reallocate_aligned_zero_at_least(p, alignment, n).ptr;
			}
			else
			{
				if constexpr (::fast_io::details::has_reallocate_n_mode_impl<alloc, throwing> ||
							  ::fast_io::details::has_reallocate_zero_n_mode_impl<alloc, throwing> ||
							  ::fast_io::details::has_reallocate_mode_impl<alloc, throwing> ||
							  ::fast_io::details::has_reallocate_zero_mode_impl<alloc, throwing>)
				{
					if (alignment <= default_alignment)
					{
						return reallocate_n_impl<throwing>(p, oldn, n);
					}
				}
				auto newptr{::fast_io::details::allocator_pointer_aligned_impl<alloc, throwing>(alignment, n, false)};
				if (p != nullptr)
				{
					if (n)
					{
						bool moren{oldn < n};
						::std::size_t copyn{moren ? oldn : n};
						::fast_io::freestanding::nonoverlapped_bytes_copy_n(reinterpret_cast<::std::byte const *>(p), copyn, reinterpret_cast<::std::byte *>(newptr));
					}
					generic_allocator_adapter::deallocate_aligned_n(p, alignment, oldn);
				}
				return newptr;
			}
		}
	}
}
static inline constexpr void *reallocate_aligned_n_conditional_zero(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status)) || (throws_on_allocation_failure && (!has_status && has_native_allocate_try)))
{
	return reallocate_aligned_n_conditional_zero_impl<throws_on_allocation_failure>(p, oldn, alignment, n, zero);
}
static inline constexpr void *reallocate_aligned_n_conditional_zero_die(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n, bool zero) noexcept
	requires(!has_status)
{
	return reallocate_aligned_n_conditional_zero_impl<false>(p, oldn, alignment, n, zero);
}
static inline constexpr void *reallocate_aligned_n_conditional_zero_try(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_native_allocate_try)
{
	return reallocate_aligned_n_conditional_zero_impl<true>(p, oldn, alignment, n, zero);
}


template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *
reallocate_aligned_n_impl(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status)
{
	if (p != nullptr && oldn == n)
	{
		return p;
	}
	if constexpr (::fast_io::details::has_reallocate_aligned_n_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_n(p, oldn, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_n_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_n_at_least(p, oldn, alignment, n).ptr;
	}
	else
	{
		return reallocate_aligned_n_conditional_zero_impl<throwing>(p, oldn, alignment, n, false);
	}
}
static inline constexpr void *reallocate_aligned_n(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status)) || (throws_on_allocation_failure && (!has_status && has_native_allocate_try)))
{
	return reallocate_aligned_n_impl<throws_on_allocation_failure>(p, oldn, alignment, n);
}
static inline constexpr void *reallocate_aligned_n_die(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status)
{
	return reallocate_aligned_n_impl<false>(p, oldn, alignment, n);
}
static inline constexpr void *reallocate_aligned_n_try(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_native_allocate_try)
{
	return reallocate_aligned_n_impl<true>(p, oldn, alignment, n);
}


template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *
reallocate_aligned_zero_n_impl(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status)
{
	if (p != nullptr && oldn == n)
	{
		return p;
	}
	if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_zero_n(p, oldn, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_zero_n_at_least(p, oldn, alignment, n).ptr;
	}
	else
	{
		auto newptr{reallocate_aligned_n_impl<throwing>(p, oldn, alignment, n)};
		if (oldn < n)
		{
			::std::size_t const to_zero_bytes{static_cast<::std::size_t>(n - oldn)};
			::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(newptr) + oldn, to_zero_bytes);
		}
		return newptr;
	}
}
static inline constexpr void *reallocate_aligned_zero_n(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status)) || (throws_on_allocation_failure && (!has_status && has_native_allocate_try)))
{
	return reallocate_aligned_zero_n_impl<throws_on_allocation_failure>(p, oldn, alignment, n);
}
static inline constexpr void *reallocate_aligned_zero_n_die(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status)
{
	return reallocate_aligned_zero_n_impl<false>(p, oldn, alignment, n);
}
static inline constexpr void *reallocate_aligned_zero_n_try(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_native_allocate_try)
{
	return reallocate_aligned_zero_n_impl<true>(p, oldn, alignment, n);
}


static inline constexpr bool has_native_reallocate_at_least = (has_reallocate &&
															   (::fast_io::details::has_reallocate_aligned_at_least_impl<alloc> ||
																::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>));
static inline constexpr bool has_native_reallocate_at_least_try = (has_reallocate &&
																   (::fast_io::details::has_reallocate_aligned_at_least_try_impl<alloc> ||
																	::fast_io::details::has_reallocate_aligned_zero_at_least_try_impl<alloc>));
template <bool throwing>
static inline ::fast_io::allocation_least_result
reallocate_at_least_impl(void *p, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status && has_reallocate)
{
	if constexpr (::fast_io::details::has_reallocate_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_at_least(p, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_at_least(p, default_alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_zero_at_least(p, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_zero_at_least(p, default_alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_mode_impl<alloc, throwing>)
	{
		return {dispatch<throwing>::reallocate(p, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_mode_impl<alloc, throwing>)
	{
		return {dispatch<throwing>::reallocate_aligned(p, default_alignment, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_zero_mode_impl<alloc, throwing>)
	{
		return {dispatch<throwing>::reallocate_zero(p, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_mode_impl<alloc, throwing>)
	{
		return {dispatch<throwing>::reallocate_aligned_zero(p, default_alignment, n), n};
	}
}
static inline constexpr ::fast_io::allocation_least_result reallocate_at_least(void *p, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status && has_reallocate)) || (throws_on_allocation_failure && (!has_status && has_reallocate_try)))
{
	return reallocate_at_least_impl<throws_on_allocation_failure>(p, n);
}
static inline constexpr ::fast_io::allocation_least_result reallocate_at_least_die(void *p, ::std::size_t n) noexcept
	requires(!has_status && has_reallocate)
{
	return reallocate_at_least_impl<false>(p, n);
}
static inline constexpr ::fast_io::allocation_least_result reallocate_at_least_try(void *p, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_reallocate_try)
{
	return reallocate_at_least_impl<true>(p, n);
}


static inline constexpr bool has_native_reallocate_zero_at_least = (has_reallocate_zero &&
																	(::fast_io::details::has_reallocate_zero_at_least_impl<alloc> ||
																	 ::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>));
static inline constexpr bool has_native_reallocate_zero_at_least_try = (has_reallocate_zero &&
																		(::fast_io::details::has_reallocate_zero_at_least_try_impl<alloc> ||
																		 ::fast_io::details::has_reallocate_aligned_zero_at_least_try_impl<alloc>));

template <bool throwing>
static inline ::fast_io::allocation_least_result
reallocate_zero_at_least_impl(void *p, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status && has_reallocate)
{
	if constexpr (::fast_io::details::has_reallocate_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_zero_at_least(p, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_zero_at_least(p, default_alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_zero_mode_impl<alloc, throwing>)
	{
		return {dispatch<throwing>::reallocate_zero(p, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_mode_impl<alloc, throwing>)
	{
		return {dispatch<throwing>::reallocate_aligned(p, default_alignment, n), n};
	}
}
static inline constexpr ::fast_io::allocation_least_result reallocate_zero_at_least(void *p, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status && has_reallocate)) || (throws_on_allocation_failure && (!has_status && has_reallocate_try)))
{
	return reallocate_zero_at_least_impl<throws_on_allocation_failure>(p, n);
}
static inline constexpr ::fast_io::allocation_least_result reallocate_zero_at_least_die(void *p, ::std::size_t n) noexcept
	requires(!has_status && has_reallocate)
{
	return reallocate_zero_at_least_impl<false>(p, n);
}
static inline constexpr ::fast_io::allocation_least_result reallocate_zero_at_least_try(void *p, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_reallocate_try)
{
	return reallocate_zero_at_least_impl<true>(p, n);
}


template <bool throwing>
static inline ::fast_io::allocation_least_result
reallocate_n_at_least_impl(void *p, ::std::size_t oldn, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status)
{
	if constexpr (::fast_io::details::has_reallocate_n_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_n_at_least(p, oldn, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_n_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_n_at_least(p, oldn, default_alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_zero_n_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_zero_n_at_least(p, oldn, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_zero_n_at_least(p, oldn, default_alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_at_least(p, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_at_least(p, default_alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_zero_at_least(p, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_zero_at_least(p, default_alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_n_mode_impl<alloc, throwing>)
	{
		return {dispatch<throwing>::reallocate_n(p, oldn, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_n_mode_impl<alloc, throwing>)
	{
		return {dispatch<throwing>::reallocate_aligned_n(p, oldn, default_alignment, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_zero_n_mode_impl<alloc, throwing>)
	{
		return {dispatch<throwing>::reallocate_zero_n(p, oldn, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_mode_impl<alloc, throwing>)
	{
		return {dispatch<throwing>::reallocate_aligned_zero_n(p, oldn, default_alignment, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_mode_impl<alloc, throwing>)
	{
		return {dispatch<throwing>::reallocate(p, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_mode_impl<alloc, throwing>)
	{
		return {dispatch<throwing>::reallocate_aligned(p, default_alignment, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_zero_mode_impl<alloc, throwing>)
	{
		return {dispatch<throwing>::reallocate_zero(p, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_mode_impl<alloc, throwing>)
	{
		return {dispatch<throwing>::reallocate_aligned_zero(p, default_alignment, n), n};
	}
	else
	{
		auto newres{allocate_at_least_impl<throwing>(n)};
		auto newptr{newres.ptr};
		if (p != nullptr)
		{
			if (n)
			{
				if (oldn < n)
				{
					n = oldn;
				}
				::fast_io::freestanding::nonoverlapped_bytes_copy_n(reinterpret_cast<::std::byte const *>(p), n, reinterpret_cast<::std::byte *>(newptr));
			}
			generic_allocator_adapter::deallocate_n(p, oldn);
		}
		return newres;
	}
}
static inline constexpr ::fast_io::allocation_least_result reallocate_n_at_least(void *p, ::std::size_t oldn, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status)) || (throws_on_allocation_failure && (!has_status && has_native_allocate_try)))
{
	return reallocate_n_at_least_impl<throws_on_allocation_failure>(p, oldn, n);
}
static inline constexpr ::fast_io::allocation_least_result reallocate_n_at_least_die(void *p, ::std::size_t oldn, ::std::size_t n) noexcept
	requires(!has_status)
{
	return reallocate_n_at_least_impl<false>(p, oldn, n);
}
static inline constexpr ::fast_io::allocation_least_result reallocate_n_at_least_try(void *p, ::std::size_t oldn, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_native_allocate_try)
{
	return reallocate_n_at_least_impl<true>(p, oldn, n);
}


template <bool throwing>
static inline ::fast_io::allocation_least_result
reallocate_zero_n_at_least_impl(void *p, ::std::size_t oldn, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status)
{
	if constexpr (::fast_io::details::has_reallocate_zero_n_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_zero_n_at_least(p, oldn, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_zero_n_at_least(p, oldn, default_alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_zero_at_least(p, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_zero_at_least(p, default_alignment, n);
	}
	else
	{
		auto newres{reallocate_n_at_least_impl<throwing>(p, oldn, n)};
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

static inline constexpr ::fast_io::allocation_least_result reallocate_zero_n_at_least(void *p, ::std::size_t oldn, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status)) || (throws_on_allocation_failure && (!has_status && has_native_allocate_try)))
{
	return reallocate_zero_n_at_least_impl<throws_on_allocation_failure>(p, oldn, n);
}
static inline constexpr ::fast_io::allocation_least_result reallocate_zero_n_at_least_die(void *p, ::std::size_t oldn, ::std::size_t n) noexcept
	requires(!has_status)
{
	return reallocate_zero_n_at_least_impl<false>(p, oldn, n);
}
static inline constexpr ::fast_io::allocation_least_result reallocate_zero_n_at_least_try(void *p, ::std::size_t oldn, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_native_allocate_try)
{
	return reallocate_zero_n_at_least_impl<true>(p, oldn, n);
}

static inline constexpr bool has_native_reallocate_aligned_at_least = (has_reallocate_aligned &&
																	   (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc> ||
																		::fast_io::details::has_reallocate_aligned_at_least_impl<alloc>));
static inline constexpr bool has_native_reallocate_aligned_at_least_try = (has_reallocate_aligned &&
																		   (::fast_io::details::has_reallocate_aligned_zero_at_least_try_impl<alloc> ||
																			::fast_io::details::has_reallocate_aligned_at_least_try_impl<alloc>));

template <bool throwing>
static inline ::fast_io::allocation_least_result
reallocate_aligned_at_least_impl(void *p, ::std::size_t alignment, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status && has_reallocate_aligned_zero)
{
	if constexpr (::fast_io::details::has_reallocate_aligned_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_at_least(p, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_zero_at_least(p, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_mode_impl<alloc, throwing>)
	{
		return {dispatch<throwing>::reallocate_aligned(p, alignment, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_mode_impl<alloc, throwing>)
	{
		return {dispatch<throwing>::reallocate_aligned_zero(p, alignment, n), n};
	}
}
static inline constexpr ::fast_io::allocation_least_result reallocate_aligned_at_least(void *p, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status && has_reallocate_aligned_zero)) || (throws_on_allocation_failure && (!has_status && has_reallocate_aligned_zero_try)))
{
	return reallocate_aligned_at_least_impl<throws_on_allocation_failure>(p, alignment, n);
}
static inline constexpr ::fast_io::allocation_least_result reallocate_aligned_at_least_die(void *p, ::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status && has_reallocate_aligned_zero)
{
	return reallocate_aligned_at_least_impl<false>(p, alignment, n);
}
static inline constexpr ::fast_io::allocation_least_result reallocate_aligned_at_least_try(void *p, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_reallocate_aligned_zero_try)
{
	return reallocate_aligned_at_least_impl<true>(p, alignment, n);
}


static inline constexpr bool has_native_reallocate_aligned_zero_at_least = (has_reallocate_aligned_zero &&
																			::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>);
static inline constexpr bool has_native_reallocate_aligned_zero_at_least_try = (has_reallocate_aligned_zero &&
																				::fast_io::details::has_reallocate_aligned_zero_at_least_try_impl<alloc>);

template <bool throwing>
static inline ::fast_io::allocation_least_result
reallocate_aligned_zero_at_least_impl(void *p, ::std::size_t alignment, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status && has_reallocate_aligned_zero)
{
	if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_zero_at_least(p, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_mode_impl<alloc, throwing>)
	{
		return {dispatch<throwing>::reallocate_aligned_zero(p, alignment, n), n};
	}
}
static inline constexpr ::fast_io::allocation_least_result reallocate_aligned_zero_at_least(void *p, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status && has_reallocate_aligned_zero)) || (throws_on_allocation_failure && (!has_status && has_reallocate_aligned_zero_try)))
{
	return reallocate_aligned_zero_at_least_impl<throws_on_allocation_failure>(p, alignment, n);
}
static inline constexpr ::fast_io::allocation_least_result reallocate_aligned_zero_at_least_die(void *p, ::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status && has_reallocate_aligned_zero)
{
	return reallocate_aligned_zero_at_least_impl<false>(p, alignment, n);
}
static inline constexpr ::fast_io::allocation_least_result reallocate_aligned_zero_at_least_try(void *p, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_reallocate_aligned_zero_try)
{
	return reallocate_aligned_zero_at_least_impl<true>(p, alignment, n);
}


static inline constexpr bool has_native_reallocate_aligned_n_at_least = (has_reallocate_aligned &&
																		 (::fast_io::details::has_reallocate_aligned_n_at_least_impl<alloc> ||
																		  ::fast_io::details::has_reallocate_aligned_zero_n_at_least_impl<alloc>));
static inline constexpr bool has_native_reallocate_aligned_n_at_least_try = (has_reallocate_aligned &&
																			 (::fast_io::details::has_reallocate_aligned_n_at_least_try_impl<alloc> ||
																			  ::fast_io::details::has_reallocate_aligned_zero_n_at_least_try_impl<alloc>));
template <bool throwing>
static inline ::fast_io::allocation_least_result
reallocate_aligned_n_at_least_impl(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status)
{
	if constexpr (::fast_io::details::has_reallocate_aligned_n_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_n_at_least(p, oldn, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_at_least(p, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_zero_n_at_least(p, oldn, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_zero_at_least(p, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_mode_impl<alloc, throwing>)
	{
		return {dispatch<throwing>::reallocate_aligned_n(p, oldn, alignment, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_mode_impl<alloc, throwing>)
	{
		return {dispatch<throwing>::reallocate_aligned(p, alignment, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_mode_impl<alloc, throwing>)
	{
		return {dispatch<throwing>::reallocate_aligned_zero_n(p, oldn, alignment, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_mode_impl<alloc, throwing>)
	{
		return {dispatch<throwing>::reallocate_aligned_zero(p, alignment, n), n};
	}
	else
	{
		if constexpr (
			::fast_io::details::has_reallocate_at_least_mode_impl<alloc, throwing> ||
			::fast_io::details::has_reallocate_zero_at_least_mode_impl<alloc, throwing> ||
			::fast_io::details::has_reallocate_n_at_least_mode_impl<alloc, throwing> ||
			::fast_io::details::has_reallocate_zero_n_at_least_mode_impl<alloc, throwing>)
		{
			if (alignment <= default_alignment)
			{
				return reallocate_n_at_least_impl<throwing>(p, oldn, n);
			}
		}
		auto newres{::fast_io::details::allocator_pointer_aligned_at_least_impl<alloc, throwing>(alignment, n, false)};
		auto newptr{newres.ptr};
		if (p != nullptr)
		{
			if (n)
			{
				if (oldn < n)
				{
					n = oldn;
				}
				::fast_io::freestanding::nonoverlapped_bytes_copy_n(reinterpret_cast<::std::byte const *>(p), n, reinterpret_cast<::std::byte *>(newptr));
			}
			generic_allocator_adapter::deallocate_aligned_n(p, alignment, oldn);
		}
		return newres;
	}
}
static inline constexpr ::fast_io::allocation_least_result reallocate_aligned_n_at_least(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status)) || (throws_on_allocation_failure && (!has_status && has_native_allocate_try)))
{
	return reallocate_aligned_n_at_least_impl<throws_on_allocation_failure>(p, oldn, alignment, n);
}
static inline constexpr ::fast_io::allocation_least_result reallocate_aligned_n_at_least_die(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status)
{
	return reallocate_aligned_n_at_least_impl<false>(p, oldn, alignment, n);
}
static inline constexpr ::fast_io::allocation_least_result reallocate_aligned_n_at_least_try(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_native_allocate_try)
{
	return reallocate_aligned_n_at_least_impl<true>(p, oldn, alignment, n);
}


static inline constexpr bool has_native_reallocate_aligned_zero_n_at_least = (has_reallocate_aligned_zero && ::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>);
static inline constexpr bool has_native_reallocate_aligned_zero_n_at_least_try = (has_reallocate_aligned_zero && ::fast_io::details::has_reallocate_aligned_zero_at_least_try_impl<alloc>);

template <bool throwing>
static inline ::fast_io::allocation_least_result reallocate_aligned_zero_n_at_least_impl(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(!has_status)
{
	if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::reallocate_aligned_zero_n_at_least(p, oldn, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_mode_impl<alloc, throwing>)
	{
		return {dispatch<throwing>::reallocate_aligned_zero_n(p, oldn, alignment, n), n};
	}
	else
	{
		auto newres = reallocate_aligned_n_at_least_impl<throwing>(p, oldn, alignment, n);
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
static inline constexpr ::fast_io::allocation_least_result reallocate_aligned_zero_n_at_least(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (!has_status)) || (throws_on_allocation_failure && (!has_status && has_native_allocate_try)))
{
	return reallocate_aligned_zero_n_at_least_impl<throws_on_allocation_failure>(p, oldn, alignment, n);
}
static inline constexpr ::fast_io::allocation_least_result reallocate_aligned_zero_n_at_least_die(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status)
{
	return reallocate_aligned_zero_n_at_least_impl<false>(p, oldn, alignment, n);
}
static inline constexpr ::fast_io::allocation_least_result reallocate_aligned_zero_n_at_least_try(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(!has_status && has_native_allocate_try)
{
	return reallocate_aligned_zero_n_at_least_impl<true>(p, oldn, alignment, n);
}


static inline constexpr bool has_deallocate_aligned = (::fast_io::details::has_deallocate_aligned_impl<alloc> ||
													   ::fast_io::details::has_deallocate_impl<alloc>);
static inline void deallocate_aligned(void *p, ::std::size_t alignment) noexcept
	requires(!has_status && has_deallocate_aligned && !secure_clear)
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
	if constexpr (secure_clear)
	{
		FAST_IO_IF_NOT_CONSTEVAL
		{
			if (p != nullptr)
			{
				::fast_io::freestanding::bytes_secure_clear_n(reinterpret_cast<::std::byte *>(p), n);
			}
		}
	}
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

// Handle-based allocation functions (for allocators with non-empty handle_type)
static inline constexpr bool has_native_handle_allocate{
	has_status && (::fast_io::details::has_handle_allocate_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_aligned_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_zero_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_aligned_zero_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_aligned_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_zero_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_aligned_zero_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_conditional_zero_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_aligned_conditional_zero_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_conditional_zero_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_aligned_conditional_zero_at_least_impl<alloc>)};
static inline constexpr bool has_native_handle_allocate_try{
	has_status && (::fast_io::details::has_handle_allocate_try_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_aligned_try_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_zero_try_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_aligned_zero_try_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_at_least_try_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_aligned_at_least_try_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_zero_at_least_try_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_aligned_zero_at_least_try_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_conditional_zero_try_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_aligned_conditional_zero_try_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_conditional_zero_at_least_try_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_aligned_conditional_zero_at_least_try_impl<alloc>)};

template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *handle_allocate_conditional_zero_impl(handle_type handle, ::std::size_t n, bool zero)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(has_status && has_native_handle_allocate)
{
	FAST_IO_IF_CONSTEVAL
	{
		auto p{::operator new(n)};
		if (zero)
		{
			::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(p), n);
		}
		return p;
	}
	else
	{
		if constexpr (::fast_io::details::has_handle_allocate_conditional_zero_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::handle_allocate_conditional_zero(handle, n, zero);
		}
		else if constexpr (::fast_io::details::has_handle_allocate_conditional_zero_at_least_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::handle_allocate_conditional_zero_at_least(handle, n, zero).ptr;
		}
		else if constexpr (::fast_io::details::has_handle_allocate_aligned_conditional_zero_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::handle_allocate_aligned_conditional_zero(handle, default_alignment, n, zero);
		}
		else if constexpr (::fast_io::details::has_handle_allocate_aligned_conditional_zero_at_least_mode_impl<alloc, throwing>)
		{
			return dispatch<throwing>::handle_allocate_aligned_conditional_zero_at_least(handle, default_alignment, n, zero).ptr;
		}
		else if constexpr (::fast_io::details::has_handle_allocate_mode_impl<alloc, throwing> ||
						   ::fast_io::details::has_handle_allocate_at_least_mode_impl<alloc, throwing> ||
						   ::fast_io::details::has_handle_allocate_aligned_mode_impl<alloc, throwing> ||
						   ::fast_io::details::has_handle_allocate_aligned_at_least_mode_impl<alloc, throwing>)
		{
			if (zero)
			{
				if constexpr (::fast_io::details::has_handle_allocate_zero_mode_impl<alloc, throwing>)
				{
					return dispatch<throwing>::handle_allocate_zero(handle, n);
				}
				else if constexpr (::fast_io::details::has_handle_allocate_zero_at_least_mode_impl<alloc, throwing>)
				{
					return dispatch<throwing>::handle_allocate_zero_at_least(handle, n).ptr;
				}
				else if constexpr (::fast_io::details::has_handle_allocate_aligned_zero_mode_impl<alloc, throwing>)
				{
					return dispatch<throwing>::handle_allocate_aligned_zero(handle, default_alignment, n);
				}
				else if constexpr (::fast_io::details::has_handle_allocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
				{
					return dispatch<throwing>::handle_allocate_aligned_zero_at_least(handle, default_alignment, n).ptr;
				}
				else
				{
					auto p{handle_allocate_impl<throwing>(handle, n)};
					::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(p), n);
					return p;
				}
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
				else if constexpr (::fast_io::details::has_handle_allocate_aligned_mode_impl<alloc, throwing>)
				{
					return dispatch<throwing>::handle_allocate_aligned(handle, default_alignment, n);
				}
				else
				{
					return dispatch<throwing>::handle_allocate_aligned_at_least(handle, default_alignment, n).ptr;
				}
			}
		}
		else
		{
			if constexpr (::fast_io::details::has_handle_allocate_zero_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::handle_allocate_zero(handle, n);
			}
			else if constexpr (::fast_io::details::has_handle_allocate_zero_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::handle_allocate_zero_at_least(handle, n).ptr;
			}
			else if constexpr (::fast_io::details::has_handle_allocate_aligned_zero_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::handle_allocate_aligned_zero(handle, default_alignment, n);
			}
			else if constexpr (::fast_io::details::has_handle_allocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::handle_allocate_aligned_zero_at_least(handle, default_alignment, n).ptr;
			}
			else
			{
				::fast_io::fast_terminate();
			}
		}
	}
}
static inline constexpr void *handle_allocate_conditional_zero(handle_type handle, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (has_status && has_native_handle_allocate)) || (throws_on_allocation_failure && (has_status && has_native_handle_allocate_try)))
{
	return handle_allocate_conditional_zero_impl<throws_on_allocation_failure>(handle, n, zero);
}
static inline constexpr void *handle_allocate_conditional_zero_die(handle_type handle, ::std::size_t n, bool zero) noexcept
	requires(has_status && has_native_handle_allocate)
{
	return handle_allocate_conditional_zero_impl<false>(handle, n, zero);
}
static inline constexpr void *handle_allocate_conditional_zero_try(handle_type handle, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS
	requires(has_status && has_native_handle_allocate_try)
{
	return handle_allocate_conditional_zero_impl<true>(handle, n, zero);
}

template <bool throwing>
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *handle_allocate_aligned_conditional_zero_impl(handle_type handle, ::std::size_t alignment, ::std::size_t n, bool zero)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(has_status && has_native_handle_allocate)
{
	if constexpr (::fast_io::details::has_handle_allocate_aligned_conditional_zero_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::handle_allocate_aligned_conditional_zero(handle, alignment, n, zero);
	}
	else if constexpr (::fast_io::details::has_handle_allocate_aligned_conditional_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::handle_allocate_aligned_conditional_zero_at_least(handle, alignment, n, zero).ptr;
	}
	else
	{
		if (alignment <= default_alignment)
		{
			return handle_allocate_conditional_zero_impl<throwing>(handle, n, zero);
		}
		if (zero)
		{
			if constexpr (::fast_io::details::has_handle_allocate_aligned_zero_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::handle_allocate_aligned_zero(handle, alignment, n);
			}
			else if constexpr (::fast_io::details::has_handle_allocate_aligned_zero_at_least_mode_impl<alloc, throwing>)
			{
				return dispatch<throwing>::handle_allocate_aligned_zero_at_least(handle, alignment, n).ptr;
			}
		}
		if constexpr (::fast_io::details::has_handle_allocate_aligned_mode_impl<alloc, throwing>)
		{
			auto p{dispatch<throwing>::handle_allocate_aligned(handle, alignment, n)};
			if (zero)
			{
				::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(p), n);
			}
			return p;
		}
		else if constexpr (::fast_io::details::has_handle_allocate_aligned_at_least_mode_impl<alloc, throwing>)
		{
			auto p{dispatch<throwing>::handle_allocate_aligned_at_least(handle, alignment, n).ptr};
			if (zero)
			{
				::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(p), n);
			}
			return p;
		}
		else
		{
			return ::fast_io::details::status_allocator_pointer_aligned_impl<alloc, throwing>(handle, alignment, n, zero);
		}
	}
}
static inline constexpr void *handle_allocate_aligned_conditional_zero(handle_type handle, ::std::size_t alignment, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (has_status && has_native_handle_allocate)) || (throws_on_allocation_failure && (has_status && has_native_handle_allocate_try)))
{
	return handle_allocate_aligned_conditional_zero_impl<throws_on_allocation_failure>(handle, alignment, n, zero);
}
static inline constexpr void *handle_allocate_aligned_conditional_zero_die(handle_type handle, ::std::size_t alignment, ::std::size_t n, bool zero) noexcept
	requires(has_status && has_native_handle_allocate)
{
	return handle_allocate_aligned_conditional_zero_impl<false>(handle, alignment, n, zero);
}
static inline constexpr void *handle_allocate_aligned_conditional_zero_try(handle_type handle, ::std::size_t alignment, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS
	requires(has_status && has_native_handle_allocate_try)
{
	return handle_allocate_aligned_conditional_zero_impl<true>(handle, alignment, n, zero);
}

static inline constexpr void *handle_allocate_aligned(handle_type handle, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (has_status && has_native_handle_allocate)) || (throws_on_allocation_failure && (has_status && has_native_handle_allocate_try)))
{
	return handle_allocate_aligned_conditional_zero_impl<throws_on_allocation_failure>(handle, alignment, n, false);
}
static inline constexpr void *handle_allocate_aligned_die(handle_type handle, ::std::size_t alignment, ::std::size_t n) noexcept
	requires(has_status && has_native_handle_allocate)
{
	return handle_allocate_aligned_conditional_zero_impl<false>(handle, alignment, n, false);
}
static inline constexpr void *handle_allocate_aligned_try(handle_type handle, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(has_status && has_native_handle_allocate_try)
{
	return handle_allocate_aligned_conditional_zero_impl<true>(handle, alignment, n, false);
}

static inline constexpr void *handle_allocate_aligned_zero(handle_type handle, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (has_status && has_native_handle_allocate)) || (throws_on_allocation_failure && (has_status && has_native_handle_allocate_try)))
{
	return handle_allocate_aligned_conditional_zero_impl<throws_on_allocation_failure>(handle, alignment, n, true);
}
static inline constexpr void *handle_allocate_aligned_zero_die(handle_type handle, ::std::size_t alignment, ::std::size_t n) noexcept
	requires(has_status && has_native_handle_allocate)
{
	return handle_allocate_aligned_conditional_zero_impl<false>(handle, alignment, n, true);
}
static inline constexpr void *handle_allocate_aligned_zero_try(handle_type handle, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(has_status && has_native_handle_allocate_try)
{
	return handle_allocate_aligned_conditional_zero_impl<true>(handle, alignment, n, true);
}

template <bool throwing>
static inline ::fast_io::allocation_least_result handle_allocate_conditional_zero_at_least_impl(handle_type handle, ::std::size_t n, bool zero)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(has_status && has_native_handle_allocate)
{
	if constexpr (::fast_io::details::has_handle_allocate_conditional_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::handle_allocate_conditional_zero_at_least(handle, n, zero);
	}
	else if constexpr (::fast_io::details::has_handle_allocate_aligned_conditional_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::handle_allocate_aligned_conditional_zero_at_least(handle, default_alignment, n, zero);
	}
	else if constexpr (::fast_io::details::has_handle_allocate_at_least_mode_impl<alloc, throwing>)
	{
		auto res{dispatch<throwing>::handle_allocate_at_least(handle, n)};
		if (zero)
		{
			::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(res.ptr), res.count);
		}
		return res;
	}
	else if constexpr (::fast_io::details::has_handle_allocate_aligned_at_least_mode_impl<alloc, throwing>)
	{
		auto res{dispatch<throwing>::handle_allocate_aligned_at_least(handle, default_alignment, n)};
		if (zero)
		{
			::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(res.ptr), res.count);
		}
		return res;
	}
	else
	{
		return {handle_allocate_conditional_zero_impl<throwing>(handle, n, zero), n};
	}
}
static inline constexpr ::fast_io::allocation_least_result handle_allocate_conditional_zero_at_least(handle_type handle, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (has_status && has_native_handle_allocate)) || (throws_on_allocation_failure && (has_status && has_native_handle_allocate_try)))
{
	return handle_allocate_conditional_zero_at_least_impl<throws_on_allocation_failure>(handle, n, zero);
}
static inline constexpr ::fast_io::allocation_least_result handle_allocate_conditional_zero_at_least_die(handle_type handle, ::std::size_t n, bool zero) noexcept
	requires(has_status && has_native_handle_allocate)
{
	return handle_allocate_conditional_zero_at_least_impl<false>(handle, n, zero);
}
static inline constexpr ::fast_io::allocation_least_result handle_allocate_conditional_zero_at_least_try(handle_type handle, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS
	requires(has_status && has_native_handle_allocate_try)
{
	return handle_allocate_conditional_zero_at_least_impl<true>(handle, n, zero);
}

static inline constexpr ::fast_io::allocation_least_result handle_allocate_at_least(handle_type handle, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (has_status && has_native_handle_allocate)) || (throws_on_allocation_failure && (has_status && has_native_handle_allocate_try)))
{
	return handle_allocate_conditional_zero_at_least_impl<throws_on_allocation_failure>(handle, n, false);
}
static inline constexpr ::fast_io::allocation_least_result handle_allocate_at_least_die(handle_type handle, ::std::size_t n) noexcept
	requires(has_status && has_native_handle_allocate)
{
	return handle_allocate_conditional_zero_at_least_impl<false>(handle, n, false);
}
static inline constexpr ::fast_io::allocation_least_result handle_allocate_at_least_try(handle_type handle, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(has_status && has_native_handle_allocate_try)
{
	return handle_allocate_conditional_zero_at_least_impl<true>(handle, n, false);
}

static inline constexpr ::fast_io::allocation_least_result handle_allocate_zero_at_least(handle_type handle, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (has_status && has_native_handle_allocate)) || (throws_on_allocation_failure && (has_status && has_native_handle_allocate_try)))
{
	if constexpr (::fast_io::details::has_handle_allocate_zero_at_least_mode_impl<alloc, throws_on_allocation_failure>)
	{
		return dispatch<throws_on_allocation_failure>::handle_allocate_zero_at_least(handle, n);
	}
	else if constexpr (::fast_io::details::has_handle_allocate_aligned_zero_at_least_mode_impl<alloc, throws_on_allocation_failure>)
	{
		return dispatch<throws_on_allocation_failure>::handle_allocate_aligned_zero_at_least(handle, default_alignment, n);
	}
	else
	{
		return handle_allocate_conditional_zero_at_least_impl<throws_on_allocation_failure>(handle, n, true);
	}
}
static inline constexpr ::fast_io::allocation_least_result handle_allocate_zero_at_least_die(handle_type handle, ::std::size_t n) noexcept
	requires(has_status && has_native_handle_allocate)
{
	if constexpr (::fast_io::details::has_handle_allocate_zero_at_least_mode_impl<alloc, false>)
	{
		return dispatch<false>::handle_allocate_zero_at_least(handle, n);
	}
	else if constexpr (::fast_io::details::has_handle_allocate_aligned_zero_at_least_mode_impl<alloc, false>)
	{
		return dispatch<false>::handle_allocate_aligned_zero_at_least(handle, default_alignment, n);
	}
	else
	{
		return handle_allocate_conditional_zero_at_least_impl<false>(handle, n, true);
	}
}
static inline constexpr ::fast_io::allocation_least_result handle_allocate_zero_at_least_try(handle_type handle, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(has_status && has_native_handle_allocate_try)
{
	if constexpr (::fast_io::details::has_handle_allocate_zero_at_least_mode_impl<alloc, true>)
	{
		return dispatch<true>::handle_allocate_zero_at_least(handle, n);
	}
	else if constexpr (::fast_io::details::has_handle_allocate_aligned_zero_at_least_mode_impl<alloc, true>)
	{
		return dispatch<true>::handle_allocate_aligned_zero_at_least(handle, default_alignment, n);
	}
	else
	{
		return handle_allocate_conditional_zero_at_least_impl<true>(handle, n, true);
	}
}

template <bool throwing>
static inline ::fast_io::allocation_least_result handle_allocate_aligned_conditional_zero_at_least_impl(handle_type handle, ::std::size_t alignment, ::std::size_t n, bool zero)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
	requires(has_status && has_native_handle_allocate)
{
	if constexpr (::fast_io::details::has_handle_allocate_aligned_conditional_zero_at_least_mode_impl<alloc, throwing>)
	{
		return dispatch<throwing>::handle_allocate_aligned_conditional_zero_at_least(handle, alignment, n, zero);
	}
	else
	{
		if (alignment <= default_alignment)
		{
			return handle_allocate_conditional_zero_at_least_impl<throwing>(handle, n, zero);
		}
		if constexpr (::fast_io::details::has_handle_allocate_aligned_at_least_mode_impl<alloc, throwing> ||
					  ::fast_io::details::has_handle_allocate_aligned_zero_at_least_mode_impl<alloc, throwing> ||
					  ::fast_io::details::has_handle_allocate_aligned_conditional_zero_mode_impl<alloc, throwing> ||
					  ::fast_io::details::has_handle_allocate_aligned_mode_impl<alloc, throwing> ||
					  ::fast_io::details::has_handle_allocate_aligned_zero_mode_impl<alloc, throwing>)
		{
			return {handle_allocate_aligned_conditional_zero_impl<throwing>(handle, alignment, n, zero), n};
		}
		else
		{
			return ::fast_io::details::status_allocator_pointer_aligned_at_least_impl<alloc, throwing>(handle, alignment, n, zero);
		}
	}
}
static inline constexpr ::fast_io::allocation_least_result handle_allocate_aligned_conditional_zero_at_least(handle_type handle, ::std::size_t alignment, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (has_status && has_native_handle_allocate)) || (throws_on_allocation_failure && (has_status && has_native_handle_allocate_try)))
{
	return handle_allocate_aligned_conditional_zero_at_least_impl<throws_on_allocation_failure>(handle, alignment, n, zero);
}
static inline constexpr ::fast_io::allocation_least_result handle_allocate_aligned_conditional_zero_at_least_die(handle_type handle, ::std::size_t alignment, ::std::size_t n, bool zero) noexcept
	requires(has_status && has_native_handle_allocate)
{
	return handle_allocate_aligned_conditional_zero_at_least_impl<false>(handle, alignment, n, zero);
}
static inline constexpr ::fast_io::allocation_least_result handle_allocate_aligned_conditional_zero_at_least_try(handle_type handle, ::std::size_t alignment, ::std::size_t n, bool zero) FAST_IO_HERBCEPTIONS_THROWS
	requires(has_status && has_native_handle_allocate_try)
{
	return handle_allocate_aligned_conditional_zero_at_least_impl<true>(handle, alignment, n, zero);
}

static inline constexpr ::fast_io::allocation_least_result handle_allocate_aligned_at_least(handle_type handle, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (has_status && has_native_handle_allocate)) || (throws_on_allocation_failure && (has_status && has_native_handle_allocate_try)))
{
	if constexpr (::fast_io::details::has_handle_allocate_aligned_at_least_mode_impl<alloc, throws_on_allocation_failure>)
	{
		return dispatch<throws_on_allocation_failure>::handle_allocate_aligned_at_least(handle, alignment, n);
	}
	else
	{
		return handle_allocate_aligned_conditional_zero_at_least_impl<throws_on_allocation_failure>(handle, alignment, n, false);
	}
}
static inline constexpr ::fast_io::allocation_least_result handle_allocate_aligned_at_least_die(handle_type handle, ::std::size_t alignment, ::std::size_t n) noexcept
	requires(has_status && has_native_handle_allocate)
{
	if constexpr (::fast_io::details::has_handle_allocate_aligned_at_least_mode_impl<alloc, false>)
	{
		return dispatch<false>::handle_allocate_aligned_at_least(handle, alignment, n);
	}
	else
	{
		return handle_allocate_aligned_conditional_zero_at_least_impl<false>(handle, alignment, n, false);
	}
}
static inline constexpr ::fast_io::allocation_least_result handle_allocate_aligned_at_least_try(handle_type handle, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(has_status && has_native_handle_allocate_try)
{
	if constexpr (::fast_io::details::has_handle_allocate_aligned_at_least_mode_impl<alloc, true>)
	{
		return dispatch<true>::handle_allocate_aligned_at_least(handle, alignment, n);
	}
	else
	{
		return handle_allocate_aligned_conditional_zero_at_least_impl<true>(handle, alignment, n, false);
	}
}

static inline constexpr ::fast_io::allocation_least_result handle_allocate_aligned_zero_at_least(handle_type handle, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS_IF(throws_on_allocation_failure)
	requires((!throws_on_allocation_failure && (has_status && has_native_handle_allocate)) || (throws_on_allocation_failure && (has_status && has_native_handle_allocate_try)))
{
	if constexpr (::fast_io::details::has_handle_allocate_aligned_zero_at_least_mode_impl<alloc, throws_on_allocation_failure>)
	{
		return dispatch<throws_on_allocation_failure>::handle_allocate_aligned_zero_at_least(handle, alignment, n);
	}
	else
	{
		return handle_allocate_aligned_conditional_zero_at_least_impl<throws_on_allocation_failure>(handle, alignment, n, true);
	}
}
static inline constexpr ::fast_io::allocation_least_result handle_allocate_aligned_zero_at_least_die(handle_type handle, ::std::size_t alignment, ::std::size_t n) noexcept
	requires(has_status && has_native_handle_allocate)
{
	if constexpr (::fast_io::details::has_handle_allocate_aligned_zero_at_least_mode_impl<alloc, false>)
	{
		return dispatch<false>::handle_allocate_aligned_zero_at_least(handle, alignment, n);
	}
	else
	{
		return handle_allocate_aligned_conditional_zero_at_least_impl<false>(handle, alignment, n, true);
	}
}
static inline constexpr ::fast_io::allocation_least_result handle_allocate_aligned_zero_at_least_try(handle_type handle, ::std::size_t alignment, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	requires(has_status && has_native_handle_allocate_try)
{
	if constexpr (::fast_io::details::has_handle_allocate_aligned_zero_at_least_mode_impl<alloc, true>)
	{
		return dispatch<true>::handle_allocate_aligned_zero_at_least(handle, alignment, n);
	}
	else
	{
		return handle_allocate_aligned_conditional_zero_at_least_impl<true>(handle, alignment, n, true);
	}
}
