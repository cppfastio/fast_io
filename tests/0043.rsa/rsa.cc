#include <fast_io.h>
#include <fast_io_crypto.h>

/*
RSA verification known-answer tests. The key is a real RSA-2048 key
generated with openssl genpkey; the signatures were produced by
openssl pkeyutl (RSASSA-PKCS1-v1_5 and RSASSA-PSS, SHA-256, salt length
32). Tampered signatures must be rejected.
*/

namespace
{

constexpr ::std::size_t hex2bytes(::std::byte *out, char8_t const *hex) noexcept
{
	::std::size_t n{};
	for (; *hex; hex += 2)
	{
		auto hv = [](char8_t c) constexpr noexcept -> ::std::uint_least8_t {
			return c <= u8'9' ? static_cast<::std::uint_least8_t>(c - u8'0') : static_cast<::std::uint_least8_t>((c | u8' ') - u8'a' + 10);
		};
		out[n++] = ::std::byte{static_cast<char unsigned>(hv(hex[0]) << 4 | hv(hex[1]))};
	}
	return n;
}

constexpr char8_t modulus_hex[] = u8"d7add04939dcd0ffd8b2ca33ccc39d3809e1f78a2a01543eb0f34e7316ecd12647e6acd02230d3aba3a477a8180566e241431af4656430b9b57d2ecd0a369f36de1c00a50b3a5ef86512fc6ad5243618864c86bade0af14c40cd8a9e526d7aad9cc1a79b85f64493720e76fee3af8238a9e0cc5a7cc51acc4a0c1905ad16a08b642ae0f8ad7c5f026d244785fde23bf1621559facfe166c3bb693f949672ede9979e8e314fcd081f2bb2228c76d9ff7cebe3504eecda4c1306cbe6dc93b75fed08a501ea0e7fbf2b652be945e12cac3189625f8aa1a274963c479695d055db6fe6a8c26e3a7b0d2817bd6ac495b851858ce6246c1f73da9efc3fcd283d573917";
constexpr char8_t exponent_hex[] = u8"010001";
constexpr char8_t sig_pkcs1_hex[] = u8"16c69ea227358b3d67ef9c336e22ebc01b24b7780f78fcb6724ceaf348266011f2cfa0dd06a0cca0317b3c6a3a49c4a74e683ebf493d4c370469cdab7db69c5bbe790f46a5597942ff8146a6835228aa6fa9d70d448a6208742622224fb5549546d844e5eb85a8d8cbe902adf3155026088a720b5ea420af38f7d418db19036bc3f423429eea2a31c0b5e5de96af9f773646feeb240ddf0021641cf5470cf11e38e3d7f7336abf7d93ed65b7abb145a9d03fdbd150495f561bbbde421c4c5774d7196f1aef96de98c6eae7b85dee252ba023c6df2f0f7ba9e530a3d73053edb3b9f8be2a692a648f9ecc89bd461ddef17ebec0d70bb3f1dcab6dba362a42eb07";
constexpr char8_t sig_pss_hex[] = u8"4b46f26ac7efe84115e3115134e85f20d7d1bf2c8d9a06ff84f4e5024f14bcd8939935a0fd4355284c9d7c83b6bb3825118ed2695767d6d8995f7fb2d95fef2df2600bafc322c9f45eabf18ff771665017414e14778ffc0d70cd6fc05ddfd7d1a3c93cde42a68fd2c90901d93f006577150cdbe6e97e689d2829d4dabd6efd2ebc9428488540e701d5838d91d5ba3e4a33ed540e3f3d6a8a02ef761b349cb41e1a077172f1a33d5e32d2c55f5121b8c7e52ade1df51162dd33ef9a23aafc85451bdaa622e8d7199eeda4cffa08bb0bd8152be78c170e9f86a55f34deef72700476c10fdc77516c31c0152bb4d6341596613af4e91ba4ba62bb2a5f6b49a92474";
constexpr char8_t digest_hex[] = u8"a63d116ec46f8086cd96b16f2b1c9cca95d1ea2fa7a52bc1915b9c6de0d85cc1";

/*
Compile-time check: 42^65537 mod (RSA-512 modulus below) evaluated by the
constant evaluator. The expected value was computed with python pow().
*/
constexpr char8_t modulus512_hex[] = u8"b348b871936adeD14a298c2d2f4055697e8dd5531806b26d1a663ca462b8355959211b3371f68b624b25e91d197f8232b85adf7c2abd89214d583617e9e7480b";
constexpr char8_t expect512_hex[] = u8"13d3ee511b1a4e96d9cba95f8f67b0ac36a8afba61f84486655294a856a5155a3b346fc02974e2b604a4aa2b23e774a6f71c60cdcc461b122b76e87503078d1f";

constexpr bool test_constexpr()
{
	::std::byte modulus[64], exponent[3], sig[64]{}, em[64], expect[64];
	::std::size_t const nlen{hex2bytes(modulus, modulus512_hex)};
	hex2bytes(expect, expect512_hex);
	exponent[0] = ::std::byte{1};
	exponent[1] = ::std::byte{0};
	exponent[2] = ::std::byte{1};
	sig[63] = ::std::byte{42};
	::fast_io::rsa::verify_context ctx;
	if (!::fast_io::rsa::verify_init_to_ptr(ctx, modulus, nlen, exponent, 3))
	{
		return false;
	}
	if (!::fast_io::rsa::public_op_to_ptr(em, ctx, sig, sizeof(sig)))
	{
		return false;
	}
	return ::fast_io::details::rsa::bytes_equal(em, expect, 64);
}
static_assert(test_constexpr());

} // namespace

int main()
{
	::std::byte modulus[512], exponent[16], sig1[512], sig2[512], digest[64];
	::std::size_t const nlen{hex2bytes(modulus, modulus_hex)};
	::std::size_t const elen{hex2bytes(exponent, exponent_hex)};
	::std::size_t const s1len{hex2bytes(sig1, sig_pkcs1_hex)};
	::std::size_t const s2len{hex2bytes(sig2, sig_pss_hex)};
	::std::size_t const dlen{hex2bytes(digest, digest_hex)};

	::fast_io::rsa::verify_context ctx;
	if (!::fast_io::rsa::verify_init_to_ptr(ctx, modulus, nlen, exponent, elen))
	{
		::fast_io::io::perrln("rsa: init failed");
		return 1;
	}

	int fails{};
	if (!::fast_io::rsa::verify_pkcs1v15_to_ptr<::fast_io::sha256_context>(ctx, sig1, s1len, digest))
	{
		::fast_io::io::perrln("rsa: pkcs1v15 verify failed");
		++fails;
	}
	if (!::fast_io::rsa::verify_pss_to_ptr<::fast_io::sha256_context>(ctx, sig2, s2len, digest, 32))
	{
		::fast_io::io::perrln("rsa: pss verify failed");
		++fails;
	}
	sig1[100] ^= ::std::byte{1};
	if (::fast_io::rsa::verify_pkcs1v15_to_ptr<::fast_io::sha256_context>(ctx, sig1, s1len, digest))
	{
		::fast_io::io::perrln("rsa: tampered pkcs1v15 accepted");
		++fails;
	}
	sig2[100] ^= ::std::byte{1};
	if (::fast_io::rsa::verify_pss_to_ptr<::fast_io::sha256_context>(ctx, sig2, s2len, digest, 32))
	{
		::fast_io::io::perrln("rsa: tampered pss accepted");
		++fails;
	}
	if (fails == 0)
	{
		::fast_io::io::println("rsa: all tests passed");
	}
	return fails;
}
