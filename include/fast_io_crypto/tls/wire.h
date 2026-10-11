#pragma once

/*
TLS wire-format readers/writers. Everything on the wire is big-endian.
A reader walks a buffer once; every read bounds-checks and returns false
on truncation so callers can raise decode_error / decrypt_error.
*/

namespace fast_io::tls
{

/* big-endian scalar encode */
inline constexpr ::std::byte *wire_put_u16(::std::byte *p, ::std::uint_least16_t v) noexcept
{
	*p++ = static_cast<::std::byte>(v >> 8u);
	*p++ = static_cast<::std::byte>(v);
	return p;
}

inline constexpr ::std::byte *wire_put_u24(::std::byte *p, ::std::uint_least32_t v) noexcept
{
	*p++ = static_cast<::std::byte>(v >> 16u);
	*p++ = static_cast<::std::byte>(v >> 8u);
	*p++ = static_cast<::std::byte>(v);
	return p;
}

inline constexpr ::std::byte *wire_put_u32(::std::byte *p, ::std::uint_least32_t v) noexcept
{
	*p++ = static_cast<::std::byte>(v >> 24u);
	*p++ = static_cast<::std::byte>(v >> 16u);
	*p++ = static_cast<::std::byte>(v >> 8u);
	*p++ = static_cast<::std::byte>(v);
	return p;
}

inline constexpr ::std::byte *wire_put_u64(::std::byte *p, ::std::uint_least64_t v) noexcept
{
	for (unsigned s{56u};;)
	{
		*p++ = static_cast<::std::byte>(v >> s);
		if (s == 0)
		{
			return p;
		}
		s -= 8u;
	}
}

inline constexpr ::std::byte *wire_put_bytes(::std::byte *p, ::std::byte const *q, ::std::size_t n) noexcept
{
	return ::fast_io::details::non_overlapped_copy_n(q, n, p);
}

struct wire_reader
{
	::std::byte const *cur{};
	::std::byte const *end{};

	inline constexpr ::std::size_t remaining() const noexcept
	{
		return static_cast<::std::size_t>(end - cur);
	}

	inline constexpr bool is_empty() const noexcept
	{
		return cur == end;
	}

	inline constexpr bool take_u8(::std::uint_least8_t &v) noexcept
	{
		if (cur == end)
		{
			return false;
		}
		v = static_cast<::std::uint_least8_t>(*cur++);
		return true;
	}

	inline constexpr bool take_u16(::std::uint_least16_t &v) noexcept
	{
		if (remaining() < 2)
		{
			return false;
		}
		v = static_cast<::std::uint_least16_t>((static_cast<::std::uint_least16_t>(cur[0]) << 8u) |
											   static_cast<::std::uint_least16_t>(cur[1]));
		cur += 2;
		return true;
	}

	inline constexpr bool take_u24(::std::uint_least32_t &v) noexcept
	{
		if (remaining() < 3)
		{
			return false;
		}
		v = (static_cast<::std::uint_least32_t>(cur[0]) << 16u) |
			(static_cast<::std::uint_least32_t>(cur[1]) << 8u) |
			static_cast<::std::uint_least32_t>(cur[2]);
		cur += 3;
		return true;
	}

	inline constexpr bool take_u32(::std::uint_least32_t &v) noexcept
	{
		if (remaining() < 4)
		{
			return false;
		}
		v = (static_cast<::std::uint_least32_t>(cur[0]) << 24u) |
			(static_cast<::std::uint_least32_t>(cur[1]) << 16u) |
			(static_cast<::std::uint_least32_t>(cur[2]) << 8u) |
			static_cast<::std::uint_least32_t>(cur[3]);
		cur += 4;
		return true;
	}

	/* raw span of n bytes (bounds-checked) */
	inline constexpr bool take_bytes(::std::byte const *&p, ::std::size_t n) noexcept
	{
		if (remaining() < n)
		{
			return false;
		}
		p = cur;
		cur += n;
		return true;
	}

	/* length-prefixed field: u8 len, u16 len or u24 len */
	inline constexpr bool take_vector8(::std::byte const *&p, ::std::size_t &n) noexcept
	{
		::std::uint_least8_t len;
		if (!take_u8(len) || remaining() < len)
		{
			return false;
		}
		p = cur;
		n = len;
		cur += len;
		return true;
	}

	inline constexpr bool take_vector16(::std::byte const *&p, ::std::size_t &n) noexcept
	{
		::std::uint_least16_t len;
		if (!take_u16(len) || remaining() < len)
		{
			return false;
		}
		p = cur;
		n = len;
		cur += len;
		return true;
	}

	inline constexpr bool take_vector24(::std::byte const *&p, ::std::size_t &n) noexcept
	{
		::std::uint_least32_t len;
		if (!take_u24(len) || remaining() < len)
		{
			return false;
		}
		p = cur;
		n = len;
		cur += len;
		return true;
	}

	/* sub-reader over a vector16; advances this reader past it */
	inline constexpr bool take_sub16(wire_reader &sub) noexcept
	{
		::std::byte const *p;
		::std::size_t n;
		if (!take_vector16(p, n))
		{
			return false;
		}
		sub.cur = p;
		sub.end = p + n;
		return true;
	}
};

} // namespace fast_io::tls
