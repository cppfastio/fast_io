/*
Benchmark: Mehdi Sotoodeh's curve25519 C+asm reference implementation.

Requires the reference repo as a sibling checkout. Build its amd64
asm objects, e.g. (from repo root):

  as --64 --defsym GCC=1 -I ../ed25519/curve25519/source/asm64/amd64.gnu \
     -o build/Add.o ../ed25519/curve25519/source/asm64/amd64.gnu/Add.s   (etc.)
  clang++ -std=c++2c -O2 -I../../include -I. -o build/mehdi mehdi.cc build/Add.o ...
*/

#include <fast_io.h>
#include "bench_common.h"

extern "C" {
	void curve25519_dh_CalculatePublicKey(unsigned char *pk, unsigned char *sk);
	void curve25519_dh_CalculatePublicKey_fast(unsigned char *pk, unsigned char *sk);
	void curve25519_dh_CreateSharedKey(unsigned char *shared, const unsigned char *pk, unsigned char *sk);
	void ed25519_CreateKeyPair(unsigned char *pk, unsigned char *priv, void const *blinding, unsigned char const *sk);
	void ed25519_SignMessage(unsigned char *sig, unsigned char const *priv, void const *blinding, unsigned char const *msg, ::std::size_t msg_size);
	int ed25519_VerifySignature(unsigned char const *sig, unsigned char const *pk, unsigned char const *msg, ::std::size_t msg_size);
}

int main()
{
	unsigned char msk[32], mpk[32], mpriv[64], msig[64], msg[64]{};
	hex2bytes(msk, rfc8032_sk());

	ed25519_CreateKeyPair(mpk, mpriv, nullptr, msk);
	ed25519_SignMessage(msig, mpriv, nullptr, msg, 64);
	unsigned char epk[32];
	hex2bytes(epk, rfc8032_pk());
	int v = ed25519_VerifySignature(msig, mpk, msg, 64);
	if (!bytes_eq(mpk, epk, 32) || v != 1)
	{
		::fast_io::print("!! mehdi sanity failed\n");
		return 1;
	}
	::fast_io::print("sanity: mehdi outputs ok\n\n");

	::std::uint64_t c;
	c = bench([&] { ed25519_CreateKeyPair(mpk, mpriv, nullptr, msk); }, bench_iters);
	::fast_io::print("ed25519 keypair   mehdi: ", c, " cycles\n");
	c = bench([&] { ed25519_SignMessage(msig, mpriv, nullptr, msg, 64); }, bench_iters);
	::fast_io::print("ed25519 sign      mehdi: ", c, " cycles\n");
	c = bench([&] { (void)ed25519_VerifySignature(msig, mpk, msg, 64); }, bench_iters);
	::fast_io::print("ed25519 verify    mehdi: ", c, " cycles\n");

	unsigned char dhsk[32], dhpk[32], shared[32];
	hex2bytes(dhsk, rfc7748_alice_sk());
	c = bench([&]
			  {
				  unsigned char t[32];
				  __builtin_memcpy(t, dhsk, 32);
				  curve25519_dh_CalculatePublicKey(dhpk, t);
			  },
			  bench_iters);
	::fast_io::print("x25519 pubkey     mehdi: ", c, " cycles\n");
	c = bench([&]
			  {
				  unsigned char t[32];
				  __builtin_memcpy(t, dhsk, 32);
				  curve25519_dh_CalculatePublicKey_fast(dhpk, t);
			  },
			  bench_iters);
	::fast_io::print("x25519 pub (fast) mehdi: ", c, " cycles\n");
	c = bench([&]
			  {
				  unsigned char t[32];
				  __builtin_memcpy(t, dhsk, 32);
				  curve25519_dh_CreateSharedKey(shared, dhpk, t);
			  },
			  bench_iters);
	::fast_io::print("x25519 shared     mehdi: ", c, " cycles\n");
	return 0;
}
