#pragma once

namespace fast_io::curve25519
{

/*
Default blinding context.  Generated once for a fixed internal seed by
src/generate_default_blinding.cc; bp = -bl*B holds so that for any
scalar sk, (sk+bl)*B + bp = sk*B.
*/
inline constexpr edp_blinding_context default_blinding{
	field_number_from_u64(0xe319c5e2e91917c2ULL, 0x0f25cfbcafe30c0aULL, 0x64e218a30ba1e198ULL, 0x08ba065b737c6d8fULL),
	field_number_from_u64(0xbc0bf373210c75b1ULL, 0x2b0f08392ebd0a04ULL, 0x982b1498f84e78c2ULL, 0xbe8c10169eb775bcULL),
	{field_number_from_u64(0x36bc52267bdeeca0ULL, 0x76a4f80c7eceb9a9ULL, 0xd528c977e9aee2c9ULL, 0xe7323079ebdfca4fULL),
	 field_number_from_u64(0x4ffb48a56423d116ULL, 0x7124ba3dd2497a51ULL, 0x0585388a8dcdd182ULL, 0xdd0f7d0aeff6fbddULL),
	 field_number_from_u64(0xe820f4f685949928ULL, 0x707c614e16efaf6bULL, 0x981c2357c00efe26ULL, 0xf60cb134bb2b35bdULL),
	 field_number_from_u64(0x4f59fb2afbf7b10aULL, 0xea0b4e1de2650398ULL, 0x0e22d48a421f4d9cULL, 0xcb7fd2170d1a4888ULL)},
};

/*
S = sk*B with scalar + projective blinding.
S = (sk+bc.bl)*B + bc.bp = sk*B.
*/
inline constexpr void blinded_base_point_mult(extended_point &s, field_number const &sk, edp_blinding_context const &bc) noexcept
{
	field_number t FAST_IO_INDETERMINATE;
	eco_add_reduce(t, sk, bc.bl);
	base_point_mult(s, t, bc.zr);
	add_point(s, s, bc.bp);
}

inline constexpr void base_point_multiply(affine_point &r, field_number const &sk, edp_blinding_context const &bc) noexcept
{
	extended_point s FAST_IO_INDETERMINATE;
	blinded_base_point_mult(s, sk, bc);
	field_number_inverse(s.z, s.z);
	field_multiplication_mod(r.x, s.x, s.z);
	field_multiplication_mod(r.y, s.y, s.z);
}

namespace details
{

/*
r = sk*B mapped to Montgomery u coordinate, with blinding.
u = (1+y)/(1-y) = (Z+Y)/(Z-Y)
*/
inline constexpr void x25519_base_point_multiply_with_blinding(std::byte *r, field_number const &sk, edp_blinding_context const &bc) noexcept
{
	extended_point s FAST_IO_INDETERMINATE;
	blinded_base_point_mult(s, sk, bc);
	field_number_addition(s.t, s.z, s.y);
	field_number_subtraction(s.z, s.z, s.y);
	field_number_inverse(s.z, s.z);
	field_multiplication_mod(s.t, s.t, s.z);
	::fast_io::freestanding::type_punning_to_bytes(s.t, r);
}

/*
Derive a blinding context from seed bytes:
  digest = SHA512(le(default_blinding.zr) || seed)
  ctx.zr = digest[32:64]                      (raw field element)
  t      = digest[0:32] mod BPO
  ctx.bl = BPO - t                            (= -t mod BPO)
  ctx.bp = t*B                                (= -bl*B)
The bp point is computed through default_blinding itself so the
generation scalar multiply is also blinded.
*/
inline constexpr edp_blinding_context &ed25519_blinding_init_to_ptr(edp_blinding_context &ctx, std::byte const *seed, std::size_t seed_size) noexcept
{
	::fast_io::sha512_context H;
	::fast_io::containers::array<std::byte, 32> zb FAST_IO_INDETERMINATE;
	::fast_io::containers::array<std::byte, ::fast_io::sha512_context::digest_size> digest FAST_IO_INDETERMINATE;
	for (std::size_t i{}; i != field_number::array_size; ++i)
	{
		limb_to_bytes_little_endian(zb.data() + i * sizeof(field_number::value_type), default_blinding.zr.index_unchecked(i));
	}
	H.update(zb.data(), zb.data() + zb.size());
	H.update(seed, seed + seed_size);
	H.do_final();
	H.digest_to_byte_ptr(digest.data());

	field_number t FAST_IO_INDETERMINATE, g FAST_IO_INDETERMINATE;
	for (std::size_t i{}; i != field_number::array_size; ++i)
	{
		t.index_unchecked(i) = bytes_to_limb_little_endian(digest.data() + i * sizeof(field_number::value_type));
		ctx.zr.index_unchecked(i) = bytes_to_limb_little_endian(digest.data() + 32 + i * sizeof(field_number::value_type));
	}
	eco_mod(t);
	subtraction_u256(ctx.bl, NxBPOraw.index_unchecked(1), t); /* bl = BPO - t */

	/* ctx.bp = t*B computed under the default context: (t+bl_d)*B + bp_d = t*B */
	extended_point T FAST_IO_INDETERMINATE;
	eco_add_reduce(g, t, default_blinding.bl);
	base_point_mult(T, g, default_blinding.zr);
	add_point(T, T, default_blinding.bp);
	edp_ext_point_2e(ctx.bp, T);

	// Clear sensitive data. Not needed (nor possible) during constant evaluation.
	FAST_IO_IF_NOT_CONSTEVAL
	{
		::fast_io::secure_clear(__builtin_addressof(t), sizeof(t));
		::fast_io::secure_clear(__builtin_addressof(g), sizeof(g));
		::fast_io::secure_clear(zb.data(), zb.size_bytes());
		::fast_io::secure_clear(digest.data(), digest.size_bytes());
	}
	return ctx;
}

inline constexpr edp_blinding_context &ed25519_blinding_init(edp_blinding_context &ctx, ::fast_io::containers::span<std::byte const> seed) noexcept
{
	return ed25519_blinding_init_to_ptr(ctx, seed.data(), seed.size());
}

/*
Derive a blinding context by reading a 64-byte seed from an input stream.
*/
template <typename instmtype>
inline edp_blinding_context &ed25519_blinding_init_from_input_stream(edp_blinding_context &ctx, instmtype &&instm)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::defines::input_stream_operations_nothrow<instmtype>)
{
	::fast_io::containers::array<std::byte, 64> seed FAST_IO_INDETERMINATE;
	::fast_io::operations::read_all_bytes(instm, seed.data(), seed.size());
	ed25519_blinding_init_to_ptr(ctx, seed.data(), seed.size());
	::fast_io::secure_clear(seed.data(), seed.size_bytes());
	return ctx;
}

} // namespace details

} // namespace fast_io::curve25519

namespace fast_io::diffie_hellman::details
{

/* Faster alternative to calculate_public_key_to_ptr using the ed25519 base point table, with blinding */
inline constexpr void calculate_public_key_fast_to_ptr_with_blinding(std::byte *pk, std::byte *sk, ::fast_io::curve25519::edp_blinding_context const &blinding) noexcept
{
	::fast_io::curve25519::details::x25519_trim_secret_key(::fast_io::containers::index_span<::std::byte, 32>{::fast_io::containers::index_unchecked, sk});
	::fast_io::curve25519::field_number t FAST_IO_INDETERMINATE;
	::fast_io::freestanding::type_punning_from_bytes(sk, t);
	::fast_io::curve25519::details::x25519_base_point_multiply_with_blinding(pk, t, blinding);
}

inline constexpr void calculate_public_key_fast_with_blinding(::fast_io::containers::index_span<std::byte, 32> public_key, ::fast_io::containers::index_span<std::byte, 32> secret_key, ::fast_io::curve25519::edp_blinding_context const &blinding) noexcept
{
	::fast_io::diffie_hellman::details::calculate_public_key_fast_to_ptr_with_blinding(public_key.data(), secret_key.data(), blinding);
}

} // namespace fast_io::diffie_hellman::details
