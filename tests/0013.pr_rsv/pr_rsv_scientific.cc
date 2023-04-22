#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	char buffer[::fast_io::pr_rsv_size<char,decltype(fast_io::mnp::scientific(2.21421421412412214124124))>];
	auto i{::fast_io::pr_rsv_to_c_array(buffer,fast_io::mnp::scientific(2.21421421412412214124124))};
	println("buffer size:",sizeof(buffer),"\ndata:",fast_io::mnp::strvw(buffer,i));
}
