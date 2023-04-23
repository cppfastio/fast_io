#include<fast_io.h>

using namespace fast_io::io;

int main()
{
	constexpr
		::fast_io::ipv4 v4{{0xFF,0xFF,0xFF,0xFF},0xFFFF};
	constexpr
		::fast_io::ipv6 v6{{0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF},0xFFFF};
	constexpr
		::fast_io::ip ip0{::fast_io::ipv6{{0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF},0xFFFF}};

	constexpr
		::fast_io::ip ip1{::fast_io::ipv6{{0xFFFF,0xFFFF,0,0,0xFFFF,0xFFFF,0xFFFF,0xFFFF},0xFFFF}};

	constexpr
		::fast_io::ip ip2{::fast_io::ipv6{{0xFFFF,0xFFFF,0,0,0,0xFFFF,0,0},0xFFFF}};

	constexpr
		::fast_io::ip ip3{::fast_io::ipv6{{0xFFFF,0xFFFF,0,0,0,0,0,0},0xFFFF}};

	constexpr
		::fast_io::ip ip4{::fast_io::ipv6{{0,0,0,0,0xFFFF,0xFFFF,0,0},0xFFFF}};

	constexpr
		::fast_io::ip ip5{::fast_io::ipv6{{0,0,0,0,0,0,0xFFFF,0xFFFF},0xFFFF}};

	constexpr
		::fast_io::ip ip6{::fast_io::ipv6{{},0xFFFF}};

	println("v4:",v4,"\n"
		"v6:",v6,"\n"
		"ip0:",ip0,"\n"
		"ip1:",ip1,"\n"
		"ip2:",ip2,"\n"
		"ip3:",ip3,"\n"
		"ip4:",ip4,"\n"
		"ip5:",ip5,"\n"
		"ip6:",ip6);
}