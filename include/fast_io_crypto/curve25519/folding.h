#pragma once

namespace fast_io::curve25519
{

struct extended_point
{
	field_number x, y, z, t;
};

struct precomputed_extended_point
{
	field_number ypx;
	field_number ymx;
	field_number t2d;
	field_number z2;
};

/*
Side-channel blinding context.  BP = -bl*B so that
(sk+bl)*B + BP = sk*B for any scalar sk, while zr projectively
randomizes the intermediate coordinates.
*/
struct edp_blinding_context
{
	field_number bl;
	field_number zr;
	precomputed_extended_point bp;
};

inline constexpr field_number w2d{field_number_from_u64(16993941304535871833ULL, 63073048630374742ULL, 1841551078520508720ULL, 2596001775599221991ULL)};
inline constexpr field_number wdi{field_number_from_u64(2729447966259148867ULL, 819046656689919022ULL, 3104705366353104742ULL, 4652357855831510595ULL)};

inline constexpr field_number wprime{field_number_from_u64(0xFFFFFFFFFFFFFFED, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF)};
inline constexpr field_number w_maxprime{field_number_from_u64(0xFFFFFFFFFFFFFFDA, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF)};

inline constexpr auto folding1{folding.index_unchecked(1)};

inline constexpr void add_affine_point(extended_point &p, precomputed_affine_point const &q) noexcept
{
	field_number a FAST_IO_INDETERMINATE, b FAST_IO_INDETERMINATE, c FAST_IO_INDETERMINATE, d FAST_IO_INDETERMINATE, e FAST_IO_INDETERMINATE;
	field_number_subtraction(a, p.y, p.x);
	field_number_multiplication(a, a, q.ymx);
	field_number_addition(b, p.y, p.x);
	field_number_multiplication(b, b, q.ypx);
	field_number_multiplication(c, p.t, q.t2d);
	field_number_addition(d, p.z, p.z);
	field_number_subtraction(e, b, a);
	field_number_addition(b, b, a);
	field_number_subtraction(a, d, c);
	field_number_addition(d, d, c);
	field_number_multiplication(p.x, e, a);
	field_number_multiplication(p.y, b, d);
	field_number_multiplication(p.t, e, b);
	field_number_multiplication(p.z, d, a);
}

inline constexpr void add_base_point(extended_point &p) noexcept
{
	add_affine_point(p, folding1);
}

inline constexpr void edp_double_point(extended_point &p) noexcept
{
	field_number a FAST_IO_INDETERMINATE, b FAST_IO_INDETERMINATE, c FAST_IO_INDETERMINATE, d FAST_IO_INDETERMINATE, e FAST_IO_INDETERMINATE;
	field_number_square(a, p.x);
	field_number_square(b, p.y);
	field_number_square(c, p.z);
	field_number_addition(c, c, c);
	field_number_subtraction(d, w_maxprime, a);

	field_number_subtraction(a, d, b);
	field_number_addition(d, d, b);
	field_number_subtraction(b, d, c);
	field_number_addition(e, p.x, p.y);
	field_number_square(e, e);
	field_number_addition(e, e, a);

	field_number_multiplication(p.x, e, b);
	field_number_multiplication(p.y, a, d);
	field_number_multiplication(p.z, d, b);
	field_number_multiplication(p.t, e, a);
}

/*
rcl $1, aa is ::fast_io::intrinsics::addc(aa,aa): shift left through carry.
shl $1, xx is ::fast_io::intrinsics::addc(xx,xx,0).
*/
template <typename T>
inline constexpr void rl_msb(std::uint_least8_t &aa, T &xx) noexcept
{
	bool carry{};
	xx = ::fast_io::intrinsics::addc(xx, xx, false, carry);
	aa = ::fast_io::intrinsics::addc(aa, aa, carry, carry);
}

inline constexpr void rl_msbs(std::uint_least8_t &aa, std::uint_least64_t &xx) noexcept
{
	bool carry{};
	xx = ::fast_io::intrinsics::addc(xx, xx, false, carry);
	aa = ::fast_io::intrinsics::addc(aa, aa, carry, carry);
	constexpr std::uint_least64_t mask{static_cast<std::uint_least64_t>(1) << 32};
	carry = ((xx & mask) == mask);
	aa = ::fast_io::intrinsics::addc(aa, aa, carry, carry);
}

inline constexpr void ecp_8folds(::fast_io::containers::array<std::byte, 32> &y, field_number const &x) noexcept
{
	if constexpr (::std::same_as<field_number::value_type, ::std::uint_least32_t>)
	{
		/* eight u32 columns, MSB-first */
		std::uint_least32_t xw FAST_IO_INDETERMINATE[field_number::array_size];
		for (::std::size_t i{}; i != field_number::array_size; ++i)
		{
			xw[i] = x.index_unchecked(i);
		}
		for (::std::size_t i{}; i != 32; ++i)
		{
			std::uint_least8_t low{};
			for (::std::size_t w{field_number::array_size}; w--;)
			{
				rl_msb(low, xw[w]);
			}
			y.index_unchecked(i) = static_cast<std::byte>(low);
		}
	}
	else
	{
		std::uint_least64_t x0{x.front_unchecked()}, x1{x.index_unchecked(1)}, x2{x.index_unchecked(2)}, x3{x.back_unchecked()};
		std::uint_least8_t low{};
		for (std::uint_least8_t i{}; i != 32; ++i)
		{
			rl_msbs(low, x3);
			rl_msbs(low, x2);
			rl_msbs(low, x1);
			rl_msbs(low, x0);
			y.index_unchecked(i) = static_cast<std::byte>(low);
		}
	}
}

inline constexpr void ecp_4folds(::fast_io::containers::array<std::byte, 64> &y, field_number const &x) noexcept
{
	if constexpr (::std::same_as<field_number::value_type, ::std::uint_least32_t>)
	{
		/*
		u64 emits four columns of 64 bits; in u32 limbs the high halves
		are limbs 7,5,3,1 and the low halves are 6,4,2,0.
		*/
		std::uint_least32_t xw FAST_IO_INDETERMINATE[field_number::array_size];
		for (::std::size_t i{}; i != field_number::array_size; ++i)
		{
			xw[i] = x.index_unchecked(i);
		}
		for (::std::size_t i{}; i != 32; ++i)
		{
			std::uint_least8_t low{};
			rl_msb(low, xw[7]);
			rl_msb(low, xw[5]);
			rl_msb(low, xw[3]);
			rl_msb(low, xw[1]);
			y.index_unchecked(i) = static_cast<std::byte>(low);
		}
		for (::std::size_t i{}; i != 32; ++i)
		{
			std::uint_least8_t low{};
			rl_msb(low, xw[6]);
			rl_msb(low, xw[4]);
			rl_msb(low, xw[2]);
			rl_msb(low, xw[0]);
			y.index_unchecked(32 + i) = static_cast<std::byte>(low);
		}
	}
	else
	{
		std::uint_least64_t x0{x.front_unchecked()}, x1{x.index_unchecked(1)}, x2{x.index_unchecked(2)}, x3{x.back_unchecked()};
		for (std::uint_least8_t i{}; i != 64; ++i)
		{
			std::uint_least8_t low{};
			rl_msb(low, x3);
			rl_msb(low, x2);
			rl_msb(low, x1);
			rl_msb(low, x0);
			y.index_unchecked(i) = static_cast<std::byte>(low);
		}
	}
}

inline constexpr void base_point_mult(extended_point &s, field_number const &sk, field_number const &r) noexcept
{
	::fast_io::containers::array<std::byte, 32> cut;
	ecp_8folds(cut, sk);
	auto const &p0{folding.index_unchecked(static_cast<char unsigned>(cut.front_unchecked()))};
	field_number_subtraction(s.x, p0.ypx, p0.ymx);
	field_number_addition(s.y, p0.ypx, p0.ymx);
	field_number_multiplication(s.t, p0.t2d, wdi);

	field_number_addition(s.z, r, r);
	field_number_multiplication(s.x, s.x, r);
	field_number_multiplication(s.t, s.t, r);
	field_number_multiplication(s.y, s.y, r);

	for (std::size_t i{1}; i != 32; ++i)
	{
		edp_double_point(s);
		add_affine_point(s, folding.index_unchecked(static_cast<char unsigned>(cut.index_unchecked(i))));
	}
}

inline constexpr void base_point_multiply(affine_point &r, field_number const &sk) noexcept
{
	extended_point s FAST_IO_INDETERMINATE;
	base_point_mult(s, sk, ::fast_io::curve25519::custom_blindings::zr);
	field_number_inverse(s.z, s.z);
	field_multiplication_mod(r.x, s.x, s.z);
	field_multiplication_mod(r.y, s.y, s.z);
}

namespace details
{

/*
Return r = sk*B mapped to Montgomery u coordinate.
u = (1+y)/(1-y) = (Z+Y)/(Z-Y)
*/
inline constexpr void x25519_base_point_multiply(std::byte *r, field_number const &sk) noexcept
{
	extended_point s FAST_IO_INDETERMINATE;
	base_point_mult(s, sk, ::fast_io::curve25519::custom_blindings::zr);
	field_number_addition(s.t, s.z, s.y);
	field_number_subtraction(s.z, s.z, s.y);
	field_number_inverse(s.z, s.z);
	field_multiplication_mod(s.t, s.t, s.z);
	::fast_io::freestanding::type_punning_to_bytes(s.t, r);
}

} // namespace details

/*
P = P + Q where Q is a pre-computed extended point.
r may alias p.
*/
inline constexpr void add_point(extended_point &r, extended_point const &p, precomputed_extended_point const &q) noexcept
{
	field_number a FAST_IO_INDETERMINATE, b FAST_IO_INDETERMINATE, c FAST_IO_INDETERMINATE, d FAST_IO_INDETERMINATE, e FAST_IO_INDETERMINATE;
	field_number_subtraction(a, p.y, p.x);
	field_number_multiplication(a, a, q.ymx);
	field_number_addition(b, p.y, p.x);
	field_number_multiplication(b, b, q.ypx);
	field_number_multiplication(c, p.t, q.t2d);
	field_number_multiplication(d, p.z, q.z2);
	field_number_subtraction(e, b, a);
	field_number_addition(b, b, a);
	field_number_subtraction(a, d, c);
	field_number_addition(d, d, c);
	field_number_multiplication(r.x, e, a);
	field_number_multiplication(r.y, b, d);
	field_number_multiplication(r.t, e, b);
	field_number_multiplication(r.z, d, a);
}

inline constexpr void edp_ext_point_2e(precomputed_extended_point &r, extended_point const &p) noexcept
{
	field_number_addition(r.ypx, p.y, p.x);
	field_number_subtraction(r.ymx, p.y, p.x);
	field_number_multiplication(r.t2d, p.t, w2d);
	field_number_addition(r.z2, p.z, p.z);
}

} // namespace fast_io::curve25519

namespace fast_io::diffie_hellman::details
{

/* Faster alternative to calculate_public_key_to_ptr using the ed25519 base point table */
inline constexpr void calculate_public_key_fast_to_ptr(std::byte *pk, std::byte *sk) noexcept
{
	::fast_io::curve25519::details::x25519_trim_secret_key(::fast_io::containers::index_span<::std::byte, 32>{::fast_io::containers::index_unchecked, sk});
	::fast_io::curve25519::field_number t FAST_IO_INDETERMINATE;
	::fast_io::freestanding::type_punning_from_bytes(sk, t);
	::fast_io::curve25519::details::x25519_base_point_multiply(pk, t);
}

inline constexpr void calculate_public_key_fast(::fast_io::containers::index_span<std::byte, 32> public_key, ::fast_io::containers::index_span<std::byte, 32> secret_key) noexcept
{
	::fast_io::diffie_hellman::details::calculate_public_key_fast_to_ptr(public_key.data(), secret_key.data());
}

} // namespace fast_io::diffie_hellman::details
