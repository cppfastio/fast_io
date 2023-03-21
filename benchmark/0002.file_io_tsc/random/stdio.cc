#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/timer.h>
#include"generate.h"

int main()
{
	auto tb{generate_random_data()};
	{
	fast_io::timer t(u8"output");
	fast_io::c_file cf(u8"stdio.txt",::fast_io::open_mode::out);
	for(auto& e : tb)
		fprintf(cf.fp,"%zu\n",e);
	}
	::fast_io::vector<std::size_t> vec(N);
	{
	fast_io::timer t(u8"input");
	fast_io::c_file cf(u8"stdio.txt",::fast_io::open_mode::in);
	for(std::size_t i{};i!=N;++i)
		fscanf(cf.fp,"%zu",vec.data()+i);
	}
}
