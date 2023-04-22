#include<fast_io.h>

// Basic functions like print and scan are in namespace fast_io::io
// The namespace is introduced to avoid conflict with std::print
// It's encouraged to directly be 'using'
using namespace fast_io::io;

int main()
{
	print("Hello World\n");
}
/*
This outputs to C's stdout FILE*
*/