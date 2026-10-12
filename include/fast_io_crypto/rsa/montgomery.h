#pragma once

/*
Montgomery modular arithmetic for the RSA public-key operation
(m = s^e mod n). This is the same domain OpenSSL's bn_mont.c works in:

  n0inv = -n^-1 mod B            (B = 2^limb_digits, the limb base)
  R     = B^nl                   (nl limbs)
  r2    = R^2 mod n              (setup constant, converts into the domain)
  redc(t) = t * R^-1 mod n       (SOS reduction, word by word)
  mont_mul(a,b) = redc(a*b) = a*b*R^-1 mod n

openssl computes n0inv through the full bignum extended gcd as
(R * (R^-1 mod n0) - 1) / n0; for a single limb the Newton iteration
x = x*(2 - n0*x) reaches the identical -n^-1 mod B value, so we use it
directly. openssl computes RR = 2^(2*ri) mod N with a bignum division;
limbs_pow2_mod does the same division a limb at a time through a
normalized quotient-digit estimate.
*/

namespace fast_io::details::rsa
{

/* -n0^-1 mod B; n0 must be odd (true for every RSA modulus). */
inline constexpr value_type montgomery_n0_inverse(value_type n0) noexcept
{
	value_type x{n0};
	/* n0^2 == 1 mod 8 for odd n0, so x already has 3 correct bits;
	   each Newton step doubles them: 3 -> 6 -> 12 -> 24 -> 48 -> 96. */
	for (int i{}; i != 5; ++i)
	{
		x *= static_cast<value_type>(2) - n0 * x;
	}
	return static_cast<value_type>(0) - x;
}

/*
r2[0..nl) = 2^(2*nl*limb_digits) mod n = R^2 mod n.
Computed by limbs_pow2_mod, which does the division a limb at a time.
scratch must hold 3*nl+1 limbs.
*/
inline constexpr void montgomery_r2_setup(value_type *r2, value_type const *n, ::std::size_t nl,
										  value_type *scratch) noexcept
{
	limbs_pow2_mod(r2, 2 * nl * limb_digits, n, nl, scratch);
}

/*
Montgomery reduction, separated operand scanning (same shape as openssl's
bn_from_montgomery_word): t[0..2nl] holds a product in its low 2nl limbs
with t[2nl] == 0, and we add m_i * n * B^i with m_i = t[i]*n0inv mod B so
the low limb zeroes out. Carries may ripple one limb above i+nl, reaching
at most index 2nl. Afterwards the value lives in t[nl..2nl] (< 2n) and a
single conditional subtract of n brings it below n.

PERFORMANCE: dominant cost of the whole verify (~all of the modexp time
lands here and in limbs_multiplication). Two issues: (1) SOS does a full
n-limb multiply and then a full n-limb reduction pass; a fused CIOS loop
(interleave product row and reduction row) halves memory traffic. (2) all
loops are rolled runtime-n loops -- see limbs_addition; no unrolled carry
chains, no mulx/adx. openssl's montgomery is hand-written asm.
*/
inline constexpr void montgomery_reduction(value_type *z, value_type *t, value_type const *n, value_type n0inv, ::std::size_t nl) noexcept
{
	constexpr value_type zero{};
	for (::std::size_t i{}; i != nl; ++i)
	{
		value_type const m{static_cast<value_type>(t[i] * n0inv)};
		value_type carry{};
		for (::std::size_t j{}; j != nl; ++j)
		{
			value_type hi;
			value_type const lo{::fast_io::intrinsics::umul(m, n[j], hi)};
			bool c1, c2;
			value_type const s{::fast_io::intrinsics::addc(t[i + j], lo, false, c1)};
			t[i + j] = ::fast_io::intrinsics::addc(s, carry, false, c2);
			carry = hi + static_cast<value_type>(c1) + static_cast<value_type>(c2);
		}
		bool c{};
		t[i + nl] = ::fast_io::intrinsics::addc(t[i + nl], carry, false, c);
		for (::std::size_t k{i + nl + 1}; c && k <= 2 * nl; ++k)
		{
			t[k] = ::fast_io::intrinsics::addc(t[k], zero, c, c);
		}
	}
	bool borrow{};
	for (::std::size_t i{}; i != nl; ++i)
	{
		z[i] = ::fast_io::intrinsics::subc(t[nl + i], n[i], borrow, borrow);
	}
	::fast_io::intrinsics::subc(t[2 * nl], zero, borrow, borrow);
	/* borrow means t[nl..2nl] < n: restore; else z holds the difference. */
	value_type const mask{static_cast<value_type>(0) - static_cast<value_type>(borrow)};
	for (::std::size_t i{}; i != nl; ++i)
	{
		z[i] = (z[i] & ~mask) | (t[nl + i] & mask);
	}
}

/*
z[0..nl) = a * b * R^-1 mod n. t is scratch of 2*nl+1 limbs.
Inputs must be < n (or at least < R).
*/
inline constexpr void montgomery_multiplication(value_type *z, value_type const *a, value_type const *b,
												value_type const *n, value_type n0inv, ::std::size_t nl,
												value_type *t) noexcept
{
	limbs_multiplication(t, a, b, nl);
	t[2 * nl] = 0;
	montgomery_reduction(z, t, n, n0inv, nl);
}

/*
z[0..nl) = x^e mod n. x is a normal-domain value < n; e is a big-endian
byte string (the DER INTEGER content). r2 is the setup constant above.
scratch must hold 5*nl+2 limbs (product buffer, xm, acc, one).

Binary square-and-multiply over the exponent bits, most significant
first. e is public (the RSA public exponent), so no blinding or window
machinery is needed.

PERFORMANCE: variable-time by design -- do NOT reuse this for secret
exponents (RSA private op, FFDHE). For the common e = 65537 the cost is
17 squarings + 1 multiply, so windowing buys nothing; the squarings
themselves are the rolled-loop montgomery_multiplication above, which is
where the ~4x gap vs openssl's ADX asm lives.
*/
inline constexpr void montgomery_pow(value_type *z, value_type const *x,
									 ::std::byte const *e, ::std::size_t elen,
									 value_type const *n, value_type const *r2, value_type n0inv,
									 ::std::size_t nl,
									 value_type *scratch) noexcept
{
	value_type *const t{scratch};
	value_type *const xm{t + 2 * nl + 2};
	value_type *const acc{xm + nl};
	value_type *const one{acc + nl};
	for (::std::size_t i{}; i != nl; ++i)
	{
		acc[i] = 0;
		one[i] = 0;
	}
	one[0] = 1;
	/* acc = R mod n = mont(1); xm = x*R mod n */
	montgomery_multiplication(acc, one, r2, n, n0inv, nl, t);
	montgomery_multiplication(xm, x, r2, n, n0inv, nl, t);
	while (elen != 0 && *e == ::std::byte{})
	{
		++e;
		--elen;
	}
	for (::std::size_t i{}; i != elen; ++i)
	{
		unsigned const byte{static_cast<unsigned>(e[i])};
		for (unsigned bit{8}; bit--;)
		{
			montgomery_multiplication(acc, acc, acc, n, n0inv, nl, t);
			if (((byte >> bit) & 1u) != 0)
			{
				montgomery_multiplication(acc, acc, xm, n, n0inv, nl, t);
			}
		}
	}
	/* mont(acc, 1) = acc*R^-1: back to the normal domain */
	montgomery_multiplication(z, acc, one, n, n0inv, nl, t);
}

} // namespace fast_io::details::rsa
