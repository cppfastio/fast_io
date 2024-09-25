#include <string>
#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_driver/timer.h>
using namespace fast_io::io;

int main()
{
	constexpr std::size_t N(10000000);
	{
		fast_io::timer t(u8"u32concat_std");
		fast_io::u32obuf_file obf(u8"u32concat_std.txt");
		for (std::size_t i{}; i != N; ++i)
		{
			print(obf, fast_io::u32concatln_std(i));
		}
	}
}
