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
	fast_io::obuf_file_mutex obf(u8"iobuf_file_mutex_big.txt");
	fast_io::io_lock_guard guard{obf};
	for(std::size_t i{};i!=N;++i)
		println(obf.unlocked_handle(),i);
	}
	std::vector<std::size_t> vec(N);
	{
	fast_io::timer t(u8"input");
	fast_io::ibuf_file_mutex ibf(u8"iobuf_file_mutex_big.txt");
	fast_io::io_lock_guard guard{ibf};
	for(std::size_t i{};i!=N;++i)
		scan(ibf.unlocked_handle(),vec[i]);
	}
}
