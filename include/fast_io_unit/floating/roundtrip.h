#pragma once
/*
Algorithm: shortest round-trip conversion (Schubfach-family interval method).

The conversion is compiled separately in the fast_io.floating module
(share/fast_io/fast_io_floating.cppm).  Link that object to enable binary32
and binary64 output; without it these calls fail to link, which disables
floating point entirely.
*/
namespace fast_io::details
{

template <typename mantissa_type>
struct m10_result
{
	mantissa_type m10;
	::std::int_least32_t e10;
};

// the decimal significand width each format needs: narrow formats produce
// at most 9 digits, binary64 at most 17, and the wide formats up to 37
template <typename flt>
struct fp_m10_type
{
	using type = ::std::conditional_t<(iec559_traits<flt>::mbits <= 23u), ::std::uint_least32_t,
#if defined(__SIZEOF_INT128__)
									  ::std::conditional_t<(iec559_traits<flt>::mbits <= 52u),
														   ::std::uint_least64_t, __uint128_t>
#else
									  ::std::uint_least64_t
#endif
									  >;
};

m10_result<::std::uint_least64_t>
to_decimal_binary64(::std::uint_least64_t m2, ::std::int_least32_t e2) noexcept;

m10_result<::std::uint_least32_t>
to_decimal_binary32(::std::uint_least32_t m2, ::std::int_least32_t e2) noexcept;

m10_result<::std::uint_least32_t>
to_decimal_binary16(::std::uint_least32_t m2, ::std::int_least32_t e2) noexcept;

m10_result<::std::uint_least32_t>
to_decimal_bfloat16(::std::uint_least32_t m2, ::std::int_least32_t e2) noexcept;

#if defined(__SIZEOF_INT128__)
m10_result<__uint128_t>
to_decimal_binary128(__uint128_t m2, ::std::int_least32_t e2) noexcept;

m10_result<__uint128_t>
to_decimal_binary80(::std::uint_least64_t m2, ::std::int_least32_t e2) noexcept;
#endif

/*
Exact decimal expansion and decimal scan, also compiled in the
fast_io.floating module.

fp_decimal_to_digits writes the exact significant decimal digit values
(0-9, not character codes) of m * 2^e2 (m nonzero, up to 113 bits) into
out and returns {n, e10} with v = 0.d1..dn * 10^e10.  fp_digits_capacity
bounds n for every supported format.

fp_scan_decimal rounds digits * 10^e10 to the format described by
p (total significand bits, implicit bit included) and ebits.  digits
holds digit values 0-9, not character codes.  The caller
retains at most the per-format midpoint capacity significant digits (at
most fp_scan_digits_cap for the widest format) and sets sticky when a
dropped nonzero digit exists; the retained count must cover the longest
midpoint expansion of the target format so the straddle comparison fits.
The result is the full significand with the implicit bit set for normals
(binary80 stores the explicit integer bit in lo); callers mask to the
stored width.  code is 1 for out-of-range results (infinity or a value
that rounded to zero).
*/
struct fp_digits_result
{
	::std::int_least32_t n;
	::std::int_least32_t e10;
};

inline constexpr ::std::size_t fp_digits_capacity{11566u};
inline constexpr ::std::size_t fp_scan_digits_cap{11534u};

fp_digits_result
fp_decimal_to_digits(::std::uint_least64_t m_lo, ::std::uint_least64_t m_hi,
					 ::std::int_least32_t e2, char *out) noexcept;

struct fp_scan_result
{
	::std::uint_least64_t lo;
	::std::uint_least64_t hi;
	::std::int_least32_t efield;
	::std::int_least32_t code;
};

fp_scan_result
fp_scan_decimal(char const *digits, ::std::size_t n_digits, ::std::int_least64_t e10,
				bool sticky, ::std::uint_least32_t p, ::std::uint_least32_t ebits) noexcept;

/*
Hexadecimal scan, same result contract as fp_scan_decimal.  nibs holds
the retained significant nibbles as values 0-15 and V = (S + tail) * 2^e2
with tail in [0,1) nonzero iff sticky.  The caller's per-format nibble
capacity keeps S inside 128 bits and every dropped nibble strictly below
the rounding bit, so sticky is a pure boolean here.
*/
fp_scan_result
fp_scan_hex(char const *nibs, ::std::size_t n, ::std::int_least64_t e2,
			bool sticky, ::std::uint_least32_t p, ::std::uint_least32_t ebits) noexcept;

template <typename flt>
inline m10_result<typename fp_m10_type<flt>::type>
to_decimal(typename iec559_traits<flt>::mantissa_type m2, ::std::int_least32_t e2) noexcept
{
	constexpr ::std::size_t mbits{iec559_traits<flt>::mbits};
	if constexpr (mbits == 52u)
	{
		return to_decimal_binary64(m2, e2);
	}
	else if constexpr (mbits == 23u)
	{
		return to_decimal_binary32(m2, e2);
	}
	else if constexpr (mbits == 10u)
	{
		return to_decimal_binary16(m2, e2);
	}
	else if constexpr (mbits == 7u)
	{
		return to_decimal_bfloat16(m2, e2);
	}
	else
	{
#if defined(__SIZEOF_INT128__)
		if constexpr (mbits == 112u)
		{
			return to_decimal_binary128(m2, e2);
		}
		else
		{
			static_assert(mbits == 63u, "unsupported floating-point format for shortest conversion");
			return to_decimal_binary80(m2, e2);
		}
#else
		static_assert(mbits <= 52u, "wide floating point requires 128-bit integer support");
#endif
	}
}



// dedicated decimal-significand writer for the floating emit: narrow
// coefficients go through jeaiii directly; wide ones split at 10^19 so all
// writes stay on the 64-bit jeaiii path
template <::std::integral char_type, my_unsigned_integral U>
inline constexpr char_type *fp_m10_len(char_type *iter, U m10, ::std::uint_least32_t len) noexcept
{
#if defined(__SIZEOF_INT128__)
	if constexpr (sizeof(U) > sizeof(::std::uint_least64_t))
	{
		constexpr __uint128_t pow19{__uint128_t{10000000000000000000u}};
		if (m10 < pow19)
		{
			::fast_io::details::jeaiii::jeaiii_main_len(iter, static_cast<::std::uint_least64_t>(m10), len);
		}
		else
		{
			auto const hi{static_cast<::std::uint_least64_t>(m10 / pow19)};
			auto const lo{static_cast<::std::uint_least64_t>(m10 % pow19)};
			auto const hi_len{len - 19u};
			::fast_io::details::jeaiii::jeaiii_main_len(iter, hi, hi_len);
			::fast_io::details::jeaiii::jeaiii_main_len(iter + hi_len, lo, 19u);
		}
		return iter + len;
	}
	else
#endif
	{
		::fast_io::details::jeaiii::jeaiii_main_len(iter, m10, len);
		return iter + len;
	}
}

template <bool comma, ::std::integral char_type, my_unsigned_integral U>
inline constexpr char_type *print_rsv_fp_decimal_scientific_common_impl(char_type *iter, U m10,
																		::std::uint_least32_t m10len) noexcept
{
	auto itp1{iter + 1};
	fp_m10_len(itp1, m10, m10len);
	*iter = *itp1;
	*itp1 = char_literal_v < comma ? u8',' : u8'.', char_type > ;
	return itp1 + m10len;
}

template <bool comma, ::std::integral char_type, my_unsigned_integral U>
inline constexpr char_type *print_rsv_fp_decimal_common_impl(char_type *iter, U m10,
															 ::std::uint_least32_t m10len) noexcept
{
	if (m10len == 1) [[unlikely]]
	{
		*iter = ::fast_io::char_literal_add<char_type>(m10);
		++iter;
		return iter;
	}
	else
	{
		return print_rsv_fp_decimal_scientific_common_impl<comma>(iter, m10, m10len);
	}
}

template <typename flt, bool uppercase_e, ::std::integral char_type>
inline constexpr char_type *print_rsv_fp_e_impl(char_type *iter, ::std::int_least32_t e10) noexcept
{
	*iter = char_literal_v < uppercase_e ? u8'E' : u8'e', char_type > ;
	++iter;
	::std::uint_least32_t ue10{static_cast<::std::uint_least32_t>(e10)};
	if (e10 < 0)
	{
		ue10 = 0u - ue10;
		*iter = char_literal_v<u8'-', char_type>;
	}
	else
	{
		*iter = char_literal_v<u8'+', char_type>;
	}
	++iter;
	return prt_rsv_exponent_impl<iec559_traits<flt>::e10digits, true>(iter, ue10);
}

template <::std::integral char_type>
inline constexpr char_type *fill_zeros_impl(char_type *iter, ::std::size_t n) noexcept
{
	for (::std::size_t i{}; i != n; ++i)
	{
		*iter = char_literal_v<u8'0', char_type>;
		++iter;
	}
	return iter;
}

template <bool comma, ::std::integral char_type>
inline constexpr char_type *fill_zero_point_impl(char_type *iter) noexcept
{
	if constexpr (comma)
	{
		if constexpr (::std::same_as<char_type, char>)
		{
			return copy_string_literal("0,", iter);
		}
		else if constexpr (::std::same_as<char_type, wchar_t>)
		{
			return copy_string_literal(L"0,", iter);
		}
		else if constexpr (::std::same_as<char_type, char16_t>)
		{
			return copy_string_literal(u"0,", iter);
		}
		else if constexpr (::std::same_as<char_type, char32_t>)
		{
			return copy_string_literal(U"0,", iter);
		}
		else
		{
			return copy_string_literal(u8"0,", iter);
		}
	}
	else
	{
		if constexpr (::std::same_as<char_type, char>)
		{
			return copy_string_literal("0.", iter);
		}
		else if constexpr (::std::same_as<char_type, wchar_t>)
		{
			return copy_string_literal(L"0.", iter);
		}
		else if constexpr (::std::same_as<char_type, char16_t>)
		{
			return copy_string_literal(u"0.", iter);
		}
		else if constexpr (::std::same_as<char_type, char32_t>)
		{
			return copy_string_literal(U"0.", iter);
		}
		else
		{
			return copy_string_literal(u8"0.", iter);
		}
	}
}

template <typename flt, ::std::integral char_type>
inline constexpr char_type *fixed_case0_full_integer(char_type *iter, typename fp_m10_type<flt>::type m10,
													 ::std::int_least32_t olength,
													 ::std::int_least32_t real_exp) noexcept
{
	fp_m10_len(iter, m10, static_cast<::std::uint_least32_t>(olength));
	iter += olength;
	return fill_zeros_impl(iter, static_cast<::std::uint_least32_t>(real_exp + 1 - olength));
}

template <typename flt, bool comma, ::std::integral char_type>
inline constexpr char_type *
fixed_case1_integer_and_point(char_type *iter, typename fp_m10_type<flt>::type m10,
							  ::std::int_least32_t olength, ::std::int_least32_t real_exp) noexcept
{
	auto eposition(real_exp + 1);
	if (olength == eposition)
	{
		fp_m10_len(iter, m10, static_cast<::std::uint_least32_t>(olength));
		iter += olength;
	}
	else
	{
		auto tmp{iter};
		fp_m10_len(iter + 1, m10, static_cast<::std::uint_least32_t>(olength));
		iter += olength + 1;
		my_copy_n(tmp + 1, static_cast<::std::uint_least32_t>(eposition), tmp);
		tmp[eposition] = char_literal_v<(comma ? u8',' : u8'.'), char_type>;
	}
	return iter;
}

template <typename flt, bool comma, ::std::integral char_type>
inline constexpr char_type *fixed_case2_all_point(char_type *iter, typename fp_m10_type<flt>::type m10,
												  ::std::int_least32_t olength, ::std::int_least32_t real_exp) noexcept
{
	iter = fill_zero_point_impl<comma>(iter);
	iter = fill_zeros_impl(iter, static_cast<::std::uint_least32_t>(-real_exp - 1));
	fp_m10_len(iter, m10, static_cast<::std::uint_least32_t>(olength));
	iter += olength;
	return iter;
}

template <typename flt, bool comma, ::std::integral char_type>
inline constexpr char_type *print_rsv_fp_fixed_decision_impl(char_type *iter,
															 typename fp_m10_type<flt>::type m10,
															 ::std::int_least32_t e10) noexcept
{
	::std::int_least32_t olength(static_cast<::std::int_least32_t>(chars_len<10, true>(m10)));
	::std::int_least32_t const real_exp(static_cast<::std::int_least32_t>(e10 + olength - 1));
	if (olength <= real_exp)
	{
		return fixed_case0_full_integer<flt>(iter, m10, olength, real_exp);
	}
	else if (0 <= real_exp && real_exp < olength)
	{
		return fixed_case1_integer_and_point<flt, comma>(iter, m10, olength, real_exp);
	}
	else
	{
		return fixed_case2_all_point<flt, comma>(iter, m10, olength, real_exp);
	}
}

template <typename flt, bool comma, bool uppercase_e, ::fast_io::manipulators::floating_format mt,
		  ::std::integral char_type>
inline constexpr char_type *print_rsv_fp_decision_impl(char_type *iter, typename fp_m10_type<flt>::type m10,
													   ::std::int_least32_t e10) noexcept
{
	if constexpr (mt == ::fast_io::manipulators::floating_format::general)
	{
		if (-5 < e10 && e10 < 7)
		{
			return print_rsv_fp_fixed_decision_impl<flt, comma>(iter, m10, e10);
		}
		return print_rsv_fp_decision_impl<flt, comma, uppercase_e,
										  ::fast_io::manipulators::floating_format::scientific>(iter, m10, e10);
	}
	else if constexpr (mt == ::fast_io::manipulators::floating_format::scientific)
	{
		if (m10 < 10u) [[unlikely]]
		{
			*iter = ::fast_io::char_literal_add<char_type>(m10);
			++iter;
		}
		else
		{
			auto iterp1{iter};
			++iterp1;
			auto new_iter{fp_m10_len(iterp1, m10,
									 static_cast<::std::uint_least32_t>(chars_len<10, true>(m10)))};
			e10 += static_cast<::std::int_least32_t>(static_cast<::std::uint_least32_t>(new_iter - iterp1) - 1u);
			*iter = *iterp1;
			*iterp1 = char_literal_v < comma ? u8',' : u8'.', char_type > ;
			iter = new_iter;
		}
		return print_rsv_fp_e_impl<flt, uppercase_e>(iter, e10);
	}
	else // decimal
	{
		::std::int_least32_t olength{static_cast<::std::int_least32_t>(chars_len<10, true>(m10))};
		::std::int_least32_t const real_exp{static_cast<::std::int_least32_t>(e10 + olength - 1)};
		::std::uint_least32_t fixed_length{}, this_case{};
		if (olength <= real_exp)
		{
			fixed_length = static_cast<::std::uint_least32_t>(real_exp + 1);
			this_case = 1;
		}
		else if (0 <= real_exp && real_exp < olength)
		{
			fixed_length = static_cast<::std::uint_least32_t>(olength + 2);
			if (olength == real_exp + 1)
			{
				--fixed_length;
			}
			this_case = 2;
		}
		else
		{
			fixed_length = static_cast<::std::uint_least32_t>(static_cast<::std::uint_least32_t>(-real_exp) +
															  static_cast<::std::uint_least32_t>(olength) + 1u);
		}
		::std::uint_least32_t scientific_length{
			static_cast<::std::uint_least32_t>(olength == 1 ? olength + 3 : olength + 5)};
		if (scientific_length < fixed_length)
		{
			// scientific decision
			iter = print_rsv_fp_decimal_common_impl<comma>(iter, m10, static_cast<::std::uint_least32_t>(olength));
			return print_rsv_fp_e_impl<flt, uppercase_e>(iter, real_exp);
		}
		// fixed decision
		switch (this_case)
		{
		case 1:
			return fixed_case0_full_integer<flt>(iter, m10, olength, real_exp);
		case 2:
		{
			return fixed_case1_integer_and_point<flt, comma>(iter, m10, olength, real_exp);
		}
		default:
		{
			return fixed_case2_all_point<flt, comma>(iter, m10, olength, real_exp);
		}
		}
	}
}

template <bool showpos, bool uppercase, bool uppercase_e, bool comma, ::fast_io::manipulators::floating_format mt,
		  typename flt, ::std::integral char_type>
inline constexpr char_type *print_rsvflt_define_impl(char_type *iter, flt f) noexcept
{
	if constexpr (::fast_io::manipulators::floating_format::fixed == mt && uppercase_e)
	{
		return print_rsvflt_define_impl<showpos, uppercase, false, comma, mt>(iter, f);
	}
	else
	{
		using trait = iec559_traits<flt>;
		using mantissa_type = typename trait::mantissa_type;
		constexpr ::std::size_t ebits{trait::ebits};
		constexpr mantissa_type exponent_mask{(static_cast<mantissa_type>(1) << ebits) - 1};
		constexpr ::std::uint_least32_t exponent_mask_u32{static_cast<::std::uint_least32_t>(exponent_mask)};
		auto [mantissa, exponent, sign] = get_punned_result(f);
		iter = print_rsv_fp_sign_impl<showpos>(iter, sign);
		if (exponent == exponent_mask_u32)
		{
			return prsv_fp_nan_impl<uppercase>(iter, mantissa != 0u);
		}
		if (!mantissa && !exponent)
		{
			if constexpr (mt != ::fast_io::manipulators::floating_format::scientific)
			{
				*iter = char_literal_v<u8'0', char_type>;
				++iter;
				return iter;
			}
			else
			{
				return prsv_fp_dece0<uppercase>(iter);
			}
		}
		auto [m10, e10] = to_decimal<flt>(mantissa, static_cast<::std::int_least32_t>(exponent));
		if constexpr (mt == ::fast_io::manipulators::floating_format::fixed)
		{
			return print_rsv_fp_fixed_decision_impl<flt, comma>(iter, m10, e10);
		}
		else
		{
			return print_rsv_fp_decision_impl<flt, comma, uppercase_e, mt>(iter, m10, e10);
		}
	}
}

template <typename flt, ::fast_io::manipulators::floating_format mf>
inline constexpr ::std::size_t print_rsvflt_size_impl() noexcept
{
	using trait = iec559_traits<flt>;
	if constexpr (mf == ::fast_io::manipulators::floating_format::fixed)
	{
		// general's max length is equal to scientific's max length
		//(+/-)(significants+sep)
		::std::size_t sum{1}; // sign(+/-)
		sum += 2;             // 0./,
		sum += trait::e10max;
		sum += trait::m10digits;
		return sum;
	}
	else
	{
		// decimal and general's max lengths are equal to scientific's max length
		//(+/-)(significants+sep)(E/e)(+/-)e
		::std::size_t sum{1}; // sign(+/-)
		sum += trait::m10digits;
		++sum;    //./,
		sum += 2; //(E/e)(+/-)
		sum += trait::e10digits;
		return sum;
	}
}

template <typename flt, ::fast_io::manipulators::floating_format mt>
inline constexpr ::std::size_t print_rsv_cache{print_rsvflt_size_impl<flt, mt>()};

} // namespace fast_io::details
