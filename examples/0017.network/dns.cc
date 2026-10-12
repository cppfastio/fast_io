#include <fast_io.h>

using namespace fast_io::io;

int main(int argc, char const **argv)
{
	if (argc < 2)
	{
		if (argc == 0)
		{
			return 1;
		}
		perr("Usage: ", ::fast_io::mnp::os_c_str(*argv), " <domain>\n");
		return 1;
	}
	try
	{
		fast_io::net_service service;
		for (auto ent : ::fast_io::native_dns_file(::fast_io::mnp::string_filters::host(::fast_io::mnp::os_c_str(argv[1]))))
		{
			println(to_ip_address(ent));
		}
	}
	catch throws(::std::error e)
	{
		::fast_io::io::perrln(e);
		return -1;
	}
}
