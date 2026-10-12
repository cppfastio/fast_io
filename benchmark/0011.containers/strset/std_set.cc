#include <set>
#include <string>
#include <fast_io.h>
#include <fast_io_driver/timer.h>
#include "gentest.h"

int main()
{
	auto vec{::gentest<::std::string>()};
	::fast_io::timer t(u8"str_btree_set");
	::std::set<typename decltype(vec)::value_type> bset;
	{
		::fast_io::timer t(u8"insert");
		for (auto const &e : vec)
		{
			bset.insert(e);
		}
	}
	{
		::std::size_t count{};
		{
			::fast_io::timer t(u8"contains");
			for (auto const &e : vec)
			{
				count += static_cast<::std::size_t>(bset.contains(e));
			}
		}
		::fast_io::io::perrln("count=", count);
	}
	{
		::std::size_t total_size{};
		{
			::fast_io::timer t(u8"iteration");
			for (auto const &e : bset)
			{
				total_size += e.size();
			}
		}
		::fast_io::io::perrln("total_size=", total_size);
	}
	{
		::std::size_t total_size{};
		{
			::fast_io::timer t(u8"reverse iteration");
			for (auto const &e : ::std::ranges::reverse_view(bset))
			{
				total_size += e.size();
			}
		}
		::fast_io::io::perrln("total_size=", total_size);
	}
}
