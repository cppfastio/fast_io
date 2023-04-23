#include<fast_io.h>
#include<charconv>
#include<random>
#include<cassert>
#include<cstring>

using namespace fast_io::io;

inline void test(std::uint_least64_t s)
{
	char buffer[fast_io::pr_rsv_size<char,std::uint_least64_t>];
	char charconv_buffer[fast_io::pr_rsv_size<char,std::uint_least64_t>*2];
	auto [p,ec]=std::to_chars(charconv_buffer,charconv_buffer+sizeof(charconv_buffer),s);
	auto it{fast_io::pr_rsv_to_c_array(buffer,s)};
	std::size_t my_size{static_cast<std::size_t>(it-buffer)};
	assert(my_size==static_cast<std::size_t>(p-charconv_buffer)&&memcmp(charconv_buffer,buffer,my_size)==0);
}

int main()
{
	test(0);
	test(1);
	test(10);
	test(100);
	test(1000);
	test(UINT_LEAST64_MAX);
	test(UINT_LEAST64_MAX/10u);
	test(UINT_LEAST64_MAX/100u);
	fast_io::ibuf_white_hole_engine engine;
	std::vector<std::uniform_int_distribution<std::uint_least64_t>> distributions;
	std::uint_least64_t value{1};
	for(std::size_t i{};i!=std::numeric_limits<std::uint_least64_t>::digits10;++i)
	{
		distributions.emplace_back(0u,value);
		value*=10u;
	}
	for(std::size_t i{};i!=1000000ULL;++i)
	{
		for(auto& dis : distributions)
		{
			test(dis(engine));
		}
	}
}