﻿#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_driver/timer.h>
#include <fast_io_dsal/vector.h>

int main()
{
	constexpr std::size_t N(10000000);
	{
		fast_io::timer t(u8"output");
		fast_io::c_file obf(u8"stdiotext.txt", fast_io::open_mode::out | fast_io::open_mode::text);
		for (std::size_t i{}; i != N; ++i)
		{
			[[maybe_unused]] auto _ = fprintf(obf.fp, "%zu\n", i);
		}
	}
	::fast_io::vector<std::size_t> vec(N);
	{
		fast_io::timer t(u8"input");
		fast_io::c_file ibf(u8"stdiotext.txt", fast_io::open_mode::in | fast_io::open_mode::text);
		for (std::size_t i{}; i != N; ++i)
		{
			[[maybe_unused]] auto _ = fscanf(ibf.fp, "%zu", vec.data() + i);
		}
	}
}
