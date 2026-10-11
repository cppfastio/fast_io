#pragma once

/*
TLS 1.3 record-layer userspace AEAD (used for the handshake flight;
kTLS takes over at the application epoch).

per-record nonce = static_iv(12) XOR (00000000 || seq u64 be)
AAD             = the 5-byte record header as transmitted
inner plaintext = content || content_type || zeros
*/

#include "../aead/gf128.h"
#include "../aead/aes_gcm.h"
#include "../aead/poly1305.h"
#include "../aead/chacha20_poly1305.h"

namespace fast_io::tls::details
{

inline constexpr void tls_nonce_to_ptr(::std::byte *out /*12*/,
									   ::std::byte const *iv, ::std::uint_least64_t seq) noexcept
{
	for (::std::size_t i{}; i != 12; ++i)
	{
		out[i] = iv[i];
	}
	for (::std::size_t i{}; i != 8; ++i)
	{
		out[4 + i] ^= static_cast<::std::byte>(seq >> (56u - 8u * i));
	}
}

/*
open one ciphertext record. hdr is the record's 5-byte header, ct its
len-field bytes (ciphertext+tag). Writes the inner plaintext to out and
returns {inner_type, content_size}, or false on tag failure.
*/
inline bool tls_record_open(::std::byte *out, ::std::size_t &out_size, content_type &inner_type,
							::std::byte const *hdr5, ::std::byte const *ct, ::std::size_t ct_size,
							cipher_suite suite, ::std::byte const *key, ::std::byte const *iv,
							::std::uint_least64_t seq) noexcept
{
	if (ct_size < 17) /* tag(16) + at least the inner type byte */
	{
		return false;
	}
	::std::byte nonce[12];
	tls_nonce_to_ptr(nonce, iv, seq);
	::std::size_t const inner_size{ct_size - 16};
	::std::byte tag[16];
	for (::std::size_t i{}; i != 16; ++i)
	{
		tag[i] = ct[inner_size + i];
	}
	bool ok{};
	switch (suite)
	{
	case cipher_suite::aes_128_gcm_sha256:
	{
		::std::byte k[16];
		for (::std::size_t i{}; i != 16; ++i)
		{
			k[i] = key[i];
		}
		ok = ::fast_io::aes_gcm_open_to_ptr<16>(out, k, nonce, hdr5, 5, ct, inner_size, tag);
		break;
	}
	case cipher_suite::aes_256_gcm_sha384:
	{
		::std::byte k[32];
		for (::std::size_t i{}; i != 32; ++i)
		{
			k[i] = key[i];
		}
		ok = ::fast_io::aes_gcm_open_to_ptr<32>(out, k, nonce, hdr5, 5, ct, inner_size, tag);
		break;
	}
	default:
	{
		::std::byte k[32];
		for (::std::size_t i{}; i != 32; ++i)
		{
			k[i] = key[i];
		}
		ok = ::fast_io::chacha20_poly1305_open_to_ptr(out, k, nonce, hdr5, 5, ct, inner_size, tag);
		break;
	}
	}
	if (!ok)
	{
		return false;
	}
	/* strip padding: last nonzero byte is the inner content type */
	::std::size_t i{inner_size};
	while (i != 0 && out[i - 1] == ::std::byte{})
	{
		--i;
	}
	if (i == 0)
	{
		return false;
	}
	inner_type = static_cast<content_type>(out[i - 1]);
	out_size = i - 1;
	return true;
}

/*
seal a precomposed inner plaintext (content || content_type already
appended by the caller) into a ciphertext record. Writes hdr(5)+ct into
out; returns total bytes written.
*/
inline ::std::size_t tls_record_seal_inner(::std::byte *out,
										   ::std::byte const *inner, ::std::size_t inner_size,
										   cipher_suite suite,
										   ::std::byte const *key, ::std::byte const *iv,
										   ::std::uint_least64_t seq) noexcept
{
	::std::byte nonce[12];
	tls_nonce_to_ptr(nonce, iv, seq);
	::std::byte *p{record_header_write(out, content_type::application_data,
									   static_cast<::std::uint_least16_t>(inner_size + 16))};
	::std::byte *const hdr{out};
	::std::byte tag[16];
	switch (suite)
	{
	case cipher_suite::aes_128_gcm_sha256:
	{
		::std::byte k[16];
		for (::std::size_t i{}; i != 16; ++i)
		{
			k[i] = key[i];
		}
		::fast_io::aes_gcm_seal_to_ptr<16>(p, tag, k, nonce, hdr, 5, inner, inner_size);
		break;
	}
	case cipher_suite::aes_256_gcm_sha384:
	{
		::std::byte k[32];
		for (::std::size_t i{}; i != 32; ++i)
		{
			k[i] = key[i];
		}
		::fast_io::aes_gcm_seal_to_ptr<32>(p, tag, k, nonce, hdr, 5, inner, inner_size);
		break;
	}
	default:
	{
		::std::byte k[32];
		for (::std::size_t i{}; i != 32; ++i)
		{
			k[i] = key[i];
		}
		::fast_io::chacha20_poly1305_seal_to_ptr(p, tag, k, nonce, hdr, 5, inner, inner_size);
		break;
	}
	}
	p += inner_size;
	for (::std::size_t i{}; i != 16; ++i)
	{
		p[i] = tag[i];
	}
	return static_cast<::std::size_t>((p + 16) - out);
}

/*
seal inner plaintext into a ciphertext record. Writes hdr(5)+ct into
out; returns total bytes written.
*/
inline ::std::size_t tls_record_seal(::std::byte *out,
									 content_type inner_type,
									 ::std::byte const *pt, ::std::size_t pt_size,
									 cipher_suite suite,
									 ::std::byte const *key, ::std::byte const *iv,
									 ::std::uint_least64_t seq) noexcept
{
	/* inner = pt || type || (no padding -- records are small).
	   PERFORMANCE: 16K stack buffer per call -- fine for handshake
	   records; a real write path should take a caller buffer instead */
	if (pt_size >= 16641)
	{
		::fast_io::fast_terminate();
	}
	::std::byte inner[16641];
	::fast_io::freestanding::non_overlapped_copy_n(pt, pt_size, inner);
	inner[pt_size] = static_cast<::std::byte>(inner_type);
	return tls_record_seal_inner(out, inner, pt_size + 1, suite, key, iv, seq);
}

} // namespace fast_io::tls::details
