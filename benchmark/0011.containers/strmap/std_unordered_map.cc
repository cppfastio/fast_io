#include <unordered_map>
#include <string>
#include <string_view>
#include <fast_io.h>
#include <fast_io_driver/timer.h>
#include "gentest.h"

int main()
{
	auto vec{::gentest<::std::string>()};
	::fast_io::timer t(u8"::std::unordered_map<::std::string>");
	::std::unordered_map<::std::string, ::std::size_t> bmap;
	{
		::fast_io::timer t(u8"insert");
		for (::std::size_t i{}; i != vec.size(); ++i)
		{
			bmap.emplace(vec[i], i);
		}
	}
	{
		::std::size_t count{};
		{
			::fast_io::timer t(u8"contains");
			for (auto const &e : vec)
			{
				count += static_cast<::std::size_t>(bmap.contains(e));
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
				total_size += e.first.size();
				total_mapped += e.second;
			}
		}
		::fast_io::io::perrln("total_size=", total_size, " total_mapped=", total_mapped);
	}
	{
		::std::size_t total_size{};
		::std::size_t total_mapped{};
		{
			::fast_io::timer t(u8"reverse iteration (does not exist so we loop again)");
			for (auto const &e : bmap)
			{
				total_size += e.first.size();
				total_mapped += e.second;
			}
		}
		::fast_io::io::perrln("total_size=", total_size, " total_mapped=", total_mapped);
	}
}
