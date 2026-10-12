#include <fast_io.h>
#include <fast_io_crypto.h>

/*
The compile-time checks evaluate whole scalar multiplications; raise the
constexpr step budget when building:  -fconstexpr-steps=100000000
*/

/*
RFC 8032 Ed25519 known-answer tests (TEST 1-3): public key derivation,
signature generation and verification, plus rejection of a tampered
signature. Exercised at compile time and at runtime.
*/

namespace
{

constexpr ::std::size_t hex2bytes(::std::byte *out, char8_t const *hex) noexcept
{
	::std::size_t n{};
	for (; *hex; hex += 2)
	{
		auto hv = [](char8_t c) constexpr noexcept -> ::std::uint_least8_t
		{
			return c <= u8'9' ? static_cast<::std::uint_least8_t>(c - u8'0') : static_cast<::std::uint_least8_t>((c | u8' ') - u8'a' + 10);
		};
		out[n++] = ::std::byte{static_cast<char unsigned>(hv(hex[0]) << 4 | hv(hex[1]))};
	}
	return n;
}

constexpr bool bytes_eq(::std::byte const *a, ::std::byte const *b, ::std::size_t n) noexcept
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

struct ed25519_vector
{
	char8_t const *sk;
	char8_t const *pk;
	char8_t const *sig;
	char8_t const *msg;
};

inline constexpr ed25519_vector ed25519_vectors[]{
	{
		u8"9d61b19deffd5a60ba844af492ec2cc44449c5697b326919703bac031cae7f60",
		u8"d75a980182b10ab7d54bfed3c964073a0ee172f3daa62325af021a68f707511a",
		u8"e5564300c360ac729086e2cc806e828a84877f1eb8e5d974d873e065224901555fb8821590a33bacc61e39701cf9b46bd25bf5f0595bbe24655141438e7a100b",
		u8"",
	},
	{
		u8"4ccd089b28ff96da9db6c346ec114e0f5b8a319f35aba624da8cf6ed4fb8a6fb",
		u8"3d4017c3e843895a92b70aa74d1b7ebc9c982ccf2ec4968cc0cd55f12af4660c",
		u8"92a009a9f0d4cab8720e820b5f642540a2b27b5416503f8fb3762223ebdb69da085ac1e43e15996e458f3613d0f11d8c387b2eaeb4302aeeb00d291612bb0c00",
		u8"72",
	},
	{
		u8"c5aa8df43f9f837bedb7442f31dcb7b166d38535076f094b85ce3a2e0b4458f7",
		u8"fc51cd8e6218a1a38da47ed00230f0580816ed13ba3303ac5deb911548908025",
		u8"6291d657deec24024827e69c3abe01a30ce548a284743a445e3680d7db5ac3ac18ff9b538d16f290ae67f760984dc6594a7c15e9716ed28dc027beceea1ec40a",
		u8"af82",
	},
};

constexpr bool test_ed25519_vector(ed25519_vector const &v) noexcept
{
	::std::byte sk[32], pk[32], priv[64], sig[64], msg[2];
	::std::size_t const msglen{hex2bytes(msg, v.msg)};
	hex2bytes(sk, v.sk);
	::fast_io::ed25519::create_key_pair_to_ptr(pk, priv, sk);
	::std::byte expected[32];
	hex2bytes(expected, v.pk);
	if (!bytes_eq(pk, expected, 32))
	{
		if (!::std::is_constant_evaluated())
		{
			::fast_io::println("pk mismatch\n  expect: ", ::fast_io::mnp::hex_encode(expected),
							   "\n  got   : ", ::fast_io::mnp::hex_encode(pk));
		}
		return false;
	}
	::fast_io::ed25519::sign_message_to_ptr(sig, priv, msg, msglen);
	::std::byte expected_sig[64];
	hex2bytes(expected_sig, v.sig);
	if (!bytes_eq(sig, expected_sig, 64))
	{
		if (!::std::is_constant_evaluated())
		{
			::fast_io::println("sig mismatch\n  expect: ", ::fast_io::mnp::hex_encode(expected_sig),
							   "\n  got   : ", ::fast_io::mnp::hex_encode(sig));
		}
		return false;
	}
	if (!::fast_io::ed25519::verify_signature_to_ptr(sig, pk, msg, msglen))
	{
		if (!::std::is_constant_evaluated())
		{
			::fast_io::println("verify failed for sig: ", ::fast_io::mnp::hex_encode(sig));
		}
		return false;
	}
	sig[0] ^= ::std::byte{1};
	return !::fast_io::ed25519::verify_signature_to_ptr(sig, pk, msg, msglen);
}

constexpr bool test_ed25519() noexcept
{
	for (auto const &v : ed25519_vectors)
	{
		if (!test_ed25519_vector(v))
		{
			return false;
		}
	}
	return true;
}

static_assert(test_ed25519());

/* the same vectors through the index_span/span API surface */
bool test_ed25519_span() noexcept
{
	namespace c = ::fast_io::containers;
	for (auto const &v : ed25519_vectors)
	{
		::std::byte sk[32], pk[32], priv[64], sig[64], msg[2];
		::std::size_t const msglen{hex2bytes(msg, v.msg)};
		hex2bytes(sk, v.sk);
		::fast_io::ed25519::create_key_pair(
			c::index_span<::std::byte, 32>{c::index_unchecked, pk},
			c::index_span<::std::byte, 64>{c::index_unchecked, priv},
			c::index_span<::std::byte const, 32>{c::index_unchecked, sk});
		::std::byte expected[32];
		hex2bytes(expected, v.pk);
		if (!bytes_eq(pk, expected, 32))
		{
			::fast_io::println("pk mismatch\n  expect: ", ::fast_io::mnp::hex_encode(expected),
							   "\n  got   : ", ::fast_io::mnp::hex_encode(pk));
			return false;
		}
		::fast_io::ed25519::sign_message(
			c::index_span<::std::byte, 64>{c::index_unchecked, sig},
			c::index_span<::std::byte const, 64>{c::index_unchecked, priv},
			c::span<::std::byte const>{msg, msg + msglen});
		if (!::fast_io::ed25519::verify_signature(
				c::index_span<::std::byte const, 64>{c::index_unchecked, sig},
				c::index_span<::std::byte const, 32>{c::index_unchecked, pk},
				c::span<::std::byte const>{msg, msg + msglen}))
		{
			return false;
		}
		/* verify_context path */
		::fast_io::ed25519::verify_context ctx;
		::fast_io::ed25519::verify_init(ctx, c::index_span<::std::byte const, 32>{c::index_unchecked, pk});
		if (!::fast_io::ed25519::verify_check(ctx,
											c::index_span<::std::byte const, 64>{c::index_unchecked, sig},
											c::span<::std::byte const>{msg, msg + msglen}))
		{
			return false;
		}
	}
	return true;
}

} // namespace

int main()
{
	bool const ok{test_ed25519()};
	bool const span_ok{test_ed25519_span()};
	::fast_io::print("ed25519 sign/verify: ", ok, "\ned25519 span API: ", span_ok, "\n");
	return !(ok && span_ok);
}
