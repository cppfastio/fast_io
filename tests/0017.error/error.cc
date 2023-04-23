#include<fast_io.h>
#include<fast_io_device.h>

using namespace fast_io::io;

int main()
try
{
	fast_io::u8ibuf_file ibf("not_exist.txt");
}
catch(fast_io::error e)
{
	if(e==std::errc::no_such_file_or_directory)
	{
		perr("errc:no_such_file_or_directory\n");
	}
	return 1;
}
