#include<fast_io.h>

int main()
{
	println(fast_io::mnp::cond(true,"hello","world"),"\n",
		fast_io::mnp::cond(false,"hello","world"));
}
