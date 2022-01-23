#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/timer.h>
#include<vector>

int main()
{
	constexpr std::size_t N(10000000);
	{
	fast_io::timer t(u8"output");
	fast_io::c_file obf(u8"stdio.txt",fast_io::open_mode::out);
	for(std::size_t i{};i!=N;++i)
		fprintf(obf.fp,"%zu\n",i);
	}
	std::vector<std::size_t> vec(N);
	{
	fast_io::timer t(u8"input");
	fast_io::c_file ibf(u8"stdio.txt",fast_io::open_mode::in);
	for(std::size_t i{};i!=N;++i)
		fscanf(ibf.fp,"%zu",vec.data()+i);
	}
}