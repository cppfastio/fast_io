#include<fast_io_legacy.h>

int main()
{
	fast_io::filebuf_file fbf("filebuf_file.txt",fast_io::open_mode::out);
/*
filebuf_file will construct a std::filebuf directly through syscalls.
*/
	std::ofstream fout;
	*fout.rdbuf()=std::move(*fbf.fb);//move resource to std::ofstream

	fast_io::filebuf_io_observer fiob{fout.rdbuf()};
//because we moved the resources to fstream, we need to use filebuf_io_observer to use it instead
	print(fiob,"Hello World from fast_io::filebuf_io_observer\n");
	fout<<"Hello World from std::ofstream\n";
}
