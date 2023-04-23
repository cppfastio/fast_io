#include<array>
#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	std::array<char,20> buffer;
	//this would fail to compile
	auto i{::fast_io::pr_rsv_to_array(buffer,fast_io::mnp::scientific(2.21421421412412214124124))};
	println("buffer size:",buffer.size(),"\ndata:",fast_io::mnp::strvw(buffer.data(),i));
}
