#pragma once

/*
RSA operations on DER INTEGER byte strings:

  public  m = s^e mod n  -- verify (and encrypt); e <= 2^64-1
  private s = m^d mod n  -- sign (and decrypt); d is modulus-sized

The private op reuses the same Montgomery square-and-multiply, which is
variable-time -- see the warning on rsa_private_op_to_ptr. There is no
CRT factoring or blinding yet.
*/

namespace fast_io::details::rsa
{

inline constexpr ::std::size_t modulus_max_bytes{1024}; /* 8192-bit */
inline constexpr ::std::size_t exponent_max_bytes{8};   /* e <= 2^64-1, 65537 in practice */
inline constexpr ::std::size_t modulus_max_limbs{modulus_max_bytes / limb_bytes};

struct rsa_verify_context
{
	value_type modulus[modulus_max_limbs];
	value_type r2[modulus_max_limbs];
	value_type n0inv;
	::std::byte exponent[exponent_max_bytes];
	::std::size_t exponent_size;
	::std::size_t limbs_count;
	::std::size_t modulus_bits;
	::std::size_t modulus_bytes;
};

/*
Fill ctx from the DER-encoded modulus/exponent byte strings. Returns
false on malformed input (even modulus, empty or oversized fields).
*/
inline constexpr bool rsa_verify_init_to_ptr(rsa_verify_context &ctx,
											 ::std::byte const *modulus, ::std::size_t modulus_size,
											 ::std::byte const *exponent, ::std::size_t exponent_size) noexcept
{
	while (modulus_size != 0 && *modulus == ::std::byte{})
	{
		++modulus;
		--modulus_size;
	}
	while (exponent_size != 0 && *exponent == ::std::byte{})
	{
		++exponent;
		--exponent_size;
	}
	if (modulus_size == 0 || modulus_size > modulus_max_bytes ||
		exponent_size == 0 || exponent_size > exponent_max_bytes)
	{
		return false;
	}
	::std::size_t const nl{(modulus_size + limb_bytes - 1) / limb_bytes};
	if (!limbs_from_bytes_be(ctx.modulus, modulus, modulus_size, nl))
	{
		return false;
	}
	if ((ctx.modulus[0] & 1u) == 0) /* RSA modulus is odd */
	{
		return false;
	}
	ctx.n0inv = montgomery_n0_inverse(ctx.modulus[0]);
	value_type d FAST_IO_INDETERMINATE[3 * modulus_max_limbs + 1];
	montgomery_r2_setup(ctx.r2, ctx.modulus, nl, d);
	for (::std::size_t i{}; i != exponent_size; ++i)
	{
		ctx.exponent[i] = exponent[i];
	}
	ctx.exponent_size = exponent_size;
	ctx.limbs_count = nl;
	ctx.modulus_bits = limbs_bit_length(ctx.modulus, nl);
	ctx.modulus_bytes = modulus_size;
	return true;
}

/*
RSA private-key context: same Montgomery domain as verify, but the
exponent buffer is modulus-sized -- the private exponent d is
comparable to n in size, unlike e <= 2^64-1.
*/
struct rsa_private_context
{
	value_type modulus[modulus_max_limbs];
	value_type r2[modulus_max_limbs];
	value_type n0inv;
	::std::byte exponent[modulus_max_bytes];
	::std::size_t exponent_size;
	::std::size_t limbs_count;
	::std::size_t modulus_bits;
	::std::size_t modulus_bytes;
};

/*
Fill ctx from DER INTEGER content (n, d) byte strings. Returns false on
malformed input (even modulus, empty or oversized fields).
*/
inline constexpr bool rsa_private_init_to_ptr(rsa_private_context &ctx,
											  ::std::byte const *modulus, ::std::size_t modulus_size,
											  ::std::byte const *exponent, ::std::size_t exponent_size) noexcept
{
	while (modulus_size != 0 && *modulus == ::std::byte{})
	{
		++modulus;
		--modulus_size;
	}
	while (exponent_size != 0 && *exponent == ::std::byte{})
	{
		++exponent;
		--exponent_size;
	}
	if (modulus_size == 0 || modulus_size > modulus_max_bytes ||
		exponent_size == 0 || exponent_size > modulus_max_bytes)
	{
		return false;
	}
	::std::size_t const nl{(modulus_size + limb_bytes - 1) / limb_bytes};
	if (!limbs_from_bytes_be(ctx.modulus, modulus, modulus_size, nl))
	{
		return false;
	}
	if ((ctx.modulus[0] & 1u) == 0) /* RSA modulus is odd */
	{
		return false;
	}
	ctx.n0inv = montgomery_n0_inverse(ctx.modulus[0]);
	value_type d FAST_IO_INDETERMINATE[3 * modulus_max_limbs + 1];
	montgomery_r2_setup(ctx.r2, ctx.modulus, nl, d);
	for (::std::size_t i{}; i != exponent_size; ++i)
	{
		ctx.exponent[i] = exponent[i];
	}
	ctx.exponent_size = exponent_size;
	ctx.limbs_count = nl;
	ctx.modulus_bits = limbs_bit_length(ctx.modulus, nl);
	ctx.modulus_bytes = modulus_size;
	return true;
}

/*
RSASP1: s[0..modulus_bytes) = em^d mod n, big-endian. scratch holds
5*nl+2 limbs. Returns false when em does not fit the modulus or is >= n.

SIDE CHANNEL WARNING: montgomery_pow is variable-time -- the exponent
bits steer a square/multiply branch. For the TLS 1.3 server
CertificateVerify this is a single offline signature per connection, but
a constant-time CRT implementation with blinding belongs here before
this is used where timing or cache observation of d matters.
*/
inline constexpr bool rsa_private_op_to_ptr(::std::byte *s, rsa_private_context const &ctx,
											::std::byte const *em, ::std::size_t em_size,
											value_type *scratch) noexcept
{
	::std::size_t const nl{ctx.limbs_count};
	if (em_size == 0 || em_size > ctx.modulus_bytes)
	{
		return false;
	}
	value_type m FAST_IO_INDETERMINATE[modulus_max_limbs];
	if (!limbs_from_bytes_be(m, em, em_size, nl))
	{
		return false;
	}
	if (!limbs_less(m, ctx.modulus, nl))
	{
		return false;
	}
	montgomery_pow(m, m, ctx.exponent, ctx.exponent_size,
				   ctx.modulus, ctx.r2, ctx.n0inv, nl, scratch);
	limbs_to_bytes_be(s, m, ctx.modulus_bytes, nl);
	return true;
}

/*
RSAVP1: em[0..modulus_bytes) = signature^exponent mod modulus,
big-endian. scratch holds 5*nl+2 limbs. Returns false when the
signature does not fit the modulus size or is >= n.
*/
inline constexpr bool rsa_public_op_to_ptr(::std::byte *em, rsa_verify_context const &ctx,
										   ::std::byte const *signature, ::std::size_t signature_size,
										   value_type *scratch) noexcept
{
	::std::size_t const nl{ctx.limbs_count};
	if (signature_size == 0 || signature_size > ctx.modulus_bytes)
	{
		return false;
	}
	value_type s FAST_IO_INDETERMINATE[modulus_max_limbs];
	if (!limbs_from_bytes_be(s, signature, signature_size, nl))
	{
		return false;
	}
	if (!limbs_less(s, ctx.modulus, nl))
	{
		return false;
	}
	montgomery_pow(s, s, ctx.exponent, ctx.exponent_size,
				   ctx.modulus, ctx.r2, ctx.n0inv, nl, scratch);
	limbs_to_bytes_be(em, s, ctx.modulus_bytes, nl);
	return true;
}

} // namespace fast_io::details::rsa

namespace fast_io
{

class rsa
{
public:
	using verify_context = ::fast_io::details::rsa::rsa_verify_context;
	using private_context = ::fast_io::details::rsa::rsa_private_context;

	static inline constexpr ::std::size_t modulus_max_bytes{::fast_io::details::rsa::modulus_max_bytes};
	static inline constexpr ::std::size_t exponent_max_bytes{::fast_io::details::rsa::exponent_max_bytes};

	static inline constexpr bool verify_init_to_ptr(verify_context &ctx,
													::std::byte const *modulus, ::std::size_t modulus_size,
													::std::byte const *exponent, ::std::size_t exponent_size) noexcept
	{
		return ::fast_io::details::rsa::rsa_verify_init_to_ptr(ctx, modulus, modulus_size, exponent, exponent_size);
	}

	static inline constexpr bool verify_init(verify_context &ctx,
											 ::fast_io::containers::span<::std::byte const> modulus,
											 ::fast_io::containers::span<::std::byte const> exponent) noexcept
	{
		return verify_init_to_ptr(ctx, modulus.data(), modulus.size(), exponent.data(), exponent.size());
	}

	/* raw RSAVP1: em receives modulus_bytes big-endian bytes of s^e mod n */
	static inline constexpr bool public_op_to_ptr(::std::byte *em, verify_context const &ctx,
												  ::std::byte const *signature, ::std::size_t signature_size) noexcept
	{
		::fast_io::details::rsa::value_type scratch
			FAST_IO_INDETERMINATE[5 * ::fast_io::details::rsa::modulus_max_limbs + 2];
		return ::fast_io::details::rsa::rsa_public_op_to_ptr(em, ctx, signature, signature_size, scratch);
	}

	/*
	RSASSA-PKCS1-v1_5 verify. The DigestInfo prefix selects the hash;
	the template overload picks it from the hash context type.
	*/
	static inline constexpr bool verify_pkcs1v15_to_ptr(verify_context const &ctx,
														::std::byte const *signature, ::std::size_t signature_size,
														::std::byte const *digest_info_prefix, ::std::size_t prefix_size,
														::std::byte const *digest, ::std::size_t digest_size) noexcept
	{
		::std::byte em FAST_IO_INDETERMINATE[modulus_max_bytes];
		if (!public_op_to_ptr(em, ctx, signature, signature_size))
		{
			return false;
		}
		return ::fast_io::details::rsa::emsa_pkcs1v15_check(em, ctx.modulus_bytes,
															digest_info_prefix, prefix_size, digest, digest_size);
	}

	template <typename hasher>
	static inline constexpr bool verify_pkcs1v15_to_ptr(verify_context const &ctx,
														::std::byte const *signature, ::std::size_t signature_size,
														::std::byte const *digest) noexcept
	{
		constexpr auto &prefix{::fast_io::details::rsa::pkcs1v15_digest_info<hasher>::prefix};
		return verify_pkcs1v15_to_ptr(ctx, signature, signature_size,
									  prefix, sizeof(prefix), digest, hasher::digest_size);
	}

	/* RSASSA-PSS verify; salt_size per RFC 8017 (TLS 1.3 uses hasher::digest_size). */
	template <typename hasher>
	static inline constexpr bool verify_pss_to_ptr(verify_context const &ctx,
												   ::std::byte const *signature, ::std::size_t signature_size,
												   ::std::byte const *mhash, ::std::size_t salt_size) noexcept
	{
		::std::byte em FAST_IO_INDETERMINATE[modulus_max_bytes];
		if (!public_op_to_ptr(em, ctx, signature, signature_size))
		{
			return false;
		}
		::std::size_t const emlen{(ctx.modulus_bits + 6) / 8}; /* ceil((modBits-1)/8) */
		::std::size_t const off{ctx.modulus_bytes - emlen};
		for (::std::size_t i{}; i != off; ++i) /* I2OSP must not drop nonzero bytes */
		{
			if (em[i] != ::std::byte{})
			{
				return false;
			}
		}
		return ::fast_io::details::rsa::emsa_pss_verify<hasher>(em + off, emlen, ctx.modulus_bits - 1,
																mhash, salt_size);
	}

	/* ---------------- signing ---------------- */

	static inline constexpr bool private_init_to_ptr(private_context &ctx,
													 ::std::byte const *modulus, ::std::size_t modulus_size,
													 ::std::byte const *exponent, ::std::size_t exponent_size) noexcept
	{
		return ::fast_io::details::rsa::rsa_private_init_to_ptr(ctx, modulus, modulus_size, exponent, exponent_size);
	}

	static inline constexpr bool private_init(private_context &ctx,
											  ::fast_io::containers::span<::std::byte const> modulus,
											  ::fast_io::containers::span<::std::byte const> exponent) noexcept
	{
		return private_init_to_ptr(ctx, modulus.data(), modulus.size(), exponent.data(), exponent.size());
	}

	/* raw RSASP1: s receives modulus_bytes big-endian bytes of em^d mod n.
	   VARIABLE-TIME -- see rsa_private_op_to_ptr. */
	static inline constexpr bool private_op_to_ptr(::std::byte *s, private_context const &ctx,
												   ::std::byte const *em, ::std::size_t em_size) noexcept
	{
		::fast_io::details::rsa::value_type scratch
			FAST_IO_INDETERMINATE[5 * ::fast_io::details::rsa::modulus_max_limbs + 2];
		return ::fast_io::details::rsa::rsa_private_op_to_ptr(s, ctx, em, em_size, scratch);
	}

	/*
	RSASSA-PSS sign (rfc8017 8.1.1 + 9.1.1): EMSA-PSS-ENCODE the message
	hash, then the private op. salt is caller-supplied entropy of
	salt_size bytes (TLS 1.3: salt_size == hasher::digest_size).
	sig_out receives ctx.modulus_bytes.
	*/
	template <typename hasher>
	static inline constexpr bool sign_pss_to_ptr(::std::byte *sig_out, private_context const &ctx,
												 ::std::byte const *mhash,
												 ::std::byte const *salt, ::std::size_t salt_size) noexcept
	{
		::std::size_t const emlen{(ctx.modulus_bits + 6) / 8};
		::std::byte em FAST_IO_INDETERMINATE[modulus_max_bytes];
		if (!::fast_io::details::rsa::emsa_pss_encode<hasher>(em, emlen, ctx.modulus_bits - 1,
															mhash, salt, salt_size))
		{
			return false;
		}
		return private_op_to_ptr(sig_out, ctx, em, emlen);
	}
};

} // namespace fast_io
