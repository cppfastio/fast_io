#include <fast_io.h>
#include <fast_io_dsal/vector.h>

struct not_a_char_t
{
	char content;
	not_a_char_t(char c) noexcept : content(c) {}
	not_a_char_t(std::uint8_t c) noexcept : content(static_cast<char>(c)) {}
};

extern "C" int LLVMFuzzerTestOneInput(std::uint8_t* data, std::size_t size)
{
	constexpr auto forloop_size = 16u;
	fast_io::vector<char> vec_char;
	fast_io::vector<not_a_char_t> vec_not_char;
	vec_char.clear();
	vec_char.shrink_to_fit();
	vec_not_char.clear();
	vec_not_char.shrink_to_fit();
	for (; size > 2 * forloop_size; size -= 2 * forloop_size)
	{
		for (std::size_t i{}; i < forloop_size; ++i)
		{
			vec_char.emplace_back(*data++);
		}
		for (std::size_t i{}; i < forloop_size; ++i)
		{
			vec_not_char.emplace_back(*data++);
		}
	}
	return 0;
}