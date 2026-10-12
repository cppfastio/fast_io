#pragma once
/*
Precision-based decimal output.

The exact significant digit values of the value come from
fp_decimal_to_digits in the fast_io.floating module; this header rounds
them on the requested decimal grid (round to nearest, ties to even) and
emits fixed, scientific, general or decimal spellings directly from the
digit array, so precision of any size works for every format.
*/
namespace fast_io::details
{

// round the exact expansion to keep significant digits, ties to even.
// digits holds n digit values (0-9), v = 0.digits * 10^e10.  keep may be
// <= 0, in which case the value rounds to zero or to the unit at the cut
// position.  Returns the new digit count (out may gain a leading 1 after
// carry; e10 is bumped accordingly).  Trailing zeros are NOT stripped.
inline ::std::size_t fp_round_significant(char *digits, ::std::size_t n, ::std::int_least32_t &e10,
										  ::std::int_least64_t keep) noexcept
{
	if (keep <= 0)
	{
		// grid unit sits at or above the leading digit: nonzero only when
		// the value exceeds the half unit (possible solely for keep == 0);
		// an exact 0.5 ties to even, which is zero
		if (keep == 0 && digits[0] >= 5)
		{
			bool up{digits[0] > 5};
			if (!up)
			{
				for (::std::size_t i{1u}; i != n; ++i)
				{
					if (digits[i])
					{
						up = true;
						break;
					}
				}
			}
			if (up)
			{
				digits[0] = 1;
				++e10;
				return 1u;
			}
		}
		return 0u;
	}
	auto const k{static_cast<::std::size_t>(keep)};
	if (n <= k)
	{
		return n;
	}
	bool up{digits[k] > 5};
	if (!up && digits[k] == 5)
	{
		for (::std::size_t i{k + 1u}; i != n; ++i)
		{
			if (digits[i])
			{
				up = true;
				break;
			}
		}
		if (!up)
		{
			// exact tie: round to even
			up = (digits[k - 1u] & 1) != 0;
		}
	}
	if (up)
	{
		::std::size_t i{k};
		while (i-- != 0u)
		{
			if (digits[i] != 9)
			{
				++digits[i];
				return k;
			}
			digits[i] = 0;
		}
		// all kept digits were 9: carry out into 1000..0
		digits[0] = 1;
		++e10;
	}
	return k;
}

template <bool comma, ::std::integral char_type>
inline constexpr char_type *fp_precision_fill_digits(char_type *iter, char const *digits,
													 ::std::size_t n, ::std::size_t want) noexcept
{
	for (::std::size_t i{}; i != want; ++i)
	{
		*iter = i < n ? ::fast_io::char_literal_add<char_type>(digits[i]) : char_literal_v<u8'0', char_type>;
		++iter;
	}
	return iter;
}

// fixed spelling with frac fractional digits: integer part, point, frac.
// digits/e10 describe the already-rounded value 0.digits*10^e10.
template <bool comma, ::std::integral char_type>
inline constexpr char_type *fp_precision_emit_fixed(char_type *iter, char const *digits,
													::std::size_t n, ::std::int_least32_t e10,
													::std::size_t frac) noexcept
{
	if (e10 <= 0)
	{
		*iter = char_literal_v<u8'0', char_type>;
		++iter;
		if (!frac)
		{
			return iter;
		}
		*iter = char_literal_v<(comma ? u8',' : u8'.'), char_type>;
		++iter;
		auto const zeros{static_cast<::std::size_t>(-e10)};
		if (zeros >= frac)
		{
			return fill_zeros_impl(iter, frac);
		}
		iter = fill_zeros_impl(iter, zeros);
		auto const rest{frac - zeros};
		return fp_precision_fill_digits<comma>(iter, digits, n, rest);
	}
	auto const int_digits{static_cast<::std::size_t>(e10)};
	for (::std::size_t i{}; i != int_digits; ++i)
	{
		*iter = i < n ? ::fast_io::char_literal_add<char_type>(digits[i]) : char_literal_v<u8'0', char_type>;
		++iter;
	}
	if (!frac)
	{
		return iter;
	}
	*iter = char_literal_v<(comma ? u8',' : u8'.'), char_type>;
	++iter;
	for (::std::size_t i{}; i != frac; ++i)
	{
		auto const pos{int_digits + i};
		*iter = pos < n ? ::fast_io::char_literal_add<char_type>(digits[pos]) : char_literal_v<u8'0', char_type>;
		++iter;
	}
	return iter;
}

// scientific spelling: leading digit, point, frac fractional digits, e±exp
template <bool comma, bool uppercase_e, typename flt, ::std::integral char_type>
inline constexpr char_type *fp_precision_emit_scientific(char_type *iter, char const *digits,
														 ::std::size_t n, ::std::int_least32_t e10,
														 ::std::size_t frac) noexcept
{
	*iter = n ? ::fast_io::char_literal_add<char_type>(digits[0]) : char_literal_v<u8'0', char_type>;
	++iter;
	if (frac)
	{
		*iter = char_literal_v<(comma ? u8',' : u8'.'), char_type>;
		++iter;
		for (::std::size_t i{}; i != frac; ++i)
		{
			auto const pos{i + 1u};
			*iter = pos < n ? ::fast_io::char_literal_add<char_type>(digits[pos]) : char_literal_v<u8'0', char_type>;
			++iter;
		}
	}
	return print_rsv_fp_e_impl<flt, uppercase_e>(iter, e10 - 1);
}

// fixed spelling without forced fractional digits (used by general and
// decimal decisions): emits the stripped coefficient with its real
// exponent, no padding
template <bool comma, ::std::integral char_type>
inline constexpr char_type *fp_precision_emit_plain_fixed(char_type *iter, char const *digits,
														  ::std::size_t n,
														  ::std::int_least32_t e10) noexcept
{
	auto const real_exp{e10 - 1};
	if (real_exp >= static_cast<::std::int_least32_t>(n) - 1)
	{
		// integer part covers every digit: digits then zeros
		iter = fp_precision_fill_digits<comma>(iter, digits, n, n);
		return fill_zeros_impl(iter, static_cast<::std::size_t>(real_exp - static_cast<::std::int_least32_t>(n) + 1));
	}
	if (real_exp >= 0)
	{
		// point inside the digit string
		auto const ipos{static_cast<::std::size_t>(real_exp + 1)};
		iter = fp_precision_fill_digits<comma>(iter, digits, n, ipos);
		*iter = char_literal_v<(comma ? u8',' : u8'.'), char_type>;
		++iter;
		return fp_precision_fill_digits<comma>(iter, digits + ipos, n - ipos, n - ipos);
	}
	// all fractional: 0.000ddd
	*iter = char_literal_v<u8'0', char_type>;
	++iter;
	*iter = char_literal_v<(comma ? u8',' : u8'.'), char_type>;
	++iter;
	iter = fill_zeros_impl(iter, static_cast<::std::size_t>(-real_exp - 1));
	return fp_precision_fill_digits<comma>(iter, digits, n, n);
}

template <bool comma, bool uppercase_e, typename flt, ::std::integral char_type>
inline constexpr char_type *fp_precision_emit_plain_scientific(char_type *iter, char const *digits,
															   ::std::size_t n,
															   ::std::int_least32_t e10) noexcept
{
	*iter = ::fast_io::char_literal_add<char_type>(digits[0]);
	++iter;
	if (n > 1u)
	{
		*iter = char_literal_v<(comma ? u8',' : u8'.'), char_type>;
		++iter;
		iter = fp_precision_fill_digits<comma>(iter, digits + 1u, n - 1u, n - 1u);
	}
	return print_rsv_fp_e_impl<flt, uppercase_e>(iter, e10 - 1);
}

// significant digits of 0.digits*10^e10 with trailing zeros stripped
inline ::std::size_t fp_strip_trailing_zeros(char const *digits, ::std::size_t n) noexcept
{
	while (n > 1u && !digits[n - 1u])
	{
		--n;
	}
	return n;
}

// decimal mode: shortest presentation picked between fixed and scientific
// (same rule as the round-trip emit)
template <bool comma, bool uppercase_e, typename flt, ::std::integral char_type>
inline constexpr char_type *fp_precision_emit_decimal(char_type *iter, char const *digits,
													  ::std::size_t n, ::std::int_least32_t e10) noexcept
{
	auto const real_exp{e10 - 1};
	::std::uint_least32_t fixed_length{};
	auto const ni{static_cast<::std::int_least32_t>(n)};
	if (ni <= real_exp)
	{
		fixed_length = static_cast<::std::uint_least32_t>(real_exp + 1);
	}
	else if (0 <= real_exp && real_exp < ni)
	{
		fixed_length = static_cast<::std::uint_least32_t>(ni + 2);
		if (ni == real_exp + 1)
		{
			--fixed_length;
		}
	}
	else
	{
		fixed_length = static_cast<::std::uint_least32_t>(static_cast<::std::uint_least32_t>(-real_exp) +
														  static_cast<::std::uint_least32_t>(ni) + 1u);
	}
	::std::uint_least32_t scientific_length{
		static_cast<::std::uint_least32_t>(ni == 1 ? ni + 3 : ni + 5)};
	if (scientific_length < fixed_length)
	{
		return fp_precision_emit_plain_scientific<comma, uppercase_e, flt>(iter, digits, n, e10);
	}
	return fp_precision_emit_plain_fixed<comma>(iter, digits, n, e10);
}

// general mode: fixed when -5 < e10 - n < 7, where e10 - n is the
// coefficient's decimal exponent (same rule as the round-trip emit)
template <bool comma, bool uppercase_e, typename flt, ::std::integral char_type>
inline constexpr char_type *fp_precision_emit_general(char_type *iter, char const *digits,
													  ::std::size_t n, ::std::int_least32_t e10) noexcept
{
	auto const coeff_exp{e10 - static_cast<::std::int_least32_t>(n)};
	if (-5 < coeff_exp && coeff_exp < 7)
	{
		return fp_precision_emit_plain_fixed<comma>(iter, digits, n, e10);
	}
	return fp_precision_emit_plain_scientific<comma, uppercase_e, flt>(iter, digits, n, e10);
}

template <bool showpos, bool uppercase, bool uppercase_e, bool comma,
		  ::fast_io::manipulators::floating_format mt, typename flt, ::std::integral char_type>
inline constexpr char_type *print_precision_flt_define_impl(char_type *iter, flt f,
															::std::size_t precision) noexcept
{
	using trait = iec559_traits<flt>;
	using mantissa_type = typename trait::mantissa_type;
	constexpr ::std::size_t mbits{trait::mbits};
	constexpr ::std::size_t ebits{trait::ebits};
	constexpr mantissa_type exponent_mask{(static_cast<mantissa_type>(1) << ebits) - 1};
	constexpr ::std::uint_least32_t exponent_mask_u32{static_cast<::std::uint_least32_t>(exponent_mask)};
	auto [mantissa, exponent, sign] = get_punned_result(f);
	iter = print_rsv_fp_sign_impl<showpos>(iter, sign);
	if (exponent == exponent_mask_u32)
	{
		return prsv_fp_nan_impl<uppercase>(iter, mantissa != 0u);
	}
	// zeros and requested fixed spellings
	if (!mantissa && !exponent)
	{
		if constexpr (mt == ::fast_io::manipulators::floating_format::scientific)
		{
			*iter = char_literal_v<u8'0', char_type>;
			++iter;
			if (precision)
			{
				*iter = char_literal_v<(comma ? u8',' : u8'.'), char_type>;
				++iter;
				iter = fill_zeros_impl(iter, precision);
			}
			return print_rsv_fp_e_impl<flt, uppercase_e>(iter, 0);
		}
		else if constexpr (mt == ::fast_io::manipulators::floating_format::fixed)
		{
			*iter = char_literal_v<u8'0', char_type>;
			++iter;
			if (precision)
			{
				*iter = char_literal_v<(comma ? u8',' : u8'.'), char_type>;
				++iter;
				iter = fill_zeros_impl(iter, precision);
			}
			return iter;
		}
		else
		{
			*iter = char_literal_v<u8'0', char_type>;
			++iter;
			return iter;
		}
	}
	// exact decimal expansion
	char digits[fp_digits_capacity];
	::std::uint_least64_t sig_lo, sig_hi{};
	auto const full_sig{exponent ? (mantissa | (static_cast<mantissa_type>(1) << mbits))
								 : mantissa};
	if constexpr (sizeof(mantissa_type) > sizeof(::std::uint_least64_t))
	{
		sig_lo = static_cast<::std::uint_least64_t>(full_sig);
		sig_hi = static_cast<::std::uint_least64_t>(full_sig >> 64u);
	}
	else
	{
		sig_lo = static_cast<::std::uint_least64_t>(full_sig);
	}
	constexpr ::std::int_least32_t bias{(1 << (ebits - 1u)) - 1};
	auto const e2{(exponent ? static_cast<::std::int_least32_t>(exponent) - bias : 1 - bias) -
				  static_cast<::std::int_least32_t>(mbits)};
	auto const dr{fp_decimal_to_digits(sig_lo, sig_hi, e2, digits)};
	auto e10{dr.e10};
	if constexpr (mt == ::fast_io::manipulators::floating_format::scientific)
	{
		auto n{fp_round_significant(digits, static_cast<::std::size_t>(dr.n), e10,
									static_cast<::std::int_least64_t>(precision) + 1)};
		if (!n)
		{
			digits[0] = 0;
			n = 1u;
		}
		return fp_precision_emit_scientific<comma, uppercase_e, flt>(iter, digits, n, e10, precision);
	}
	else if constexpr (mt == ::fast_io::manipulators::floating_format::fixed)
	{
		auto const keep{static_cast<::std::int_least64_t>(e10) +
						static_cast<::std::int_least64_t>(precision)};
		auto n{fp_round_significant(digits, static_cast<::std::size_t>(dr.n), e10, keep)};
		if (!n)
		{
			// rounds to zero
			return fp_precision_emit_fixed<comma>(iter, digits, 0u, e10, precision);
		}
		return fp_precision_emit_fixed<comma>(iter, digits, n, e10, precision);
	}
	else
	{
		// general / decimal: n significant digits, then strip trailing zeros
		auto n{fp_round_significant(digits, static_cast<::std::size_t>(dr.n), e10,
									static_cast<::std::int_least64_t>(precision))};
		if (!n)
		{
			*iter = char_literal_v<u8'0', char_type>;
			++iter;
			return iter;
		}
		n = fp_strip_trailing_zeros(digits, n);
		if constexpr (mt == ::fast_io::manipulators::floating_format::general)
		{
			return fp_precision_emit_general<comma, uppercase_e, flt>(iter, digits, n, e10);
		}
		else
		{
			return fp_precision_emit_decimal<comma, uppercase_e, flt>(iter, digits, n, e10);
		}
	}
}

template <typename flt, ::fast_io::manipulators::floating_format mt>
inline constexpr ::std::size_t print_precision_flt_size_impl() noexcept
{
	using trait = iec559_traits<flt>;
	if constexpr (mt == ::fast_io::manipulators::floating_format::fixed)
	{
		// sign + full integer part + point + fractional capacity
		return 1u + static_cast<::std::size_t>(trait::e10max) + 2u +
			   fp_digits_capacity;
	}
	// scientific/general/decimal worst: sign + digits + point + e + sign + exp
	return 1u + fp_digits_capacity + 1u + 2u + trait::e10digits;
}

template <typename flt, ::fast_io::manipulators::floating_format mt>
inline constexpr ::std::size_t print_precision_flt_cache{print_precision_flt_size_impl<flt, mt>()};


// ---------------------------------------------------------------------------
// hexfloat precision: n fractional hexadecimal digits, round to nearest
// (ties to even) on the nibble grid
// ---------------------------------------------------------------------------

template <bool showbase, bool showbase_uppercase, bool showpos, bool uppercase, bool uppercase_e,
		  bool comma, typename flt, ::std::integral char_type>
inline constexpr char_type *print_precision_hexfloat_define_impl(char_type *iter, flt f,
																 ::std::size_t n) noexcept
{
	using trait = iec559_traits<flt>;
	using mantissa_type = typename trait::mantissa_type;
	constexpr ::std::size_t mbits{trait::mbits};
	constexpr ::std::size_t ebits{trait::ebits};
	constexpr ::std::int_least32_t bias{static_cast<::std::int_least32_t>((1u << (ebits - 1u)) - 1u)};
	constexpr mantissa_type exponent_mask{(static_cast<mantissa_type>(1) << ebits) - 1};
	constexpr ::std::uint_least32_t exponent_mask_u32{static_cast<::std::uint_least32_t>(exponent_mask)};
	constexpr ::std::uint_least32_t makeup_bits{static_cast<::std::uint_least32_t>(
		((mbits / 4 + 1) * 4 - mbits) % 4)};
	constexpr ::std::uint_least32_t aligned_bits{
		static_cast<::std::uint_least32_t>(mbits) + makeup_bits};
	constexpr ::std::size_t total_nibbles{aligned_bits / 4u};
	auto [mantissa, exponent, sign] = get_punned_result(f);
	iter = print_rsv_fp_sign_impl<showpos>(iter, sign);
	if (exponent == exponent_mask_u32)
	{
		return prsv_fp_nan_impl<uppercase>(iter, mantissa != 0u);
	}
	if constexpr (showbase)
	{
		iter = print_reserve_show_base_impl<16, showbase_uppercase>(iter);
	}
	if (!mantissa && !exponent)
	{
		*iter = char_literal_v<u8'0', char_type>;
		++iter;
		if (n)
		{
			*iter = char_literal_v<(comma ? u8',' : u8'.'), char_type>;
			++iter;
			iter = fill_zeros_impl(iter, n);
		}
		*iter = char_literal_v<(uppercase_e ? u8'P' : u8'p'), char_type>;
		++iter;
		return with_sign_prt_rsv_exponent_hex_impl<trait::e2hexdigits>(iter, 0);
	}
	bool leading_one{exponent != 0u};
	auto e2{leading_one ? static_cast<::std::int_least32_t>(exponent) - bias : 1 - bias};
	mantissa <<= makeup_bits;
	if (n < total_nibbles)
	{
		// round to the n-nibble fraction grid, ties to even
		auto const drop{aligned_bits - static_cast<::std::uint_least32_t>(n) * 4u};
		auto const half{static_cast<mantissa_type>(1) << (drop - 1u)};
		auto const rem{drop == aligned_bits
						   ? mantissa
						   : mantissa & ((static_cast<mantissa_type>(1) << drop) - 1u)};
		auto base{drop == aligned_bits ? static_cast<mantissa_type>(0)
									   : mantissa >> drop};
		if (rem > half || (rem == half && (base & 1u) != 0u))
		{
			++base;
			if (base >> (n * 4u))
			{
				// carry out of the kept nibbles
				base = 0;
				if (leading_one)
				{
					++e2;
				}
				else
				{
					leading_one = true; // denormal rounds up to 1.000.. x 2^e_min
				}
			}
		}
		mantissa = base << drop;
	}
	*iter = leading_one ? char_literal_v<u8'1', char_type> : char_literal_v<u8'0', char_type>;
	++iter;
	if (n)
	{
		*iter = char_literal_v<(comma ? u8',' : u8'.'), char_type>;
		++iter;
		auto const emit_nibbles{n < total_nibbles ? n : total_nibbles};
		print_reserve_integral_main_impl<16, uppercase>(
			iter + emit_nibbles,
			static_cast<mantissa_type>(mantissa >> (aligned_bits - emit_nibbles * 4u)),
			emit_nibbles);
		iter += emit_nibbles;
		if (n > total_nibbles)
		{
			iter = fill_zeros_impl(iter, n - total_nibbles);
		}
	}
	*iter = char_literal_v<(uppercase_e ? u8'P' : u8'p'), char_type>;
	++iter;
	return with_sign_prt_rsv_exponent_hex_impl<trait::e2hexdigits>(iter, e2);
}

} // namespace fast_io::details
