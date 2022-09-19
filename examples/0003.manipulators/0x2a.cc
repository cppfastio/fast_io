#include<string>
#include<fast_io.h>

int main()
{
	using namespace fast_io::mnp;
	println(
	fast_io::concatln(middle(hex<true>(42),8,'-'),"\t",addrvw(42)),
	middle(hex<true>(42),8,'-'),"\t",addrvw(42));
}
