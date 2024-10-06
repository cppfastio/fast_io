#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_driver/timer.h>
using namespace fast_io::io;

inline void test()
{
	fast_io::out_buf_type obf{fast_io::out()};
	for (std::size_t i{}; i != 1000000; ++i)
	{
		println(obf, i);
	}
}
// STDOUT_HANDLE
int main()
{
	fast_io::u8obuf_file timer_obf(u"out_buf.txt");
	fast_io::timer t(u8"out_buf");
	test();
}
