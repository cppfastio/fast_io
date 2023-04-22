#include<fast_io.h>
#include<fast_io_device.h>

using namespace fast_io::io;

int main()
{
	fast_io::net_service serivce;
	fast_io::native_socket_file socket(fast_io::tcp_listen(2000));
	fast_io::obuf_file null_device(u8"/dev/null");
	fast_io::native_file logs(u8"log.txt",fast_io::open_mode::out);
	for(fast_io::iobuf_socket_file soc;;)
	{
		try
		{
			soc.reopen(fast_io::tcp_accept(socket));
			for(std::size_t sz;scan<true>(soc,::fast_io::mnp::le_get<64>(sz)););
			soc.close();
		}
		catch(...)
		{
			perrln(logs,std::source_location::current());
		}
	}
	return 0;
}