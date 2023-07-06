#include<fast_io.h>
#include<fast_io_driver/timer.h>

int main()
{
	fast_io::timer tm(u8"nt");
	::fast_io::nt_file nf(u"ntfile.txt",fast_io::open_mode::out);
	for(::std::size_t i{};i!=100000;++i)
	{
		::fast_io::io::print(nf,"Hello World\n","Hello World\n","Hello World\n");
	}
}

