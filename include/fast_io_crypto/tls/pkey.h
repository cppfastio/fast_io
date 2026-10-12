#pragma once

/*
TLS server private key: DER parsing for the key types the
CertificateVerify signer understands, plus the sign dispatch itself.

Accepted key forms:

  PKCS#8 PrivateKeyInfo  -- rsaEncryption / rsassaPss (inner PKCS#1),
							ed25519 (RFC8410 CurvePrivateKey)
  PKCS#1 RSAPrivateKey   -- bare "RSA PRIVATE KEY" DER
  raw ed25519            -- 64-byte expanded (seed || public key) or
							32-byte seed (expanded internally)

RSA signs via the private operation m^d mod n over an EMSA-PSS
encoding -- see the variable-time warning on rsa_private_op_to_ptr.
ed25519 signs deterministic RFC8032 signatures.
*/

namespace fast_io::tls::details
{

enum class tls_pkey_kind : ::std::uint_least8_t
{
	none,
	rsa,
	ed25519,
	ec
};

struct tls_pkey
{
	tls_pkey_kind kind{tls_pkey_kind::none};
	/* rsa: spans into the DER input */
	::std::byte const *rsa_modulus{};
	::std::size_t rsa_modulus_size{};
	::std::byte const *rsa_exponent{}; /* private exponent d */
	::std::size_t rsa_exponent_size{};
	/* ed25519: the expanded seed || public key (64-byte fast_io form) */
	::std::byte ed25519_key[64]{};
	/* ec: the scalar d and the curve it belongs to */
	::std::byte ec_d[66]{};
	::fast_io::ecc::details::ec_curve const *ec_curve{};
	::std::size_t ec_d_size{};
};

/*
RSAPrivateKey ::= SEQ { version, n, e, d, p, q, dP, dQ, qInv }.
r spans the SEQUENCE body; only n and d are read (the CRT fields are
skipped -- the private op is a straight m^d mod n for now).
*/
inline constexpr bool tls_rsa_pkey_parse_body(tls_pkey *key, wire_reader &r) noexcept
{
	der_tlv v, n, e, d;
	if (!der_expect_tag(r, 0x02, v) || !der_expect_tag(r, 0x02, n) ||
		!der_expect_tag(r, 0x02, e) || !der_expect_tag(r, 0x02, d))
	{
		return false;
	}
	key->kind = tls_pkey_kind::rsa;
	key->rsa_modulus = n.value;
	key->rsa_modulus_size = n.value_size;
	key->rsa_exponent = d.value;
	key->rsa_exponent_size = d.value_size;
	return true;
}

/*
SEC1 ECPrivateKey ::= SEQ { INTEGER(1), OCTET STRING(d),
[0] parameters?, [1] publicKey? }. The [0] params OID pins the curve;
without it the scalar size picks (32 -> P-256, 48 -> P-384).
*/
inline constexpr bool tls_sec1_pkey_parse_body(tls_pkey *key, wire_reader &r) noexcept
{
	der_tlv v, d;
	if (!der_expect_tag(r, 0x02, v) || !der_expect_tag(r, 0x04, d))
	{
		return false;
	}
	if (v.value_size != 1 || v.value[0] != ::std::byte{1})
	{
		return false;
	}
	key->ec_curve = nullptr;
	while (!r.is_empty())
	{
		der_tlv opt;
		if (!der_read_tlv(r, opt))
		{
			return false;
		}
		if (opt.tag != 0xa0)
		{
			continue;
		}
		/* [0] ECParameters { namedCurve OBJECT IDENTIFIER } */
		wire_reader sub{der_sub(opt)};
		der_tlv coid;
		if (der_read_tlv(sub, coid))
		{
			if (der_oid_eq(coid, oid::secp256r1))
			{
				key->ec_curve = __builtin_addressof(::fast_io::ecc::secp256r1);
			}
			else if (der_oid_eq(coid, oid::secp384r1))
			{
				key->ec_curve = __builtin_addressof(::fast_io::ecc::secp384r1);
			}
			else if (der_oid_eq(coid, oid::secp521r1))
			{
				key->ec_curve = __builtin_addressof(::fast_io::ecc::secp521r1);
			}
		}
	}
	if (key->ec_curve == nullptr)
	{
		/* no params -- infer the curve from the scalar size */
		if (d.value_size == 32)
		{
			key->ec_curve = __builtin_addressof(::fast_io::ecc::secp256r1);
		}
		else if (d.value_size == 48)
		{
			key->ec_curve = __builtin_addressof(::fast_io::ecc::secp384r1);
		}
		else if (d.value_size == 66)
		{
			key->ec_curve = __builtin_addressof(::fast_io::ecc::secp521r1);
		}
	}
	if (key->ec_curve == nullptr || d.value_size != key->ec_curve->nbytes)
	{
		return false;
	}
	::fast_io::freestanding::non_overlapped_copy_n(d.value, d.value_size, key->ec_d);
	key->ec_d_size = d.value_size;
	key->kind = tls_pkey_kind::ec;
	return true;
}

/* seed -> expanded 64-byte (seed || public key) fast_io form */
inline constexpr bool tls_ed25519_seed_expand(tls_pkey *key,
											  ::std::byte const *seed, ::std::size_t seed_size) noexcept
{
	if (seed_size != 32)
	{
		return false;
	}
	::std::byte pk[32];
	::fast_io::curve25519::details::ed25519_create_key_pair_to_ptr(pk, key->ed25519_key, seed);
	key->kind = tls_pkey_kind::ed25519;
	return true;
}

/*
PrivateKeyInfo ::= SEQ { version INTEGER, algorithm AlgorithmIdentifier,
privateKey OCTET STRING, attributes [0] OPTIONAL }.
*/
inline constexpr bool tls_pkcs8_parse(tls_pkey *key,
									  ::std::byte const *der, ::std::size_t der_size) noexcept
{
	wire_reader r{der, der + der_size};
	der_tlv seq;
	if (!der_expect_tag(r, 0x30, seq))
	{
		return false;
	}
	wire_reader s{der_sub(seq)};
	der_tlv v, alg_seq, pk;
	if (!der_expect_tag(s, 0x02, v) || !der_expect_tag(s, 0x30, alg_seq) ||
		!der_expect_tag(s, 0x04, pk))
	{
		return false;
	}
	wire_reader a{der_sub(alg_seq)};
	der_tlv alg_oid;
	if (!der_read_tlv(a, alg_oid))
	{
		return false;
	}
	if (der_oid_eq(alg_oid, oid::rsa_encryption) || der_oid_eq(alg_oid, oid::rsassa_pss))
	{
		/* the privateKey OCTET STRING carries a PKCS#1 RSAPrivateKey */
		wire_reader inner{pk.value, pk.value + pk.value_size};
		der_tlv iseq;
		if (!der_expect_tag(inner, 0x30, iseq))
		{
			return false;
		}
		wire_reader b{der_sub(iseq)};
		return tls_rsa_pkey_parse_body(key, b);
	}
	if (der_oid_eq(alg_oid, oid::ed25519))
	{
		/* CurvePrivateKey: the OCTET STRING wraps a second DER OCTET
		   STRING holding the 32-byte seed */
		wire_reader inner{pk.value, pk.value + pk.value_size};
		der_tlv iseed;
		if (!der_expect_tag(inner, 0x04, iseed))
		{
			return false;
		}
		return tls_ed25519_seed_expand(key, iseed.value, iseed.value_size);
	}
	if (der_oid_eq(alg_oid, oid::ec_public_key))
	{
		/* id-ecPublicKey -- the params pin the curve; the privateKey
		   OCTET STRING carries a SEC1 ECPrivateKey */
		der_tlv params;
		if (!der_read_tlv(a, params))
		{
			return false;
		}
		::fast_io::ecc::details::ec_curve const *curve{};
		if (der_oid_eq(params, oid::secp256r1))
		{
			curve = __builtin_addressof(::fast_io::ecc::secp256r1);
		}
		else if (der_oid_eq(params, oid::secp384r1))
		{
			curve = __builtin_addressof(::fast_io::ecc::secp384r1);
		}
		else if (der_oid_eq(params, oid::secp521r1))
		{
			curve = __builtin_addressof(::fast_io::ecc::secp521r1);
		}
		else
		{
			return false;
		}
		wire_reader inner{pk.value, pk.value + pk.value_size};
		der_tlv iseq;
		if (!der_expect_tag(inner, 0x30, iseq))
		{
			return false;
		}
		wire_reader b{der_sub(iseq)};
		if (!tls_sec1_pkey_parse_body(key, b))
		{
			return false;
		}
		return key->ec_curve == curve;
	}
	return false;
}

inline constexpr bool tls_pkey_parse(tls_pkey *key,
									 ::std::byte const *der, ::std::size_t der_size) noexcept
{
	/* PKCS#8 first -- its structure is unambiguous. */
	if (tls_pkcs8_parse(key, der, der_size))
	{
		return true;
	}
	/* bare PKCS#1 RSAPrivateKey -- the CRT fields past d stay unread
	   (a SEC1 EC key fails the n INTEGER check instead of matching) */
	{
		wire_reader r{der, der + der_size};
		der_tlv seq;
		if (der_expect_tag(r, 0x30, seq))
		{
			wire_reader s{der_sub(seq)};
			if (tls_rsa_pkey_parse_body(key, s))
			{
				return true;
			}
		}
	}
	/* bare SEC1 ECPrivateKey */
	{
		wire_reader r{der, der + der_size};
		der_tlv seq;
		if (der_expect_tag(r, 0x30, seq))
		{
			wire_reader s{der_sub(seq)};
			if (tls_sec1_pkey_parse_body(key, s))
			{
				return true;
			}
		}
	}
	/* raw ed25519: expanded key or bare seed */
	if (der_size == 32)
	{
		return tls_ed25519_seed_expand(key, der, 32);
	}
	if (der_size == 64)
	{
		::fast_io::freestanding::non_overlapped_copy_n(der, 64, key->ed25519_key);
		key->kind = tls_pkey_kind::ed25519;
		return true;
	}
	return false;
}

/* one RSA-PSS signature of `covered` under ctx (n, d already loaded) */
template <typename hasher>
inline bool tls_cv_sign_rsa_pss(::std::byte *sig_out, ::fast_io::rsa::private_context const &ctx,
								::std::byte const *covered, ::std::size_t covered_size,
								::std::byte const *salt) noexcept
{
	hasher h;
	h.update(covered, covered + covered_size);
	h.do_final();
	::std::byte digest[hasher::digest_size];
	h.digest_to_byte_ptr(digest);
	return ::fast_io::rsa::sign_pss_to_ptr<hasher>(sig_out, ctx, digest, salt, hasher::digest_size);
}

/*
sign the CertificateVerify covered content (64x0x20 || label || 0 ||
transcript) with key. salt supplies entropy -- at least 48 bytes
(the largest digest); ed25519 ignores it, ecdsa uses the first 32
bytes as the per-message secret with the rest folded in on a
degenerate retry. sig_out must hold modulus_bytes for RSA / 64 for
ed25519 / 72 for ecdsa; *sig_size receives the wire signature size.
*/
inline bool tls_cv_sign(signature_scheme scheme,
						::std::byte const *covered, ::std::size_t covered_size,
						tls_pkey const &key, ::std::byte const *salt,
						::std::byte *sig_out, ::std::size_t *sig_size) noexcept
{
	switch (scheme)
	{
	case signature_scheme::ed25519:
	{
		if (key.kind != tls_pkey_kind::ed25519)
		{
			return false;
		}
		::fast_io::curve25519::details::ed25519_sign_message_to_ptr(
			sig_out, key.ed25519_key, covered, covered_size);
		*sig_size = 64;
		return true;
	}
	case signature_scheme::rsa_pss_rsae_sha256:
	case signature_scheme::rsa_pss_rsae_sha384:
	case signature_scheme::rsa_pss_rsae_sha512:
	case signature_scheme::rsa_pss_pss_sha256:
	case signature_scheme::rsa_pss_pss_sha384:
	case signature_scheme::rsa_pss_pss_sha512:
	{
		if (key.kind != tls_pkey_kind::rsa)
		{
			return false;
		}
		::fast_io::rsa::private_context ctx;
		if (!::fast_io::rsa::private_init_to_ptr(ctx, key.rsa_modulus, key.rsa_modulus_size,
												 key.rsa_exponent, key.rsa_exponent_size))
		{
			return false;
		}
		bool ok{};
		switch (scheme)
		{
		case signature_scheme::rsa_pss_rsae_sha256:
		case signature_scheme::rsa_pss_pss_sha256:
			ok = tls_cv_sign_rsa_pss<::fast_io::sha256_context>(sig_out, ctx, covered, covered_size, salt);
			break;
		case signature_scheme::rsa_pss_rsae_sha384:
		case signature_scheme::rsa_pss_pss_sha384:
			ok = tls_cv_sign_rsa_pss<::fast_io::sha384_context>(sig_out, ctx, covered, covered_size, salt);
			break;
		default:
			ok = tls_cv_sign_rsa_pss<::fast_io::sha512_context>(sig_out, ctx, covered, covered_size, salt);
			break;
		}
		if (ok)
		{
			*sig_size = ctx.modulus_bytes;
		}
		::fast_io::secure_clear(__builtin_addressof(ctx), sizeof(ctx));
		return ok;
	}
	case signature_scheme::ecdsa_secp256r1_sha256:
	case signature_scheme::ecdsa_secp384r1_sha384:
	case signature_scheme::ecdsa_secp521r1_sha512:
	{
		::fast_io::ecc::details::ec_curve const *curve{
			scheme == signature_scheme::ecdsa_secp256r1_sha256
				? __builtin_addressof(::fast_io::ecc::secp256r1)
			: scheme == signature_scheme::ecdsa_secp384r1_sha384
				? __builtin_addressof(::fast_io::ecc::secp384r1)
				: __builtin_addressof(::fast_io::ecc::secp521r1)};
		if (key.kind != tls_pkey_kind::ec || key.ec_curve != curve)
		{
			return false;
		}
		(void)salt; /* rfc6979: the nonce is key+message deterministic --
			   no per-signature entropy consumed */
		if (curve->nl == 4)
		{
			::fast_io::sha256_context hh{};
			hh.update(covered, covered + covered_size);
			hh.do_final();
			::std::byte digest[::fast_io::sha256_context::digest_size];
			hh.digest_to_byte_ptr(digest);
			return ::fast_io::ecc::ecdsa_sign_rfc6979_to_ptr<::fast_io::sha256_context>(
				*curve, sig_out, sig_size, key.ec_d, digest, sizeof(digest));
		}
		if (curve->nl == 6)
		{
			::fast_io::sha384_context hh{};
			hh.update(covered, covered + covered_size);
			hh.do_final();
			::std::byte digest[::fast_io::sha384_context::digest_size];
			hh.digest_to_byte_ptr(digest);
			return ::fast_io::ecc::ecdsa_sign_rfc6979_to_ptr<::fast_io::sha384_context>(
				*curve, sig_out, sig_size, key.ec_d, digest, sizeof(digest));
		}
		::fast_io::sha512_context hh{};
		hh.update(covered, covered + covered_size);
		hh.do_final();
		::std::byte digest[::fast_io::sha512_context::digest_size];
		hh.digest_to_byte_ptr(digest);
		return ::fast_io::ecc::ecdsa_sign_rfc6979_to_ptr<::fast_io::sha512_context>(
			*curve, sig_out, sig_size, key.ec_d, digest, sizeof(digest));
	}
	default:
		return false;
	}
}

} // namespace fast_io::tls::details
