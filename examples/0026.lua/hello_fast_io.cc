#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	print(fast_io::out(),"Hello Lua World from C++ 23 fast_io library. Thank to wasm2lua!\n");
#if 0
	print("Hello Lua World from C++ 23 fast_io library. Thank to wasm2lua!\n");
/*	
	will cause out of memory access. need to report bugs
luajit: hello_fast_io.lua:1: out of memory access
stack traceback:
        [C]: in function 'assert'
        hello_fast_io.lua:1: in function 'al'
        hello_fast_io.lua:1: in function '_start'
        main.lua:8: in main chunk
        [C]: at 0x5652a18d41d0
*/
#endif
}
