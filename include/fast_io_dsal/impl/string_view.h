#pragma once

namespace fast_io::containers
{
template <::std::integral char_type>
class basic_string_view
{
public:
	using value_type = char_type;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	using reference = value_type &;
	using const_reference = value_type const &;
	using pointer = value_type *;
	using const_pointer = value_type const *;
	using const_iterator = const_pointer;
	using iterator = const_iterator;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;
	using reverse_iterator = const_reverse_iterator;

	constexpr basic_string_view() noexcept = default;

	constexpr basic_string_view(::std::nullptr_t) = delete;

	explicit constexpr basic_string_view(const_pointer p, size_type s) noexcept
		: ptr{p}, n{s}
	{}

	template <::std::size_t N>
	constexpr basic_string_view(char_type const (&buffer)[N]) noexcept
	{
		constexpr ::std::size_t nm1{N - 1u};
		ptr = buffer;
		n = nm1;
	}

	template <::std::ranges::contiguous_range rg>
		requires(::std::same_as<::std::ranges::range_value_t<rg>, char_type> && !::std::is_array_v<::std::remove_cvref_t<rg>> &&
				 !::std::is_rvalue_reference_v<rg>)
	explicit constexpr basic_string_view(::fast_io::freestanding::from_range_t, rg const &&r) noexcept
		: ptr{::std::ranges::cdata(r)}, n{::std::ranges::size(r)}
	{
	}

	constexpr basic_string_view(::fast_io::manipulators::basic_os_c_str<char_type> osstr) noexcept
		: ptr(osstr.ptr), n(::fast_io::cstr_len(osstr.ptr))
	{}
	constexpr basic_string_view(::fast_io::manipulators::basic_os_c_str_with_known_size<char_type> osstr) noexcept
		: ptr(osstr.ptr), n(osstr.n)
	{}

	constexpr basic_string_view(basic_string_view const &) noexcept = default;
	constexpr basic_string_view &operator=(basic_string_view const &) noexcept = default;

	const_pointer ptr{};
	::std::size_t n{};

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
		constexpr size_type n{SIZE_MAX / sizeof(value_type)};
		return n;
	}

	inline constexpr const_iterator begin() const noexcept
	{
		return ptr;
	}

	inline constexpr const_iterator cbegin() const noexcept
	{
		return ptr;
	}

	inline constexpr const_iterator end() const noexcept
	{
		return ptr + n;
	}

	inline constexpr const_iterator cend() const noexcept
	{
		return ptr + n;
	}

	inline constexpr const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator(ptr + n);
	}

	inline constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator(ptr + n);
	}

	inline constexpr const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator(ptr);
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
	inline constexpr const_pointer
	data() const noexcept
	{
		return ptr;
	}

	constexpr void swap(basic_string_view &other) noexcept
	{
		basic_string_view const tmp{other};
		other = *this;
		*this = tmp;
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference
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
	inline constexpr const_reference
	index_unchecked(size_type idx) const noexcept
	{
		return ptr[idx];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr const_reference
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
	inline constexpr const_reference
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
	inline constexpr const_reference
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
	inline constexpr const_reference
	front_unchecked() const noexcept
	{
		return *ptr;
	}

	inline constexpr bool starts_with(basic_string_view sv) const noexcept
	{
		if (sv.n <= n)
		{
			return ::std::equal(ptr, ptr + sv.n, sv.ptr, sv.ptr + sv.n);
		}
		return false;
	}
	inline constexpr bool starts_with_character(value_type ch) const noexcept
	{
		return !n && ch == *ptr;
	}

	inline constexpr bool ends_with(basic_string_view sv) const noexcept
	{
		if (sv.n <= n)
		{
			return ::std::equal(ptr + (n - sv.n), ptr + n, sv.ptr, sv.ptr + sv.n);
		}
		return false;
	}
	inline constexpr bool ends_with_character(value_type ch) const noexcept
	{
		return !n && ch == ptr[n - 1u];
	}
};

template <::std::integral char_type>
constexpr void swap(basic_string_view<char_type> &a, basic_string_view<char_type> &b) noexcept
{
	a.swap(b);
}

template <::std::integral char_type>
inline constexpr ::fast_io::basic_io_scatter_t<char_type> print_alias_define(io_alias_t, basic_string_view<char_type> str) noexcept
{
	return {str.ptr, str.n};
}

template <::std::integral char_type>
constexpr bool operator==(basic_string_view<char_type> a, basic_string_view<char_type> b) noexcept
{
	return ::std::equal(a.ptr, a.ptr + a.n, b.ptr, b.ptr + b.n);
}

template <::std::integral char_type, ::std::size_t n>
constexpr bool operator==(basic_string_view<char_type> a, char_type const (&buffer)[n]) noexcept
{
	constexpr ::std::size_t nm1{n - 1u};
	return ::std::equal(a.ptr, a.ptr + a.n, buffer, buffer + nm1);
}

template <::std::integral char_type, ::std::size_t n>
constexpr bool operator==(char_type const (&buffer)[n], basic_string_view<char_type> a) noexcept
{
	constexpr ::std::size_t nm1{n - 1u};
	return ::std::equal(buffer, buffer + nm1, a.ptr, a.ptr + a.n);
}

#ifdef __cpp_lib_three_way_comparison
template <::std::integral char_type>
constexpr auto operator<=>(basic_string_view<char_type> a, basic_string_view<char_type> b) noexcept
{
	return ::std::lexicographical_compare_three_way(a.ptr, a.ptr + a.n, b.ptr, b.ptr + b.n, ::std::compare_three_way{});
}

template <::std::integral char_type, ::std::size_t n>
constexpr auto operator<=>(basic_string_view<char_type> a, char_type const (&buffer)[n]) noexcept
{
	constexpr ::std::size_t nm1{n - 1u};
	return ::std::lexicographical_compare_three_way(a.ptr, a.ptr + a.n, buffer, buffer + nm1, ::std::compare_three_way{});
}

template <::std::integral char_type, ::std::size_t n>
constexpr auto operator<=>(char_type const (&buffer)[n], basic_string_view<char_type> a) noexcept
{
	constexpr ::std::size_t nm1{n - 1u};
	return ::std::lexicographical_compare_three_way(buffer, buffer + nm1, a.ptr, a.ptr + a.n, ::std::compare_three_way{});
}

#endif

} // namespace fast_io::containers
