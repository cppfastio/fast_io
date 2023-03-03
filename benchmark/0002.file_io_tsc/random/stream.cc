#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_legacy.h>
#include<fast_io_driver/timer.h>
#include"generate.h"

int main()
{
	auto tb{generate_random_data()};
	{
	fast_io::timer t(u8"output");
	fast_io::filebuf_file fbf(u8"stream.txt",::fast_io::open_mode::out);
	::std::ostream os(fbf.fb);
	for(auto& e : tb)
	{
		os<<e<<'\n';
	}
	}
	::fast_io::vector<std::size_t> vec(N);
	{
	fast_io::timer t(u8"input");
	fast_io::filebuf_file fbf(u8"stream.txt",::fast_io::open_mode::in);
	::std::istream ism(fbf.fb);
	for(std::size_t i{};i!=N;++i)
		ism>>vec[i];
	}
}
