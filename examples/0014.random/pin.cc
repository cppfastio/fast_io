#include<fast_io.h>
#include<fast_io_device.h>
#include<random>

using namespace fast_io::io;

int main(int argc,char** argv)
{
	std::size_t n{100};
	if(1<argc)
	{
		n=fast_io::to<std::size_t>(fast_io::mnp::os_c_str(argv[1]));
	}
	fast_io::u8obuf_file obf(u"pin.txt");
	fast_io::ibuf_white_hole_engine eng;
	std::uniform_int_distribution<unsigned> ud(0,9);
	for(std::size_t i{};i!=n;++i)
	{
		for(std::size_t j{};j!=4zu;++j)
		{
			char8_t ch(static_cast<char8_t>(ud(eng)));
			ch+=u8'0';
			print(obf,fast_io::mnp::chvw(ch));
		}
		println(obf);
	}
}
