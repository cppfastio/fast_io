#pragma once

/*
GF(2^128) multiplication for GHASH (NIST SP 800-38D).
Polynomial x^128 + x^7 + x^2 + x + 1; operands are the 16-byte
big-endian bit strings the GCM spec uses.
*/

namespace fast_io::details::aead
{

/*
x * y in GF(2^128), written to r (16 bytes). Straight bit loop:
GCM blocks arrive a few hundred bytes at a time here (handshake
records), so a table-free reduction is plenty; if bulk throughput
ever matters a 4-bit window table or PMULL backend belongs here.

PERFORMANCE: bit-serial multiply -- ~16 rounds of shifting per input
block is fine for handshake traffic but too slow for bulk appdata
if this path is ever used for it.
*/
inline constexpr void gf128_mul(::std::byte r[16], ::std::byte const x[16], ::std::byte const y[16]) noexcept
{
	::std::byte z[16]{};
	::std::byte v[16];
	for (::std::size_t i{}; i != 16; ++i)
	{
		v[i] = y[i];
	}
	for (::std::size_t i{}; i != 128; ++i)
	{
		/* MSB-first bit scan of x */
		if (((static_cast<unsigned>(x[i >> 3u]) >> (7u - (i & 7u))) & 1u) != 0u)
		{
			for (::std::size_t j{}; j != 16; ++j)
			{
				z[j] ^= v[j];
			}
		}
		/* v = v * x (shift the whole big-endian number right 1;
		   v[0] is most significant, v[15]&1 is the outgoing bit) */
		bool const lsb{(v[15] & ::std::byte{1u}) != ::std::byte{0u}};
		bool carry{};
		for (::std::size_t j{}; j != 16; ++j)
		{
			bool const next{(v[j] & ::std::byte{1u}) != ::std::byte{0u}};
			v[j] = static_cast<::std::byte>((static_cast<unsigned>(v[j]) >> 1u) |
											(carry ? 0x80u : 0u));
			carry = next;
		}
		if (lsb)
		{
			v[0] ^= ::std::byte{0xe1u};
		}
	}
	for (::std::size_t i{}; i != 16; ++i)
	{
		r[i] = z[i];
	}
}

} // namespace fast_io::details::aead
