#include<fast_io_legacy.h>

using namespace fast_io::io;

int main()
{
	fast_io::filebuf_file fbf(u8"filebuf_file_ostream.txt",fast_io::open_mode::out);
/*
filebuf_file will construct a std::filebuf directly through syscalls.
*/
	std::ostream fout(fbf.fb); //make std::ostream's streambuf binds to our filebuf_file
	print(fbf,"Hello World from fast_io::filebuf_file\n");
	fout<<"Hello World from std::ostream\n";
}
