#include<random>
#include<fast_io.h>

int main()
{
	::fast_io::ibuf_white_hole_engine iwhe;
	::std::uniform_int_distribution<::std::uint_least64_t> udis(0,UINT_LEAST64_MAX);
	for(::std::size_t i{};i!=10;++i)
	{
		::fast_io::io::println(::fast_io::mnp::addrvw(udis(iwhe)));
	}
}

