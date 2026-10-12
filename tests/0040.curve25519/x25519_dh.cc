#include <fast_io.h>
#include <fast_io_crypto.h>

/*
The compile-time checks evaluate whole scalar multiplications; raise the
constexpr step budget when building:  -fconstexpr-steps=100000000
*/

/*
RFC 7748 x25519 known-answer test:
alice/bob public keys and the agreed shared secret, through both the
Montgomery ladder path and the ed25519-table fast path.
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

template <::std::size_t n>
constexpr bool bytes_eq(::std::byte const *a, char8_t const (&hex)[n]) noexcept
{
	::std::byte b[(n - 1) >> 1];
	hex2bytes(b, hex);
	return bytes_eq(a, b, (n - 1) >> 1);
}

constexpr bool test_x25519_dh() noexcept
{
	namespace x = ::fast_io::diffie_hellman;
	::std::byte ask[32], bsk[32], apk[32], bpk[32], sa[32], sb[32];
	hex2bytes(ask, u8"77076d0a7318a57d3c16c17251b26645df4c2f87ebc0992ab177fba51db92c2a");
	hex2bytes(bsk, u8"5dab087e624a8a4b79e17f8b83800ee66f3bb1292618b6fd1c2f8b27ff88e0eb");
	x::x25519::calculate_public_key_to_ptr(apk, ask);
	x::x25519::calculate_public_key_to_ptr(bpk, bsk);
	if (!bytes_eq(apk, u8"8520f0098930a754748b7ddcb43ef75a0dbf3a0d26381af4eba4a98eaa9b4e6a"))
	{
		return false;
	}
	if (!bytes_eq(bpk, u8"de9edb7d7b7dc1b4d35b61c2ece435373f8343c85b78674dadfc7e146f882b4f"))
	{
		return false;
	}
	x::x25519::create_shared_key_to_ptr(sa, bpk, ask);
	x::x25519::create_shared_key_to_ptr(sb, apk, bsk);
	if (!bytes_eq(sa, sb, 32))
	{
		return false;
	}
	if (!bytes_eq(sa, u8"4a5d9d5ba4ce2de1728e3bf480350f25e07e21c947d19e3376f09b3c1e161742"))
	{
		return false;
	}

	/* same vectors through the index_span API and the fast base-table path */
	hex2bytes(ask, u8"77076d0a7318a57d3c16c17251b26645df4c2f87ebc0992ab177fba51db92c2a");
	hex2bytes(bsk, u8"5dab087e624a8a4b79e17f8b83800ee66f3bb1292618b6fd1c2f8b27ff88e0eb");
	x::x25519::calculate_public_key(
		::fast_io::containers::index_span<::std::byte, 32>{::fast_io::containers::index_unchecked, apk},
		::fast_io::containers::index_span<::std::byte, 32>{::fast_io::containers::index_unchecked, ask});
	if (!bytes_eq(apk, u8"8520f0098930a754748b7ddcb43ef75a0dbf3a0d26381af4eba4a98eaa9b4e6a"))
	{
		return false;
	}
	::std::byte ask2[32];
	for (::std::size_t i{}; i != 32; ++i)
	{
		ask2[i] = ask[i];
	}
	x::x25519::calculate_public_key_to_ptr(bpk, bsk);
	x::x25519::create_shared_key(
		::fast_io::containers::index_span<::std::byte, 32>{::fast_io::containers::index_unchecked, sa},
		::fast_io::containers::index_span<::std::byte const, 32>{::fast_io::containers::index_unchecked, bpk},
		::fast_io::containers::index_span<::std::byte, 32>{::fast_io::containers::index_unchecked, ask2});
	if (!bytes_eq(sa, u8"4a5d9d5ba4ce2de1728e3bf480350f25e07e21c947d19e3376f09b3c1e161742"))
	{
		return false;
	}

	/* the ed25519 base-table fast path gives the same public key */
	hex2bytes(bsk, u8"5dab087e624a8a4b79e17f8b83800ee66f3bb1292618b6fd1c2f8b27ff88e0eb");
	x::x25519::calculate_public_key_fast_to_ptr(bpk, bsk);
	if (!bytes_eq(bpk, u8"de9edb7d7b7dc1b4d35b61c2ece435373f8343c85b78674dadfc7e146f882b4f"))
	{
		return false;
	}
	hex2bytes(bsk, u8"5dab087e624a8a4b79e17f8b83800ee66f3bb1292618b6fd1c2f8b27ff88e0eb");
	x::x25519::calculate_public_key_fast(
		::fast_io::containers::index_span<::std::byte, 32>{::fast_io::containers::index_unchecked, bpk},
		::fast_io::containers::index_span<::std::byte, 32>{::fast_io::containers::index_unchecked, bsk});
	return bytes_eq(bpk, u8"de9edb7d7b7dc1b4d35b61c2ece435373f8343c85b78674dadfc7e146f882b4f");
}

static_assert(test_x25519_dh());

} // namespace

int main()
{
	bool const ok{test_x25519_dh()};
	::fast_io::print("x25519 DH: ", ok, "\n");
	return !ok;
}
