#include<cstdio>
#include<fast_io.h>

int main()
{
	char buffer[20]{};//must be a string literal
	fast_io::native_file nf(buffer,fast_io::open_mode::out);
}
