#pragma once

// To make a constexpr allocator, we need ::std::allocator. Because only new expression and
// ::std::allocator<T>::allocate are allowed in constexpr functions. See https://github.com/microsoft/STL/issues/1532
// https://github.com/microsoft/STL/issues/4002 gcc and clang provide constexpr new, but still won't compile.
// ::std::allocator<T> is NOT freestanding.

namespace fast_io
{

#include "allocator_adapter_flags.h"

namespace details
{

#include "has_methods_detect.h"

template <typename alloc>
concept has_default_alignment_impl = requires(::std::size_t n) { alloc::default_alignment; };

template <typename alloc>
concept has_adapter_flags_impl = requires {
	{ alloc::adapter_flags };
	requires ::std::same_as<::std::remove_cvref_t<decltype(alloc::adapter_flags)>, ::fast_io::allocator_adapter_flags>;
};

template <typename alloc>
inline constexpr ::fast_io::allocator_adapter_flags adapter_flags_or_default() noexcept
{
	if constexpr (has_adapter_flags_impl<alloc>)
	{
		return alloc::adapter_flags;
	}
	else
	{
		return ::fast_io::allocator_adapter_flags::none;
	}
}

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

template <bool throwing>
inline constexpr ::std::size_t allocator_compute_aligned_total_size_impl(::std::size_t alignment, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
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
		if constexpr (throwing)
		{
			::fast_io::herbceptions::throws_errc(::std::errc::value_too_large);
		}
		else
		{
			::fast_io::fast_terminate();
		}
	}
	::std::size_t total_extra_space{alignment + sizeofptr};
	::std::size_t upperlimit{static_cast<::std::size_t>(mxn - total_extra_space)};
	if (n > upperlimit)
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
	return n + total_extra_space;
}

inline void *allocator_adjust_ptr_to_aligned_impl(void *p, ::std::size_t alignment) noexcept
{
	void *aligned_ptr{reinterpret_cast<void *>((reinterpret_cast<::std::uintptr_t>(p) + alignment) & (0 - alignment))};
	reinterpret_cast<void **>(aligned_ptr)[-1] = p;
	return aligned_ptr;
}

// Dispatches a native allocator API call to its _die or _try flavour.
// In die mode an allocator that only provides the _try flavour has its
// herbception caught and the program terminated. In try mode an allocator
// that only provides the _die flavour is rejected.
template <typename alloc, bool throwing>
struct allocator_die_try_dispatch
{
#define FAST_IO_ALLOCATION_DISPATCH(api)                                                              \
	template <typename... Args>                                                                       \
	static inline constexpr decltype(auto) api(Args &&...args)                                        \
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)                                                      \
	{                                                                                                 \
		if constexpr (throwing)                                                                       \
		{                                                                                             \
			static_assert(::fast_io::details::has_##api##_try_impl<alloc>,                            \
						  "the underlying allocator does not provide " #api "_try");                  \
			return alloc::api##_try(::std::forward<Args>(args)...);                                   \
		}                                                                                             \
		else if constexpr (::fast_io::details::has_##api##_die_impl<alloc>)                           \
		{                                                                                             \
			return alloc::api##_die(::std::forward<Args>(args)...);                                   \
		}                                                                                             \
		else                                                                                          \
		{                                                                                             \
			static_assert(::fast_io::details::has_##api##_try_impl<alloc>,                            \
						  "the underlying allocator provides neither " #api "_die nor " #api "_try"); \
			FAST_IO_HERBCEPTIONS_TRY                                                                  \
			{                                                                                         \
				return alloc::api##_try(::std::forward<Args>(args)...);                               \
			}                                                                                         \
			FAST_IO_HERBCEPTIONS_CATCH_ALL                                                            \
			{                                                                                         \
				::fast_io::fast_terminate();                                                          \
			}                                                                                         \
		}                                                                                             \
	}

	FAST_IO_ALLOCATION_DISPATCH(allocate)
	FAST_IO_ALLOCATION_DISPATCH(allocate_aligned)
	FAST_IO_ALLOCATION_DISPATCH(allocate_zero)
	FAST_IO_ALLOCATION_DISPATCH(allocate_aligned_zero)
	FAST_IO_ALLOCATION_DISPATCH(allocate_conditional_zero)
	FAST_IO_ALLOCATION_DISPATCH(allocate_aligned_conditional_zero)
	FAST_IO_ALLOCATION_DISPATCH(reallocate)
	FAST_IO_ALLOCATION_DISPATCH(reallocate_aligned)
	FAST_IO_ALLOCATION_DISPATCH(reallocate_zero)
	FAST_IO_ALLOCATION_DISPATCH(reallocate_aligned_zero)
	FAST_IO_ALLOCATION_DISPATCH(reallocate_conditional_zero)
	FAST_IO_ALLOCATION_DISPATCH(reallocate_aligned_conditional_zero)
	FAST_IO_ALLOCATION_DISPATCH(reallocate_n)
	FAST_IO_ALLOCATION_DISPATCH(reallocate_aligned_n)
	FAST_IO_ALLOCATION_DISPATCH(reallocate_zero_n)
	FAST_IO_ALLOCATION_DISPATCH(reallocate_aligned_zero_n)
	FAST_IO_ALLOCATION_DISPATCH(reallocate_n_conditional_zero)
	FAST_IO_ALLOCATION_DISPATCH(reallocate_aligned_n_conditional_zero)
	FAST_IO_ALLOCATION_DISPATCH(allocate_at_least)
	FAST_IO_ALLOCATION_DISPATCH(allocate_aligned_at_least)
	FAST_IO_ALLOCATION_DISPATCH(allocate_zero_at_least)
	FAST_IO_ALLOCATION_DISPATCH(allocate_aligned_zero_at_least)
	FAST_IO_ALLOCATION_DISPATCH(allocate_conditional_zero_at_least)
	FAST_IO_ALLOCATION_DISPATCH(allocate_aligned_conditional_zero_at_least)
	FAST_IO_ALLOCATION_DISPATCH(reallocate_at_least)
	FAST_IO_ALLOCATION_DISPATCH(reallocate_aligned_at_least)
	FAST_IO_ALLOCATION_DISPATCH(reallocate_zero_at_least)
	FAST_IO_ALLOCATION_DISPATCH(reallocate_aligned_zero_at_least)
	FAST_IO_ALLOCATION_DISPATCH(reallocate_conditional_zero_at_least)
	FAST_IO_ALLOCATION_DISPATCH(reallocate_aligned_conditional_zero_at_least)
	FAST_IO_ALLOCATION_DISPATCH(reallocate_n_at_least)
	FAST_IO_ALLOCATION_DISPATCH(reallocate_aligned_n_at_least)
	FAST_IO_ALLOCATION_DISPATCH(reallocate_zero_n_at_least)
	FAST_IO_ALLOCATION_DISPATCH(reallocate_aligned_zero_n_at_least)
	FAST_IO_ALLOCATION_DISPATCH(reallocate_n_conditional_zero_at_least)
	FAST_IO_ALLOCATION_DISPATCH(reallocate_aligned_n_conditional_zero_at_least)
	FAST_IO_ALLOCATION_DISPATCH(handle_allocate)
	FAST_IO_ALLOCATION_DISPATCH(handle_allocate_aligned)
	FAST_IO_ALLOCATION_DISPATCH(handle_allocate_zero)
	FAST_IO_ALLOCATION_DISPATCH(handle_allocate_aligned_zero)
	FAST_IO_ALLOCATION_DISPATCH(handle_allocate_conditional_zero)
	FAST_IO_ALLOCATION_DISPATCH(handle_allocate_aligned_conditional_zero)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate_aligned)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate_zero)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate_aligned_zero)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate_conditional_zero)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate_aligned_conditional_zero)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate_n)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate_aligned_n)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate_zero_n)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate_aligned_zero_n)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate_n_conditional_zero)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate_aligned_n_conditional_zero)
	FAST_IO_ALLOCATION_DISPATCH(handle_allocate_at_least)
	FAST_IO_ALLOCATION_DISPATCH(handle_allocate_aligned_at_least)
	FAST_IO_ALLOCATION_DISPATCH(handle_allocate_zero_at_least)
	FAST_IO_ALLOCATION_DISPATCH(handle_allocate_aligned_zero_at_least)
	FAST_IO_ALLOCATION_DISPATCH(handle_allocate_conditional_zero_at_least)
	FAST_IO_ALLOCATION_DISPATCH(handle_allocate_aligned_conditional_zero_at_least)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate_at_least)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate_aligned_at_least)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate_zero_at_least)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate_aligned_zero_at_least)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate_conditional_zero_at_least)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate_aligned_conditional_zero_at_least)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate_n_at_least)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate_aligned_n_at_least)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate_zero_n_at_least)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate_aligned_zero_n_at_least)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate_n_conditional_zero_at_least)
	FAST_IO_ALLOCATION_DISPATCH(handle_reallocate_aligned_n_conditional_zero_at_least)

#undef FAST_IO_ALLOCATION_DISPATCH
};

template <typename alloc, bool throwing>
inline constexpr void *allocator_pointer_aligned_impl(::std::size_t, ::std::size_t, bool)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing);

template <typename alloc, bool throwing>
inline constexpr ::fast_io::allocation_least_result allocator_pointer_aligned_at_least_impl(::std::size_t, ::std::size_t, bool)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing);

template <typename alloc, bool throwing>
inline constexpr void *status_allocator_pointer_aligned_impl(typename alloc::handle_type, ::std::size_t, ::std::size_t, bool)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing);

template <typename alloc, bool throwing>
inline constexpr ::fast_io::allocation_least_result status_allocator_pointer_aligned_at_least_impl(typename alloc::handle_type, ::std::size_t, ::std::size_t, bool)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing);


template <typename alloc, bool throwing>
concept native_allocate_aligned_has_none_zero_ops =
	::fast_io::details::has_allocate_aligned_mode_impl<alloc, throwing> ||
	::fast_io::details::has_allocate_aligned_at_least_mode_impl<alloc, throwing>;
template <typename alloc, bool throwing>
concept native_allocate_aligned_has_zero_ops =
	::fast_io::details::has_allocate_aligned_zero_mode_impl<alloc, throwing> ||
	::fast_io::details::has_allocate_aligned_zero_at_least_mode_impl<alloc, throwing>;

template <typename alloc, bool throwing>
concept native_allocate_aligned_has_ops =
	::fast_io::details::native_allocate_aligned_has_none_zero_ops<alloc, throwing> ||
	::fast_io::details::native_allocate_aligned_has_zero_ops<alloc, throwing>;

template <typename alloc, bool throwing>
concept native_allocate_has_none_zero_ops = ::fast_io::details::has_allocate_mode_impl<alloc, throwing> ||
											::fast_io::details::has_allocate_at_least_mode_impl<alloc, throwing> ||
											::fast_io::details::native_allocate_aligned_has_none_zero_ops<alloc, throwing>;
template <typename alloc, bool throwing>
concept native_allocate_has_zero_ops =
	::fast_io::details::has_allocate_zero_mode_impl<alloc, throwing> ||
	::fast_io::details::has_allocate_zero_at_least_mode_impl<alloc, throwing> ||
	::fast_io::details::native_allocate_aligned_has_zero_ops<alloc, throwing>;
} // namespace details

template <typename alloc, ::fast_io::allocator_adapter_flags flags = ::fast_io::allocator_adapter_flags::none>
class generic_allocator_adapter
{
public:
	using allocator_type = alloc;
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
	static inline constexpr bool has_status{::fast_io::details::has_non_empty_handle_type<allocator_type>};
	static_assert(!::fast_io::details::has_any_handle_method_impl<allocator_type> || has_status,
				  "handle-based allocator APIs require a non-empty trivially copyable handle_type");
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

	template <bool throwing>
	using dispatch = ::fast_io::details::allocator_die_try_dispatch<allocator_type, throwing>;

#include "adapters_no_status.h"
#include "adapters_status.h"
};

namespace details
{

template <typename alloc, bool throwing>
inline constexpr void *allocator_pointer_aligned_impl(::std::size_t alignment, ::std::size_t n, bool zero)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
{
	static_assert(::fast_io::generic_allocator_adapter<alloc>::has_native_allocate);

	constexpr ::std::size_t defaultalignment{::fast_io::details::calculate_default_alignment<alloc>()};
	bool const alignedadjustment{defaultalignment < alignment};
	if (alignedadjustment)
	{
		n = ::fast_io::details::allocator_compute_aligned_total_size_impl<throwing>(alignment, n);
	}
	void *p;
	if constexpr (throwing)
	{
		p = ::fast_io::generic_allocator_adapter<alloc>::allocate_conditional_zero_try(n, zero);
	}
	else
	{
		p = ::fast_io::generic_allocator_adapter<alloc>::allocate_conditional_zero_die(n, zero);
	}
	if (alignedadjustment)
	{
		p = ::fast_io::details::allocator_adjust_ptr_to_aligned_impl(p, alignment);
	}
	return p;
}

template <typename alloc, bool throwing>
inline constexpr ::fast_io::allocation_least_result allocator_pointer_aligned_at_least_impl(::std::size_t alignment, ::std::size_t n, bool zero)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
{
	static_assert(::fast_io::generic_allocator_adapter<alloc>::has_native_allocate);

	constexpr ::std::size_t defaultalignment{::fast_io::details::calculate_default_alignment<alloc>()};
	bool const alignedadjustment{defaultalignment < alignment};
	if (alignedadjustment)
	{
		n = ::fast_io::details::allocator_compute_aligned_total_size_impl<throwing>(alignment, n);
	}
	::fast_io::allocation_least_result res;
	if constexpr (throwing)
	{
		res = ::fast_io::generic_allocator_adapter<alloc>::allocate_conditional_zero_at_least_try(n, zero);
	}
	else
	{
		res = ::fast_io::generic_allocator_adapter<alloc>::allocate_conditional_zero_at_least_die(n, zero);
	}
	if (alignedadjustment)
	{
		auto resptr{res.ptr};
		auto aligned_ptr = ::fast_io::details::allocator_adjust_ptr_to_aligned_impl(resptr, alignment);
		res = {aligned_ptr, res.count - static_cast<::std::size_t>(reinterpret_cast<char unsigned *>(aligned_ptr) - reinterpret_cast<char unsigned *>(resptr))};
	}
	return res;
}

template <typename alloc, bool throwing>
inline constexpr void *status_allocator_pointer_aligned_impl(typename alloc::handle_type handle, ::std::size_t alignment, ::std::size_t n, bool zero)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
{
	using adapter = ::fast_io::generic_allocator_adapter<alloc>;
	static_assert(adapter::has_native_handle_allocate);

	constexpr ::std::size_t defaultalignment{adapter::default_alignment};
	bool const alignedadjustment{defaultalignment < alignment};
	if (alignedadjustment)
	{
		n = ::fast_io::details::allocator_compute_aligned_total_size_impl<throwing>(alignment, n);
	}
	void *p{adapter::template handle_allocate_conditional_zero_impl<throwing>(handle, n, zero)};
	if (alignedadjustment)
	{
		p = ::fast_io::details::allocator_adjust_ptr_to_aligned_impl(p, alignment);
	}
	return p;
}

template <typename alloc, bool throwing>
inline constexpr ::fast_io::allocation_least_result status_allocator_pointer_aligned_at_least_impl(typename alloc::handle_type handle, ::std::size_t alignment, ::std::size_t n, bool zero)
	FAST_IO_HERBCEPTIONS_THROWS_IF(throwing)
{
	using adapter = ::fast_io::generic_allocator_adapter<alloc>;
	static_assert(adapter::has_native_handle_allocate);

	constexpr ::std::size_t defaultalignment{adapter::default_alignment};
	bool const alignedadjustment{defaultalignment < alignment};
	if (alignedadjustment)
	{
		n = ::fast_io::details::allocator_compute_aligned_total_size_impl<throwing>(alignment, n);
	}
	::fast_io::allocation_least_result res{adapter::template handle_allocate_conditional_zero_at_least_impl<throwing>(handle, n, zero)};
	if (alignedadjustment)
	{
		auto resptr{res.ptr};
		auto aligned_ptr{::fast_io::details::allocator_adjust_ptr_to_aligned_impl(resptr, alignment)};
		res = {aligned_ptr, res.count - static_cast<::std::size_t>(reinterpret_cast<char unsigned *>(aligned_ptr) - reinterpret_cast<char unsigned *>(resptr))};
	}
	return res;
}
} // namespace details

} // namespace fast_io
