#include<fast_io.h>

int main()
try
{
	::fast_io::u8wine_file wnf(u8"hello.txt", ::fast_io::open_mode::out);
	print(wnf, u8"hello world\n");
}
catch throws(::std::error e)
{
	::fast_io::perrln(e);
	return 1;
}
