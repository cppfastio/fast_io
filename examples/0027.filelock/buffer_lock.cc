#include<fast_io.h>
#include<fast_io_device.h>

using namespace fast_io::io;

int main()
{
	fast_io::obuf_file pf(u8"example.txt",fast_io::open_mode::app);
	fast_io::file_lock_guard g(file_lock(pf),fast_io::flock_request_l64{});
	for(std::size_t i{};i!=1000000;++i)
	{
		print(pf,"Hello ");
		print(pf,"World\n");
	}
}