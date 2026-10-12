#include <fast_io.h>
#include <fast_io_crypto.h>

/*
The compile-time checks evaluate whole scalar multiplications; raise the
constexpr step budget when building:  -fconstexpr-steps=100000000
*/

/*
Blinding-context tests:
  - a context derived from a seed produces keypair/sign output identical
    to the unblinded path (blinding must not change results)
  - blinding_init from a byte span and from an ibuffer_view stream agree
  - get_default_blinding() works as a ready-made context
*/

namespace
{

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

constexpr bool test_blinded_equivalence() noexcept
{
	::std::byte sk[32]{}, seed[64]{}, msg[8]{};
	seed[0] = ::std::byte{0x42};
	::fast_io::ed25519::blinding_context ctx;
	::fast_io::ed25519::blinding_init(ctx, ::fast_io::containers::span<::std::byte const>{seed, seed + 64});

	::std::byte pk1[32], pk2[32], priv1[64], priv2[64], sig1[64], sig2[64];
	::fast_io::ed25519::create_key_pair_to_ptr(pk1, priv1, sk);
	::fast_io::ed25519::create_key_pair_with_blinding_to_ptr(pk2, priv2, sk, ctx);
	if (!bytes_eq(pk1, pk2, 32) || !bytes_eq(priv1, priv2, 64))
	{
		return false;
	}
	::fast_io::ed25519::sign_message_to_ptr(sig1, priv1, msg, 8);
	::fast_io::ed25519::sign_message_with_blinding_to_ptr(sig2, priv2, msg, 8, ctx);
	if (!bytes_eq(sig1, sig2, 64))
	{
		return false;
	}
	return ::fast_io::ed25519::verify_signature_to_ptr(sig2, pk2, msg, 8);
}

static_assert(test_blinded_equivalence());

constexpr bool test_default_blinding() noexcept
{
	::std::byte sk[32]{};
	::fast_io::ed25519::blinding_context ctx{::fast_io::ed25519::get_default_blinding()};
	::std::byte pk1[32], pk2[32], priv1[64], priv2[64];
	::fast_io::ed25519::create_key_pair_to_ptr(pk1, priv1, sk);
	::fast_io::ed25519::create_key_pair_with_blinding_to_ptr(pk2, priv2, sk, ctx);
	return bytes_eq(pk1, pk2, 32) && bytes_eq(priv1, priv2, 64);
}

static_assert(test_default_blinding());

bool test_stream_init() noexcept
{
	::std::byte seed[64]{};
	seed[0] = ::std::byte{0x7f};
	::fast_io::ibuffer_view view{
		reinterpret_cast<char const *>(seed),
		reinterpret_cast<char const *>(seed + 64)};
	::fast_io::ed25519::blinding_context ctx;
	::fast_io::ed25519::blinding_init_from_input_stream(ctx, view);

	::fast_io::ed25519::blinding_context ref;
	::fast_io::ed25519::blinding_init_to_ptr(ref, seed, 64);
	return bytes_eq(reinterpret_cast<::std::byte const *>(__builtin_addressof(ctx)),
					reinterpret_cast<::std::byte const *>(__builtin_addressof(ref)),
					sizeof(ctx));
}

} // namespace

int main()
{
	bool const ok{test_blinded_equivalence()};
	bool const def_ok{test_default_blinding()};
	bool const stream_ok{test_stream_init()};
	::fast_io::print("ed25519 blinded == unblinded: ", ok,
					 "\ndefault blinding: ", def_ok,
					 "\nstream blinding_init: ", stream_ok, "\n");
	return !(ok && def_ok && stream_ok);
}
