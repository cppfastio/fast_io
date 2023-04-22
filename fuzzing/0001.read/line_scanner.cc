#include<string_view>
#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_crypto.h>

using namespace fast_io::io;

int main()
{
	fast_io::net_service serivce;
	fast_io::native_socket_file socket(fast_io::tcp_listen(2000));
	fast_io::sha256_context ctx;//use sha256 to prevent zero-copy transmission
	fast_io::native_file logs(u8"log.txt",fast_io::open_mode::out);
	for(fast_io::u8ibuf_file soc;;)//should work directly on unix like OS
	{
		try
		{
			soc.reopen(fast_io::tcp_accept(socket));
			for(std::u8string_view line:line_scanner(soc))
			{
				println(::fast_io::mnp::u8as_file(ctx),line);
			}
			soc.close();
			println("sha256:",fast_io::mnp::hash_digest(ctx));
		}
		catch(...)
		{
			perrln(logs,std::source_location::current());
		}
	}
	return 0;
}
