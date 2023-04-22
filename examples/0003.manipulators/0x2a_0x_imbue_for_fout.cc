#include<fast_io.h>
#include<fast_io_i18n.h>
#include<fast_io_legacy.h>
#include<fstream>

using namespace fast_io::io;

int main()
{
	using namespace fast_io::mnp;
	fast_io::native_l10n loc(u8"");
	std::ofstream fout("ofstream.txt");
	fast_io::filebuf_io_observer fiob{fout.rdbuf()};
	println(imbue(loc,fiob),
	middle(hex0x(420000U),11,'-'),"\t",addrvw(420000U),"\n",
	middle(hex0xupper(420000U),11,'-'),"\t",addrvw(420000U),"\n",
	middle(base<36,true>(420000U),12,'-'),"\n",
	24124214.64364,"\t",hexfloat(24124214.64364));
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

D:\hg\fast_io\examples\0003.manipulators>g++ -o 0x2a_0x_imbue_for_fout 0x2a_0x_imbue_for_fout.cc -Ofast -std=c++23 -s -flto -march=native -I../../include

D:\hg\fast_io\examples\0003.manipulators>0x2a_0x_imbue

ofstream.txt:
-0x66,8a0--	0x00,066,8a0
-0X66,8A0--	0x00,066,8a0
-0[36]9,02o-
24,124,214.64364	0x1.701b36a4c5975p+24
*/
