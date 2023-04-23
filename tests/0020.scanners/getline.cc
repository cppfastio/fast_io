#include<string>
#include<fast_io_legacy.h>

using namespace fast_io::io;

int main()
{
	fast_io::filebuf_file fbf(u8"test.txt",fast_io::open_mode::in);
	std::istream fin(fbf.fb);
	for(std::string line;std::getline(fin,line);println(line));
}
