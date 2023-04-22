#include<string_view>
#include<fast_io.h>
#include<fast_io_device.h>

using namespace fast_io::io;

int main()
{
	fast_io::native_file_loader loader(u8"test.txt");
	fast_io::ibuffer_view view(loader);
	for(auto e: line_scanner(view))
	{
		std::string_view ev{e};
		println(ev);
	}
}