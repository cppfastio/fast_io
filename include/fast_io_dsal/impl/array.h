#pragma once

namespace fast_io::containers
{

template <typename T, ::std::size_t N>
class array
{
	static_assert((N <= ::std::numeric_limits<::std::size_t>::max() / sizeof(T)), "N is too big");

public:
	using value_type = T;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	using reference = value_type &;
	using const_reference = value_type const &;
	using pointer = value_type *;
	using const_pointer = value_type const *;
	using iterator = pointer;
	using const_iterator = const_pointer;
	using reverse_iterator = ::std::reverse_iterator<iterator>;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(no_unique_address)
	[[no_unique_address]]
#elif __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#endif
#endif
	value_type element[N];

	static inline constexpr bool is_empty() noexcept
	{
		return false;
	}
	static inline constexpr bool empty() noexcept
	{
		return false;
	}
	static inline constexpr size_type size() noexcept
	{
		return N;
	}
	static inline constexpr size_type size_bytes() noexcept
	{
		constexpr size_type szb{N * sizeof(value_type)};
		return szb;
	}
	static inline constexpr size_type max_size() noexcept
	{
		return N;
	}
	static inline constexpr size_type max_size_bytes() noexcept
	{
		constexpr size_type szb{N * sizeof(value_type)};
		return szb;
	}
	inline constexpr iterator begin() noexcept
	{
		return element;
	}
	inline constexpr const_iterator begin() const noexcept
	{
		return element;
	}
	inline constexpr const_iterator cbegin() const noexcept
	{
		return element;
	}
	inline constexpr iterator end() noexcept
	{
		return element + N;
	}
	inline constexpr const_iterator end() const noexcept
	{
		return element + N;
	}
	inline constexpr const_iterator cend() const noexcept
	{
		return element + N;
	}

	inline constexpr reverse_iterator rbegin() noexcept
	{
		return reverse_iterator(element + N);
	}
	inline constexpr const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator(element + N);
	}
	inline constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator(element + N);
	}
	inline constexpr reverse_iterator rend() noexcept
	{
		return reverse_iterator(element);
	}
	inline constexpr const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator(element);
	}
	inline constexpr const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator(element);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_pointer data() const noexcept
	{
		return element;
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr pointer data() noexcept
	{
		return element;
	}

	inline constexpr void fill(const_reference u) noexcept
	{
		::std::fill_n(element, N, u);
	}

	inline constexpr void swap(array &other) noexcept(::std::is_nothrow_swappable_v<value_type>)
	{
		::std::swap_ranges(element, element + N, other.element);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference operator[](size_type idx) noexcept
	{
		if (N <= idx) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return element[idx];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference operator[](size_type idx) const noexcept
	{
		if (N <= idx) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return element[idx];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference index_unchecked(size_type idx) noexcept
	{
		return element[idx];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference index_unchecked(size_type idx) const noexcept
	{
		return element[idx];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference back() noexcept
	{
		constexpr size_type nm1{N - 1};
		return element[nm1];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference back() const noexcept
	{
		constexpr size_type nm1{N - 1};
		return element[nm1];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference back_unchecked() noexcept
	{
		constexpr size_type nm1{N - 1};
		return element[nm1];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference back_unchecked() const noexcept
	{
		constexpr size_type nm1{N - 1};
		return element[nm1];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference front() noexcept
	{
		return *element;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference front() const noexcept
	{
		return *element;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference front_unchecked() noexcept
	{
		return *element;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference front_unchecked() const noexcept
	{
		return *element;
	}
};

template <typename T>
class array<T, 0>
{
public:
	using value_type = T;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	using reference = value_type &;
	using const_reference = value_type const &;
	using pointer = value_type *;
	using const_pointer = value_type const *;
	using iterator = pointer;
	using const_iterator = const_pointer;
	using reverse_iterator = ::std::reverse_iterator<iterator>;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;

	static inline constexpr bool is_empty() noexcept
	{
		return true;
	}
	static inline constexpr bool empty() noexcept
	{
		return true;
	}
	static inline constexpr size_type size() noexcept
	{
		return 0;
	}
	static inline constexpr size_type max_size() noexcept
	{
		return 0;
	}
	inline constexpr iterator begin() noexcept
	{
		return nullptr;
	}
	inline constexpr const_iterator begin() const noexcept
	{
		return nullptr;
	}
	inline constexpr const_iterator cbegin() const noexcept
	{
		return nullptr;
	}
	inline constexpr iterator end() noexcept
	{
		return nullptr;
	}
	inline constexpr const_iterator end() const noexcept
	{
		return nullptr;
	}
	inline constexpr const_iterator cend() const noexcept
	{
		return nullptr;
	}

	inline constexpr reverse_iterator rbegin() noexcept
	{
		return reverse_iterator(nullptr);
	}
	inline constexpr const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator(nullptr);
	}
	inline constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator(nullptr);
	}
	inline constexpr reverse_iterator rend() noexcept
	{
		return reverse_iterator(nullptr);
	}
	inline constexpr const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator(nullptr);
	}
	inline constexpr const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator(nullptr);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_pointer data() const noexcept
	{
		return nullptr;
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr pointer data() noexcept
	{
		return nullptr;
	}

	inline constexpr void fill(const_reference) noexcept
	{
	}

	inline constexpr void swap(array &) noexcept
	{
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference operator[](size_type) noexcept
	{
		::fast_io::fast_terminate();
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference operator[](size_type) const noexcept
	{
		::fast_io::fast_terminate();
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference index_unchecked(size_type) noexcept
	{
		::fast_io::unreachable();
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference index_unchecked(size_type) const noexcept
	{
		::fast_io::unreachable();
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference back() noexcept
	{
		::fast_io::fast_terminate();
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference back() const noexcept
	{
		::fast_io::fast_terminate();
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference back_unchecked() noexcept
	{
		::fast_io::unreachable();
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference back_unchecked() const noexcept
	{
		::fast_io::unreachable();
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference front() noexcept
	{
		::fast_io::fast_terminate();
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference front() const noexcept
	{
		::fast_io::fast_terminate();
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference front_unchecked() noexcept
	{
		::fast_io::unreachable();
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference front_unchecked() const noexcept
	{
		::fast_io::unreachable();
	}
};

template <typename T, ::std::size_t N>
inline constexpr void swap(::fast_io::containers::array<T, N> &a, ::fast_io::containers::array<T, N> &b) noexcept(noexcept(a.swap(b)))
{
	a.swap(b);
}

template <typename T, ::std::size_t N1, ::std::size_t N2>
	requires ::std::equality_comparable<T>
inline constexpr bool operator==(::fast_io::containers::array<T, N1> const &a, ::fast_io::containers::array<T, N2> const &b)
{
	if constexpr (N1 == N2)
	{
		if constexpr (N1 == 0)
		{
			return true;
		}
		else
		{
			return ::std::equal(a.data(), a.data() + N1, b.data());
		}
	}
	else
	{
		return false;
	}
}

#if __cpp_impl_three_way_comparison >= 201907L

template <typename T, ::std::size_t N1, ::std::size_t N2>
	requires ::std::three_way_comparable<T>
inline constexpr auto operator<=>(::fast_io::containers::array<T, N1> const &a, ::fast_io::containers::array<T, N2> const &b)
{
	return ::fast_io::freestanding::lexicographical_compare_three_way(a.data(), a.data() + N1, b.data(), b.data() + N2, ::std::compare_three_way{});
}

#endif

namespace details
{

template <typename T, std::size_t N, std::size_t... I>
inline constexpr ::fast_io::containers::array<std::remove_cv_t<T>, N>
to_array_lvalueref_impl(T (&a)[N], ::std::index_sequence<I...>)
{
	return {{a[I]...}};
}

template <typename T, std::size_t N, std::size_t... I>
inline constexpr ::fast_io::containers::array<std::remove_cv_t<T>, N>
to_array_rvalueref_impl(T (&&a)[N], ::std::index_sequence<I...>)
{
	return {{::std::move(a[I])...}};
}
} // namespace details

template <typename T, typename... U>
	requires(::std::constructible_from<T, U> && ...)
array(T, U...) -> array<T, 1 + sizeof...(U)>;

template <typename T, ::std::size_t N>
	requires(!::std::is_array_v<T>)
inline constexpr ::fast_io::containers::array<::std::remove_cv_t<T>, N> to_array(T (&a)[N]) noexcept(::std::is_nothrow_copy_constructible_v<T>)
{
	return ::fast_io::containers::details::to_array_lvalueref_impl(a, ::std::make_index_sequence<N>{});
}

template <typename T, ::std::size_t N>
	requires(!::std::is_array_v<T>)
inline constexpr ::fast_io::containers::array<::std::remove_cv_t<T>, N> to_array(T (&&a)[N]) noexcept(::std::is_nothrow_move_constructible_v<T>)
{
	return ::fast_io::containers::details::to_array_rvalueref_impl(::std::move(a), ::std::make_index_sequence<N>{});
}

} // namespace fast_io::containers
