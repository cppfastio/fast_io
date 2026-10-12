// module consumer: curve25519 via import fast_io_crypto
#include <cstddef>
import fast_io_crypto;
import fast_io;

int main()
{
	try
	{
		::fast_io::containers::array<::std::byte, 32> sk{}, pk{};
		sk[0] = ::std::byte{9};
		::fast_io::diffie_hellman::x25519::calculate_public_key_to_ptr(pk.data(), sk.data());

		::fast_io::containers::array<::std::byte, 32> pubk{};
		::fast_io::containers::array<::std::byte, 64> privk{}, sig{};
		::fast_io::ed25519::create_key_pair_to_ptr(pubk.data(), privk.data(), sk.data());
		char msg[] = "hello";
		::fast_io::ed25519::sign_message_to_ptr(sig.data(), privk.data(),
											  reinterpret_cast<::std::byte const *>(msg), 5);
		bool ok = ::fast_io::ed25519::verify_signature_to_ptr(sig.data(), pubk.data(),
															reinterpret_cast<::std::byte const *>(msg), 5);
		::fast_io::io::perrln("ed25519 verify:", ok);
		return !ok;
	}
	catch throws(::std::error e)
	{
		::fast_io::io::perrln("unexpected error: ", e);
		return 1;
	}
}
