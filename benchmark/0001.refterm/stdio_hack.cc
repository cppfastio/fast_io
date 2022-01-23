#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/timer.h>
inline void test()
{
	for(std::size_t i{};i!=1000000;++i)
		println(i);
}

int main()
{
	fast_io::u8obuf_file timer_obf(u"stdio_hack.txt");
	auto t0{posix_clock_gettime(fast_io::posix_clock_id::monotonic)};
	test();
	print(timer_obf,posix_clock_gettime(fast_io::posix_clock_id::monotonic)-t0,u8"s\n");
}
