#include<string>
#include<fast_io.h>

int main()
{
	fast_io::ipv6 v6{{0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF},0xFFFF};
	std::string str = fast_io::concat(v6);
	println(v6,"\n",str,"\n",fast_io::to<fast_io::ipv6>(str));
}

