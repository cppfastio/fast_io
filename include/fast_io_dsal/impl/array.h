#pragma once

namespace fast_io::containers
{

template <typename T, ::std::size_t N>
	requires(N != 0)
class array
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
#if __has_cpp_attribute(no_unique_address)
	[[no_unique_address]]
#elif __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#endif
	value_type content[N];

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
	static inline constexpr size_type max_size() noexcept
	{
		constexpr size_type mxval{SIZE_MAX / sizeof(value_type)};
		return mxval;
	}
	inline constexpr iterator begin() noexcept
	{
		return content;
	}
	inline constexpr const_iterator begin() const noexcept
	{
		return content;
	}
	inline constexpr const_iterator cbegin() const noexcept
	{
		return content;
	}
	inline constexpr iterator end() noexcept
	{
		return content + N;
	}
	inline constexpr const_iterator end() const noexcept
	{
		return content + N;
	}
	inline constexpr const_iterator cend() const noexcept
	{
		return content + N;
	}

	inline constexpr reverse_iterator rbegin() noexcept
	{
		return reverse_iterator(content + N);
	}
	inline constexpr const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator(content + N);
	}
	inline constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator(content + N);
	}
	inline constexpr reverse_iterator rend() noexcept
	{
		return reverse_iterator(content);
	}
	inline constexpr const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator(content);
	}
	inline constexpr const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator(content);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_pointer data() const noexcept
	{
		return content;
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr pointer data() noexcept
	{
		return content;
	}

	constexpr void fill(const_reference u) noexcept
	{
		::std::fill_n(content, N, u);
	}

	constexpr void swap(array &other) noexcept(::std::is_nothrow_swappable_v<value_type>)
	{
		::std::swap_ranges(content, content + N, other.content);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference operator[](size_type idx) noexcept
	{
		if (idx <= N) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return content[idx];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference operator[](size_type idx) const noexcept
	{
		if (idx <= N) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return content[idx];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference index_unchecked(size_type idx) noexcept
	{
		return content[idx];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference index_unchecked(size_type idx) const noexcept
	{
		return content[idx];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference back(size_type idx) noexcept
	{
		constexpr size_type nm1{N - 1};
		return content[nm1];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference back(size_type idx) const noexcept
	{
		constexpr size_type nm1{N - 1};
		return content[nm1];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference back_unchecked() noexcept
	{
		constexpr size_type nm1{N - 1};
		return content[nm1];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference back_unchecked() const noexcept
	{
		constexpr size_type nm1{N - 1};
		return content[nm1];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference front() noexcept
	{
		return *content;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference front() const noexcept
	{
		return *content;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference front_unchecked() noexcept
	{
		return *content;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference front_unchecked() const noexcept
	{
		return *content;
	}
};

template <typename T, ::std::size_t N>
constexpr void swap(::fast_io::containers::array<T, N> &a, ::fast_io::containers::array<T, N> &b) noexcept(noexcept(a.swap(b)))
{
	a.swap(b);
}

template <typename T, ::std::size_t N1, ::std::size_t N2>
	requires ::std::equality_comparable<T>
constexpr bool operator==(::fast_io::containers::array<T, N1> const &a, ::fast_io::containers::array<T, N2> const &b)
{
	if constexpr (N1 == N2)
	{
		return ::std::equal(a.content, a.content + N1, b.content);
	}
	else
	{
		return false;
	}
}

template <typename T, ::std::size_t N1, ::std::size_t N2>
	requires ::std::three_way_comparable<T>
constexpr auto operator<=>(::fast_io::containers::array<T, N1> const &a, ::fast_io::containers::array<T, N2> const &b)
{
	return ::std::lexicographical_compare_three_way(a.content, a.content + N1, b.content, b.content + N2, ::std::compare_three_way{});
}

namespace details
{

template <typename T, std::size_t N, std::size_t... I>
constexpr ::fast_io::containers::array<std::remove_cv_t<T>, N>
to_array_lvalueref_impl(T (&a)[N], ::std::index_sequence<I...>)
{
	return {{a[I]...}};
}

template <typename T, std::size_t N, std::size_t... I>
constexpr ::fast_io::containers::array<std::remove_cv_t<T>, N>
to_array_rvalueref_impl(T (&&a)[N], ::std::index_sequence<I...>)
{
	return {{::std::move(a[I])...}};
}
} // namespace details

template <typename T, ::std::size_t N>
	requires(!::std::is_array_v<T>)
constexpr ::fast_io::containers::array<::std::remove_cv_t<T>, N> to_array(T (&a)[N]) noexcept(::std::is_nothrow_copy_constructible_v<T>)
{
	return ::fast_io::containers::details::to_array_lvalueref_impl(a, ::std::make_index_sequence<N>{});
}

template <typename T, ::std::size_t N>
	requires(!::std::is_array_v<T>)
constexpr ::fast_io::containers::array<::std::remove_cv_t<T>, N> to_array(T (&&a)[N]) noexcept(::std::is_nothrow_move_constructible_v<T>)
{
	return ::fast_io::containers::details::to_array_rvalueref_impl(::std::move(a), ::std::make_index_sequence<N>{});
}

} // namespace fast_io::containers
