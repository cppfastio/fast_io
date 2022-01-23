#include<fast_io.h>

int main()
{
	fast_io::posix_tzset();
	fast_io::unix_timestamp tsp{fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime)};
	println("Unix Timestamp:",tsp,"\n"
		"utc:",utc(tsp),"\n"
		"local:",local(tsp),"\n"
		"timezone:",fast_io::timezone_name());
}
