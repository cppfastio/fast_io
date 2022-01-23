#pragma once

namespace fast_io
{


/*
we do not accept char const* since we never know whether it has null terminator.
This avoids security vulneralbilities for
	char * str = 0;
	print(str);
Instead, we print out its pointer value

We extend print pointers to print contiguous_iterator. No we can write things like

std::vector<std::size_t> vec(100,2);
println("vec.begin():",vec.begin()," vec.end()",vec.end());
*/
namespace manipulators
{
template<typename T>
struct chvw_t
{
	using manip_tag = manip_tag_t;
	T reference;
};

template<std::integral T>
inline constexpr chvw_t<T> chvw(T ch) noexcept
{
	return {ch};
}

template<std::integral T>
[[deprecated("chvw for printing c style string is duplicated. Just use fast_io::mnp::os_c_str(char_type const*) instead for both filename and printing")]]
inline constexpr basic_io_scatter_t<T> chvw(T const* ch) noexcept
{
	return {ch,::fast_io::cstr_len(ch)};
}

template<std::integral char_type>
inline constexpr basic_io_scatter_t<char_type> print_alias_define(io_alias_t,basic_os_c_str<char_type> bas) noexcept
{
	auto ptr{bas.ptr};
	return {ptr,::fast_io::cstr_len(ptr)};
}

template<std::integral T>
[[deprecated("chvw for printing c style string is duplicated. Just use fast_io::mnp::os_c_str(char_type const*,std::size_t) instead for both filename and printing")]]
inline constexpr basic_io_scatter_t<T> chvw(T const* ch,std::size_t n) noexcept
{
	return {ch,::fast_io::cstr_nlen(ch,n)};
}

template<std::integral T>
inline constexpr basic_io_scatter_t<T> os_c_str(T const* ch,std::size_t n) noexcept
{
	return {ch,::fast_io::cstr_nlen(ch,n)};
}

template<std::integral T>
inline constexpr void os_c_str(decltype(nullptr),std::size_t) noexcept=delete;

template<::fast_io::freestanding::contiguous_iterator Iter>
requires ::std::integral<::fast_io::freestanding::iter_value_t<Iter>>
inline constexpr basic_io_scatter_t<::std::remove_cvref_t<::fast_io::freestanding::iter_value_t<Iter>>> strvw(Iter first,Iter last) noexcept
{
	return {::fast_io::freestanding::to_address(first),static_cast<std::size_t>(last-first)};
}
#if __STDC_HOSTED__==1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED==1) && __has_include(<ranges>)
template<::std::ranges::contiguous_range rg>
requires ::std::integral<::std::ranges::range_value_t<rg>>
inline constexpr basic_io_scatter_t<::std::remove_cvref_t<::std::ranges::range_value_t<rg>>> strvw(rg&& r) noexcept
{
	return {::std::ranges::data(r),::std::ranges::size(r)};
}

template<::std::ranges::contiguous_range rg>
requires ::std::integral<::std::ranges::range_value_t<rg>>
inline constexpr basic_io_scatter_t<::std::remove_cvref_t<::std::ranges::range_value_t<rg>>> chvw(rg&& r) noexcept
{
	return ::fast_io::mnp::chvw(::std::ranges::data(r),::std::ranges::size(r));
}
#endif
}

template<std::integral char_type>
inline constexpr basic_io_scatter_t<char_type> print_scatter_define(io_reserve_type_t<char_type,basic_io_scatter_t<char_type>>,basic_io_scatter_t<char_type> iosc) noexcept
{
	return iosc;
}

template<std::integral char_type,std::size_t n>
inline constexpr auto print_alias_define(io_alias_t,char_type const(&s)[n]) noexcept
{
	if constexpr(n==2)
		return manipulators::chvw_t<char_type>{*s};
	else
		return basic_io_scatter_t<char_type>{s,n-1};
}

#if __STDC_HOSTED__==1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED==1) && __has_include(<ranges>)
template<typename T>
requires (::std::ranges::contiguous_range<T>&&requires(T&& t)
{
	t.substr();
})
inline constexpr basic_io_scatter_t<::std::remove_cvref_t<::std::ranges::range_value_t<T>>> print_alias_define(io_alias_t,T&& svw) noexcept
{
	return {::std::ranges::data(svw),::std::ranges::size(svw)};
}
#endif

template<std::integral char_type,std::integral pchar_type>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,manipulators::chvw_t<pchar_type>>) noexcept
{
	return 1;
}

template<std::integral char_type,std::integral pchar_type,::fast_io::freestanding::contiguous_iterator caiter,typename T>
inline constexpr caiter print_reserve_define(io_reserve_type_t<char_type,manipulators::chvw_t<pchar_type>>,caiter iter,T ch) noexcept
{
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	*iter=static_cast<char_type>(static_cast<unsigned_char_type>(ch.reference));
	return ++iter;
}

}
