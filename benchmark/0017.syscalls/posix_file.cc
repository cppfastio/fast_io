#include<fast_io.h>
#include<fast_io_driver/timer.h>

int main()
{
	fast_io::timer tm(u8"posix");
	::fast_io::posix_file nf(u"posixfile.txt",fast_io::open_mode::out);
	for(::std::size_t i{};i!=100000;++i)
	{
		::fast_io::io::print(nf,"Hello World\n","Hello World\n","Hello World\n");
	}
}
