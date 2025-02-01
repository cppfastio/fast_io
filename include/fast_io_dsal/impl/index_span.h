#pragma once

namespace fast_io::containers
{

struct index_unchecked_t
{
	explicit constexpr index_unchecked_t() noexcept = default;
};
inline constexpr index_unchecked_t index_unchecked{};

template <typename T, ::std::size_t N>
class index_span
{
	static_assert((N <= ::std::numeric_limits<::std::size_t>::max() / sizeof(T)), "N is too big");

public:
	using element_type = T;
	using span_type = ::fast_io::containers::span<element_type>;
	using value_type = std::remove_cv_t<element_type>;
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
	inline constexpr index_span() noexcept = default;
	template <::std::contiguous_iterator Iter>
		requires ::std::same_as<value_type, ::std::iter_value_t<Iter>>
	inline explicit constexpr index_span(::fast_io::containers::index_unchecked_t, Iter first) noexcept(noexcept(::std::to_address(first)))
		: ptr{::std::to_address(first)}
	{}
	template <::std::ranges::contiguous_range R>
		requires(::std::same_as<value_type, ::std::ranges::range_value_t<R>>)
	inline explicit constexpr index_span(::fast_io::containers::index_unchecked_t, R &&range) noexcept(noexcept(::std::ranges::data(range)))
		: ptr{::std::ranges::data(range)}
	{}
	template <::std::ranges::contiguous_range R>
		requires(::std::same_as<value_type, ::std::ranges::range_value_t<R>> && !::std::same_as<::std::remove_cvref_t<R>, ::fast_io::containers::index_span<element_type, N>>)
	inline constexpr index_span(R &&range) noexcept(noexcept(::std::ranges::data(range)) && noexcept(::std::ranges::size(range)))
		: ptr{::std::ranges::data(range)}
	{
		if (::std::ranges::size(range) < N) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
	}

	inline static constexpr size_type size() noexcept
	{
		return N;
	}

	inline static constexpr size_type size_bytes() noexcept
	{
		constexpr size_type nbs{N * sizeof(element_type)};
		return nbs;
	}

	inline static constexpr size_type max_size() noexcept
	{
		return N;
	}

	inline static constexpr size_type max_size_bytes() noexcept
	{
		constexpr size_type mxsz{N * sizeof(element_type)};
		return mxsz;
	}

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
		return iterator(ptr + N);
	}
	inline constexpr const_iterator cend() const noexcept
	{
		return const_iterator(ptr + N);
	}

	inline constexpr reverse_iterator rbegin() const noexcept
	{
		return reverse_iterator(ptr + N);
	}
	inline constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator(ptr + N);
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
	operator[](size_type idx) const noexcept
	{
		if (N <= idx) [[unlikely]]
		{
			::fast_io::fast_terminate();
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
	back() const noexcept
	{
		if constexpr (!N)
		{
			::fast_io::fast_terminate();
		}
		else
		{
			constexpr size_type nm1{N - 1u};
			return ptr[nm1];
		}
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference
	back_unchecked() const noexcept
	{
		constexpr size_type nm1{N - 1u};
		return ptr[nm1];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference
	front() const noexcept
	{
		if constexpr (!N)
		{
			::fast_io::fast_terminate();
		}
		else
		{
			return *ptr;
		}
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

	inline static constexpr bool is_empty() noexcept
	{
		constexpr bool emptyval{!N};
		return emptyval;
	}

	inline static constexpr bool empty() noexcept
	{
		constexpr bool emptyval{!N};
		return emptyval;
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr ::fast_io::containers::span<element_type> subspan_front(size_type count) const noexcept
	{
		if (N < count) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return ::fast_io::containers::span<element_type>(this->ptr, count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr ::fast_io::containers::span<element_type> subspan_front_unchecked(size_type count) const noexcept
	{
		return ::fast_io::containers::span<element_type>(this->ptr, count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr span_type subspan_back(size_type count) const noexcept
	{
		if (N < count) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return span_type(this->ptr + (N - count), count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr span_type subspan_back_unchecked(size_type count) const noexcept
	{
		return span_type(this->ptr + (N - count), count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr span_type subspan(size_type pos, size_type count = ::fast_io::containers::npos) const noexcept
	{
		if (this->n < pos) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		size_type const val{this->n - pos};
		if (val < count)
		{
			if (count != ::fast_io::containers::npos) [[unlikely]]
			{
				::fast_io::fast_terminate();
			}
			count = val;
		}
		return span_type(this->ptr + pos, count);
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr span_type subspan_unchecked(size_type pos, size_type count = ::fast_io::containers::npos) const noexcept
	{
		size_type const val{this->n - pos};
		if (count == ::fast_io::containers::npos)
		{
			count = val;
		}
		return span_type(this->ptr + pos, count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr size_type copy(element_type *dest, size_type count, size_type pos = 0) const noexcept
	{
		if (N < pos) [[unlikely]]
		{
			::fast_io::fast_terminate();
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
		size_type const val{N - pos};
		if (val < count)
		{
			count = val;
		}
		auto start{this->ptr + pos};
		::std::copy(start, start + count, dest);
		return count;
	}
	inline constexpr void fill(const_reference u) noexcept(!::std::is_const_v<element_type> && ::std::is_nothrow_copy_constructible_v<element_type>)
	{
		if constexpr (N)
		{
			::std::fill_n(this->ptr, N, u);
		}
	}
};

template <typename T, ::std::size_t N>
inline constexpr ::fast_io::containers::index_span<::std::byte const, sizeof(T) * N> as_index_bytes(::fast_io::containers::index_span<T, N> sp) noexcept
{
	return ::fast_io::containers::index_span<::std::byte const, sizeof(T) * N>(static_cast<::std::byte const *>(static_cast<void const *>(sp.ptr)));
}

template <typename T, ::std::size_t N>
	requires(!::std::is_const_v<T>)
inline constexpr ::fast_io::containers::index_span<::std::byte, sizeof(T) * N> as_index_writable_bytes(::fast_io::containers::index_span<T, N> sp) noexcept
{
	return ::fast_io::containers::index_span<::std::byte, sizeof(T) * N>(static_cast<::std::byte *>(static_cast<void *>(sp.ptr)));
}

template <typename T, ::std::size_t N>
inline constexpr ::fast_io::containers::span<::std::byte const> as_bytes(::fast_io::containers::index_span<T, N> sp) noexcept
{
	constexpr ::std::size_t bytescount{N * sizeof(T)};
	return ::fast_io::containers::span<::std::byte const>(static_cast<::std::byte const *>(static_cast<void const *>(sp.ptr)), bytescount);
}

template <typename T, ::std::size_t N>
	requires(!::std::is_const_v<T>)
inline constexpr ::fast_io::containers::span<::std::byte> as_writable_bytes(::fast_io::containers::index_span<T, N> sp) noexcept
{
	constexpr ::std::size_t bytescount{N * sizeof(T)};
	return ::fast_io::containers::span<::std::byte>(static_cast<::std::byte *>(static_cast<void *>(sp.ptr)), bytescount);
}

template <typename T, ::std::size_t N1, ::std::size_t N2>
	requires ::std::equality_comparable<T>
inline constexpr bool operator==(::fast_io::containers::index_span<T, N1> a, ::fast_io::containers::index_span<T, N2> b)
{
	if constexpr (N1 != N2)
	{
		return false;
	}
	else
	{
		return ::std::equal(a.ptr, a.ptr + N1, b.ptr, b.ptr + N2);
	}
}

#if __cpp_impl_three_way_comparison >= 201907L
template <typename T, ::std::size_t N1, ::std::size_t N2>
	requires ::std::three_way_comparable<T>
inline constexpr auto operator<=>(::fast_io::containers::index_span<T, N1> a, ::fast_io::containers::index_span<T, N2> b)
{
	return ::fast_io::freestanding::lexicographical_compare_three_way(a.ptr, a.ptr + N1, b.ptr, b.ptr + N2, ::std::compare_three_way{});
}
#endif

} // namespace fast_io::containers
