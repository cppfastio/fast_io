#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_i18n.h>
#include<fast_io_driver/timer.h>
#include<vector>

int main()
{
	constexpr std::size_t N(10000000);
	{
	fast_io::timer t(u8"output");
	fast_io::obuf_file obf("iobuf_file_l10n.txt");
	fast_io::l10n loc("");
	for(std::size_t i{};i!=N;++i)
		println(imbue(loc,obf),fast_io::mnp::hex(i));
	}
}
