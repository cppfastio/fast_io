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
	fast_io::obuf_file obf(u8"iobuf_file_le32.txt");
	for(std::size_t i{};i!=N;++i)
	{
		print(obf,fast_io::mnp::le_put<32>(i));
	}
	}
	std::vector<std::size_t> vec(N,u8'0');
	{
	fast_io::timer t(u8"input");
	fast_io::ibuf_file ibf(u8"iobuf_file_le32.txt");
	for(auto it{vec.begin()};it!=vec.cend();++it)
	{
		scan(ibf,fast_io::mnp::le_get<32>(*it));
	}
	}
}
