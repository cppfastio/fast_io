#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/timer.h>
using namespace fast_io::io;

int main()
{
	fast_io::timer tm(u8"u8obuf_file");
	fast_io::u8obuf_file obf(u8"u8obuf_file.txt");
	for(std::size_t i{};i!=10000000;++i)
		print(obf,u8"Hello World\n");
}
