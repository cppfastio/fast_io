//should compile with -fsanitize=address -fsanitize=undefined to verify code
#include<fast_io.h>

int main()
{
	using namespace fast_io::mnp;
	println(std::numeric_limits<double>::infinity(),"\n",
		std::numeric_limits<double>::min(),"\n",
		std::numeric_limits<double>::max(),"\n",
		fixed(std::numeric_limits<double>::min()),"\n",
		fixed(std::numeric_limits<double>::max()),"\n",
		comma_fixed(std::numeric_limits<double>::min()),"\n",
		comma_fixed(std::numeric_limits<double>::max()),"\n",
		general(std::numeric_limits<double>::min()),"\n",
		general(std::numeric_limits<double>::max()),"\n",
		comma_general(std::numeric_limits<double>::min()),"\n",
		comma_general(std::numeric_limits<double>::max()),"\n",
		scientific(std::numeric_limits<double>::min()),"\n",
		scientific(std::numeric_limits<double>::max()),"\n",
		comma_scientific(std::numeric_limits<double>::min()),"\n",
		comma_scientific(std::numeric_limits<double>::max()),"\n",
		hexfloat(std::numeric_limits<double>::min()),"\n",
		hexfloat(std::numeric_limits<double>::max()),"\n",
		comma_hexfloat(std::numeric_limits<double>::min()),"\n",
		comma_hexfloat(std::numeric_limits<double>::max())
		);
}
