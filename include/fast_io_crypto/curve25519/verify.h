#pragma once

namespace fast_io::curve25519
{

/* sqrt(-1) mod p */
inline constexpr field_number w_i{field_number_from_u64(0xC4EE1B274A0EA0B0ULL, 0x2F431806AD2FE478ULL, 0x2B4D00993DFBD7A7ULL, 0x2B8324804FC1DF0BULL)};

/* d = -121665/121666 mod p */
inline constexpr field_number w_d{field_number_from_u64(0x75EB4DCA135978A3ULL, 0x00700A4D4141D8ABULL, 0x8CC740797779E898ULL, 0x52036CEE2B6FFE73ULL)};

inline constexpr field_number w_one{field_number_from_u64(1, 0, 0, 0)};
inline constexpr field_number w_zero{};

/* Z = X^(2^n) * Y */
inline constexpr void field_number_sqr_mul_reduce(field_number &z, field_number const &x, std::size_t n, field_number const &y) noexcept
{
	field_number t FAST_IO_INDETERMINATE;
	field_number_square(t, x);
	while (--n)
	{
		field_number_square(t, t);
	}
	field_number_multiplication(z, t, y);
}

/* Y = X^(2^252-3) mod p */
inline constexpr void field_number_pow_2523(field_number &y, field_number const &x) noexcept
{
	field_number x2 FAST_IO_INDETERMINATE, x9 FAST_IO_INDETERMINATE, x11 FAST_IO_INDETERMINATE, x5 FAST_IO_INDETERMINATE, x10 FAST_IO_INDETERMINATE, x20 FAST_IO_INDETERMINATE, x50 FAST_IO_INDETERMINATE, x100 FAST_IO_INDETERMINATE, t FAST_IO_INDETERMINATE;

	field_number_square(x2, x);                      /* 2 */
	field_number_sqr_mul_reduce(x9, x2, 2, x);       /* 9 */
	field_number_multiplication(x11, x9, x2);        /* 11 */
	field_number_square(t, x11);                     /* 22 */
	field_number_multiplication(x5, t, x9);          /* 31 = 2^5 - 1 */
	field_number_sqr_mul_reduce(x10, x5, 5, x5);     /* 2^10 - 1 */
	field_number_sqr_mul_reduce(x20, x10, 10, x10);  /* 2^20 - 1 */
	field_number_sqr_mul_reduce(t, x20, 20, x20);    /* 2^40 - 1 */
	field_number_sqr_mul_reduce(x50, t, 10, x10);    /* 2^50 - 1 */
	field_number_sqr_mul_reduce(x100, x50, 50, x50); /* 2^100 - 1 */
	field_number_sqr_mul_reduce(t, x100, 100, x100); /* 2^200 - 1 */
	field_number_sqr_mul_reduce(t, t, 50, x50);      /* 2^250 - 1 */
	field_number_square(t, t);                       /* 2^251 - 2 */
	field_number_square(t, t);                       /* 2^252 - 4 */
	field_number_multiplication(y, t, x);            /* 2^252 - 3 */
}

namespace details
{

/*
Recover x coordinate from y and its parity bit.
x = sqrt((y^2-1)/(d*y^2+1)) = u*v^3 * (u*v^7)^((p-5)/8)
*/
inline constexpr void ed25519_calculate_x(field_number &x, field_number const &y, std::uint_least64_t parity) noexcept
{
	field_number u FAST_IO_INDETERMINATE, v FAST_IO_INDETERMINATE, a FAST_IO_INDETERMINATE, b FAST_IO_INDETERMINATE;

	field_number_square(u, y);              /* u = y^2 */
	field_number_multiplication(v, u, w_d); /* v = d*y^2 */
	field_number_subtraction(u, u, w_one);  /* u = y^2-1 */
	field_number_addition(v, v, w_one);     /* v = d*y^2+1 */

	field_number_square(b, v); /* b = v^2 */
	field_number_multiplication(a, u, b);
	field_number_multiplication(a, a, v); /* a = u*v^3 */
	field_number_square(b, b);            /* b = v^4 */
	field_number_multiplication(b, a, b); /* b = u*v^7 */
	field_number_pow_2523(b, b);          /* b = (u*v^7)^((p-5)/8) */
	field_number_multiplication(x, b, a); /* x = u*v^3 * (u*v^7)^((p-5)/8) */

	/* Check if we have the correct sqrt, else multiply by sqrt(-1) */
	field_number_square(b, x);
	field_number_multiplication(b, b, v);
	field_number_subtraction(b, b, u);
	field_number_reduce_to_25519(b);
	field_number::value_type bor{};
	for (::std::size_t bi{}; bi != field_number::array_size; ++bi)
	{
		bor |= b.index_unchecked(bi);
	}
	if (bor != 0)
	{
		field_number_multiplication(x, x, w_i);
	}

	field_number_reduce_to_25519(x);

	/* match parity */
	if (((x.front_unchecked() ^ parity) & 1) != 0)
	{
		subtraction_u256(x, wprime, x);
	}
}

inline constexpr void ed25519_unpack_point(affine_point &r, std::byte const *p) noexcept
{
	std::uint_least64_t const parity{ed25519_decode_int(r.y, p)};
	ed25519_calculate_x(r.x, r.y, parity);
}

} // namespace details

struct ed25519_verify_context
{
	::fast_io::containers::array<std::byte, 32> pk;
	::fast_io::containers::array<precomputed_extended_point, 16> q_table;
};

namespace details
{
inline constexpr void qtable_set(::fast_io::containers::array<precomputed_extended_point, 16> &tbl, std::size_t d, std::size_t s, extended_point const &q, extended_point &t) noexcept
{
	add_point(t, q, tbl.index_unchecked(s));
	edp_ext_point_2e(tbl.index_unchecked(d), t);
}

/*
Pre-compute the q-table for a public key.
q_table[i] = -(i0*Q + i1*2^64*Q + i2*2^128*Q + i3*2^192*Q) for i = i3i2i1i0 bits.
*/
inline constexpr void ed25519_verify_init_to_ptr(ed25519_verify_context &ctx, std::byte const *public_key) noexcept
{
	::fast_io::freestanding::nonoverlapped_bytes_copy_n(public_key, 32, ctx.pk.data());

	extended_point q FAST_IO_INDETERMINATE, t FAST_IO_INDETERMINATE;
	std::uint_least64_t const parity{ed25519_decode_int(q.y, public_key)};
	ed25519_calculate_x(q.x, q.y, parity ^ 1); /* -Q: inverted parity */
	field_multiplication_mod(q.t, q.x, q.y);
	q.z = w_one;

	auto &tbl{ctx.q_table};

	tbl.index_unchecked(0).ypx = w_one;
	tbl.index_unchecked(0).ymx = w_one;
	tbl.index_unchecked(0).t2d = w_zero;
	tbl.index_unchecked(0).z2 = field_number_from_u64(2, 0, 0, 0);

	edp_ext_point_2e(tbl.index_unchecked(1), q); /* -- -- -- q0 */

	std::size_t i{};
	for (; i != 64; ++i)
	{
		edp_double_point(q);
	}
	edp_ext_point_2e(tbl.index_unchecked(2), q); /* -- -- q1 -- */
	details::qtable_set(tbl, 3, 1, q, t);        /* -- -- q1 q0 */

	for (; i != 128; ++i)
	{
		edp_double_point(q);
	}
	edp_ext_point_2e(tbl.index_unchecked(4), q); /* -- q2 -- -- */
	details::qtable_set(tbl, 5, 1, q, t);        /* -- q2 -- q0 */
	details::qtable_set(tbl, 6, 2, q, t);        /* -- q2 q1 -- */
	details::qtable_set(tbl, 7, 3, q, t);        /* -- q2 q1 q0 */

	for (; i != 192; ++i)
	{
		edp_double_point(q);
	}
	edp_ext_point_2e(tbl.index_unchecked(8), q); /* q3 -- -- -- */
	for (std::size_t k{1}; k != 8; ++k)
	{
		details::qtable_set(tbl, 8 + k, k, q, t);
	}
}

/*
Calculate point R = a*P + b*Q where P is the ed25519 base point and
qtable = pre-computed -Q table for the signer's public key.
*/
inline constexpr void poly_point_multiply(affine_point &r, field_number const &a, field_number const &b,
										  ::fast_io::containers::array<precomputed_extended_point, 16> const &qtable) noexcept
{
	::fast_io::containers::array<std::byte, 32> u;
	::fast_io::containers::array<std::byte, 64> v;
	ecp_8folds(u, a);
	ecp_4folds(v, b);

	extended_point s FAST_IO_INDETERMINATE;
	auto const &q0{qtable.index_unchecked(static_cast<char unsigned>(v.front_unchecked()))};
	field_number_subtraction(s.x, q0.ypx, q0.ymx); /* 2x */
	field_number_addition(s.y, q0.ypx, q0.ymx);    /* 2y */
	field_number_multiplication(s.t, q0.t2d, wdi); /* 2xy */
	s.z = q0.z2;                                   /* 2z */

	for (std::size_t i{1}; i != 32; ++i)
	{
		edp_double_point(s);
		add_point(s, s, qtable.index_unchecked(static_cast<char unsigned>(v.index_unchecked(i))));
	}
	for (std::size_t i{32}; i != 64; ++i)
	{
		edp_double_point(s);
		add_affine_point(s, folding.index_unchecked(static_cast<char unsigned>(u.index_unchecked(i - 32))));
		add_point(s, s, qtable.index_unchecked(static_cast<char unsigned>(v.index_unchecked(i))));
	}

	field_number_inverse(s.z, s.z);
	field_multiplication_mod(r.x, s.x, s.z);
	field_multiplication_mod(r.y, s.y, s.z);
}

inline constexpr bool ed25519_verify_check_to_ptr(ed25519_verify_context const &ctx, std::byte const *signature, std::byte const *msg, std::size_t msg_size) noexcept
{
	::fast_io::sha512_context H;
	affine_point t FAST_IO_INDETERMINATE;
	field_number h FAST_IO_INDETERMINATE, s FAST_IO_INDETERMINATE;
	::fast_io::containers::array<std::byte, ::fast_io::sha512_context::digest_size> md;

	/* h = H(enc(R) || pk || m) mod BPO */
	H.update(signature, signature + 32);
	H.update(ctx.pk.data(), ctx.pk.data() + 32);
	H.update(msg, msg + msg_size);
	H.do_final();
	H.digest_to_byte_ptr(md.data());
	eco_digest_to_words(h, md.data());
	eco_mod(h);

	for (::std::size_t i{}; i != field_number::array_size; ++i)
	{
		s.index_unchecked(i) = bytes_to_limb_little_endian(signature + 32 + i * sizeof(field_number::value_type));
	}

	/* T = s*B + h*(-Q) = (s - h*a)*B = r*B = R */
	poly_point_multiply(t, s, h, ctx.q_table);
	ed25519_packpoint(md.data(), t.y, static_cast<std::byte>(t.x.front_unchecked()));

	std::uint_least64_t diff{};
	for (std::uint_fast8_t i{}; i != 32; ++i)
	{
		diff |= static_cast<std::uint_least64_t>(md.index_unchecked(i) ^ signature[i]);
	}
	return diff == 0;
}

inline constexpr bool ed25519_verify_signature_to_ptr(std::byte const *signature, std::byte const *public_key, std::byte const *msg, std::size_t msg_size) noexcept
{
	ed25519_verify_context ctx;
	ed25519_verify_init_to_ptr(ctx, public_key);
	return ed25519_verify_check_to_ptr(ctx, signature, msg, msg_size);
}

inline constexpr void ed25519_verify_init(ed25519_verify_context &ctx, ::fast_io::containers::index_span<std::byte const, 32> public_key) noexcept
{
	ed25519_verify_init_to_ptr(ctx, public_key.data());
}

inline constexpr bool ed25519_verify_check(ed25519_verify_context const &ctx, ::fast_io::containers::index_span<std::byte const, 64> signature, ::fast_io::containers::span<std::byte const> msg) noexcept
{
	return ed25519_verify_check_to_ptr(ctx, signature.data(), msg.data(), msg.size());
}

inline constexpr bool ed25519_verify_signature(::fast_io::containers::index_span<std::byte const, 64> signature, ::fast_io::containers::index_span<std::byte const, 32> public_key, ::fast_io::containers::span<std::byte const> msg) noexcept
{
	return ed25519_verify_signature_to_ptr(signature.data(), public_key.data(), msg.data(), msg.size());
}

} // namespace details

} // namespace fast_io::curve25519
