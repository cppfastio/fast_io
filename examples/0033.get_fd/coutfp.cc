#include <fast_io_legacy.h>
#include <fast_io_driver/timer.h>
#include <iostream>

int main()
{
	::std::uintptr_t u{};
	{
	::fast_io::timer tm(u8"coutfd");
	for (::std::size_t i{};i!=10000000;++i)
	{
		::fast_io::streambuf_io_observer siob{std::cout.rdbuf()};
		u+=std::bit_cast<std::uintptr_t>(static_cast<::fast_io::c_io_observer>(siob).fp);
	}
	}
	::fast_io::io::println("u=",u);
}
