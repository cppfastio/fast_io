#include<complex>
#include<fast_io.h>

int main()
{
	using namespace fast_io::mnp;
	println(std::complex{std::numeric_limits<double>::infinity(),std::numeric_limits<double>::infinity()},"\n",
		std::complex{std::numeric_limits<double>::min(),std::numeric_limits<double>::min()},"\n",
		std::complex{std::numeric_limits<double>::max(),std::numeric_limits<double>::max()},"\n",
		fixed(std::complex{std::numeric_limits<double>::min(),std::numeric_limits<double>::min()}),"\n",
		fixed(std::complex{std::numeric_limits<double>::max(),std::numeric_limits<double>::max()}),"\n",
		comma_fixed(std::complex{std::numeric_limits<double>::min(),std::numeric_limits<double>::min()}),"\n",
		comma_fixed(std::complex{std::numeric_limits<double>::max(),std::numeric_limits<double>::max()}),"\n",
		general(std::complex{std::numeric_limits<double>::min(),std::numeric_limits<double>::min()}),"\n",
		general(std::complex{std::numeric_limits<double>::max(),std::numeric_limits<double>::max()}),"\n",
		comma_general(std::complex{std::numeric_limits<double>::min(),std::numeric_limits<double>::min()}),"\n",
		comma_general(std::complex{std::numeric_limits<double>::max(),std::numeric_limits<double>::max()}),"\n",
		scientific(std::complex{std::numeric_limits<double>::min(),std::numeric_limits<double>::min()}),"\n",
		scientific(std::complex{std::numeric_limits<double>::max(),std::numeric_limits<double>::max()}),"\n",
		comma_scientific(std::complex{std::numeric_limits<double>::min(),std::numeric_limits<double>::min()}),"\n",
		comma_scientific(std::complex{std::numeric_limits<double>::max(),std::numeric_limits<double>::max()}),"\n",
		hexfloat(std::complex{std::numeric_limits<double>::min(),std::numeric_limits<double>::min()}),"\n",
		hexfloat(std::complex{std::numeric_limits<double>::max(),std::numeric_limits<double>::max()}),"\n",
		comma_hexfloat(std::complex{std::numeric_limits<double>::min(),std::numeric_limits<double>::min()}),"\n",
		comma_hexfloat(std::complex{std::numeric_limits<double>::max(),std::numeric_limits<double>::max()})
		);
}
