#include<complex>
#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	std::complex<double> c(3.46e2,32424.436e-40);
	println(c,"\n",fast_io::mnp::hexfloat(c));
}