#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	print(fast_io::win32_box_t{}, "Hello World\n");
}
/*
This outputs to file descriptor 1 on non-windows systems. STDOUT_HANDLE on windows or Wine or ReactOS.
*/
