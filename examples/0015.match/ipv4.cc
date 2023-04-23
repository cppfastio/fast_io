#include<fast_io.h>
#include<fast_io_device.h>

using namespace fast_io::io;

int main()
{
	char8_t a,b,c,d;
	scan(a,".",b,".",c,".",d);
	println(a,".",b,".",c,".",d);
}