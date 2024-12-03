#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_driver/timer.h>
using namespace fast_io::io;

inline void test()
{
	for (std::size_t i{}; i != 1000000; ++i)
	{
		println(i);
	}
}

int main()
{
	fast_io::u8obuf_file timer_obf(u"stdio_hack.txt");
	fast_io::timer t(u8"stdio_hack");
	test();
}
