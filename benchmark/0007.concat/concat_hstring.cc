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
		print(obf,fast_io::wconcatln_winrt_hstring(i));
	}
}
