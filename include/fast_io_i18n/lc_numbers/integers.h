#pragma once


namespace fast_io
{

namespace details
{

// how many separators does an ndigits-digit number get
template <::std::integral char_type>
inline ::std::size_t lc_grouped_count(lc_ctx<char_type> const *ctx,
									  ::std::size_t ndigits) noexcept
{
	::std::size_t seps{};
	::std::uint_least8_t prev{};
	for (::std::size_t rem{ndigits}, gi{}; rem != 0; ++gi)
	{
		auto const g{lc_group_size(ctx, gi, prev)};
		if (g == 0)
		{
			break;
		}
		prev = g;
		if (rem <= g)
		{
			break;
		}
		rem -= g;
		++seps;
	}
	return seps;
}

// digits written most-significant-first with the separator inserted at
// each group boundary (group sizes count from the right). u is the
// magnitude — the sign and base prefix, if any, are emitted by the
// caller. full pads with leading zeros to the type's maximum digit
// count in the base. Grouping applies to every base — the library has
// grouped non-decimal output for years.
template <::std::size_t base, bool upper, bool full, ::std::integral char_type,
		  ::fast_io::details::my_unsigned_integral T>
inline char_type *lc_grouped_write(lc_ctx<char_type> const *ctx,
								   char_type *first, T u) noexcept
{
	using int_type = ::std::remove_cv_t<T>;
	int_type tmp[::fast_io::details::cal_max_int_size<int_type, base>()];
	int_type *tmpend{tmp};
	do
	{
		*tmpend++ = static_cast<int_type>(u % base);
		u /= base;
	} while (u != 0);
	::std::size_t nd{static_cast<::std::size_t>(tmpend - tmp)};
	if constexpr (full)
	{
		nd = sizeof(tmp) / sizeof(int_type);
		// leading zeros fill the remaining most-significant digits
		for (auto p{tmpend}; p != tmp + nd; ++p)
		{
			*p = int_type{};
		}
		tmpend = tmp + nd;
	}
	// group boundaries: a separator goes between the digits when the
	// remaining count equals a cumulative group sum counted from the
	// right — for 10 digits with grouping 3 the bounds are 9,6,3
	// ("1|234|567|890"), descending
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
				break; // the most-significant group needs no leading sep
			}
			cum += g;
			bounds[nb++] = cum;
		}
	}
	::std::size_t bi{};
	for (::std::size_t pos{nd}; pos != 0; --pos)
	{
		*first++ = ::fast_io::details::charliteralofnumber<char_type, upper>(tmp[pos - 1]);
		if (bi < nb && pos - 1 == bounds[nb - 1 - bi])
		{
			::fast_io::details::my_memcpy(first, ctx->sep,
										  ctx->sep_len * sizeof(char_type));
			first += ctx->sep_len;
			++bi;
		}
	}
	return first;
}

// full emit: sign for decimal only (non-decimal bases write the
// unsigned representation, like print_reserve on scalar_manip), the
// showbase prefix, then grouped or plain digits in the base
template <::fast_io::manipulators::scalar_flags flags, ::std::integral char_type,
		  ::fast_io::details::my_integral T>
inline char_type *lc_print_scalar(lc_ctx<char_type> const *ctx,
								  char_type *first, T t) noexcept
{
	using int_type = ::std::remove_cv_t<T>;
	using unsigned_type = ::fast_io::details::my_make_unsigned_t<int_type>;
	unsigned_type u{static_cast<unsigned_type>(t)};
	if constexpr (flags.base == 10)
	{
		if constexpr (flags.showpos)
		{
			if constexpr (::fast_io::details::my_unsigned_integral<int_type>)
			{
				*first = ::fast_io::char_literal_v<u8'+', char_type>;
			}
			else
			{
				if (t < 0)
				{
					*first = ::fast_io::char_literal_v<u8'-', char_type>;
					constexpr unsigned_type zero{};
					u = static_cast<unsigned_type>(zero - u);
				}
				else
				{
					*first = ::fast_io::char_literal_v<u8'+', char_type>;
				}
			}
			++first;
		}
		else if constexpr (::fast_io::details::my_signed_integral<int_type>)
		{
			if (t < 0)
			{
				*first = ::fast_io::char_literal_v<u8'-', char_type>;
				++first;
				constexpr unsigned_type zero{};
				u = static_cast<unsigned_type>(zero - u);
			}
		}
	}
	if constexpr (flags.showbase)
	{
		first = ::fast_io::details::print_reserve_show_base_impl<flags.base,
															   flags.uppercase_showbase>(first);
	}
	if (ctx->grouping_len != 0 && ctx->sep_len != 0)
	{
		return lc_grouped_write<flags.base, flags.uppercase, flags.full>(ctx, first, u);
	}
	return ::fast_io::details::print_reserve_integral_withfull_main_impl<
		flags.full, flags.base, flags.uppercase>(first, u);
}

} // namespace details

// scalar_manip_t of an integral, non-alphabet type — locale grouping
// applies to every base (the library has grouped non-decimal output
// for years)
template <typename T>
inline constexpr bool lc_grouped_scalar_v{false};

template <::fast_io::manipulators::scalar_flags flags, typename T>
inline constexpr bool lc_grouped_scalar_v<::fast_io::manipulators::scalar_manip_t<flags, T>>{
	::fast_io::details::my_integral<T> && !flags.alphabet &&
	!::std::same_as<::std::remove_cv_t<T>, bool>};

// locale print hooks keyed on lc_ctx — the dynamic_reserve_printable
// analogue. print_reserve_size reports the exact char count,
// print_reserve_define writes it.
template <::std::integral char_type, ::fast_io::manipulators::scalar_flags flags, typename T>
	requires(::fast_io::lc_grouped_scalar_v<
			 ::fast_io::manipulators::scalar_manip_t<flags, T>>)
inline constexpr ::std::size_t
print_reserve_size(lc_ctx<char_type> const *ctx,
				   ::fast_io::manipulators::scalar_manip_t<flags, T> t) noexcept
{
	using unsigned_type = ::fast_io::details::my_make_unsigned_t<T>;
	unsigned_type u{static_cast<unsigned_type>(t.reference)};
	::std::size_t sign{};
	if constexpr (flags.base != 10)
	{
		/* non-decimal bases write the unsigned representation — no sign,
		 * and an unsigned magnitude is always used */
		if constexpr (::fast_io::details::my_signed_integral<T>)
		{
			u = static_cast<unsigned_type>(t.reference);
		}
	}
	else if constexpr (flags.showpos)
	{
		sign = 1;
		if constexpr (::fast_io::details::my_signed_integral<T>)
		{
			if (t.reference < 0)
			{
				constexpr unsigned_type zero{};
				u = static_cast<unsigned_type>(zero - u);
			}
		}
	}
	else if constexpr (::fast_io::details::my_signed_integral<T>)
	{
		if (t.reference < 0)
		{
			sign = 1;
			constexpr unsigned_type zero{};
			u = static_cast<unsigned_type>(zero - u);
		}
	}
	::std::size_t const nd{flags.full ? ::fast_io::details::cal_max_int_size<T, flags.base>()
									  : ::fast_io::details::chars_len<flags.base>(u)};
	if constexpr (flags.showbase)
	{
		sign += ::fast_io::details::base_prefix_array<char_type, flags.base>.size();
	}
	return sign + nd +
		   ::fast_io::details::lc_grouped_count(ctx, nd) * ctx->sep_len;
}

template <::std::integral char_type, ::fast_io::manipulators::scalar_flags flags, typename T>
	requires(::fast_io::lc_grouped_scalar_v<
			 ::fast_io::manipulators::scalar_manip_t<flags, T>>)
inline constexpr char_type *
print_reserve_define(lc_ctx<char_type> const *ctx, char_type *iter,
					 ::fast_io::manipulators::scalar_manip_t<flags, T> t) noexcept
{
	return ::fast_io::details::lc_print_scalar<flags>(ctx, iter, t.reference);
}

} // namespace fast_io

