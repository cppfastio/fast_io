#pragma once

namespace fast_io::containers
{

struct null_terminated_t
{
	explicit inline constexpr null_terminated_t() noexcept = default;
};

inline constexpr null_terminated_t null_terminated{};

template <::std::integral char_type>
class basic_cstring_view : private ::fast_io::containers::basic_string_view<char_type>
{
public:
	using string_view_type = ::fast_io::containers::basic_string_view<char_type>;
	using typename string_view_type::value_type;
	using typename string_view_type::size_type;
	using typename string_view_type::difference_type;
	using typename string_view_type::reference;
	using typename string_view_type::const_reference;
	using typename string_view_type::const_pointer;
	using typename string_view_type::pointer;
	using typename string_view_type::const_iterator;
	using typename string_view_type::iterator;
	using typename string_view_type::const_reverse_iterator;
	using typename string_view_type::reverse_iterator;
	using string_view_type::ptr;
	using string_view_type::n;

	inline constexpr basic_cstring_view() noexcept
		: string_view_type(::fast_io::null_terminated_c_str_v<char_type>, 0)
	{}
	inline constexpr void clear() noexcept
	{
		this->ptr = ::fast_io::null_terminated_c_str_v<char_type>;
		this->n = 0;
	}
	inline constexpr basic_cstring_view(::std::nullptr_t) = delete;

	inline explicit constexpr basic_cstring_view(::fast_io::containers::null_terminated_t, const_pointer p, size_type s) noexcept
		: string_view_type(p, s)
	{}

	template <size_type N>
	inline constexpr basic_cstring_view(char_type const (&buffer)[N]) noexcept
		: string_view_type(buffer)
	{
		constexpr size_type nm1{N - 1};
		if (buffer[nm1]) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
	}

	inline explicit constexpr basic_cstring_view(::fast_io::containers::null_terminated_t, string_view_type stvw) noexcept
		: string_view_type(stvw)
	{
	}

	template <::std::ranges::contiguous_range rg>
		requires(::std::constructible_from<string_view_type, rg const &&>)
	inline explicit constexpr basic_cstring_view(::fast_io::containers::null_terminated_t, ::fast_io::freestanding::from_range_t, rg const &&r) noexcept
		: string_view_type(::std::forward<rg>(r))
	{
	}

	inline constexpr basic_cstring_view(::fast_io::manipulators::basic_os_c_str<char_type> osstr) noexcept
		: string_view_type(osstr)
	{}
	inline constexpr basic_cstring_view(::fast_io::manipulators::basic_os_c_str_with_known_size<char_type> osstr) noexcept
		: string_view_type(osstr)
	{}

	inline constexpr basic_cstring_view(basic_cstring_view const &) noexcept = default;
	inline constexpr basic_cstring_view &operator=(basic_cstring_view const &) noexcept = default;
	inline constexpr void swap(basic_cstring_view &other) noexcept
	{
		basic_cstring_view const tmp{other};
		other = *this;
		*this = tmp;
	}
	inline constexpr const_pointer c_str() const noexcept
	{
		return this->ptr;
	}

	using string_view_type::is_empty;
	using string_view_type::empty;
	using string_view_type::size;
	using string_view_type::size_bytes;
	using string_view_type::max_size;
	using string_view_type::max_size_bytes;
	using string_view_type::begin;
	using string_view_type::cbegin;
	using string_view_type::end;
	using string_view_type::cend;
	using string_view_type::rbegin;
	using string_view_type::crbegin;
	using string_view_type::rend;
	using string_view_type::crend;
	using string_view_type::data;
	using string_view_type::index_unchecked;
	using string_view_type::operator[];
	using string_view_type::front;
	using string_view_type::front_unchecked;
	using string_view_type::back;
	using string_view_type::back_unchecked;
	using string_view_type::starts_with;
	using string_view_type::starts_with_character;
	using string_view_type::ends_with;
	using string_view_type::ends_with_character;
	using string_view_type::remove_prefix;
	using string_view_type::remove_prefix_unchecked;
	using string_view_type::contains;
	using string_view_type::contains_character;
	using string_view_type::subview_front;
	using string_view_type::subview_front_unchecked;

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr basic_cstring_view subview_back(size_type count) const noexcept
	{
		if (this->n < count) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return basic_cstring_view(this->ptr + (this->n - count), count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr basic_cstring_view subview_back_unchecked(size_type count) const noexcept
	{
		return basic_cstring_view(this->ptr + (this->n - count), count);
	}

	using string_view_type::subview;
	using string_view_type::subview_unchecked;
	using string_view_type::copy;
	using string_view_type::copy_unchecked;
	using string_view_type::find_character;
	using string_view_type::rfind_character;
	using string_view_type::find_not_character;
	using string_view_type::rfind_not_character;
	using string_view_type::find;
	using string_view_type::rfind;
	using string_view_type::find_first_of;
	using string_view_type::find_first_not_of;
	using string_view_type::find_last_of;
	using string_view_type::find_last_not_of;
	using string_view_type::compare_three_way;
	using string_view_type::compare_three_way_unchecked;
	using string_view_type::trim_subview;
	using string_view_type::trim_subview_c_space;
	template <typename charcate>
		requires requires(charcate traits, const_pointer beginptr, const_pointer currptr) {
			{ traits.find_not(beginptr, currptr) } -> ::std::convertible_to<const_pointer>;
		}
	inline constexpr basic_cstring_view trim_prefix_subview(charcate traits) const noexcept
	{
		using string_view_type::trim_prefix_subview;
		auto csvw{string_view_type::trim_prefix_subview(traits)};
		return basic_cstring_view(csvw.ptr, csvw.n);
	}
	inline constexpr basic_cstring_view trim_prefix_subview_c_space() const noexcept
	{
		return trim_prefix_subview(::fast_io::char_category::c_space{});
	}
	using string_view_type::trim_suffix_subview;
	using string_view_type::trim_suffix_subview_c_space;
};

template <::std::integral char_type>
inline constexpr void swap(::fast_io::containers::basic_cstring_view<char_type> &a, basic_cstring_view<char_type> &b) noexcept
{
	a.swap(b);
}
template <::std::integral char_type>
inline constexpr ::fast_io::basic_io_scatter_t<char_type> print_alias_define(::fast_io::io_alias_t, ::fast_io::containers::basic_cstring_view<char_type> str) noexcept
{
	return {str.ptr, str.n};
}

template <::std::integral char_type>
inline constexpr bool operator==(::fast_io::containers::basic_cstring_view<char_type> a, ::fast_io::containers::basic_cstring_view<char_type> b) noexcept
{
	return ::std::equal(a.ptr, a.ptr + a.n, b.ptr, b.ptr + b.n);
}

template <::std::integral char_type, ::std::size_t n>
inline constexpr bool operator==(::fast_io::containers::basic_cstring_view<char_type> a, char_type const (&buffer)[n]) noexcept
{
	constexpr ::std::size_t nm1{n - 1u};
	return ::std::equal(a.ptr, a.ptr + a.n, buffer, buffer + nm1);
}

template <::std::integral char_type, ::std::size_t n>
inline constexpr bool operator==(char_type const (&buffer)[n], ::fast_io::containers::basic_cstring_view<char_type> a) noexcept
{
	constexpr ::std::size_t nm1{n - 1u};
	return ::std::equal(buffer, buffer + nm1, a.ptr, a.ptr + a.n);
}

template <::std::integral char_type>
inline constexpr bool operator==(::fast_io::containers::basic_cstring_view<char_type> a, ::fast_io::containers::basic_string_view<char_type> b) noexcept
{
	return ::std::equal(a.ptr, a.ptr + a.n, b.ptr, b.ptr + b.n);
}

template <::std::integral char_type>
inline constexpr bool operator==(::fast_io::containers::basic_string_view<char_type> a, ::fast_io::containers::basic_cstring_view<char_type> b) noexcept
{
	return b == a;
}

#if __cpp_impl_three_way_comparison >= 201907L
template <::std::integral char_type>
inline constexpr auto operator<=>(::fast_io::containers::basic_cstring_view<char_type> a, ::fast_io::containers::basic_cstring_view<char_type> b) noexcept
{
	return ::fast_io::freestanding::lexicographical_compare_three_way(a.ptr, a.ptr + a.n, b.ptr, b.ptr + b.n, ::std::compare_three_way{});
}

template <::std::integral char_type, ::std::size_t n>
inline constexpr auto operator<=>(::fast_io::containers::basic_cstring_view<char_type> a, char_type const (&buffer)[n]) noexcept
{
	constexpr ::std::size_t nm1{n - 1u};
	return ::fast_io::freestanding::lexicographical_compare_three_way(a.ptr, a.ptr + a.n, buffer, buffer + nm1, ::std::compare_three_way{});
}

template <::std::integral char_type, ::std::size_t n>
inline constexpr auto operator<=>(char_type const (&buffer)[n], ::fast_io::containers::basic_cstring_view<char_type> a) noexcept
{
	constexpr ::std::size_t nm1{n - 1u};
	return ::fast_io::freestanding::lexicographical_compare_three_way(buffer, buffer + nm1, a.ptr, a.ptr + a.n, ::std::compare_three_way{});
}

template <::std::integral char_type>
inline constexpr auto operator<=>(::fast_io::containers::basic_cstring_view<char_type> a, ::fast_io::containers::basic_string_view<char_type> b) noexcept
{
	return ::fast_io::freestanding::lexicographical_compare_three_way(a.ptr, a.ptr + a.n, b.ptr, b.ptr + b.n, ::std::compare_three_way{});
}

template <::std::integral char_type>
inline constexpr auto operator<=>(::fast_io::containers::basic_string_view<char_type> a, ::fast_io::containers::basic_cstring_view<char_type> b) noexcept
{
	return ::fast_io::freestanding::lexicographical_compare_three_way(a.ptr, a.ptr + a.n, b.ptr, b.ptr + b.n, ::std::compare_three_way{});
}

#endif

} // namespace fast_io::containers
