#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/timer.h>
#include<vector>
#include"longspaces.h"
using namespace fast_io::io;

int main()
{
	{
	fast_io::timer t(u8"output");
	fast_io::obuf_file obf("iobuf_file.txt");
	for(std::size_t i{};i!=N;++i)
		print(obf,i,fast_io::mnp::strvw(filled_chars<typename decltype(obf)::char_type>));
	}
	std::vector<std::size_t> vec(N);
	{
	fast_io::timer t(u8"input");
	fast_io::ibuf_file ibf("iobuf_file.txt");
	for(std::size_t i{};i!=N;++i)
		scan(ibf,vec[i]);
	}
}
