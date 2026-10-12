#include <ranges>
#include <fast_io_dsal/str_btree_set.h>
#include <fast_io.h>
#include <fast_io_driver/timer.h>
#include "gentest.h"

int main()
{
	auto vec{::gentest<::fast_io::u8string>()};
	::fast_io::timer t(u8"str_btree_set");
	::fast_io::u8str_btree_set bset;
	{
		::fast_io::timer t(u8"insert_key");
		for (auto const &e : vec)
		{
			bset.insert_key(::fast_io::u8string_view(::std::from_range, e));
		}
	}
	{
		::std::size_t count{};
		{
			::fast_io::timer t(u8"contains");
			for (auto const &e : vec)
			{
				count += static_cast<::std::size_t>(bset.contains(::fast_io::u8string_view(::std::from_range, e)));
			}
		}
		::fast_io::io::perrln("count=", count);
	}
}
