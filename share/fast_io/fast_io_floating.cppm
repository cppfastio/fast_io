module;

#include <fast_io_freestanding.h>
#include <fast_io_unit/floating/punning.h>
#include <fast_io_unit/floating/hexfloat.h>
#include <fast_io_unit/floating/roundtrip.h>
#include <fast_io_unit/floating/precision.h>
#include <fast_io_unit/floating/scan.h>
#include <fast_io_unit/floating/impl.h>

/*
Shortest round-trip floating-point conversion, separately compiled.

Implements the nearest-even shortest-decimal conversion for binary16,
bfloat16, binary32, binary64, binary80 and binary128.  Narrow formats
(mantissa <= 24 bits) use B = 2^34 fixed point, binary64 uses B = 2^64, and
the wide formats compute the cached power of ten by square-and-multiply in
320-bit fixed point so that no large table is needed for them.
Round-to-nearest-ties-to-even is the only supported policy.

The 12 KiB conversion cache (2048 exponent-shift bytes + 618 normalized
128-bit power-of-ten significands) is embedded from da_cache.bin, which is
generated and verified by gen_da_cache.py.  Compilers without #embed support
cannot build this module; users who do not link it simply get no floating
point output.

The conversion functions are defined in the global module fragment so they
have ordinary external linkage: the declarations in
fast_io_unit/floating/roundtrip.h resolve against this object.
*/

#if !defined(__has_embed) || (__has_embed("da_cache.bin") != __STDC_EMBED_FOUND__)
#error "fast_io floating-point module requires a compiler with #embed support"
#endif

#if defined(__clang__) && __has_warning("-Wc23-extensions")
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc23-extensions"
#endif

namespace fast_io::details
{

// ---------------------------------------------------------------------------
// embedded cache layout (little-endian, produced by gen_da_cache.py):
//   [0, 2048)         one shift byte per raw binary64 exponent field,
//                     s(raw) = compute_exponent_shift(e, d+1) + extra_shift
//                     where e = raw - 1075 (raw 0 maps to e = -1074),
//                     d = floor(e * log10(2)), extra_shift = 6
//   [2048, 2048+9888) 618 x {hi, lo} u64 pairs: the normalized 128-bit
//                     significand of 10^q for q = index - 293, top bit set
// ---------------------------------------------------------------------------

inline constexpr unsigned char floating_decimal_cache[]{
#embed "da_cache.bin"
};

#if defined(__clang__) && __has_warning("-Wc23-extensions")
#pragma clang diagnostic pop
#endif

inline constexpr ::std::uint_least64_t floating_cache_u64(::std::size_t offset) noexcept
{
	auto const *p{floating_decimal_cache + offset};
	return ::std::uint_least64_t{p[0]} | (::std::uint_least64_t{p[1]} << 8u) |
		   (::std::uint_least64_t{p[2]} << 16u) | (::std::uint_least64_t{p[3]} << 24u) |
		   (::std::uint_least64_t{p[4]} << 32u) | (::std::uint_least64_t{p[5]} << 40u) |
		   (::std::uint_least64_t{p[6]} << 48u) | (::std::uint_least64_t{p[7]} << 56u);
}

struct floating_pow10
{
	::std::uint_least64_t hi;
	::std::uint_least64_t lo;
};

// normalized 128-bit lower-endpoint significand of 10^k, k in [-293, 324]
inline constexpr floating_pow10 floating_pow10_get(::std::int_least32_t k) noexcept
{
	auto const offset{2048u + (static_cast<::std::uint_least32_t>(k + 293) << 4u)};
	return {floating_cache_u64(offset), floating_cache_u64(offset + 8u)};
}

// high 64 bits of x*y + c, i.e. floor((x*y + c) / 2^64)
inline constexpr ::std::uint_least64_t floating_umul_add_hi64(::std::uint_least64_t x, ::std::uint_least64_t y,
															::std::uint_least64_t c) noexcept
{
	::std::uint_least64_t high;
	auto const low{::fast_io::intrinsics::umul(x, y, high)};
	auto const sum{low + c};
	return high + (sum < low);
}

struct floating_product
{
	::std::uint_least64_t hi;
	::std::uint_least64_t lo;
};

// bits [64, 191] of the 192-bit product x * (hi:lo), returned as {hi64, mid64}
inline constexpr floating_product floating_umul64x128_hi(::std::uint_least64_t x, ::std::uint_least64_t lo,
													   ::std::uint_least64_t hi) noexcept
{
	::std::uint_least64_t top;
	auto const mid0{::fast_io::intrinsics::umul(x, hi, top)};
	auto const mid{mid0 + ::fast_io::intrinsics::umulh(x, lo)};
	return {top + (mid < mid0), mid};
}

// ---------------------------------------------------------------------------
// conversion core.  A = I + F/B is the source scaled by 10^{-(d+1)} into
// cell units of the coarser of the two candidate grids.  With B = 2^64 the
// endpoint membership predicates are
//
//     F < H      <=>  I   in R(x)
//     B - F <= H <=>  I+1 in R(x)
//
// where H is the scaled cell half-width with +even closing both midpoint
// endpoints for an even significand.  When neither holds, the nearest
// fine-grid digit is round(10F/B), with the B/4 -> 2.5 -> even-2 exception.
// ---------------------------------------------------------------------------

// Shared tail: significand (with implicit bit for normals), binary exponent
// e = raw - offset, and the effective raw table index (irregular ignores it).
inline m10_result<::std::uint_least64_t>
floating_to_decimal_binary64_common(::std::uint_least64_t significand, ::std::int_least32_t binary_exponent,
									::std::uint_least32_t effective_raw_exponent, bool regular) noexcept
{
	::std::uint_least64_t integral;
	::std::uint_least32_t digit;
	::std::int_least32_t decimal_exponent;
	bool has_last_digit;
	if (regular) [[likely]]
	{
		// floor(e * log10(2)) == (e * 78913) >> 18 exhaustively on [-1074, 971]
		decimal_exponent = (binary_exponent * 78913) >> 18u;
		auto const shift{static_cast<::std::uint_least32_t>(floating_decimal_cache[effective_raw_exponent])};
		auto const power{floating_pow10_get(-decimal_exponent - 1)};
		auto const product{floating_umul64x128_hi(significand << shift, power.lo, power.hi)};
		integral = product.hi >> 6u;
		auto const fractional{(product.hi << 58u) | (product.lo >> 6u)};
		auto const half_ulp{(power.hi >> (7u - shift)) + (1u - (significand & 1u))};
		auto const round_up{static_cast<bool>(fractional + half_ulp < fractional)};
		auto const round_down{static_cast<bool>(half_ulp > fractional)};
		integral += round_up;
		digit = static_cast<::std::uint_least32_t>(
			floating_umul_add_hi64(fractional, 10u, (::std::uint_least64_t{1} << 63u) + 6u));
		if (fractional == (::std::uint_least64_t{1} << 62u))
		{
			// 10F/B = 2.5 is the only exact half where add-half selects the odd
			// upper digit; nearest-even requires 2.
			digit = 2u;
		}
		has_last_digit = !(round_up || round_down);
	}
	else
	{
		// exact power of two: asymmetric cell [x - 2^(e-2), x + 2^(e-1)], so
		// the decimal exponent uses floor(log10(3/4 * 2^e)) and the lower
		// radius is half the upper radius.
		decimal_exponent = (binary_exponent * 315653 - 131072) >> 20u;
		auto const shift{static_cast<::std::uint_least32_t>(
			binary_exponent + (((-decimal_exponent - 1) * 217707) >> 16) + 7)};
		auto const power{floating_pow10_get(-decimal_exponent - 1)};
		auto const product{floating_umul64x128_hi(significand << shift, power.lo, power.hi)};
		integral = product.hi >> 6u;
		auto const fractional{(product.hi << 58u) | (product.lo >> 6u)};
		auto const half_ulp{power.hi >> (7u - shift)};
		auto const round_up{static_cast<bool>(half_ulp > ~(::std::uint_least64_t{0}) - fractional)};
		// the power-of-two significand is even, so the closer lower boundary
		// is closed: membership is inclusive here (F <= H/2, not F < H/2)
		auto const round_down{static_cast<bool>((half_ulp >> 1u) >= fractional)};
		integral += round_up;
		digit = static_cast<::std::uint_least32_t>(
			floating_umul_add_hi64(fractional, 10u, (::std::uint_least64_t{1} << 63u) - 1u));
		auto const lower{static_cast<::std::uint_least32_t>(
			floating_umul_add_hi64(fractional - (half_ulp >> 1u), 10u, ~(::std::uint_least64_t{0})))};
		if (digit < lower)
		{
			// the nearest tenth lay below the asymmetric cell's closer lower
			// boundary; select its first member
			digit = lower;
		}
		has_last_digit = !(round_up || round_down);
	}
	::std::uint_least64_t m10;
	::std::int_least32_t e10;
	if (has_last_digit)
	{
		m10 = integral * 10u + digit;
		e10 = decimal_exponent;
	}
	else
	{
		m10 = integral;
		e10 = decimal_exponent + 1;
	}
	if (m10 % 10u == 0u) [[unlikely]]
	{
		auto const [v, n]{::fast_io::bitops::rtz_iec559(m10)};
		m10 = v;
		e10 += static_cast<::std::int_least32_t>(n);
	}
	return {m10, e10};
}

// (raw mantissa, raw exponent field) -> {m10, e10}, shortest round-trip
m10_result<::std::uint_least64_t>
to_decimal_binary64(::std::uint_least64_t m2, ::std::int_least32_t e2) noexcept
{
	if (e2 == 0)
	{
		// subnormal: m2 * 2^-1074 == m2 * 2^(1-1075); the regular cell holds
		// because the subnormal half-width never fills a cell
		return floating_to_decimal_binary64_common(m2, -1074, 1u, true);
	}
	if (m2 == 0)
	{
		return floating_to_decimal_binary64_common(::std::uint_least64_t{1} << 52u, e2 - 1075,
												   static_cast<::std::uint_least32_t>(e2), false);
	}
	return floating_to_decimal_binary64_common(m2 | (::std::uint_least64_t{1} << 52u), e2 - 1075,
											   static_cast<::std::uint_least32_t>(e2), true);
}

// ---------------------------------------------------------------------------
// narrow formats (mantissa <= 24 bits): B = 2^34, one 64x64->128 product.
// Template parameters are the explicit significand bits, the exponent field
// bits, and the offset into the shared binary64 shift table
// (1075 - bias - mantissa_bits).
// ---------------------------------------------------------------------------

template <::std::uint_least32_t mantissa_bits, ::std::uint_least32_t exponent_bits,
		  ::std::uint_least32_t shift_table_offset>
inline m10_result<::std::uint_least32_t>
floating_to_decimal_narrow_common(::std::uint_least32_t significand, ::std::int_least32_t binary_exponent,
								  ::std::uint_least32_t effective_raw_exponent, bool regular) noexcept
{
	::std::uint_least64_t integral;
	::std::uint_least32_t digit;
	::std::int_least32_t decimal_exponent;
	bool has_last_digit;
	if (regular) [[likely]]
	{
		decimal_exponent = (binary_exponent * 78913) >> 18u;
		// the shared binary64 shift table is indexed at raw + offset; +28 moves
		// the guard width from 6 to 34
		auto const shift{static_cast<::std::uint_least32_t>(
			floating_decimal_cache[effective_raw_exponent + shift_table_offset] + 28u)};
		auto const power_high{floating_pow10_get(-decimal_exponent - 1).hi};
		auto const product{::fast_io::intrinsics::umulh(
			power_high + 1u, static_cast<::std::uint_least64_t>(significand) << shift)};
		auto const fractional{product & ((::std::uint_least64_t{1} << 34u) - 1u)};
		auto const half_ulp{(power_high >> (65u - shift)) + (1u - (significand & 1u))};
		auto const round_up{static_cast<bool>((fractional + half_ulp) >> 34u)};
		auto const round_down{static_cast<bool>(half_ulp > fractional)};
		integral = (product >> 34u) + round_up;
		digit = static_cast<::std::uint_least32_t>(
			(fractional * 10u + (::std::uint_least64_t{1} << 33u)) >> 34u);
		if (fractional == (::std::uint_least64_t{1} << 32u))
		{
			digit = 2u;
		}
		has_last_digit = !(round_up || round_down);
	}
	else
	{
		// For narrow powers of two the closed lower cell boundary can itself
		// be a decimal member of R at a scale one coarser than the working
		// grid; the fixed-point predicates assume an open lower edge and miss
		// it.  This only occurs when 2^(mbits+2)-1 supplies a factor of five,
		// which holds for binary16 (4095) but not bfloat16 (511) or binary32.
		// Find the coarsest scale with a member inside [v - 2^(e-2),
		// v + 2^(e-1)] exactly and take the member nearest to v.
		if constexpr (((1u << (mantissa_bits + 2u)) - 1u) % 5u == 0u)
		{
			if (binary_exponent >= 3)
			{
				auto const lo{static_cast<::std::uint_least64_t>((1u << (mantissa_bits + 2u)) - 1u)
							  << (binary_exponent - 2)};
				auto const hi{static_cast<::std::uint_least64_t>((1u << (mantissa_bits + 1u)) + 1u)
							  << (binary_exponent - 1)};
				auto const value{::std::uint_least64_t{1}
								 << (mantissa_bits + static_cast<::std::uint_least32_t>(binary_exponent))};
				::std::uint_least64_t scale{1u};
				::std::int_least32_t scale_exp{};
				for (;;)
				{
					auto const next{scale * 10u};
					auto const candidate_lo{(lo + next - 1u) / next};
					auto const candidate_hi{hi / next};
					if (candidate_lo > candidate_hi)
					{
						break;
					}
					scale = next;
					++scale_exp;
				}
				if (scale != 1u)
				{
					auto const candidate_lo{(lo + scale - 1u) / scale};
					auto const candidate_hi{hi / scale};
					auto candidate{(value + (scale >> 1u)) / scale};
					candidate = candidate < candidate_lo ? candidate_lo
						: candidate > candidate_hi		   ? candidate_hi
														   : candidate;
					return {static_cast<::std::uint_least32_t>(candidate), scale_exp};
				}
			}
		}
		// narrow powers of two share the binary64 asymmetric path
		auto const r{floating_to_decimal_binary64_common(
			significand, binary_exponent, effective_raw_exponent, false)};
		return {static_cast<::std::uint_least32_t>(r.m10), r.e10};
	}
	::std::uint_least32_t m10;
	::std::int_least32_t e10;
	if (has_last_digit)
	{
		m10 = static_cast<::std::uint_least32_t>(integral * 10u + digit);
		e10 = decimal_exponent;
	}
	else
	{
		m10 = static_cast<::std::uint_least32_t>(integral);
		e10 = decimal_exponent + 1;
	}
	if (m10 % 10u == 0u) [[unlikely]]
	{
		auto const [v, n]{::fast_io::bitops::rtz_iec559(m10)};
		m10 = v;
		e10 += static_cast<::std::int_least32_t>(n);
	}
	return {m10, e10};
}

template <::std::uint_least32_t mantissa_bits, ::std::uint_least32_t exponent_bits,
		  ::std::uint_least32_t shift_table_offset>
inline m10_result<::std::uint_least32_t>
floating_to_decimal_narrow(::std::uint_least32_t m2, ::std::int_least32_t e2) noexcept
{
	constexpr ::std::int_least32_t exponent_offset{
		static_cast<::std::int_least32_t>((1u << (exponent_bits - 1u)) + mantissa_bits - 1u)};
	constexpr ::std::uint_least32_t implicit_bit{1u << mantissa_bits};
	if (e2 == 0)
	{
		return floating_to_decimal_narrow_common<mantissa_bits, exponent_bits, shift_table_offset>(
			m2, 1 - exponent_offset, 1u, true);
	}
	if (m2 == 0)
	{
		return floating_to_decimal_narrow_common<mantissa_bits, exponent_bits, shift_table_offset>(
			implicit_bit, static_cast<::std::int_least32_t>(e2) - exponent_offset,
			static_cast<::std::uint_least32_t>(e2), false);
	}
	return floating_to_decimal_narrow_common<mantissa_bits, exponent_bits, shift_table_offset>(
		m2 | implicit_bit, static_cast<::std::int_least32_t>(e2) - exponent_offset,
		static_cast<::std::uint_least32_t>(e2), true);
}

m10_result<::std::uint_least32_t>
to_decimal_binary32(::std::uint_least32_t m2, ::std::int_least32_t e2) noexcept
{
	return floating_to_decimal_narrow<23u, 8u, 925u>(m2, e2);
}

m10_result<::std::uint_least32_t>
to_decimal_binary16(::std::uint_least32_t m2, ::std::int_least32_t e2) noexcept
{
	return floating_to_decimal_narrow<10u, 5u, 1050u>(m2, e2);
}

m10_result<::std::uint_least32_t>
to_decimal_bfloat16(::std::uint_least32_t m2, ::std::int_least32_t e2) noexcept
{
	return floating_to_decimal_narrow<7u, 8u, 941u>(m2, e2);
}

#if defined(__SIZEOF_INT128__)

// ---------------------------------------------------------------------------
// wide formats (binary80, binary128): the cached power of ten is computed by
// square-and-multiply in 320-bit fixed point, so no table is needed.
// Multi-limb arithmetic uses the same add-with-carry style as curve25519.
// All limb arrays are little-endian uint_least64_t.
// ---------------------------------------------------------------------------

inline constexpr ::std::size_t floating_big_work_limbs{5u};  // 320 bits
inline constexpr ::std::size_t floating_big_pow10_limbs{4u}; // 256 bits
inline constexpr ::std::int_least32_t floating_big_work_bits{320};

// out[0..na+nb) = a[0..na) * b[0..nb), out must be zeroed by the caller
inline void floating_limb_mul(::std::uint_least64_t const *a, ::std::size_t na,
							  ::std::uint_least64_t const *b, ::std::size_t nb,
							  ::std::uint_least64_t *out) noexcept
{
	for (::std::size_t i{}; i != na; ++i)
	{
		::std::uint_least64_t carry{};
		for (::std::size_t j{}; j != nb; ++j)
		{
			::std::uint_least64_t high;
			auto const low{::fast_io::intrinsics::umul(a[i], b[j], high)};
			bool c1, c2;
			auto const sum{::fast_io::intrinsics::addc(out[i + j], low, false, c1)};
			out[i + j] = ::fast_io::intrinsics::addc(sum, carry, false, c2);
			// high <= 2^64 - 2 and c1 + c2 <= 2 can never wrap carry to zero:
			// high is maximal only when low == 1, and then c1 + c2 <= 1.
			carry = high + static_cast<::std::uint_least64_t>(c1) + static_cast<::std::uint_least64_t>(c2);
		}
		out[i + nb] = carry;
	}
}

// out[0..outn) = bits [shift, shift + 64*outn) of a[0..n)
inline void floating_limb_extract(::std::uint_least64_t const *a, ::std::size_t n,
								  ::std::uint_least32_t shift, ::std::uint_least64_t *out,
								  ::std::size_t outn) noexcept
{
	auto const word_shift{shift >> 6u};
	auto const bit_shift{shift & 63u};
	for (::std::size_t i{}; i != outn; ++i)
	{
		auto const low{word_shift + i < n ? a[word_shift + i] : ::std::uint_least64_t{}};
		auto const high{word_shift + i + 1u < n ? a[word_shift + i + 1u] : ::std::uint_least64_t{}};
		out[i] = bit_shift ? (low >> bit_shift | high << (64u - bit_shift)) : low;
	}
}

// r = a * b normalized so that the top bit of r[4] is set; returns the binary
// exponent shift applied to the product
inline ::std::int_least32_t floating_limb_fmul(::std::uint_least64_t const *a,
											   ::std::uint_least64_t const *b,
											   ::std::uint_least64_t *r) noexcept
{
	::std::uint_least64_t product[2u * floating_big_work_limbs]{};
	floating_limb_mul(a, floating_big_work_limbs, b, floating_big_work_limbs, product);
	// both operands are normalized, so the product MSB is bit 638 or 639
	auto const shift{floating_big_work_bits - 1 +
					 static_cast<::std::int_least32_t>(product[2u * floating_big_work_limbs - 1u] >> 63u)};
	floating_limb_extract(product, 2u * floating_big_work_limbs,
						  static_cast<::std::uint_least32_t>(shift), r, floating_big_work_limbs);
	return shift;
}

// r = base**n at 320-bit precision (a lower approximation of base**n);
// returns rexp with r * 2**rexp <= base**n.  base is clobbered.
inline ::std::int_least32_t floating_fpow(::std::uint_least64_t *base, ::std::int_least32_t base_exp,
										  ::std::uint_least32_t n, ::std::uint_least64_t *r) noexcept
{
	r[0] = r[1] = r[2] = r[3] = 0;
	r[4] = ::std::uint_least64_t{1} << 63u; // 1.0
	auto rexp{1 - floating_big_work_bits};
	while (n != 0u)
	{
		if ((n & 1u) != 0u)
		{
			rexp += base_exp + floating_limb_fmul(r, base, r);
		}
		n >>= 1u;
		if (n != 0u)
		{
			base_exp = 2 * base_exp + floating_limb_fmul(base, base, base);
		}
	}
	return rexp;
}

// m[0..4) = floor(10**x / 2**e) normalized to 256 bits (top bit set), for
// |x| <= 6000; returns e.
inline ::std::int_least32_t floating_pow10_big(::std::int_least32_t x, ::std::uint_least64_t *m) noexcept
{
	::std::uint_least64_t p[floating_big_work_limbs];
	::std::int_least32_t power_exponent;
	if (x >= 0)
	{
		::std::uint_least64_t five[floating_big_work_limbs]{};
		five[floating_big_work_limbs - 1u] = ::std::uint_least64_t{5} << 61u;
		power_exponent = floating_fpow(five, 3 - floating_big_work_bits,
									   static_cast<::std::uint_least32_t>(x), p);
	}
	else
	{
		::std::uint_least64_t inv5[floating_big_work_limbs];
		for (auto &limb : inv5)
		{
			limb = ::std::uint_least64_t{0xccccccccccccccccu}; // floor(2**322 / 5)
		}
		power_exponent = floating_fpow(inv5, -floating_big_work_bits - 2,
									   static_cast<::std::uint_least32_t>(-x), p);
	}
	// truncate p to 256 bits; the guard limbs absorb fpow rounding to an
	// exact floor
	for (::std::size_t k{}; k != floating_big_pow10_limbs; ++k)
	{
		m[k] = p[k + (floating_big_work_limbs - floating_big_pow10_limbs)];
	}
	return power_exponent + static_cast<::std::int_least32_t>(
								(floating_big_work_limbs - floating_big_pow10_limbs) * 64u) + x;
}

struct floating_big_result
{
	__uint128_t significand;
	::std::int_least32_t exponent;
};

// (full significand as two limbs, binary exponent, regular) -> {m10, e10}.
// significand is the complete significand: for normals it already includes
// the implicit/integer bit; the caller also tells whether the cell is the
// symmetric regular one or the asymmetric power-of-two one.
inline floating_big_result
floating_to_decimal_big(::std::uint_least64_t significand_low, ::std::uint_least64_t significand_high,
						::std::int_least32_t binary_exponent, bool regular) noexcept
{
	// dec_exp = floor(bin_exp * log10(2) [+ log10(3/4) at a power of two]);
	// scaling by 10^-dec_exp puts the ulp in [1, 10) so the shortest form is
	// the integral part or a neighbor.
	constexpr ::std::int_least64_t log10_2_sig{20201781};  // round(log10(2) * 2^26)
	constexpr ::std::int_least64_t log10_3_4_sig{8384497}; // round(-log10(3/4) * 2^26)
	auto const decimal_exponent{static_cast<::std::int_least32_t>(
		(static_cast<::std::int_least64_t>(binary_exponent) * log10_2_sig -
		 (regular ? ::std::int_least64_t{} : log10_3_4_sig)) >>
		26u)};

	::std::uint_least64_t power_sig[floating_big_pow10_limbs];
	auto const power_exponent{floating_pow10_big(-decimal_exponent, power_sig)};
	// value * 10^-dec_exp = significand * power_sig / 2^shift
	auto const shift{static_cast<::std::uint_least32_t>(-(binary_exponent + power_exponent))};

	::std::uint_least64_t const significand_limbs[2]{significand_low, significand_high};
	::std::uint_least64_t product[2u + floating_big_pow10_limbs]{};
	floating_limb_mul(significand_limbs, 2u, power_sig, floating_big_pow10_limbs, product);
	::std::uint_least64_t scaled[4];
	floating_limb_extract(product, 2u + floating_big_pow10_limbs, shift - 128u, scaled, 4u);
	auto const integral{(static_cast<__uint128_t>(scaled[3]) << 64u) | scaled[2]};
	auto const fractional{(static_cast<__uint128_t>(scaled[1]) << 64u) | scaled[0]};
	auto const last_digit{static_cast<::std::uint_least64_t>(integral % 10u)};

	// half_ulp = floor(power_sig / 2^(shift-123)): half a binary ulp in cell
	// units
	::std::uint_least64_t half_ulp_limbs[2];
	floating_limb_extract(power_sig, floating_big_pow10_limbs, shift - 123u, half_ulp_limbs, 2u);
	auto const half_ulp{(static_cast<__uint128_t>(half_ulp_limbs[1]) << 64u) | half_ulp_limbs[0]};

	auto const candidate{(static_cast<__uint128_t>(last_digit) << 124u) | (fractional >> 4u)};
	constexpr __uint128_t half{static_cast<__uint128_t>(1) << 127u};
	auto const even{static_cast<bool>((significand_low & 1u) == 0u)};

	// round_up keeps all digits and rounds the significand to nearest;
	// trim_down drops the last digit; trim_up drops it and carries to the
	// next ten.  Exact boundaries are detected by equality and broken toward
	// an even significand.
	bool round_up, trim_down;
	if (regular)
	{
		round_up = fractional >= half;
		if (fractional == half)
		{
			round_up = (static_cast<::std::uint_least64_t>(integral) & 1u) != 0u;
		}
		trim_down = candidate <= half_ulp;
		if (candidate == half_ulp)
		{
			// 124-bit tie: the low 64 bits break it, to even on exact match
			::std::uint_least64_t ulp_low;
			floating_limb_extract(power_sig, floating_big_pow10_limbs, shift - 127u, &ulp_low, 1u);
			auto const fraction_low{static_cast<::std::uint_least64_t>(fractional)};
			trim_down = fraction_low == ulp_low ? even : fraction_low < ulp_low;
		}
	}
	else
	{
		round_up = fractional > half;
		auto const quarter_ulp{half_ulp >> 1u};
		if ((fractional >> 4u) > quarter_ulp)
		{
			round_up = true;
		}
		trim_down = candidate <= quarter_ulp;
	}

	// trim_up iff value + half_ulp reaches the next ten, i.e. candidate +
	// half_ulp reaches ten; compared as candidate >= ten - half_ulp so the
	// sum cannot overflow 128 bits.  A boundary (gap in {0,1}) breaks to
	// even, guarded by decimal_exponent == 0 for the exact gap == 0 case.
	constexpr __uint128_t ten{static_cast<__uint128_t>(10) << 124u};
	auto trim_up{static_cast<bool>(candidate >= ten - half_ulp)};
	auto const gap{ten - half_ulp - candidate};
	if (gap <= 1u && (decimal_exponent == 0 || gap == 1u))
	{
		trim_up = even;
	}

	auto significand{trim_down || trim_up ? integral - last_digit +
											  static_cast<::std::uint_least64_t>(trim_up) * 10u
										: integral + round_up};
	auto exponent{decimal_exponent};
	if (significand % 10u == 0u) [[unlikely]]
	{
		do
		{
			significand /= 10u;
			++exponent;
		} while (significand % 10u == 0u);
	}
	return {significand, exponent};
}

// (raw mantissa, raw exponent field) -> {m10, e10} for IEEE binary128
m10_result<__uint128_t>
to_decimal_binary128(__uint128_t m2, ::std::int_least32_t e2) noexcept
{
	constexpr ::std::int_least32_t exponent_offset{16495};
	constexpr __uint128_t implicit_bit{static_cast<__uint128_t>(1) << 112u};
	floating_big_result result;
	if (e2 == 0)
	{
		result = floating_to_decimal_big(static_cast<::std::uint_least64_t>(m2),
										 static_cast<::std::uint_least64_t>(m2 >> 64u),
										 1 - exponent_offset, true);
	}
	else if (m2 == 0)
	{
		// implicit bit 2^112 is bit 48 of the high significand limb
		result = floating_to_decimal_big(0, ::std::uint_least64_t{1} << 48u, e2 - exponent_offset, false);
	}
	else
	{
		auto const significand{m2 | implicit_bit};
		result = floating_to_decimal_big(static_cast<::std::uint_least64_t>(significand),
										 static_cast<::std::uint_least64_t>(significand >> 64u),
										 e2 - exponent_offset, true);
	}
	return {result.significand, result.exponent};
}

// (raw mantissa without the explicit integer bit, raw exponent field) ->
// {m10, e10} for x87 binary80.  The caller masks the stored significand's
// integer bit off, matching the ieee559 decomposition convention.
m10_result<__uint128_t>
to_decimal_binary80(::std::uint_least64_t m2, ::std::int_least32_t e2) noexcept
{
	constexpr ::std::int_least32_t exponent_offset{16446};
	constexpr ::std::uint_least64_t explicit_bit{::std::uint_least64_t{1} << 63u};
	floating_big_result result;
	if (e2 == 0)
	{
		result = floating_to_decimal_big(m2, 0, 1 - exponent_offset, true);
	}
	else if (m2 == 0)
	{
		result = floating_to_decimal_big(explicit_bit, 0, e2 - exponent_offset, false);
	}
	else
	{
		result = floating_to_decimal_big(m2 | explicit_bit, 0, e2 - exponent_offset, true);
	}
	return {result.significand, result.exponent};
}

#endif

// ---------------------------------------------------------------------------
// arbitrary-precision integer used by the exact decimal expansion (precision
// output) and by the exact scan fallback.  Little-endian u64 limbs; the
// capacity covers binary80/binary128 worst cases: scanned coefficients and
// midpoint expansions are at most 11532 decimal digits (~38300 bits), powers
// of five reach 5^16495 (~38300 bits), and large positive exponents need
// 2^16384 times a 113-bit significand (~16500 bits).
// ---------------------------------------------------------------------------

inline constexpr ::std::size_t fp_bigint_limbs{704u};

struct fp_bigint
{
	::std::uint_least64_t limb[fp_bigint_limbs];
	::std::size_t size;
};

inline void fp_big_normalize(fp_bigint &v) noexcept
{
	while (v.size && !v.limb[v.size - 1u])
	{
		--v.size;
	}
}

inline void fp_big_set_u64(fp_bigint &v, ::std::uint_least64_t x) noexcept
{
	v.limb[0] = x;
	v.size = x != 0u;
}

inline void fp_big_set_u128(fp_bigint &v, ::std::uint_least64_t lo, ::std::uint_least64_t hi) noexcept
{
	v.limb[0] = lo;
	v.limb[1] = hi;
	v.size = hi ? 2u : (lo != 0u);
}

// v = v * m + a, limb-wise carry chain
inline void fp_big_mul_add_u64(fp_bigint &v, ::std::uint_least64_t m, ::std::uint_least64_t a) noexcept
{
	::std::uint_least64_t carry{a};
	for (::std::size_t i{}; i != v.size; ++i)
	{
		::std::uint_least64_t hi;
		auto const lo{::fast_io::intrinsics::umul(v.limb[i], m, hi)};
		auto const s{lo + carry};
		v.limb[i] = s;
		carry = hi + (s < lo);
	}
	if (carry)
	{
		v.limb[v.size++] = carry;
	}
}

inline void fp_big_add_u64(fp_bigint &v, ::std::uint_least64_t a) noexcept
{
	if (!v.size)
	{
		fp_big_set_u64(v, a);
		return;
	}
	auto s{v.limb[0] + a};
	bool carry{s < v.limb[0]};
	v.limb[0] = s;
	for (::std::size_t i{1u}; carry && i != v.size; ++i)
	{
		carry = !++v.limb[i];
	}
	if (carry)
	{
		v.limb[v.size++] = 1u;
	}
}

// v *= 5^k via 5^27 chunks
inline void fp_big_mul_pow5(fp_bigint &v, ::std::uint_least64_t k) noexcept
{
	constexpr ::std::uint_least64_t pow5_27{7450580596923828125u}; // 5^27
	for (; k >= 27u; k -= 27u)
	{
		fp_big_mul_add_u64(v, pow5_27, 0u);
	}
	if (k)
	{
		::std::uint_least64_t m{1u};
		for (::std::uint_least64_t i{}; i != k; ++i)
		{
			m *= 5u;
		}
		fp_big_mul_add_u64(v, m, 0u);
	}
}

// v <<= k, growing limbs as needed
inline void fp_big_shl(fp_bigint &v, ::std::size_t k) noexcept
{
	if (!v.size || !k)
	{
		return;
	}
	auto const limbs{k >> 6u};
	auto const bits{static_cast<::std::uint_least32_t>(k & 63u)};
	if (limbs)
	{
		for (::std::size_t i{v.size}; i-- != 0u;)
		{
			v.limb[i + limbs] = v.limb[i];
		}
		for (::std::size_t i{}; i != limbs; ++i)
		{
			v.limb[i] = 0u;
		}
		v.size += limbs;
	}
	if (bits)
	{
		::std::uint_least64_t carry{};
		for (::std::size_t i{}; i != v.size; ++i)
		{
			auto const cur{v.limb[i]};
			v.limb[i] = (cur << bits) | carry;
			carry = cur >> (64u - bits);
		}
		if (carry)
		{
			v.limb[v.size++] = carry;
		}
	}
}

inline ::std::size_t fp_big_bitlen(fp_bigint const &v) noexcept
{
	return v.size ? ((v.size - 1u) << 6u) +
						static_cast<::std::size_t>(64u - static_cast<unsigned>(
														::std::countl_zero(v.limb[v.size - 1u])))
					: 0u;
}

inline int fp_big_cmp(fp_bigint const &a, fp_bigint const &b) noexcept
{
	if (a.size != b.size)
	{
		return a.size < b.size ? -1 : 1;
	}
	for (::std::size_t i{a.size}; i-- != 0u;)
	{
		if (a.limb[i] != b.limb[i])
		{
			return a.limb[i] < b.limb[i] ? -1 : 1;
		}
	}
	return 0;
}

// a -= b, requires a >= b
inline void fp_big_sub(fp_bigint &a, fp_bigint const &b) noexcept
{
	::std::uint_least64_t borrow{};
	for (::std::size_t i{}; i != a.size; ++i)
	{
		auto const bi{i < b.size ? b.limb[i] : 0u};
		auto const t{a.limb[i] - bi};
		auto const s{t - borrow};
		borrow = (t > a.limb[i]) || (s > t);
		a.limb[i] = s;
	}
	fp_big_normalize(a);
}

inline bool fp_big_bit(fp_bigint const &v, ::std::size_t i) noexcept
{
	auto const l{i >> 6u};
	return l < v.size && ((v.limb[l] >> (i & 63u)) & 1u) != 0u;
}

// any set bit strictly below position i
inline bool fp_big_any_below(fp_bigint const &v, ::std::size_t i) noexcept
{
	auto const l{i >> 6u};
	for (::std::size_t k{}; k != l && k != v.size; ++k)
	{
		if (v.limb[k])
		{
			return true;
		}
	}
	return l < v.size && (v.limb[l] & ((::std::uint_least64_t{1} << (i & 63u)) - 1u)) != 0u;
}

// a ? b<<k without materializing the shifted operand
inline int fp_big_cmp_shifted(fp_bigint const &a, fp_bigint const &b, ::std::size_t k) noexcept
{
	auto const lb{fp_big_bitlen(b) + k};
	auto const la{fp_big_bitlen(a)};
	if (la != lb)
	{
		return la < lb ? -1 : 1;
	}
	for (::std::size_t i{la}; i-- != 0u;)
	{
		auto const idx{i >= k ? i - k : i};
		::std::uint_least64_t bv{};
		if (i >= k)
		{
			auto const l{idx >> 6u};
			bv = (b.limb[l] >> (idx & 63u));
			if ((idx & 63u) && l + 1u < b.size)
			{
				bv |= b.limb[l + 1u] << (64u - (idx & 63u));
			}
		}
		auto const l{i >> 6u};
		auto const av{l < a.size ? (a.limb[l] >> (i & 63u)) & 1u : 0u};
		if (av != (bv & 1u))
		{
			return av ? 1 : -1;
		}
	}
	return 0;
}

// a -= b<<k, requires a >= b<<k
inline void fp_big_sub_shifted(fp_bigint &a, fp_bigint const &b, ::std::size_t k) noexcept
{
	auto const limbs{k >> 6u};
	auto const bits{static_cast<::std::uint_least32_t>(k & 63u)};
	::std::uint_least64_t borrow{};
	for (::std::size_t i{}; i != a.size; ++i)
	{
		::std::uint_least64_t bv{};
		if (i >= limbs)
		{
			auto const l{i - limbs};
			bv = l < b.size ? b.limb[l] : 0u;
			if (bits)
			{
				bv = (bv << bits) | (l && l - 1u < b.size ? b.limb[l - 1u] >> (64u - bits) : 0u);
			}
		}
		auto const t{a.limb[i] - bv};
		auto const s{t - borrow};
		borrow = (t > a.limb[i]) || (s > t);
		a.limb[i] = s;
	}
	fp_big_normalize(a);
}

// v /= d (u64), returns remainder; used to emit decimal digit groups
inline ::std::uint_least64_t fp_big_divmod_u64(fp_bigint &v, ::std::uint_least64_t d) noexcept
{
	::std::uint_least64_t rem{};
	for (::std::size_t i{v.size}; i-- != 0u;)
	{
		auto const qr{::fast_io::intrinsics::udivmod(v.limb[i], rem, d, ::std::uint_least64_t{0})};
		v.limb[i] = qr.quotientlow;
		rem = qr.remainderlow;
	}
	fp_big_normalize(v);
	return rem;
}

// ---------------------------------------------------------------------------
// fp_decimal_to_digits: exact significant decimal digits of v = m * 2^e2.
// m is a nonzero significand of up to 113 bits passed as two words.
// Writes digit values 0-9 into out (capacity fp_digits_capacity) and
// returns {n, e10} with v = 0.d[0]d[1]...d[n-1] * 10^e10, d[0] != 0.
// fp_digits_result and fp_digits_capacity are declared in roundtrip.h.
// ---------------------------------------------------------------------------

fp_digits_result
fp_decimal_to_digits(::std::uint_least64_t m_lo, ::std::uint_least64_t m_hi, ::std::int_least32_t e2,
					 char *out) noexcept
{
	fp_bigint n;
	fp_big_set_u128(n, m_lo, m_hi);
	::std::int_least32_t e10{};
	if (e2 >= 0)
	{
		fp_big_shl(n, static_cast<::std::size_t>(e2));
	}
	else
	{
		fp_big_mul_pow5(n, static_cast<::std::uint_least64_t>(
							 -static_cast<::std::int_least64_t>(e2)));
		e10 = e2;
	}
	// digits of n, least significant first
	constexpr ::std::uint_least64_t group{10000000000000000000u};
	::std::size_t len{};
	while (n.size)
	{
		auto r{fp_big_divmod_u64(n, group)};
		for (::std::uint_least32_t i{}; i != 19u; ++i)
		{
			out[len++] = static_cast<char>(r % 10u);
			r /= 10u;
		}
	}
	// leading zeros only appear in the top group
	while (len > 1u && !out[len - 1u])
	{
		--len;
	}
	// reverse to most-significant-first
	for (::std::size_t i{}, j{len - 1u}; i < j; ++i, --j)
	{
		auto const t{out[i]};
		out[i] = out[j];
		out[j] = t;
	}
	return {static_cast<::std::int_least32_t>(len),
			e10 + static_cast<::std::int_least32_t>(len)};
}

// ---------------------------------------------------------------------------
// decimal scan:  D * 10^e10  ->  nearest-even IEEE bits.
//
// The parser keeps at most fp_scan_digits_cap significant input digits plus a
// sticky flag for the dropped tail.  For p <= 53 a 64x128 product against the
// cached power of ten resolves almost every input: the product's error is
// strictly below 2^64 in the remainder units, so a remainder within 2^64 of
// the rounding midpoint is the only ambiguity and falls through to the exact
// path.  The exact path rounds the full retained integer by limb arithmetic;
// it covers binary80, binary128, exponents outside the cache and inputs of
// more than 19 digits.
//
// A truncated (sticky) input is decided by rounding both interval endpoints;
// they are at most one ulp apart because the dropped unit sits at least ~750
// decimal orders below the leading digit.  When the endpoints disagree the
// input digits are compared against the binary midpoint's own decimal
// expansion, which never exceeds fp_scan_digits_cap digits.
// ---------------------------------------------------------------------------

// fp_scan_result and fp_scan_digits_cap are declared in roundtrip.h:
// lo/hi carry the full significand (implicit bit set for normals, binary80
// stores the explicit integer bit), efield the encoded exponent field, and
// code is 0 ok / 1 out-of-range (infinity or a value that rounded to zero).

// floor(q * log2(10)) for the decimal exponent range we care about
inline constexpr ::std::int_least32_t fp_scan_pow10_log2(::std::int_least64_t q) noexcept
{
	return static_cast<::std::int_least32_t>((q * 1741647) >> 19u);
}

// compare a against 2^s: a is a limb integer, s any exponent
inline int fp_big_cmp_pow2(fp_bigint const &a, ::std::int_least64_t s) noexcept
{
	if (s < 0)
	{
		return a.size ? 1 : -1;
	}
	auto const alen{fp_big_bitlen(a)};
	if (alen != static_cast<::std::size_t>(s + 1))
	{
		return alen < static_cast<::std::size_t>(s + 1) ? -1 : 1;
	}
	// a == 2^s iff its only set bit is the top one
	return fp_big_any_below(a, alen - 1u) ? 1 : 0;
}

// compare a*2^sa against b*2^sb without materializing a shifted operand
inline int fp_big_cmp_shift(fp_bigint const &a, fp_bigint const &b,
							::std::int_least64_t sb_minus_sa) noexcept
{
	if (sb_minus_sa >= 0)
	{
		return fp_big_cmp_shifted(a, b, static_cast<::std::size_t>(sb_minus_sa));
	}
	return -fp_big_cmp_shifted(b, a, static_cast<::std::size_t>(-sb_minus_sa));
}

// compare D * 10^E against 2^e
inline int fp_scan_cmp_value_pow2(fp_bigint const &d, ::std::int_least64_t e10,
								  ::std::int_least64_t e2) noexcept
{
	if (e10 >= 0)
	{
		fp_bigint n;
		n = d;
		fp_big_mul_pow5(n, static_cast<::std::uint_least64_t>(e10));
		return fp_big_cmp_pow2(n, e2 - e10);
	}
	fp_bigint den;
	fp_big_set_u64(den, 1u);
	fp_big_mul_pow5(den, static_cast<::std::uint_least64_t>(-e10));
	return fp_big_cmp_shift(d, den, e2 - e10);
}

struct fp_scan_target
{
	::std::uint_least32_t p;     // total significand bits including implicit
	::std::uint_least32_t ebits; // exponent field bits
	::std::int_least32_t bias;
	::std::int_least32_t denorm_exp; // exponent of the denormal unit: e_min - (p-1)
};

// exact floor(V * 2^-c) quotient and remainder-vs-half classification.
// e10 >= 0:  V * 2^-c = (D * 5^E) * 2^(E-c)
// e10 < 0 :  V * 2^-c = D * 2^(E-c) / 5^-E
// d is consumed.  rem_cmp: -1, 0, +1 for below / exactly at / above half.
inline void fp_scan_quotient(fp_bigint &d, ::std::int_least64_t e10,
							 ::std::int_least64_t c, __uint128_t &quot,
							 int &rem_cmp) noexcept
{
	if (e10 >= 0)
	{
		fp_big_mul_pow5(d, static_cast<::std::uint_least64_t>(e10));
		auto const t{e10 - c};
		if (t >= 0)
		{
			fp_big_shl(d, static_cast<::std::size_t>(t));
			quot = static_cast<__uint128_t>(d.limb[0]) |
				   (static_cast<__uint128_t>(d.size > 1u ? d.limb[1] : 0u) << 64u);
			rem_cmp = -1;
			return;
		}
		auto const s{static_cast<::std::size_t>(-t)};
		quot = 0u;
		for (::std::uint_least32_t i{}; i != 128u; ++i)
		{
			if (fp_big_bit(d, s + i))
			{
				quot |= static_cast<__uint128_t>(1) << i;
			}
		}
		rem_cmp = fp_big_bit(d, s - 1u) ? (fp_big_any_below(d, s - 1u) ? 1 : 0) : -1;
		return;
	}
	fp_bigint den;
	fp_big_set_u64(den, 1u);
	fp_big_mul_pow5(den, static_cast<::std::uint_least64_t>(-e10));
	auto const t{e10 - c};
	if (t >= 0)
	{
		fp_big_shl(d, static_cast<::std::size_t>(t));
	}
	else
	{
		fp_big_shl(den, static_cast<::std::size_t>(-t));
	}
	quot = 0u;
	auto j{fp_big_bitlen(d) > fp_big_bitlen(den) ? fp_big_bitlen(d) - fp_big_bitlen(den)
												: ::std::size_t{}};
	for (auto i{j + 1u}; i-- != 0u;)
	{
		if (fp_big_cmp_shifted(d, den, i) >= 0)
		{
			fp_big_sub_shifted(d, den, i);
			if (i < 128u)
			{
				quot |= static_cast<__uint128_t>(1) << i;
			}
		}
	}
	if (!d.size)
	{
		rem_cmp = -1;
		return;
	}
	// 2*remainder vs den == compare den vs rem<<1, negated
	auto const cmp{fp_big_cmp_shifted(den, d, 1u)};
	rem_cmp = cmp < 0 ? 1 : (cmp == 0 ? 0 : -1);
}

// overflow result: for binary80 the explicit integer bit stays set on
// infinity; every other format stores a zero significand on infinity
inline fp_scan_result fp_scan_inf(fp_scan_target const &tg) noexcept
{
	auto const max_field{(::std::int_least64_t{1} << tg.ebits) - 1};
	return {tg.p == 64u ? ::std::uint_least64_t{1} << 63u : ::std::uint_least64_t{},
			0u, static_cast<::std::int_least32_t>(max_field), 1};
}

// pack a rounded full significand (top bit = implicit) and exponent into
// the stored-words result; no implicit stripping, callers mask per format
inline fp_scan_result fp_scan_pack(__uint128_t q, ::std::int_least64_t e2,
								   fp_scan_target const &tg) noexcept
{
	fp_scan_result r{0, 0, 0, 0};
	auto const max_field{(::std::int_least64_t{1} << tg.ebits) - 1};
	if (q >> tg.p)
	{
		q >>= 1u;
		++e2;
	}
	if (e2 + tg.bias >= max_field)
	{
		return fp_scan_inf(tg);
	}
	r.efield = static_cast<::std::int_least32_t>(e2 + tg.bias);
	r.lo = static_cast<::std::uint_least64_t>(q);
	r.hi = static_cast<::std::uint_least64_t>(q >> 64u);
	return r;
}

// exact-path rounding of V = d * 10^e10 (d consumed)
inline fp_scan_result fp_scan_exact_one(fp_bigint &d, ::std::int_least64_t e10,
										fp_scan_target const &tg) noexcept
{
	auto const e_min{1 - tg.bias};
	auto const max_field{(::std::int_least64_t{1} << tg.ebits) - 1};
	// exact e2 = floor(log2 V): fixed-point estimate then limb comparisons
	auto e2{static_cast<::std::int_least64_t>(fp_scan_pow10_log2(e10)) +
			static_cast<::std::int_least64_t>(fp_big_bitlen(d)) - 1};
	for (; fp_scan_cmp_value_pow2(d, e10, e2 + 1) >= 0; ++e2)
	{
	}
	for (; fp_scan_cmp_value_pow2(d, e10, e2) < 0; --e2)
	{
	}
	if (e2 + tg.bias >= max_field)
	{
		return fp_scan_inf(tg);
	}
	if (e2 < tg.denorm_exp - 1)
	{
		// V < denorm_min/2 rounds to zero
		return {0, 0, 0, 1};
	}
	auto const c{e2 >= e_min ? e2 - static_cast<::std::int_least64_t>(tg.p) + 1
							 : static_cast<::std::int_least64_t>(tg.denorm_exp)};
	__uint128_t q{};
	int rem_cmp{};
	fp_scan_quotient(d, e10, c, q, rem_cmp);
	if (rem_cmp > 0 || (rem_cmp == 0 && (q & 1u)))
	{
		++q;
	}
	if (e2 < e_min)
	{
		// subnormal scale: q is the stored significand; q == 2^(p-1) rounds
		// up to the least normal value
		fp_scan_result r{static_cast<::std::uint_least64_t>(q),
						 static_cast<::std::uint_least64_t>(q >> 64u),
						 q >= (static_cast<__uint128_t>(1) << (tg.p - 1u)) ? 1 : 0, !q};
		return r;
	}
	return fp_scan_pack(q, e2, tg);
}

// exact significant digits of the midpoint (2k+1) * 2^t
inline fp_digits_result fp_scan_midpoint_digits(__uint128_t odd_sig,
												::std::int_least64_t t, char *out) noexcept
{
	fp_bigint n;
	fp_big_set_u128(n, static_cast<::std::uint_least64_t>(odd_sig),
					static_cast<::std::uint_least64_t>(odd_sig >> 64u));
	::std::int_least32_t e10{};
	if (t >= 0)
	{
		fp_big_shl(n, static_cast<::std::size_t>(t));
	}
	else
	{
		fp_big_mul_pow5(n, static_cast<::std::uint_least64_t>(-t));
		e10 = static_cast<::std::int_least32_t>(t);
	}
	constexpr ::std::uint_least64_t group{10000000000000000000u};
	::std::size_t len{};
	while (n.size)
	{
		auto r{fp_big_divmod_u64(n, group)};
		for (::std::uint_least32_t i{}; i != 19u; ++i)
		{
			out[len++] = static_cast<char>(r % 10u);
			r /= 10u;
		}
	}
	while (len > 1u && !out[len - 1u])
	{
		--len;
	}
	for (::std::size_t i{}, j{len - 1u}; i < j; ++i, --j)
	{
		auto const tc{out[i]};
		out[i] = out[j];
		out[j] = tc;
	}
	return {static_cast<::std::int_least32_t>(len),
			e10 + static_cast<::std::int_least32_t>(len)};
}

fp_scan_result
fp_scan_decimal(char const *digits, ::std::size_t n_digits, ::std::int_least64_t e10,
				bool sticky, ::std::uint_least32_t p, ::std::uint_least32_t ebits) noexcept
{
	fp_scan_target const tg{p, ebits, (::std::int_least32_t{1} << (ebits - 1u)) - 1,
							1 - ((::std::int_least32_t{1} << (ebits - 1u)) - 1) -
								static_cast<::std::int_least32_t>(p) + 1};
	if (!n_digits)
	{
		return {0, 0, 0, 0};
	}
	fp_bigint d;
	fp_big_set_u64(d, 0u);
	for (::std::size_t i{}; i != n_digits; ++i)
	{
		fp_big_mul_add_u64(d, 10u, static_cast<::std::uint_least64_t>(digits[i]));
	}
	if (!d.size)
	{
		return {0, 0, 0, 0};
	}
	auto const max_field{(::std::int_least64_t{1} << ebits) - 1};
	auto const dec_exp{e10 + static_cast<::std::int_least64_t>(n_digits)};
	// definite overflow: V >= 10^(dec_exp-1) with dec_exp-1 > floor(log10 max)+1
	{
		auto const bound{static_cast<::std::int_least64_t>(
			((static_cast<::std::int_least64_t>(tg.bias) + 1) * 78913) >> 18u) + 2};
		if (dec_exp > bound)
		{
			return fp_scan_inf(tg);
		}
	}
	// definite zero: V < 10^dec_exp <= 2^(denorm_exp-1) = denorm_min/2
	{
		auto const bound{static_cast<::std::int_least64_t>(
			(static_cast<::std::int_least64_t>(tg.denorm_exp) - 1) * 78913 >> 18u)};
		if (dec_exp <= bound)
		{
			return {0, 0, 0, 1};
		}
	}
	auto const e_min{1 - tg.bias};
	// fast path: exact short input inside the cached power range, narrow target
	if (n_digits <= 19u && !sticky && e10 >= -293 && e10 <= 324 && p <= 53u)
	{
		::std::uint_least64_t sig{};
		for (::std::size_t i{}; i != n_digits; ++i)
		{
			sig = sig * 10u + static_cast<::std::uint_least64_t>(digits[i]);
		}
		auto const lz{static_cast<::std::int_least32_t>(::std::countl_zero(sig))};
		auto const q10{static_cast<::std::int_least32_t>(e10)};
		auto const power{floating_pow10_get(q10)};
		auto const product{floating_umul64x128_hi(sig << lz, power.lo, power.hi)};
		auto const upperbit{static_cast<::std::int_least32_t>(product.hi >> 63u)};
		auto const e2{static_cast<::std::int_least64_t>(63) +
					  fp_scan_pow10_log2(q10) - lz + upperbit};
		if (e2 + tg.bias >= max_field)
		{
			return fp_scan_inf(tg);
		}
		if (e2 < tg.denorm_exp - 1)
		{
			return {0, 0, 0, 1};
		}
		// the product is V * 2^(126+ub-e2); bit j of it carries 2^c where
		// c is the kept-grid exponent (e2-p+1 normals, denorm_exp subnormals)
		auto const j{e2 >= e_min ? 127 + upperbit - static_cast<::std::int_least32_t>(p)
								 : 127 + upperbit - static_cast<::std::int_least32_t>(p) + (e_min - e2)};
		__uint128_t const prod{(static_cast<__uint128_t>(product.hi) << 64u) | product.lo};
		if (j <= 128)
		{
			auto const rem{j == 128 ? prod : prod & ((static_cast<__uint128_t>(1)
													<< static_cast<unsigned>(j)) - 1u)};
			auto const halfv{static_cast<__uint128_t>(1) << static_cast<unsigned>(j - 1)};
			auto const diff{rem > halfv ? rem - halfv : halfv - rem};
			if (diff > (static_cast<__uint128_t>(1) << 64u))
			{
				// unambiguous: product error < 2^64 cannot cross the midpoint
				auto q{j == 128 ? __uint128_t{} : prod >> static_cast<unsigned>(j)};
				if (rem > halfv)
				{
					++q;
				}
				if (e2 < e_min)
				{
					fp_scan_result r{static_cast<::std::uint_least64_t>(q),
									 static_cast<::std::uint_least64_t>(q >> 64u),
									 q >= (static_cast<__uint128_t>(1) << (p - 1u)) ? 1 : 0,
									 !q};
					return r;
				}
				return fp_scan_pack(q, e2, tg);
			}
		}
	}
	// exact path
	if (!sticky)
	{
		return fp_scan_exact_one(d, e10, tg);
	}
	// truncated input: round both endpoints; <= 1 ulp apart by the capacity bound
	fp_bigint dlo{d};
	auto lo_res{fp_scan_exact_one(dlo, e10, tg)};
	fp_bigint dhi{d};
	fp_big_add_u64(dhi, 1u);
	auto hi_res{fp_scan_exact_one(dhi, e10, tg)};
	if (lo_res.lo == hi_res.lo && lo_res.hi == hi_res.hi && lo_res.efield == hi_res.efield)
	{
		return lo_res;
	}
	// straddles the midpoint above lo_res: compare the input digits against
	// the midpoint's exact decimal expansion (bounded by fp_scan_digits_cap)
	__uint128_t odd_sig;
	::std::int_least64_t mt;
	if (!lo_res.efield)
	{
		// subnormal unit grid: M = (2*sig+1) * 2^(denorm_exp-1)
		auto const full{(static_cast<__uint128_t>(lo_res.hi) << 64u) | lo_res.lo};
		odd_sig = full * 2u + 1u;
		mt = static_cast<::std::int_least64_t>(tg.denorm_exp) - 1;
	}
	else
	{
		auto const full{((static_cast<__uint128_t>(lo_res.hi) << 64u) | lo_res.lo) |
						(static_cast<__uint128_t>(1) << (p - 1u))};
		odd_sig = full * 2u + 1u;
		mt = static_cast<::std::int_least64_t>(lo_res.efield) - tg.bias -
			 static_cast<::std::int_least64_t>(p);
	}
	char mid_digits[fp_scan_digits_cap];
	auto const md{fp_scan_midpoint_digits(odd_sig, mt, mid_digits)};
	int order{};
	if (dec_exp != md.e10)
	{
		order = dec_exp < md.e10 ? -1 : 1;
	}
	else
	{
		// compare digit strings at equal exponent, zeros padding the
		// shorter; the midpoint expansion never exceeds the capacity
		auto const k{n_digits > static_cast<::std::size_t>(md.n)
						 ? n_digits
						 : static_cast<::std::size_t>(md.n)};
		for (::std::size_t i{}; i != k; ++i)
		{
			auto const a{i < n_digits ? digits[i] : 0};
			auto const b{i < static_cast<::std::size_t>(md.n) ? mid_digits[i] : 0};
			if (a != b)
			{
				order = a < b ? -1 : 1;
				break;
			}
		}
		if (!order)
		{
			// input matches the midpoint through every retained digit: the
			// dropped tail decides, positive iff any dropped digit is nonzero
			order = sticky ? 1 : 0;
		}
	}
	if (order < 0)
	{
		return lo_res;
	}
	if (order == 0)
	{
		// exact tie: nearest-even keeps the even endpoint
		return (lo_res.lo & 1u) ? hi_res : lo_res;
	}
	return hi_res;
}

// hexfloat scanning: V = (S + tail) * 2^e2 where S is the retained
// significant nibble sequence (values 0-15 in nibs[0..n)) and the tail
// in [0,1) is nonzero iff sticky.  The header's retention cap keeps S
// at 30 nibbles or fewer so it always fits in a u128, and guarantees a
// dropped nibble sits strictly below the rounding bit (pure sticky).
fp_scan_result
fp_scan_hex(char const *nibs, ::std::size_t n, ::std::int_least64_t e2,
			bool sticky, ::std::uint_least32_t p, ::std::uint_least32_t ebits) noexcept
{
	fp_scan_target const tg{p, ebits, (::std::int_least32_t{1} << (ebits - 1u)) - 1,
							1 - ((::std::int_least32_t{1} << (ebits - 1u)) - 1) -
								static_cast<::std::int_least32_t>(p) + 1};
	__uint128_t sig{};
	for (::std::size_t i{}; i != n; ++i)
	{
		sig = sig * 16u + static_cast<::std::uint_least32_t>(nibs[i]);
	}
	if (!sig)
	{
		return {0, 0, 0, 0};
	}
	auto const hi{static_cast<::std::uint_least64_t>(sig >> 64u)};
	auto const bits{static_cast<::std::int_least64_t>(
		hi ? 128u - ::std::countl_zero(hi)
		   : 64u - ::std::countl_zero(static_cast<::std::uint_least64_t>(sig)))};
	// value < (sig+1) * 2^e2 <= 2^(bits+e2): floor(log2 V) = e2 + bits - 1
	auto const E{e2 + bits - 1};
	auto const max_field{(::std::int_least64_t{1} << ebits) - 1};
	if (E + tg.bias >= max_field)
	{
		return fp_scan_inf(tg);
	}
	if (E < tg.denorm_exp - 1)
	{
		// V < denorm_min/2 rounds to zero
		return {0, 0, 0, 1};
	}
	auto const e_min{1 - tg.bias};
	auto const c{E >= e_min ? E - static_cast<::std::int_least64_t>(p) + 1
							: static_cast<::std::int_least64_t>(tg.denorm_exp)};
	auto const drop{c - e2};
	__uint128_t q{};
	int rem_cmp{-1};
	if (drop <= 0)
	{
		// exact grid hit; unreachable when sticky (a dropped nibble
		// requires the retained sequence to carry more than p bits)
		q = sig << static_cast<::std::uint_least32_t>(-drop);
	}
	else
	{
		auto const d{static_cast<::std::uint_least32_t>(drop)};
		auto const one{static_cast<__uint128_t>(1)};
		q = d >= 128u ? __uint128_t{} : sig >> d;
		auto const rem{d >= 128u ? sig : sig & ((one << d) - 1u)};
		auto const half{one << (d - 1u)};
		rem_cmp = rem > half ? 1 : rem < half ? -1 : (sticky ? 1 : 0);
	}
	if (rem_cmp > 0 || (rem_cmp == 0 && (q & 1u)))
	{
		++q;
	}
	if (E < e_min)
	{
		// subnormal scale: q is the stored significand; q == 2^(p-1)
		// rounds up to the least normal value
		fp_scan_result r{static_cast<::std::uint_least64_t>(q),
						 static_cast<::std::uint_least64_t>(q >> 64u),
						 q >= (static_cast<__uint128_t>(1) << (p - 1u)) ? 1 : 0, !q};
		return r;
	}
	return fp_scan_pack(q, E, tg);
}

} // namespace fast_io::details

export module fast_io.floating;
