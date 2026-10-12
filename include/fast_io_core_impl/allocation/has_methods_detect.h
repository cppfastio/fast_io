#pragma once

// Detection concepts for native allocator APIs.
// Fallible APIs come in two flavours:
//   *_die: fail fast on allocation failure (::fast_io::fast_terminate)
//   *_try: throw a herbception (::std::error) on allocation failure
// Infallible APIs (deallocate family) keep unsuffixed names.
//
// has_*_impl: the API exists in either flavour.
// has_*_mode_impl<alloc, throwing>: picks the _try detection when throwing is true,
// otherwise either flavour is acceptable (a _try only allocator is caught and
// terminated by the adapter's _die entry points).

#define FAST_IO_ALLOCATION_DETECT(api, rettype, params, args)                                            \
	template <typename alloc>                                                                             \
	concept has_##api##_die_impl = requires params {                                                      \
		{                                                                                                 \
			alloc::api##_die args                                                                           \
		} -> ::std::same_as<rettype>;                                                                     \
	};                                                                                                    \
	template <typename alloc>                                                                             \
	concept has_##api##_try_impl = requires params {                                                      \
		{                                                                                                 \
			alloc::api##_try args                                                                           \
		} -> ::std::same_as<rettype>;                                                                     \
	};                                                                                                    \
	template <typename alloc>                                                                             \
	concept has_##api##_impl = has_##api##_die_impl<alloc> || has_##api##_try_impl<alloc>;                \
	template <typename alloc, bool throwing>                                                                \
	concept has_##api##_mode_impl =                                                                       \
		(throwing && has_##api##_try_impl<alloc>) || (!throwing && has_##api##_impl<alloc>);

#define FAST_IO_ALLOCATION_DETECT_INFALLIBLE(api, params, args)                                          \
	template <typename alloc>                                                                             \
	concept has_##api##_impl = requires params {                                                          \
		{                                                                                                 \
			alloc::api args                                                                                 \
		} -> ::std::same_as<void>;                                                                        \
	};

FAST_IO_ALLOCATION_DETECT(allocate, void *, (::std::size_t n), (n))
FAST_IO_ALLOCATION_DETECT(allocate_aligned, void *, (::std::size_t alignment, ::std::size_t n), (alignment, n))
FAST_IO_ALLOCATION_DETECT(allocate_zero, void *, (::std::size_t n), (n))
FAST_IO_ALLOCATION_DETECT(allocate_aligned_zero, void *, (::std::size_t alignment, ::std::size_t n), (alignment, n))
FAST_IO_ALLOCATION_DETECT(allocate_conditional_zero, void *, (::std::size_t n, bool zero), (n, zero))
FAST_IO_ALLOCATION_DETECT(allocate_aligned_conditional_zero, void *, (::std::size_t alignment, ::std::size_t n, bool zero), (alignment, n, zero))

FAST_IO_ALLOCATION_DETECT(reallocate, void *, (void *p, ::std::size_t n), (p, n))
FAST_IO_ALLOCATION_DETECT(reallocate_aligned, void *, (void *p, ::std::size_t alignment, ::std::size_t n), (p, alignment, n))
FAST_IO_ALLOCATION_DETECT(reallocate_zero, void *, (void *p, ::std::size_t n), (p, n))
FAST_IO_ALLOCATION_DETECT(reallocate_aligned_zero, void *, (void *p, ::std::size_t alignment, ::std::size_t n), (p, alignment, n))
FAST_IO_ALLOCATION_DETECT(reallocate_conditional_zero, void *, (void *p, ::std::size_t n, bool zero), (p, n, zero))
FAST_IO_ALLOCATION_DETECT(reallocate_aligned_conditional_zero, void *, (void *p, ::std::size_t alignment, ::std::size_t n, bool zero), (p, alignment, n, zero))

FAST_IO_ALLOCATION_DETECT(reallocate_n, void *, (void *p, ::std::size_t oldn, ::std::size_t n), (p, oldn, n))
FAST_IO_ALLOCATION_DETECT(reallocate_aligned_n, void *, (void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n), (p, oldn, alignment, n))
FAST_IO_ALLOCATION_DETECT(reallocate_zero_n, void *, (void *p, ::std::size_t oldn, ::std::size_t n), (p, oldn, n))
FAST_IO_ALLOCATION_DETECT(reallocate_aligned_zero_n, void *, (void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n), (p, oldn, alignment, n))
FAST_IO_ALLOCATION_DETECT(reallocate_n_conditional_zero, void *, (void *p, ::std::size_t oldn, ::std::size_t n, bool zero), (p, oldn, n, zero))
FAST_IO_ALLOCATION_DETECT(reallocate_aligned_n_conditional_zero, void *, (void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n, bool zero), (p, oldn, alignment, n, zero))

FAST_IO_ALLOCATION_DETECT_INFALLIBLE(deallocate, (void *p), (p))
FAST_IO_ALLOCATION_DETECT_INFALLIBLE(deallocate_aligned, (void *p, ::std::size_t alignment), (p, alignment))
FAST_IO_ALLOCATION_DETECT_INFALLIBLE(deallocate_n, (void *p, ::std::size_t n), (p, n))
FAST_IO_ALLOCATION_DETECT_INFALLIBLE(deallocate_aligned_n, (void *p, ::std::size_t alignment, ::std::size_t n), (p, alignment, n))

FAST_IO_ALLOCATION_DETECT(allocate_at_least, ::fast_io::allocation_least_result, (::std::size_t n), (n))
FAST_IO_ALLOCATION_DETECT(allocate_aligned_at_least, ::fast_io::allocation_least_result, (::std::size_t alignment, ::std::size_t n), (alignment, n))
FAST_IO_ALLOCATION_DETECT(allocate_zero_at_least, ::fast_io::allocation_least_result, (::std::size_t n), (n))
FAST_IO_ALLOCATION_DETECT(allocate_aligned_zero_at_least, ::fast_io::allocation_least_result, (::std::size_t alignment, ::std::size_t n), (alignment, n))
FAST_IO_ALLOCATION_DETECT(allocate_conditional_zero_at_least, ::fast_io::allocation_least_result, (::std::size_t n, bool zero), (n, zero))
FAST_IO_ALLOCATION_DETECT(allocate_aligned_conditional_zero_at_least, ::fast_io::allocation_least_result, (::std::size_t alignment, ::std::size_t n, bool zero), (alignment, n, zero))

FAST_IO_ALLOCATION_DETECT(reallocate_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t n), (p, n))
FAST_IO_ALLOCATION_DETECT(reallocate_aligned_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t alignment, ::std::size_t n), (p, alignment, n))
FAST_IO_ALLOCATION_DETECT(reallocate_zero_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t n), (p, n))
FAST_IO_ALLOCATION_DETECT(reallocate_aligned_zero_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t alignment, ::std::size_t n), (p, alignment, n))
FAST_IO_ALLOCATION_DETECT(reallocate_conditional_zero_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t n, bool zero), (p, n, zero))
FAST_IO_ALLOCATION_DETECT(reallocate_aligned_conditional_zero_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t alignment, ::std::size_t n, bool zero), (p, alignment, n, zero))

FAST_IO_ALLOCATION_DETECT(reallocate_n_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t oldn, ::std::size_t n), (p, oldn, n))
FAST_IO_ALLOCATION_DETECT(reallocate_aligned_n_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n), (p, oldn, alignment, n))
FAST_IO_ALLOCATION_DETECT(reallocate_zero_n_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t oldn, ::std::size_t n), (p, oldn, n))
FAST_IO_ALLOCATION_DETECT(reallocate_aligned_zero_n_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n), (p, oldn, alignment, n))
FAST_IO_ALLOCATION_DETECT(reallocate_n_conditional_zero_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t oldn, ::std::size_t n, bool zero), (p, oldn, n, zero))
FAST_IO_ALLOCATION_DETECT(reallocate_aligned_n_conditional_zero_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n, bool zero), (p, oldn, alignment, n, zero))

// Handle based (status) allocators. The handle must be a non-empty trivially
// copyable type: it is passed by value into every operation.

template <typename alloc>
concept has_non_empty_handle_type = requires {
	typename alloc::handle_type;
	requires !::std::is_empty_v<typename alloc::handle_type>;
	requires ::std::is_trivially_copyable_v<typename alloc::handle_type>;
};

#define FAST_IO_ALLOCATION_UNPAREN(...) __VA_ARGS__
#define FAST_IO_ALLOCATION_DETECT_HANDLE(api, rettype, params, args)                                     \
	FAST_IO_ALLOCATION_DETECT(handle_##api, rettype,                                                       \
							  (typename alloc::handle_type handle, FAST_IO_ALLOCATION_UNPAREN params),   \
							  (handle, FAST_IO_ALLOCATION_UNPAREN args))

FAST_IO_ALLOCATION_DETECT_HANDLE(allocate, void *, (::std::size_t n), (n))
FAST_IO_ALLOCATION_DETECT_HANDLE(allocate_aligned, void *, (::std::size_t alignment, ::std::size_t n), (alignment, n))
FAST_IO_ALLOCATION_DETECT_HANDLE(allocate_zero, void *, (::std::size_t n), (n))
FAST_IO_ALLOCATION_DETECT_HANDLE(allocate_aligned_zero, void *, (::std::size_t alignment, ::std::size_t n), (alignment, n))
FAST_IO_ALLOCATION_DETECT_HANDLE(allocate_conditional_zero, void *, (::std::size_t n, bool zero), (n, zero))
FAST_IO_ALLOCATION_DETECT_HANDLE(allocate_aligned_conditional_zero, void *, (::std::size_t alignment, ::std::size_t n, bool zero), (alignment, n, zero))

FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate, void *, (void *p, ::std::size_t n), (p, n))
FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate_aligned, void *, (void *p, ::std::size_t alignment, ::std::size_t n), (p, alignment, n))
FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate_zero, void *, (void *p, ::std::size_t n), (p, n))
FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate_aligned_zero, void *, (void *p, ::std::size_t alignment, ::std::size_t n), (p, alignment, n))
FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate_conditional_zero, void *, (void *p, ::std::size_t n, bool zero), (p, n, zero))
FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate_aligned_conditional_zero, void *, (void *p, ::std::size_t alignment, ::std::size_t n, bool zero), (p, alignment, n, zero))

FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate_n, void *, (void *p, ::std::size_t oldn, ::std::size_t n), (p, oldn, n))
FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate_aligned_n, void *, (void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n), (p, oldn, alignment, n))
FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate_zero_n, void *, (void *p, ::std::size_t oldn, ::std::size_t n), (p, oldn, n))
FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate_aligned_zero_n, void *, (void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n), (p, oldn, alignment, n))
FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate_n_conditional_zero, void *, (void *p, ::std::size_t oldn, ::std::size_t n, bool zero), (p, oldn, n, zero))
FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate_aligned_n_conditional_zero, void *, (void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n, bool zero), (p, oldn, alignment, n, zero))

FAST_IO_ALLOCATION_DETECT_HANDLE(allocate_at_least, ::fast_io::allocation_least_result, (::std::size_t n), (n))
FAST_IO_ALLOCATION_DETECT_HANDLE(allocate_aligned_at_least, ::fast_io::allocation_least_result, (::std::size_t alignment, ::std::size_t n), (alignment, n))
FAST_IO_ALLOCATION_DETECT_HANDLE(allocate_zero_at_least, ::fast_io::allocation_least_result, (::std::size_t n), (n))
FAST_IO_ALLOCATION_DETECT_HANDLE(allocate_aligned_zero_at_least, ::fast_io::allocation_least_result, (::std::size_t alignment, ::std::size_t n), (alignment, n))
FAST_IO_ALLOCATION_DETECT_HANDLE(allocate_conditional_zero_at_least, ::fast_io::allocation_least_result, (::std::size_t n, bool zero), (n, zero))
FAST_IO_ALLOCATION_DETECT_HANDLE(allocate_aligned_conditional_zero_at_least, ::fast_io::allocation_least_result, (::std::size_t alignment, ::std::size_t n, bool zero), (alignment, n, zero))

FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t n), (p, n))
FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate_aligned_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t alignment, ::std::size_t n), (p, alignment, n))
FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate_zero_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t n), (p, n))
FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate_aligned_zero_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t alignment, ::std::size_t n), (p, alignment, n))
FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate_conditional_zero_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t n, bool zero), (p, n, zero))
FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate_aligned_conditional_zero_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t alignment, ::std::size_t n, bool zero), (p, alignment, n, zero))

FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate_n_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t oldn, ::std::size_t n), (p, oldn, n))
FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate_aligned_n_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n), (p, oldn, alignment, n))
FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate_zero_n_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t oldn, ::std::size_t n), (p, oldn, n))
FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate_aligned_zero_n_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n), (p, oldn, alignment, n))
FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate_n_conditional_zero_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t oldn, ::std::size_t n, bool zero), (p, oldn, n, zero))
FAST_IO_ALLOCATION_DETECT_HANDLE(reallocate_aligned_n_conditional_zero_at_least, ::fast_io::allocation_least_result, (void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n, bool zero), (p, oldn, alignment, n, zero))

#undef FAST_IO_ALLOCATION_UNPAREN
#undef FAST_IO_ALLOCATION_DETECT_HANDLE

template <typename alloc>
concept has_handle_deallocate_impl = requires(typename alloc::handle_type handle, void *p) {
	{ alloc::handle_deallocate(handle, p) } -> ::std::same_as<void>;
};

template <typename alloc>
concept has_handle_deallocate_aligned_impl = requires(typename alloc::handle_type handle, void *p, ::std::size_t alignment) {
	{ alloc::handle_deallocate_aligned(handle, p, alignment) } -> ::std::same_as<void>;
};

template <typename alloc>
concept has_handle_deallocate_n_impl = requires(typename alloc::handle_type handle, void *p, ::std::size_t n) {
	{ alloc::handle_deallocate_n(handle, p, n) } -> ::std::same_as<void>;
};

template <typename alloc>
concept has_handle_deallocate_aligned_n_impl = requires(typename alloc::handle_type handle, void *p, ::std::size_t alignment, ::std::size_t n) {
	{ alloc::handle_deallocate_aligned_n(handle, p, alignment, n) } -> ::std::same_as<void>;
};

// An allocator offers handle-based allocation when it declares a handle_type
// together with at least one handle_* method. The handle_type itself must be a
// non-empty trivially copyable type so it can be passed by value into every
// operation; anything else is a malformed handle allocator and gets rejected
// loudly instead of silently degrading to a statusless allocator.
template <typename alloc>
concept has_any_handle_method_impl =
	has_handle_allocate_impl<alloc> || has_handle_allocate_aligned_impl<alloc> ||
	has_handle_allocate_zero_impl<alloc> || has_handle_allocate_aligned_zero_impl<alloc> ||
	has_handle_allocate_conditional_zero_impl<alloc> || has_handle_allocate_aligned_conditional_zero_impl<alloc> ||
	has_handle_allocate_at_least_impl<alloc> || has_handle_allocate_aligned_at_least_impl<alloc> ||
	has_handle_allocate_zero_at_least_impl<alloc> || has_handle_allocate_aligned_zero_at_least_impl<alloc> ||
	has_handle_allocate_conditional_zero_at_least_impl<alloc> ||
	has_handle_allocate_aligned_conditional_zero_at_least_impl<alloc> ||
	has_handle_reallocate_impl<alloc> || has_handle_reallocate_aligned_impl<alloc> ||
	has_handle_reallocate_zero_impl<alloc> || has_handle_reallocate_aligned_zero_impl<alloc> ||
	has_handle_reallocate_n_impl<alloc> || has_handle_reallocate_aligned_n_impl<alloc> ||
	has_handle_reallocate_zero_n_impl<alloc> || has_handle_reallocate_aligned_zero_n_impl<alloc> ||
	has_handle_deallocate_impl<alloc> || has_handle_deallocate_aligned_impl<alloc> ||
	has_handle_deallocate_n_impl<alloc> || has_handle_deallocate_aligned_n_impl<alloc>;

#undef FAST_IO_ALLOCATION_DETECT
#undef FAST_IO_ALLOCATION_DETECT_INFALLIBLE
