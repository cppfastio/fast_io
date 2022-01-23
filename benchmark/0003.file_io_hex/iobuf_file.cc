#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/timer.h>
#include<vector>

int main()
{
	constexpr std::size_t N(10000000);
	{
	fast_io::timer t(u8"output");
	fast_io::obuf_file obf(u8"iobuf_file.txt");
	for(std::size_t i{};i!=N;++i)
		println(obf,fast_io::mnp::hex(i));
	}
	std::vector<std::size_t> vec(N);
	{
	fast_io::timer t(u8"input");
	fast_io::ibuf_file ibf(u8"iobuf_file.txt");
	for(std::size_t i{};i!=N;++i)
		scan(ibf,fast_io::mnp::hex_get(vec[i]));
	}
}
