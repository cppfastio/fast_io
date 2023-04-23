#include<fast_io_device.h>
#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	fast_io::obuf_file obf(u8"constant.txt");
	println(obf,3);
	static_assert(fast_io::constant_buffer_output_stream<fast_io::obuf_file>);
	static_assert(fast_io::constant_buffer_output_stream<fast_io::u8ogb18030_file>);
}