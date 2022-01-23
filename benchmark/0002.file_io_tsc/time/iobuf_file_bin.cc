#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/timer.h>
#include<vector>

int main()
{
	constexpr std::size_t N(10000000);
	{
	fast_io::timer t(u8"output");
	fast_io::obuf_file obf(u8"iobuf_file_bin.txt");
	for(std::size_t i{};i!=N;++i)
	{
		write(obf,__builtin_addressof(i),__builtin_addressof(i)+1);
	}
	}
	std::vector<std::size_t> vec(N,u8'0');
	{
	fast_io::timer t(u8"input");
	fast_io::ibuf_file ibf(u8"iobuf_file_bin.txt");
	for(auto it{vec.begin()};it!=vec.cend();++it)
	{
		::fast_io::read_all(ibf,it,it+1);
	}
	}
}
