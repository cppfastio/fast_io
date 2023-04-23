#include<string>
#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	using namespace fast_io::mnp;
	constexpr std::size_t v{142142};
	auto str{fast_io::u8concat(leb128_put(v))};
	println(rgvw(str,"\t"));

	str=fast_io::u8concat(leb128_put(PTRDIFF_MAX));
	println(rgvw(str,"\t"));

	str=fast_io::u8concat(leb128_put(PTRDIFF_MIN));
	println(rgvw(str,"\t"));
}