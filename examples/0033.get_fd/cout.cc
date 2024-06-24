#include <fast_io_legacy.h>
#include <fast_io_driver/timer.h>
#include <iostream>

int main()
{
	unsigned u{};
	{
	::fast_io::timer tm(u8"coutfd");
	for (::std::size_t i{};i!=10000000;++i)
	{
		::fast_io::streambuf_io_observer siob{std::cout.rdbuf()};
		u+=static_cast<unsigned>(static_cast<::fast_io::posix_io_observer>(siob).fd);
	}
	}
	::fast_io::io::println("u=",u);
}
