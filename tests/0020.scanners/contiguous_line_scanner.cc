#include<string_view>
#include<fast_io.h>
#include<fast_io_device.h>

int main()
{
	fast_io::native_file_loader loader(u8"test.txt");
	fast_io::ibuffer_view view(loader);
	for(std::string_view e : line_scanner(view))
	{
		println(e);
	}
}