#include<fast_io.h>

int main()
{
	constexpr __uint128_t s{fast_io::to<__uint128_t>("   2424",fast_io::mnp::left(0,30,'0')," sgdjasdopgjdasgpojadsgpoadjsgpoadsjgopjaghdsoighasdioghadsoi")};
	println(s);
}
