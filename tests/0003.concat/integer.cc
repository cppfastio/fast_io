#include<string>
#include<fast_io.h>
#include<climits>

int main()
{
	print(fast_io::concat(stdout),"\n",
	fast_io::concatln(stdout),
	fast_io::concatln(nullptr),
	fast_io::concatln(true),
	fast_io::concatln(fast_io::mnp::boolalpha(true)),
	fast_io::concatln(0),
	fast_io::concatln(1000),
	fast_io::concatln(-1000),
	fast_io::concatln(-1000LL),
	fast_io::concatln(SIZE_MAX),
	fast_io::concatln(PTRDIFF_MAX),
	fast_io::concatln(PTRDIFF_MIN),
	fast_io::concatln(UINTMAX_MAX),
	fast_io::concatln(INTMAX_MIN),
	fast_io::concatln(fast_io::mnp::base<36>(INTMAX_MIN)),
	fast_io::concatln(fast_io::mnp::base<36>(INTMAX_MAX)),
	fast_io::concatln(fast_io::mnp::base<36,true,true>(INTMAX_MAX)),
	fast_io::concatln(fast_io::mnp::base<36,true,true>(INTMAX_MAX/100)),
	fast_io::concatln(fast_io::mnp::base<36,true,true>(INTMAX_MIN/100))
	);
}