#include <fast_io.h>

int main()
try
{
	::fast_io::operations::transmit_all_bytes(fast_io::c_stdout(), {}, fast_io::c_stdin(), {}, {});
}
catch throws(::std::error e)
{
	using namespace ::fast_io::iomnp;
	perrln(e);
	return 1;
}
