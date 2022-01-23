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
	fast_io::filebuf_file obf(u8"fstream.txt",fast_io::open_mode::out);
	std::ostream fout(obf.fb);
	fout<<std::hex;
	for(std::size_t i{};i!=N;++i)
		fout<<i<<'\n';
	}
	std::vector<std::size_t> vec(N);
	{
	fast_io::timer t(u8"input");
	fast_io::filebuf_file ibf(u8"fstream.txt",fast_io::open_mode::in);
	std::istream fin(ibf.fb);
	fin>>std::hex;
	for(std::size_t i{};i!=N;++i)
		fin>>vec[i];
	}
}
