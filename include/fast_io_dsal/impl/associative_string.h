#pragma once

namespace fast_io::details
{
/*
The string is null terminated
*/
template <::std::integral chtype>
struct associative_string
{
	using char_type = chtype;
	using size_type = ::std::size_t;
	char_type const *ptr;
	size_type n;
	constexpr ::fast_io::containers::basic_cstring_view<char_type> strvw() const noexcept
	{
		return ::fast_io::containers::basic_cstring_view<char_type>(::fast_io::null_terminated, ptr, n);
	}
	constexpr ::fast_io::containers::basic_string_view<char_type> key() const noexcept
	{
		return ::fast_io::containers::basic_string_view<char_type>(ptr, n);
	}
};

template <::std::integral chtype>
inline constexpr bool operator==(::fast_io::details::associative_string<chtype> a, ::fast_io::containers::basic_string_view<chtype> b) noexcept
{
	return a.key() == b;
}

template <::std::integral chtype>
inline constexpr bool operator==(::fast_io::containers::basic_string_view<chtype> b, ::fast_io::details::associative_string<chtype> a) noexcept
{
	return a.key() == b;
}

template <::std::integral chtype>
inline constexpr bool operator!=(::fast_io::details::associative_string<chtype> a, ::fast_io::containers::basic_string_view<chtype> b) noexcept
{
	return a.key() != b;
}

template <::std::integral chtype>
inline constexpr bool operator!=(::fast_io::containers::basic_string_view<chtype> b, ::fast_io::details::associative_string<chtype> a) noexcept
{
	return a.key() != b;
}

template <typename allocator_type, ::std::integral char_type>
inline constexpr ::fast_io::details::associative_string<char_type> create_associative_string(char_type const *p, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator_type>)
{
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, char_type>;
	if (n == SIZE_MAX) [[unlikely]]
	{
		::fast_io::containers::details::contract_violation_report<typed_allocator_type::throws_on_overflow>(::std::errc::value_too_large);
	}
	::std::size_t const np1{static_cast<::std::size_t>(n + 1u)};
	char_type *newp{typed_allocator_type::allocate(np1)};
	*::fast_io::details::non_overlapped_copy_n(p, n, newp) = 0;
	return {newp, n};
}

template <typename allocator_type, ::std::integral char_type>
inline constexpr void deallocate_associative_string(void const *p, ::std::size_t n) noexcept
{
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, char_type>;
	::std::size_t const np1{static_cast<::std::size_t>(n + 1u)};
	typed_allocator_type::deallocate_n(static_cast<char_type *>(const_cast<void *>(p)), np1);
}

} // namespace fast_io::details
