//haven't finished do not print out float
//should compile with -fsanitize=address -fsanitize=undefined to verify code
#include<fast_io.h>
#include<fast_io_unit/floating.h>

int main()
{
	using namespace fast_io::mnp;
	println(std::numeric_limits<float>::infinity(),"\n",
		std::numeric_limits<float>::min(),"\n",
		std::numeric_limits<float>::max(),"\n",
		fixed(std::numeric_limits<float>::min()),"\n",
		fixed(std::numeric_limits<float>::max()),"\n",
		comma_fixed(std::numeric_limits<float>::min()),"\n",
		comma_fixed(std::numeric_limits<float>::max()),"\n",
		general(std::numeric_limits<float>::min()),"\n",
		general(std::numeric_limits<float>::max()),"\n",
		comma_general(std::numeric_limits<float>::min()),"\n",
		comma_general(std::numeric_limits<float>::max()),"\n",
		scientific(std::numeric_limits<float>::min()),"\n",
		scientific(std::numeric_limits<float>::max()),"\n",
		comma_scientific(std::numeric_limits<float>::min()),"\n",
		comma_scientific(std::numeric_limits<float>::max()),"\n",
		hexfloat(std::numeric_limits<float>::min()),"\n",
		hexfloat(std::numeric_limits<float>::max()),"\n",
		comma_hexfloat(std::numeric_limits<float>::min()),"\n",
		comma_hexfloat(std::numeric_limits<float>::max())
		);
}
