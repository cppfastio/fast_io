#include<string>
#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/timer.h>
#include<vector>
#include<format>

int main()
{
	constexpr std::size_t N(10000000);
	{
	fast_io::timer t(u8"output");
	fast_io::obuf_file obf(u8"iobuf_file_std_format.txt");
	for(std::size_t i{};i!=N;++i)
	{
		print(obf,std::format("{}\n",i));
	}
	}
}
