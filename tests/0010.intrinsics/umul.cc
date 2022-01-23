#include<fast_io.h>

int main()
{
	constexpr std::uint_least64_t a{4124214124124112412ULL},b{61241241421441214ULL};
	std::uint_least64_t high{};
	std::uint_least64_t low{::fast_io::details::intrinsics::umul(a,b,high)};
	println("high:",high," low:",low);
}