#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	char buffer[::fast_io::pr_rsv_size<char,double>];
	auto i{::fast_io::pr_rsv_to_c_array(buffer,2.21421421412412214124124)};
	println("buffer size:",sizeof(buffer),"\ndata:",fast_io::mnp::strvw(buffer,i));
}
