#include <string>
#include <limits>
#include <fast_io.h>

using namespace fast_io::io;

int main()
{
	std::uint64_t u1, u2;
	std::int64_t i1, i2;
	std::string buffer(10000, '\0');
	u1 = 14142;
	using namespace fast_io::mnp;
	print(fast_io::obuffer_view{ buffer }, leb128_put(u1));
	scan(fast_io::ibuffer_view{ buffer }, leb128_get(u2));
	println("u2 == ", hex0x(u2), "\t u1 == u2: ", boolalpha(u1 == u2));

	i1 = std::numeric_limits<std::int64_t>::max();
	print(fast_io::obuffer_view{ buffer }, leb128_put(i1));
	scan(fast_io::ibuffer_view{ buffer }, leb128_get(i2));
	println("i2 == ", hex0x(i2), "\t i1 == i2: ", boolalpha(i1 == i2));
	
	i1 = std::numeric_limits<std::int64_t>::min();
	print(fast_io::obuffer_view{ buffer }, leb128_put(i1));
	scan(fast_io::ibuffer_view{ buffer }, leb128_get(i2));
	println("i2 == ", hex0x(i2), "\t i1 == i2: ", boolalpha(i1 == i2));
}
