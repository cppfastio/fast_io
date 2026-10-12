#pragma once

/*
Algorithm: JEAIII
Author: jeaiii
*/

namespace fast_io::details::jeaiii
{

inline constexpr ::std::uint_least64_t jeaiii_3digits{(::std::uint_least64_t{1} << 32u) / 10u + 1u};
inline constexpr ::std::uint_least64_t jeaiii_4digits{(::std::uint_least64_t{1} << 32u) / 100u + 1u};
inline constexpr ::std::uint_least64_t jeaiii_5digits{(::std::uint_least64_t{1} << 32u) / 1000u + 1u};
inline constexpr ::std::uint_least64_t jeaiii_6digits{(::std::uint_least64_t{1} << 32u) / 10000u + 1u};
inline constexpr ::std::uint_least64_t jeaiii_7digits{(::std::uint_least64_t{1} << 48u) / 100000u + 1u};
inline constexpr ::std::uint_least64_t jeaiii_8digits{(::std::uint_least64_t{1} << 51u) / 1000000u + 2u};
inline constexpr ::std::uint_least64_t jeaiii_9digits{(::std::uint_least64_t{1} << 55u) / 10000000u + 2u};
inline constexpr ::std::uint_least64_t jeaiii_10digits{(::std::uint_least64_t{1} << 58u) / 100000000u + 1u};
inline constexpr ::std::uint_least32_t jeaiii_u32max{::std::numeric_limits<::std::uint_least32_t>::max()};

template <::std::integral char_type>
inline constexpr char_type *jeaiii_write9_common(char_type *iter, ::std::uint_least32_t u, auto const *digitstb) noexcept
{
	constexpr ::std::size_t tocopybytes{sizeof(char_type) * 2u};
	::std::uint_least64_t t{(jeaiii_9digits * u >> 23u) + 4u};
	::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
	t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
	::fast_io::details::intrinsics::typed_memcpy(iter + 2, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
	t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
	::fast_io::details::intrinsics::typed_memcpy(iter + 4, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
	t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
	::fast_io::details::intrinsics::typed_memcpy(iter + 6, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
	t = ::std::uint_least64_t{10} * static_cast<::std::uint_least32_t>(t);
	iter[8] = ::fast_io::char_literal_add<char_type>(static_cast<::std::uint_least32_t>(t >> 32u));
	return iter + 9;
}

template <::std::integral char_type>
inline constexpr char_type *jeaiii_write9(char_type *iter, ::std::uint_least32_t u) noexcept
{
	constexpr auto const *digitstb{::fast_io::details::digits_table<char_type, 10, false>};
	return jeaiii_write9_common(iter, u, digitstb);
}

template <::std::integral char_type>
inline constexpr char_type *jeaiii_main_u8(char_type *iter, ::std::uint_least8_t n) noexcept
{
	constexpr auto const *digitstb{::fast_io::details::digits_table<char_type, 10, false>};
	constexpr ::std::size_t tocopybytes{sizeof(char_type) * 2u};
	::std::uint_least32_t u{n};
	if (u < 100u)
	{
		if (u < 10u)
		{
			*iter = ::fast_io::char_literal_add<char_type>(u);
			return iter + 1;
		}
		::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(u) << 1u), tocopybytes);
		return iter + 2;
	}
	::std::uint_least32_t const h{u / 100u};
	*iter = ::fast_io::char_literal_add<char_type>(h);
	::fast_io::details::intrinsics::typed_memcpy(iter + 1, digitstb + (static_cast<::std::size_t>(u - h * 100u) << 1u), tocopybytes);
	return iter + 3;
}

template <::std::integral char_type>
inline constexpr char_type *jeaiii_main_u16(char_type *iter, ::std::uint_least16_t n) noexcept
{
	constexpr auto const *digitstb{::fast_io::details::digits_table<char_type, 10, false>};
	constexpr ::std::size_t tocopybytes{sizeof(char_type) * 2u};
	::std::uint_least32_t u{n};
	::std::uint_least64_t t;
	if (u < 100u)
	{
		if (u < 10u)
		{
			*iter = ::fast_io::char_literal_add<char_type>(u);
			return iter + 1;
		}
		::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(u) << 1u), tocopybytes);
		return iter + 2;
	}
	if (u < 10000u)
	{
		if (u < 1000u)
		{
			t = jeaiii_3digits * u;
			::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
			t = ::std::uint_least64_t{10} * static_cast<::std::uint_least32_t>(t);
			iter[2] = ::fast_io::char_literal_add<char_type>(static_cast<::std::uint_least32_t>(t >> 32u));
			return iter + 3;
		}
		t = jeaiii_4digits * u;
		::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 2, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		return iter + 4;
	}
	t = jeaiii_5digits * u;
	::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
	t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
	::fast_io::details::intrinsics::typed_memcpy(iter + 2, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
	t = ::std::uint_least64_t{10} * static_cast<::std::uint_least32_t>(t);
	iter[4] = ::fast_io::char_literal_add<char_type>(static_cast<::std::uint_least32_t>(t >> 32u));
	return iter + 5;
}

template <::std::integral char_type>
inline constexpr char_type *jeaiii_main_u32_common(char_type *iter, ::std::uint_least32_t u, auto const *digitstb) noexcept
{
	constexpr ::std::size_t tocopybytes{sizeof(char_type) * 2u};
	::std::uint_least64_t t;
	if (u < 10000u)
	{
		if (u < 100u)
		{
			if (u < 10u)
			{
				*iter = ::fast_io::char_literal_add<char_type>(u);
				return iter + 1;
			}
			::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(u) << 1u), tocopybytes);
			return iter + 2;
		}
		if (u < 1000u)
		{
			t = jeaiii_3digits * u;
			::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
			t = ::std::uint_least64_t{10} * static_cast<::std::uint_least32_t>(t);
			iter[2] = ::fast_io::char_literal_add<char_type>(static_cast<::std::uint_least32_t>(t >> 32u));
			return iter + 3;
		}
		t = jeaiii_4digits * u;
		::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 2, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		return iter + 4;
	}
	if (u < 100000000u)
	{
		if (u < 1000000u)
		{
			if (u < 100000u)
			{
				t = jeaiii_5digits * u;
				::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
				t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
				::fast_io::details::intrinsics::typed_memcpy(iter + 2, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
				t = ::std::uint_least64_t{10} * static_cast<::std::uint_least32_t>(t);
				iter[4] = ::fast_io::char_literal_add<char_type>(static_cast<::std::uint_least32_t>(t >> 32u));
				return iter + 5;
			}
			t = jeaiii_6digits * u;
			::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
			t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
			::fast_io::details::intrinsics::typed_memcpy(iter + 2, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
			t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
			::fast_io::details::intrinsics::typed_memcpy(iter + 4, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
			return iter + 6;
		}
		if (u < 10000000u)
		{
			t = (jeaiii_7digits * u) >> 16u;
			::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
			t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
			::fast_io::details::intrinsics::typed_memcpy(iter + 2, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
			t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
			::fast_io::details::intrinsics::typed_memcpy(iter + 4, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
			t = ::std::uint_least64_t{10} * static_cast<::std::uint_least32_t>(t);
			iter[6] = ::fast_io::char_literal_add<char_type>(static_cast<::std::uint_least32_t>(t >> 32u));
			return iter + 7;
		}
		t = (jeaiii_8digits * u >> 19u) + 4u;
		::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 2, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 4, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 6, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		return iter + 8;
	}
	if (u < 1000000000u)
	{
		t = (jeaiii_9digits * u >> 23u) + 4u;
		::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 2, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 4, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 6, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{10} * static_cast<::std::uint_least32_t>(t);
		iter[8] = ::fast_io::char_literal_add<char_type>(static_cast<::std::uint_least32_t>(t >> 32u));
		return iter + 9;
	}
	t = (jeaiii_10digits * u >> 26u) + 4u;
	::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
	t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
	::fast_io::details::intrinsics::typed_memcpy(iter + 2, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
	t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
	::fast_io::details::intrinsics::typed_memcpy(iter + 4, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
	t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
	::fast_io::details::intrinsics::typed_memcpy(iter + 6, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
	t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
	::fast_io::details::intrinsics::typed_memcpy(iter + 8, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
	return iter + 10;
}

template <::std::integral char_type>
inline constexpr char_type *jeaiii_main_u32(char_type *iter, ::std::uint_least32_t u) noexcept
{
	constexpr auto const *digitstb{::fast_io::details::digits_table<char_type, 10, false>};
	return jeaiii_main_u32_common(iter, u, digitstb);
}

template <::std::integral char_type>
inline constexpr char_type *jeaiii_main_u64(char_type *iter, ::std::uint_least64_t n) noexcept
{
	constexpr auto const *digitstb{::fast_io::details::digits_table<char_type, 10, false>};
	constexpr ::std::size_t tocopybytes{sizeof(char_type) * 2u};
	constexpr ::std::uint_least64_t divisor{1000000000u};
	if (static_cast<::std::uint_least32_t>(n) == n)
	{
		return jeaiii_main_u32_common(iter, static_cast<::std::uint_least32_t>(n), digitstb);
	}
	::std::uint_least64_t a{n / divisor};
	if (a <= jeaiii_u32max)
	{
		iter = jeaiii_main_u32_common(iter, static_cast<::std::uint_least32_t>(a), digitstb);
	}
	else
	{
		::std::uint_least32_t v{static_cast<::std::uint_least32_t>(a / divisor)};
		if (v < 10u)
		{
			*iter = ::fast_io::char_literal_add<char_type>(v);
			++iter;
		}
		else
		{
			::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(v) << 1u), tocopybytes);
			iter += 2;
		}
		iter = jeaiii_write9_common(iter, static_cast<::std::uint_least32_t>(a % divisor), digitstb);
	}
	return jeaiii_write9_common(iter, static_cast<::std::uint_least32_t>(n % divisor), digitstb);
}

template <::std::integral char_type>
inline constexpr void jeaiii_len_u32_common(char_type *iter, ::std::uint_least32_t u, ::std::uint_least32_t len, auto const *digitstb) noexcept
{
	constexpr ::std::size_t tocopybytes{sizeof(char_type) * 2u};
	::std::uint_least64_t t;
	switch (len)
	{
	case 1:
	{
		*iter = ::fast_io::char_literal_add<char_type>(u);
		return;
	}
	case 2:
	{
		::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(u) << 1u), tocopybytes);
		return;
	}
	case 3:
	{
		t = jeaiii_3digits * u;
		::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{10} * static_cast<::std::uint_least32_t>(t);
		iter[2] = ::fast_io::char_literal_add<char_type>(static_cast<::std::uint_least32_t>(t >> 32u));
		return;
	}
	case 4:
	{
		t = jeaiii_4digits * u;
		::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 2, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		return;
	}
	case 5:
	{
		t = jeaiii_5digits * u;
		::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 2, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{10} * static_cast<::std::uint_least32_t>(t);
		iter[4] = ::fast_io::char_literal_add<char_type>(static_cast<::std::uint_least32_t>(t >> 32u));
		return;
	}
	case 6:
	{
		t = jeaiii_6digits * u;
		::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 2, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 4, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		return;
	}
	case 7:
	{
		t = (jeaiii_7digits * u) >> 16u;
		::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 2, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 4, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{10} * static_cast<::std::uint_least32_t>(t);
		iter[6] = ::fast_io::char_literal_add<char_type>(static_cast<::std::uint_least32_t>(t >> 32u));
		return;
	}
	case 8:
	{
		t = (jeaiii_8digits * u >> 19u) + 4u;
		::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 2, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 4, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 6, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		return;
	}
	case 9:
	{
		t = (jeaiii_9digits * u >> 23u) + 4u;
		::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 2, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 4, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 6, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{10} * static_cast<::std::uint_least32_t>(t);
		iter[8] = ::fast_io::char_literal_add<char_type>(static_cast<::std::uint_least32_t>(t >> 32u));
		return;
	}
	default:
	{
		t = (jeaiii_10digits * u >> 26u) + 4u;
		::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 2, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 4, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 6, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		t = ::std::uint_least64_t{100} * static_cast<::std::uint_least32_t>(t);
		::fast_io::details::intrinsics::typed_memcpy(iter + 8, digitstb + (static_cast<::std::size_t>(t >> 32u) << 1u), tocopybytes);
		return;
	}
	}
}

template <::std::integral char_type>
inline constexpr void jeaiii_len_u32(char_type *iter, ::std::uint_least32_t u, ::std::uint_least32_t len) noexcept
{
	constexpr auto const *digitstb{::fast_io::details::digits_table<char_type, 10, false>};
	jeaiii_len_u32_common(iter, u, len, digitstb);
}

template <::std::integral char_type>
inline constexpr void jeaiii_main_len_u64(char_type *iter, ::std::uint_least64_t n, ::std::uint_least32_t len) noexcept
{
	constexpr auto const *digitstb{::fast_io::details::digits_table<char_type, 10, false>};
	constexpr ::std::size_t tocopybytes{sizeof(char_type) * 2u};
	constexpr ::std::uint_least64_t divisor{1000000000u};
	if (static_cast<::std::uint_least32_t>(n) == n)
	{
		jeaiii_len_u32_common(iter, static_cast<::std::uint_least32_t>(n), len, digitstb);
		return;
	}
	::std::uint_least64_t a{n / divisor};
	len -= 9u;
	if (a <= jeaiii_u32max)
	{
		jeaiii_len_u32_common(iter, static_cast<::std::uint_least32_t>(a), len, digitstb);
		iter += len;
	}
	else
	{
		::std::uint_least32_t v{static_cast<::std::uint_least32_t>(a / divisor)};
		if (v < 10u)
		{
			*iter = ::fast_io::char_literal_add<char_type>(v);
			++iter;
		}
		else
		{
			::fast_io::details::intrinsics::typed_memcpy(iter, digitstb + (static_cast<::std::size_t>(v) << 1u), tocopybytes);
			iter += 2;
		}
		iter = jeaiii_write9_common(iter, static_cast<::std::uint_least32_t>(a % divisor), digitstb);
	}
	jeaiii_write9_common(iter, static_cast<::std::uint_least32_t>(n % divisor), digitstb);
}

} // namespace fast_io::details::jeaiii
