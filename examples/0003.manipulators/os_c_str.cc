#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	using namespace fast_io::mnp;
	constexpr char const* ptr{"Hello\0World\n"};
	println("Hello\0World\n",
		ptr,"\n",
		os_c_str(ptr),"\n",
		os_c_str(ptr,4),"\n",
		os_c_str(ptr,10));
}

/*
$ g++ -v
Using built-in specs.
COLLECT_GCC=g++
COLLECT_LTO_WRAPPER=/usr/local/libexec/gcc/x86_64-pc-linux-gnu/13.0.0/lto-wrapper
Target: x86_64-pc-linux-gnu
Configured with: ../../../../gcc/configure --disable-nls --disable-werror --enable-languages=c,c++ --enable-multilib --disable-bootstrap --disable-libstdcxx-verbose --with-gxx-libcxx-include-dir=/usr/local/include/c++/v1 --with-pkgversion=cqwrteur --with-multilib-list=m64,m32,mx32
Thread model: posix
Supported LTO compression algorithms: zlib
gcc version 13.0.0 20221006 (experimental) (cqwrteur) 
$ g++ -o os_c_str os_c_str.cc -Ofast -std=c++23 -s -flto -march=native -fuse-ld=gold -I../../include
$ ./os_c_str 
HelloWorld
0x0000000000400d60
Hello
Hell
Hello
*/
