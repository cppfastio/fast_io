#include<string>
#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	using namespace fast_io::mnp;
	println(
	fast_io::concatln(middle(hex<true>(42),8,'-'),"\t",addrvw(42)),
	middle(hex<true>(42),8,'-'),"\t",addrvw(42));
}

/*
D:\hg\fast_io\examples\0003.manipulators>g++ -v
Using built-in specs.
COLLECT_GCC=g++
COLLECT_LTO_WRAPPER=d:/x86_64-windows-gnu/x86_64-w64-mingw32/bin/../libexec/gcc/x86_64-w64-mingw32/13.0.0/lto-wrapper.exe
Target: x86_64-w64-mingw32
Configured with: ../../../../gcc/configure --disable-nls --disable-werror --target=x86_64-w64-mingw32 --prefix=/home/cqwrteur/toolchains/x86_64-w64-mingw32/x86_64-w64-mingw32 --disable-sjlj-exceptions --disable-libstdcxx-verbose --enable-languages=c,c++ --with-gxx-libcxx-include-dir=/home/cqwrteur/toolchains/x86_64-w64-mingw32/x86_64-w64-mingw32/include/c++/v1 --host=x86_64-w64-mingw32
Thread model: win32
Supported LTO compression algorithms: zlib
gcc version 13.0.0 20220703 (experimental) (GCC)

D:\hg\fast_io\examples\0003.manipulators>g++ -o 0x2a 0x2a.cc -Ofast -std=c++23 -s -flto -march=native -I../../include

D:\hg\fast_io\examples\0003.manipulators>0x2a
--0x2a--        0x0000002a
--0x2a--        0x0000002a
*/