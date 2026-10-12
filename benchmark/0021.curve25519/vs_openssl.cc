/*
Benchmark: fast_io curve25519 vs OpenSSL EVP, head-to-head in one process.

Build (from this directory):
  clang++ -std=c++2c -O2 -I../../include -I. -o build/vs_openssl vs_openssl.cc -lcrypto
*/

#include <fast_io.h>
#include <fast_io_crypto.h>
#include <openssl/evp.h>
#include "bench_common.h"

namespace
{

unsigned char msk[32], epk[32], osig[64];
::std::byte sk[32], pk[32], priv[64], sig[64];
::std::byte msg[64]{};
unsigned char omsg[64]{};
EVP_PKEY *osk, *opk, *xsk, *xpk;

unsigned char dhsk[32], dhpk[32];
::std::byte fsk[32], fpk[32], fshared[32];

void sanity()
{
	hex2bytes(msk, rfc8032_sk());
	hex2bytes(epk, rfc8032_pk());
	hex2bytes(dhsk, rfc7748_alice_sk());
	hex2bytes(dhpk, rfc7748_alice_pk());
	hex2bytes(reinterpret_cast<unsigned char *>(sk), rfc8032_sk());
	hex2bytes(reinterpret_cast<unsigned char *>(fsk), rfc7748_alice_sk());
	hex2bytes(reinterpret_cast<unsigned char *>(fpk), rfc7748_alice_pk());

	osk = EVP_PKEY_new_raw_private_key(EVP_PKEY_ED25519, nullptr, msk, 32);
	opk = EVP_PKEY_new_raw_public_key(EVP_PKEY_ED25519, nullptr, epk, 32);
	xsk = EVP_PKEY_new_raw_private_key(EVP_PKEY_X25519, nullptr, dhsk, 32);
	xpk = EVP_PKEY_new_raw_public_key(EVP_PKEY_X25519, nullptr, dhpk, 32);
	if (osk == nullptr || opk == nullptr || xsk == nullptr || xpk == nullptr)
	{
		::fast_io::print("!! EVP_PKEY setup failed\n");
		::std::abort();
	}

	/* openssl self check */
	EVP_MD_CTX *ctx = EVP_MD_CTX_new();
	::std::size_t sl{64};
	EVP_DigestSignInit(ctx, nullptr, nullptr, nullptr, osk);
	EVP_DigestSign(ctx, osig, &sl, omsg, 64);
	EVP_MD_CTX_free(ctx);
	ctx = EVP_MD_CTX_new();
	EVP_DigestVerifyInit(ctx, nullptr, nullptr, nullptr, opk);
	int rv{EVP_DigestVerify(ctx, osig, 64, omsg, 64)};
	EVP_MD_CTX_free(ctx);
	if (rv != 1)
	{
		::fast_io::print("!! openssl sanity failed\n");
		::std::abort();
	}

	/* fast_io: RFC 8032 pk + sig, and cross-check verify feeds openssl sig */
	::fast_io::ed25519::create_key_pair_to_ptr(pk, priv, sk);
	if (!bytes_eq(reinterpret_cast<unsigned char const *>(pk), epk, 32))
	{
		::fast_io::print("!! fast_io pubkey mismatch\n");
		::std::abort();
	}
	::fast_io::ed25519::sign_message_to_ptr(sig, priv, msg, 64);
	if (!::fast_io::ed25519::verify_signature_to_ptr(sig, pk, msg, 64))
	{
		::fast_io::print("!! fast_io verify failed\n");
		::std::abort();
	}
	if (!bytes_eq(reinterpret_cast<unsigned char const *>(sig), osig, 64))
	{
		::fast_io::print("!! signature mismatch vs openssl\n");
		::std::abort();
	}
	::fast_io::print("sanity: fast_io == openssl == rfc8032\n\n");
}

void report(char const *name, ::std::uint64_t ours, ::std::uint64_t ossl)
{
	::std::uint64_t const hundredths{ossl * 100 / ours};
	char const d0{static_cast<char>('0' + (hundredths / 10) % 10)};
	char const d1{static_cast<char>('0' + hundredths % 10)};
	::fast_io::print(::fast_io::mnp::os_c_str(name),
					 " ours : ", ours, ::fast_io::mnp::os_c_str(bench_unit()),
					 "   ossl : ", ossl, ::fast_io::mnp::os_c_str(bench_unit()),
					 "   speedup x", hundredths / 100, ".",
					 ::fast_io::mnp::chvw(d0), ::fast_io::mnp::chvw(d1), "\n");
}

} // namespace

int main()
{
	sanity();

	report("ed25519 keypair  ",
		   bench([&] { ::fast_io::ed25519::create_key_pair_to_ptr(pk, priv, sk); }, bench_iters),
		   bench([&]
				 {
					 EVP_PKEY *k = EVP_PKEY_new_raw_private_key(EVP_PKEY_ED25519, nullptr, msk, 32);
					 unsigned char b[32];
					 ::std::size_t l{32};
					 EVP_PKEY_get_raw_public_key(k, b, &l);
					 EVP_PKEY_free(k);
				 },
				 bench_iters));

	report("ed25519 sign     ",
		   bench([&] { ::fast_io::ed25519::sign_message_to_ptr(sig, priv, msg, 64); }, bench_iters),
		   bench([&]
				 {
					 EVP_MD_CTX *ctx = EVP_MD_CTX_new();
					 ::std::size_t sl{64};
					 EVP_DigestSignInit(ctx, nullptr, nullptr, nullptr, osk);
					 EVP_DigestSign(ctx, osig, &sl, omsg, 64);
					 EVP_MD_CTX_free(ctx);
				 },
				 bench_iters));

	report("ed25519 verify   ",
		   bench([&] { ::fast_io::ed25519::verify_signature_to_ptr(sig, pk, msg, 64); }, bench_iters),
		   bench([&]
				 {
					 EVP_MD_CTX *ctx = EVP_MD_CTX_new();
					 EVP_DigestVerifyInit(ctx, nullptr, nullptr, nullptr, opk);
					 EVP_DigestVerify(ctx, osig, 64, omsg, 64);
					 EVP_MD_CTX_free(ctx);
				 },
				 bench_iters));

	report("x25519 pubkey    ",
		   bench([&]
				 {
					 ::std::byte t[32];
					 ::fast_io::freestanding::nonoverlapped_bytes_copy_n(fsk, 32, t);
					 ::fast_io::diffie_hellman::x25519::calculate_public_key_to_ptr(fpk, t);
				 },
				 bench_iters),
		   bench([&]
				 {
					 EVP_PKEY *k = EVP_PKEY_new_raw_private_key(EVP_PKEY_X25519, nullptr, dhsk, 32);
					 unsigned char xb[32];
					 ::std::size_t xl{32};
					 EVP_PKEY_get_raw_public_key(k, xb, &xl);
					 EVP_PKEY_free(k);
				 },
				 bench_iters));

	report("x25519 shared    ",
		   bench([&]
				 {
					 ::std::byte t[32];
					 ::fast_io::freestanding::nonoverlapped_bytes_copy_n(fsk, 32, t);
					 ::fast_io::diffie_hellman::x25519::create_shared_key_to_ptr(fshared, fpk, t);
				 },
				 bench_iters),
		   bench([&]
				 {
					 EVP_PKEY_CTX *dctx = EVP_PKEY_CTX_new(xsk, nullptr);
					 EVP_PKEY_derive_init(dctx);
					 EVP_PKEY_derive_set_peer(dctx, xpk);
					 unsigned char sh[32];
					 ::std::size_t shl{32};
					 EVP_PKEY_derive(dctx, sh, &shl);
					 EVP_PKEY_CTX_free(dctx);
				 },
				 bench_iters));

	EVP_PKEY_free(osk);
	EVP_PKEY_free(opk);
	EVP_PKEY_free(xsk);
	EVP_PKEY_free(xpk);
	return 0;
}
