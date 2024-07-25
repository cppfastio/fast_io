#pragma once

template <typename alloc>
concept has_allocate_impl = requires(::std::size_t n) {
	{ alloc::allocate(n) } -> ::std::same_as<void *>;
};

template <typename alloc>
concept has_allocate_aligned_impl = requires(::std::size_t n) {
	{ alloc::allocate_aligned(n, n) } -> ::std::same_as<void *>;
};

template <typename alloc>
concept has_allocate_zero_impl = requires(::std::size_t n) {
	{ alloc::allocate_zero(n) } -> ::std::same_as<void *>;
};

template <typename alloc>
concept has_allocate_aligned_zero_impl = requires(::std::size_t n) {
	{ alloc::allocate_aligned_zero(n, n) } -> ::std::same_as<void *>;
};

template <typename alloc>
concept has_reallocate_impl = requires(void *p, ::std::size_t n) {
	{ alloc::reallocate(p, n) } -> ::std::same_as<void *>;
};

template <typename alloc>
concept has_reallocate_aligned_impl = requires(void *p, ::std::size_t n) {
	{ alloc::reallocate_aligned(p, n, n) } -> ::std::same_as<void *>;
};

template <typename alloc>
concept has_reallocate_zero_impl = requires(void *p, ::std::size_t n) {
	{ alloc::reallocate_zero(p, n) } -> ::std::same_as<void *>;
};

template <typename alloc>
concept has_reallocate_aligned_zero_impl = requires(void *p, ::std::size_t n) {
	{ alloc::reallocate_aligned_zero(p, n, n) } -> ::std::same_as<void *>;
};

template <typename alloc>
concept has_reallocate_n_impl = requires(void *p, ::std::size_t n) {
	{ alloc::reallocate_n(p, n, n) } -> ::std::same_as<void *>;
};

template <typename alloc>
concept has_reallocate_aligned_n_impl = requires(void *p, ::std::size_t n) {
	{ alloc::reallocate_aligned_n(p, n, n, n) } -> ::std::same_as<void *>;
};

template <typename alloc>
concept has_reallocate_zero_n_impl = requires(void *p, ::std::size_t n) {
	{ alloc::reallocate_zero_n(p, n, n) } -> ::std::same_as<void *>;
};

template <typename alloc>
concept has_reallocate_aligned_zero_n_impl = requires(void *p, ::std::size_t n) {
	{ alloc::reallocate_aligned_zero_n(p, n, n, n) } -> ::std::same_as<void *>;
};

template <typename alloc>
concept has_deallocate_impl = requires(void *p) {
	{ alloc::deallocate(p) } -> ::std::same_as<void>;
};

template <typename alloc>
concept has_deallocate_aligned_impl = requires(void *p, ::std::size_t n) {
	{ alloc::deallocate_aligned(p, n) } -> ::std::same_as<void>;
};

template <typename alloc>
concept has_deallocate_n_impl = requires(void *p, ::std::size_t n) {
	{ alloc::deallocate_n(p, n) } -> ::std::same_as<void>;
};

template <typename alloc>
concept has_deallocate_aligned_n_impl = requires(void *p, ::std::size_t n) {
	{ alloc::deallocate_aligned_n(p, n, n) } -> ::std::same_as<void>;
};

template <typename alloc>
concept has_allocate_at_least_impl = requires(::std::size_t n) {
	{ alloc::allocate_at_least(n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};

template <typename alloc>
concept has_allocate_aligned_at_least_impl = requires(::std::size_t n) {
	{ alloc::allocate_aligned_at_least(n, n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};

template <typename alloc>
concept has_allocate_zero_at_least_impl = requires(::std::size_t n) {
	{ alloc::allocate_zero_at_least(n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};

template <typename alloc>
concept has_allocate_aligned_zero_at_least_impl = requires(::std::size_t n) {
	{ alloc::allocate_aligned_zero_at_least(n, n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};

template <typename alloc>
concept has_reallocate_at_least_impl = requires(void *p, ::std::size_t n) {
	{ alloc::reallocate_at_least(p, n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};

template <typename alloc>
concept has_reallocate_aligned_at_least_impl = requires(void *p, ::std::size_t n) {
	{ alloc::reallocate_aligned_at_least(p, n, n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};

template <typename alloc>
concept has_reallocate_zero_at_least_impl = requires(void *p, ::std::size_t n) {
	{ alloc::reallocate_zero_at_least(p, n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};

template <typename alloc>
concept has_reallocate_aligned_zero_at_least_impl = requires(void *p, ::std::size_t n) {
	{ alloc::reallocate_aligned_zero_at_least(p, n, n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};

template <typename alloc>
concept has_reallocate_n_at_least_impl = requires(void *p, ::std::size_t n) {
	{ alloc::reallocate_n_at_least(p, n, n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};

template <typename alloc>
concept has_reallocate_aligned_n_at_least_impl = requires(void *p, ::std::size_t n) {
	{ alloc::reallocate_aligned_n_at_least(p, n, n, n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};

template <typename alloc>
concept has_reallocate_zero_n_at_least_impl = requires(void *p, ::std::size_t n) {
	{ alloc::reallocate_zero_n_at_least(p, n, n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};

template <typename alloc>
concept has_reallocate_aligned_zero_n_at_least_impl = requires(void *p, ::std::size_t n) {
	{ alloc::reallocate_aligned_zero_n_at_least(p, n, n, n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};

template <typename alloc>
concept has_non_empty_handle_type = requires {
	typename alloc::handle_type;
	requires !::std::is_empty_v<typename alloc::handle_type>;
};

template <typename alloc>
concept has_handle_allocate_impl = requires(typename alloc::handle_type handle, ::std::size_t n) {
	{ alloc::handle_allocate(handle, n) } -> ::std::same_as<void *>;
};

template <typename alloc>
concept has_handle_allocate_aligned_impl = requires(typename alloc::handle_type handle, ::std::size_t n) {
	{ alloc::handle_allocate_aligned(handle, n, n) } -> ::std::same_as<void *>;
};

template <typename alloc>
concept has_handle_allocate_zero_impl = requires(typename alloc::handle_type handle, ::std::size_t n) {
	{ alloc::handle_allocate_zero(handle, n) } -> ::std::same_as<void *>;
};

template <typename alloc>
concept has_handle_allocate_aligned_zero_impl = requires(typename alloc::handle_type handle, ::std::size_t n) {
	{ alloc::handle_allocate_aligned_zero(handle, n, n) } -> ::std::same_as<void *>;
};

template <typename alloc>
concept has_handle_reallocate_impl = requires(typename alloc::handle_type handle, void *p, ::std::size_t n) {
	{ alloc::handle_reallocate(handle, p, n) } -> ::std::same_as<void *>;
};

template <typename alloc>
concept has_handle_reallocate_aligned_impl = requires(typename alloc::handle_type handle, void *p, ::std::size_t n) {
	{ alloc::handle_reallocate_aligned(handle, p, n, n) } -> ::std::same_as<void *>;
};

template <typename alloc>
concept has_handle_reallocate_zero_impl = requires(typename alloc::handle_type handle, void *p, ::std::size_t n) {
	{ alloc::handle_reallocate_zero(handle, p, n) } -> ::std::same_as<void *>;
};

template <typename alloc>
concept has_handle_reallocate_aligned_zero_impl = requires(typename alloc::handle_type handle, void *p, ::std::size_t n) {
	{ alloc::handle_reallocate_aligned_zero(handle, p, n, n) } -> ::std::same_as<void *>;
};

template <typename alloc>
concept has_handle_reallocate_n_impl = requires(typename alloc::handle_type handle, void *p, ::std::size_t n) {
	{ alloc::handle_reallocate_n(handle, p, n, n) } -> ::std::same_as<void *>;
};

template <typename alloc>
concept has_handle_reallocate_aligned_n_impl = requires(typename alloc::handle_type handle, void *p, ::std::size_t n) {
	{ alloc::handle_reallocate_aligned_n(handle, p, n, n, n) } -> ::std::same_as<void *>;
};

template <typename alloc>
concept has_handle_reallocate_zero_n_impl = requires(typename alloc::handle_type handle, void *p, ::std::size_t n) {
	{ alloc::handle_reallocate_zero_n(handle, p, n, n) } -> ::std::same_as<void *>;
};

template <typename alloc>
concept has_handle_reallocate_aligned_zero_n_impl = requires(typename alloc::handle_type handle, void *p, ::std::size_t n) {
	{ alloc::handle_reallocate_aligned_zero_n(handle, p, n, n, n) } -> ::std::same_as<void *>;
};


template <typename alloc>
concept has_handle_allocate_at_least_impl = requires(typename alloc::handle_type handle, ::std::size_t n) {
	{ alloc::handle_allocate_at_least(handle, n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};

template <typename alloc>
concept has_handle_allocate_aligned_at_least_impl = requires(typename alloc::handle_type handle, ::std::size_t n) {
	{ alloc::handle_allocate_aligned_at_least(handle, n, n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};

template <typename alloc>
concept has_handle_allocate_zero_at_least_impl = requires(typename alloc::handle_type handle, ::std::size_t n) {
	{ alloc::handle_allocate_zero_at_least(handle, n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};

template <typename alloc>
concept has_handle_allocate_aligned_zero_at_least_impl = requires(typename alloc::handle_type handle, ::std::size_t n) {
	{ alloc::handle_allocate_aligned_zero_at_least(handle, n, n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};

template <typename alloc>
concept has_handle_reallocate_at_least_impl = requires(typename alloc::handle_type handle, void *p, ::std::size_t n) {
	{ alloc::handle_reallocate_at_least(handle, p, n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};

template <typename alloc>
concept has_handle_reallocate_aligned_at_least_impl = requires(typename alloc::handle_type handle, void *p, ::std::size_t n) {
	{ alloc::handle_reallocate_aligned_at_least(handle, p, n, n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};

template <typename alloc>
concept has_handle_reallocate_zero_at_least_impl = requires(typename alloc::handle_type handle, void *p, ::std::size_t n) {
	{ alloc::handle_reallocate_zero_at_least(handle, p, n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};

template <typename alloc>
concept has_handle_reallocate_aligned_zero_at_least_impl = requires(typename alloc::handle_type handle, void *p, ::std::size_t n) {
	{ alloc::handle_reallocate_aligned_zero_at_least(handle, p, n, n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};

template <typename alloc>
concept has_handle_reallocate_n_at_least_impl = requires(typename alloc::handle_type handle, void *p, ::std::size_t n) {
	{ alloc::handle_reallocate_n_at_least(handle, p, n, n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};

template <typename alloc>
concept has_handle_reallocate_aligned_n_at_least_impl = requires(typename alloc::handle_type handle, void *p, ::std::size_t n) {
	{ alloc::handle_reallocate_aligned_n_at_least(handle, p, n, n, n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};

template <typename alloc>
concept has_handle_reallocate_zero_n_at_least_impl = requires(typename alloc::handle_type handle, void *p, ::std::size_t n) {
	{ alloc::handle_reallocate_zero_n_at_least(handle, p, n, n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};

template <typename alloc>
concept has_handle_reallocate_aligned_zero_n_at_least_impl = requires(typename alloc::handle_type handle, void *p, ::std::size_t n) {
	{ alloc::handle_reallocate_aligned_zero_n_at_least(handle, p, n, n, n) } -> ::std::same_as<::fast_io::allocation_least_result>;
};


template <typename alloc>
concept has_handle_deallocate_impl = requires(typename alloc::handle_type handle, void *p) {
	{ alloc::handle_deallocate(handle, p) } -> ::std::same_as<void>;
};

template <typename alloc>
concept has_handle_deallocate_aligned_impl = requires(typename alloc::handle_type handle, void *p, ::std::size_t n) {
	{ alloc::handle_deallocate_aligned(handle, p, n) } -> ::std::same_as<void>;
};

template <typename alloc>
concept has_handle_deallocate_n_impl = requires(typename alloc::handle_type handle, void *p, ::std::size_t n) {
	{ alloc::handle_deallocate_n(handle, p, n) } -> ::std::same_as<void>;
};

template <typename alloc>
concept has_handle_deallocate_aligned_n_impl = requires(typename alloc::handle_type handle, void *p, ::std::size_t n) {
	{ alloc::handle_deallocate_aligned_n(handle, p, n, n) } -> ::std::same_as<void>;
};
