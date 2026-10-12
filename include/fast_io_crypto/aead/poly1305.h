#pragma once

/*
Poly1305 message authentication (RFC 8439 2.5).
Classic 5x26-bit limb representation; message processed in 16-byte
blocks with the hibit carried per block.
*/

namespace fast_io::details::aead
{

class poly1305_state
{
	::std::uint_least32_t r[5]{}, h[5]{};
	::std::uint_least32_t pad[4]{};
	::std::byte buf[16];
	::std::size_t buf_size{};

	static inline constexpr ::std::uint_least32_t load_le32(::std::byte const *p) noexcept
	{
		return static_cast<::std::uint_least32_t>(p[0]) |
			   (static_cast<::std::uint_least32_t>(p[1]) << 8u) |
			   (static_cast<::std::uint_least32_t>(p[2]) << 16u) |
			   (static_cast<::std::uint_least32_t>(p[3]) << 24u);
	}

	inline constexpr void block(::std::byte const (&m)[16], bool final) noexcept
	{
		::std::uint_least64_t const t0{load_le32(m + 0)};
		::std::uint_least64_t const t1{load_le32(m + 4)};
		::std::uint_least64_t const t2{load_le32(m + 8)};
		::std::uint_least64_t const t3{load_le32(m + 12)};
		h[0] += static_cast<::std::uint_least32_t>(t0 & 0x3ffffffu);
		h[1] += static_cast<::std::uint_least32_t>(((t0 >> 26u) | (t1 << 6u)) & 0x3ffffffu);
		h[2] += static_cast<::std::uint_least32_t>(((t1 >> 20u) | (t2 << 12u)) & 0x3ffffffu);
		h[3] += static_cast<::std::uint_least32_t>(((t2 >> 14u) | (t3 << 18u)) & 0x3ffffffu);
		h[4] += static_cast<::std::uint_least32_t>((t3 >> 8u) | (final ? 0u : (1u << 24u)));

		::std::uint_least64_t const s1{static_cast<::std::uint_least64_t>(r[1]) * 5u};
		::std::uint_least64_t const s2{static_cast<::std::uint_least64_t>(r[2]) * 5u};
		::std::uint_least64_t const s3{static_cast<::std::uint_least64_t>(r[3]) * 5u};
		::std::uint_least64_t const s4{static_cast<::std::uint_least64_t>(r[4]) * 5u};
		::std::uint_least64_t d[5];
		d[0] = static_cast<::std::uint_least64_t>(h[0]) * r[0] + static_cast<::std::uint_least64_t>(h[1]) * s4 +
			   static_cast<::std::uint_least64_t>(h[2]) * s3 + static_cast<::std::uint_least64_t>(h[3]) * s2 +
			   static_cast<::std::uint_least64_t>(h[4]) * s1;
		d[1] = static_cast<::std::uint_least64_t>(h[0]) * r[1] + static_cast<::std::uint_least64_t>(h[1]) * r[0] +
			   static_cast<::std::uint_least64_t>(h[2]) * s4 + static_cast<::std::uint_least64_t>(h[3]) * s3 +
			   static_cast<::std::uint_least64_t>(h[4]) * s2;
		d[2] = static_cast<::std::uint_least64_t>(h[0]) * r[2] + static_cast<::std::uint_least64_t>(h[1]) * r[1] +
			   static_cast<::std::uint_least64_t>(h[2]) * r[0] + static_cast<::std::uint_least64_t>(h[3]) * s4 +
			   static_cast<::std::uint_least64_t>(h[4]) * s3;
		d[3] = static_cast<::std::uint_least64_t>(h[0]) * r[3] + static_cast<::std::uint_least64_t>(h[1]) * r[2] +
			   static_cast<::std::uint_least64_t>(h[2]) * r[1] + static_cast<::std::uint_least64_t>(h[3]) * r[0] +
			   static_cast<::std::uint_least64_t>(h[4]) * s4;
		d[4] = static_cast<::std::uint_least64_t>(h[0]) * r[4] + static_cast<::std::uint_least64_t>(h[1]) * r[3] +
			   static_cast<::std::uint_least64_t>(h[2]) * r[2] + static_cast<::std::uint_least64_t>(h[3]) * r[1] +
			   static_cast<::std::uint_least64_t>(h[4]) * r[0];

		::std::uint_least64_t c{d[0] >> 26u};
		h[0] = static_cast<::std::uint_least32_t>(d[0] & 0x3ffffffu);
		d[1] += c;
		c = d[1] >> 26u;
		h[1] = static_cast<::std::uint_least32_t>(d[1] & 0x3ffffffu);
		d[2] += c;
		c = d[2] >> 26u;
		h[2] = static_cast<::std::uint_least32_t>(d[2] & 0x3ffffffu);
		d[3] += c;
		c = d[3] >> 26u;
		h[3] = static_cast<::std::uint_least32_t>(d[3] & 0x3ffffffu);
		d[4] += c;
		c = d[4] >> 26u;
		h[4] = static_cast<::std::uint_least32_t>(d[4] & 0x3ffffffu);
		h[0] += static_cast<::std::uint_least32_t>(c * 5u);
		h[1] += h[0] >> 26u;
		h[0] &= 0x3ffffffu;
	}

public:
	inline constexpr void init(::std::byte const (&key)[32]) noexcept
	{
		::std::uint_least64_t const t0{load_le32(key + 0)};
		::std::uint_least64_t const t1{load_le32(key + 4)};
		::std::uint_least64_t const t2{load_le32(key + 8)};
		::std::uint_least64_t const t3{load_le32(key + 12)};
		r[0] = static_cast<::std::uint_least32_t>(t0 & 0x3ffffffu);
		r[1] = static_cast<::std::uint_least32_t>(((t0 >> 26u) | (t1 << 6u)) & 0x3ffff03u);
		r[2] = static_cast<::std::uint_least32_t>(((t1 >> 20u) | (t2 << 12u)) & 0x3ffc0ffu);
		r[3] = static_cast<::std::uint_least32_t>(((t2 >> 14u) | (t3 << 18u)) & 0x3f03fffu);
		r[4] = static_cast<::std::uint_least32_t>((t3 >> 8u) & 0x00fffffu);
		for (::std::size_t i{}; i != 4; ++i)
		{
			pad[i] = load_le32(key + 16 + i * 4);
		}
		for (::std::size_t i{}; i != 5; ++i)
		{
			h[i] = 0;
		}
		buf_size = 0;
	}

	inline constexpr void update(::std::byte const *m, ::std::size_t n) noexcept
	{
		if (buf_size != 0)
		{
			::std::size_t const take{16 - buf_size < n ? 16 - buf_size : n};
			for (::std::size_t i{}; i != take; ++i)
			{
				buf[buf_size + i] = m[i];
			}
			buf_size += take;
			m += take;
			n -= take;
			if (buf_size == 16)
			{
				block(buf, false);
				buf_size = 0;
			}
		}
		for (; n >= 16; m += 16, n -= 16)
		{
			::std::byte tmp[16];
			for (::std::size_t i{}; i != 16; ++i)
			{
				tmp[i] = m[i];
			}
			block(tmp, false);
		}
		if (n != 0)
		{
			for (::std::size_t i{}; i != n; ++i)
			{
				buf[i] = m[i];
			}
			buf_size = n;
		}
	}

	inline constexpr void digest_to_byte_ptr(::std::byte *tag /* 16 */) noexcept
	{
		if (buf_size != 0)
		{
			::std::byte m[16]{};
			for (::std::size_t i{}; i != buf_size; ++i)
			{
				m[i] = buf[i];
			}
			m[buf_size] = ::std::byte{1};
			block(m, true);
			buf_size = 0;
		}

		/* full carry */
		::std::uint_least32_t c{h[1] >> 26u};
		h[1] &= 0x3ffffffu;
		h[2] += c;
		c = h[2] >> 26u;
		h[2] &= 0x3ffffffu;
		h[3] += c;
		c = h[3] >> 26u;
		h[3] &= 0x3ffffffu;
		h[4] += c;
		c = h[4] >> 26u;
		h[4] &= 0x3ffffffu;
		h[0] += c * 5u;
		c = h[0] >> 26u;
		h[0] &= 0x3ffffffu;
		h[1] += c;

		/* g = h + 5 - 2^130 (i.e. h - p); select g iff h >= p */
		::std::uint_least32_t g[5];
		c = 5u;
		for (::std::size_t i{}; i != 4; ++i)
		{
			g[i] = h[i] + c;
			c = g[i] >> 26u;
			g[i] &= 0x3ffffffu;
		}
		g[4] = h[4] + c - (1u << 26u);
		/* mask = ~0 when h >= p (no borrow out of g[4]) */
		::std::uint_least32_t const mask{static_cast<::std::uint_least32_t>(static_cast<::std::int_least32_t>(g[4]) >> 31u)};
		for (::std::size_t i{}; i != 5; ++i)
		{
			h[i] = (h[i] & mask) | (g[i] & ~mask);
		}

		/* serialize to four 32-bit words FIRST (upper limb bits beyond
		   each word belong to the NEXT word -- they must not ride the
		   pad carry chain or they would count twice) */
		::std::uint_least32_t w[4];
		w[0] = h[0] | (h[1] << 26u);
		w[1] = (h[1] >> 6u) | (h[2] << 20u);
		w[2] = (h[2] >> 12u) | (h[3] << 14u);
		w[3] = (h[3] >> 18u) | (h[4] << 8u);
		::std::uint_least64_t f{static_cast<::std::uint_least64_t>(w[0]) + pad[0]};
		w[0] = static_cast<::std::uint_least32_t>(f);
		f = static_cast<::std::uint_least64_t>(w[1]) + pad[1] + (f >> 32u);
		w[1] = static_cast<::std::uint_least32_t>(f);
		f = static_cast<::std::uint_least64_t>(w[2]) + pad[2] + (f >> 32u);
		w[2] = static_cast<::std::uint_least32_t>(f);
		f = static_cast<::std::uint_least64_t>(w[3]) + pad[3] + (f >> 32u);
		w[3] = static_cast<::std::uint_least32_t>(f);
		for (::std::size_t i{}; i != 4; ++i)
		{
			tag[i * 4 + 0] = static_cast<::std::byte>(w[i]);
			tag[i * 4 + 1] = static_cast<::std::byte>(w[i] >> 8u);
			tag[i * 4 + 2] = static_cast<::std::byte>(w[i] >> 16u);
			tag[i * 4 + 3] = static_cast<::std::byte>(w[i] >> 24u);
		}
	}
};

} // namespace fast_io::details::aead
