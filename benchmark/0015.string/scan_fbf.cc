#include<string>
#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_legacy.h>
#include<fast_io_driver/timer.h>
using namespace fast_io::io;

int main()
{
	std::size_t spaces{};
	{
		fast_io::timer t(u8"input");
		//use std::filebuf_file to provide a fair comparison to C++ stream facilities
		fast_io::filebuf_file fbf(u8"ibuf_white_hole_engine.txt",fast_io::open_mode::in);
		for(std::string str;scan<true>(fbf,str);++spaces){}
	}
	println(spaces);
}
