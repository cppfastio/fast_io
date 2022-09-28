#include<array>
#include<string_view>
#include<fast_io.h>
#include<fast_io_crypto.h>

constexpr auto evaluate_sha512() noexcept
{
	using fast_io::mnp::hash_digest;
	std::array<char8_t,print_reserve_size(fast_io::io_reserve_type<char8_t,decltype(hash_digest(fast_io::sha512_context{}))>)> array{};
	fast_io::sha512_context ctx;
	ctx.do_final();
	print_reserve_define(fast_io::io_reserve_type<char8_t,decltype(hash_digest(fast_io::sha512_context{}))>,array.data(),hash_digest(ctx));
	return array;
}

//constexpr auto evaluate_hmac_sha256() noexcept
//{
//	using T = fast_io::hmac_sha256;
//	std::array<char8_t,print_reserve_size(fast_io::io_reserve_type<char8_t,T>)> array{};
//	T sha("key");
//	fast_io::hash_processor processor{sha};
//	print_freestanding(processor,"The quick brown fox jumps over the lazy dog");
//	processor.do_final();
//	print_reserve_define(fast_io::io_reserve_type<char8_t,T>,array.data(),sha);
//	return array;
//}

//constexpr auto evaluate_hmac_sha512() noexcept
//{
//	using T = fast_io::hmac_sha512;
//	std::array<char8_t,print_reserve_size(fast_io::io_reserve_type<char8_t,T>)> array{};
//	T sha("key");
//	fast_io::hash_processor processor{sha};
//	print_freestanding(processor,"The quick brown fox jumps over the lazy dog");
//	processor.do_final();
//	print_reserve_define(fast_io::io_reserve_type<char8_t,T>,array.data(),sha);
//	return array;
//}

int main()
{
	using namespace std::string_view_literals;

	constexpr auto sha512{evaluate_sha512()};
	static_assert(std::u8string_view(sha512.data(),sha512.size())==u8"bdb8ef7e35e183cf07806dd6502854f1dc15570b05e420d6cee96cd321a9f483b0f2855d3cd1d0472fec7e87d21883ff817a4147bd31b9633eda27f97a3238a5"sv);

	////https://en.wikipedia.org/wiki/HMAC
	//constexpr auto hmac_sha_256_value{evaluate_hmac_sha256()};
	//static_assert(std::u8string_view(hmac_sha_256_value.data(),hmac_sha_256_value.size())==u8"f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8"sv);

	//constexpr auto hmac_sha_512_value{evaluate_hmac_sha512()};
	//static_assert(std::u8string_view(hmac_sha_512_value.data(),hmac_sha_512_value.size())==u8"b42af09057bac1e2d41708e48a902e09b5ff7f12ab428a4fe86653c73dd248fb82f948a549f7b791a5b41915ee4d1ec3935357e4e2317250d0372afa2ebeeb3a"sv);
}