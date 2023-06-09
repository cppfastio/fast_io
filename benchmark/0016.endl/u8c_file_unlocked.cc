#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_legacy.h>
#include<fast_io_driver/timer.h>
using namespace fast_io::io;

int main()
{
	fast_io::timer tm(u8"u8c_file_unlocked");
	fast_io::u8c_file_unlocked cf(u8"filebuf_file.txt",fast_io::open_mode::out);
	for(std::size_t i{};i!=10000000;++i)
		print(cf,u8"Hello World\n");
}

