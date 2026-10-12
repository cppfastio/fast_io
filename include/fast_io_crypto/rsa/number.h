#pragma once

/*
Multi-precision unsigned integer primitives for RSA.

Unlike curve25519's field_number, RSA modulus sizes are not known at compile
time (certificates carry 1024..8192-bit keys), so numbers are plain
little-endian limb arrays with a runtime limb count. RSA verification works
on public data only, so all routines are variable-time.
*/

namespace fast_io::details::rsa
{

using value_type = ::std::uint_least64_t;
inline constexpr ::std::size_t limb_digits{::std::numeric_limits<value_type>::digits};
inline constexpr ::std::size_t limb_bytes{sizeof(value_type)};

/*
z[0..n) = x[0..n) + y[0..n); returns the carry-out as a limb (0 or 1).

PERFORMANCE: runtime-n loop does not unroll under clang -O2/-O3
(even -funroll-loops); each limb costs movzbl+btl+adcq+setb (~8 insns)
instead of a flat movq/adcq chain (~3 insns). A fixed limb count or a
manually expanded chain fixes it, but the limb count is genuinely
runtime here.
*/
inline constexpr value_type limbs_addition(value_type *z, value_type const *x, value_type const *y, ::std::size_t n) noexcept
{
	bool carry{};
	for (::std::size_t i{}; i != n; ++i)
	{
		z[i] = ::fast_io::intrinsics::addc(x[i], y[i], carry, carry);
	}
	constexpr value_type zero{};
	return ::fast_io::intrinsics::addc(zero, zero, carry, carry);
}

/*
z[0..n) = x[0..n) - y[0..n); returns ~0 when x < y (borrow) else 0.

PERFORMANCE: same rolled-loop problem as limbs_addition -- sbb/setb
per limb rather than a straight sbb chain.
*/
inline constexpr value_type limbs_subtraction(value_type *z, value_type const *x, value_type const *y, ::std::size_t n) noexcept
{
	bool borrow{};
	for (::std::size_t i{}; i != n; ++i)
	{
		z[i] = ::fast_io::intrinsics::subc(x[i], y[i], borrow, borrow);
	}
	constexpr value_type zero{};
	return ::fast_io::intrinsics::subc(zero, zero, borrow, borrow);
}

/* x < y over n limbs. */
inline constexpr bool limbs_less(value_type const *x, value_type const *y, ::std::size_t n) noexcept
{
	for (::std::size_t i{n}; i--;)
	{
		if (x[i] != y[i])
		{
			return x[i] < y[i];
		}
	}
	return false;
}

/* true when every limb is zero. */
inline constexpr bool limbs_is_zero(value_type const *x, ::std::size_t n) noexcept
{
	for (::std::size_t i{}; i != n; ++i)
	{
		if (x[i] != 0)
		{
			return false;
		}
	}
	return true;
}

/*
z[0..n) = x[0..n) << 1, returning the shifted-out top bit as a limb (0 or 1).
addc(v, v) doubles through the carry flag (the adc x,x idiom). z and x may
alias for in-place shifts.

PERFORMANCE: rolled loop; see limbs_addition.
*/
inline constexpr value_type limbs_shift_left1(value_type *z, value_type const *x, ::std::size_t n) noexcept
{
	bool carry{};
	for (::std::size_t i{}; i != n; ++i)
	{
		z[i] = ::fast_io::intrinsics::addc(x[i], x[i], carry, carry);
	}
	constexpr value_type zero{};
	return ::fast_io::intrinsics::addc(zero, zero, carry, carry);
}

/*
dest[0..n) += x[0..n) * b and dest[n] = carry-out.
dest[n] is written (not accumulated); callers arrange scratch so it is
untouched. One schoolbook row of a product.

PERFORMANCE: this is the hot inner row of both the product and the
Montgomery reduction. Rolled loop: mulq+add+adc per limb (~7 insns).
-march=native does not turn it into a mulxq/adcxq/adoxq sequence; openssl's
asm rsaz/avx512 paths are ~4x faster on the full verify partly for this
reason.
*/
inline constexpr void limbs_multiply_add(value_type *dest, value_type const *x, value_type b, ::std::size_t n) noexcept
{
	value_type carry{};
	for (::std::size_t i{}; i != n; ++i)
	{
		value_type hi;
		value_type const lo{::fast_io::intrinsics::umul(x[i], b, hi)};
		bool c1, c2;
		value_type const s{::fast_io::intrinsics::addc(dest[i], lo, false, c1)};
		dest[i] = ::fast_io::intrinsics::addc(s, carry, false, c2);
		/*
		x[i]*b + dest[i] + carry <= (B-1)^2 + 2(B-1) = B^2 - 1, so
		hi + c1 + c2 <= B - 1 and cannot wrap.
		*/
		carry = hi + static_cast<value_type>(c1) + static_cast<value_type>(c2);
	}
	dest[n] = carry;
}

/* t[0..2n) = x[0..n) * y[0..n). */
inline constexpr void limbs_multiplication(value_type *t, value_type const *x, value_type const *y, ::std::size_t n) noexcept
{
	for (::std::size_t i{}; i != n; ++i)
	{
		t[i] = 0;
	}
	for (::std::size_t i{}; i != n; ++i)
	{
		limbs_multiply_add(t + i, x, y[i], n);
	}
}

/*
x = 2x mod n over nl limbs, with x < n on entry and exit.
d is scratch of nl limbs. One step of the R^2 doubling loop.
*/
inline constexpr void limbs_mod_double(value_type *x, value_type const *n, value_type *d, ::std::size_t nl) noexcept
{
	value_type const c{limbs_shift_left1(x, x, nl)};
	value_type const borrow{limbs_subtraction(d, x, n, nl)};
	/*
	Take d = x - n when the shift overflowed (2x >= 2^(nl*64) > n) or when
	x >= n (no borrow). sel = ~0 means "take d".
	*/
	value_type const sel{(static_cast<value_type>(0) - c) | ~borrow};
	for (::std::size_t i{}; i != nl; ++i)
	{
		x[i] = (d[i] & sel) | (x[i] & ~sel);
	}
}

/* One limb from limb_bytes big-endian bytes. */
inline constexpr value_type limb_from_bytes_be(::std::byte const *p) noexcept
{
	value_type v FAST_IO_INDETERMINATE;
	::fast_io::freestanding::type_punning_from_bytes(p, v);
	if constexpr (::std::endian::native == ::std::endian::little)
	{
		v = ::fast_io::byte_swap(v);
	}
	return v;
}

inline constexpr void limb_to_bytes_be(::std::byte *p, value_type v) noexcept
{
	if constexpr (::std::endian::native == ::std::endian::little)
	{
		v = ::fast_io::byte_swap(v);
	}
	::fast_io::freestanding::type_punning_to_bytes(v, p);
}

/*
z[0..nl) = the unsigned big-endian byte string p[0..plen).
plen may exceed nl*limb_bytes; the dropped leading bytes must be zero.
*/
inline constexpr bool limbs_from_bytes_be(value_type *z, ::std::byte const *p, ::std::size_t plen, ::std::size_t nl) noexcept
{
	for (::std::size_t i{}; i != nl; ++i)
	{
		z[i] = 0;
	}
	::std::size_t const capacity{nl * limb_bytes};
	if (capacity < plen)
	{
		::std::size_t const drop{plen - capacity};
		for (::std::size_t i{}; i != drop; ++i)
		{
			if (p[i] != ::std::byte{})
			{
				return false;
			}
		}
		p += drop;
		plen = capacity;
	}
	for (::std::size_t i{}; i != nl; ++i)
	{
		/* limb i holds the last limb_bytes bytes of the remaining string */
		::std::size_t const tail{plen - i * limb_bytes};
		if (tail == 0)
		{
			break;
		}
		value_type v{};
		if (tail < limb_bytes)
		{
			/* partial top limb */
			for (::std::size_t j{}; j != tail; ++j)
			{
				v = static_cast<value_type>(v << 8u) | static_cast<value_type>(p[j]);
			}
		}
		else
		{
			v = limb_from_bytes_be(p + tail - limb_bytes);
		}
		z[i] = v;
	}
	return true;
}

/* p[0..plen) = z[0..nl) as a big-endian byte string; z must fit in plen bytes. */
inline constexpr void limbs_to_bytes_be(::std::byte *p, value_type const *z, ::std::size_t plen, ::std::size_t nl) noexcept
{
	::std::size_t i{};
	::std::byte *q{p + plen};
	for (::std::size_t n{plen / limb_bytes}; n--; ++i)
	{
		q -= limb_bytes;
		limb_to_bytes_be(q, i < nl ? z[i] : static_cast<value_type>(0));
	}
	if (::std::size_t const rem{plen % limb_bytes})
	{
		value_type const v{i < nl ? z[i] : static_cast<value_type>(0)};
		/*
		the top limb's trailing rem bytes, most significant first:
		p[rem-1] takes v's low byte, p[0] its high one
		*/
		for (::std::size_t j{}; j != rem; ++j)
		{
			*--q = static_cast<::std::byte>(v >> (j * 8u));
		}
	}
}

/*
x[0..nl) = 2^bits mod n.

Divides a power of two by n one limb at a time instead of one bit at a
time (what BN_mod ends up doing for 2^k). Each step multiplies the
running remainder by 2^limb_digits and reduces it with a single
2-by-1 quotient-digit estimate:

  qhat = floor((v_hi * B + v_next) / n_top)   (clamped to B-1)
  v   -= qhat * n
  while (v top limb wrapped) v += n           (add-back, <= 2 times)

The divisor is normalized so its top limb has the high bit set, which is
the Knuth D precondition bounding qhat - q <= 2. The remainder is
unnormalized at the end. scratch must hold 3*nl+1 limbs
[normalized n | running remainder | shifted dividend].
*/
inline constexpr void limbs_pow2_mod(value_type *x, ::std::size_t bits,
									 value_type const *n, ::std::size_t nl,
									 value_type *scratch) noexcept
{
	constexpr value_type zero{};
	value_type *const np{scratch};
	value_type *const rem{np + nl};
	value_type *const v{rem + nl};
	unsigned const s{static_cast<unsigned>(::std::countl_zero(n[nl - 1]))};
	/* np = n << s */
	np[0] = static_cast<value_type>(n[0] << s);
	for (::std::size_t i{1}; i != nl; ++i)
	{
		np[i] = ::fast_io::intrinsics::shiftleft(n[i - 1], n[i], s);
	}
	for (::std::size_t i{}; i != nl; ++i)
	{
		rem[i] = 0;
	}
	rem[0] = static_cast<value_type>(value_type{1} << s); /* normalized 1 */
	/* sub-word leading steps: rem = rem * 2 mod n, done in normalized domain */
	::std::size_t whole{bits / limb_digits};
	for (::std::size_t i{bits % limb_digits}; i--;)
	{
		limbs_mod_double(rem, np, v, nl);
	}
	value_type const ntop{np[nl - 1]};
	for (; whole--;)
	{
		/* v = rem * B: limbs shifted up by one */
		v[0] = 0;
		for (::std::size_t i{}; i != nl; ++i)
		{
			v[i + 1] = rem[i];
		}
		/* quotient-digit estimate (v < n*B, so qhat is a limb) */
		value_type qhat;
		if (v[nl] >= ntop)
		{
			qhat = static_cast<value_type>(~0);
		}
		else
		{
			qhat = ::fast_io::intrinsics::udivbigbysmalltosmalldefault(v[nl], v[nl - 1], ntop).quotient;
		}
		/* v -= qhat * np */
		value_type cy{};
		for (::std::size_t j{}; j != nl; ++j)
		{
			value_type hi;
			value_type const lo{::fast_io::intrinsics::umul(qhat, np[j], hi)};
			bool b1, b2;
			value_type const t{::fast_io::intrinsics::subc(v[j], cy, false, b1)};
			v[j] = ::fast_io::intrinsics::subc(t, lo, false, b2);
			cy = hi + static_cast<value_type>(b1) + static_cast<value_type>(b2);
		}
		v[nl] = static_cast<value_type>(v[nl] - cy);
		/*
		Result is in (-2*n, n): a wrapped (nonzero) top limb means qhat
		overshot; adding np once or twice restores a small nonneg value.
		*/
		while (v[nl] != 0)
		{
			bool c{};
			for (::std::size_t j{}; j != nl; ++j)
			{
				v[j] = ::fast_io::intrinsics::addc(v[j], np[j], c, c);
			}
			v[nl] = ::fast_io::intrinsics::addc(v[nl], zero, c, c);
		}
		for (::std::size_t i{}; i != nl; ++i)
		{
			rem[i] = v[i];
		}
	}
	/* x = rem >> s */
	x[nl - 1] = ::fast_io::intrinsics::shiftright(rem[nl - 1], zero, s);
	for (::std::size_t i{nl - 1}; i--;)
	{
		x[i] = ::fast_io::intrinsics::shiftright(rem[i], rem[i + 1], s);
	}
}

/* bit length of x[0..nl) (0 for all-zero). */
inline constexpr ::std::size_t limbs_bit_length(value_type const *x, ::std::size_t nl) noexcept
{
	for (::std::size_t i{nl}; i--;)
	{
		if (x[i] != 0)
		{
			return i * limb_digits + (limb_digits - static_cast<::std::size_t>(::std::countl_zero(x[i])));
		}
	}
	return 0;
}

} // namespace fast_io::details::rsa
