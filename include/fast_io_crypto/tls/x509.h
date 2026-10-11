#pragma once

/*
Minimal DER/X.509 reader for certificate validation (rfc5280).
Everything is spans into the original certificate buffer -- no copies,
no allocation. Signature verification dispatches on the algorithm OID:
rsaEncryption + shaXXXWithRSAEncryption -> PKCS#1 v1.5,
rsassaPss (+params) -> PSS, ed25519 -> Ed25519.
ecdsa-with-SHA* is parsed but verification needs the P-256 curve, which
is not implemented yet -- those certs are rejected closed.
*/

namespace fast_io::tls::details
{

/* ---------------- DER TLV ---------------- */

struct der_tlv
{
	::std::uint_least8_t tag{};
	::std::byte const *value{};
	::std::size_t value_size{};
};

/* read one TLV; sub.value/value_size cover the content octets */
inline constexpr bool der_read_tlv(wire_reader &r, der_tlv &tlv) noexcept
{
	::std::uint_least8_t tag, len0;
	if (!r.take_u8(tag) || !r.take_u8(len0))
	{
		return false;
	}
	::std::size_t len{len0};
	if (len0 == 0x80)
	{
		return false; /* indefinite form is forbidden in DER */
	}
	if (len0 & 0x80u)
	{
		::std::size_t nbytes{len0 & 0x7fu};
		if (nbytes == 0 || nbytes > 4 || r.remaining() < nbytes)
		{
			return false;
		}
		len = 0;
		for (::std::size_t i{}; i != nbytes; ++i)
		{
			::std::uint_least8_t b;
			if (!r.take_u8(b))
			{
				return false;
			}
			len = (len << 8u) | b;
		}
		if (len < 128)
		{
			return false; /* long form for short length is not DER */
		}
	}
	if (r.remaining() < len)
	{
		return false;
	}
	tlv.tag = tag;
	tlv.value = r.cur;
	tlv.value_size = len;
	r.cur += len;
	return true;
}

inline constexpr bool der_expect_tag(wire_reader &r, ::std::uint_least8_t tag, der_tlv &tlv) noexcept
{
	return der_read_tlv(r, tlv) && tlv.tag == tag;
}

inline constexpr wire_reader der_sub(der_tlv const &tlv) noexcept
{
	return {tlv.value, tlv.value + tlv.value_size};
}

/* ---------------- OIDs (content octets only, no 06 tag/len) ---------------- */

template <::std::size_t n>
inline constexpr bool der_oid_eq(der_tlv const &tlv, ::std::uint_least8_t const (&oid)[n]) noexcept
{
	if (tlv.tag != 0x06 || tlv.value_size != n)
	{
		return false;
	}
	for (::std::size_t i{}; i != n; ++i)
	{
		if (static_cast<::std::uint_least8_t>(tlv.value[i]) != oid[i])
		{
			return false;
		}
	}
	return true;
}

namespace oid
{
inline constexpr ::std::uint_least8_t rsa_encryption[]{0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01};
inline constexpr ::std::uint_least8_t md5_with_rsa[]{0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x04};
inline constexpr ::std::uint_least8_t sha1_with_rsa[]{0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x05};
inline constexpr ::std::uint_least8_t sha224_with_rsa[]{0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0e};
inline constexpr ::std::uint_least8_t sha256_with_rsa[]{0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b};
inline constexpr ::std::uint_least8_t sha384_with_rsa[]{0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0c};
inline constexpr ::std::uint_least8_t sha512_with_rsa[]{0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0d};
inline constexpr ::std::uint_least8_t rsassa_pss[]{0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0a};
inline constexpr ::std::uint_least8_t mgf1[]{0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x08};
inline constexpr ::std::uint_least8_t md5[]{0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x02, 0x05};
inline constexpr ::std::uint_least8_t sha1[]{0x2b, 0x0e, 0x03, 0x02, 0x1a};
inline constexpr ::std::uint_least8_t sha224[]{0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x04};
inline constexpr ::std::uint_least8_t sha256[]{0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01};
inline constexpr ::std::uint_least8_t sha384[]{0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02};
inline constexpr ::std::uint_least8_t sha512[]{0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03};
inline constexpr ::std::uint_least8_t sha512_224[]{0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x05};
inline constexpr ::std::uint_least8_t sha512_256[]{0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x06};
inline constexpr ::std::uint_least8_t ec_public_key[]{0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01};
inline constexpr ::std::uint_least8_t secp256r1[]{0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07};
inline constexpr ::std::uint_least8_t secp384r1[]{0x2b, 0x81, 0x04, 0x00, 0x22};
inline constexpr ::std::uint_least8_t secp521r1[]{0x2b, 0x81, 0x04, 0x00, 0x23};
inline constexpr ::std::uint_least8_t ecdsa_with_sha256[]{0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02};
inline constexpr ::std::uint_least8_t ecdsa_with_sha384[]{0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x03};
inline constexpr ::std::uint_least8_t ecdsa_with_sha512[]{0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x04};
inline constexpr ::std::uint_least8_t ed25519[]{0x2b, 0x65, 0x70};
inline constexpr ::std::uint_least8_t subject_alt_name[]{0x55, 0x1d, 0x11};
inline constexpr ::std::uint_least8_t basic_constraints[]{0x55, 0x1d, 0x13};
} // namespace oid

/* ---------------- certificate ---------------- */

struct algorithm_identifier
{
	der_tlv oid{};
	der_tlv params{}; /* whole params TLV (may be absent: value==nullptr) */
	bool has_params{};
};

inline constexpr bool algorithm_identifier_parse(wire_reader &r, algorithm_identifier &alg) noexcept
{
	der_tlv seq;
	if (!der_expect_tag(r, 0x30, seq))
	{
		return false;
	}
	wire_reader s{der_sub(seq)};
	if (!der_read_tlv(s, alg.oid))
	{
		return false;
	}
	alg.has_params = !s.empty();
	if (alg.has_params && !der_read_tlv(s, alg.params))
	{
		return false;
	}
	return s.empty();
}

struct x509_certificate
{
	/* spans into the DER input */
	::std::byte const *tbs{};
	::std::size_t tbs_size{};
	der_tlv signature_algorithm_oid{};
	der_tlv signature_algorithm_params{};
	bool signature_algorithm_has_params{};
	::std::byte const *signature{}; /* bit string content, unused-bits byte stripped */
	::std::size_t signature_size{};

	::std::byte const *issuer{}; /* whole Name TLV (re-encoded) for chaining */
	::std::size_t issuer_size{};
	::std::byte const *subject{};
	::std::size_t subject_size{};
	::std::int_least64_t not_before{}; /* seconds since epoch */
	::std::int_least64_t not_after{};

	algorithm_identifier spki_algorithm{};
	::std::byte const *spki_der{}; /* whole SubjectPublicKeyInfo TLV -- the
									   form provider backends feed their key
									   importers (d2i_PUBKEY, gnutls_pubkey) */
	::std::size_t spki_der_size{};
	::std::byte const *public_key{}; /* bit string content */
	::std::size_t public_key_size{};

	::std::byte const *san{}; /* GeneralNames content octets, if extension present */
	::std::size_t san_size{};
	bool has_basic_constraints{};
	bool is_ca{};
};

/* strip the leading "unused bits" byte of a BIT STRING */
inline constexpr bool der_bit_string(der_tlv const &tlv, ::std::byte const *&bits, ::std::size_t &bits_size) noexcept
{
	if (tlv.tag != 0x03 || tlv.value_size == 0 || tlv.value[0] != ::std::byte{})
	{
		return false;
	}
	bits = tlv.value + 1;
	bits_size = tlv.value_size - 1;
	return true;
}

/* UTCTime (0x17) YYMMDDHHMMSSZ / GeneralizedTime (0x18) YYYYMMDDHHMMSSZ -> epoch */
inline constexpr bool x509_time_to_epoch(der_tlv const &tlv, ::std::int_least64_t &out) noexcept
{
	if (tlv.tag != 0x17 && tlv.tag != 0x18)
	{
		return false;
	}
	::std::size_t const expect{static_cast<::std::size_t>(tlv.tag == 0x17 ? 13 : 15)};
	if (tlv.value_size < expect)
	{
		return false;
	}
	auto dig2{[](::std::byte const *p, ::std::int_least64_t &v) noexcept {
		unsigned const a{static_cast<unsigned>(p[0])}, b{static_cast<unsigned>(p[1])};
		if (a < u8'0' || a > u8'9' || b < u8'0' || b > u8'9')
		{
			return false;
		}
		v = static_cast<::std::int_least64_t>((a - u8'0') * 10 + (b - u8'0'));
		return true;
	}};
	::std::byte const *p{tlv.value};
	::std::int_least64_t y, mo, d, h, mi, s;
	if (tlv.tag == 0x17)
	{
		if (!dig2(p, y))
		{
			return false;
		}
		y += (y < 50) ? 2000 : 1900; /* rfc5280 4.1.2.5.1 */
		p += 2;
	}
	else
	{
		::std::int_least64_t c;
		if (!dig2(p, c) || !dig2(p + 2, y))
		{
			return false;
		}
		y += c * 100;
		p += 4;
	}
	if (!dig2(p, mo) || !dig2(p + 2, d) || !dig2(p + 4, h) || !dig2(p + 6, mi) || !dig2(p + 8, s))
	{
		return false;
	}
	p += 10;
	/* tolerate trailing 'Z' only (no offsets; DER certs in the wild use Z) */
	if (*p != static_cast<::std::byte>(u8'Z'))
	{
		return false;
	}
	/* days from civil (Howard Hinnant) */
	y -= mo <= 2;
	::std::int_least64_t const era{(y >= 0 ? y : y - 399) / 400};
	unsigned const yoe{static_cast<unsigned>(y - era * 400)};
	unsigned const doy{(153u * static_cast<unsigned>(mo > 2 ? mo - 3 : mo + 9) + 2u) / 5u + static_cast<unsigned>(d - 1)};
	unsigned const doe{yoe * 365u + yoe / 4u - yoe / 100u + doy};
	out = (era * 146097 + static_cast<::std::int_least64_t>(doe) - 719468) * 86400 + h * 3600 + mi * 60 + s;
	return true;
}

/*
walk the TBS fields. version/serial/signature are skipped; issuer,
validity, subject, SPKI, and [3] extensions are captured. All pointers
refer into the certificate buffer; issuer/subject keep their SEQ tag so
an exact byte compare is a valid Name equality test.
*/
inline constexpr bool x509_tbs_parse(x509_certificate &cert, wire_reader &tbs) noexcept
{
	der_tlv tlv;
	/* [0] version -- optional explicit */
	{
		wire_reader probe{tbs};
		if (der_read_tlv(probe, tlv) && tlv.tag == 0xa0)
		{
			tbs.cur = probe.cur;
		}
	}
	if (!der_expect_tag(tbs, 0x02, tlv)) /* serialNumber */
	{
		return false;
	}
	if (!der_expect_tag(tbs, 0x30, tlv)) /* signature AlgorithmIdentifier */
	{
		return false;
	}
	{
		/* issuer Name -- keep the whole TLV */
		::std::byte const *mark{tbs.cur};
		if (!der_expect_tag(tbs, 0x30, tlv))
		{
			return false;
		}
		cert.issuer = mark;
		cert.issuer_size = static_cast<::std::size_t>(tbs.cur - mark);
	}
	{
		der_tlv validity;
		if (!der_expect_tag(tbs, 0x30, validity))
		{
			return false;
		}
		wire_reader v{der_sub(validity)};
		der_tlv nb, na;
		if (!der_read_tlv(v, nb) || !der_read_tlv(v, na) ||
			!x509_time_to_epoch(nb, cert.not_before) || !x509_time_to_epoch(na, cert.not_after))
		{
			return false;
		}
	}
	{
		::std::byte const *mark{tbs.cur};
		if (!der_expect_tag(tbs, 0x30, tlv))
		{
			return false;
		}
		cert.subject = mark;
		cert.subject_size = static_cast<::std::size_t>(tbs.cur - mark);
	}
	{
		/* subjectPublicKeyInfo */
		::std::byte const *mark{tbs.cur};
		der_tlv spki;
		if (!der_expect_tag(tbs, 0x30, spki))
		{
			return false;
		}
		cert.spki_der = mark;
		cert.spki_der_size = static_cast<::std::size_t>(tbs.cur - mark);
		wire_reader s{der_sub(spki)};
		if (!algorithm_identifier_parse(s, cert.spki_algorithm) ||
			!der_read_tlv(s, tlv) ||
			!der_bit_string(tlv, cert.public_key, cert.public_key_size))
		{
			return false;
		}
	}
	/* optional [1] issuerUniqueID, [2] subjectUniqueID, [3] extensions */
	while (!tbs.empty())
	{
		::std::byte const *mark{tbs.cur};
		if (!der_read_tlv(tbs, tlv))
		{
			return false;
		}
		if (tlv.tag == 0xa3)
		{
			wire_reader e{der_sub(tlv)};
			der_tlv exts_seq;
			if (!der_expect_tag(e, 0x30, exts_seq))
			{
				return false;
			}
			wire_reader exts{der_sub(exts_seq)};
			while (!exts.empty())
			{
				der_tlv ext_seq;
				if (!der_expect_tag(exts, 0x30, ext_seq))
				{
					return false;
				}
				wire_reader ex{der_sub(ext_seq)};
				der_tlv ext_oid;
				if (!der_read_tlv(ex, ext_oid))
				{
					return false;
				}
				/* optional critical BOOLEAN */
				if (!ex.empty() && *ex.cur == ::std::byte{0x01})
				{
					der_tlv tmp;
					if (!der_read_tlv(ex, tmp))
					{
						return false;
					}
				}
				der_tlv ext_value;
				if (!der_expect_tag(ex, 0x04, ext_value)) /* OCTET STRING */
				{
					return false;
				}
				if (der_oid_eq(ext_oid, oid::subject_alt_name))
				{
					cert.san = ext_value.value;
					cert.san_size = ext_value.value_size;
				}
				else if (der_oid_eq(ext_oid, oid::basic_constraints))
				{
					cert.has_basic_constraints = true;
					wire_reader bc{ext_value.value, ext_value.value + ext_value.value_size};
					der_tlv seq;
					if (der_expect_tag(bc, 0x30, seq))
					{
						wire_reader b{der_sub(seq)};
						der_tlv ca;
						if (!b.empty() && der_read_tlv(b, ca) && ca.tag == 0x01 && ca.value_size == 1)
						{
							cert.is_ca = (ca.value[0] == ::std::byte{0xff});
						}
					}
				}
			}
		}
		else
		{
			(void)mark;
		}
	}
	return true;
}

/* Certificate ::= SEQ { tbs, signatureAlgorithm, signatureValue } */
inline constexpr bool x509_certificate_parse(x509_certificate &cert,
											 ::std::byte const *der, ::std::size_t der_size) noexcept
{
	wire_reader r{der, der + der_size};
	der_tlv outer;
	if (!der_expect_tag(r, 0x30, outer) || !r.empty())
	{
		return false;
	}
	wire_reader body{der_sub(outer)};
	der_tlv tbs_tlv;
	::std::byte const *tbs_mark{body.cur};
	if (!der_expect_tag(body, 0x30, tbs_tlv))
	{
		return false;
	}
	cert.tbs = tbs_mark;
	cert.tbs_size = static_cast<::std::size_t>(body.cur - tbs_mark);
	{
		algorithm_identifier sig_alg;
		if (!algorithm_identifier_parse(body, sig_alg))
		{
			return false;
		}
		cert.signature_algorithm_oid = sig_alg.oid;
		cert.signature_algorithm_params = sig_alg.params;
		cert.signature_algorithm_has_params = sig_alg.has_params;
	}
	der_tlv sig_tlv;
	if (!der_read_tlv(body, sig_tlv) ||
		!der_bit_string(sig_tlv, cert.signature, cert.signature_size) || !body.empty())
	{
		return false;
	}
	wire_reader tbs_reader{tbs_tlv.value, tbs_tlv.value + tbs_tlv.value_size};
	return x509_tbs_parse(cert, tbs_reader);
}

/* ---------------- signature dispatch ---------------- */

enum class x509_verify_result : ::std::uint_least8_t
{
	ok,
	bad_signature,
	unsupported_algorithm,
	malformed
};

/* RSASSA-PSS-params (rfc8017): all fields optional, defaults sha1/mgf1-sha1/20/1 */
struct rsassa_pss_params
{
	der_tlv hash_oid{};
	der_tlv mgf_hash_oid{};
	::std::uint_least32_t salt_size{20};
	bool has_hash{};
	bool has_mgf_hash{};
	bool has_salt{};
};

inline constexpr bool rsassa_pss_params_parse(der_tlv const &params, rsassa_pss_params &out) noexcept
{
	if (params.tag != 0x30)
	{
		return false;
	}
	wire_reader r{der_sub(params)};
	while (!r.empty())
	{
		der_tlv tlv;
		if (!der_read_tlv(r, tlv))
		{
			return false;
		}
		if (tlv.tag == 0xa0) /* [0] hash AlgorithmIdentifier */
		{
			wire_reader s{der_sub(tlv)};
			algorithm_identifier h;
			if (!algorithm_identifier_parse(s, h))
			{
				return false;
			}
			out.hash_oid = h.oid;
			out.has_hash = true;
		}
		else if (tlv.tag == 0xa1) /* [1] mgf AlgorithmIdentifier = mgf1 + hash params */
		{
			wire_reader s{der_sub(tlv)};
			algorithm_identifier m;
			if (!algorithm_identifier_parse(s, m) || !der_oid_eq(m.oid, oid::mgf1) || !m.has_params)
			{
				return false;
			}
			wire_reader mp{der_sub(m.params)};
			algorithm_identifier mh;
			if (!algorithm_identifier_parse(mp, mh))
			{
				return false;
			}
			out.mgf_hash_oid = mh.oid;
			out.has_mgf_hash = true;
		}
		else if (tlv.tag == 0xa2) /* [2] saltLength INTEGER */
		{
			wire_reader s{der_sub(tlv)};
			der_tlv i;
			if (!der_expect_tag(s, 0x02, i) || i.value_size > 4 || i.value_size == 0)
			{
				return false;
			}
			::std::uint_least32_t v{};
			for (::std::size_t k{}; k != i.value_size; ++k)
			{
				v = (v << 8u) | static_cast<::std::uint_least8_t>(i.value[k]);
			}
			out.salt_size = v;
			out.has_salt = true;
		}
		else if (tlv.tag == 0xa3) /* [3] trailerField, must be 1 */
		{
			wire_reader s{der_sub(tlv)};
			der_tlv i;
			if (!der_expect_tag(s, 0x02, i) || i.value_size != 1 || i.value[0] != ::std::byte{1})
			{
				return false;
			}
		}
	}
	return true;
}

/* runtime OID compare: content octets of tlv vs a byte span */
inline constexpr bool der_oid_eq_span(der_tlv const &tlv, ::std::uint_least8_t const *oid_bytes, ::std::size_t oid_size) noexcept
{
	if (tlv.tag != 0x06 || tlv.value_size != oid_size)
	{
		return false;
	}
	for (::std::size_t i{}; i != oid_size; ++i)
	{
		if (static_cast<::std::uint_least8_t>(tlv.value[i]) != oid_bytes[i])
		{
			return false;
		}
	}
	return true;
}

/*
hash tbs with the hash named by oid into digest_out (>= 64 bytes);
on success digest_size is set and ctx_out is left untouched -- the
verifier dispatches on the oid again with the matching hasher type.
*/
template <typename func>
inline constexpr bool x509_hash_dispatch(der_tlv const &hash_oid, func &&f) noexcept
{
	if (der_oid_eq(hash_oid, oid::sha224))
	{
		return f(::fast_io::sha224_context{});
	}
	if (der_oid_eq(hash_oid, oid::sha256))
	{
		return f(::fast_io::sha256_context{});
	}
	if (der_oid_eq(hash_oid, oid::sha384))
	{
		return f(::fast_io::sha384_context{});
	}
	if (der_oid_eq(hash_oid, oid::sha512))
	{
		return f(::fast_io::sha512_context{});
	}
	if (der_oid_eq(hash_oid, oid::sha512_224))
	{
		return f(::fast_io::sha512_224_context{});
	}
	if (der_oid_eq(hash_oid, oid::sha512_256))
	{
		return f(::fast_io::sha512_256_context{});
	}
	return false;
}

/* RSAPublicKey ::= SEQ { modulus INTEGER, publicExponent INTEGER } */
inline constexpr bool rsa_public_key_decode(::std::byte const *pk, ::std::size_t pk_size,
											::std::byte const *&n, ::std::size_t &n_size,
											::std::byte const *&e, ::std::size_t &e_size) noexcept
{
	wire_reader r{pk, pk + pk_size};
	der_tlv seq, ni, ei;
	if (!der_expect_tag(r, 0x30, seq) || !r.empty())
	{
		return false;
	}
	wire_reader s{der_sub(seq)};
	if (!der_expect_tag(s, 0x02, ni) || !der_expect_tag(s, 0x02, ei) || !s.empty())
	{
		return false;
	}
	n = ni.value;
	n_size = ni.value_size;
	e = ei.value;
	e_size = ei.value_size;
	return true;
}

/*
run one RSA verification for the cert's signature: hash oid ->
hasher -> pkcs1v15 or pss.
*/
template <typename hash_ctx>
inline constexpr bool x509_rsa_verify_impl(bool use_pss, ::std::size_t pss_salt_size,
										   ::std::byte const *tbs, ::std::size_t tbs_size,
										   ::std::byte const *signature, ::std::size_t signature_size,
										   ::std::byte const *issuer_public_key, ::std::size_t issuer_public_key_size) noexcept
{
	::std::byte const *n;
	::std::size_t n_size;
	::std::byte const *e;
	::std::size_t e_size;
	if (!rsa_public_key_decode(issuer_public_key, issuer_public_key_size, n, n_size, e, e_size))
	{
		return false;
	}
	::fast_io::rsa::verify_context ctx;
	if (!::fast_io::rsa::verify_init_to_ptr(ctx, n, n_size, e, e_size))
	{
		return false;
	}
	hash_ctx h{};
	h.update(tbs, tbs + tbs_size);
	h.do_final();
	::std::byte digest[hash_ctx::digest_size];
	h.digest_to_byte_ptr(digest);
	if (use_pss)
	{
		return ::fast_io::rsa::verify_pss_to_ptr<hash_ctx>(ctx, signature, signature_size, digest, pss_salt_size);
	}
	return ::fast_io::rsa::verify_pkcs1v15_to_ptr<hash_ctx>(ctx, signature, signature_size, digest);
}

/*
ECDSA signature wire form (rfc5758, same in TLS 1.3 CertificateVerify):
SEQUENCE { INTEGER r, INTEGER s }. Decodes to the two INTEGER TLVs;
trailing bytes are forbidden.
*/
inline constexpr bool ecdsa_signature_decode(::std::byte const *sig,
											 ::std::size_t sig_size,
											 der_tlv &r, der_tlv &s) noexcept
{
	wire_reader rd{sig, sig + sig_size};
	der_tlv seq;
	if (!der_expect_tag(rd, 0x30, seq))
	{
		return false;
	}
	wire_reader b{der_sub(seq)};
	if (!der_expect_tag(b, 0x02, r) || !der_expect_tag(b, 0x02, s))
	{
		return false;
	}
	return rd.empty() && b.empty();
}

/* one functor for both RSA verify dispatches below (PSS and v1.5):
   a lambda per site would mint two x509_hash_dispatch instantiations
   of the same shape */
struct x509_rsa_verify_cb
{
	x509_certificate const *cert;
	::std::byte const *issuer_public_key;
	::std::size_t issuer_public_key_size;
	bool use_pss;
	::std::size_t pss_salt_size;
	bool *ok;
	template <typename hash_ctx>
	inline constexpr bool operator()(hash_ctx) const noexcept
	{
		*ok = x509_rsa_verify_impl<hash_ctx>(use_pss, pss_salt_size,
											 cert->tbs, cert->tbs_size,
											 cert->signature, cert->signature_size,
											 issuer_public_key, issuer_public_key_size);
		return true;
	}
};

/*
verify cert.signature over cert.tbs with the issuer's public key.
issuer_* come from the issuer certificate's SPKI.
RSA public key BIT STRING content is the DER SEQ { n INTEGER, e INTEGER }.
md5/sha1 signature algorithms are refused outright.
*/
inline constexpr x509_verify_result x509_verify_signature(
	x509_certificate const &cert,
	algorithm_identifier const &issuer_alg,
	::std::byte const *issuer_public_key, ::std::size_t issuer_public_key_size) noexcept
{
	/* RSASSA-PSS */
	if (der_oid_eq(cert.signature_algorithm_oid, oid::rsassa_pss))
	{
		if (!der_oid_eq(issuer_alg.oid, oid::rsa_encryption) &&
			!der_oid_eq(issuer_alg.oid, oid::rsassa_pss))
		{
			return x509_verify_result::unsupported_algorithm;
		}
		if (!cert.signature_algorithm_has_params)
		{
			return x509_verify_result::malformed;
		}
		rsassa_pss_params pp;
		if (!rsassa_pss_params_parse(cert.signature_algorithm_params, pp))
		{
			return x509_verify_result::malformed;
		}
		der_tlv hash_oid;
		hash_oid.tag = 0x06;
		if (pp.has_hash)
		{
			hash_oid = pp.hash_oid;
		}
		else
		{
			hash_oid.value = reinterpret_cast<::std::byte const *>(oid::sha1);
			hash_oid.value_size = sizeof(oid::sha1);
		}
		if (pp.has_mgf_hash &&
			(pp.mgf_hash_oid.value_size != hash_oid.value_size ||
			 ::fast_io::freestanding::my_memcmp(pp.mgf_hash_oid.value, hash_oid.value, hash_oid.value_size) != 0))
		{
			return x509_verify_result::unsupported_algorithm;
		}
		/* md5/sha1 PSS is not worth trusting; accept sha224 and up */
		if (der_oid_eq(hash_oid, oid::md5) || der_oid_eq(hash_oid, oid::sha1))
		{
			return x509_verify_result::unsupported_algorithm;
		}
		::std::size_t const salt{pp.has_salt ? pp.salt_size : 20u /* rfc8017 default */};
		bool ok{false};
		bool const known{x509_hash_dispatch(
			hash_oid,
			x509_rsa_verify_cb{&cert, issuer_public_key, issuer_public_key_size,
							   true, salt, &ok})};
		if (!known)
		{
			return x509_verify_result::unsupported_algorithm;
		}
		return ok ? x509_verify_result::ok : x509_verify_result::bad_signature;
	}

	/* shaXXXWithRSAEncryption -> PKCS#1 v1.5 */
	struct rsa_sig_map
	{
		::std::uint_least8_t const *sig_oid;
		::std::size_t sig_oid_size;
		::std::uint_least8_t const *hash_oid;
		::std::size_t hash_oid_size;
	};
	constexpr rsa_sig_map rsa_map[]{
		{oid::sha256_with_rsa, sizeof(oid::sha256_with_rsa), oid::sha256, sizeof(oid::sha256)},
		{oid::sha384_with_rsa, sizeof(oid::sha384_with_rsa), oid::sha384, sizeof(oid::sha384)},
		{oid::sha512_with_rsa, sizeof(oid::sha512_with_rsa), oid::sha512, sizeof(oid::sha512)},
		{oid::sha224_with_rsa, sizeof(oid::sha224_with_rsa), oid::sha224, sizeof(oid::sha224)},
	};
	for (auto const &e : rsa_map)
	{
		der_tlv const &so{cert.signature_algorithm_oid};
		if (so.tag != 0x06 || so.value_size != e.sig_oid_size)
		{
			continue;
		}
		bool match{true};
		for (::std::size_t i{}; i != e.sig_oid_size; ++i)
		{
			match &= (static_cast<::std::uint_least8_t>(so.value[i]) == e.sig_oid[i]);
		}
		if (!match)
		{
			continue;
		}
		if (!der_oid_eq(issuer_alg.oid, oid::rsa_encryption))
		{
			return x509_verify_result::unsupported_algorithm;
		}
		der_tlv hash_oid;
		hash_oid.tag = 0x06;
		hash_oid.value = reinterpret_cast<::std::byte const *>(e.hash_oid);
		hash_oid.value_size = e.hash_oid_size;
		bool ok{false};
		bool const known{x509_hash_dispatch(
			hash_oid,
			x509_rsa_verify_cb{&cert, issuer_public_key, issuer_public_key_size,
							   false, 0, &ok})};
		if (!known)
		{
			return x509_verify_result::unsupported_algorithm;
		}
		return ok ? x509_verify_result::ok : x509_verify_result::bad_signature;
	}
	if (der_oid_eq(cert.signature_algorithm_oid, oid::md5_with_rsa) ||
		der_oid_eq(cert.signature_algorithm_oid, oid::sha1_with_rsa))
	{
		return x509_verify_result::unsupported_algorithm; /* weak: refuse */
	}

	/* ed25519: signature over raw TBS */
	if (der_oid_eq(cert.signature_algorithm_oid, oid::ed25519))
	{
		if (!der_oid_eq(issuer_alg.oid, oid::ed25519) || issuer_public_key_size != 32)
		{
			return x509_verify_result::unsupported_algorithm;
		}
		bool const ok{::fast_io::ed25519::verify_signature_to_ptr(cert.signature, issuer_public_key,
																  cert.tbs, cert.tbs_size)};
		return ok ? x509_verify_result::ok : x509_verify_result::bad_signature;
	}

	/* ecdsa-with-SHA{256,384,512}: the signature OID picks only the
	   hash -- the curve comes from the issuer's SPKI params; x509
	   mixes them freely (openssl signs P-384 certs with SHA-256 by
	   default) */
	{
		::std::size_t digest_size{};
		if (der_oid_eq(cert.signature_algorithm_oid, oid::ecdsa_with_sha256))
		{
			digest_size = ::fast_io::sha256_context::digest_size;
		}
		else if (der_oid_eq(cert.signature_algorithm_oid, oid::ecdsa_with_sha384))
		{
			digest_size = ::fast_io::sha384_context::digest_size;
		}
		else if (der_oid_eq(cert.signature_algorithm_oid, oid::ecdsa_with_sha512))
		{
			digest_size = ::fast_io::sha512_context::digest_size;
		}
		if (digest_size != 0)
		{
			::fast_io::ecc::details::ec_curve const *curve{};
			if (der_oid_eq(issuer_alg.params, oid::secp256r1))
			{
				curve = __builtin_addressof(::fast_io::ecc::secp256r1);
			}
			else if (der_oid_eq(issuer_alg.params, oid::secp384r1))
			{
				curve = __builtin_addressof(::fast_io::ecc::secp384r1);
			}
			else if (der_oid_eq(issuer_alg.params, oid::secp521r1))
			{
				curve = __builtin_addressof(::fast_io::ecc::secp521r1);
			}
			if (curve == nullptr ||
				!der_oid_eq(issuer_alg.oid, oid::ec_public_key))
			{
				return x509_verify_result::unsupported_algorithm;
			}
			der_tlv r, s;
			if (!ecdsa_signature_decode(cert.signature, cert.signature_size, r, s))
			{
				return x509_verify_result::malformed;
			}
			::std::byte digest[64];
			if (digest_size == ::fast_io::sha256_context::digest_size)
			{
				::fast_io::sha256_context hh{};
				hh.update(cert.tbs, cert.tbs + cert.tbs_size);
				hh.do_final();
				hh.digest_to_byte_ptr(digest);
			}
			else if (digest_size == ::fast_io::sha384_context::digest_size)
			{
				::fast_io::sha384_context hh{};
				hh.update(cert.tbs, cert.tbs + cert.tbs_size);
				hh.do_final();
				hh.digest_to_byte_ptr(digest);
			}
			else
			{
				::fast_io::sha512_context hh{};
				hh.update(cert.tbs, cert.tbs + cert.tbs_size);
				hh.do_final();
				hh.digest_to_byte_ptr(digest);
			}
			bool const ok{::fast_io::ecc::ecdsa_verify_to_ptr(
				*curve, r.value, r.value_size, s.value, s.value_size,
				issuer_public_key, issuer_public_key_size,
				digest, digest_size)};
			return ok ? x509_verify_result::ok : x509_verify_result::bad_signature;
		}
	}
	return x509_verify_result::unsupported_algorithm;
}

/* ---------------- hostname check ---------------- */

/* case-insensitive ascii equal for one dns label chunk */
inline constexpr bool ascii_ieq(::std::byte const *a, ::std::byte const *b, ::std::size_t n) noexcept
{
	for (::std::size_t i{}; i != n; ++i)
	{
		unsigned x{static_cast<unsigned>(a[i])}, y{static_cast<unsigned>(b[i])};
		x = (x >= u8'a' && x <= u8'z') ? x - 32 : x;
		y = (y >= u8'a' && y <= u8'z') ? y - 32 : y;
		if (x != y)
		{
			return false;
		}
	}
	return true;
}

/*
match one dNSName pattern against the hostname.
Only a leftmost-label wildcard ("*.example.com") is honored, per the
usual CABF/browser rules.
*/
inline constexpr bool dns_name_match(::std::byte const *pattern, ::std::size_t pattern_size,
									 char8_t const *hostname, ::std::size_t hostname_size) noexcept
{
	if (pattern_size == 0 || hostname_size == 0)
	{
		return false;
	}
	if (pattern[0] == static_cast<::std::byte>(u8'*'))
	{
		if (pattern_size < 3 || pattern[1] != static_cast<::std::byte>(u8'.'))
		{
			return false;
		}
		/* the host must have one extra leftmost label */
		char8_t const *dot{hostname};
		while (dot != hostname + hostname_size && *dot != u8'.')
		{
			++dot;
		}
		if (dot == hostname || dot == hostname + hostname_size)
		{
			return false;
		}
		::std::size_t const suffix{static_cast<::std::size_t>(hostname + hostname_size - dot - 1)};
		if (suffix != pattern_size - 2)
		{
			return false;
		}
		return ascii_ieq(pattern + 2, reinterpret_cast<::std::byte const *>(dot + 1), suffix);
	}
	return pattern_size == hostname_size &&
		   ascii_ieq(pattern, reinterpret_cast<::std::byte const *>(hostname), hostname_size);
}

/*
check hostname against the cert's subjectAltName dNSName list.
GeneralNames ::= SEQ { GeneralName* }; dNSName is [2] IA5String (0x82).
*/
inline constexpr bool x509_hostname_match(x509_certificate const &cert,
										  char8_t const *hostname, ::std::size_t hostname_size) noexcept
{
	if (cert.san == nullptr)
	{
		return false; /* SAN required in TLS 1.3 practice; CN fallback deliberately unsupported */
	}
	wire_reader r{cert.san, cert.san + cert.san_size};
	der_tlv seq;
	if (!der_expect_tag(r, 0x30, seq))
	{
		return false;
	}
	wire_reader names{der_sub(seq)};
	while (!names.empty())
	{
		der_tlv gn;
		if (!der_read_tlv(names, gn))
		{
			return false;
		}
		if (gn.tag == 0x82 && dns_name_match(gn.value, gn.value_size, hostname, hostname_size))
		{
			return true;
		}
	}
	return false;
}

/* ---------------- chain validation ---------------- */

inline constexpr bool x509_name_eq(::std::byte const *a, ::std::size_t an,
								   ::std::byte const *b, ::std::size_t bn) noexcept
{
	return an == bn &&
		   ::fast_io::freestanding::my_memcmp(a, b, an) == 0;
}

enum class x509_chain_result : ::std::uint_least8_t
{
	ok,
	expired,
	not_yet_valid,
	bad_signature,
	unsupported_algorithm,
	malformed,
	untrusted, /* no path to a trust anchor */
	not_ca,    /* an issuer lacks CA:TRUE */
	chain_too_long
};

/*
presented[0] is the leaf; presented[1..] are intermediates; roots[] are
trust anchors (already parsed). Issuer resolution is exact byte equality
of the issuer Name TLV against candidate subject Name TLVs. Max depth 8.
now is seconds since epoch for the validity window.

Trusted-first path building (rfc5280 treats a trust anchor as a
{subject name, public key} pair, the self-signed cert wrapper is
incidental): at every step the trust anchors are consulted BEFORE the
presented certs. This matters for cross-signed anchors — a server may
present an anchor re-issued by some other CA (e.g. GTS Root R1
cross-signed by a retired GlobalSign root); its name+key still match the
trusted anchor, so the path must terminate there instead of chasing the
cross-sign's dead-end issuer.
*/
/* takes the anchors as raw DER (a bundle may contain certs our minimal
   parser cannot handle) so the caller keeps no parsed vector -- each
   candidate is parsed when checked and skipped on failure */
template <typename crypto>
inline constexpr x509_chain_result x509_chain_verify(
	x509_certificate const *presented, ::std::size_t presented_count,
	::std::byte const *const *roots_der, ::std::size_t const *root_sizes,
	::std::size_t root_count, ::std::int_least64_t now) noexcept
{
	if (presented_count == 0)
	{
		return x509_chain_result::malformed;
	}
	::std::size_t idx{0};
	for (::std::size_t depth{}; depth != 8; ++depth)
	{
		x509_certificate const &cur{presented[idx]};
		if (now < cur.not_before)
		{
			return x509_chain_result::not_yet_valid;
		}
		if (cur.not_after < now)
		{
			return x509_chain_result::expired;
		}
		/* is cur itself a trust anchor? (same name+key as a root) */
		for (::std::size_t i{}; i != root_count; ++i)
		{
			x509_certificate r{};
			if (!x509_certificate_parse(r, roots_der[i], root_sizes[i]))
			{
				continue;
			}
			if (x509_name_eq(cur.subject, cur.subject_size, r.subject, r.subject_size) &&
				x509_name_eq(cur.spki_algorithm.oid.value, cur.spki_algorithm.oid.value_size,
							 r.spki_algorithm.oid.value, r.spki_algorithm.oid.value_size) &&
				cur.public_key_size == r.public_key_size &&
				::fast_io::freestanding::my_memcmp(cur.public_key, r.public_key, r.public_key_size) == 0)
			{
				return x509_chain_result::ok;
			}
		}
		/* issuers among anchors first: signature under an anchor's key
		   terminates the path immediately */
		bool root_name_matched{};
		for (::std::size_t i{}; i != root_count; ++i)
		{
			x509_certificate r{};
			if (!x509_certificate_parse(r, roots_der[i], root_sizes[i]) ||
				!x509_name_eq(cur.issuer, cur.issuer_size, r.subject, r.subject_size))
			{
				continue;
			}
			root_name_matched = true;
			if (crypto::cert_sig_verify(cur, r) ==
				x509_verify_result::ok)
			{
				return x509_chain_result::ok;
			}
			/* same-name anchor with a different key (rotation): fall
			   through to the presented issuers */
		}
		/* then issuers among the presented certs */
		::std::size_t next_idx{presented_count};
		bool bad_sig{};
		for (::std::size_t i{1}; i != presented_count; ++i)
		{
			if (i == idx ||
				!x509_name_eq(cur.issuer, cur.issuer_size, presented[i].subject, presented[i].subject_size))
			{
				continue;
			}
			if (presented[i].has_basic_constraints && !presented[i].is_ca)
			{
				return x509_chain_result::not_ca;
			}
			x509_verify_result const vr{crypto::cert_sig_verify(cur, presented[i])};
			if (vr == x509_verify_result::ok)
			{
				next_idx = i;
				break;
			}
			if (vr != x509_verify_result::bad_signature)
			{
				return x509_chain_result::unsupported_algorithm;
			}
			bad_sig = true;
			/* other same-subject presented certs may still verify */
		}
		if (next_idx != presented_count)
		{
			idx = next_idx;
			continue;
		}
		return (bad_sig || root_name_matched) ? x509_chain_result::bad_signature
											  : x509_chain_result::untrusted;
	}
	return x509_chain_result::chain_too_long;
}

/* ---------------- TLS CertificateVerify ---------------- */

/*
verify the server CertificateVerify signature.
covered = 64x0x20 || "TLS 1.3, server CertificateVerify" || 0x00 || th
leaf_key is the leaf cert's SPKI public key bit string content.
Returns x509_verify_result (unsupported_algorithm covers schemes we do
not advertise and schemes TLS 1.3 forbids, like rsa_pkcs1_*).
*/
inline constexpr x509_verify_result tls_certificate_verify(
	signature_scheme scheme,
	::std::byte const *covered, ::std::size_t covered_size,
	::std::byte const *signature, ::std::size_t signature_size,
	algorithm_identifier const &leaf_alg,
	::std::byte const *leaf_key, ::std::size_t leaf_key_size) noexcept
{
	switch (scheme)
	{
	case signature_scheme::rsa_pss_rsae_sha256:
	case signature_scheme::rsa_pss_rsae_sha384:
	case signature_scheme::rsa_pss_rsae_sha512:
	case signature_scheme::rsa_pss_pss_sha256:
	case signature_scheme::rsa_pss_pss_sha384:
	case signature_scheme::rsa_pss_pss_sha512:
	{
		::std::byte const *n;
		::std::size_t n_size;
		::std::byte const *e;
		::std::size_t e_size;
		if (!rsa_public_key_decode(leaf_key, leaf_key_size, n, n_size, e, e_size))
		{
			return x509_verify_result::malformed;
		}
		::fast_io::rsa::verify_context ctx;
		if (!::fast_io::rsa::verify_init_to_ptr(ctx, n, n_size, e, e_size))
		{
			return x509_verify_result::malformed;
		}
		/* PSS salt len must equal the hash len (rfc8446 4.2.3) */
		::std::byte digest[64];
		::std::size_t dsize{};
		switch (scheme)
		{
		case signature_scheme::rsa_pss_rsae_sha256:
		case signature_scheme::rsa_pss_pss_sha256:
		{
			::fast_io::sha256_context hh{};
			hh.update(covered, covered + covered_size);
			hh.do_final();
			hh.digest_to_byte_ptr(digest);
			dsize = ::fast_io::sha256_context::digest_size;
			bool const ok{::fast_io::rsa::verify_pss_to_ptr<::fast_io::sha256_context>(ctx, signature, signature_size, digest, dsize)};
			return ok ? x509_verify_result::ok : x509_verify_result::bad_signature;
		}
		case signature_scheme::rsa_pss_rsae_sha384:
		case signature_scheme::rsa_pss_pss_sha384:
		{
			::fast_io::sha384_context hh{};
			hh.update(covered, covered + covered_size);
			hh.do_final();
			hh.digest_to_byte_ptr(digest);
			dsize = ::fast_io::sha384_context::digest_size;
			bool const ok{::fast_io::rsa::verify_pss_to_ptr<::fast_io::sha384_context>(ctx, signature, signature_size, digest, dsize)};
			return ok ? x509_verify_result::ok : x509_verify_result::bad_signature;
		}
		default:
		{
			::fast_io::sha512_context hh{};
			hh.update(covered, covered + covered_size);
			hh.do_final();
			hh.digest_to_byte_ptr(digest);
			dsize = ::fast_io::sha512_context::digest_size;
			bool const ok{::fast_io::rsa::verify_pss_to_ptr<::fast_io::sha512_context>(ctx, signature, signature_size, digest, dsize)};
			return ok ? x509_verify_result::ok : x509_verify_result::bad_signature;
		}
		}
	}
	case signature_scheme::ed25519:
	{
		if (leaf_key_size != 32 || !der_oid_eq(leaf_alg.oid, oid::ed25519))
		{
			return x509_verify_result::unsupported_algorithm;
		}
		bool const ok{::fast_io::ed25519::verify_signature_to_ptr(signature, leaf_key, covered, covered_size)};
		return ok ? x509_verify_result::ok : x509_verify_result::bad_signature;
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
		::std::uint_least8_t const *want_oid{curve->nl == 4
												 ? oid::secp256r1
											 : curve->nl == 6 ? oid::secp384r1
															  : oid::secp521r1};
		::std::size_t const want_oid_size{curve->nl == 4   ? sizeof(oid::secp256r1)
										  : curve->nl == 6 ? sizeof(oid::secp384r1)
														   : sizeof(oid::secp521r1)};
		if (!der_oid_eq(leaf_alg.oid, oid::ec_public_key) ||
			!der_oid_eq_span(leaf_alg.params, want_oid, want_oid_size))
		{
			return x509_verify_result::unsupported_algorithm;
		}
		der_tlv r, s;
		if (!ecdsa_signature_decode(signature, signature_size, r, s))
		{
			return x509_verify_result::malformed;
		}
		::std::byte digest[64];
		::std::size_t digest_size{};
		if (curve->nl == 4)
		{
			::fast_io::sha256_context hh{};
			hh.update(covered, covered + covered_size);
			hh.do_final();
			hh.digest_to_byte_ptr(digest);
			digest_size = ::fast_io::sha256_context::digest_size;
		}
		else if (curve->nl == 6)
		{
			::fast_io::sha384_context hh{};
			hh.update(covered, covered + covered_size);
			hh.do_final();
			hh.digest_to_byte_ptr(digest);
			digest_size = ::fast_io::sha384_context::digest_size;
		}
		else
		{
			::fast_io::sha512_context hh{};
			hh.update(covered, covered + covered_size);
			hh.do_final();
			hh.digest_to_byte_ptr(digest);
			digest_size = ::fast_io::sha512_context::digest_size;
		}
		bool const ok{::fast_io::ecc::ecdsa_verify_to_ptr(
			*curve, r.value, r.value_size, s.value, s.value_size,
			leaf_key, leaf_key_size, digest, digest_size)};
		return ok ? x509_verify_result::ok : x509_verify_result::bad_signature;
	}
	default:
		/* rsa_pkcs1_* is forbidden in CertificateVerify by TLS 1.3 */
		return x509_verify_result::unsupported_algorithm;
	}
}

/* parse every DER blob of a stored peer chain -- false on the first
   unparseable entry */
inline bool x509_certificate_parse_all(x509_certificate *out,
									   ::std::size_t cap,
									   ::std::byte const *storage,
									   ::std::size_t const *offsets,
									   ::std::size_t const *sizes,
									   ::std::size_t count) noexcept
{
	if (count > cap)
	{
		return false;
	}
	for (::std::size_t i{}; i != count; ++i)
	{
		if (!x509_certificate_parse(out[i], storage + offsets[i], sizes[i]))
		{
			return false;
		}
	}
	return true;
}

} // namespace fast_io::tls::details
