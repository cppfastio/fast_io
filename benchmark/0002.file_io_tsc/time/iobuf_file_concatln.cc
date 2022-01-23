#include<string>
#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/timer.h>
#include<vector>

int main()
{
	constexpr std::size_t N(10000000);
	{
	fast_io::timer t(u8"output");
	fast_io::obuf_file obf("iobuf_file_concatln.txt");
	for(std::size_t i{};i!=N;++i)
		print(obf,fast_io::concatln(i));
	}
}
