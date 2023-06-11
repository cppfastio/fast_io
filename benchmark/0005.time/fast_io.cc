#include<fast_io_device.h>
#include<fast_io.h>
#include<fast_io_driver/timer.h>
#include<fast_io_i18n.h>
using namespace fast_io::io;

int main()
{
	fast_io::native_l10n l10n("");
	constexpr std::size_t N(10000000);
	auto iso8601_ts{utc(fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime))};
	{
	fast_io::timer timer(u8"fast_io");
	fast_io::obuf_file obf(u8"fast_io.txt");
	for(std::size_t i{};i!=N;++i)
	{
		println(imbue(l10n,obf),fast_io::mnp::date_fmt(iso8601_ts));
	}
	}
}