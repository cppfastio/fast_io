#include<string>
#include<fast_io_legacy.h>
#include<fast_io_driver/timer.h>
using namespace fast_io::io;

int main()
{
	std::size_t lines{};
	{
		fast_io::timer t(u8"input");
		fast_io::filebuf_file fbf(u8"ibuf_white_hole_engine.txt",fast_io::open_mode::in);
		std::istream fin(fbf.fb);
		for(std::string line;fin>>line;++lines){}
	}
	println(lines);
}
