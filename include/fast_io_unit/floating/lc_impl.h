#pragma once

#include "../../fast_io_dsal/impl/misc/push_macros.h"

namespace fast_io
{

namespace details
{

// ---------------------------------------------------------------------------
// locale float rewrite — the standard scalar/precision format is emitted
// into scratch, then the text is walked once:
//   [-]int[.frac][eEpP ±exp]   or   inf / nan / 0x…p…
//   '.' becomes numeric.decimal_point units; the trailing decimal-digit
//   run before '.' (the integer part) gets the numeric grouping +
//   thousands_sep when the format is decimal
// ---------------------------------------------------------------------------

// emit digit characters df..dl with grouping separators, or raw when
// the locale has none — same ascending-cum bound math as lc_grouped_write
template <::std::integral char_type, typename sink>
inline constexpr void lc_put_grouped_digits(lc_ctx<char_type> const *ctx,
											char_type const *df,
											char_type const *dl, sink &sk) noexcept
{
	auto const nd{static_cast<::std::size_t>(dl - df)};
	if (ctx->grouping_len == 0 || ctx->sep_len == 0 || nd == 0)
	{
		for (auto p{df}; p != dl; ++p)
		{
			sk.put(*p);
		}
		return;
	}
	::std::size_t bounds[32];
	::std::size_t nb{};
	{
		::std::uint_least8_t prev{};
		::std::size_t cum{};
		for (::std::size_t gi{}; nb < 32; ++gi)
		{
			auto const g{lc_group_size(ctx, gi, prev)};
			if (g == 0)
			{
				break;
			}
			prev = g;
			if (cum + g >= nd)
			{
				break;
			}
			cum += g;
			bounds[nb++] = cum;
		}
	}
	::std::size_t bi{};
	for (::std::size_t i{}; i < nd; ++i)
	{
		sk.put(df[i]);
		if (bi < nb && nd - i - 1 == bounds[nb - 1 - bi])
		{
			sk.put_units(ctx->sep, ctx->sep_len);
			++bi;
		}
	}
}

// rewrite one formatted float range — group=true applies the numeric
// grouping to the decimal-digit run before '.'
template <bool group, ::std::integral char_type, typename sink>
inline constexpr void lc_float_rewrite(lc_ctx<char_type> const *ctx,
									   char_type const *f,
									   char_type const *l, sink &sk)
	FAST_IO_HERBCEPTIONS_THROWS
{
	auto const dp{ctx->sc(ctx->all->numeric.decimal_point)};
	// split at '.' or the exponent marker, whichever comes first
	char_type const *dot{l}, *ex{l};
	for (auto p{f}; p != l; ++p)
	{
		auto const ch{*p};
		if (ch == ::fast_io::char_literal_v<u8'.', char_type>)
		{
			dot = p;
			break;
		}
		if (ch == ::fast_io::char_literal_v<u8'e', char_type> ||
			ch == ::fast_io::char_literal_v<u8'E', char_type> ||
			ch == ::fast_io::char_literal_v<u8'p', char_type> ||
			ch == ::fast_io::char_literal_v<u8'P', char_type>)
		{
			ex = p;
			break;
		}
	}
	if (dot == l)
	{
		dot = ex; // no dot — the tail starts at the exponent
	}
	// trailing decimal-digit run of [f, dot) — the groupable integer
	// digits; "0x1", "inf", "nan" leave no suffix digits
	auto de{dot};
	while (de != f && ::fast_io::char_category::is_c_digit(de[-1]))
	{
		--de;
	}
	// the integer region minus its digit suffix (sign / 0x / inf-nan
	// letters) is raw
	for (auto p{f}; p != de; ++p)
	{
		sk.put(*p);
	}
	if constexpr (group)
	{
		lc_put_grouped_digits<char_type>(ctx, de, dot, sk);
	}
	else
	{
		for (auto p{de}; p != dot; ++p)
		{
			sk.put(*p);
		}
	}
	if (dot != l)
	{
		// '.' is replaced by the locale decimal_point units
		if (dp.base != nullptr)
		{
			sk.put_units(dp.base, dp.len);
		}
		else
		{
			sk.put(*dot);
		}
		for (auto p{dot + 1}; p != l; ++p)
		{
			sk.put(*p);
		}
	}
	else
	{
		for (auto p{ex}; p != l; ++p)
		{
			sk.put(*p);
		}
	}
}

// emit one formatted float arg — format into scratch (constexpr bound
// for scalar, dynamic for precision), then rewrite through the sink
template <bool group, ::std::integral char_type, typename T, typename sink>
inline constexpr void lc_float_emit(lc_ctx<char_type> const *ctx, T const &t,
									sink &sk) FAST_IO_HERBCEPTIONS_THROWS
{
	if constexpr (requires { t.precision; })
	{
		::fast_io::details::local_operator_new_array_ptr<char_type> tmp(
			print_reserve_size(::fast_io::io_reserve_type<char_type, T>, t));
		auto const *l{print_reserve_define(::fast_io::io_reserve_type<char_type, T>,
										   tmp.ptr, t)};
		lc_float_rewrite<group>(ctx, tmp.ptr, l, sk);
	}
	else
	{
		constexpr ::std::size_t scratch{
			print_reserve_size(::fast_io::io_reserve_type<char_type, T>)};
		char_type tmp[scratch + 4];
		auto const *l{print_reserve_define(::fast_io::io_reserve_type<char_type, T>,
										   tmp, t)};
		lc_float_rewrite<group>(ctx, tmp, l, sk);
	}
}

template <::std::integral char_type, typename T>
inline constexpr ::std::size_t lc_float_size(lc_ctx<char_type> const *ctx,
											 T const &t, bool group)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if (ctx->all == nullptr)
	{
		return 0;
	}
	lc_count_sink<char_type> sk{};
	if (group)
	{
		lc_float_emit<true>(ctx, t, sk);
	}
	else
	{
		lc_float_emit<false>(ctx, t, sk);
	}
	return sk.n;
}

// a floating scalar — shortest or precision — base-10 only (hexfloat
// gets decimal_point but no hex grouping)
template <typename T>
inline constexpr bool lc_float_scalar_v{false};

template <::fast_io::manipulators::scalar_flags flags, typename T>
inline constexpr bool lc_float_scalar_v<::fast_io::manipulators::scalar_manip_t<flags, T>>{
	::fast_io::details::my_floating_point<::std::remove_cvref_t<T>> && flags.base == 10};

template <::fast_io::manipulators::scalar_flags flags, typename T>
inline constexpr bool lc_float_scalar_v<::fast_io::manipulators::scalar_manip_precision_t<flags, T>>{
	::fast_io::details::my_floating_point<::std::remove_cvref_t<T>> && flags.base == 10};

} // namespace details

// ---------------------------------------------------------------------------
// print hooks keyed on lc_ctx — count and write share lc_float_emit so
// they cannot disagree
// ---------------------------------------------------------------------------

template <::std::integral char_type, ::fast_io::manipulators::scalar_flags flags,
		  typename T>
	requires(::fast_io::details::lc_float_scalar_v<
			 ::fast_io::manipulators::scalar_manip_t<flags, T>>)
inline constexpr ::std::size_t
print_reserve_size(lc_ctx<char_type> const *ctx,
				   ::fast_io::manipulators::scalar_manip_t<flags, T> t)
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr bool group{
		flags.floating != ::fast_io::manipulators::floating_format::hexfloat};
	return ::fast_io::details::lc_float_size<char_type>(ctx, t, group);
}

template <::std::integral char_type, ::fast_io::manipulators::scalar_flags flags,
		  typename T>
	requires(::fast_io::details::lc_float_scalar_v<
			 ::fast_io::manipulators::scalar_manip_t<flags, T>>)
inline constexpr char_type *
print_reserve_define(lc_ctx<char_type> const *ctx, char_type *iter,
					 ::fast_io::manipulators::scalar_manip_t<flags, T> t)
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr bool group{
		flags.floating != ::fast_io::manipulators::floating_format::hexfloat};
	::fast_io::details::lc_write_sink<char_type> sk{
		iter, iter + ::fast_io::details::lc_float_size<char_type>(ctx, t, group)};
	::fast_io::details::lc_float_emit<group>(ctx, t, sk);
	return sk.it;
}

template <::std::integral char_type, ::fast_io::manipulators::scalar_flags flags,
		  typename T>
	requires(::fast_io::details::lc_float_scalar_v<
			 ::fast_io::manipulators::scalar_manip_precision_t<flags, T>>)
inline constexpr ::std::size_t
print_reserve_size(lc_ctx<char_type> const *ctx,
				   ::fast_io::manipulators::scalar_manip_precision_t<flags, T> t)
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr bool group{
		flags.floating != ::fast_io::manipulators::floating_format::hexfloat};
	return ::fast_io::details::lc_float_size<char_type>(ctx, t, group);
}

template <::std::integral char_type, ::fast_io::manipulators::scalar_flags flags,
		  typename T>
	requires(::fast_io::details::lc_float_scalar_v<
			 ::fast_io::manipulators::scalar_manip_precision_t<flags, T>>)
inline constexpr char_type *
print_reserve_define(lc_ctx<char_type> const *ctx, char_type *iter,
					 ::fast_io::manipulators::scalar_manip_precision_t<flags, T> t)
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr bool group{
		flags.floating != ::fast_io::manipulators::floating_format::hexfloat};
	::fast_io::details::lc_write_sink<char_type> sk{
		iter, iter + ::fast_io::details::lc_float_size<char_type>(ctx, t, group)};
	::fast_io::details::lc_float_emit<group>(ctx, t, sk);
	return sk.it;
}

} // namespace fast_io

#include "../../fast_io_dsal/impl/misc/pop_macros.h"
