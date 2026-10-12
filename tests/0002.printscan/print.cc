#include <fast_io.h>

using namespace fast_io::io;

int main()
{
	print("Hello World\n");
	print("Hello World\n");
	print(fast_io::out(), "Hello World\n");
	print(fast_io::out(), "Hello World\n");
	print(fast_io::u8out(), u8"Hello World\n");
	print(fast_io::u8out(), u8"Hello World\n");

	perr("Hello World\n");
	perr("Hello World\n");
	perr(fast_io::out(), "Hello World\n");
	perr(fast_io::out(), "Hello World\n");
	perr(fast_io::u8out(), u8"Hello World\n");
	perr(fast_io::u8out(), u8"Hello World\n");

#ifndef NDEBUG
	debug_print("Hello World\n");
	debug_print("Hello World\n");
	debug_print(fast_io::out(), "Hello World\n");
	debug_print(fast_io::out(), "Hello World\n");
	debug_print(fast_io::u8out(), u8"Hello World\n");
	debug_print(fast_io::u8out(), u8"Hello World\n");

	debug_perr("Hello World\n");
	debug_perr("Hello World\n");
	debug_perr(fast_io::out(), "Hello World\n");
	debug_perr(fast_io::out(), "Hello World\n");
	debug_perr(fast_io::u8out(), u8"Hello World\n");
	debug_perr(fast_io::u8out(), u8"Hello World\n");
#endif
}
