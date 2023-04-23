#include<random>
#include<fast_io.h>
#include<fast_io_device.h>

using namespace fast_io::io;

int main(int argc,char** argv)
{
	using namespace fast_io::mnp;
	if(argc<2)
	{
		if(argc==0)
		{
			return 1;
		}
		perr("Usage: ",os_c_str(*argv)," N\n");
		return 1;
	}
//I guess with artificial womb. The male to female birth ratio would be 1:1 due to elimination of all factors for female fetus. I maybe wrong about this.
//With artificial womb. Probably we will live in the future where male to female ratio is below 0.9 due to higher rate of death of men. I maybe wrong at this either.
	constexpr auto natural_male_to_female_ratio{1};
	constexpr auto total_ratio{natural_male_to_female_ratio+1.0};
	constexpr auto male_probability{natural_male_to_female_ratio/total_ratio};
	fast_io::ibuf_white_hole_engine engine;
	std::bernoulli_distribution dis;//no need to put a factor here. It is 0.5 by default
	::std::size_t boys{};
	::std::size_t girls{};
	fast_io::u8obuf_file obf(u8"sexratio_with_artificial_womb.txt");
	print(obf,u8"Natural Male to female birth ratio:",natural_male_to_female_ratio,u8"\n"
		u8"Male Baby birth Probability:",male_probability,u8"\n\n");	
	::std::uint_least64_t const n{::fast_io::to<::std::uint_least64_t>(os_c_str(argv[1]))};
	for(::std::uint_least64_t i{};i!=n;++i)
	{
		bool const is_boy{dis(engine)};
		if(is_boy)
		{
			++boys;
		}
		else
		{
			++girls;
		}
		println(obf,u8"Round ",i,u8": Is ",cond(is_boy,u8"boy",u8"girl"), u8"\n"
			u8"Total boys:",boys,u8"\n"
			u8"Total girls:",girls);
		
		if(girls!=0)
		{
			println(obf,u8"Sex Ratio:",static_cast<double>(boys)/girls);
		}
		print(obf,u8"Male Ratio in Entire Population:",static_cast<double>(boys)/(boys+girls),u8"\n\n");
	}
}
