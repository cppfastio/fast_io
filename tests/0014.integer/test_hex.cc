#include<fast_io.h>

using namespace fast_io::io;

template<typename U,std::size_t n>
inline void test_type()
{
	print("start\n");
	using namespace fast_io::mnp;
	U u{};
	for(std::size_t i{};i<=n;++i)
	{
		println(i," ",hex(u));
		u<<=4u;
		u|=0xF;
	}
	print("\n");
}

int main()
{
	test_type<std::uint8_t,2>();
	test_type<std::uint16_t,4>();
	test_type<std::uint32_t,8>();
	test_type<std::uint64_t,16>();
#if defined(__SIZEOF_INT128__)
	test_type<__uint128_t,32>();
#endif
}