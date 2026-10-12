#pragma once
/*
fast_io AES (FIPS-197).

Public API:
	::fast_io::aes_ctx<key_size>  key_size is 16, 24 or 32 bytes
	aliases: aes128 / aes192 / aes256

	ctx.encrypt(from, nblocks, to) / ctx.decrypt(from, nblocks, to)
	operate on nblocks consecutive 16-byte blocks (ECB); from and to
	may alias. The block loop lives inside the backend functions so
	the optimizer sees the whole buffer.

One context serves both directions: decryption reuses the encryption
round keys in reverse order (straightforward inverse cipher), so the
object stores a single byte-oriented key schedule.

Backend layout:
	details::aes::* in aes_scalar.h is the portable constexpr software
	implementation. A hardware backend (AES-NI, ARMv8 CE) or a portable
	SIMD backend (built on ::fast_io::intrinsics::simd_vector /
	__builtin_shufflevector, never ISA-specific intrinsics) may later be
	selected here behind the same API.
*/

#include "aes_scalar.h"

namespace fast_io
{

template <::std::size_t key_size_bytes>
	requires(key_size_bytes == 16 || key_size_bytes == 24 || key_size_bytes == 32)
class aes_ctx
{
public:
	inline static constexpr ::std::size_t block_size{16};
	inline static constexpr ::std::size_t key_size{key_size_bytes};
	inline static constexpr ::std::size_t nk{key_size_bytes / 4u};
	inline static constexpr ::std::size_t rounds{nk + 6u};
	inline static constexpr ::std::size_t key_schedule_size{block_size * (rounds + 1u)};

private:
	::std::uint_least8_t round_keys[key_schedule_size]{};

public:
	/* key points to key_size bytes */
	inline explicit constexpr aes_ctx(::std::byte const *key) noexcept
	{
		::fast_io::details::aes::key_expansion<nk>(key, round_keys);
	}

	/* ECB over nblocks consecutive 16-byte blocks; from/to may alias */
	inline constexpr void encrypt(::std::byte const *from, ::std::size_t nblocks,
								  ::std::byte *to) const noexcept
	{
		::fast_io::details::aes::encrypt<nk>(round_keys, from, nblocks, to);
	}

	inline constexpr void decrypt(::std::byte const *from, ::std::size_t nblocks,
								  ::std::byte *to) const noexcept
	{
		::fast_io::details::aes::decrypt<nk>(round_keys, from, nblocks, to);
	}
};

using aes128 = aes_ctx<16>;
using aes192 = aes_ctx<24>;
using aes256 = aes_ctx<32>;

} // namespace fast_io
