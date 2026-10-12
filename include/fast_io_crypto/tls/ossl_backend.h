#pragma once

/*
ossl_crypto_backend: a fast_io::tls crypto backend whose primitives are
OpenSSL EVP calls. The TLS protocol layer -- record framing, sequence
numbers, key schedule, X.509 parse/chain/SAN -- stays in fast_io;
OpenSSL only supplies the math. Since -lssl -lcrypto is already linked
the fast_io AEAD and X25519 instantiations drop out of the binary.

The transcript hash and HKDF keep the fast_io implementations: they are
tiny constexpr table-free routines, while EVP digest contexts are heap
objects that cannot satisfy the constexpr noexcept shape the key
schedule requires. Certificate signature verify likewise stays on the
fast_io parser for now -- the ops that matter for throughput (record
AEAD) and state size (X25519) are the EVP ones.
*/

#if !defined(FAST_IO_TLS_HAS_OSSL_CRYPTO)
#if __has_include(<openssl/evp.h>)
#define FAST_IO_TLS_HAS_OSSL_CRYPTO 1
#else
#define FAST_IO_TLS_HAS_OSSL_CRYPTO 0
#endif
#endif

#if FAST_IO_TLS_HAS_OSSL_CRYPTO

#include <openssl/evp.h>
#include <openssl/kdf.h>
#include <openssl/core_names.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>

namespace fast_io::tls::details
{

/* EVP message-digest context for the transcript. The ctx is a heap
   object -- allocation failure is OOM, which terminates. Copies go
   through EVP_MD_CTX_copy_ex so transcript snapshots work. The md is
   carried at runtime so one type serves every suite hash. */
struct ossl_evp_hash_ctx
{
	EVP_MD const *md_{};
	::std::size_t digest_size_{};
	::std::size_t block_size_{};
	EVP_MD_CTX *ctx{};
	::std::byte digest[64]{};

	inline ossl_evp_hash_ctx(EVP_MD const *md) noexcept
		: md_{md}, digest_size_{static_cast<::std::size_t>(EVP_MD_size(md))},
		  block_size_{static_cast<::std::size_t>(EVP_MD_block_size(md))}
	{
	}
	inline ossl_evp_hash_ctx(ossl_evp_hash_ctx const &o) noexcept
		: md_{o.md_}, digest_size_{o.digest_size_}, block_size_{o.block_size_}
	{
		if (o.ctx != nullptr)
		{
			ctx = EVP_MD_CTX_new();
			if (ctx == nullptr)
			{
				::fast_io::fast_terminate();
			}
			if (EVP_MD_CTX_copy_ex(ctx, o.ctx) != 1)
			{
				::fast_io::fast_terminate();
			}
		}
		::fast_io::freestanding::non_overlapped_copy_n(o.digest, digest_size_, digest);
	}
	inline ossl_evp_hash_ctx(ossl_evp_hash_ctx &&o) noexcept
		: md_{o.md_}, digest_size_{o.digest_size_}, block_size_{o.block_size_}, ctx{o.ctx}
	{
		o.ctx = nullptr;
		::fast_io::freestanding::non_overlapped_copy_n(o.digest, digest_size_, digest);
	}
	inline ~ossl_evp_hash_ctx()
	{
		EVP_MD_CTX_free(ctx);
	}
	inline void ensure() noexcept
	{
		if (ctx == nullptr)
		{
			ctx = EVP_MD_CTX_new();
			if (ctx == nullptr)
			{
				::fast_io::fast_terminate();
			}
			if (EVP_DigestInit_ex(ctx, md_, nullptr) != 1)
			{
				::fast_io::fast_terminate();
			}
		}
	}
	inline void update(::std::byte const *first, ::std::byte const *last) noexcept
	{
		this->ensure();
		if (EVP_DigestUpdate(ctx, first, static_cast<::std::size_t>(last - first)) != 1)
		{
			::fast_io::fast_terminate();
		}
	}
	inline void reset() noexcept
	{
		if (ctx != nullptr)
		{
			EVP_MD_CTX_reset(ctx);
			if (EVP_DigestInit_ex(ctx, md_, nullptr) != 1)
			{
				::fast_io::fast_terminate();
			}
		}
	}
	inline void do_final() noexcept
	{
		this->ensure();
		char unsigned out[EVP_MAX_MD_SIZE];
		if (EVP_DigestFinal_ex(ctx, out, nullptr) != 1)
		{
			::fast_io::fast_terminate();
		}
		::fast_io::freestanding::non_overlapped_copy_n(
			reinterpret_cast<::std::byte *>(out), digest_size_, digest);
	}
	inline void digest_to_byte_ptr(::std::byte *ptr) const noexcept
	{
		::fast_io::freestanding::non_overlapped_copy_n(digest, digest_size_, ptr);
	}
};

} // namespace fast_io::tls::details

namespace fast_io::tls
{

struct ossl_crypto_backend
{
	using md = EVP_MD const *;

	static inline constexpr md md_for(cipher_suite suite) noexcept
	{
		return suite == cipher_suite::aes_256_gcm_sha384 ? EVP_sha384() : EVP_sha256();
	}
	static inline ::std::size_t md_digest_size(md a) noexcept
	{
		return static_cast<::std::size_t>(EVP_MD_size(a));
	}
	static inline ::std::size_t md_block_size(md a) noexcept
	{
		return static_cast<::std::size_t>(EVP_MD_block_size(a));
	}

	struct hash_ctx : details::ossl_evp_hash_ctx
	{
		hash_ctx(md m) noexcept : details::ossl_evp_hash_ctx{m}
		{
		}
	};

	/* HKDF over the EVP_KDF provider api (openssl 3.x) -- a missing
	   provider/context is OOM and terminates. */
	static inline void hkdf_extract(md a, ::std::byte *out,
									::std::byte const *salt, ::std::size_t salt_size,
									::std::byte const *ikm, ::std::size_t ikm_size) noexcept
	{
		::std::byte zero_salt[64]{};
		if (salt == nullptr)
		{
			salt = zero_salt;
			salt_size = md_digest_size(a);
		}
		EVP_KDF *kdf{EVP_KDF_fetch(nullptr, "HKDF", nullptr)};
		if (kdf == nullptr)
		{
			::fast_io::fast_terminate();
		}
		EVP_KDF_CTX *c{EVP_KDF_CTX_new(kdf)};
		if (c == nullptr)
		{
			EVP_KDF_free(kdf);
			::fast_io::fast_terminate();
		}
		char const *digest{EVP_MD_get0_name(a)};
		int kdf_extract_mode{EVP_KDF_HKDF_MODE_EXTRACT_ONLY};
		OSSL_PARAM params[] = {
			OSSL_PARAM_construct_utf8_string(
				const_cast<char *>(OSSL_KDF_PARAM_DIGEST), const_cast<char *>(digest), 0),
			OSSL_PARAM_construct_int(const_cast<char *>(OSSL_KDF_PARAM_MODE),
									 __builtin_addressof(kdf_extract_mode)),
			OSSL_PARAM_construct_octet_string(
				const_cast<char *>(OSSL_KDF_PARAM_KEY),
				const_cast<::std::byte *>(ikm), ikm_size),
			OSSL_PARAM_construct_octet_string(
				const_cast<char *>(OSSL_KDF_PARAM_SALT),
				const_cast<::std::byte *>(salt), salt_size),
			OSSL_PARAM_construct_end()};
		if (EVP_KDF_derive(c, reinterpret_cast<char unsigned *>(out),
						   md_digest_size(a), params) != 1)
		{
			EVP_KDF_CTX_free(c);
			EVP_KDF_free(kdf);
			::fast_io::fast_terminate();
		}
		EVP_KDF_CTX_free(c);
		EVP_KDF_free(kdf);
	}

	static inline void hkdf_expand(md a, ::std::byte *out, ::std::size_t out_size,
								   ::std::byte const *prk, ::std::size_t prk_size,
								   ::std::byte const *info, ::std::size_t info_size) noexcept
	{
		EVP_KDF *kdf{EVP_KDF_fetch(nullptr, "HKDF", nullptr)};
		if (kdf == nullptr)
		{
			::fast_io::fast_terminate();
		}
		EVP_KDF_CTX *c{EVP_KDF_CTX_new(kdf)};
		if (c == nullptr)
		{
			EVP_KDF_free(kdf);
			::fast_io::fast_terminate();
		}
		char const *digest{EVP_MD_get0_name(a)};
		int kdf_expand_mode{EVP_KDF_HKDF_MODE_EXPAND_ONLY};
		OSSL_PARAM params[] = {
			OSSL_PARAM_construct_utf8_string(
				const_cast<char *>(OSSL_KDF_PARAM_DIGEST), const_cast<char *>(digest), 0),
			OSSL_PARAM_construct_int(const_cast<char *>(OSSL_KDF_PARAM_MODE),
									 __builtin_addressof(kdf_expand_mode)),
			OSSL_PARAM_construct_octet_string(
				const_cast<char *>(OSSL_KDF_PARAM_KEY),
				const_cast<::std::byte *>(prk), prk_size),
			OSSL_PARAM_construct_octet_string(
				const_cast<char *>(OSSL_KDF_PARAM_INFO),
				const_cast<::std::byte *>(info), info_size),
			OSSL_PARAM_construct_end()};
		if (EVP_KDF_derive(c, reinterpret_cast<char unsigned *>(out),
						   out_size, params) != 1)
		{
			EVP_KDF_CTX_free(c);
			EVP_KDF_free(kdf);
			::fast_io::fast_terminate();
		}
		EVP_KDF_CTX_free(c);
		EVP_KDF_free(kdf);
	}

	static inline void hmac(md a, ::std::byte *out,
							::std::byte const *key, ::std::size_t key_size,
							::std::byte const *data, ::std::size_t data_size) noexcept
	{
		EVP_MAC *mac{EVP_MAC_fetch(nullptr, "HMAC", nullptr)};
		if (mac == nullptr)
		{
			::fast_io::fast_terminate();
		}
		EVP_MAC_CTX *c{EVP_MAC_CTX_new(mac)};
		if (c == nullptr)
		{
			EVP_MAC_free(mac);
			::fast_io::fast_terminate();
		}
		char const *name{EVP_MD_get0_name(a)};
		OSSL_PARAM params[] = {
			OSSL_PARAM_construct_utf8_string(const_cast<char *>("digest"),
											 const_cast<char *>(name), 0),
			OSSL_PARAM_construct_end()};
		::std::size_t out_size{md_digest_size(a)};
		if (EVP_MAC_init(c, reinterpret_cast<char unsigned const *>(key), key_size, params) != 1 ||
			EVP_MAC_update(c, reinterpret_cast<char unsigned const *>(data), data_size) != 1 ||
			EVP_MAC_final(c, reinterpret_cast<char unsigned *>(out), __builtin_addressof(out_size), out_size) != 1)
		{
			EVP_MAC_CTX_free(c);
			EVP_MAC_free(mac);
			::fast_io::fast_terminate();
		}
		EVP_MAC_CTX_free(c);
		EVP_MAC_free(mac);
	}

	static inline void x25519_keypair(::std::byte *pk, ::std::byte *sk) noexcept
	{
		EVP_PKEY *skp{EVP_PKEY_new_raw_private_key(EVP_PKEY_X25519, nullptr,
												   reinterpret_cast<char unsigned const *>(sk), 32)};
		if (skp == nullptr)
		{
			::fast_io::fast_terminate();
		}
		::std::size_t n{32};
		if (EVP_PKEY_get_raw_public_key(skp, reinterpret_cast<char unsigned *>(pk), __builtin_addressof(n)) != 1 || n != 32)
		{
			::fast_io::fast_terminate();
		}
		EVP_PKEY_free(skp);
	}

	static inline void x25519_shared_secret(::std::byte *out, ::std::byte const *peer_pk,
											::std::byte *sk) noexcept
	{
		EVP_PKEY *skp{EVP_PKEY_new_raw_private_key(EVP_PKEY_X25519, nullptr,
												   reinterpret_cast<char unsigned const *>(sk), 32)};
		EVP_PKEY *pkp{EVP_PKEY_new_raw_public_key(EVP_PKEY_X25519, nullptr,
												  reinterpret_cast<char unsigned const *>(peer_pk), 32)};
		EVP_PKEY_CTX *d{skp ? EVP_PKEY_CTX_new(skp, nullptr) : nullptr};
		if (d == nullptr || pkp == nullptr)
		{
			::fast_io::fast_terminate();
		}
		::std::size_t n{32};
		if (EVP_PKEY_derive_init(d) != 1 || EVP_PKEY_derive_set_peer(d, pkp) != 1 ||
			EVP_PKEY_derive(d, reinterpret_cast<char unsigned *>(out), __builtin_addressof(n)) != 1 ||
			n != 32)
		{
			::fast_io::fast_terminate();
		}
		EVP_PKEY_CTX_free(d);
		EVP_PKEY_free(pkp);
		EVP_PKEY_free(skp);
	}

private:
	static inline EVP_CIPHER const *ossl_tls_cipher(cipher_suite suite) noexcept
	{
		switch (suite)
		{
		case cipher_suite::aes_128_gcm_sha256:
			return EVP_aes_128_gcm();
		case cipher_suite::aes_256_gcm_sha384:
			return EVP_aes_256_gcm();
		case cipher_suite::chacha20_poly1305_sha256:
			return EVP_chacha20_poly1305();
		default:
			return nullptr;
		}
	}

	static inline ::std::size_t ossl_tls_key_size(cipher_suite suite) noexcept
	{
		return suite == cipher_suite::aes_256_gcm_sha384 || suite == cipher_suite::chacha20_poly1305_sha256
				   ? 32
				   : 16;
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

		EVP_CIPHER_CTX *x{EVP_CIPHER_CTX_new()};
		if (x == nullptr)
		{
			::fast_io::fast_terminate();
		}
		int l{}, l2{};
		bool ok{EVP_DecryptInit_ex(x, ossl_tls_cipher(suite), nullptr, nullptr, nullptr) == 1 &&
				EVP_CIPHER_CTX_ctrl(x, EVP_CTRL_AEAD_SET_IVLEN, 12, nullptr) == 1 &&
				EVP_DecryptInit_ex(x, nullptr, nullptr,
								   reinterpret_cast<char unsigned const *>(key),
								   reinterpret_cast<char unsigned const *>(nonce)) == 1 &&
				EVP_DecryptUpdate(x, nullptr, __builtin_addressof(l),
								  reinterpret_cast<char unsigned const *>(hdr5), 5) == 1 &&
				EVP_CIPHER_CTX_ctrl(x, EVP_CTRL_AEAD_SET_TAG, 16,
									const_cast<::std::byte *>(ct + inner_size)) == 1 &&
				EVP_DecryptUpdate(x, reinterpret_cast<char unsigned *>(out), __builtin_addressof(l),
								  reinterpret_cast<char unsigned const *>(ct),
								  static_cast<int>(inner_size)) == 1 &&
				EVP_DecryptFinal_ex(x, reinterpret_cast<char unsigned *>(out) + l,
									__builtin_addressof(l2)) == 1};
		EVP_CIPHER_CTX_free(x);
		if (!ok)
		{
			/* EVP reports tag mismatch and ciphertext errors the same
			   way -- either is a bad_record_mac */
			return false;
		}
		::std::size_t inner{static_cast<::std::size_t>(l)};
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

		EVP_CIPHER_CTX *x{EVP_CIPHER_CTX_new()};
		if (x == nullptr)
		{
			::fast_io::fast_terminate();
		}
		int l{}, l2{};
		::std::byte tag[16];
		bool ok{EVP_EncryptInit_ex(x, ossl_tls_cipher(suite), nullptr, nullptr, nullptr) == 1 &&
				EVP_CIPHER_CTX_ctrl(x, EVP_CTRL_AEAD_SET_IVLEN, 12, nullptr) == 1 &&
				EVP_EncryptInit_ex(x, nullptr, nullptr,
								   reinterpret_cast<char unsigned const *>(key),
								   reinterpret_cast<char unsigned const *>(nonce)) == 1 &&
				EVP_EncryptUpdate(x, nullptr, __builtin_addressof(l),
								  reinterpret_cast<char unsigned const *>(hdr), 5) == 1 &&
				EVP_EncryptUpdate(x, reinterpret_cast<char unsigned *>(p), __builtin_addressof(l),
								  reinterpret_cast<char unsigned const *>(inner),
								  static_cast<int>(inner_size)) == 1 &&
				EVP_EncryptFinal_ex(x, reinterpret_cast<char unsigned *>(p) + l,
									__builtin_addressof(l2)) == 1 &&
				EVP_CIPHER_CTX_ctrl(x, EVP_CTRL_AEAD_GET_TAG, 16,
									reinterpret_cast<char unsigned *>(tag)) == 1};
		EVP_CIPHER_CTX_free(x);
		if (!ok)
		{
			::fast_io::fast_terminate();
		}
		::std::byte *tag_out{p + l + l2};
		::fast_io::freestanding::non_overlapped_copy_n(tag, 16, tag_out);
		return static_cast<::std::size_t>(tag_out - out) + 16;
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
	static inline EVP_PKEY *ossl_tls_spki_pkey(::std::byte const *spki_der,
											   ::std::size_t spki_der_size) noexcept
	{
		char unsigned const *d{reinterpret_cast<char unsigned const *>(spki_der)};
		return d2i_PUBKEY(nullptr, __builtin_addressof(d),
						  static_cast<long>(spki_der_size));
	}

	/* hash oid -> EVP_MD; unsupported digests return nullptr */
	static inline EVP_MD const *ossl_tls_hash_oid(details::der_tlv const &oid) noexcept
	{
		using details::der_oid_eq;
		if (der_oid_eq(oid, details::oid::sha256))
		{
			return EVP_sha256();
		}
		if (der_oid_eq(oid, details::oid::sha384))
		{
			return EVP_sha384();
		}
		if (der_oid_eq(oid, details::oid::sha512))
		{
			return EVP_sha512();
		}
		if (der_oid_eq(oid, details::oid::sha224))
		{
			return EVP_sha224();
		}
		return nullptr;
	}

	static inline EVP_MD const *ossl_tls_scheme_md(signature_scheme scheme) noexcept
	{
		switch (scheme)
		{
		case signature_scheme::rsa_pkcs1_sha256:
		case signature_scheme::rsa_pss_rsae_sha256:
		case signature_scheme::rsa_pss_pss_sha256:
		case signature_scheme::ecdsa_secp256r1_sha256:
			return EVP_sha256();
		case signature_scheme::rsa_pkcs1_sha384:
		case signature_scheme::rsa_pss_rsae_sha384:
		case signature_scheme::rsa_pss_pss_sha384:
		case signature_scheme::ecdsa_secp384r1_sha384:
			return EVP_sha384();
		case signature_scheme::rsa_pkcs1_sha512:
		case signature_scheme::rsa_pss_rsae_sha512:
		case signature_scheme::rsa_pss_pss_sha512:
			return EVP_sha512();
		default:
			return nullptr;
		}
	}

	static inline details::x509_verify_result
	ossl_tls_run_verify(EVP_PKEY *pk, EVP_MD const *md, bool pss, int saltlen,
						::std::byte const *signature, ::std::size_t signature_size,
						::std::byte const *data, ::std::size_t data_size) noexcept
	{
		EVP_MD_CTX *c{EVP_MD_CTX_new()};
		if (c == nullptr)
		{
			::fast_io::fast_terminate();
		}
		EVP_PKEY_CTX *pctx{};
		int init{EVP_DigestVerifyInit(c, __builtin_addressof(pctx), md, nullptr, pk)};
		int ret{-1};
		if (init == 1)
		{
			bool params_ok{true};
			if (pss)
			{
				params_ok = EVP_PKEY_CTX_set_rsa_padding(pctx, RSA_PKCS1_PSS_PADDING) > 0 &&
							EVP_PKEY_CTX_set_rsa_pss_saltlen(pctx, saltlen) > 0 &&
							EVP_PKEY_CTX_set_rsa_mgf1_md(pctx, md) > 0;
			}
			if (params_ok)
			{
				ret = EVP_DigestVerify(c,
									   reinterpret_cast<char unsigned const *>(signature),
									   signature_size,
									   reinterpret_cast<char unsigned const *>(data),
									   data_size);
			}
		}
		EVP_MD_CTX_free(c);
		if (ret == 1)
		{
			return details::x509_verify_result::ok;
		}
		if (ret == 0)
		{
			return details::x509_verify_result::bad_signature;
		}
		return details::x509_verify_result::malformed;
	}

	static inline details::x509_verify_result
	ossl_tls_run_oneshot(EVP_PKEY *pk, ::std::byte const *signature,
						 ::std::size_t signature_size, ::std::byte const *data,
						 ::std::size_t data_size) noexcept
	{
		/* Ed25519/Ed448 style: no digest, message signed whole */
		EVP_MD_CTX *c{EVP_MD_CTX_new()};
		if (c == nullptr)
		{
			::fast_io::fast_terminate();
		}
		int ret{EVP_DigestVerifyInit(c, nullptr, nullptr, nullptr, pk)};
		if (ret == 1)
		{
			ret = EVP_DigestVerify(c, reinterpret_cast<char unsigned const *>(signature),
								   signature_size, reinterpret_cast<char unsigned const *>(data),
								   data_size);
		}
		EVP_MD_CTX_free(c);
		return ret == 1   ? details::x509_verify_result::ok
			   : ret == 0 ? details::x509_verify_result::bad_signature
						  : details::x509_verify_result::malformed;
	}

public:
	static inline details::x509_verify_result
	cert_sig_verify(details::x509_certificate const &cert,
					details::x509_certificate const &issuer) noexcept
	{
		EVP_PKEY *pk{ossl_tls_spki_pkey(issuer.spki_der, issuer.spki_der_size)};
		if (pk == nullptr)
		{
			return details::x509_verify_result::unsupported_algorithm;
		}
		using details::der_oid_eq;
		details::x509_verify_result res;
		if (der_oid_eq(cert.signature_algorithm_oid, details::oid::ed25519))
		{
			res = ossl_tls_run_oneshot(pk, cert.signature, cert.signature_size, cert.tbs,
									   cert.tbs_size);
		}
		else
		{
			EVP_MD const *md{};
			bool pss{};
			int saltlen{0};
			if (der_oid_eq(cert.signature_algorithm_oid, details::oid::rsassa_pss))
			{
				if (!cert.signature_algorithm_has_params)
				{
					EVP_PKEY_free(pk);
					return details::x509_verify_result::malformed;
				}
				details::rsassa_pss_params pp;
				if (!details::rsassa_pss_params_parse(cert.signature_algorithm_params, pp) ||
					!pp.has_hash || !pp.has_mgf_hash || !pp.has_salt)
				{
					EVP_PKEY_free(pk);
					return details::x509_verify_result::malformed;
				}
				md = ossl_tls_hash_oid(pp.hash_oid);
				saltlen = static_cast<int>(pp.salt_size);
				pss = true;
				/* mgf1 hash must equal the hash oid (checked the fast_io
				   way -- providers do not support mixed mgf) */
				if (pp.mgf_hash_oid.value_size != pp.hash_oid.value_size ||
					__builtin_memcmp(pp.mgf_hash_oid.value, pp.hash_oid.value,
									 pp.hash_oid.value_size) != 0)
				{
					EVP_PKEY_free(pk);
					return details::x509_verify_result::unsupported_algorithm;
				}
			}
			else if (der_oid_eq(cert.signature_algorithm_oid, details::oid::sha256_with_rsa))
			{
				md = EVP_sha256();
			}
			else if (der_oid_eq(cert.signature_algorithm_oid, details::oid::sha384_with_rsa))
			{
				md = EVP_sha384();
			}
			else if (der_oid_eq(cert.signature_algorithm_oid, details::oid::sha512_with_rsa))
			{
				md = EVP_sha512();
			}
			else if (der_oid_eq(cert.signature_algorithm_oid, details::oid::sha224_with_rsa))
			{
				md = EVP_sha224();
			}
			else if (der_oid_eq(cert.signature_algorithm_oid, details::oid::ecdsa_with_sha256))
			{
				md = EVP_sha256();
			}
			else if (der_oid_eq(cert.signature_algorithm_oid, details::oid::ecdsa_with_sha384))
			{
				md = EVP_sha384();
			}
			else if (der_oid_eq(cert.signature_algorithm_oid, details::oid::ecdsa_with_sha512))
			{
				md = EVP_sha512();
			}
			if (md == nullptr)
			{
				EVP_PKEY_free(pk);
				return details::x509_verify_result::unsupported_algorithm;
			}
			res = ossl_tls_run_verify(pk, md, pss, saltlen, cert.signature,
									  cert.signature_size, cert.tbs, cert.tbs_size);
		}
		EVP_PKEY_free(pk);
		return res;
	}

	static inline details::x509_verify_result
	cert_cv_verify(signature_scheme scheme, ::std::byte const *covered,
				   ::std::size_t covered_size, ::std::byte const *signature,
				   ::std::size_t signature_size,
				   details::x509_certificate const &leaf) noexcept
	{
		EVP_PKEY *pk{ossl_tls_spki_pkey(leaf.spki_der, leaf.spki_der_size)};
		if (pk == nullptr)
		{
			return details::x509_verify_result::unsupported_algorithm;
		}
		details::x509_verify_result res;
		if (scheme == signature_scheme::ed25519)
		{
			res = ossl_tls_run_oneshot(pk, signature, signature_size, covered, covered_size);
		}
		else
		{
			EVP_MD const *md{ossl_tls_scheme_md(scheme)};
			if (md == nullptr)
			{
				EVP_PKEY_free(pk);
				return details::x509_verify_result::unsupported_algorithm;
			}
			bool const pss{scheme == signature_scheme::rsa_pss_rsae_sha256 ||
						   scheme == signature_scheme::rsa_pss_rsae_sha384 ||
						   scheme == signature_scheme::rsa_pss_rsae_sha512 ||
						   scheme == signature_scheme::rsa_pss_pss_sha256 ||
						   scheme == signature_scheme::rsa_pss_pss_sha384 ||
						   scheme == signature_scheme::rsa_pss_pss_sha512};
			/* rfc8446 4.2.3: PSS salt length equals the digest length */
			res = ossl_tls_run_verify(pk, md, pss,
									  pss ? static_cast<int>(EVP_MD_size(md))
										  : RSA_PKCS1_PADDING,
									  signature, signature_size, covered, covered_size);
		}
		EVP_PKEY_free(pk);
		return res;
	}

	/*
	server-side CertificateVerify signature. The private key DER is
	handed to the provider's importer: d2i_AutoPrivateKey takes PKCS#8,
	d2i_PrivateKey(EVP_PKEY_RSA) a bare PKCS#1, and a raw 32/64-byte
	ed25519 key feeds EVP_PKEY_new_raw_private_key (the seed is the
	first 32 bytes either way). salt is unused -- the provider RNG
	supplies PSS entropy.
	*/
	static inline bool cert_cv_sign(signature_scheme scheme,
									::std::byte const *covered, ::std::size_t covered_size,
									::std::byte const *pkey_der, ::std::size_t pkey_size,
									::std::byte const *salt,
									::std::byte *sig_out, ::std::size_t *sig_size) noexcept
	{
		(void)salt;
		char unsigned const *d{reinterpret_cast<char unsigned const *>(pkey_der)};
		EVP_PKEY *pk{d2i_AutoPrivateKey(nullptr, __builtin_addressof(d),
										static_cast<long>(pkey_size))};
		if (pk == nullptr)
		{
			d = reinterpret_cast<char unsigned const *>(pkey_der);
			pk = d2i_PrivateKey(EVP_PKEY_RSA, nullptr, __builtin_addressof(d),
								static_cast<long>(pkey_size));
		}
		if (pk == nullptr && (pkey_size == 32 || pkey_size == 64))
		{
			pk = EVP_PKEY_new_raw_private_key(EVP_PKEY_ED25519, nullptr,
											  reinterpret_cast<char unsigned const *>(pkey_der), 32);
		}
		if (pk == nullptr)
		{
			return false;
		}
		bool ok{};
		::std::size_t out{static_cast<::std::size_t>(EVP_PKEY_size(pk))};
		EVP_MD_CTX *c{EVP_MD_CTX_new()};
		if (c != nullptr)
		{
			if (scheme == signature_scheme::ed25519)
			{
				ok = EVP_DigestSignInit(c, nullptr, nullptr, nullptr, pk) == 1 &&
					 EVP_DigestSign(c, reinterpret_cast<char unsigned *>(sig_out),
									__builtin_addressof(out),
									reinterpret_cast<char unsigned const *>(covered),
									covered_size) == 1;
			}
			else
			{
				EVP_MD const *md{ossl_tls_scheme_md(scheme)};
				EVP_PKEY_CTX *pctx{};
				if (md != nullptr &&
					EVP_DigestSignInit(c, __builtin_addressof(pctx), md, nullptr, pk) == 1)
				{
					bool const pss{scheme == signature_scheme::rsa_pss_rsae_sha256 ||
								   scheme == signature_scheme::rsa_pss_rsae_sha384 ||
								   scheme == signature_scheme::rsa_pss_rsae_sha512 ||
								   scheme == signature_scheme::rsa_pss_pss_sha256 ||
								   scheme == signature_scheme::rsa_pss_pss_sha384 ||
								   scheme == signature_scheme::rsa_pss_pss_sha512};
					bool params{true};
					if (pss)
					{
						/* rfc8446 4.2.3: salt length equals the digest */
						params = EVP_PKEY_CTX_set_rsa_padding(pctx, RSA_PKCS1_PSS_PADDING) > 0 &&
								 EVP_PKEY_CTX_set_rsa_pss_saltlen(
									 pctx, static_cast<int>(EVP_MD_size(md))) > 0 &&
								 EVP_PKEY_CTX_set_rsa_mgf1_md(pctx, md) > 0;
					}
					if (params)
					{
						ok = EVP_DigestSign(c, reinterpret_cast<char unsigned *>(sig_out),
											__builtin_addressof(out),
											reinterpret_cast<char unsigned const *>(covered),
											covered_size) == 1;
					}
				}
			}
			EVP_MD_CTX_free(c);
		}
		EVP_PKEY_free(pk);
		if (ok)
		{
			*sig_size = out;
		}
		return ok;
	}
};

} // namespace fast_io::tls

#endif /* FAST_IO_TLS_HAS_OSSL_CRYPTO */
