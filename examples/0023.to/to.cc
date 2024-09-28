#include <cstdint>
#include <fast_io.h>

using namespace fast_io::io;

int main()
{
#ifdef __SIZEOF_INT128__
	constexpr __uint128_t s{fast_io::to<std::uint_least64_t>("   2424", fast_io::mnp::left(0, 30, '0'),
													 " sgdjasdopgjdasgpojadsgpoadjsgpoadsjgopjaghdsoighasdioghadsoi")};
	println(s);
#endif
}
