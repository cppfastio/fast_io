#include<atlstr.h>
#include<fast_io_driver/mfc.h>
#include<fast_io.h>
#include<fast_io_driver/timer.h>
#include<fast_io_device.h>

int main()
{
	constexpr std::size_t N(10000000);
	{
	fast_io::timer t(u8"concat_cstring");
	fast_io::obuf_file obf("concat_cstring.txt");
	for(std::size_t i{};i!=N;++i)
		print(obf,fast_io::concatln_atl_cstring(i));
	}
}
