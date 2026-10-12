/*
Benchmark: OpenSSL EVP ed25519/x25519.

Build:  clang++ -std=c++2c -O2 -I../../include -I. -o build/openssl openssl.cc -lcrypto
*/

#include <fast_io.h>
#include <openssl/evp.h>
#include "bench_common.h"

int main()
{
	unsigned char msk[32], epk[32], osig[64], msg[64]{};
	hex2bytes(msk, rfc8032_sk());
	hex2bytes(epk, rfc8032_pk());

	EVP_PKEY *osk = EVP_PKEY_new_raw_private_key(EVP_PKEY_ED25519, nullptr, msk, 32);
	EVP_PKEY *opk = EVP_PKEY_new_raw_public_key(EVP_PKEY_ED25519, nullptr, epk, 32);

	/* sign once + verify for sanity */
	{
		EVP_MD_CTX *ctx = EVP_MD_CTX_new();
		::std::size_t sl{64};
		EVP_DigestSignInit(ctx, nullptr, nullptr, nullptr, osk);
		EVP_DigestSign(ctx, osig, &sl, msg, 64);
		EVP_MD_CTX_free(ctx);
		ctx = EVP_MD_CTX_new();
		EVP_DigestVerifyInit(ctx, nullptr, nullptr, nullptr, opk);
		int rv{EVP_DigestVerify(ctx, osig, 64, msg, 64)};
		EVP_MD_CTX_free(ctx);
		if (rv != 1)
		{
			::fast_io::print("!! openssl sanity failed\n");
			return 1;
		}
	}
	::fast_io::print("sanity: openssl outputs ok\n\n");

	::std::uint64_t c;
	c = bench([&]
			  {
				  EVP_PKEY *k = EVP_PKEY_new_raw_private_key(EVP_PKEY_ED25519, nullptr, msk, 32);
				  unsigned char b[32];
				  ::std::size_t l{32};
				  EVP_PKEY_get_raw_public_key(k, b, &l);
				  EVP_PKEY_free(k);
			  },
			  bench_iters);
	::fast_io::print("ed25519 keypair   ossl : ", c, " cycles\n");
	c = bench([&]
			  {
				  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
				  ::std::size_t sl{64};
				  EVP_DigestSignInit(ctx, nullptr, nullptr, nullptr, osk);
				  EVP_DigestSign(ctx, osig, &sl, msg, 64);
				  EVP_MD_CTX_free(ctx);
			  },
			  bench_iters);
	::fast_io::print("ed25519 sign      ossl : ", c, " cycles\n");
	c = bench([&]
			  {
				  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
				  EVP_DigestVerifyInit(ctx, nullptr, nullptr, nullptr, opk);
				  EVP_DigestVerify(ctx, osig, 64, msg, 64);
				  EVP_MD_CTX_free(ctx);
			  },
			  bench_iters);
	::fast_io::print("ed25519 verify    ossl : ", c, " cycles\n");

	unsigned char dhsk[32], dhpk[32];
	hex2bytes(dhsk, rfc7748_alice_sk());
	hex2bytes(dhpk, rfc7748_alice_pk());
	EVP_PKEY *xsk = EVP_PKEY_new_raw_private_key(EVP_PKEY_X25519, nullptr, dhsk, 32);
	EVP_PKEY *xpk = EVP_PKEY_new_raw_public_key(EVP_PKEY_X25519, nullptr, dhpk, 32);
	c = bench([&]
			  {
				  EVP_PKEY *k = EVP_PKEY_new_raw_private_key(EVP_PKEY_X25519, nullptr, dhsk, 32);
				  unsigned char xb[32];
				  ::std::size_t xl{32};
				  EVP_PKEY_get_raw_public_key(k, xb, &xl);
				  EVP_PKEY_free(k);
			  },
			  bench_iters);
	::fast_io::print("x25519 pubkey     ossl : ", c, " cycles\n");
	c = bench([&]
			  {
				  EVP_PKEY_CTX *dctx = EVP_PKEY_CTX_new(xsk, nullptr);
				  EVP_PKEY_derive_init(dctx);
				  EVP_PKEY_derive_set_peer(dctx, xpk);
				  unsigned char sh[32];
				  ::std::size_t shl{32};
				  EVP_PKEY_derive(dctx, sh, &shl);
				  EVP_PKEY_CTX_free(dctx);
			  },
			  bench_iters);
	::fast_io::print("x25519 shared     ossl : ", c, " cycles\n");

	EVP_PKEY_free(osk);
	EVP_PKEY_free(opk);
	EVP_PKEY_free(xsk);
	EVP_PKEY_free(xpk);
	return 0;
}
