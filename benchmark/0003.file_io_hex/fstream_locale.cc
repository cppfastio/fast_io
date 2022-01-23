#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_legacy.h>
#include<fast_io_driver/timer.h>
#include<vector>
#include<iomanip>

int main()
{
	constexpr std::size_t N(10000000);
	{
	fast_io::timer t(u8"output");
	fast_io::filebuf_file obf(u8"fstream_locale.txt",fast_io::open_mode::out);
	std::ostream fout(obf.fb);
	fout.imbue(std::locale(""));
	fout<<std::hex;
	for(std::size_t i{};i!=N;++i)
		fout<<i<<'\n';
	}
}
