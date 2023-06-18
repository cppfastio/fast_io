#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/timer.h>
#include<vector>
using namespace fast_io::io;

int main()
{
	constexpr std::size_t N(10000000);
	{
	fast_io::timer t(u8"output");
	fast_io::u32obuf_file obf(u8"u32iobuf_file.txt");
	for(std::size_t i{};i!=N;++i)
	{
		println(obf,i);
	}
	}
	std::vector<std::size_t> vec(N,u8'0');
	{
	fast_io::timer t(u8"input");
	fast_io::u32ibuf_file ibf(u8"u32iobuf_file.txt");
	for(auto it{vec.begin()};it!=vec.cend();++it)
	{
		scan(ibf,*it);
	}
	}
}
