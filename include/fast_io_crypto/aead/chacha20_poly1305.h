#pragma once

/*
ChaCha20-Poly1305 AEAD (RFC 8439) for TLS 1.3 records.
96-bit nonce: state = sigma || key(8xle32) || counter(le32) || nonce(3xle32).
Counter 0 produces the 64-byte block whose first 32 bytes are the
poly1305 one-time key; counters 1.. encrypt.
*/

#include "poly1305.h"
#include "../streamcipher/chacha/scalar.h"

namespace fast_io::details::aead
{

inline constexpr ::std::uint_least32_t chacha_load_le(::std::byte const *p) noexcept
{
	return static_cast<::std::uint_least32_t>(p[0]) |
		   (static_cast<::std::uint_least32_t>(p[1]) << 8u) |
		   (static_cast<::std::uint_least32_t>(p[2]) << 16u) |
		   (static_cast<::std::uint_least32_t>(p[3]) << 24u);
}

/* one 64-byte keystream block */
inline constexpr void chacha20_block(::std::byte out[64], ::std::byte const (&key)[32],
									 ::std::byte const (&nonce)[12], ::std::uint_least32_t counter) noexcept
{
	::std::uint_least32_t st[16]{0x61707865u, 0x3320646eu, 0x79622d32u, 0x6b206574u};
	for (::std::size_t i{}; i != 8; ++i)
	{
		st[4 + i] = chacha_load_le(key + i * 4);
	}
	st[12] = counter;
	for (::std::size_t i{}; i != 3; ++i)
	{
		st[13 + i] = chacha_load_le(nonce + i * 4);
	}
	::fast_io::details::chacha::chacha_main_routine(out, st);
}

inline constexpr void chacha20_xor(::std::byte *out, ::std::byte const *in, ::std::size_t n,
								   ::std::byte const (&key)[32], ::std::byte const (&nonce)[12],
								   ::std::uint_least32_t counter) noexcept
{
	::std::byte stream[64];
	while (n != 0)
	{
		chacha20_block(stream, key, nonce, counter++);
		::std::size_t const chunk{n < 64 ? n : 64};
		for (::std::size_t i{}; i != chunk; ++i)
		{
			out[i] = in[i] ^ stream[i];
		}
		in += chunk;
		out += chunk;
		n -= chunk;
	}
}

/* mac_data = aad || pad16 || ct || pad16 || aad_len || ct_len (le64 each) */
inline constexpr void chacha_poly_mac_to_ptr(::std::byte tag[16],
											 ::std::byte const (&otk)[32],
											 ::std::byte const *aad, ::std::size_t aad_size,
											 ::std::byte const *ct, ::std::size_t ct_size) noexcept
{
	poly1305_state st{};
	st.init(otk);
	st.update(aad, aad_size);
	if ((aad_size & 15u) != 0u)
	{
		::std::byte zeros[15]{};
		st.update(zeros, 16u - (aad_size & 15u));
	}
	st.update(ct, ct_size);
	if ((ct_size & 15u) != 0u)
	{
		::std::byte zeros[15]{};
		st.update(zeros, 16u - (ct_size & 15u));
	}
	::std::byte lens[16];
	for (::std::size_t i{}; i != 8; ++i)
	{
		lens[i] = static_cast<::std::byte>(aad_size >> (8u * i));
		lens[8 + i] = static_cast<::std::byte>(ct_size >> (8u * i));
	}
	st.update(lens, 16);
	st.digest_to_byte_ptr(tag);
}

} // namespace fast_io::details::aead

namespace fast_io
{

inline constexpr void chacha20_poly1305_seal_to_ptr(::std::byte *out, ::std::byte *tag16,
													::std::byte const (&key)[32],
													::std::byte const (&nonce)[12],
													::std::byte const *aad, ::std::size_t aad_size,
													::std::byte const *pt, ::std::size_t pt_size) noexcept
{
	::std::byte block0[64];
	::fast_io::details::aead::chacha20_block(block0, key, nonce, 0);
	::fast_io::details::aead::chacha20_xor(out, pt, pt_size, key, nonce, 1);
	::std::byte otk[32];
	for (::std::size_t i{}; i != 32; ++i)
	{
		otk[i] = block0[i];
	}
	::fast_io::details::aead::chacha_poly_mac_to_ptr(tag16, otk, aad, aad_size, out, pt_size);
}

inline constexpr bool chacha20_poly1305_open_to_ptr(::std::byte *out,
													::std::byte const (&key)[32],
													::std::byte const (&nonce)[12],
													::std::byte const *aad, ::std::size_t aad_size,
													::std::byte const *ct, ::std::size_t ct_size,
													::std::byte const (&tag)[16]) noexcept
{
	::std::byte block0[64];
	::fast_io::details::aead::chacha20_block(block0, key, nonce, 0);
	::std::byte otk[32];
	for (::std::size_t i{}; i != 32; ++i)
	{
		otk[i] = block0[i];
	}
	::std::byte expect[16];
	::fast_io::details::aead::chacha_poly_mac_to_ptr(expect, otk, aad, aad_size, ct, ct_size);
	bool same{true};
	for (::std::size_t i{}; i != 16; ++i)
	{
		same &= (expect[i] == tag[i]);
	}
	if (!same)
	{
		return false;
	}
	::fast_io::details::aead::chacha20_xor(out, ct, ct_size, key, nonce, 1);
	return true;
}

} // namespace fast_io
