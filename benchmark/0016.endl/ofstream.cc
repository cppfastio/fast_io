#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_legacy.h>
#include<fast_io_driver/timer.h>

int main()
{
	fast_io::timer tm(u8"ofstream");
	fast_io::filebuf_file fbf(u8"ofstream.txt",fast_io::open_mode::out);
	std::ostream fout(fbf.fb);
	for(std::size_t i{};i!=10000000;++i)
		fout<<"Hello World\n";
}

