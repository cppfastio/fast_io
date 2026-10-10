#pragma once

/*
crypto primitive provider for basic_tls_client. The protocol machinery
-- record framing, sequence numbers, key schedule, X.509 parse, chain
and SAN checks -- always stays in fast_io; the backend only supplies the
math primitives. Substituting a provider (OpenSSL EVP, GnuTLS, Windows
CNG/bcrypt) swaps the algorithm implementations while the protocol,
kTLS offload and async paths are untouched; the provider's library is
already linked, so fast_io's AES/SHA/X25519/RSA instantiations drop out
of the binary.

A backend supplies:

	md                   a hash descriptor selected per cipher suite at
						   runtime -- md_for/md_digest_size/md_block_size
	hash_ctx             transcript context built on an md: update /
						   do_final / digest_to_byte_ptr / reset / copy
	hkdf_extract         HKDF-Extract(salt, ikm) -> prk[digest_size]
	hkdf_expand          HKDF-Expand(prk, info, out_size) -> out
	hmac                 HMAC(md, key, data) -> out[digest_size]
	x25519_keypair         clamp sk[32], write pk[32]
	x25519_shared_secret   X25519(sk, peer_pk) -> out[32]; the caller
						   checks the all-zero result
	record_open/seal       suite-dispatched AEAD on TLS 1.3 records
	cert_sig_verify        cert.signature over cert.tbs under the
						   issuer's SPKI -- DER parsing stays ours,
						   only the signature math delegates; the
						   issuer cert is passed whole so providers
						   can import its spki_der directly
	cert_cv_verify         the CertificateVerify signature over the
						   transcript, leaf cert passed whole
	cert_cv_sign           the server-side CertificateVerify signature:
						   covered content signed per scheme with the
						   private key (DER in -- PKCS#8, PKCS#1 or a
						   raw ed25519 key). salt supplies PSS entropy;
						   deterministic schemes ignore it

The digest and HKDF ops take the md descriptor so a single handshake
flight2 instantiates once per backend instead of once per suite hash.
*/

namespace fast_io::tls::details
{

/* runtime hash-algorithm tag for the fast_io backend's dyn dispatch */
enum class tls_hash_alg : ::std::uint_least8_t
{
	sha256,
	sha384
};

} // namespace fast_io::tls::details

namespace fast_io::tls
{

struct fast_io_crypto_backend
{
	using md = details::tls_hash_alg;

	static inline constexpr md md_for(cipher_suite suite) noexcept
	{
		return suite == cipher_suite::aes_256_gcm_sha384 ? md::sha384 : md::sha256;
	}
	static inline constexpr ::std::size_t md_digest_size(md a) noexcept
	{
		return a == md::sha384 ? 48 : 32;
	}
	static inline constexpr ::std::size_t md_block_size(md a) noexcept
	{
		return a == md::sha384 ? 128 : 64;
	}

	/* dyn transcript ctx: dispatches between the two fast_io contexts.
	   trivially copyable -- transcript snapshots are value copies */
	struct hash_ctx
	{
		::fast_io::sha256_context s256{};
		::fast_io::sha384_context s384{};
		::std::byte digest[64]{};
		md a{md::sha256};

		constexpr hash_ctx(md m) noexcept : a{m}
		{
		}
		inline void update(::std::byte const *first, ::std::byte const *last) noexcept
		{
			if (a == md::sha384)
			{
				s384.update(first, last);
			}
			else
			{
				s256.update(first, last);
			}
		}
		inline void reset() noexcept
		{
			if (a == md::sha384)
			{
				s384.reset();
			}
			else
			{
				s256.reset();
			}
		}
		inline void do_final() noexcept
		{
			if (a == md::sha384)
			{
				s384.do_final();
				s384.digest_to_byte_ptr(digest);
			}
			else
			{
				s256.do_final();
				s256.digest_to_byte_ptr(digest);
			}
		}
		inline void digest_to_byte_ptr(::std::byte *ptr) const noexcept
		{
			::fast_io::freestanding::non_overlapped_copy_n(digest, md_digest_size(a), ptr);
		}
	};

	static inline void hkdf_extract(md a, ::std::byte *out,
									::std::byte const *salt, ::std::size_t salt_size,
									::std::byte const *ikm, ::std::size_t ikm_size) noexcept
	{
		if (a == md::sha384)
		{
			details::hkdf_extract_to_ptr<::fast_io::sha384_context>(out, salt, salt_size,
																	ikm, ikm_size);
		}
		else
		{
			details::hkdf_extract_to_ptr<::fast_io::sha256_context>(out, salt, salt_size,
																	ikm, ikm_size);
		}
	}

	static inline void hkdf_expand(md a, ::std::byte *out, ::std::size_t out_size,
								   ::std::byte const *prk, ::std::size_t,
								   ::std::byte const *info, ::std::size_t info_size) noexcept
	{
		if (a == md::sha384)
		{
			details::hkdf_expand_to_ptr<::fast_io::sha384_context>(out, out_size, prk,
																   info, info_size);
		}
		else
		{
			details::hkdf_expand_to_ptr<::fast_io::sha256_context>(out, out_size, prk,
																   info, info_size);
		}
	}

	static inline void hmac(md a, ::std::byte *out,
							::std::byte const *key, ::std::size_t key_size,
							::std::byte const *data, ::std::size_t data_size) noexcept
	{
		if (a == md::sha384)
		{
			::fast_io::hmac_once_to_ptr<::fast_io::sha384_context>(out, key, key_size,
																   data, data_size);
		}
		else
		{
			::fast_io::hmac_once_to_ptr<::fast_io::sha256_context>(out, key, key_size,
																   data, data_size);
		}
	}

	static inline void x25519_keypair(::std::byte *pk, ::std::byte *sk) noexcept
	{
		::fast_io::diffie_hellman::x25519::trim_secret_key(
			::fast_io::containers::index_span<::std::byte, 32>{
				::fast_io::containers::index_unchecked, sk});
		::fast_io::diffie_hellman::x25519::calculate_public_key_to_ptr(pk, sk);
	}

	static inline void x25519_shared_secret(::std::byte *out, ::std::byte const *peer_pk,
											::std::byte *sk) noexcept
	{
		::fast_io::diffie_hellman::x25519::create_shared_key_to_ptr(out, peer_pk, sk);
	}

	static inline bool record_open(::std::byte *out, ::std::size_t &out_size,
								   content_type &inner_type, ::std::byte const *hdr5,
								   ::std::byte const *ct, ::std::size_t ct_size,
								   cipher_suite suite, ::std::byte const *key,
								   ::std::byte const *iv, ::std::uint_least64_t seq) noexcept
	{
		return details::tls_record_open(out, out_size, inner_type, hdr5, ct,
										ct_size, suite, key, iv, seq);
	}

	static inline ::std::size_t
	record_seal_inner(::std::byte *out, ::std::byte const *inner, ::std::size_t inner_size,
					  cipher_suite suite, ::std::byte const *key, ::std::byte const *iv,
					  ::std::uint_least64_t seq) noexcept
	{
		return details::tls_record_seal_inner(out, inner, inner_size, suite, key, iv, seq);
	}

	static inline ::std::size_t
	record_seal(::std::byte *out, content_type inner_type, ::std::byte const *pt,
				::std::size_t pt_size, cipher_suite suite, ::std::byte const *key,
				::std::byte const *iv, ::std::uint_least64_t seq) noexcept
	{
		return details::tls_record_seal(out, inner_type, pt, pt_size, suite, key, iv, seq);
	}

	static inline details::x509_verify_result
	cert_sig_verify(details::x509_certificate const &cert,
					details::x509_certificate const &issuer) noexcept
	{
		return details::x509_verify_signature(cert, issuer.spki_algorithm, issuer.public_key,
											  issuer.public_key_size);
	}

	static inline details::x509_verify_result
	cert_cv_verify(signature_scheme scheme, ::std::byte const *covered,
				   ::std::size_t covered_size, ::std::byte const *signature,
				   ::std::size_t signature_size,
				   details::x509_certificate const &leaf) noexcept
	{
		return details::tls_certificate_verify(scheme, covered, covered_size, signature,
											   signature_size, leaf.spki_algorithm, leaf.public_key,
											   leaf.public_key_size);
	}

	/* sig_out must hold the modulus size (rsa) / 64 (ed25519) */
	static inline bool cert_cv_sign(signature_scheme scheme,
									::std::byte const *covered, ::std::size_t covered_size,
									::std::byte const *pkey_der, ::std::size_t pkey_size,
									::std::byte const *salt,
									::std::byte *sig_out, ::std::size_t *sig_size) noexcept
	{
		details::tls_pkey key;
		if (!details::tls_pkey_parse(__builtin_addressof(key), pkey_der, pkey_size))
		{
			return false;
		}
		bool const ok{details::tls_cv_sign(scheme, covered, covered_size, key, salt, sig_out, sig_size)};
		::fast_io::secure_clear(__builtin_addressof(key), sizeof(key));
		return ok;
	}
};

} // namespace fast_io::tls
