#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_driver/tsc_timer.h>
#include <fast_io_driver/timer.h>
#include <vector>
#include <cstdio>
using namespace fast_io::io;

int main()
{
	constexpr std::size_t N = 10'000'000;
	// fast_io file write
	{
		fast_io::timer t(u8"fastio_file_write");
		fast_io::obuf_file obf("io_bench.txt");
		for (std::size_t i{}; i != N; ++i)
		{
			println(obf, i);
		}
	}
	// stdio file write
	{
		fast_io::timer t(u8"stdio_file_write");
		fast_io::c_file cf("io_bench_stdio.txt", fast_io::open_mode::out);
		for (std::size_t i{}; i != N; ++i)
		{
			std::fprintf(cf.fp, "%zu\n", i);
		}
	}
	std::vector<std::size_t> vec(N);
	// fast_io file read
	{
		fast_io::timer t(u8"fastio_file_read");
		fast_io::ibuf_file ibf("io_bench.txt");
		for (std::size_t i{}; i != N; ++i)
		{
			scan(ibf, vec[i]);
		}
	}
	// stdio file read
	{
		fast_io::timer t(u8"stdio_file_read");
		fast_io::c_file cf("io_bench_stdio.txt", fast_io::open_mode::in);
		for (std::size_t i{}; i != N; ++i)
		{
			[[maybe_unused]] auto _ = std::fscanf(cf.fp, "%zu", &vec[i]);
		}
	}
}
