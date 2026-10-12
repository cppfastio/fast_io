#pragma once

namespace fast_io
{

namespace details
{

inline constexpr ::std::uint_least64_t rapidhash_secret_fio[]{
	0x2d358dccaa6c78a5, 0x8bb84b93962eacc9, 0x4b33a62ed433d4a3,
	0x4d5a2da51de1aa47, 0xa0761d6478bd642f, 0xe7037ed1a0b428db,
	0x90ed1765281c388c, 0xaaaaaaaaaaaaaaaa};

inline constexpr ::std::uint_least64_t
rapidhash_mix_fio(::std::uint_least64_t a, ::std::uint_least64_t b) noexcept
{
	::std::uint_least64_t high;
	auto low{::fast_io::intrinsics::umul(a, b, high)};
	return low ^ high;
}

template <typename T>
inline constexpr T rapidhash_generic_fio(::std::byte const *p) noexcept
{
	constexpr unsigned digits{::std::numeric_limits<char unsigned>::digits};
	T v{};
	for (::std::size_t i{}; i != sizeof(v); ++i)
	{
		v = (v << digits) + static_cast<char unsigned>(p[i]);
	}
	return v;
}

inline constexpr ::std::uint_least64_t
rapidhash_read64_fio(::std::byte const *p) noexcept
{
	FAST_IO_IF_CONSTEVAL
	{
		return ::fast_io::details::rapidhash_generic_fio<::std::uint_least64_t>(p);
	}
	else
	{
		::std::uint_least64_t v;
#if defined(__GNUC__) || defined(__clang__)
		__builtin_memcpy
#else
		::std::memcpy
#endif
			(__builtin_addressof(v), p, sizeof(v));
		return v;
	}
}

inline constexpr ::std::uint_least32_t
rapidhash_read32_fio(::std::byte const *p) noexcept
{
	FAST_IO_IF_CONSTEVAL
	{
		return ::fast_io::details::rapidhash_generic_fio<::std::uint_least32_t>(p);
	}
	else
	{
		::std::uint_least32_t v;
#if defined(__GNUC__) || defined(__clang__)
		__builtin_memcpy
#else
		::std::memcpy
#endif
			(__builtin_addressof(v), p, sizeof(v));
		return v;
	}
}

inline ::std::uint_least64_t
rapidhash_64bits_with_seed_fio(::std::byte const *p, ::std::size_t len, ::std::uint_least64_t seed) noexcept
{

	seed ^= rapidhash_mix_fio(seed ^ rapidhash_secret_fio[2],
							  rapidhash_secret_fio[1]);

	::std::uint_least64_t a{}, b{};
	auto i{len};

	if (len <= 16)
	{
		if (len >= 4)
		{
			seed ^= len;
			if (len >= 8)
			{
				a = rapidhash_read64_fio(p);
				b = rapidhash_read64_fio(p + len - 8);
			}
			else
			{
				a = rapidhash_read32_fio(p);
				b = rapidhash_read32_fio(p + len - 4);
			}
		}
		else if (len > 0)
		{
			a = (static_cast<::std::uint_least64_t>(static_cast<char unsigned>(p[0])) << 45) | static_cast<char unsigned>(p[len - 1]);
			b = static_cast<char unsigned>(p[len >> 1]);
		}
	}
	else
	{
		if (i > 112)
		{
			auto s1{seed}, s2{seed}, s3{seed}, s4{seed}, s5{seed}, s6{seed};
			do
			{
				seed = rapidhash_mix_fio(rapidhash_read64_fio(p) ^ rapidhash_secret_fio[0],
										 rapidhash_read64_fio(p + 8) ^ seed);
				s1 = rapidhash_mix_fio(rapidhash_read64_fio(p + 16) ^ rapidhash_secret_fio[1],
									   rapidhash_read64_fio(p + 24) ^ s1);
				s2 = rapidhash_mix_fio(rapidhash_read64_fio(p + 32) ^ rapidhash_secret_fio[2],
									   rapidhash_read64_fio(p + 40) ^ s2);
				s3 = rapidhash_mix_fio(rapidhash_read64_fio(p + 48) ^ rapidhash_secret_fio[3],
									   rapidhash_read64_fio(p + 56) ^ s3);
				s4 = rapidhash_mix_fio(rapidhash_read64_fio(p + 64) ^ rapidhash_secret_fio[4],
									   rapidhash_read64_fio(p + 72) ^ s4);
				s5 = rapidhash_mix_fio(rapidhash_read64_fio(p + 80) ^ rapidhash_secret_fio[5],
									   rapidhash_read64_fio(p + 88) ^ s5);
				s6 = rapidhash_mix_fio(rapidhash_read64_fio(p + 96) ^ rapidhash_secret_fio[6],
									   rapidhash_read64_fio(p + 104) ^ s6);
				p += 112;
				i -= 112;
			} while (i > 112);
			seed ^= s1;
			s2 ^= s3;
			s4 ^= s5;
			seed ^= s6;
			s2 ^= s4;
			seed ^= s2;
		}

		if (i > 16)
		{
			seed = rapidhash_mix_fio(rapidhash_read64_fio(p) ^ rapidhash_secret_fio[2],
									 rapidhash_read64_fio(p + 8) ^ seed);
			if (i > 32)
			{
				seed = rapidhash_mix_fio(rapidhash_read64_fio(p + 16) ^ rapidhash_secret_fio[2],
										 rapidhash_read64_fio(p + 24) ^ seed);
				if (i > 48)
				{
					seed = rapidhash_mix_fio(rapidhash_read64_fio(p + 32) ^ rapidhash_secret_fio[1],
											 rapidhash_read64_fio(p + 40) ^ seed);
					if (i > 64)
					{
						seed = rapidhash_mix_fio(rapidhash_read64_fio(p + 48) ^ rapidhash_secret_fio[1],
												 rapidhash_read64_fio(p + 56) ^ seed);
						if (i > 80)
						{
							seed = rapidhash_mix_fio(rapidhash_read64_fio(p + 64) ^ rapidhash_secret_fio[2],
													 rapidhash_read64_fio(p + 72) ^ seed);
							if (i > 96)
							{
								seed = rapidhash_mix_fio(rapidhash_read64_fio(p + 80) ^ rapidhash_secret_fio[1],
														 rapidhash_read64_fio(p + 88) ^ seed);
							}
						}
					}
				}
			}
		}

		a = rapidhash_read64_fio(p + i - 16) ^ i;
		b = rapidhash_read64_fio(p + i - 8);
	}

	a ^= rapidhash_secret_fio[1];
	b ^= seed;
	::std::uint_least64_t high;
	a = ::fast_io::intrinsics::umul(a, b, high);
	return rapidhash_mix_fio(a ^ rapidhash_secret_fio[7],
							 high ^ rapidhash_secret_fio[1] ^ i);
}

inline ::std::uint_least64_t
rapidhash_64bits_fio(void const *data, ::std::size_t len) noexcept
{
	return ::fast_io::details::rapidhash_64bits_with_seed_fio(reinterpret_cast<::std::byte const *>(data), len, 0u);
}

} // namespace details

struct rapidhash64
{
	using digest_type = ::std::uint_least64_t;
	using seed_type = ::std::uint_least64_t;
	seed_type seed{};
	inline constexpr digest_type do_hash(::std::byte const *blocks_start, ::std::byte const *blocks_last) const noexcept
	{
		return ::fast_io::details::rapidhash_64bits_with_seed_fio(blocks_start, static_cast<::std::size_t>(blocks_last - blocks_start), seed);
	}
};

using native_swiss_hasher = ::fast_io::rapidhash64;
} // namespace fast_io
