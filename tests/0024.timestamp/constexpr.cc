#include <fast_io.h>

using namespace fast_io::io;

int main()
{
	constexpr auto tsp = fast_io::to<fast_io::iso8601_timestamp>("2022-12-01T01:23:45.1928303948203949323422366Z");
	println(tsp);
}
