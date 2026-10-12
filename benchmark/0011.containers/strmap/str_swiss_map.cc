#include <ranges>
#include <fast_io_dsal/str_swiss_map.h>
#include <fast_io.h>
#include <fast_io_driver/timer.h>
#include "gentest.h"

int main()
{
	auto vec{::gentest()};
	::fast_io::timer t(u8"str_swiss_map");
	::fast_io::str_swiss_map<::std::size_t> bmap;
	{
		::fast_io::timer t(u8"insert_key");
		for (::std::size_t i{}; i != vec.size(); ++i)
		{
			bmap.insert_key(::fast_io::string_view(::std::from_range, vec[i]), i);
		}
	}
	{
		::std::size_t count{};
		{
			::fast_io::timer t(u8"contains");
			for (auto const &e : vec)
			{
				count += static_cast<::std::size_t>(bmap.contains(::fast_io::string_view(::std::from_range, e)));
			}
		}
		::fast_io::io::perrln("count=", count);
	}
	{
		::std::size_t total_size{};
		::std::size_t total_mapped{};
		{
			::fast_io::timer t(u8"iteration");
			for (auto const &e : bmap)
			{
				total_size += e.key().size();
				total_mapped += e.mapped();
			}
		}
		::fast_io::io::perrln("total_size=", total_size, " total_mapped=", total_mapped);
	}
	{
		::std::size_t total_size{};
		::std::size_t total_mapped{};
		{
			::fast_io::timer t(u8"reverse iteration");
			for (auto const &e : ::std::ranges::reverse_view(bmap))
			{
				total_size += e.key().size();
				total_mapped += e.mapped();
			}
		}
		::fast_io::io::perrln("total_size=", total_size, " total_mapped=", total_mapped);
	}
}
