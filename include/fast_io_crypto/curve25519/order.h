#pragma once

namespace fast_io::curve25519
{

/*
Mod BPO (Base Point Order) operations

  BPO = 2**252 + 27742317777372353535851937790883648493
  BPO = 0x1000000000000000000000000000000014DEF9DEA2F79CD65812631A5CF5D3ED
*/

inline constexpr ::fast_io::containers::array<::fast_io::curve25519::field_number, 16> NxBPOraw{{
	::fast_io::curve25519::field_number_from_u64(0x0, 0x0, 0x0, 0x0),
	::fast_io::curve25519::field_number_from_u64(0x5812631a5cf5d3ed, 0x14def9dea2f79cd6, 0x0, 0x1000000000000000),
	::fast_io::curve25519::field_number_from_u64(0xb024c634b9eba7da, 0x29bdf3bd45ef39ac, 0x0, 0x2000000000000000),
	::fast_io::curve25519::field_number_from_u64(0x837294f16e17bc7, 0x3e9ced9be8e6d683, 0x0, 0x3000000000000000),
	::fast_io::curve25519::field_number_from_u64(0x60498c6973d74fb4, 0x537be77a8bde7359, 0x0, 0x4000000000000000),
	::fast_io::curve25519::field_number_from_u64(0xb85bef83d0cd23a1, 0x685ae1592ed6102f, 0x0, 0x5000000000000000),
	::fast_io::curve25519::field_number_from_u64(0x106e529e2dc2f78e, 0x7d39db37d1cdad06, 0x0, 0x6000000000000000),
	::fast_io::curve25519::field_number_from_u64(0x6880b5b88ab8cb7b, 0x9218d51674c549dc, 0x0, 0x7000000000000000),
	::fast_io::curve25519::field_number_from_u64(0xc09318d2e7ae9f68, 0xa6f7cef517bce6b2, 0x0, 0x8000000000000000),
	::fast_io::curve25519::field_number_from_u64(0x18a57bed44a47355, 0xbbd6c8d3bab48389, 0x0, 0x9000000000000000),
	::fast_io::curve25519::field_number_from_u64(0x70b7df07a19a4742, 0xd0b5c2b25dac205f, 0x0, 0xa000000000000000),
	::fast_io::curve25519::field_number_from_u64(0xc8ca4221fe901b2f, 0xe594bc9100a3bd35, 0x0, 0xb000000000000000),
	::fast_io::curve25519::field_number_from_u64(0x20dca53c5b85ef1c, 0xfa73b66fa39b5a0c, 0x0, 0xc000000000000000),
	::fast_io::curve25519::field_number_from_u64(0x78ef0856b87bc309, 0xf52b04e4692f6e2, 0x1, 0xd000000000000000),
	::fast_io::curve25519::field_number_from_u64(0xd1016b71157196f6, 0x2431aa2ce98a93b8, 0x1, 0xe000000000000000),
	::fast_io::curve25519::field_number_from_u64(0x2913ce8b72676ae3, 0x3910a40b8c82308f, 0x1, 0xf000000000000000),
}};

/* X mod BPO */
inline constexpr void eco_mod(::fast_io::curve25519::field_number &x) noexcept
{
	using unsigned_type = ::fast_io::curve25519::field_number::value_type;
	unsigned_type const c{subtraction_u256(x, x, NxBPOraw.index_unchecked(x.back_unchecked() >> (::std::numeric_limits<unsigned_type>::digits - 4)))};
	constexpr unsigned_type zero{};
	addition_u256_discard(x, x, NxBPOraw.index_unchecked(zero - c));
}

/* T = X*Y (512-bit, no reduction) */
inline constexpr void multiplication_u256(::fast_io::curve25519::field_number::value_type *t, ::fast_io::curve25519::field_number const &x, ::fast_io::curve25519::field_number const &y) noexcept
{
	if constexpr (::std::same_as<::fast_io::curve25519::field_number::value_type, ::std::uint_least32_t>)
	{
		multiply_single<0, true>(t, x, y.index_unchecked(0));
		multiply_single<1>(t, x, y.index_unchecked(1));
		multiply_single<2>(t, x, y.index_unchecked(2));
		multiply_single<3>(t, x, y.index_unchecked(3));
		multiply_single<4>(t, x, y.index_unchecked(4));
		multiply_single<5>(t, x, y.index_unchecked(5));
		multiply_single<6>(t, x, y.index_unchecked(6));
		multiply_single<7>(t, x, y.index_unchecked(7));
	}
	else
	{
		multiply_single<0, true>(t, x, y.front_unchecked());
		multiply_single<1>(t, x, y.index_unchecked(1));
		multiply_single<2>(t, x, y.index_unchecked(2));
		multiply_single<3>(t, x, y.back_unchecked());
	}
}

/*
Calculate: Y = [b:X] mod BPO
For R = 2^256, we calculate Y = b*R + X mod BPO.
Since -R mod BPO is only 129-bits, it reduces number of multiplications if
we calculate: Y = X - b*(-R) mod BPO instead.
-R mod BPO = {0x812631A5CF5D3ED0, 0x4DEF9DEA2F79CD65, 1, 0}

X is any indexable view of the ::fast_io::curve25519::field_number limb width.
*/
inline constexpr void eco_reduce_hi_word(::fast_io::curve25519::field_number &y, ::fast_io::curve25519::field_number::value_type b, ::fast_io::curve25519::field_number::value_type const *x) noexcept
{
	using unsigned_type = ::fast_io::curve25519::field_number::value_type;
	constexpr unsigned_type zero{};
	constexpr ::std::size_t n{::fast_io::curve25519::field_number::array_size};
	if constexpr (::std::same_as<unsigned_type, ::std::uint_least32_t>)
	{
		/* -R mod BPO in u32 limbs (nonzero through limb 4) */
		constexpr unsigned_type minus_r[5]{0xCF5D3ED0u, 0x812631A5u, 0x2F79CD65u, 0x4DEF9DEAu, 1u};
		/* BPO in u32 limbs */
		constexpr unsigned_type bpo[8]{0x5CF5D3EDu, 0x5812631Au, 0xA2F79CD6u, 0x14DEF9DEu, 0u, 0u, 0u, 0x10000000u};

		unsigned_type lo FAST_IO_INDETERMINATE[5];
		unsigned_type hi FAST_IO_INDETERMINATE[5];
		for (::std::size_t i{}; i != 5; ++i)
		{
			lo[i] = ::fast_io::intrinsics::umul(b, minus_r[i], hi[i]);
		}
		unsigned_type p FAST_IO_INDETERMINATE[6];
		bool carry{};
		p[0] = lo[0];
		for (::std::size_t i{1}; i != 5; ++i)
		{
			p[i] = ::fast_io::intrinsics::addc(lo[i], hi[i - 1], carry, carry);
		}
		p[5] = ::fast_io::intrinsics::addc(zero, hi[4], carry, carry);

		bool borrow{};
		for (::std::size_t i{}; i != 6; ++i)
		{
			y.index_unchecked(i) = ::fast_io::intrinsics::subc(x[i], p[i], borrow, borrow);
		}
		y.index_unchecked(6) = ::fast_io::intrinsics::subc(x[6], zero, borrow, borrow);
		y.index_unchecked(7) = ::fast_io::intrinsics::subc(x[7], zero, borrow, borrow);

		/* Add BPO if there is a borrow */
		unsigned_type mask{::fast_io::intrinsics::subc(zero, zero, borrow, borrow)};
		carry = false;
		for (::std::size_t i{}; i != n; ++i)
		{
			y.index_unchecked(i) = ::fast_io::intrinsics::addc(y.index_unchecked(i), mask & bpo[i], carry, carry);
		}
		return;
	}
	else
	{
	constexpr std::uint_least64_t minus_r0{0x812631A5CF5D3ED0ULL};
	constexpr std::uint_least64_t minus_r1{0x4DEF9DEA2F79CD65ULL};
	constexpr std::uint_least64_t zero64{};

	std::uint_least64_t const bw{static_cast<std::uint_least64_t>(b)};
	std::uint_least64_t t1;
	std::uint_least64_t t0{::fast_io::intrinsics::umul(bw, minus_r0, t1)};
	std::uint_least64_t t2;
	std::uint_least64_t lo1{::fast_io::intrinsics::umul(bw, minus_r1, t2)};
	bool carry{};
	t1 = ::fast_io::intrinsics::addc(t1, lo1, false, carry);
	t2 = ::fast_io::intrinsics::addc(t2, bw, carry, carry);
	std::uint_least64_t t3{::fast_io::intrinsics::addc(zero64, zero64, carry, carry)};

	bool borrow{};
	std::uint_least64_t const x0{static_cast<std::uint_least64_t>(x[0])};
	std::uint_least64_t const x1{static_cast<std::uint_least64_t>(x[1])};
	std::uint_least64_t const x2{static_cast<std::uint_least64_t>(x[2])};
	std::uint_least64_t const x3{static_cast<std::uint_least64_t>(x[3])};
	y.front_unchecked() = static_cast<unsigned_type>(::fast_io::intrinsics::subc(x0, t0, false, borrow));
	y.index_unchecked(1) = static_cast<unsigned_type>(::fast_io::intrinsics::subc(x1, t1, borrow, borrow));
	y.index_unchecked(2) = static_cast<unsigned_type>(::fast_io::intrinsics::subc(x2, t2, borrow, borrow));
	y.index_unchecked(3) = static_cast<unsigned_type>(::fast_io::intrinsics::subc(x3, t3, borrow, borrow));

	/* Add BPO if there is a borrow */
	std::uint_least64_t mask{::fast_io::intrinsics::subc(zero64, zero64, borrow, borrow)};
	y.front_unchecked() = static_cast<unsigned_type>(::fast_io::intrinsics::addc(static_cast<std::uint_least64_t>(y.front_unchecked()), mask & static_cast<std::uint_least64_t>(0x5812631A5CF5D3EDULL), false, carry));
	y.index_unchecked(1) = static_cast<unsigned_type>(::fast_io::intrinsics::addc(static_cast<std::uint_least64_t>(y.index_unchecked(1)), mask & static_cast<std::uint_least64_t>(0x14DEF9DEA2F79CD6ULL), carry, carry));
	y.index_unchecked(2) = static_cast<unsigned_type>(::fast_io::intrinsics::addc(static_cast<std::uint_least64_t>(y.index_unchecked(2)), zero64, carry, carry));
	y.index_unchecked(3) = static_cast<unsigned_type>(::fast_io::intrinsics::addc(static_cast<std::uint_least64_t>(y.index_unchecked(3)), mask & static_cast<std::uint_least64_t>(0x1000000000000000ULL), carry, carry));
	}
}

/*
In-place variant: reduce words t[off..off+N-1] in place using b = t[off+N].
Equivalent to eco_reduce_hi_word(t+off, t[off+N], t+off).
*/
template <std::size_t off>
inline constexpr void eco_reduce_hi_word_inplace(::fast_io::curve25519::field_number::value_type *t) noexcept
{
	using unsigned_type = ::fast_io::curve25519::field_number::value_type;
	constexpr unsigned_type zero{};
	constexpr ::std::size_t n{::fast_io::curve25519::field_number::array_size};
	if constexpr (::std::same_as<unsigned_type, ::std::uint_least32_t>)
	{
		constexpr unsigned_type minus_r[5]{0xCF5D3ED0u, 0x812631A5u, 0x2F79CD65u, 0x4DEF9DEAu, 1u};
		constexpr unsigned_type bpo[8]{0x5CF5D3EDu, 0x5812631Au, 0xA2F79CD6u, 0x14DEF9DEu, 0u, 0u, 0u, 0x10000000u};

		unsigned_type const b{t[off + n]};
		unsigned_type lo FAST_IO_INDETERMINATE[5];
		unsigned_type hi FAST_IO_INDETERMINATE[5];
		for (::std::size_t i{}; i != 5; ++i)
		{
			lo[i] = ::fast_io::intrinsics::umul(b, minus_r[i], hi[i]);
		}
		unsigned_type p FAST_IO_INDETERMINATE[6];
		bool carry{};
		p[0] = lo[0];
		for (::std::size_t i{1}; i != 5; ++i)
		{
			p[i] = ::fast_io::intrinsics::addc(lo[i], hi[i - 1], carry, carry);
		}
		p[5] = ::fast_io::intrinsics::addc(zero, hi[4], carry, carry);

		bool borrow{};
		for (::std::size_t i{}; i != 6; ++i)
		{
			t[off + i] = ::fast_io::intrinsics::subc(t[off + i], p[i], borrow, borrow);
		}
		for (::std::size_t i{6}; i != n; ++i)
		{
			t[off + i] = ::fast_io::intrinsics::subc(t[off + i], zero, borrow, borrow);
		}

		/* Add BPO if there is a borrow */
		unsigned_type mask{::fast_io::intrinsics::subc(zero, zero, borrow, borrow)};
		carry = false;
		for (::std::size_t i{}; i != n; ++i)
		{
			t[off + i] = ::fast_io::intrinsics::addc(t[off + i], mask & bpo[i], carry, carry);
		}
		return;
	}
	else
	{
		constexpr std::uint_least64_t minus_r0{0x812631A5CF5D3ED0ULL};
		constexpr std::uint_least64_t minus_r1{0x4DEF9DEA2F79CD65ULL};
		constexpr std::uint_least64_t zero64{};

		std::uint_least64_t const b{static_cast<std::uint_least64_t>(t[off + 4])};
		std::uint_least64_t t1;
		std::uint_least64_t t0{::fast_io::intrinsics::umul(b, minus_r0, t1)};
		std::uint_least64_t t2;
		std::uint_least64_t lo1{::fast_io::intrinsics::umul(b, minus_r1, t2)};
		bool carry{};
		t1 = ::fast_io::intrinsics::addc(t1, lo1, false, carry);
		t2 = ::fast_io::intrinsics::addc(t2, b, carry, carry);
		std::uint_least64_t t3{::fast_io::intrinsics::addc(zero64, zero64, carry, carry)};

		bool borrow{};
		t[off] = static_cast<unsigned_type>(::fast_io::intrinsics::subc(static_cast<std::uint_least64_t>(t[off]), t0, false, borrow));
		t[off + 1] = static_cast<unsigned_type>(::fast_io::intrinsics::subc(static_cast<std::uint_least64_t>(t[off + 1]), t1, borrow, borrow));
		t[off + 2] = static_cast<unsigned_type>(::fast_io::intrinsics::subc(static_cast<std::uint_least64_t>(t[off + 2]), t2, borrow, borrow));
		t[off + 3] = static_cast<unsigned_type>(::fast_io::intrinsics::subc(static_cast<std::uint_least64_t>(t[off + 3]), t3, borrow, borrow));

		/* Add BPO if there is a borrow */
		std::uint_least64_t mask{::fast_io::intrinsics::subc(zero64, zero64, borrow, borrow)};
		t[off] = static_cast<unsigned_type>(::fast_io::intrinsics::addc(static_cast<std::uint_least64_t>(t[off]), mask & static_cast<std::uint_least64_t>(0x5812631A5CF5D3EDULL), false, carry));
		t[off + 1] = static_cast<unsigned_type>(::fast_io::intrinsics::addc(static_cast<std::uint_least64_t>(t[off + 1]), mask & static_cast<std::uint_least64_t>(0x14DEF9DEA2F79CD6ULL), carry, carry));
		t[off + 2] = static_cast<unsigned_type>(::fast_io::intrinsics::addc(static_cast<std::uint_least64_t>(t[off + 2]), zero64, carry, carry));
		t[off + 3] = static_cast<unsigned_type>(::fast_io::intrinsics::addc(static_cast<std::uint_least64_t>(t[off + 3]), mask & static_cast<std::uint_least64_t>(0x1000000000000000ULL), carry, carry));
	}
}

/* Z = X*Y mod BPO */
inline constexpr void eco_mul_reduce(::fast_io::curve25519::field_number &z, ::fast_io::curve25519::field_number const &x, ::fast_io::curve25519::field_number const &y) noexcept
{
	::fast_io::curve25519::field_number::value_type t FAST_IO_INDETERMINATE[::fast_io::curve25519::field_number::array_size * 2];
	multiplication_u256(t, x, y);
	if constexpr (::std::same_as<::fast_io::curve25519::field_number::value_type, ::std::uint_least32_t>)
	{
		eco_reduce_hi_word_inplace<7>(t);
		eco_reduce_hi_word_inplace<6>(t);
		eco_reduce_hi_word_inplace<5>(t);
		eco_reduce_hi_word_inplace<4>(t);
		eco_reduce_hi_word_inplace<3>(t);
		eco_reduce_hi_word_inplace<2>(t);
		eco_reduce_hi_word_inplace<1>(t);
		eco_reduce_hi_word(z, t[8], t);
	}
	else
	{
		eco_reduce_hi_word_inplace<3>(t);
		eco_reduce_hi_word_inplace<2>(t);
		eco_reduce_hi_word_inplace<1>(t);
		eco_reduce_hi_word(z, t[4], t);
	}
}

/* Z = X + Y mod BPO */
inline constexpr void eco_add_reduce(::fast_io::curve25519::field_number &z, ::fast_io::curve25519::field_number const &x, ::fast_io::curve25519::field_number const &y) noexcept
{
	::fast_io::curve25519::field_number::value_type const c{addition_u256(z, x, y)};
	eco_reduce_hi_word(z, c, z.content);
}

inline constexpr ::fast_io::curve25519::field_number::value_type bytes_to_limb_little_endian(std::byte const *p) noexcept
{
	::fast_io::curve25519::field_number::value_type v FAST_IO_INDETERMINATE;
	::fast_io::freestanding::type_punning_from_bytes(p, v);
	if constexpr (::std::endian::native == ::std::endian::big)
	{
		v = ::fast_io::byte_swap(v);
	}
	return v;
}

inline constexpr void limb_to_bytes_little_endian(std::byte *p, ::fast_io::curve25519::field_number::value_type v) noexcept
{
	if constexpr (::std::endian::native == ::std::endian::big)
	{
		v = ::fast_io::byte_swap(v);
	}
	::fast_io::freestanding::type_punning_to_bytes(v, p);
}

/* Y = D mod BPO where D is a 512-bit little-endian byte array (i.e SHA512 digest) */
inline constexpr void eco_digest_to_words(::fast_io::curve25519::field_number &y, std::byte const *md) noexcept
{
	constexpr ::std::size_t n{::fast_io::curve25519::field_number::array_size};
	::fast_io::curve25519::field_number::value_type t FAST_IO_INDETERMINATE[::fast_io::curve25519::field_number::array_size * 2];
	for (::std::size_t i{}; i != n * 2; ++i)
	{
		t[i] = bytes_to_limb_little_endian(md + i * sizeof(::fast_io::curve25519::field_number::value_type));
	}
	if constexpr (::std::same_as<::fast_io::curve25519::field_number::value_type, ::std::uint_least32_t>)
	{
		eco_reduce_hi_word_inplace<7>(t);
		eco_reduce_hi_word_inplace<6>(t);
		eco_reduce_hi_word_inplace<5>(t);
		eco_reduce_hi_word_inplace<4>(t);
		eco_reduce_hi_word_inplace<3>(t);
		eco_reduce_hi_word_inplace<2>(t);
		eco_reduce_hi_word_inplace<1>(t);
		eco_reduce_hi_word(y, t[8], t);
	}
	else
	{
		eco_reduce_hi_word_inplace<3>(t);
		eco_reduce_hi_word_inplace<2>(t);
		eco_reduce_hi_word_inplace<1>(t);
		eco_reduce_hi_word(y, t[4], t);
	}
}

} // namespace fast_io::curve25519
