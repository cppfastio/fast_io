#pragma once

namespace fast_io {
namespace details {

template <typename T>
struct temp_array_scoped_ptr {
	using Alloc = ::fast_io::native_typed_thread_local_allocator<T>;

	T* ptr{};
	constexpr temp_array_scoped_ptr() noexcept = default;
	explicit constexpr temp_array_scoped_ptr(::std::size_t n) : ptr(Alloc::allocate(n)) {}
	temp_array_scoped_ptr(temp_array_scoped_ptr const&) = delete;
	temp_array_scoped_ptr& operator=(temp_array_scoped_ptr const&) = delete;
#if __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
		~temp_array_scoped_ptr() {
		Alloc::deallocate(ptr);
	}
	inline constexpr T* release() noexcept {
		auto temp{ptr};
		ptr = nullptr;
		return temp;
	}
	inline constexpr T const* get() const noexcept {
		return ptr;
	}
	inline constexpr T* get() noexcept {
		return ptr;
	}
	constexpr T& operator[](::std::size_t pos) noexcept {
		return ptr[pos];
	}
	constexpr T const& operator[](::std::size_t pos) const noexcept {
		return ptr[pos];
	}
};

template <::std::forward_iterator Iter>
inline
#if __cpp_constexpr_dynamic_alloc >= 201907L
//	constexpr
#endif
	char const* const*
	dup_enviro_impl_with_size(Iter begin, Iter end, ::std::size_t size) {
	temp_array_scoped_ptr<char const*> uptr(size + 1);
	if constexpr (requires(::std::iter_value_t<Iter> v) {
					  { v.c_str() } -> ::std::convertible_to<char const*>;
				  }) {
		for (char const** it{uptr.get()}; begin != end; ++begin) {
			*it = begin->c_str();
			++it;
		}
	} else
		non_overlapped_copy_n(begin, size, uptr.get());
	uptr[size] = nullptr;
	return uptr.release();
}

template <::std::forward_iterator Iter>
inline
#if __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	char const* const*
	dup_enviro_impl(Iter begin, Iter end) {
	return dup_enviro_impl_with_size(begin, end, static_cast<::std::size_t>(::std::distance(begin, end)));
}

template <::std::forward_iterator Iter>
inline
#if __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	char const* const*
	dup_enviro_entry(Iter begin, Iter end) {
	if constexpr (::std::contiguous_iterator<Iter>)
		return dup_enviro_impl(::std::to_address(begin), ::std::to_address(end));
	else
		return dup_enviro_impl(begin, end);
}

}  // namespace details

struct process_args {
	using Alloc = ::fast_io::native_typed_thread_local_allocator<char const*>;

	char const* const* args{};
	bool is_dynamic_allocated{};
	inline constexpr process_args(char const* const* envir) noexcept : args(envir) {}
	template <::std::random_access_iterator Iter>
		requires(::std::convertible_to<::std::iter_value_t<Iter>, char const*> || requires(::std::iter_value_t<Iter> v) {
					{ v.c_str() } -> ::std::convertible_to<char const*>;
				})
	inline constexpr process_args(Iter begin, Iter end) : args(details::dup_enviro_entry(begin, end)), is_dynamic_allocated(true) {}
	template <::std::ranges::random_access_range range>
		requires(::std::convertible_to<::std::ranges::range_value_t<range>, char const*> || requires(::std::ranges::range_value_t<range> v) {
			{ v.c_str() } -> ::std::convertible_to<char const*>;
		})
	inline constexpr process_args(range&& rg) : process_args(::std::ranges::cbegin(rg), ::std::ranges::cend(rg)) {}
	inline constexpr process_args(::std::initializer_list<char const*> ilist) : process_args(ilist.begin(), ilist.end()) {}
	process_args(process_args const&) = delete;
	process_args& operator=(process_args const&) = delete;
#if __cpp_constexpr_dynamic_alloc >= 201907L
	inline constexpr
#endif
	~process_args() {
		if (is_dynamic_allocated)
			Alloc::deallocate(const_cast<char const**>(args));
	}
};

}  // namespace fast_io
