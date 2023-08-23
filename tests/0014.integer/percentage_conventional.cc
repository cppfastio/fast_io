#include<fast_io.h>

int main()
{
	using namespace ::fast_io::mnp;
	::fast_io::io::println(
		"UINT64_MAX/0u=",percentage_conventional(UINT64_MAX,0u),"\n"
		"UINT64_MAX/0u=",percentage_conventional<true>(UINT64_MAX,0u),"\n"
		"UINT64_MAX/1u=",percentage_conventional(UINT64_MAX,1u),"\n"
		"UINT64_MAX/2u=",percentage_conventional(UINT64_MAX,2u),"\n"
		"UINT64_MAX/3u=",percentage_conventional(UINT64_MAX,3u),"\n"
		"UINT64_MAX/99u=",percentage_conventional(UINT64_MAX,99u),"\n"
		"99995u/100000u=",percentage_conventional(99995u,100000u),"\n"
		"99985u/100000u=",percentage_conventional(99985u,100000u),"\n"
		"99985001u/100000000u=",percentage_conventional(99985001u,100000000u),"\n"
		"99984999u/100000000u=",percentage_conventional(99984999u,100000000u),"\n"
		"99984999u/-100000000=",percentage_conventional(99984999u,-100000000),"\n"
		"99984999/-100000000=",percentage_conventional(99984999,-100000000),"\n"
		"99985000/-100000000=",percentage_conventional(99985000,-100000000),"\n"
		"-99985000/-100000000=",percentage_conventional(99985000,-100000000),"\n"
		"-99985000/100000000=",percentage_conventional(99985000,-100000000),"\n"
		"comma: -99985000/-100000000=",comma_percentage_conventional(-99985000,-100000000),"\n"
		"99985000/100000000=",percentage_conventional<false,false,true>(99985000,100000000),"\n"
		"99985000/100000000=",percentage_conventional<false,true,true>(99985000,100000000),"\n"
		"99985000/100000000=",comma_base_percentage_conventional<16,false,true,true,true>(99985000,100000000),"\n"
		);
}
