#include<fast_io.h>

using namespace fast_io::io;

int main(int argc, char const** argv)
{
	if(argc < 2)
	{
		if(argc == 0)
		{
			return 1;
		}
		perr("Usage: ",::fast_io::mnp::os_c_str(*argv)," <domain>\n");
		return 1;
	}
	fast_io::net_service service;
	fast_io::native_dns_file dnsfile(::fast_io::mnp::os_c_str(argv[1]));
	for(auto ent : dnsfile)
	{
		println(to_ip_address(ent));
	}
}
