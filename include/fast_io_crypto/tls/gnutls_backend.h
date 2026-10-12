#pragma once

/*
gnutls_crypto_backend: a fast_io::tls crypto backend whose primitives
are the gnutls low-level crypto API (gnutls_aead_cipher_*) plus nettle
curve25519_mul -- nettle is gnutls's own backend, already linked with
-lgnutls. The TLS protocol layer stays in fast_io; the libraries only
supply the math. Link with -lgnutls -lnettle.

The transcript hash and HKDF keep the fast_io implementations, same
reasoning as the openssl backend: they are tiny constexpr routines and
the provider handles are heap objects that cannot satisfy the key
schedule's noexcept machinery. Certificate signature verify likewise
stays on the fast_io parser for now.
*/

#if !defined(FAST_IO_TLS_HAS_GNUTLS_CRYPTO)
#if __has_include(<gnutls/crypto.h>) && __has_include(<nettle/curve25519.h>)
#define FAST_IO_TLS_HAS_GNUTLS_CRYPTO 1
#else
#define FAST_IO_TLS_HAS_GNUTLS_CRYPTO 0
#endif
#endif

#if FAST_IO_TLS_HAS_GNUTLS_CRYPTO

#include <gnutls/crypto.h>
#include <gnutls/abstract.h>
#include <nettle/curve25519.h>

namespace fast_io::tls::details
{

/* gnutls_digest context for the transcript. The handle is a heap
   object; allocation failure is OOM and terminates. Copies go through
   gnutls_hash_copy so transcript snapshots work. The mac is carried at
   runtime so one type serves every suite hash. */
struct gnutls_evp_hash_ctx
{
	gnutls_mac_algorithm_t mac_{};
	::std::size_t digest_size_{};
	::std::size_t block_size_{};
	gnutls_hash_hd_t ctx{};
	::std::byte digest[64]{};

	inline gnutls_evp_hash_ctx(gnutls_mac_algorithm_t mac) noexcept
		: mac_{mac},
		  digest_size_{static_cast<::std::size_t>(gnutls_hmac_get_len(mac))},
		  block_size_{mac == GNUTLS_MAC_SHA384 ? static_cast<::std::size_t>(128)
											   : static_cast<::std::size_t>(64)}
	{
	}
	inline gnutls_evp_hash_ctx(gnutls_evp_hash_ctx const &o) noexcept
		: mac_{o.mac_}, digest_size_{o.digest_size_}, block_size_{o.block_size_}
	{
		if (o.ctx != nullptr)
		{
			ctx = gnutls_hash_copy(o.ctx);
			if (ctx == nullptr)
			{
				::fast_io::fast_terminate();
			}
		}
		::fast_io::freestanding::non_overlapped_copy_n(o.digest, digest_size_, digest);
	}
	inline gnutls_evp_hash_ctx(gnutls_evp_hash_ctx &&o) noexcept
		: mac_{o.mac_}, digest_size_{o.digest_size_}, block_size_{o.block_size_}, ctx{o.ctx}
	{
		o.ctx = nullptr;
		::fast_io::freestanding::non_overlapped_copy_n(o.digest, digest_size_, digest);
	}
	inline ~gnutls_evp_hash_ctx()
	{
		if (ctx != nullptr)
		{
			gnutls_hash_deinit(ctx, nullptr);
		}
	}
	inline void ensure() noexcept
	{
		if (ctx == nullptr &&
			gnutls_hash_init(__builtin_addressof(ctx),
							 static_cast<gnutls_digest_algorithm_t>(mac_)) != 0)
		{
			::fast_io::fast_terminate();
		}
	}
	inline void update(::std::byte const *first, ::std::byte const *last) noexcept
	{
		this->ensure();
		if (gnutls_hash(ctx, first, static_cast<::std::size_t>(last - first)) != 0)
		{
			::fast_io::fast_terminate();
		}
	}
	inline void reset() noexcept
	{
		if (ctx != nullptr)
		{
			gnutls_hash_deinit(ctx, nullptr);
			ctx = nullptr;
		}
	}
	inline void do_final() noexcept
	{
		this->ensure();
		gnutls_hash_output(ctx, digest);
	}
	inline void digest_to_byte_ptr(::std::byte *ptr) const noexcept
	{
		::fast_io::freestanding::non_overlapped_copy_n(digest, digest_size_, ptr);
	}
};

} // namespace fast_io::tls::details

namespace fast_io::tls
{

struct gnutls_crypto_backend
{
	using md = gnutls_mac_algorithm_t;

	static inline constexpr md md_for(cipher_suite suite) noexcept
	{
		return suite == cipher_suite::aes_256_gcm_sha384 ? GNUTLS_MAC_SHA384 : GNUTLS_MAC_SHA256;
	}
	static inline ::std::size_t md_digest_size(md a) noexcept
	{
		return static_cast<::std::size_t>(gnutls_hmac_get_len(a));
	}
	static inline ::std::size_t md_block_size(md a) noexcept
	{
		return a == GNUTLS_MAC_SHA384 ? static_cast<::std::size_t>(128)
									  : static_cast<::std::size_t>(64);
	}

	struct hash_ctx : details::gnutls_evp_hash_ctx
	{
		hash_ctx(md m) noexcept : details::gnutls_evp_hash_ctx{m}
		{
		}
	};

	static inline void hkdf_extract(md a, ::std::byte *out,
									::std::byte const *salt, ::std::size_t salt_size,
									::std::byte const *ikm, ::std::size_t ikm_size) noexcept
	{
		gnutls_datum_t const ik{const_cast<char unsigned *>(
									reinterpret_cast<char unsigned const *>(ikm)),
								static_cast<unsigned>(ikm_size)};
		gnutls_datum_t const sl{const_cast<char unsigned *>(
									reinterpret_cast<char unsigned const *>(salt)),
								static_cast<unsigned>(salt_size)};
		if (gnutls_hkdf_extract(a, __builtin_addressof(ik), __builtin_addressof(sl), out) != 0)
		{
			::fast_io::fast_terminate();
		}
	}

	static inline void hkdf_expand(md a, ::std::byte *out, ::std::size_t out_size,
								   ::std::byte const *prk, ::std::size_t prk_size,
								   ::std::byte const *info, ::std::size_t info_size) noexcept
	{
		gnutls_datum_t const pk{const_cast<char unsigned *>(
									reinterpret_cast<char unsigned const *>(prk)),
								static_cast<unsigned>(prk_size)};
		gnutls_datum_t const inf{const_cast<char unsigned *>(
									 reinterpret_cast<char unsigned const *>(info)),
								 static_cast<unsigned>(info_size)};
		if (gnutls_hkdf_expand(a, __builtin_addressof(pk), __builtin_addressof(inf), out,
							   out_size) != 0)
		{
			::fast_io::fast_terminate();
		}
	}

	static inline void hmac(md a, ::std::byte *out,
							::std::byte const *key, ::std::size_t key_size,
							::std::byte const *data, ::std::size_t data_size) noexcept
	{
		if (gnutls_hmac_fast(a, key, key_size, data, data_size, out) != 0)
		{
			::fast_io::fast_terminate();
		}
	}

	static inline void x25519_keypair(::std::byte *pk, ::std::byte *sk) noexcept
	{
		curve25519_mul_g(reinterpret_cast<uint8_t *>(pk),
						 reinterpret_cast<uint8_t const *>(sk));
	}

	static inline void x25519_shared_secret(::std::byte *out, ::std::byte const *peer_pk,
											::std::byte *sk) noexcept
	{
		curve25519_mul(reinterpret_cast<uint8_t *>(out),
					   reinterpret_cast<uint8_t const *>(sk),
					   reinterpret_cast<uint8_t const *>(peer_pk));
	}

private:
	static inline gnutls_cipher_algorithm_t gnutls_tls_cipher(cipher_suite suite) noexcept
	{
		switch (suite)
		{
		case cipher_suite::aes_128_gcm_sha256:
			return GNUTLS_CIPHER_AES_128_GCM;
		case cipher_suite::aes_256_gcm_sha384:
			return GNUTLS_CIPHER_AES_256_GCM;
		case cipher_suite::chacha20_poly1305_sha256:
			return GNUTLS_CIPHER_CHACHA20_POLY1305;
		default:
			return GNUTLS_CIPHER_UNKNOWN;
		}
	}

public:
	static inline bool record_open(::std::byte *out, ::std::size_t &out_size,
								   content_type &inner_type, ::std::byte const *hdr5,
								   ::std::byte const *ct, ::std::size_t ct_size,
								   cipher_suite suite, ::std::byte const *key,
								   ::std::byte const *iv, ::std::uint_least64_t seq) noexcept
	{
		if (ct_size < 17) /* tag(16) + inner type byte */
		{
			return false;
		}
		::std::byte nonce[12];
		details::tls_nonce_to_ptr(nonce, iv, seq);
		::std::size_t const inner_size{ct_size - 16};

		gnutls_aead_cipher_hd_t h{};
		gnutls_datum_t const kd{const_cast<char unsigned *>(reinterpret_cast<char unsigned const *>(key)),
								static_cast<unsigned>(details::cipher_suite_key_size(suite))};
		if (gnutls_aead_cipher_init(__builtin_addressof(h), gnutls_tls_cipher(suite),
									__builtin_addressof(kd)) != 0)
		{
			::fast_io::fast_terminate();
		}
		::std::size_t inner{inner_size};
		int const ret{gnutls_aead_cipher_decrypt(
			h, nonce, 12, hdr5, 5, 16, ct, ct_size, out, __builtin_addressof(inner))};
		gnutls_aead_cipher_deinit(h);
		if (ret != 0)
		{
			return false;
		}
		/* strip zero padding, then the inner content type byte */
		while (inner != 0 && out[inner - 1] == ::std::byte{})
		{
			--inner;
		}
		if (inner == 0)
		{
			return false;
		}
		inner_type = static_cast<content_type>(out[--inner]);
		out_size = inner;
		return true;
	}

	static inline ::std::size_t
	record_seal_inner(::std::byte *out, ::std::byte const *inner, ::std::size_t inner_size,
					  cipher_suite suite, ::std::byte const *key, ::std::byte const *iv,
					  ::std::uint_least64_t seq) noexcept
	{
		::std::byte nonce[12];
		details::tls_nonce_to_ptr(nonce, iv, seq);
		::std::byte *p{details::record_header_write(out, content_type::application_data,
													static_cast<::std::uint_least16_t>(inner_size + 16))};
		::std::byte *const hdr{out};

		gnutls_aead_cipher_hd_t h{};
		gnutls_datum_t const kd{const_cast<char unsigned *>(reinterpret_cast<char unsigned const *>(key)),
								static_cast<unsigned>(details::cipher_suite_key_size(suite))};
		if (gnutls_aead_cipher_init(__builtin_addressof(h), gnutls_tls_cipher(suite),
									__builtin_addressof(kd)) != 0)
		{
			::fast_io::fast_terminate();
		}
		::std::size_t sealed{static_cast<::std::size_t>(out + 16640 + 16 - p)};
		if (gnutls_aead_cipher_encrypt(h, nonce, 12, hdr, 5, 16, inner, inner_size,
									   p, __builtin_addressof(sealed)) != 0)
		{
			::fast_io::fast_terminate();
		}
		gnutls_aead_cipher_deinit(h);
		return static_cast<::std::size_t>(p - out) + sealed;
	}

	static inline ::std::size_t
	record_seal(::std::byte *out, content_type inner_type, ::std::byte const *pt,
				::std::size_t pt_size, cipher_suite suite, ::std::byte const *key,
				::std::byte const *iv, ::std::uint_least64_t seq) noexcept
	{
		if (pt_size >= 16641)
		{
			::fast_io::fast_terminate();
		}
		::std::byte inner[16641];
		::fast_io::freestanding::non_overlapped_copy_n(pt, pt_size, inner);
		inner[pt_size] = static_cast<::std::byte>(inner_type);
		return record_seal_inner(out, inner, pt_size + 1, suite, key, iv, seq);
	}

private:
	static inline gnutls_pubkey_t gnutls_tls_spki_pkey(::std::byte const *spki_der,
													   ::std::size_t spki_der_size) noexcept
	{
		gnutls_pubkey_t pk{};
		if (gnutls_pubkey_init(__builtin_addressof(pk)) != 0)
		{
			::fast_io::fast_terminate();
		}
		gnutls_datum_t const spki{const_cast<char unsigned *>(
									  reinterpret_cast<char unsigned const *>(spki_der)),
								  static_cast<unsigned>(spki_der_size)};
		if (gnutls_pubkey_import(pk, __builtin_addressof(spki), GNUTLS_X509_FMT_DER) != 0)
		{
			gnutls_pubkey_deinit(pk);
			return nullptr;
		}
		return pk;
	}

	static inline details::x509_verify_result
	gnutls_tls_run_verify(gnutls_pubkey_t pk, gnutls_sign_algorithm_t algo,
						  ::std::byte const *signature, ::std::size_t signature_size,
						  ::std::byte const *data, ::std::size_t data_size) noexcept
	{
		gnutls_datum_t const sig{const_cast<char unsigned *>(
									 reinterpret_cast<char unsigned const *>(signature)),
								 static_cast<unsigned>(signature_size)};
		gnutls_datum_t const dat{const_cast<char unsigned *>(
									 reinterpret_cast<char unsigned const *>(data)),
								 static_cast<unsigned>(data_size)};
		int const ret{gnutls_pubkey_verify_data2(pk, algo, 0, __builtin_addressof(dat),
												 __builtin_addressof(sig))};
		return ret == 0                               ? details::x509_verify_result::ok
			   : ret == GNUTLS_E_PK_SIG_VERIFY_FAILED ? details::x509_verify_result::bad_signature
													  : details::x509_verify_result::malformed;
	}

	static inline details::x509_verify_result
	gnutls_tls_cert_sig_algo(details::x509_certificate const &cert,
							 gnutls_sign_algorithm_t &algo) noexcept
	{
		using details::der_oid_eq;
		if (der_oid_eq(cert.signature_algorithm_oid, details::oid::sha256_with_rsa))
		{
			algo = GNUTLS_SIGN_RSA_SHA256;
		}
		else if (der_oid_eq(cert.signature_algorithm_oid, details::oid::sha384_with_rsa))
		{
			algo = GNUTLS_SIGN_RSA_SHA384;
		}
		else if (der_oid_eq(cert.signature_algorithm_oid, details::oid::sha512_with_rsa))
		{
			algo = GNUTLS_SIGN_RSA_SHA512;
		}
		else if (der_oid_eq(cert.signature_algorithm_oid, details::oid::sha224_with_rsa))
		{
			algo = GNUTLS_SIGN_RSA_SHA224;
		}
		else if (der_oid_eq(cert.signature_algorithm_oid, details::oid::ecdsa_with_sha256))
		{
			algo = GNUTLS_SIGN_ECDSA_SHA256;
		}
		else if (der_oid_eq(cert.signature_algorithm_oid, details::oid::ecdsa_with_sha384))
		{
			algo = GNUTLS_SIGN_ECDSA_SHA384;
		}
		else if (der_oid_eq(cert.signature_algorithm_oid, details::oid::ecdsa_with_sha512))
		{
			algo = GNUTLS_SIGN_ECDSA_SHA512;
		}
		else if (der_oid_eq(cert.signature_algorithm_oid, details::oid::ed25519))
		{
			algo = GNUTLS_SIGN_EDDSA_ED25519;
		}
		else if (der_oid_eq(cert.signature_algorithm_oid, details::oid::rsassa_pss))
		{
			if (!cert.signature_algorithm_has_params)
			{
				return details::x509_verify_result::malformed;
			}
			details::rsassa_pss_params pp;
			if (!details::rsassa_pss_params_parse(cert.signature_algorithm_params, pp) ||
				!pp.has_hash || !pp.has_mgf_hash || !pp.has_salt ||
				pp.mgf_hash_oid.value_size != pp.hash_oid.value_size ||
				__builtin_memcmp(pp.mgf_hash_oid.value, pp.hash_oid.value,
								 pp.hash_oid.value_size) != 0)
			{
				return details::x509_verify_result::unsupported_algorithm;
			}
			if (der_oid_eq(pp.hash_oid, details::oid::sha256))
			{
				algo = GNUTLS_SIGN_RSA_PSS_SHA256;
			}
			else if (der_oid_eq(pp.hash_oid, details::oid::sha384))
			{
				algo = GNUTLS_SIGN_RSA_PSS_SHA384;
			}
			else if (der_oid_eq(pp.hash_oid, details::oid::sha512))
			{
				algo = GNUTLS_SIGN_RSA_PSS_SHA512;
			}
			else
			{
				return details::x509_verify_result::unsupported_algorithm;
			}
		}
		else
		{
			return details::x509_verify_result::unsupported_algorithm;
		}
		return details::x509_verify_result::ok;
	}

	static inline details::x509_verify_result
	gnutls_tls_scheme_algo(signature_scheme scheme, gnutls_sign_algorithm_t &algo) noexcept
	{
		switch (scheme)
		{
		case signature_scheme::rsa_pss_rsae_sha256:
			algo = GNUTLS_SIGN_RSA_PSS_RSAE_SHA256;
			break;
		case signature_scheme::rsa_pss_rsae_sha384:
			algo = GNUTLS_SIGN_RSA_PSS_RSAE_SHA384;
			break;
		case signature_scheme::rsa_pss_rsae_sha512:
			algo = GNUTLS_SIGN_RSA_PSS_RSAE_SHA512;
			break;
		case signature_scheme::rsa_pss_pss_sha256:
			algo = GNUTLS_SIGN_RSA_PSS_SHA256;
			break;
		case signature_scheme::rsa_pss_pss_sha384:
			algo = GNUTLS_SIGN_RSA_PSS_SHA384;
			break;
		case signature_scheme::rsa_pss_pss_sha512:
			algo = GNUTLS_SIGN_RSA_PSS_SHA512;
			break;
		case signature_scheme::ecdsa_secp256r1_sha256:
			algo = GNUTLS_SIGN_ECDSA_SECP256R1_SHA256;
			break;
		case signature_scheme::ecdsa_secp384r1_sha384:
			algo = GNUTLS_SIGN_ECDSA_SECP384R1_SHA384;
			break;
		case signature_scheme::ed25519:
			algo = GNUTLS_SIGN_EDDSA_ED25519;
			break;
		case signature_scheme::rsa_pkcs1_sha256:
			algo = GNUTLS_SIGN_RSA_SHA256;
			break;
		case signature_scheme::rsa_pkcs1_sha384:
			algo = GNUTLS_SIGN_RSA_SHA384;
			break;
		case signature_scheme::rsa_pkcs1_sha512:
			algo = GNUTLS_SIGN_RSA_SHA512;
			break;
		default:
			return details::x509_verify_result::unsupported_algorithm;
		}
		return details::x509_verify_result::ok;
	}

public:
	static inline details::x509_verify_result
	cert_sig_verify(details::x509_certificate const &cert,
					details::x509_certificate const &issuer) noexcept
	{
		gnutls_pubkey_t pk{gnutls_tls_spki_pkey(issuer.spki_der, issuer.spki_der_size)};
		if (pk == nullptr)
		{
			return details::x509_verify_result::unsupported_algorithm;
		}
		gnutls_sign_algorithm_t algo{};
		details::x509_verify_result res{gnutls_tls_cert_sig_algo(cert, algo)};
		if (res == details::x509_verify_result::ok)
		{
			res = gnutls_tls_run_verify(pk, algo, cert.signature, cert.signature_size,
										cert.tbs, cert.tbs_size);
		}
		gnutls_pubkey_deinit(pk);
		return res;
	}

	static inline details::x509_verify_result
	cert_cv_verify(signature_scheme scheme, ::std::byte const *covered,
				   ::std::size_t covered_size, ::std::byte const *signature,
				   ::std::size_t signature_size,
				   details::x509_certificate const &leaf) noexcept
	{
		gnutls_pubkey_t pk{gnutls_tls_spki_pkey(leaf.spki_der, leaf.spki_der_size)};
		if (pk == nullptr)
		{
			return details::x509_verify_result::unsupported_algorithm;
		}
		gnutls_sign_algorithm_t algo{};
		details::x509_verify_result res{gnutls_tls_scheme_algo(scheme, algo)};
		if (res == details::x509_verify_result::ok)
		{
			res = gnutls_tls_run_verify(pk, algo, signature, signature_size, covered,
										covered_size);
		}
		gnutls_pubkey_deinit(pk);
		return res;
	}

	/*
	server-side CertificateVerify signature. Signing is a once-per-
	handshake op -- not throughput-critical -- so it stays on the
	in-repo DER parse and primitives, shared with the fast_io backend.
	*/
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
		bool const ok{details::tls_cv_sign(scheme, covered, covered_size, key, salt,
										   sig_out, sig_size)};
		::fast_io::secure_clear(__builtin_addressof(key), sizeof(key));
		return ok;
	}
};

} // namespace fast_io::tls

#endif /* FAST_IO_TLS_HAS_GNUTLS_CRYPTO */
