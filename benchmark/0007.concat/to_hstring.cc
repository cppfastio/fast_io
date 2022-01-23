#include<winrt/base.h>
#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/timer.h>

int main()
{
	constexpr std::size_t N(10000000);
	{
	fast_io::timer t(u8"concat_hstring");
	fast_io::obuf_file obf(u"concat_hstring.txt");
	for(std::size_t i{};i!=N;++i)
		println(obf,winrt::to_hstring(i));
	//this is a little bit cheat to the benchmark. but i guess it would still be extremely slow
	}
}
