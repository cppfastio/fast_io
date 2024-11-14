#include <fast_io.h>

int main(int argc, char **argv)
{

	::fast_io::posix_process p{u8"test", {}, {}, {}};
	auto ec{wait(p)};
	if (ec != 114514)
	{
		::fast_io::fast_terminate();
	}
}
