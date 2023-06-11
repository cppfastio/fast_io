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
		fast_io::native_file_loader loader(u8"ibuf_white_hole_engine.txt");
		fast_io::ibuffer_view view(loader);
		for(auto&& line:line_scanner(view))
		{
			++lines;
		}
	}
	println(lines);
}
