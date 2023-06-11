#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/timer.h>
#include"generate.h"
using namespace fast_io::io;

int main()
{
	auto tb{generate_random_data()};
	{
	fast_io::timer t(u8"output");
	fast_io::c_file cf(u8"c_file.txt",::fast_io::open_mode::out);
	for(auto& e : tb)
		println(cf,e);;
	}
	::fast_io::vector<std::size_t> vec(N);
	{
	fast_io::timer t(u8"input");
	fast_io::c_file cf(u8"c_file.txt",::fast_io::open_mode::in);
	for(std::size_t i{};i!=N;++i)
	{
		scan(cf,vec[i]);
	}
	}
}
