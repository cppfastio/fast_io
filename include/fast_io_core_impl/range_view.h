#pragma once

namespace fast_io {
template <::std::integral ch_type, ::std::input_iterator It>
struct sized_range_view_t
{
	using char_type = ch_type;
	using iterator = It;
	using value_type = typename ::std::iterator_traits<It>::value_type;
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
	using value_type = typename ::std::iterator_traits<It>::value_type;
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
			retval += print_reserve_size(io_reserve_type<char_type, typename T::value_type>, *curr_ptr);
		return retval;
	}
}
template <::std::integral char_type, ::std::input_iterator It>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type, sized_range_view_t<char_type, It>>, char_type* __restrict ptr, sized_range_view_t<char_type, It> t)
{
	if (t.size == 0) return ptr;
	using value_type = typename sized_range_view_t<char_type, It>::value_type;
	auto curr_ptr{ t.begin };
	ptr = print_reserve_define(io_reserve_type<char_type, value_type>, ptr, *curr_ptr);
	++curr_ptr;
	for (::std::size_t i{}; i != t.size - 1; ++i, ++curr_ptr)
	{
		::fast_io::freestanding::non_overlapped_copy_n(t.sep.base, t.sep.len, ptr);
		ptr += t.sep.len;
		ptr = print_reserve_define(io_reserve_type<char_type, value_type>, ptr, *curr_ptr);
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

namespace details {
template <typename char_type, typename rg>
concept dynamic_reserve_printable_range = ::std::integral<char_type> &&
	(::fast_io::reserve_printable<char_type, ::std::ranges::range_value_t<rg>> || ::fast_io::dynamic_reserve_printable<char_type, ::std::ranges::range_value_t<rg>>) &&
	(::std::ranges::contiguous_range<rg> || ::std::ranges::sized_range<rg>);
}

namespace manipulators {
template <typename rg, ::fast_io::constructible_to_os_c_str T>
	requires ::fast_io::details::dynamic_reserve_printable_range<typename decltype(print_alias_define(io_alias, ::std::remove_cvref_t<T>{}))::value_type, rg>
inline constexpr auto rgvw(rg&& r, T&& sep)
{
	if constexpr (::std::ranges::contiguous_range<rg> && ::std::ranges::sized_range<rg>)
		return ::fast_io::sized_range_view_t{ print_alias_define(io_alias, sep), ::std::to_address(::std::ranges::begin(r)), ::std::ranges::size(r) };
	else if constexpr (::std::ranges::contiguous_range<rg>)
		return ::fast_io::sized_range_view_t{ print_alias_define(io_alias, sep), ::std::to_address(::std::ranges::begin(r)), (::std::to_address(::std::ranges::end(r)) - ::std::to_address(::std::ranges::begin(r))) / sizeof(::std::ranges::range_value_t<rg>) };
	else
		return ::fast_io::sized_range_view_t{ print_alias_define(io_alias, sep), ::std::ranges::begin(r), ::std::ranges::size(r) };
}
template <typename rg, ::fast_io::constructible_to_os_c_str T>
	requires (!::fast_io::details::dynamic_reserve_printable_range<typename decltype(print_alias_define(io_alias, ::std::remove_cvref_t<T>{}))::value_type, rg>)
inline constexpr auto rgvw(rg&& r, T&& sep)
{
	if constexpr (::std::ranges::contiguous_range<rg>)
		return ::fast_io::range_view_t{ print_alias_define(io_alias, sep), ::std::to_address(::std::ranges::begin(r)), ::std::to_address(::std::ranges::end(r))};
	else
		return ::fast_io::range_view_t{ print_alias_define(io_alias, sep), ::std::ranges::begin(r), ::std::ranges::end(r) };
}
template <::std::integral char_type, typename rg>
	requires ::fast_io::details::dynamic_reserve_printable_range<char_type, rg>
inline constexpr auto rgvw(rg&& r, char_type const (&sep)[2])
{
	if constexpr (::std::ranges::contiguous_range<rg> && ::std::ranges::sized_range<rg>)
		return ::fast_io::sized_range_view_t{ ::fast_io::basic_io_scatter_t<char_type>{reinterpret_cast<char_type const*>(__builtin_addressof(sep)), 1}, ::std::to_address(::std::ranges::begin(r)), ::std::ranges::size(r)};
	else if constexpr (::std::ranges::contiguous_range<rg>)
		return ::fast_io::sized_range_view_t{ ::fast_io::basic_io_scatter_t<char_type>{reinterpret_cast<char_type const*>(__builtin_addressof(sep)), 1}, ::std::to_address(::std::ranges::begin(r)), (::std::to_address(::std::ranges::end(r)) - ::std::to_address(::std::ranges::begin(r))) / sizeof(::std::ranges::range_value_t<rg>) };
	else
		return ::fast_io::sized_range_view_t{ ::fast_io::basic_io_scatter_t<char_type>{reinterpret_cast<char_type const*>(__builtin_addressof(sep)), 1}, ::std::ranges::begin(r), ::std::ranges::size(r) };
}
template <::std::integral char_type, typename rg>
	requires (!::fast_io::details::dynamic_reserve_printable_range<char_type, rg>)
inline constexpr auto rgvw(rg&& r, char_type const (&sep)[2])
{
	if constexpr (::std::ranges::contiguous_range<rg>)
		return ::fast_io::range_view_t{ ::fast_io::basic_io_scatter_t<char_type>{reinterpret_cast<char_type const*>(__builtin_addressof(sep)), 1}, ::std::to_address(::std::ranges::begin(r)), ::std::to_address(::std::ranges::end(r)) };
	else
		return ::fast_io::range_view_t{ ::fast_io::basic_io_scatter_t<char_type>{reinterpret_cast<char_type const*>(__builtin_addressof(sep)), 1}, ::std::ranges::begin(r), ::std::ranges::end(r) };
}

}
}
