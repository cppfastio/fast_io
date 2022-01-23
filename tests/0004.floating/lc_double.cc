//should compile with -fsanitize=address -fsanitize=undefined to verify code
#include<fast_io.h>
#include<fast_io_i18n.h>

int main()
{
	using namespace fast_io::mnp;
	fast_io::l10n l10n("");
	println(imbue(l10n,fast_io::c_stdout()),std::numeric_limits<double>::infinity(),"\n",
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
