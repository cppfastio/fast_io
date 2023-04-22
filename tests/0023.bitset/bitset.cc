#ifdef TEST_VERIFY_WITH_COUT
#include<iostream>
#endif
#include<bitset>
#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	constexpr
	 std::bitset<152> b("11011110000011111111111111111111111110110111100000111111111111111111111111101101111000001111111111111111111111111011011110000011111111111111111111111110");
#ifdef TEST_VERIFY_WITH_COUT
	std::cout<<b<<'\n';
#endif
	println(b);

}