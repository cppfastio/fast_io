//should compile with -fsanitize=address -fsanitize=undefined to verify code
#include<fast_io.h>

int main()
{
	using namespace fast_io::mnp;
	constexpr double zero{};
	constexpr double minus_zero{-zero};
	println(zero,"\n",
		minus_zero,"\n",
		fixed(zero),"\n",
		fixed(minus_zero),"\n",
		comma_fixed(zero),"\n",
		comma_fixed(minus_zero),"\n",
		general(zero),"\n",
		general(minus_zero),"\n",
		comma_general(zero),"\n",
		comma_general(minus_zero),"\n",
		scientific(zero),"\n",
		scientific(minus_zero),"\n",
		comma_scientific(zero),"\n",
		comma_scientific(minus_zero),"\n",
		hexfloat(zero),"\n",
		hexfloat(minus_zero),"\n",
		comma_hexfloat(zero),"\n",
		comma_hexfloat(minus_zero)
		);
}
