#include<clocale>
#include<langinfo.h>
#include<fast_io.h>
#include<fast_io_driver/timer.h>
#include<fast_io_legacy.h>
#include<locale>
#include<iomanip>

int main()
{
	auto glibc_locale{newlocale(LC_ALL,"",locale_t(0))};
	constexpr std::size_t buffer_size{10000};
	char buffer[buffer_size];
	constexpr std::size_t N(10000000);

	std::time_t t{};
	t=time(__builtin_addressof(t));
	struct tm tm_value{};
	gmtime_r(__builtin_addressof(t),__builtin_addressof(tm_value));
	{
	auto nl_str{nl_langinfo_l(_DATE_FMT,glibc_locale)};
	fast_io::timer timer(u8"std_put_time_nl_langinfo");
	fast_io::filebuf_file fbf(u8"std_put_time_nl_langinfo.txt",fast_io::open_mode::out);
	std::ostream os(fbf.fb);
	os.imbue(std::locale(""));
	for(std::size_t i{};i!=N;++i)
	{
		os<<std::put_time(__builtin_addressof(tm_value),nl_str);
		println(fbf);
	}
	}
}