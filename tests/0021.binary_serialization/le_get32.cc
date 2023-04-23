#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	std::uint_least32_t u;
	scan(fast_io::mnp::le_get<32>(u));
}
