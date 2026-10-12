#include <fast_io.h>

constexpr ::std::size_t jeaiii_emit(char *b, auto v)
{
	return static_cast<::std::size_t>(::fast_io::details::jeaiii::jeaiii_main(b, v) - b);
}

constexpr bool buf_eq(char const *b, char const *s, ::std::size_t n)
{
	for (::std::size_t i{}; i < n; ++i)
	{
		if (b[i] != s[i])
		{
			return false;
		}
	}
	return true;
}

constexpr int test()
{
	char b[24]{};
	// u8 paths: 1, 2, 3 digits
	if (jeaiii_emit(b, static_cast<::std::uint_least8_t>(7)) != 1 || !buf_eq(b, "7", 1))
	{
		return 1;
	}
	if (jeaiii_emit(b, static_cast<::std::uint_least8_t>(42)) != 2 || !buf_eq(b, "42", 2))
	{
		return 2;
	}
	if (jeaiii_emit(b, static_cast<::std::uint_least8_t>(255)) != 3 || !buf_eq(b, "255", 3))
	{
		return 3;
	}
	// u16 paths: all digit counts
	if (jeaiii_emit(b, static_cast<::std::uint_least16_t>(9)) != 1 || !buf_eq(b, "9", 1))
	{
		return 4;
	}
	if (jeaiii_emit(b, static_cast<::std::uint_least16_t>(999)) != 3 || !buf_eq(b, "999", 3))
	{
		return 5;
	}
	if (jeaiii_emit(b, static_cast<::std::uint_least16_t>(65535)) != 5 || !buf_eq(b, "65535", 5))
	{
		return 6;
	}
	// u32 paths: 3..10 digits
	if (jeaiii_emit(b, 100u) != 3 || !buf_eq(b, "100", 3))
	{
		return 7;
	}
	if (jeaiii_emit(b, 9999u) != 4 || !buf_eq(b, "9999", 4))
	{
		return 8;
	}
	if (jeaiii_emit(b, 100000u) != 6 || !buf_eq(b, "100000", 6))
	{
		return 9;
	}
	if (jeaiii_emit(b, 9999999u) != 7 || !buf_eq(b, "9999999", 7))
	{
		return 10;
	}
	if (jeaiii_emit(b, 4294967295u) != 10 || !buf_eq(b, "4294967295", 10))
	{
		return 11;
	}
	// u64 paths: 10-digit write9 boundary, multi-group, max
	if (jeaiii_emit(b, 9999999999ull) != 10 || !buf_eq(b, "9999999999", 10))
	{
		return 12;
	}
	if (jeaiii_emit(b, 123456789012345678ull) != 18 || !buf_eq(b, "123456789012345678", 18))
	{
		return 13;
	}
	if (jeaiii_emit(b, 18446744073709551615ull) != 20 || !buf_eq(b, "18446744073709551615", 20))
	{
		return 14;
	}
	// known-length emitters
	char b2[24]{};
	::fast_io::details::jeaiii::jeaiii_main_len(b2, 42u, 2);
	if (!buf_eq(b2, "42", 2))
	{
		return 15;
	}
	::fast_io::details::jeaiii::jeaiii_main_len(b2, 5u, 1);
	if (!buf_eq(b2, "5", 1))
	{
		return 16;
	}
	::fast_io::details::jeaiii::jeaiii_main_len(b2, 12345678u, 8);
	if (!buf_eq(b2, "12345678", 8))
	{
		return 17;
	}
	::fast_io::details::jeaiii::jeaiii_main_len(b2, 9876543210ull, 10);
	if (!buf_eq(b2, "9876543210", 10))
	{
		return 18;
	}
	::fast_io::details::jeaiii::jeaiii_main_len(b2, 18446744073709551615ull, 20);
	if (!buf_eq(b2, "18446744073709551615", 20))
	{
		return 19;
	}
	return 0;
}

static_assert(test() == 0);

int main()
{
	return test();
}
