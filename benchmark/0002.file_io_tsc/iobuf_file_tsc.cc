#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/tsc_timer.h>
#include<vector>

int main()
{
	constexpr std::size_t N(10000000);
	{
	fast_io::tsc_timer t(u8"output");
	fast_io::obuf_file obf("iobuf_file_tsc.txt");
	for(std::size_t i{};i!=N;++i)
		println(obf,i);
	}
	std::vector<std::size_t> vec(N);
	{
	fast_io::tsc_timer t(u8"input");
	fast_io::ibuf_file ibf("iobuf_file_tsc.txt");
	for(std::size_t i{};i!=N;++i)
		scan(ibf,vec[i]);
	}
}
