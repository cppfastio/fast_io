#pragma once

/*
PEM (rfc7468) block extraction + base64 decode for trust stores.
All functions are constexpr span operations; no allocation -- the caller
supplies output space.
*/

namespace fast_io::tls::details
{

/* sextet -> value, or -1 for invalid. '=' handled by caller. */
inline constexpr int pem_b64_value(char8_t c) noexcept
{
	if (c >= u8'A' && c <= u8'Z')
	{
		return c - u8'A';
	}
	if (c >= u8'a' && c <= u8'z')
	{
		return c - u8'a' + 26;
	}
	if (c >= u8'0' && c <= u8'9')
	{
		return c - u8'0' + 52;
	}
	if (c == u8'+')
	{
		return 62;
	}
	if (c == u8'/')
	{
		return 63;
	}
	return -1;
}

/*
decode base64 text (no whitespace allowed inside; callers strip it).
returns bytes written, or 0 on malformed input. '=' padding only at end.
*/
inline constexpr ::std::size_t pem_b64_decode(::std::byte *out, ::std::size_t out_cap,
											  char8_t const *in, ::std::size_t in_size) noexcept
{
	::std::size_t written{};
	::std::uint_least32_t acc{};
	unsigned acc_bits{};
	for (::std::size_t i{}; i != in_size; ++i)
	{
		char8_t const c{in[i]};
		if (c == u8'=')
		{
			/* rest must all be '=' */
			for (::std::size_t j{i}; j != in_size; ++j)
			{
				if (in[j] != u8'=')
				{
					return 0;
				}
			}
			break;
		}
		int const v{pem_b64_value(static_cast<unsigned char>(c))};
		if (v < 0)
		{
			return 0;
		}
		acc = (acc << 6u) | static_cast<::std::uint_least32_t>(v);
		acc_bits += 6u;
		if (acc_bits >= 8u)
		{
			acc_bits -= 8u;
			if (written == out_cap)
			{
				return 0;
			}
			out[written++] = static_cast<::std::byte>(acc >> acc_bits);
			acc &= (static_cast<::std::uint_least32_t>(1) << acc_bits) - 1u;
		}
	}
	if (acc_bits >= 6u)
	{
		return 0; /* dangling bits that cannot be zero-padded */
	}
	return written;
}

/*
find the next "-----BEGIN <label>----- ... -----END <label>-----" block
starting at *cur (a cstring region [cur,end)); on success returns true,
sets label/body spans (body is the base64 text without whitespace
guaranteed stripped later) and advances cur past the END line.
*/
inline constexpr bool pem_next_block(char8_t const *&cur, char8_t const *end,
									 char8_t const *&label, ::std::size_t &label_size,
									 char8_t const *&b64, ::std::size_t &b64_size) noexcept
{
	constexpr char8_t beg[]{u8"-----BEGIN "};
	constexpr char8_t endm[]{u8"-----END "};
	constexpr ::std::size_t beg_size{sizeof(beg) - 1}, end_size{sizeof(endm) - 1};
	while (cur != end)
	{
		/* find BEGIN marker */
		char8_t const *p{cur};
		while (static_cast<::std::size_t>(end - p) >= beg_size &&
			   ::fast_io::freestanding::my_memcmp(p, beg, beg_size) != 0)
		{
			++p;
		}
		if (static_cast<::std::size_t>(end - p) < beg_size)
		{
			return false;
		}
		p += beg_size;
		char8_t const *ls{p};
		while (p != end && *p != u8'-')
		{
			++p;
		}
		if (static_cast<::std::size_t>(end - p) < 5 ||
			::fast_io::freestanding::my_memcmp(p, u8"-----", 5) != 0)
		{
			cur = p;
			continue; /* malformed BEGIN; keep scanning */
		}
		label = ls;
		label_size = static_cast<::std::size_t>(p - ls);
		p += 5;
		/* skip to end of line */
		while (p != end && *p != u8'\n')
		{
			++p;
		}
		if (p != end)
		{
			++p;
		}
		/* body runs until END marker */
		char8_t const *bs{p};
		char8_t const *q{bs};
		while (static_cast<::std::size_t>(end - q) >= end_size + label_size + 5)
		{
			if (::fast_io::freestanding::my_memcmp(q, endm, end_size) == 0 &&
				::fast_io::freestanding::my_memcmp(q + end_size, label, label_size) == 0 &&
				::fast_io::freestanding::my_memcmp(q + end_size + label_size, u8"-----", 5) == 0)
			{
				break;
			}
			++q;
		}
		if (static_cast<::std::size_t>(end - q) < end_size + label_size + 5)
		{
			return false; /* BEGIN without END */
		}
		b64 = bs;
		b64_size = static_cast<::std::size_t>(q - bs);
		cur = q + end_size + label_size + 5;
		return true;
	}
	return false;
}

/*
decode a pem block body (base64 text possibly containing whitespace)
into out. returns bytes written or 0.
*/
inline constexpr ::std::size_t pem_decode_block(::std::byte *out, ::std::size_t out_cap,
												char8_t const *b64, ::std::size_t b64_size) noexcept
{
	::std::size_t written{};
	::std::uint_least32_t acc{};
	unsigned acc_bits{};
	auto emit{[&](int v) constexpr noexcept -> bool {
		acc = (acc << 6u) | static_cast<::std::uint_least32_t>(v);
		acc_bits += 6u;
		if (acc_bits >= 8u)
		{
			acc_bits -= 8u;
			if (written == out_cap)
			{
				return false;
			}
			out[written++] = static_cast<::std::byte>(acc >> acc_bits);
			acc &= (static_cast<::std::uint_least32_t>(1) << acc_bits) - 1u;
		}
		return true;
	}};
	bool padding{false};
	for (::std::size_t i{}; i != b64_size; ++i)
	{
		char8_t const c{b64[i]};
		if (c == u8' ' || c == u8'\t' || c == u8'\r' || c == u8'\n')
		{
			continue;
		}
		if (c == u8'=')
		{
			padding = true;
			continue;
		}
		if (padding)
		{
			return 0;
		}
		int const v{pem_b64_value(static_cast<unsigned char>(c))};
		if (v < 0 || !emit(v))
		{
			return 0;
		}
	}
	if (acc_bits >= 6u)
	{
		return 0;
	}
	return written;
}

} // namespace fast_io::tls::details
