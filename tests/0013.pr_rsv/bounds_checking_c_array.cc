#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	char buffer[20];
	//this would fail to compile
	auto i{::fast_io::pr_rsv_to_c_array(buffer,fast_io::mnp::scientific(2.21421421412412214124124))};
	println("buffer size:",sizeof(buffer),"\ndata:",fast_io::mnp::strvw(buffer,i));
}
