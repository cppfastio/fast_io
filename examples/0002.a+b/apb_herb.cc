#include <fast_io.h>

using namespace fast_io::iomnp;

int main()
try
{
	std::size_t a, b;
#if 0
	print("Please input two integers\n");
#endif
	scan(a, b);
	println(a + b);
}
catch throws(std::error e)
{
	perrln(e);
	return 1;
}
