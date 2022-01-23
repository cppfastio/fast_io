#include<fast_io.h>
#include<random>
#include<cfloat>
#include<fast_io_device.h>
#include<fast_io_driver/timer.h>
#include<fast_io_i18n.h>

int main()
{
	constexpr std::size_t N{10000000};
	std::mt19937_64 eng{};
	std::uniform_real_distribution<double> dis(DBL_MIN,DBL_MAX);
	std::vector<double> vec;
	vec.reserve(N);
	for(std::size_t i{};i!=N;++i)
	{
		vec.emplace_back(dis(eng));
	}
	fast_io::l10n l10n("");
	{
		fast_io::timer t(u8"general_l10n");
		fast_io::u8obuf_file file(u8"general_l10n.txt");
		for(auto const & e : vec)
			println(imbue(l10n,file),e);
	}
}
