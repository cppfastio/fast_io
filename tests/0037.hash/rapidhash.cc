#include <fast_io.h>
#include <fast_io_crypto/hash/rapidhash.h>

#ifdef FAST_IO_RAPIDHASH_TEST_WITH_ORIGINAL
#include <rapidhash.h>
#endif

// Reference values from the original rapidhash library (V3).
struct ref_entry
{
	char const *name;
	void const *data;
	::std::size_t len;
	::std::uint_least64_t expected;
};

int main()
{
	using namespace fast_io::iomnp;

	char buf112[112];
	for (::std::size_t i = 0; i < 112; ++i)
	{
		buf112[i] = static_cast<char>(i * 7 + 3);
	}

	char buf[1024];
	for (::std::size_t i = 0; i < 1024; ++i)
	{
		buf[i] = static_cast<char>(i & 0xff);
	}

	ref_entry tests[] = {
		{"empty", "", 0, 0x0338dc4be2cecdae},
		{"len1", "a", 1, 0x599f47df33a2e1eb},
		{"len3", "abc", 3, 0xcb475beafa9c0da2},
		{"len4", "abcd", 4, 0xf8f44f4a65e26132},
		{"len8", "abcdefgh", 8, 0xab159e602a29f41f},
		{"len16", "abcdefghijklmnop", 16, 0xc78ae6a1774adb1e},
		{"len17", "abcdefghijklmnopq", 17, 0x00c427c11a4463b8},
		{"len43", "The quick brown fox jumps over the lazy dog", 43, 0x91722dc8d52a3f7b},
		{"buf112", buf112, 112, 0xbde27e659c073097},
		{"buf113", buf, 113, 0xabaf0e2bdacf7e23},
		{"buf150", buf, 150, 0x548edfe200a7b543},
		{"buf224", buf, 224, 0xeefa9c2e54fc0df1},
		{"buf225", buf, 225, 0xf6c6e7081ab8456d},
		{"buf336", buf, 336, 0x6ee01e545d9e26b3},
		{"buf500", buf, 500, 0xb943a1d6f7b18de4},
		{"buf1024", buf, 1024, 0x1457ae9b031a8d70},
	};

	auto h = fast_io::details::rapidhash_64bits_fio;
	::std::size_t pass{}, fail{};

	::fast_io::out_buf_type obf{::fast_io::out()};

	for (auto &t : tests)
	{
		auto got = h(t.data, t.len);
		print(obf, os_c_str(t.name));
		if (got == t.expected)
		{
			++pass;
			print(obf, " PASS");
		}
		else
		{
			++fail;
			print(obf, " FAIL");
		}
		println(obf, " got=", hex(got), " expected=", hex(t.expected));
	}

	print(obf, pass, " passed, ", fail, " failed\n");

#ifdef FAST_IO_RAPIDHASH_TEST_WITH_ORIGINAL
	print(obf, "\ncross-check with original rapidhash:\n");
	for (auto &t : tests)
	{
		auto orig = rapidhash(t.data, t.len);
		auto fio = h(t.data, t.len);
		print(obf, os_c_str(t.name), " ");
		if (orig != fio)
		{
			print(obf, "MIS");
		}
		println(obf, "MATCH orig=", hex(orig), " fio=", hex(fio));
	}
	print(obf, "cross-check done\n");
#endif
}
