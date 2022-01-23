#include<fast_io.h>
#include<fast_io_device.h>
#include<random>
#include<fast_io_driver/timer.h>

int main()
{
	fast_io::timer tm(u8"ibuf_white_hole_engine");
	fast_io::u8obuf_file obf(u"ibuf_white_hole_engine.txt");
	fast_io::ibuf_white_hole_engine eng;
	std::uniform_int_distribution<std::size_t> ud(0,61);
	std::uniform_int_distribution<std::size_t> rlen(8,20);
	for(std::size_t i(0);i!=1000000;++i)
	{
		for(std::size_t j(0),s(rlen(eng));j!=s;++j)
		{
			char8_t ch(static_cast<char8_t>(ud(eng)));
			if(ch<10u)
				ch+=u8'0';
			else if(ch<36u)
				ch=ch-10u+u8'a';
			else
				ch=ch-36u+u8'A';
			print(obf,fast_io::mnp::chvw(ch));
		}
		println(obf);
	}
}
