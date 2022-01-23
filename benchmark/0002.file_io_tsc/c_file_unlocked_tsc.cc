#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/tsc_timer.h>
#include<vector>

int main()
{
	constexpr std::size_t N(10000000);
	{
	fast_io::tsc_timer t(u8"output");
	fast_io::u8c_file_unlocked obf(u8"c_file_unlocked_tsc.txt",fast_io::open_mode::out);
	for(std::size_t i{};i!=N;++i)
		println(obf,i);
	}
	std::vector<std::size_t> vec(N);
	{
	fast_io::tsc_timer t(u8"input");
	fast_io::u8c_file_unlocked ibf(u8"c_file_unlocked_tsc.txt",fast_io::open_mode::in);
	for(std::size_t i{};i!=N;++i)
		scan(ibf,vec[i]);
	}
}
