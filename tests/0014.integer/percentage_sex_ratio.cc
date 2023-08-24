#include<fast_io.h>

int main()
{
	using namespace ::fast_io::mnp;
	::fast_io::io::println(

		"UINT64_MAX/0u=",percentage_sex_ratio(UINT64_MAX,0u),"\n"
		"UINT64_MAX/0u=",percentage_sex_ratio<true>(UINT64_MAX,0u),"\n"
		"UINT64_MAX/1u=",percentage_sex_ratio(UINT64_MAX,1u),"\n"
		"UINT64_MAX/2u=",percentage_sex_ratio(UINT64_MAX,2u),"\n"
		"UINT64_MAX/3u=",percentage_sex_ratio(UINT64_MAX,3u),"\n"
		"UINT64_MAX/99u=",percentage_sex_ratio(UINT64_MAX,99u),"\n"
		"99995u/100000u=",percentage_sex_ratio(99995u,100000u),"\n"
		"99985u/100000u=",percentage_sex_ratio(99985u,100000u),"\n"
		"99985001u/100000000u=",percentage_sex_ratio(99985001u,100000000u),"\n"
		"99984999u/100000000u=",percentage_sex_ratio(99984999u,100000000u),"\n"
		"99984999u/-100000000=",percentage_sex_ratio(99984999u,-100000000),"\n"
		"99984999/-100000000=",percentage_sex_ratio(99984999,-100000000),"\n"
		"99985000/-100000000=",percentage_sex_ratio(99985000,-100000000),"\n"
		"-99985000/-100000000=",percentage_sex_ratio(-99985000,-100000000),"\n"
		"99985000/-100000000=",percentage_sex_ratio(99985000,-100000000),"\n"
		"850308345/971424125=",percentage_sex_ratio(850308345,971424125),"\n"
		"comma: -99985000/-100000000=",comma_percentage_sex_ratio(-99985000,-100000000),"\n"
		"99985000/100000000=",percentage_sex_ratio<false,false,true>(99985000,100000000),"\n"
		"99985000/100000000=",percentage_sex_ratio<false,true,true>(99985000,100000000),"\n"
		"99985000/100000000=",comma_base_percentage_sex_ratio<16,false,true,true,true>(99985000,100000000),"\n"
		"INT64_MIN/INT64_MAX=",percentage_sex_ratio(INT64_MIN,INT64_MAX),"\n"
		"INT64_MIN/UINT64_MAX=",percentage_sex_ratio(INT64_MIN,UINT64_MAX)
		);
}

