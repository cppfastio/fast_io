#include <fast_io.h>
#include <fast_io_crypto.h>

inline constexpr ::std::uint_least32_t get_crc32c() noexcept
{
	::fast_io::crc32c_context a{};
	decltype(auto) i = "test";
	a.update((::std::byte *)i, (::std::byte *)i + 2);
	return a.digest_value();
}

int main()
{
	constexpr auto crc32c_gen_from_table = get_crc32c();
	if (crc32c_gen_from_table != get_crc32c())
	{
		::fast_io::fast_terminate();
	}
}
