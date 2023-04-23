#include<fast_io.h>
#include<fast_io_device.h>

using namespace fast_io::io;

int main()
try
{
	fast_io::obuf_file pf(u8"example.txt",fast_io::open_mode::app);
	fast_io::file_lock_guard g(file_lock(pf.handle),fast_io::flock_request_l64{});
	fast_io::io_flush_guard g2(pf);
	for(std::size_t i{};i!=1000000;++i)
	{
		print(pf,"Hello ");
		print(pf,"World\n");
	}
}
catch(fast_io::error e)
{
	perrln(e);
	return 1;
}