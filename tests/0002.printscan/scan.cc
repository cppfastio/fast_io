#include<string_view>
#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	std::string_view str;
	fast_io::ibuffer_view ibv(str);
	std::size_t a;
	if(scan<true>(ibv,a))
		println("not eof:",a);
	else
		print("EOF\n");
}
