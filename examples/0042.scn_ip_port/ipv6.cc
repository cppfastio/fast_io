#include <fast_io.h>

int main()
{
    ::fast_io::ipv6 v6;
	//auto i = ::fast_io::mnp::ip_scan_generic<::fast_io::mnp::ip_scan_default_flags>(v4);
	::fast_io::scan(::fast_io::mnp::ip_scan_generic<::fast_io::mnp::ip_scan_default_flags>(v6));
	::fast_io::perrln(v6);
}