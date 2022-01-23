#include<fast_io.h>
#include<fast_io_crypto.h>

constexpr auto evaluate_sha512() noexcept
{
	std::array<char8_t,print_reserve_size(fast_io::io_reserve_type<char8_t,fast_io::sha512>)> array{};
	fast_io::sha512 sha;
	fast_io::hash_processor processor{sha};
	processor.do_final();
	print_reserve_define(fast_io::io_reserve_type<char8_t,fast_io::sha512>,array.data(),sha);
	return array;
}

constexpr auto evaluate_hmac_sha256() noexcept
{
	using T = fast_io::hmac_sha256;
	std::array<char8_t,print_reserve_size(fast_io::io_reserve_type<char8_t,T>)> array{};
	T sha("key");
	fast_io::hash_processor processor{sha};
	print_freestanding(processor,"The quick brown fox jumps over the lazy dog");
	processor.do_final();
	print_reserve_define(fast_io::io_reserve_type<char8_t,T>,array.data(),sha);
	return array;
}

constexpr auto evaluate_hmac_sha512() noexcept
{
	using T = fast_io::hmac_sha512;
	std::array<char8_t,print_reserve_size(fast_io::io_reserve_type<char8_t,T>)> array{};
	T sha("key");
	fast_io::hash_processor processor{sha};
	print_freestanding(processor,"The quick brown fox jumps over the lazy dog");
	processor.do_final();
	print_reserve_define(fast_io::io_reserve_type<char8_t,T>,array.data(),sha);
	return array;
}

int main()
{
	using namespace std::string_view_literals;

	constexpr auto sha512{evaluate_sha512()};
	static_assert(std::u8string_view(sha512.data(),sha512.size())==u8"cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e"sv);
	
//https://en.wikipedia.org/wiki/HMAC
	constexpr auto hmac_sha_256_value{evaluate_hmac_sha256()};
	static_assert(std::u8string_view(hmac_sha_256_value.data(),hmac_sha_256_value.size())==u8"f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8"sv);

	constexpr auto hmac_sha_512_value{evaluate_hmac_sha512()};
	static_assert(std::u8string_view(hmac_sha_512_value.data(),hmac_sha_512_value.size())==u8"b42af09057bac1e2d41708e48a902e09b5ff7f12ab428a4fe86653c73dd248fb82f948a549f7b791a5b41915ee4d1ec3935357e4e2317250d0372afa2ebeeb3a"sv);
}