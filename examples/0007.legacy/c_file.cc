#include<fast_io.h>

int main()
{
	fast_io::c_file cfl("c_file.txt",fast_io::open_mode::out);
/*
c_file will construct a FILE* directly through syscalls.
*/
	print(cfl,"Hello World from fast_io::c_file\n");
/*
Just for demo purpose
This is dangerous. use fprintf to print string is awful.
*/
	std::fprintf(cfl.fp,"Hello World from FILE* danger\n");
	std::fprintf(cfl.fp,"%s","Hello World from FILE* safe\n");

}
