#include<fast_io.h>

int main()
{
	print("Hello World\n");
	println("Hello World");
	print(fast_io::out(),"Hello World\n");
	println(fast_io::out(),"Hello World");
	print(fast_io::u8out(),u8"Hello World\n");
	println(fast_io::u8out(),u8"Hello World");

	perr("Hello World\n");
	perrln("Hello World");
	perr(fast_io::out(),"Hello World\n");
	perrln(fast_io::out(),"Hello World");
	perr(fast_io::u8out(),u8"Hello World\n");
	perrln(fast_io::u8out(),u8"Hello World");

#ifndef NDEBUG
	debug_print("Hello World\n");
	debug_println("Hello World");
	debug_print(fast_io::out(),"Hello World\n");
	debug_println(fast_io::out(),"Hello World");
	debug_print(fast_io::u8out(),u8"Hello World\n");
	debug_println(fast_io::u8out(),u8"Hello World");

	debug_perr("Hello World\n");
	debug_perrln("Hello World");
	debug_perr(fast_io::out(),"Hello World\n");
	debug_perrln(fast_io::out(),"Hello World");
	debug_perr(fast_io::u8out(),u8"Hello World\n");
	debug_perrln(fast_io::u8out(),u8"Hello World");
#endif

}
