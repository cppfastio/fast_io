// libFuzzer target for fast_io's curve25519 (ed25519/x25519).
//
// Build (requires clang with -fsanitize=fuzzer):
//   clang++ -std=c++2c -O1 -g -fsanitize=fuzzer,address,undefined \
//       -Iinclude -o fuzz_curve25519 fuzzing/0010.curve25519/fuzz_curve25519.cc
// With herbceptions:
//   clang++ --config=$HOME/herbcfgs/x86_64-linux-gnu-libcxx.cfg -std=c++2c -O1 -g \
//       -fsanitize=fuzzer,address,undefined -fherbceptions -fno-exceptions \
//       -Iinclude -o fuzz_curve25519 fuzzing/0010.curve25519/fuzz_curve25519.cc \
//       -fsanitize=fuzzer,address,undefined -lherbceptions
//
// data[0] selects which API surface to exercise; the rest is payload.
// Invariants asserted via __builtin_trap (reported as crashes):
//   - sign/verify roundtrip always verifies
//   - blinded keypair/sign output is identical to unblinded
//   - x25519 DH shared secrets agree
//   - x25519 custom-zr paths match the plain ladder
// Fully-fuzzed verify/shared-key calls must simply never crash.

#include <fast_io.h>
#include <fast_io_crypto.h>

namespace
{

[[noreturn]] inline void fail() noexcept
{
	__builtin_trap();
}

inline bool bytes_eq(::std::byte const *a, ::std::byte const *b, ::std::size_t n) noexcept
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

/* mode 0: keypair -> sign -> verify must roundtrip */
void fuzz_ed25519_roundtrip(::std::byte const *p, ::std::size_t n) noexcept
{
	if (n < 32)
	{
		return;
	}
	::std::byte pk[32], priv[64], sig[64];
	::fast_io::ed25519::create_key_pair_to_ptr(pk, priv, p);
	::fast_io::ed25519::sign_message_to_ptr(sig, priv, p + 32, n - 32);
	if (!::fast_io::ed25519::verify_signature_to_ptr(sig, pk, p + 32, n - 32))
	{
		fail();
	}
	/* exercise the reject path too; result deliberately unchecked */
	if (n > 32)
	{
		::std::byte badsig[64];
		auto const t{::std::to_integer<unsigned>(p[32])};
		::fast_io::freestanding::non_overlapped_copy_n(sig, 64, badsig);
		badsig[t & 63] ^= static_cast<::std::byte>(1u << (t & 7));
		(void)::fast_io::ed25519::verify_signature_to_ptr(badsig, pk, p + 32, n - 32);
	}
}

/* mode 1: fully fuzzed verify must never crash */
void fuzz_ed25519_verify(::std::byte const *p, ::std::size_t n) noexcept
{
	if (n < 96)
	{
		return;
	}
	(void)::fast_io::ed25519::verify_signature_to_ptr(p, p + 64, p + 96, n - 96);
}

/* mode 2: blinded keypair/sign must equal unblinded */
void fuzz_ed25519_blinding(::std::byte const *p, ::std::size_t n) noexcept
{
	if (n < 96)
	{
		return;
	}
	::fast_io::ed25519::blinding_context ctx;
	::fast_io::ed25519::blinding_init_to_ptr(ctx, p, 64);
	::std::byte pk1[32], pk2[32], priv1[64], priv2[64], sig1[64], sig2[64];
	::fast_io::ed25519::create_key_pair_to_ptr(pk1, priv1, p + 64);
	::fast_io::ed25519::create_key_pair_with_blinding_to_ptr(pk2, priv2, p + 64, ctx);
	if (!bytes_eq(pk1, pk2, 32) || !bytes_eq(priv1, priv2, 64))
	{
		fail();
	}
	::fast_io::ed25519::sign_message_to_ptr(sig1, priv1, p + 96, n - 96);
	::fast_io::ed25519::sign_message_with_blinding_to_ptr(sig2, priv2, p + 96, n - 96, ctx);
	if (!bytes_eq(sig1, sig2, 64))
	{
		fail();
	}
	if (!::fast_io::ed25519::verify_signature_to_ptr(sig2, pk2, p + 96, n - 96))
	{
		fail();
	}
}

/* mode 3: x25519 DH agreement for fuzzed keypairs */
void fuzz_x25519_dh(::std::byte const *p, ::std::size_t n) noexcept
{
	if (n < 64)
	{
		return;
	}
	::std::byte ask[32], bsk[32], apk[32], bpk[32], sa[32], sb[32];
	::fast_io::freestanding::non_overlapped_copy_n(p, 32, ask);
	::fast_io::freestanding::non_overlapped_copy_n(p + 32, 32, bsk);
	::fast_io::diffie_hellman::x25519::calculate_public_key_to_ptr(apk, ask);
	::fast_io::diffie_hellman::x25519::calculate_public_key_to_ptr(bpk, bsk);
	::fast_io::diffie_hellman::x25519::create_shared_key_to_ptr(sa, bpk, ask);
	::fast_io::diffie_hellman::x25519::create_shared_key_to_ptr(sb, apk, bsk);
	if (!bytes_eq(sa, sb, 32))
	{
		fail();
	}
}

/* mode 4: fuzzed peer public key; custom zr must match plain ladder */
void fuzz_x25519_peer(::std::byte const *p, ::std::size_t n) noexcept
{
	if (n < 96)
	{
		return;
	}
	::fast_io::curve25519::field_number zr;
	::fast_io::freestanding::type_punning_from_bytes(p, zr);
	::std::byte sk1[32], sk2[32], s1[32], s2[32];
	::fast_io::freestanding::non_overlapped_copy_n(p + 64, 32, sk1);
	::fast_io::freestanding::non_overlapped_copy_n(p + 64, 32, sk2);
	::fast_io::diffie_hellman::x25519::create_shared_key_to_ptr(s1, p + 32, sk1);
	::fast_io::diffie_hellman::x25519::create_shared_key_to_ptr_with_zr(s2, p + 32, sk2, zr);
	if (!bytes_eq(s1, s2, 32))
	{
		fail();
	}
}

/* mode 5: blinded fast pubkey must match ladder pubkey */
void fuzz_x25519_fast(::std::byte const *p, ::std::size_t n) noexcept
{
	if (n < 96)
	{
		return;
	}
	::fast_io::ed25519::blinding_context ctx;
	::fast_io::ed25519::blinding_init_to_ptr(ctx, p, 64);
	::std::byte sk1[32], sk2[32], pk1[32], pk2[32];
	::fast_io::freestanding::non_overlapped_copy_n(p + 64, 32, sk1);
	::fast_io::freestanding::non_overlapped_copy_n(p + 64, 32, sk2);
	::fast_io::diffie_hellman::x25519::calculate_public_key_to_ptr(pk1, sk1);
	::fast_io::diffie_hellman::x25519::calculate_public_key_fast_to_ptr_with_blinding(pk2, sk2, ctx);
	if (!bytes_eq(pk1, pk2, 32))
	{
		fail();
	}
}

/*
mode 6: stream-based blinding init over ibuffer_view.
Short streams must throw parse_errc::end_of_file (herbceptions builds);
without herbceptions a short stream would terminate, so guard the size.
*/
void fuzz_blinding_stream(::std::byte const *p, ::std::size_t n)
{
#if defined(__HERBCEPTIONS__)
	::fast_io::ibuffer_view view{
		reinterpret_cast<char const *>(p),
		reinterpret_cast<char const *>(p) + n};
	::fast_io::ed25519::blinding_context ctx;
	try
	{
		::fast_io::ed25519::blinding_init_from_input_stream(ctx, view);
	}
	catch throws(::std::error e)
	{
		if (e == ::std::parse_errc::end_of_file)
		{
			return;
		}
		fail();
	}
#else
	if (n < 64)
	{
		return;
	}
	::fast_io::ibuffer_view view{
		reinterpret_cast<char const *>(p),
		reinterpret_cast<char const *>(p) + n};
	::fast_io::ed25519::blinding_context ctx;
	::fast_io::ed25519::blinding_init_from_input_stream(ctx, view);
#endif
	/* context from the first 64 stream bytes must match buffer init */
	::fast_io::ed25519::blinding_context ref;
	::fast_io::ed25519::blinding_init_to_ptr(ref, p, 64);
	if (!bytes_eq(reinterpret_cast<::std::byte const *>(__builtin_addressof(ctx)),
				  reinterpret_cast<::std::byte const *>(__builtin_addressof(ref)), sizeof(ctx)))
	{
		fail();
	}
}

/* mode 7: variable-length seed through the SHA512 init path */
void fuzz_blinding_seed(::std::byte const *p, ::std::size_t n) noexcept
{
	::fast_io::ed25519::blinding_context ctx;
	::fast_io::ed25519::blinding_init_to_ptr(ctx, p, n);
	/* bp must still satisfy ctx.bp == -ctx.bl*B, i.e. using it must
	   produce identical output to the unblinded path */
	::std::byte seed[32]{};
	::fast_io::freestanding::non_overlapped_copy_n(p, n < 32 ? n : 32, seed);
	::std::byte pk1[32], pk2[32], priv1[64], priv2[64];
	::fast_io::ed25519::create_key_pair_to_ptr(pk1, priv1, seed);
	::fast_io::ed25519::create_key_pair_with_blinding_to_ptr(pk2, priv2, seed, ctx);
	if (!bytes_eq(pk1, pk2, 32) || !bytes_eq(priv1, priv2, 64))
	{
		fail();
	}
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(::std::uint8_t const *data, ::std::size_t size)
{
	if (size == 0)
	{
		return 0;
	}
	auto const *p{reinterpret_cast<::std::byte const *>(data)};
	::std::size_t const n{size - 1};
	switch (::std::to_integer<unsigned>(p[0]) & 7)
	{
	case 0:
		fuzz_ed25519_roundtrip(p + 1, n);
		break;
	case 1:
		fuzz_ed25519_verify(p + 1, n);
		break;
	case 2:
		fuzz_ed25519_blinding(p + 1, n);
		break;
	case 3:
		fuzz_x25519_dh(p + 1, n);
		break;
	case 4:
		fuzz_x25519_peer(p + 1, n);
		break;
	case 5:
		fuzz_x25519_fast(p + 1, n);
		break;
	case 6:
		fuzz_blinding_stream(p + 1, n);
		break;
	case 7:
		fuzz_blinding_seed(p + 1, n);
		break;
	}
	return 0;
}
