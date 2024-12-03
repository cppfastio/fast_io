#pragma once

namespace fast_io
{

template <typename stm>
struct lc_imbuer
{
	using handle_type = stm;
	using char_type = typename handle_type::char_type;
	using output_char_type = char_type;
	using lc_type = ::fast_io::basic_lc_object<char_type>;
	lc_type const *locale{};
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
	[[no_unique_address]]
#endif
#endif
	handle_type handle{};
};

template <typename stm>
inline constexpr lc_imbuer<stm> output_stream_ref_define(lc_imbuer<stm> t) noexcept
{
	return t;
}

template <typename stm>
	requires(std::is_lvalue_reference_v<stm> || std::is_trivially_copyable_v<stm>)
inline constexpr auto imbue(::fast_io::basic_lc_object<typename std::remove_cvref_t<stm>::char_type> const &locale, stm &&out) noexcept
{
	return lc_imbuer{__builtin_addressof(locale), ::fast_io::operations::output_stream_ref(out)};
}

} // namespace fast_io
