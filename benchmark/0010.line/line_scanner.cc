#include<string_view>
#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/timer.h>
#include<vector>
using namespace fast_io::io;

int main()
{
	std::size_t lines{};
	{
		fast_io::timer t(u8"input");
		fast_io::u8ibuf_file ibf(u8"ibuf_white_hole_engine.txt");
		for(auto&& line:line_scanner(ibf))
		{
			++lines;
		}
	}
	println(lines);
}
