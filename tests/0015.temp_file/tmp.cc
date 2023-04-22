#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	fast_io::native_file file(fast_io::io_temp);
	for(std::size_t i{};i!=1000;++i)
		print(file,"Hello World\n");
	rewind(file);
	println("transmitted:",transmit(fast_io::c_stdout(),file));
}
