/*
Benchmark: Crypto++ ed25519/x25519 (libcryptopp.a).

Requires a built cryptopp checkout as a sibling repo. Manual example:

  clang++ -std=c++2c -O2 -I../../include -I. -I../../../cryptopp \
      -o build/cryptopp cryptopp.cc ../../../cryptopp/libcryptopp.a -lpthread
*/

#include <fast_io.h>
#include <xed25519.h>
#include <algparam.h>
#include <argnames.h>
#include "bench_common.h"

int main()
{
	unsigned char msk[32], epk[32], msg[64]{};
	hex2bytes(msk, rfc8032_sk());
	hex2bytes(epk, rfc8032_pk());

	CryptoPP::ed25519Signer csigner(msk);
	CryptoPP::ed25519Verifier cverifier(epk);
	CryptoPP::byte csig[64];
	csigner.SignMessage(CryptoPP::NullRNG(), msg, 64, csig);
	bool cv = cverifier.VerifyMessage(msg, 64, csig, 64);
	CryptoPP::byte const *cpk{static_cast<CryptoPP::ed25519PrivateKey const &>(csigner.GetPrivateKey()).GetPublicKeyBytePtr()};
	if (!bytes_eq(cpk, epk, 32) || !cv)
	{
		::fast_io::print("!! cryptopp sanity failed\n");
		return 1;
	}
	::fast_io::print("sanity: cryptopp outputs ok\n\n");

	::std::uint64_t c;
	c = bench([&]
			  {
				  CryptoPP::ed25519Signer s(msk);
				  (void)static_cast<CryptoPP::ed25519PrivateKey const &>(s.GetPrivateKey()).GetPublicKeyBytePtr()[0];
			  },
			  bench_iters);
	::fast_io::print("ed25519 keypair   c++pp: ", c, " cycles\n");
	c = bench([&] { csigner.SignMessage(CryptoPP::NullRNG(), msg, 64, csig); }, bench_iters);
	::fast_io::print("ed25519 sign      c++pp: ", c, " cycles\n");
	c = bench([&] { (void)cverifier.VerifyMessage(msg, 64, csig, 64); }, bench_iters);
	::fast_io::print("ed25519 verify    c++pp: ", c, " cycles\n");

	unsigned char dhsk[32], dhpk[32], shared[32];
	hex2bytes(dhsk, rfc7748_alice_sk());
	hex2bytes(dhpk, rfc7748_alice_pk());
	c = bench([&]
			  {
				  CryptoPP::x25519 k(dhsk);
				  CryptoPP::ConstByteArrayParameter p;
				  k.GetVoidValue(CryptoPP::Name::PublicElement(), typeid(CryptoPP::ConstByteArrayParameter), &p);
				  (void)p.begin()[0];
			  },
			  bench_iters);
	::fast_io::print("x25519 pubkey     c++pp: ", c, " cycles\n");
	CryptoPP::x25519 cx;
	c = bench([&] { (void)cx.Agree(shared, dhsk, dhpk); }, bench_iters);
	::fast_io::print("x25519 shared     c++pp: ", c, " cycles\n");
	return 0;
}
