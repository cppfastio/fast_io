#pragma once

/*
AES-GCM (NIST SP 800-38D) for TLS 1.3 records. 96-bit nonce only
(TLS never uses other sizes, so J0 is always the nonce itself --
GHASH'd J0 for other lengths is not implemented).

	gcm context = aes_ctx + precomputed H = E_K(0)
	seal/open take: key, nonce12, aad, and buffers; tag is 16 bytes
*/

#include "../cipher/aes.h"
#include "gf128.h"

namespace fast_io::details::aead
{

/*
GHASH over aad || ct || len(aad)||len(ct) in bits, XORed with E_K(J0)
into tag. Y starts zeroed; each 16-byte chunk is xored then multiplied
by H.
*/
template <typename aes_ctx>
inline constexpr void gcm_tag_to_ptr(::std::byte tag[16], aes_ctx const &ctx,
									 ::std::byte const (&H)[16],
									 ::std::byte const (&j0)[16],
									 ::std::byte const *aad, ::std::size_t aad_size,
									 ::std::byte const *ct, ::std::size_t ct_size) noexcept
{
	::std::byte y[16]{};
	auto fold{[&](::std::byte const *p, ::std::size_t n) noexcept {
		for (; n >= 16; p += 16, n -= 16)
		{
			for (::std::size_t i{}; i != 16; ++i)
			{
				y[i] ^= p[i];
			}
			gf128_mul(y, y, H);
		}
		if (n != 0)
		{
			for (::std::size_t i{}; i != n; ++i)
			{
				y[i] ^= p[i];
			}
			gf128_mul(y, y, H);
		}
	}};
	fold(aad, aad_size);
	fold(ct, ct_size);
	/* lengths are bit counts, big-endian u64 each */
	::std::byte lens[16]{};
	for (::std::size_t i{}; i != 8; ++i)
	{
		lens[i] = static_cast<::std::byte>((aad_size * 8u) >> (56u - 8u * i));
		lens[8 + i] = static_cast<::std::byte>((ct_size * 8u) >> (56u - 8u * i));
	}
	fold(lens, 16);
	ctx.encrypt(j0, 1, tag);
	for (::std::size_t i{}; i != 16; ++i)
	{
		tag[i] ^= y[i];
	}
}

/* GCTR with 32-bit big-endian counter starting at j0+1. */
template <typename aes_ctx>
inline constexpr void gctr(aes_ctx const &ctx, ::std::byte const (&j0)[16],
						   ::std::byte const *in, ::std::size_t n, ::std::byte *out) noexcept
{
	::std::byte ctr[16];
	for (::std::size_t i{}; i != 16; ++i)
	{
		ctr[i] = j0[i];
	}
	::std::byte stream[16];
	while (n != 0)
	{
		/* ctr++ big-endian 32-bit */
		for (::std::size_t i{16}; i-- != 12;)
		{
			ctr[i] = static_cast<::std::byte>(static_cast<unsigned>(ctr[i]) + 1u);
			if (ctr[i] != ::std::byte{0u})
			{
				break;
			}
		}
		ctx.encrypt(ctr, 1, stream);
		::std::size_t const chunk{n < 16 ? n : 16};
		for (::std::size_t i{}; i != chunk; ++i)
		{
			out[i] = in[i] ^ stream[i];
		}
		in += chunk;
		out += chunk;
		n -= chunk;
	}
}

} // namespace fast_io::details::aead

namespace fast_io
{

/* one-shot AES-GCM seal: ct||tag out */
template <::std::size_t key_size>
inline constexpr void aes_gcm_seal_to_ptr(::std::byte *out, ::std::byte *tag16,
										  ::std::byte const (&key)[key_size],
										  ::std::byte const (&nonce)[12],
										  ::std::byte const *aad, ::std::size_t aad_size,
										  ::std::byte const *pt, ::std::size_t pt_size) noexcept
{
	::fast_io::aes_ctx<key_size> ctx{key};
	::std::byte H[16]{};
	ctx.encrypt(H, 1, H);
	::std::byte j0[16]{};
	for (::std::size_t i{}; i != 12; ++i)
	{
		j0[i] = nonce[i];
	}
	j0[15] = ::std::byte{1u};
	::fast_io::details::aead::gctr(ctx, j0, pt, pt_size, out);
	::fast_io::details::aead::gcm_tag_to_ptr(tag16, ctx, H, j0, aad, aad_size, out, pt_size);
}

/*
one-shot AES-GCM open: decrypts ct into out, verifies tag.
Returns false on tag mismatch (caller must treat as bad_record_mac).
The tag check is a plain byte loop: records are public-length and the
whole tag is compared, so early-exit leaks nothing.
*/
template <::std::size_t key_size>
inline constexpr bool aes_gcm_open_to_ptr(::std::byte *out,
										  ::std::byte const (&key)[key_size],
										  ::std::byte const (&nonce)[12],
										  ::std::byte const *aad, ::std::size_t aad_size,
										  ::std::byte const *ct, ::std::size_t ct_size,
										  ::std::byte const (&tag)[16]) noexcept
{
	::fast_io::aes_ctx<key_size> ctx{key};
	::std::byte H[16]{};
	ctx.encrypt(H, 1, H);
	::std::byte j0[16]{};
	for (::std::size_t i{}; i != 12; ++i)
	{
		j0[i] = nonce[i];
	}
	j0[15] = ::std::byte{1u};
	::std::byte expect[16];
	::fast_io::details::aead::gcm_tag_to_ptr(expect, ctx, H, j0, aad, aad_size, ct, ct_size);
	bool same{true};
	for (::std::size_t i{}; i != 16; ++i)
	{
		same &= (expect[i] == tag[i]);
	}
	if (!same)
	{
		return false;
	}
	::fast_io::details::aead::gctr(ctx, j0, ct, ct_size, out);
	return true;
}

} // namespace fast_io
