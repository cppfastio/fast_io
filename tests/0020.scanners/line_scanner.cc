#include<string_view>
#include<fast_io.h>
#include<fast_io_device.h>

int main()
{
	fast_io::ibuf_file ibf(u8"test.txt");
	for(std::string_view e : line_scanner(ibf))
	{
		println(e);
	}
}