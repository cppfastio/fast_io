#include <fast_io.h>

using namespace fast_io::iomnp;

int main()
try
{
	::std::size_t sum{};
	for (::std::size_t a; scan_some(a); sum += a)
	{
	}
	println(sum);
}
catch throws(std::error e)
{
	perrln(e);
	return 1;
}
