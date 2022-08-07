#pragma once

namespace fast_io {	
template <::std::integral ch_type, ::fast_io::freestanding::input_or_output_iterator It>
struct range_view_t
{
    using char_type = ch_type;
    using iterator = It;
    ::fast_io::basic_io_scatter_t<ch_type> sep;
    It begin;
    It end;
};
template <::std::integral char_type, ::fast_io::freestanding::input_or_output_iterator I>
range_view_t(::fast_io::basic_io_scatter_t<char_type>, I, I)->range_view_t<char_type, I>;

template <::std::integral char_type, ::fast_io::freestanding::input_or_output_iterator It, output_stream output>
inline constexpr void print_define(io_reserve_type_t<char_type, range_view_t<char_type, It>>, output out, range_view_t<char_type, It> t)
{
    if (t.begin == t.end) return;
    auto cur{ t.begin };
    print_freestanding(out, *cur);
    for (++cur; cur != t.end; ++cur)
    {
        print_freestanding(out, t.sep);
        print_freestanding(out, *cur);
    }
}

namespace manipulators {
#if __STDC_HOSTED__==1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED==1) && __has_include(<ranges>)
template <::std::ranges::range rg, ::fast_io::constructible_to_os_c_str T>
    requires (!::std::ranges::contiguous_range<rg>)
inline constexpr auto rgvw(rg&& r, T&& sep)
{
    return ::fast_io::range_view_t{ print_alias_define(io_alias, sep), ::std::ranges::begin(r), ::std::ranges::end(r) };
}
template <::std::ranges::contiguous_range rg, ::fast_io::constructible_to_os_c_str T>
inline constexpr auto rgvw(rg&& r, T&& sep)
{
    return ::fast_io::range_view_t{ print_alias_define(io_alias, sep), ::std::to_address(::std::ranges::begin(r)), ::std::to_address(::std::ranges::end(r))};
}
template <::std::ranges::range rg, ::std::integral char_type>
    requires (!::std::ranges::contiguous_range<rg>)
inline constexpr auto rgvw(rg&& r, char_type const (&sep)[2])
{
    return ::fast_io::range_view_t{ ::fast_io::basic_io_scatter_t<char_type>{reinterpret_cast<char_type const*>(__builtin_addressof(sep)), 1}, ::std::ranges::begin(r), ::std::ranges::end(r) };
}
template <::std::ranges::contiguous_range rg, ::std::integral char_type>
inline constexpr auto rgvw(rg&& r, char_type const (&sep)[2])
{
    return ::fast_io::range_view_t{ ::fast_io::basic_io_scatter_t<char_type>{reinterpret_cast<char_type const*>(__builtin_addressof(sep)), 1}, ::std::to_address(::std::ranges::begin(r)), ::std::to_address(::std::ranges::end(r))};
}
#endif
}

}
