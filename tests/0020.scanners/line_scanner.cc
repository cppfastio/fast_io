#include<string_view>
#include<fast_io.h>
#include<fast_io_device.h>

using namespace fast_io::io;

int main()
{
	fast_io::ibuf_file ibf(u8"test.txt");
	for(auto&& e: line_scanner(ibf))
	{
		std::string_view ev{e};
		println(ev);
	}
}