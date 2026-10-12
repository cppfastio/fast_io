#include <fast_io_dsal/str_ranked_btree_set.h>
#include <fast_io.h>
#include <fast_io_driver/timer.h>
#include <ranges>
#include "gentest.h"

int main()
{
	auto vec{::gentest()};
	::fast_io::timer t(u8"str_ranked_btree_set");
	::fast_io::str_ranked_btree_set bset;
	{
		::fast_io::timer t(u8"insert_key");
		for (auto const &e : vec)
		{
			bset.insert_key(::fast_io::string_view(::std::from_range, e));
		}
	}
	{
		::std::size_t count{};
		{
			::fast_io::timer t(u8"contains");
			for (auto const &e : vec)
			{
				count += static_cast<::std::size_t>(bset.contains(::fast_io::string_view(::std::from_range, e)));
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
	{
		::std::size_t total_size{};
		{
			::fast_io::timer t(u8"nth_element");
			for (::std::size_t i{}; i != bset.size(); ++i)
			{
				total_size += (*bset.nth_element(i)).size();
			}
		}
		::fast_io::io::perrln("total_size=", total_size);
	}
}
