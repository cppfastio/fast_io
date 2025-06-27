#pragma once

#include <cstddef>
#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_dsal/vector.h>
#include <fast_io_dsal/string.h>
// #include <fast_io_dsal/string_view.h>

inline ::fast_io::vector<::fast_io::u8string> parse_symbols(char8_t const *path)
{
	::fast_io::vector<::fast_io::u8string> result;
	// TODO native_file_loader do not suppport ::fast_io::string_view
	::fast_io::native_file_loader symbols_loader{::fast_io::mnp::os_c_str(path)};
	auto const &&symbols_loader_size = symbols_loader.size();
	for (::std::size_t i{}; i < symbols_loader_size; ++i)
	{
		::fast_io::u8string tmp{};
		while (symbols_loader[i] != ',')
		{
			tmp.push_back(symbols_loader[i++]);
		}
		result.push_back(::std::move(tmp));
		while (symbols_loader[++i] != '\n')
		{
		}
	}

	return result;
}
