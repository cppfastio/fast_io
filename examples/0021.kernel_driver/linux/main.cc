#include<fast_io.h>
#include<fast_io_driver/linux_kernel.h>

using namespace fast_io::io;

extern "C" int fast_io_dummy_init() noexcept
{
	println(fast_io::u8kpr(),u8"Hello linux kernel from C++ fast_io, i know linus torvalds hates this: ", 20);
	return 0;
}

extern "C" void fast_io_dummy_exit() noexcept
{
	print(fast_io::u8kpr(),u8"Goodby linux kernel from C++ fast_io\n");
}
