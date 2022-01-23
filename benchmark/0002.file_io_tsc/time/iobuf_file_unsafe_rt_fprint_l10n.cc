#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_i18n.h>
#include<fast_io_driver/timer.h>

int main()
{
	constexpr std::size_t N(10000000);
	fast_io::l10n loc("");
	constexpr fast_io::unsafe_rt_format_string_view view("{}\n",3);
	{
	fast_io::timer t(u8"output");
	fast_io::obuf_file obf("iobuf_file_unsafe_rt_fprint_l10n.txt");
	for(std::size_t i{};i!=N;++i)
		unsafe_rt_fprint(imbue(loc,obf),view,i);
	}
}
