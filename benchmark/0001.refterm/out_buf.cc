#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/timer.h>

inline void test()
{
	fast_io::out_buf_type obf{fast_io::out()};
	for(std::size_t i{};i!=1000000;++i)
		println(obf,i);
}
//STDOUT_HANDLE 
int main()
{
	fast_io::u8obuf_file timer_obf(u"out_buf.txt");
	auto t0{posix_clock_gettime(fast_io::posix_clock_id::monotonic)};
	test();
	print(timer_obf,posix_clock_gettime(fast_io::posix_clock_id::monotonic)-t0,u8"s\n");
}
