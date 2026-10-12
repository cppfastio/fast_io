#include <fast_io.h>
#include <fast_io_driver/timer.h>
#include <fast_io_dsal/list.h>

int main()
{
	fast_io::timer tm(u8"fast_io::list");
	fast_io::list<std::size_t> lst;
	constexpr std::size_t n{100000000};
	{
		fast_io::timer tm1(u8"push_back");
		for (std::size_t i{}; i != n; ++i)
		{
			lst.push_back(i);
		}
	}
	::std::size_t sum{};
	{
		fast_io::timer tm1(u8"loop");
		for (auto const e : lst)
		{
			sum += e;
		}		
	}
	::fast_io::io::perrln("sum=",sum);
}
