#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_legacy.h>
#include<fast_io_driver/timer.h>

int main()
{
	fast_io::timer tm(u8"ofstream_endl");
	fast_io::filebuf_file fbf(u8"ofstream_endl.txt",fast_io::open_mode::out);
	std::ostream fout(fbf.fb);
//PLEASE NEVER USE std::endl!!!!!!!! It is a historical mistake and part of the reason why fast_io as a library even exists in the first place.
	for(std::size_t i{};i!=10000000;++i)
		fout<<"Hello World"<<std::endl;
}
