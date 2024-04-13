#pragma once

namespace fast_io::containers
{

template<typename T>
class span
{
public:
	using element_type = T;
	using value_type = std::remove_cv_t<element_type>;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	using pointer = element_type*;
	using const_pointer = element_type const*;
	using reference = element_type&;
	using const_reference = element_type const&;
	using iterator = pointer;
	using reverse_iterator = ::std::reverse_iterator<iterator>;
#ifdef __cpp_lib_ranges_as_const
	using const_iterator = ::std::basic_const_iterator<iterator>;
	using const_reverse_iterator = ::std::basic_const_iterator<reverse_iterator>;
#else
	using const_iterator = const_pointer;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;
#endif
	pointer ptr{};
	size_type n{};
	constexpr span() noexcept = default;
	template<::std::contiguous_iterator Iter>
	requires ::std::same_as<value_type,::std::iter_value_t<Iter>>
	constexpr span(Iter first, size_type count) noexcept(noexcept(::std::to_address(first))):
		ptr{::std::to_address(first)},n{count}
	{}
	template<::std::contiguous_iterator Iter, ::std::ranges::sentinel_for<Iter> S>
	requires ::std::same_as<value_type,::std::iter_value_t<Iter>>
	constexpr span(Iter first, S snt) noexcept(noexcept(::std::to_address(first))):
		ptr{::std::to_address(first)},n{snt-first}
	{}
	template< ::std::ranges::contiguous_range R >
	requires (::std::same_as<value_type,::std::ranges::range_value_t<R>>&&!::std::same_as<::std::remove_cvref_t<R>,::fast_io::containers::span<element_type>>)
	constexpr span( R&& range ) noexcept(noexcept(::std::ranges::data(range))):
		ptr{::std::ranges::data(range)},n{snt-first}
	{}

	constexpr pointer data() const noexcept
	{
		return ptr;
	}
	constexpr iterator begin() const noexcept
	{
		return iterator(ptr);
	}
	constexpr const_iterator cbegin() const noexcept
	{
		return const_iterator(ptr);
	}
	constexpr iterator end() const noexcept
	{
		return iterator(ptr+n);
	}
	constexpr const_iterator cend() const noexcept
	{
		return const_iterator(ptr+n);
	}

	constexpr reverse_iterator rbegin() const noexcept
	{
		return reverse_iterator(ptr+n);
	}
	constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator(ptr+n);
	}
	constexpr reverse_iterator rend() const noexcept
	{
		return reverse_iterator(ptr);
	}
	constexpr const_reverse_iterator crend() const noexcept
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
		if (n <= idx) [[unlikely]]
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
		if (!n) [[unlikely]]
		{
			::fast_io::fast_terminate();
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
	front() const noexcept
	{
		if (!n) [[unlikely]]
		{
			::fast_io::fast_terminate();
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

	inline constexpr bool empty() const noexcept
	{
		return !n;
	}

	inline constexpr size_type size() const noexcept
	{
		return n;
	}

	inline static constexpr size_type max_size() noexcept
	{
		constexpr size_type mxsz{SIZE_MAX / sizeof(element_type)};
		return mxsz;
	}

	inline constexpr size_type size_bytes() const noexcept
	{
		return n * sizeof(element_type);
	}
};

}