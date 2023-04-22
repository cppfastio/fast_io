#include <string>
#include <fast_io.h>

using namespace fast_io::io;

int main()
{
	std::string s{ "2022-12-01T01:60:57.1928303948203949323422366Z" };
	try
	{
		auto tsp = fast_io::to<fast_io::iso8601_timestamp>(s);
	}
	catch (fast_io::error e)
	{
		print(static_cast<fast_io::parse_code>(e.code));
	}
}