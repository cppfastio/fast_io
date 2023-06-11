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
	fast_io::u8obuf_file obf(u8"iobuf_file.txt");
	for(auto& e : tb)
		println(obf,e);
	}
	::fast_io::vector<std::size_t> vec(N);
	{
	fast_io::timer t(u8"input");
	fast_io::u8ibuf_file ibf(u8"iobuf_file.txt");
	for(std::size_t i{};i!=N;++i)
		scan(ibf,vec[i]);
	}
}
