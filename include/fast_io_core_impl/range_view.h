#pragma once

namespace fast_io {
template <::std::integral ch_type, ::std::input_iterator It>
struct sized_range_view_t
{
	using char_type = ch_type;
	using iterator = It;
	using value_type = decltype(io_print_alias(*(It{})));
	basic_io_scatter_t<char_type> sep;
	iterator begin;
	::std::size_t size;
};
template <::std::integral char_type, ::std::input_iterator I>
sized_range_view_t(basic_io_scatter_t<char_type>, I, ::std::size_t)->sized_range_view_t<char_type, I>;

template <::std::integral ch_type, ::std::input_iterator It>
struct range_view_t
{
	using char_type = ch_type;
	using iterator = It;
	basic_io_scatter_t<char_type> sep;
	iterator begin;
	iterator end;
};
template <::std::integral char_type, ::std::input_iterator I>
range_view_t(basic_io_scatter_t<char_type>, I, I)->range_view_t<char_type, I>;

template <::std::integral char_type, ::std::input_iterator It>
inline constexpr ::std::size_t print_reserve_size(io_reserve_type_t<char_type, sized_range_view_t<char_type, It>>, sized_range_view_t<char_type, It> t)
{
	if (t.size == 0) return 0;
	using T = sized_range_view_t<char_type, It>;
	if constexpr (reserve_printable<char_type, typename T::value_type>)
		// print_reserve_size(io_reserve_type<char_type, typename T::value_type>) * t.size + t.sep.len * (t.size - 1);
		return (print_reserve_size(io_reserve_type<char_type, typename T::value_type>) + t.sep.len) * t.size;
	else // dynamic_reserve_printable<char_type, T::value_type>
	{
		::std::size_t retval{ t.sep.len * t.size - 1 };
		auto curr_ptr{ t.begin };
		for (::std::size_t i{}; i != t.size; ++i, ++curr_ptr)
			retval += print_reserve_size(io_reserve_type<char_type, typename T::value_type>,
				::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(*curr_ptr)));
		return retval;
	}
}
template <::std::integral char_type, ::std::input_iterator It>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type, sized_range_view_t<char_type, It>>, char_type* __restrict ptr, sized_range_view_t<char_type, It> t)
{
	if (t.size == 0) return ptr;
	using value_type = typename sized_range_view_t<char_type, It>::value_type;
	auto curr_ptr{ t.begin };
	ptr = print_reserve_define(io_reserve_type<char_type, value_type>, ptr,
		::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(*curr_ptr)));
	++curr_ptr;
	for (::std::size_t i{}; i != t.size - 1; ++i, ++curr_ptr)
	{
		::fast_io::freestanding::non_overlapped_copy_n(t.sep.base, t.sep.len, ptr);
		ptr += t.sep.len;
		ptr = print_reserve_define(io_reserve_type<char_type, value_type>, ptr,
			::fast_io::io_print_forward<char_type>(::fast_io::io_print_alias(*curr_ptr)));
	}
	return ptr;
}

template <::std::integral char_type, ::std::input_iterator It, output_stream output>
inline constexpr void print_define(io_reserve_type_t<char_type, range_view_t<char_type, It>>, output out, range_view_t<char_type, It> t)
{
	if (t.begin == t.end) return;
	auto curr_ptr{ t.begin };
	print_freestanding(out, *curr_ptr);
	for (++curr_ptr; curr_ptr != t.end; ++curr_ptr)
	{
		print_freestanding(out, t.sep, *curr_ptr);
	}
}

namespace manipulators {
template <::std::ranges::range rg, ::std::integral char_type>
inline constexpr auto rgvw(rg&& r, char_type const (&sep)[2])
{
	using ::std::to_address;
	using ::std::ranges::begin;
	using ::std::ranges::end;
	using ::std::ranges::size;
	using ::fast_io::sized_range_view_t;
	using ::fast_io::range_view_t;
	using value_type = decltype(io_print_alias(*begin(r)));
	constexpr bool is_contiguous_range = ::std::ranges::contiguous_range<rg>;
	constexpr bool is_sized_range = ::std::ranges::sized_range<rg>;
	auto printed = ::fast_io::basic_io_scatter_t<char_type>{ reinterpret_cast<char_type const*>(__builtin_addressof(sep)), 1 };
	if constexpr (::fast_io::reserve_printable<char_type, value_type>)
	{
		if constexpr (is_contiguous_range && is_sized_range)
			return sized_range_view_t{ printed, to_address(begin(r)), size(r) };
		else if constexpr (is_contiguous_range)
			return sized_range_view_t{ printed, to_address(begin(r)), to_address(end(r)) - to_address(begin(r)) };
		else if constexpr (is_sized_range)
			return sized_range_view_t{ printed, begin(r), size(r) };
		else
			return range_view_t{ printed, begin(r), end(r) };
	}
	else
	{
		if constexpr (is_contiguous_range)
			return range_view_t{ printed, to_address(begin(r)), to_address(end(r)) };
		else
			return range_view_t{ printed, begin(r), end(r) };
	}
}

template <::std::ranges::range rg, ::fast_io::constructible_to_os_c_str T>
inline constexpr auto rgvw(rg&& r, T&& sep)
{
	using ::std::to_address;
	using ::std::ranges::begin;
	using ::std::ranges::end;
	using ::std::ranges::size;
	using ::fast_io::sized_range_view_t;
	using ::fast_io::range_view_t;
	using value_type = decltype(io_print_alias(*begin(r)));
	constexpr bool is_contiguous_range = ::std::ranges::contiguous_range<rg>;
	constexpr bool is_sized_range = ::std::ranges::sized_range<rg>;
	decltype(auto) printed = print_alias_define(io_alias, sep);
	using char_type = typename decltype(printed)::value_type;
	if constexpr (::fast_io::reserve_printable<char_type, value_type>)
	{
		if constexpr (is_contiguous_range && is_sized_range)
			return sized_range_view_t{ printed, to_address(begin(r)), size(r) };
		else if constexpr (is_contiguous_range)
			return sized_range_view_t{ printed, to_address(begin(r)), to_address(end(r)) - to_address(begin(r)) };
		else if constexpr (is_sized_range)
			return sized_range_view_t{ printed, begin(r), size(r) };
		else
			return range_view_t{ printed, begin(r), end(r) };
	}
	else
	{
		if constexpr (is_contiguous_range)
			return range_view_t{ printed, to_address(begin(r)), to_address(end(r)) };
		else
			return range_view_t{ printed, begin(r), end(r) };
	}
}

}
}
