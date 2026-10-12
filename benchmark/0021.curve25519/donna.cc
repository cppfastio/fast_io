/*
Benchmark: ed25519-donna (floodyberry) reference implementation.

Compile ../ed25519-donna/ed25519.c with its default OpenSSL SHA-512
backend (-lcrypto). Example:

  gcc -O2 -fPIE -I../../../ed25519-donna -c -o build/donna_ed25519.o \
      ../../../ed25519-donna/ed25519.c
  clang++ -std=c++2c -O2 -I../../include -I. -o build/donna donna.cc \
      build/donna_ed25519.o -lcrypto
*/

#include <fast_io.h>
#include "bench_common.h"

extern "C" {
	void ed25519_publickey(unsigned char const sk[32], unsigned char pk[32]);
	void ed25519_sign(unsigned char const *m, ::std::size_t mlen, unsigned char const sk[32], unsigned char const pk[32], unsigned char RS[64]);
	int ed25519_sign_open(unsigned char const *m, ::std::size_t mlen, unsigned char const pk[32], unsigned char const RS[64]);
	void curved25519_scalarmult_basepoint(unsigned char pk[32], unsigned char const e[32]);
}

int main()
{
	unsigned char msk[32], dpk[32], dsig[64], msg[64]{};
	hex2bytes(msk, rfc8032_sk());

	ed25519_publickey(msk, dpk);
	ed25519_sign(msg, 64, msk, dpk, dsig);
	unsigned char epk[32];
	hex2bytes(epk, rfc8032_pk());
	int dv = ed25519_sign_open(msg, 64, dpk, dsig);
	if (!bytes_eq(dpk, epk, 32) || dv != 0)
	{
		::fast_io::print("!! donna sanity failed: sign_open=", dv, "\n");
		return 1;
	}
	::fast_io::print("sanity: donna outputs ok\n\n");

	::std::uint64_t c;
	c = bench([&]
			  {
				  unsigned char dsk[32], o[32];
				  __builtin_memcpy(dsk, msk, 32);
				  ed25519_publickey(dsk, o);
			  },
			  bench_iters);
	::fast_io::print("ed25519 pubkey    donna: ", c, " cycles\n");
	c = bench([&] { ed25519_sign(msg, 64, msk, dpk, dsig); }, bench_iters);
	::fast_io::print("ed25519 sign      donna: ", c, " cycles\n");
	c = bench([&] { (void)ed25519_sign_open(msg, 64, dpk, dsig); }, bench_iters);
	::fast_io::print("ed25519 verify    donna: ", c, " cycles\n");

	unsigned char dhsk[32], dxpk[32];
	hex2bytes(dhsk, rfc7748_alice_sk());
	c = bench([&]
			  {
				  unsigned char dsk[32], o[32];
				  __builtin_memcpy(dsk, dhsk, 32);
				  curved25519_scalarmult_basepoint(o, dsk);
			  },
			  bench_iters);
	::fast_io::print("x25519 pubkey     donna: ", c, " cycles\n");
	(void)dxpk;
	return 0;
}
