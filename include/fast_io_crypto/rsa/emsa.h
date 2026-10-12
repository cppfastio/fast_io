#pragma once

/*
EMSA padding verification for RSASSA-PKCS1-v1_5 and RSASSA-PSS
(RFC 8017 section 9.2). The encoded message EM is the result of the raw
RSA public operation s^e mod n, encoded big-endian.
*/

namespace fast_io::details::rsa
{

/*
DigestInfo DER prefix (everything before the digest octets) per hash,
for EMSA-PKCS1-v1_5 T = prefix || digest.
*/
template <typename ctx>
struct pkcs1v15_digest_info;

template <>
struct pkcs1v15_digest_info<::fast_io::md5_context>
{
	static inline constexpr ::std::byte prefix[18]{
		::std::byte{0x30}, ::std::byte{0x20}, ::std::byte{0x30}, ::std::byte{0x0c}, ::std::byte{0x06}, ::std::byte{0x08},
		::std::byte{0x2a}, ::std::byte{0x86}, ::std::byte{0x48}, ::std::byte{0x86}, ::std::byte{0xf7}, ::std::byte{0x0d},
		::std::byte{0x02}, ::std::byte{0x05}, ::std::byte{0x05}, ::std::byte{0x00}, ::std::byte{0x04}, ::std::byte{0x10}};
};

template <>
struct pkcs1v15_digest_info<::fast_io::sha1_context>
{
	static inline constexpr ::std::byte prefix[15]{
		::std::byte{0x30}, ::std::byte{0x21}, ::std::byte{0x30}, ::std::byte{0x09}, ::std::byte{0x06}, ::std::byte{0x05},
		::std::byte{0x2b}, ::std::byte{0x0e}, ::std::byte{0x03}, ::std::byte{0x02}, ::std::byte{0x1a}, ::std::byte{0x05},
		::std::byte{0x00}, ::std::byte{0x04}, ::std::byte{0x14}};
};

template <>
struct pkcs1v15_digest_info<::fast_io::sha224_context>
{
	static inline constexpr ::std::byte prefix[19]{
		::std::byte{0x30}, ::std::byte{0x2d}, ::std::byte{0x30}, ::std::byte{0x0d}, ::std::byte{0x06}, ::std::byte{0x09},
		::std::byte{0x60}, ::std::byte{0x86}, ::std::byte{0x48}, ::std::byte{0x01}, ::std::byte{0x65}, ::std::byte{0x03},
		::std::byte{0x04}, ::std::byte{0x02}, ::std::byte{0x04}, ::std::byte{0x05}, ::std::byte{0x00}, ::std::byte{0x04},
		::std::byte{0x1c}};
};

template <>
struct pkcs1v15_digest_info<::fast_io::sha256_context>
{
	static inline constexpr ::std::byte prefix[19]{
		::std::byte{0x30}, ::std::byte{0x31}, ::std::byte{0x30}, ::std::byte{0x0d}, ::std::byte{0x06}, ::std::byte{0x09},
		::std::byte{0x60}, ::std::byte{0x86}, ::std::byte{0x48}, ::std::byte{0x01}, ::std::byte{0x65}, ::std::byte{0x03},
		::std::byte{0x04}, ::std::byte{0x02}, ::std::byte{0x01}, ::std::byte{0x05}, ::std::byte{0x00}, ::std::byte{0x04},
		::std::byte{0x20}};
};

template <>
struct pkcs1v15_digest_info<::fast_io::sha384_context>
{
	static inline constexpr ::std::byte prefix[19]{
		::std::byte{0x30}, ::std::byte{0x41}, ::std::byte{0x30}, ::std::byte{0x0d}, ::std::byte{0x06}, ::std::byte{0x09},
		::std::byte{0x60}, ::std::byte{0x86}, ::std::byte{0x48}, ::std::byte{0x01}, ::std::byte{0x65}, ::std::byte{0x03},
		::std::byte{0x04}, ::std::byte{0x02}, ::std::byte{0x02}, ::std::byte{0x05}, ::std::byte{0x00}, ::std::byte{0x04},
		::std::byte{0x30}};
};

template <>
struct pkcs1v15_digest_info<::fast_io::sha512_context>
{
	static inline constexpr ::std::byte prefix[19]{
		::std::byte{0x30}, ::std::byte{0x51}, ::std::byte{0x30}, ::std::byte{0x0d}, ::std::byte{0x06}, ::std::byte{0x09},
		::std::byte{0x60}, ::std::byte{0x86}, ::std::byte{0x48}, ::std::byte{0x01}, ::std::byte{0x65}, ::std::byte{0x03},
		::std::byte{0x04}, ::std::byte{0x02}, ::std::byte{0x03}, ::std::byte{0x05}, ::std::byte{0x00}, ::std::byte{0x04},
		::std::byte{0x40}};
};

template <>
struct pkcs1v15_digest_info<::fast_io::sha512_224_context>
{
	static inline constexpr ::std::byte prefix[19]{
		::std::byte{0x30}, ::std::byte{0x2d}, ::std::byte{0x30}, ::std::byte{0x0d}, ::std::byte{0x06}, ::std::byte{0x09},
		::std::byte{0x60}, ::std::byte{0x86}, ::std::byte{0x48}, ::std::byte{0x01}, ::std::byte{0x65}, ::std::byte{0x03},
		::std::byte{0x04}, ::std::byte{0x02}, ::std::byte{0x05}, ::std::byte{0x05}, ::std::byte{0x00}, ::std::byte{0x04},
		::std::byte{0x1c}};
};

template <>
struct pkcs1v15_digest_info<::fast_io::sha512_256_context>
{
	static inline constexpr ::std::byte prefix[19]{
		::std::byte{0x30}, ::std::byte{0x31}, ::std::byte{0x30}, ::std::byte{0x0d}, ::std::byte{0x06}, ::std::byte{0x09},
		::std::byte{0x60}, ::std::byte{0x86}, ::std::byte{0x48}, ::std::byte{0x01}, ::std::byte{0x65}, ::std::byte{0x03},
		::std::byte{0x04}, ::std::byte{0x02}, ::std::byte{0x06}, ::std::byte{0x05}, ::std::byte{0x00}, ::std::byte{0x04},
		::std::byte{0x20}};
};

inline constexpr bool bytes_equal(::std::byte const *a, ::std::byte const *b, ::std::size_t n) noexcept
{
	char unsigned diff{};
	for (::std::size_t i{}; i != n; ++i)
	{
		diff |= static_cast<char unsigned>(a[i] ^ b[i]);
	}
	return diff == 0;
}

/*
EMSA-PKCS1-v1_5 check: em[0..k) must be
  00 || 01 || FF x (k - tlen - 3) || 00 || T   with T = prefix || digest
and the padding run must be at least 8 octets.
*/
inline constexpr bool emsa_pkcs1v15_check(::std::byte const *em, ::std::size_t k,
										  ::std::byte const *prefix, ::std::size_t prefix_size,
										  ::std::byte const *digest, ::std::size_t digest_size) noexcept
{
	::std::size_t const tlen{prefix_size + digest_size};
	if (k < tlen + 11 || em[0] != ::std::byte{} || em[1] != ::std::byte{1})
	{
		return false;
	}
	::std::size_t const pslen{k - tlen - 3};
	for (::std::size_t i{}; i != pslen; ++i)
	{
		if (em[2 + i] != ::std::byte{0xff})
		{
			return false;
		}
	}
	if (em[2 + pslen] != ::std::byte{})
	{
		return false;
	}
	return bytes_equal(em + 3 + pslen, prefix, prefix_size) &&
		   bytes_equal(em + 3 + pslen + prefix_size, digest, digest_size);
}

/*
MGF1 (RFC 8017 B.2.1) with hash context type ctx: out = concat over
counter i of Hash(seed || i as 4 big-endian bytes), truncated to outlen.
*/
template <typename ctx>
inline constexpr void mgf1(::std::byte *out, ::std::size_t outlen, ::std::byte const *seed, ::std::size_t seedlen) noexcept
{
	ctx hasher;
	::std::byte digest[ctx::digest_size];
	for (::std::uint_least32_t counter{}; outlen != 0; ++counter)
	{
		hasher.reset();
		hasher.update(seed, seed + seedlen);
		::std::byte const cbuf[4]{static_cast<::std::byte>(counter >> 24u), static_cast<::std::byte>(counter >> 16u),
								  static_cast<::std::byte>(counter >> 8u), static_cast<::std::byte>(counter)};
		hasher.update(cbuf, cbuf + 4);
		hasher.do_final();
		hasher.digest_to_byte_ptr(digest);
		::std::size_t const take{outlen < ctx::digest_size ? outlen : ctx::digest_size};
		for (::std::size_t i{}; i != take; ++i)
		{
			out[i] = digest[i];
		}
		out += take;
		outlen -= take;
	}
}

/*
EMSA-PSS-VERIFY (RFC 8017 9.1.2), salt length given explicitly
(TLS 1.3 rsa_pss_rsae_* uses salt_size == digest_size).
em is the encoded message (emLen = ceil((modulus_bits - 1) / 8) bytes),
mhash is Hash(message).
*/
template <typename ctx>
inline constexpr bool emsa_pss_verify(::std::byte const *em, ::std::size_t emlen, ::std::size_t embits,
									  ::std::byte const *mhash, ::std::size_t salt_size) noexcept
{
	constexpr ::std::size_t hlen{ctx::digest_size};
	if (emlen < hlen + salt_size + 2 || em[emlen - 1] != ::std::byte{0xbc})
	{
		return false;
	}
	::std::size_t const dblen{emlen - hlen - 1};
	::std::byte const *const maskeddb{em};
	::std::byte const *const h{em + dblen};
	unsigned const unused{static_cast<unsigned>(8 * emlen - embits)};
	/* leftmost 8*emLen - emBits bits of maskedDB must be zero */
	if (unused != 0 && (static_cast<unsigned>(maskeddb[0]) & (0xffu << (8 - unused))) != 0)
	{
		return false;
	}
	::std::byte db[1024];
	if (dblen > sizeof(db))
	{
		return false;
	}
	mgf1<ctx>(db, dblen, h, hlen);
	for (::std::size_t i{}; i != dblen; ++i)
	{
		db[i] ^= maskeddb[i];
	}
	if (unused != 0)
	{
		db[0] &= static_cast<::std::byte>(0xffu >> unused);
	}
	::std::size_t const zerolen{dblen - salt_size - 1};
	for (::std::size_t i{}; i != zerolen; ++i)
	{
		if (db[i] != ::std::byte{})
		{
			return false;
		}
	}
	if (db[zerolen] != ::std::byte{1})
	{
		return false;
	}
	ctx hasher;
	::std::byte const zeros[8]{};
	hasher.update(zeros, zeros + 8);
	hasher.update(mhash, mhash + hlen);
	hasher.update(db + zerolen + 1, db + zerolen + 1 + salt_size);
	hasher.do_final();
	::std::byte digest[hlen];
	hasher.digest_to_byte_ptr(digest);
	return bytes_equal(digest, h, hlen);
}

/*
EMSA-PSS-ENCODE (rfc8017 9.1.1), the mirror of emsa_pss_verify.
em_out receives emlen bytes:
  maskedDB || H || 0xbc
where DB = 0...0 || 01 || salt and maskedDB = DB xor MGF1(H).
salt is caller-supplied entropy of salt_size bytes. embits is the
encoded bit length (modulus_bits - 1 for TLS); the top unused bits of
the first byte are zeroed. Returns false when emlen cannot hold the
encoding.
*/
template <typename ctx>
inline constexpr bool emsa_pss_encode(::std::byte *em_out, ::std::size_t emlen, ::std::size_t embits,
									  ::std::byte const *mhash,
									  ::std::byte const *salt, ::std::size_t salt_size) noexcept
{
	constexpr ::std::size_t hlen{ctx::digest_size};
	if (emlen < hlen + salt_size + 2)
	{
		return false;
	}
	/* H = Hash(8 zero || mHash || salt) */
	ctx hasher;
	::std::byte const zeros[8]{};
	hasher.update(zeros, zeros + 8);
	hasher.update(mhash, mhash + hlen);
	hasher.update(salt, salt + salt_size);
	hasher.do_final();
	::std::byte digest[hlen];
	hasher.digest_to_byte_ptr(digest);

	::std::size_t const dblen{emlen - hlen - 1};
	::std::byte db[1024];
	if (dblen > sizeof(db))
	{
		return false;
	}
	::std::size_t const zerolen{dblen - salt_size - 1};
	for (::std::size_t i{}; i != zerolen; ++i)
	{
		db[i] = ::std::byte{};
	}
	db[zerolen] = ::std::byte{1};
	for (::std::size_t i{}; i != salt_size; ++i)
	{
		db[zerolen + 1 + i] = salt[i];
	}
	/* maskedDB = DB xor MGF1(H, dbLen) */
	mgf1<ctx>(em_out, dblen, digest, hlen);
	for (::std::size_t i{}; i != dblen; ++i)
	{
		em_out[i] ^= db[i];
	}
	unsigned const unused{static_cast<unsigned>(8 * emlen - embits)};
	if (unused != 0)
	{
		em_out[0] &= static_cast<::std::byte>(0xffu >> unused);
	}
	for (::std::size_t i{}; i != hlen; ++i)
	{
		em_out[dblen + i] = digest[i];
	}
	em_out[emlen - 1] = ::std::byte{0xbc};
	return true;
}

} // namespace fast_io::details::rsa
