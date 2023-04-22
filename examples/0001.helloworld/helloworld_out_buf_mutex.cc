#include<fast_io.h>

using namespace fast_io::io;

fast_io::out_buf_type_mutex globalstdout{fast_io::out()};

int main()
{
	print(globalstdout,"Hello World\n");
}
/*
This creates a global object that can avoid CRT. but being thread-safe and full buffer.
*/