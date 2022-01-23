#include<fast_io.h>
#include<random>
#include<cfloat>
#include<fast_io_device.h>
#include<fast_io_driver/timer.h>
#include<charconv>

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
	{
		char buffer[40];
		auto start{buffer};
		fast_io::timer t(u8"charconv general");
		fast_io::obuf_file file(u8"general_charconv.txt");
		for(auto const & e : vec)
		{
			auto [res,ec]=std::to_chars(buffer,start+40,e,std::chars_format::general);
			*res='\n';
			++res;
			write(file,start,res);
		}
	}
}
