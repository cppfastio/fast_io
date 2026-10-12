#include <fast_io.h>

using namespace fast_io::io;

int main()
{
	fast_io::posix_tzset();
	fast_io::posix_statx_timestamp64 tsp{fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime)};
	println("Unix Timestamp:", tsp,
			"\n"
			"utc:",
			utc(tsp),
			"\n"
			"local:",
			local(tsp),
			"\n"
			"timezone:",
			fast_io::timezone_name());
}
