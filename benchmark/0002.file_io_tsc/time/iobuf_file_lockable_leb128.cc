#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_driver/timer.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;

int main()
{
	constexpr std::size_t N(10000000);
	{
		fast_io::timer t(u8"output");
		fast_io::u8obuf_file_lockable obf(u8"iobuf_file_lockable_leb128.txt");
		for (std::size_t i{}; i != N; ++i)
		{
			print(obf, fast_io::mnp::leb128_put(i));
		}
	}
	::fast_io::vector<std::size_t> vec(N);
	{
		fast_io::timer t(u8"input");
		fast_io::u8ibuf_file_lockable ibf(u8"iobuf_file_lockable_leb128.txt");
		for (auto it{vec.begin()}; it != vec.cend(); ++it)
		{
			scan(ibf, fast_io::mnp::leb128_get(*it));
		}
	}
}
