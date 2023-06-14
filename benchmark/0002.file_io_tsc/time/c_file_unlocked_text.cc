#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/timer.h>
#include<vector>
using namespace fast_io::io;

int main()
{
	constexpr std::size_t N(10000000);
	{
	fast_io::timer t(u8"output");
	fast_io::u8c_file_unlocked obf(u8"c_file_unlocked_text.txt",fast_io::open_mode::out|fast_io::open_mode::text);
	for(std::size_t i{};i!=N;++i)
		println(obf,i);
	}
	std::vector<std::size_t> vec(N);
	{
	fast_io::timer t(u8"input");
	fast_io::u8c_file_unlocked ibf(u8"c_file_unlocked_text.txt",fast_io::open_mode::in|fast_io::open_mode::text);
	for(std::size_t i{};i!=N;++i)
		scan(ibf,vec[i]);
	}
}
