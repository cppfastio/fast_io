#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_driver/qt.h>
#include<vector>
#include<fast_io_driver/timer.h>

int main()
{
	constexpr std::size_t N(10000000);
	{
	fast_io::timer t(u8"concat_qstring");
	fast_io::obuf_file obf("concat_qstring.txt");
	for(std::size_t i{};i!=N;++i)
		print(obf,fast_io::u16concatln_qt_qstring(i));
	}
}
