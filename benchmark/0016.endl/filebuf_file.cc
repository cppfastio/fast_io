#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_legacy.h>
#include<fast_io_driver/timer.h>
using namespace fast_io::io;

int main()
{
	fast_io::timer tm(u8"u8filebuf_file");
	fast_io::filebuf_file fbf(u8"filebuf_file.txt",fast_io::open_mode::out);
	for(std::size_t i{};i!=10000000;++i)
		print(fbf,"Hello World\n");
}
