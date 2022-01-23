#include<fast_io.h>
#include<random>

int main()
{
	fast_io::ibuf_white_hole_engine eng;

	std::uniform_int_distribution<std::uint16_t> u16distr(0,UINT16_MAX);
	std::uniform_int_distribution<std::uint32_t> u32distr(0,UINT32_MAX);
	std::uniform_int_distribution<std::uint64_t> u64distr(0,UINT64_MAX);
	println("posix:",u16distr(eng),"\t",u32distr(eng),"\t",u64distr(eng),"\n"
	"nt:",u16distr(eng),"\t",u32distr(eng),"\t",u64distr(eng),"\n"
	"win32:",u16distr(eng),"\t",u32distr(eng),"\t",u64distr(eng),"\n"
	"com:",u16distr(eng),"\t",u32distr(eng),"\t",u64distr(eng),"\n"
	"parse:",u16distr(eng),"\t",u32distr(eng),"\t",u64distr(eng));
}