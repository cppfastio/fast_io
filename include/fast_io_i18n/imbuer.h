#pragma once

namespace fast_io
{

template<stream stm>
requires (value_based_stream<stm>&&requires(stm sm)
{
	{io_value_handle(sm)}->std::same_as<stm>;
}&&std::is_trivially_copyable_v<stm>)
struct lc_imbuer
{
	using handle_type = stm;
	using char_type = typename handle_type::char_type;
	using lc_all_type = basic_lc_all<char_type>;
	lc_all_type const* all{};
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
[[no_unique_address]]
#endif
#endif
	handle_type handle{};
};

template<stream stm>
inline constexpr lc_imbuer<stm> io_value_handle(lc_imbuer<stm> sm) noexcept
{
	return sm;
}

template<stream stm>
requires (std::is_lvalue_reference_v<stm>||std::is_trivially_copyable_v<stm>)
inline constexpr auto imbue(basic_lc_all<typename std::remove_cvref_t<stm>::char_type> const* all,stm&& out) noexcept
{
	if constexpr(value_based_stream<stm>)
		return lc_imbuer<decltype(io_value_handle(out))>{all,io_value_handle(out)};
	else
		return lc_imbuer<decltype(io_ref(out))>{all,io_ref(out)};
}

}
