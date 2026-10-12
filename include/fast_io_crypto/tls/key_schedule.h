#pragma once

/*
TLS 1.3 key schedule (rfc8446 7.1) on top of hmac_context.

HKDF-Extract(salt, IKM)         -> PRK
HKDF-Expand(PRK, info, L)       -> OKM
HKDF-Expand-Label(S, label, ctx, L)
  = HKDF-Expand(S, u16(L) || u8(6+len("tls13 "+label)) || "tls13 "+label
					 || u8(len(ctx)) || ctx, L)
Derive-Secret(S, label, msgs)
  = HKDF-Expand-Label(S, label, Transcript-Hash(msgs), Hash.length)

secret chain:

  0 -> extract = early_secret
	   early_secret -> derive("derived", "") -> extract(dhe) = hs_secret
	   hs_secret    -> derive("derived", "") -> extract(0)   = master
*/

namespace fast_io::tls::details
{

/* out[digest_size] = HKDF-Extract(salt, ikm). salt == nullptr -> zeros. */
template <typename ctx>
inline constexpr void hkdf_extract_to_ptr(::std::byte *out,
										  ::std::byte const *salt, ::std::size_t salt_size,
										  ::std::byte const *ikm, ::std::size_t ikm_size) noexcept
{
	constexpr ::std::size_t digest_size{ctx::digest_size};
	::std::byte zero_salt[digest_size]{};
	if (salt == nullptr)
	{
		salt = zero_salt;
		salt_size = digest_size;
	}
	::fast_io::hmac_once_to_ptr<ctx>(out, salt, salt_size, ikm, ikm_size);
}

/*
out[out_size] = HKDF-Expand(prk, info). prk is digest_size bytes.
T(0) empty; T(i) = HMAC(prk, T(i-1) || info || i).
*/
template <typename ctx>
inline constexpr void hkdf_expand_to_ptr(::std::byte *out, ::std::size_t out_size,
										 ::std::byte const *prk,
										 ::std::byte const *info, ::std::size_t info_size) noexcept
{
	constexpr ::std::size_t digest_size{ctx::digest_size};
	::std::byte t[digest_size];
	::std::size_t t_size{};
	::std::size_t written{};
	for (::std::uint_least8_t i{1}; written != out_size; ++i)
	{
		::fast_io::hmac_context<ctx> h{prk, digest_size};
		h.update(t, t + t_size);
		h.update(info, info + info_size);
		::std::byte const ctr{static_cast<::std::byte>(i)};
		h.update(&ctr, &ctr + 1);
		h.do_final();
		h.digest_to_byte_ptr(t);
		t_size = digest_size;
		::std::size_t const chunk{out_size - written < digest_size ? out_size - written : digest_size};
		::fast_io::details::non_overlapped_copy_n(t, chunk, out + written);
		written += chunk;
	}
}

/*
out[out_size] = HKDF-Expand-Label(secret, label, context).
label is a NUL-free string without the "tls13 " prefix.
secret/context sizes follow the spec limits (label <= 255-7, ctx <= 255).
*/
template <typename ctx>
inline constexpr void hkdf_expand_label_to_ptr(::std::byte *out, ::std::size_t out_size,
											   ::std::byte const *secret,
											   char8_t const *label, ::std::size_t label_size,
											   ::std::byte const *context, ::std::size_t context_size) noexcept
{
	::std::byte hkdf_label[2 + 1 + 6 + 24 + 1 + 255];
	::std::byte *p{hkdf_label};
	p = ::fast_io::tls::wire_put_u16(p, static_cast<::std::uint_least16_t>(out_size));
	*p++ = static_cast<::std::byte>(6u + label_size);
	::fast_io::details::non_overlapped_copy_n(u8"tls13 ", 6, reinterpret_cast<char8_t *>(p));
	p += 6;
	::fast_io::details::non_overlapped_copy_n(label, label_size, reinterpret_cast<char8_t *>(p));
	p += label_size;
	*p++ = static_cast<::std::byte>(context_size);
	p = ::fast_io::tls::wire_put_bytes(p, context, context_size);
	hkdf_expand_to_ptr<ctx>(out, out_size, secret, hkdf_label, static_cast<::std::size_t>(p - hkdf_label));
}

/* digest a running transcript without disturbing it */
template <typename ctx>
inline constexpr void transcript_digest_to_ptr(ctx const &transcript, ::std::byte *digest) noexcept
{
	ctx copy{transcript};
	copy.do_final();
	copy.digest_to_byte_ptr(digest);
}

/* out[digest_size] = Derive-Secret(secret, label, transcript) */
template <typename ctx>
inline constexpr void derive_secret_to_ptr(::std::byte *out,
										   ::std::byte const *secret,
										   char8_t const *label, ::std::size_t label_size,
										   ctx const &transcript) noexcept
{
	constexpr ::std::size_t digest_size{ctx::digest_size};
	::std::byte th[digest_size];
	transcript_digest_to_ptr(transcript, th);
	hkdf_expand_label_to_ptr<ctx>(out, digest_size, secret, label, label_size, th, digest_size);
}

/*
traffic key/iv for one direction:
  key = HKDF-Expand-Label(secret, "key", "", key_size)
  iv  = HKDF-Expand-Label(secret, "iv",  "", 12)
*/
template <typename ctx>
inline constexpr void traffic_key_iv_to_ptr(::std::byte *key, ::std::size_t key_size,
											::std::byte *iv /* 12 bytes */,
											::std::byte const *secret) noexcept
{
	hkdf_expand_label_to_ptr<ctx>(key, key_size, secret, u8"key", 3, nullptr, 0);
	hkdf_expand_label_to_ptr<ctx>(iv, 12, secret, u8"iv", 2, nullptr, 0);
}

/*
finished_key = HKDF-Expand-Label(base_secret, "finished", "", Hash.length)
verify_data  = HMAC(finished_key, Transcript-Hash)
*/
template <typename ctx>
inline constexpr void finished_verify_data_to_ptr(::std::byte *verify_data /* digest_size */,
												  ::std::byte const *base_secret,
												  ctx const &transcript) noexcept
{
	constexpr ::std::size_t digest_size{ctx::digest_size};
	::std::byte fk[digest_size];
	hkdf_expand_label_to_ptr<ctx>(fk, digest_size, base_secret, u8"finished", 8, nullptr, 0);
	::std::byte th[digest_size];
	transcript_digest_to_ptr(transcript, th);
	::fast_io::hmac_once_to_ptr<ctx>(verify_data, fk, digest_size, th, digest_size);
}

/*
the running "secret chain" state: early -> handshake -> master.
digest_size is 32 for SHA-256 suites, 48 for SHA-384 suites.
*/


/* ---------------- crypto-backend level ----------------
   the same key schedule expressed against a backend's md + hash ctx:
   every op takes the md descriptor at runtime, so a single
   instantiation covers sha256 and sha384 suites. */

template <typename crypto>
inline constexpr void hkdf_expand_label_to_ptr(typename crypto::md md,
											   ::std::byte *out, ::std::size_t out_size,
											   ::std::byte const *secret,
											   char8_t const *label, ::std::size_t label_size,
											   ::std::byte const *context, ::std::size_t context_size) noexcept
{
	::std::byte hkdf_label[2 + 1 + 6 + 24 + 1 + 255];
	::std::byte *p{hkdf_label};
	p = ::fast_io::tls::wire_put_u16(p, static_cast<::std::uint_least16_t>(out_size));
	*p++ = static_cast<::std::byte>(6u + label_size);
	::fast_io::details::non_overlapped_copy_n(u8"tls13 ", 6, reinterpret_cast<char8_t *>(p));
	p += 6;
	::fast_io::details::non_overlapped_copy_n(label, label_size, reinterpret_cast<char8_t *>(p));
	p += label_size;
	*p++ = static_cast<::std::byte>(context_size);
	p = ::fast_io::tls::wire_put_bytes(p, context, context_size);
	crypto::hkdf_expand(md, out, out_size, secret, crypto::md_digest_size(md),
						hkdf_label, static_cast<::std::size_t>(p - hkdf_label));
}

template <typename crypto>
inline constexpr void transcript_digest_to_ptr(typename crypto::hash_ctx const &transcript,
											   ::std::byte *digest) noexcept
{
	typename crypto::hash_ctx copy{transcript};
	copy.do_final();
	copy.digest_to_byte_ptr(digest);
}

template <typename crypto>
inline constexpr void derive_secret_to_ptr(typename crypto::md md, ::std::byte *out,
										   ::std::byte const *secret,
										   char8_t const *label, ::std::size_t label_size,
										   typename crypto::hash_ctx const &transcript) noexcept
{
	::std::byte th[64];
	transcript_digest_to_ptr<crypto>(transcript, th);
	hkdf_expand_label_to_ptr<crypto>(md, out, crypto::md_digest_size(md), secret, label,
									 label_size, th, crypto::md_digest_size(md));
}

template <typename crypto>
inline constexpr void traffic_key_iv_to_ptr(typename crypto::md md, ::std::byte *key,
											::std::size_t key_size, ::std::byte *iv,
											::std::byte const *secret) noexcept
{
	hkdf_expand_label_to_ptr<crypto>(md, key, key_size, secret, u8"key", 3, nullptr, 0);
	hkdf_expand_label_to_ptr<crypto>(md, iv, 12, secret, u8"iv", 2, nullptr, 0);
}

template <typename crypto>
inline constexpr void finished_verify_data_to_ptr(typename crypto::md md,
												  ::std::byte *verify_data,
												  ::std::byte const *base_secret,
												  typename crypto::hash_ctx const &transcript) noexcept
{
	::std::size_t const ds{crypto::md_digest_size(md)};
	::std::byte fk[64];
	hkdf_expand_label_to_ptr<crypto>(md, fk, ds, base_secret, u8"finished", 8, nullptr, 0);
	::std::byte th[64];
	transcript_digest_to_ptr<crypto>(transcript, th);
	crypto::hmac(md, verify_data, fk, ds, th, ds);
}

template <typename crypto>
class key_schedule
{
public:
	crypto::md md_;
	::std::byte secret[64]{};

	constexpr key_schedule(typename crypto::md md) noexcept : md_{md}
	{
	}

	inline constexpr void init_early() noexcept
	{
		::std::byte zero[64]{};
		crypto::hkdf_extract(md_, secret, nullptr, 0, zero, crypto::md_digest_size(md_));
	}

	/* PSK handshakes seed the early secret with the psk itself instead
	   of zeros (rfc8446 7.1, resumption) */
	inline constexpr void init_early_with(::std::byte const *ikm, ::std::size_t ikm_size) noexcept
	{
		crypto::hkdf_extract(md_, secret, nullptr, 0, ikm, ikm_size);
	}

	inline constexpr void derive_empty() noexcept
	{
		::std::byte empty_digest[64];
		typename crypto::hash_ctx empty_ctx{md_};
		empty_ctx.do_final();
		empty_ctx.digest_to_byte_ptr(empty_digest);
		::std::byte next[64];
		hkdf_expand_label_to_ptr<crypto>(md_, next, crypto::md_digest_size(md_), secret,
										 u8"derived", 7, empty_digest,
										 crypto::md_digest_size(md_));
		::fast_io::details::non_overlapped_copy_n(next, crypto::md_digest_size(md_), secret);
	}

	inline constexpr void extract_into(::std::byte const *ikm, ::std::size_t ikm_size) noexcept
	{
		::std::byte next[64];
		crypto::hkdf_extract(md_, next, secret, crypto::md_digest_size(md_), ikm, ikm_size);
		::fast_io::details::non_overlapped_copy_n(next, crypto::md_digest_size(md_), secret);
	}

	inline constexpr void derive_to_ptr(::std::byte *out, char8_t const *label,
										::std::size_t label_size,
										typename crypto::hash_ctx const &transcript) const noexcept
	{
		derive_secret_to_ptr<crypto>(md_, out, secret, label, label_size, transcript);
	}
};

} // namespace fast_io::tls::details
