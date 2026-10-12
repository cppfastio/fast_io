#pragma once

namespace fast_io
{

namespace details
{

inline constexpr ::std::uint_least64_t xxh3_secrect_fio[]{
	0x9E3779B185EBCA87ULL, 0xC2B2AE3D27D4EB4FULL,
	0x165667B19E3779F9ULL, 0x85EBCA77C2B2AE63ULL,
	0x27D4EB2F165667C5ULL, 0x9E3779B185EBCA87ULL,
	0xC2B2AE3D27D4EB4FULL, 0x165667B19E3779F9ULL};

inline constexpr ::std::uint_least64_t xxh3_mix_fio(::std::uint_least64_t a, ::std::uint_least64_t b) noexcept
{
	::std::uint_least64_t r{a ^ ::std::rotl(b, 49) ^ ::std::rotl(b, 24)};
	r *= 0x9FB21C651E98DF25ULL;
	return r ^ (r >> 32u);
}

inline ::std::uint_least64_t xxh3_read64_fio(void const *p) noexcept
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

inline ::std::uint_least64_t xxh3_64bits_fio(void const *data, ::std::size_t len) noexcept
{
	char unsigned const *p{reinterpret_cast<char unsigned const *>(data)};
	::std::uint_least64_t acc{0x9E3779B185EBCA87ULL};

	while (len >= 16)
	{
		::std::uint_least64_t v1{::fast_io::details::xxh3_read64_fio(p)};
		::std::uint_least64_t v2{::fast_io::details::xxh3_read64_fio(p + 8)};

		::std::uint_least64_t s1{::fast_io::details::xxh3_secrect_fio[(*p + p[8]) & 7]};
		::std::uint_least64_t s2{::fast_io::details::xxh3_secrect_fio[(p[3] + p[11]) & 7]};

		acc ^= ::fast_io::details::xxh3_mix_fio(v1 ^ s1, v2 ^ s2);

		p += 16;
		len -= 16;
	}

	// Process remaining bytes
	::std::uint_least64_t last{len ? ::fast_io::details::xxh3_read64_fio(p + len - (len >= 8 ? 8 : len)) : 0u};
	acc ^= ::fast_io::details::xxh3_mix_fio(last, static_cast<::std::uint_least64_t>(len));

	// Final avalanche
	acc ^= acc >> 33;
	acc *= 0xC2B2AE3D27D4EB4FULL;
	acc ^= acc >> 29;
	acc *= 0x165667B19E3779F9ULL;
	acc ^= acc >> 32;
	return acc;
}

} // namespace details

} // namespace fast_io
