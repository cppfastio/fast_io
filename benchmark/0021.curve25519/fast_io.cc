/*
Benchmark: fast_io curve25519 (ed25519 + x25519).

Build (from this directory):
  clang++ -std=c++2c -O2 -I../../include -I. -o build/fast_io fast_io.cc
*/

#include <fast_io.h>
#include <fast_io_crypto.h>
#include "bench_common.h"

int main()
{
	/* RFC 8032 TEST 1 */
	::std::byte sk[32], pk[32], priv[64], sig[64];
	unsigned char msk[32];
	hex2bytes(reinterpret_cast<unsigned char *>(sk), rfc8032_sk());
	hex2bytes(msk, rfc8032_sk());
	::std::byte msg[64]{};

	/* sanity: RFC 8032 outputs */
	::fast_io::ed25519::create_key_pair_to_ptr(pk, priv, sk);
	::std::byte expected_pk[32];
	hex2bytes(reinterpret_cast<unsigned char *>(expected_pk), rfc8032_pk());
	if (!bytes_eq(reinterpret_cast<unsigned char const *>(pk),
				  reinterpret_cast<unsigned char const *>(expected_pk), 32))
	{
		::fast_io::print("!! pubkey mismatch\n");
		return 1;
	}
	::fast_io::ed25519::sign_message_to_ptr(sig, priv, msg, 64);
	if (!::fast_io::ed25519::verify_signature_to_ptr(sig, pk, msg, 64))
	{
		::fast_io::print("!! verify failed\n");
		return 1;
	}
	::fast_io::print("sanity: fast_io outputs ok\n\n");

	::std::uint64_t c;
	c = bench([&] { ::fast_io::ed25519::create_key_pair_to_ptr(pk, priv, sk); }, bench_iters);
	::fast_io::print("ed25519 keypair   ours : ", c, ::fast_io::mnp::os_c_str(bench_unit()), "\n");
	c = bench([&] { ::fast_io::ed25519::sign_message_to_ptr(sig, priv, msg, 64); }, bench_iters);
	::fast_io::print("ed25519 sign      ours : ", c, ::fast_io::mnp::os_c_str(bench_unit()), "\n");
	c = bench([&] { ::fast_io::ed25519::verify_signature_to_ptr(sig, pk, msg, 64); }, bench_iters);
	::fast_io::print("ed25519 verify    ours : ", c, ::fast_io::mnp::os_c_str(bench_unit()), "\n");

	/* X25519 — RFC 7748 alice */
	::std::byte dhsk[32], dhpk[32], shared[32];
	hex2bytes(reinterpret_cast<unsigned char *>(dhsk), rfc7748_alice_sk());
	c = bench([&]
			  {
				  ::std::byte t[32];
				  ::fast_io::freestanding::nonoverlapped_bytes_copy_n(dhsk, 32, t);
				  ::fast_io::diffie_hellman::x25519::calculate_public_key_to_ptr(dhpk, t);
			  },
			  bench_iters);
	::fast_io::print("x25519 pubkey     ours : ", c, ::fast_io::mnp::os_c_str(bench_unit()), "\n");
	c = bench([&]
			  {
				  ::std::byte t[32];
				  ::fast_io::freestanding::nonoverlapped_bytes_copy_n(dhsk, 32, t);
				  ::fast_io::diffie_hellman::x25519::calculate_public_key_fast_to_ptr(dhpk, t);
			  },
			  bench_iters);
	::fast_io::print("x25519 pub (fast) ours : ", c, ::fast_io::mnp::os_c_str(bench_unit()), "\n");
	c = bench([&]
			  {
				  ::std::byte t[32];
				  ::fast_io::freestanding::nonoverlapped_bytes_copy_n(dhsk, 32, t);
				  ::fast_io::diffie_hellman::x25519::create_shared_key_to_ptr(shared, dhpk, t);
			  },
			  bench_iters);
	::fast_io::print("x25519 shared     ours : ", c, ::fast_io::mnp::os_c_str(bench_unit()), "\n");
	return 0;
}
