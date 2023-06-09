#include<bitset>
#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/timer.h>
using namespace fast_io::io;

int main()
{
#if __cpp_lib_constexpr_bitset >= 202207L
	constexpr
#endif
	 std::bitset<152> bts("11011110000011111111111111111111111110110111100000111111111111111111111111101101111000001111111111111111111111111011011110000011111111111111111111111110");
	constexpr std::size_t N{1000000UL};
	fast_io::timer t(u8"u8obuf_file");
	fast_io::u8obuf_file obf(u8"u8obuf_file.txt",fast_io::open_mode::out);
	for(std::size_t i{};i!=N;++i)
	{
		println(obf,bts);
	}
}
