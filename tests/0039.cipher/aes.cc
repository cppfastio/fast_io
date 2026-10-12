#include <fast_io.h>
#include <fast_io_crypto.h>

/*
FIPS-197 known-answer tests (Appendix B / C.1-C.3):
key  = 00 01 02 ... (key_size-1)
pt   = 00 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff
*/

namespace
{

template <::std::size_t n>
constexpr auto seq_bytes() noexcept
{
	::fast_io::freestanding::array<::std::byte, n> a{};
	for (::std::size_t i{}; i != n; ++i)
	{
		a[i] = static_cast<::std::byte>(i);
	}
	return a;
}

constexpr bool byte_eq(::std::byte const *a, ::std::byte const *b, ::std::size_t n) noexcept
{
	for (::std::size_t i{}; i != n; ++i)
	{
		if (a[i] != b[i])
		{
			return false;
		}
	}
	return true;
}

constexpr ::std::byte plaintext[16]{
	::std::byte{0x00}, ::std::byte{0x11}, ::std::byte{0x22}, ::std::byte{0x33},
	::std::byte{0x44}, ::std::byte{0x55}, ::std::byte{0x66}, ::std::byte{0x77},
	::std::byte{0x88}, ::std::byte{0x99}, ::std::byte{0xaa}, ::std::byte{0xbb},
	::std::byte{0xcc}, ::std::byte{0xdd}, ::std::byte{0xee}, ::std::byte{0xff}};

constexpr ::std::byte ct128[16]{
	::std::byte{0x69}, ::std::byte{0xc4}, ::std::byte{0xe0}, ::std::byte{0xd8},
	::std::byte{0x6a}, ::std::byte{0x7b}, ::std::byte{0x04}, ::std::byte{0x30},
	::std::byte{0xd8}, ::std::byte{0xcd}, ::std::byte{0xb7}, ::std::byte{0x80},
	::std::byte{0x70}, ::std::byte{0xb4}, ::std::byte{0xc5}, ::std::byte{0x5a}};

constexpr ::std::byte ct192[16]{
	::std::byte{0xdd}, ::std::byte{0xa9}, ::std::byte{0x7c}, ::std::byte{0xa4},
	::std::byte{0x86}, ::std::byte{0x4c}, ::std::byte{0xdf}, ::std::byte{0xe0},
	::std::byte{0x6e}, ::std::byte{0xaf}, ::std::byte{0x70}, ::std::byte{0xa0},
	::std::byte{0xec}, ::std::byte{0x0d}, ::std::byte{0x71}, ::std::byte{0x91}};

constexpr ::std::byte ct256[16]{
	::std::byte{0x8e}, ::std::byte{0xa2}, ::std::byte{0xb7}, ::std::byte{0xca},
	::std::byte{0x51}, ::std::byte{0x67}, ::std::byte{0x45}, ::std::byte{0xbf},
	::std::byte{0xea}, ::std::byte{0xfc}, ::std::byte{0x49}, ::std::byte{0x90},
	::std::byte{0x4b}, ::std::byte{0x49}, ::std::byte{0x60}, ::std::byte{0x89}};

template <::std::size_t keysz>
constexpr bool kat(::std::byte const (&expected)[16]) noexcept
{
	auto key{seq_bytes<keysz>()};
	::fast_io::aes_ctx<keysz> ctx{key.data()};
	::std::byte buf[16];
	ctx.encrypt(plaintext, 1, buf);
	if (!byte_eq(buf, expected, 16))
	{
		return false;
	}
	ctx.decrypt(buf, 1, buf); /* decrypt in place */
	return byte_eq(buf, plaintext, 16);
}

constexpr bool ecb_roundtrip() noexcept
{
	auto key{seq_bytes<16>()};
	::fast_io::aes_ctx<16> ctx{key.data()};
	::std::byte in[48];
	for (::std::size_t i{}; i != 48; ++i)
	{
		in[i] = static_cast<::std::byte>(i * 7 + 3);
	}
	::std::byte buf[48];
	ctx.encrypt(in, 3, buf);
	ctx.decrypt(buf, 3, buf);
	return byte_eq(buf, in, 48);
}

static_assert(kat<16>(ct128));
static_assert(kat<24>(ct192));
static_assert(kat<32>(ct256));
static_assert(ecb_roundtrip());

} // namespace

int main()
{
	::fast_io::println("aes128 KAT: ", kat<16>(ct128));
	::fast_io::println("aes192 KAT: ", kat<24>(ct192));
	::fast_io::println("aes256 KAT: ", kat<32>(ct256));
	::fast_io::println("ecb roundtrip: ", ecb_roundtrip());
}
