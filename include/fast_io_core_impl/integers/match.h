#pragma once

namespace fast_io
{

namespace manipulators
{
template<typename T>
struct basic_matcher_t
{
	using manip_tag = manip_tag_t;
	T reference;
};
}

struct match_context
{
	std::size_t matched{};
};

namespace details
{

template<::std::input_iterator Iter>
inline
#if __cpp_lib_is_constant_evaluated >= 201811
constexpr
#endif
parse_result<Iter> match_context_impl(std::size_t& matched,Iter first,Iter last,::std::iter_value_t<Iter> const* base,std::size_t len) noexcept
{
	auto base_ed{base+len-matched};
	auto [first_it,base_it]=::fast_io::freestanding::my_mismatch(first,last,base,base_ed);
	matched+=static_cast<std::size_t>(base_it-base);
	if(first_it==last)
	{
		if(base_it!=base_ed)
			return {first_it,parse_code::partial};
	}
	else if(base_it!=base_ed)
		return {first_it,parse_code::invalid};
	return {first_it,parse_code::ok};
}

}

template<std::integral char_type>
inline constexpr io_type_t<match_context> scan_context_type(io_reserve_type_t<char_type,manipulators::basic_matcher_t<basic_io_scatter_t<char_type>>>) noexcept
{
	return {};
}

template<::std::input_iterator Iter>
inline constexpr parse_result<Iter> scan_context_define(io_reserve_type_t<::std::iter_value_t<Iter>,manipulators::basic_matcher_t<basic_io_scatter_t<::std::iter_value_t<Iter>>>>,match_context& ctx,Iter first,Iter last,manipulators::basic_matcher_t<basic_io_scatter_t<::std::iter_value_t<Iter>>> t) noexcept
{
	return details::match_context_impl(ctx.matched,first,last,t.reference.base,t.reference.len);
}

template<std::integral char_type>
inline constexpr parse_code scan_context_eof_define(io_reserve_type_t<char_type,manipulators::basic_matcher_t<basic_io_scatter_t<char_type>>>,match_context,manipulators::basic_matcher_t<basic_io_scatter_t<char_type>>) noexcept
{
	return parse_code::end_of_file;
}
template<std::integral char_type,std::size_t n>
requires(n!=0)
inline constexpr manipulators::basic_matcher_t<basic_io_scatter_t<char_type>> scan_alias_define(io_alias_t,char_type const(&s)[n]) noexcept
{
	return {{s,n-1}};
}

#if 0
namespace manipulators
{
template<typename rg>
//requires (std::integral<std::ranges::range_value_t<rg>>)
inline constexpr manipulators::basic_matcher_t<basic_io_scatter_t<std::ranges::range_value_t<rg>>> mtvw(rg&& r) noexcept
{
	return {{std::ranges::data(r),std::ranges::size(r)}};
}
}
#endif

}
