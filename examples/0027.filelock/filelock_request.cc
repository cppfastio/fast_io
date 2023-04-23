#include<fast_io.h>

using namespace fast_io::io;

int main()
try
{
	fast_io::native_file pf(u8"example.txt",fast_io::open_mode::app);
	for(std::size_t i{};i!=1000000;++i)
	{
		fast_io::file_lock_guard g(file_lock(pf),fast_io::flock_request_l64
		{.type=fast_io::file_lock_mode::exclusive_lock,
		.whence=fast_io::seekdir::cur,
		.start=0,
		.len=20});
		print(pf,"Hello ");
		print(pf,"World\n");
	}
}
catch(fast_io::error e)
{
	perrln(e);
	return 1;
}