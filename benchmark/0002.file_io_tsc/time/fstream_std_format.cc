#include<string>
#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/timer.h>
#include<vector>
#include<format>
#include<fstream>

int main()
{
	constexpr std::size_t N(10000000);
	{
	fast_io::timer t(u8"output");
	std::ofstream fout(u8"fstream_std_format.txt");
	for(std::size_t i{};i!=N;++i)
	{
		fout<<std::format("{}\n",i);
	}
	}
}
