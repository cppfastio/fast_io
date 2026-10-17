#pragma once

namespace fast_io::containers
{

template <typename T, ::fast_io::allocator_adapter_flags flags = ::fast_io::allocator_adapter_flags::none>
class span
{
	static inline constexpr bool throwing_violations{
		flags == ::fast_io::allocator_adapter_flags::throws_on_violations};

public:
	using element_type = T;
	using value_type = ::std::remove_cv_t<element_type>;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	using pointer = element_type *;
	using const_pointer = element_type const *;
	using reference = element_type &;
	using const_reference = element_type const &;
	using iterator = pointer;
	using reverse_iterator = ::std::reverse_iterator<iterator>;
#ifdef __cpp_lib_ranges_as_const
#if __cpp_lib_ranges_as_const >= 202311L
	using const_iterator = ::std::basic_const_iterator<iterator>;
	using const_reverse_iterator = ::std::basic_const_iterator<reverse_iterator>;
#else
	using const_iterator = const_pointer;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;
#endif
#else
	using const_iterator = const_pointer;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;
#endif
	pointer ptr{};
	size_type n{};
	inline constexpr explicit span() noexcept = default;
	template <::std::contiguous_iterator Iter>
		requires ::std::same_as<value_type, ::std::iter_value_t<Iter>>
	inline constexpr explicit span(Iter first, size_type count) noexcept(noexcept(::std::to_address(first)))
		: ptr{::std::to_address(first)}, n{count}
	{}
	template <::std::contiguous_iterator Iter, ::std::sentinel_for<Iter> S>
		requires ::std::same_as<value_type, ::std::iter_value_t<Iter>>
	inline constexpr explicit span(Iter first, S snt) noexcept(noexcept(::std::to_address(first)))
		: ptr{::std::to_address(first)}, n{static_cast<size_type>(snt - first)}
	{}
	template <::std::ranges::contiguous_range R>
		requires(::std::same_as<value_type, ::std::ranges::range_value_t<R>> &&
				 !::std::is_rvalue_reference_v<R &&>)
	inline constexpr explicit span(::fast_io::freestanding::from_range_t, R &&range) noexcept(noexcept(::std::ranges::data(range)) && noexcept(::std::ranges::size(range)))
		: ptr{::std::ranges::data(range)}, n{::std::ranges::size(range)}
	{}

	inline constexpr pointer data() const noexcept
	{
		return ptr;
	}
	inline constexpr iterator begin() const noexcept
	{
		return iterator(ptr);
	}
	inline constexpr const_iterator cbegin() const noexcept
	{
		return const_iterator(ptr);
	}
	inline constexpr iterator end() const noexcept
	{
		return iterator(ptr + n);
	}
	inline constexpr const_iterator cend() const noexcept
	{
		return const_iterator(ptr + n);
	}

	inline constexpr reverse_iterator rbegin() const noexcept
	{
		return reverse_iterator(ptr + n);
	}
	inline constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator(ptr + n);
	}
	inline constexpr reverse_iterator rend() const noexcept
	{
		return reverse_iterator(ptr);
	}
	inline constexpr const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator(ptr);
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference
	operator[](size_type idx) const FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (n <= idx) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report_invalid_argument<throwing_violations>();
		}
		return ptr[idx];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference
	index_unchecked(size_type idx) const noexcept
	{
		return ptr[idx];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference
	back() const FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (!n) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		size_type const nm1{n - 1};
		return ptr[nm1];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference
	back_unchecked() const noexcept
	{
		size_type const nm1{n - 1};
		return ptr[nm1];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference
	front() const FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (!n) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		return *ptr;
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference
	front_unchecked() const noexcept
	{
		return *ptr;
	}

	inline constexpr bool is_empty() const noexcept
	{
		return !n;
	}


	inline constexpr size_type size() const noexcept
	{
		return n;
	}

	inline constexpr size_type size_bytes() const noexcept
	{
		return n * sizeof(element_type);
	}

	inline static constexpr size_type max_size() noexcept
	{
		constexpr size_type mxsz{SIZE_MAX / sizeof(element_type)};
		return mxsz;
	}

	inline static constexpr size_type max_size_bytes() noexcept
	{
		constexpr size_type mxsz{SIZE_MAX / sizeof(element_type) * sizeof(element_type)};
		return mxsz;
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr span subspan_front(size_type count) const FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (this->n < count) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		return span(this->ptr, count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr span subspan_front_unchecked(size_type count) const noexcept
	{
		return span(this->ptr, count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr span subspan_back(size_type count) const FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (this->n < count) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		return span(this->ptr + (this->n - count), count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr span subspan_back_unchecked(size_type count) const noexcept
	{
		return span(this->ptr + (this->n - count), count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr span subspan(size_type pos, size_type count = ::fast_io::containers::npos) const FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (this->n < pos) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		size_type const val{this->n - pos};
		if (val < count)
		{
			if (count != ::fast_io::containers::npos) [[unlikely]]
			{
				::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
			}
			count = val;
		}
		return span(this->ptr + pos, count);
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr span subspan_unchecked(size_type pos, size_type count = ::fast_io::containers::npos) const noexcept
	{
		size_type const val{this->n - pos};
		if (count == ::fast_io::containers::npos)
		{
			count = val;
		}
		return span(this->ptr + pos, count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr size_type copy(element_type *dest, size_type count, size_type pos = 0) const FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		size_type const thisn{this->n};
		if (thisn < pos) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		return this->copy_unchecked(dest, count, pos);
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr size_type copy_unchecked(element_type *dest, size_type count, size_type pos = 0) const noexcept
	{
		size_type const thisn{this->n};
		size_type const val{thisn - pos};
		if (val < count)
		{
			count = val;
		}
		auto start{this->ptr + pos};
		::std::copy(start, start + count, dest);
		return count;
	}
};

template <::std::contiguous_iterator Iter>
span(Iter, ::std::size_t) -> span<::std::remove_reference_t<::std::iter_reference_t<Iter>>>;

template <::std::contiguous_iterator Iter, ::std::sentinel_for<Iter> S>
span(Iter, S) -> span<::std::remove_reference_t<::std::iter_reference_t<Iter>>>;

template <typename T, ::fast_io::allocator_adapter_flags flags>
inline constexpr ::fast_io::containers::span<::std::byte const, flags> as_bytes(::fast_io::containers::span<T, flags> sp) noexcept
{
	return ::fast_io::containers::span<::std::byte const, flags>(static_cast<::std::byte const *>(static_cast<void const *>(sp.ptr)), sp.n * sizeof(T));
}

template <typename T, ::fast_io::allocator_adapter_flags flags>
	requires(!::std::is_const_v<T>)
inline constexpr ::fast_io::containers::span<::std::byte, flags> as_writable_bytes(::fast_io::containers::span<T, flags> sp) noexcept
{
	return ::fast_io::containers::span<::std::byte, flags>(static_cast<::std::byte *>(static_cast<void *>(sp.ptr)), sp.n * sizeof(T));
}

template <::std::ranges::contiguous_range R>
span(::fast_io::freestanding::from_range_t, R &&r) -> span<::std::remove_reference_t<::std::ranges::range_reference_t<R>>>;

template <typename T, ::fast_io::allocator_adapter_flags f1, ::fast_io::allocator_adapter_flags f2>
	requires(::std::equality_comparable<T>)
inline constexpr bool operator==(::fast_io::containers::span<T, f1> a, ::fast_io::containers::span<T, f2> b)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(*a.ptr == *b.ptr))
{
	return ::std::equal(a.ptr, a.ptr + a.n, b.ptr, b.ptr + b.n);
}

#if __cpp_impl_three_way_comparison >= 201907L
template <typename T, ::fast_io::allocator_adapter_flags f1, ::fast_io::allocator_adapter_flags f2>
	requires(::std::three_way_comparable<T>)
inline constexpr auto operator<=>(::fast_io::containers::span<T, f1> a, ::fast_io::containers::span<T, f2> b)
{
	return ::fast_io::freestanding::lexicographical_compare_three_way(a.ptr, a.ptr + a.n, b.ptr, b.ptr + b.n, ::std::compare_three_way{});
}
#endif

} // namespace fast_io::containers
