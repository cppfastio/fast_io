#include<clocale>
#include<langinfo.h>
#include<fast_io_device.h>
#include<fast_io.h>
#include<fast_io_driver/timer.h>

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
	fast_io::timer timer(u8"glibc_nl_langinfo");
	fast_io::obuf_file obf(u8"glibc_nl_langinfo.txt");
	for(std::size_t i{};i!=N;++i)
	{
		std::size_t diff{strftime_l(buffer,buffer_size,nl_str,__builtin_addressof(tm_value),glibc_locale)};
		write(obf,buffer,buffer+diff);
		println(obf);
	}
	}
}