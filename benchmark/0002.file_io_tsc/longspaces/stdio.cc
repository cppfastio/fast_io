﻿#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_driver/timer.h>
#include <vector>
#include "longspaces.h"

int main()
{
	{
		fast_io::timer t(u8"output");
		fast_io::c_file obf(u8"stdio.txt", fast_io::open_mode::out);
		using char_type = typename decltype(obf)::output_char_type;
		for (std::size_t i{}; i != N; ++i)
		{
			[[maybe_unused]] auto _ = fprintf(obf.fp, "%zu", i);
			[[maybe_unused]] auto __ = fwrite(filled_chars<char_type>.data(), filled_chars<char_type>.size(), sizeof(char_type), obf.fp);
		}
	}
	std::vector<std::size_t> vec(N);
	{
		fast_io::timer t(u8"input");
		fast_io::c_file ibf(u8"stdio.txt", fast_io::open_mode::in);
		for (std::size_t i{}; i != N; ++i)
		{
			[[maybe_unused]] auto _ = fscanf(ibf.fp, "%zu", vec.data() + i);
		}
	}
}
