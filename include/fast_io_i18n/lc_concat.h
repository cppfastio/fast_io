#pragma once


// locale-aware string concat — the lc_concat family. The string is a
// strlike output stream; locale-aware args go through their lc hooks,
// everything else through the plain print path (the same segmentation
// as imbued printing).
//
//   auto s{::fast_io::lc_concat(loc, "x ", 1234567, ' ', mnp::d_t_fmt(ts))};

namespace fast_io
{

template <bool line, ::std::integral char_type, typename T, typename... Args>
	requires ::fast_io::strlike<char_type, T>
inline constexpr T basic_lc_general_concat(
	::fast_io::l10n::lc_locale const *loc, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if constexpr (::fast_io::l10n::details::lc_any_arg_v<
					  char_type,
					  decltype(::fast_io::io_print_forward<char_type>(
						  ::fast_io::io_print_alias(args)))...>)
	{
		T str;
		::fast_io::lc_ctx<char_type> ctx{::fast_io::lc_load_ctx<char_type>(loc)};
		::fast_io::io_strlike_reference_wrapper<char_type, T> ref{
			__builtin_addressof(str)};
		::fast_io::l10n::details::lc_status_print_impl<line>(
			ctx, ref,
			::fast_io::io_print_forward<char_type>(
				::fast_io::io_print_alias(args))...);
		return str;
	}
	else
	{
		// no locale-aware arg — the locale plays no role
		return ::fast_io::basic_general_concat<line, char_type, T>(
			::fast_io::io_print_forward<char_type>(
				::fast_io::io_print_alias(args))...);
	}
}

template <bool line, ::std::integral char_type, typename T, typename... Args>
	requires ::fast_io::strlike<char_type, T>
inline constexpr T basic_lc_concat(
	::fast_io::l10n::lc_locale const *loc, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::basic_lc_general_concat<line, char_type, T>(
		loc, static_cast<Args &&>(args)...);
}

template <::std::integral char_type = char, typename... Args>
inline constexpr ::fast_io::basic_string<char_type> lc_concat(
	::fast_io::l10n::lc_locale const *loc, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::basic_lc_general_concat<false, char_type,
											  ::fast_io::basic_string<char_type>>(
		loc, static_cast<Args &&>(args)...);
}

template <::std::integral char_type = char, typename... Args>
inline constexpr ::fast_io::basic_string<char_type> lc_concatln(
	::fast_io::l10n::lc_locale const *loc, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::basic_lc_general_concat<true, char_type,
											  ::fast_io::basic_string<char_type>>(
		loc, static_cast<Args &&>(args)...);
}

} // namespace fast_io

