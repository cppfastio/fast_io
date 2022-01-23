
#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/timer.h>

int main()
{
	constexpr std::size_t buffer_size{10000};
	constexpr std::size_t N(10000000);
	auto iso8601_ts{utc(fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime))};
	{
	fast_io::timer timer(u8"fast_io");
	fast_io::obuf_file obf(u8"iso8601.txt");
	for(std::size_t i{};i!=N;++i)
	{
		println(obf,iso8601_ts);
	}
	}
}