#include<fast_io.h>
#include<fast_io_driver/timer.h>

int main()
{
	fast_io::timer tm(u8"win32");
	::fast_io::win32_file nf(u"win32file.txt",fast_io::open_mode::out);
	for(::std::size_t i{};i!=100000;++i)
	{
		::fast_io::io::print(nf,"Hello World\n","Hello World\n","Hello World\n");
	}
}
