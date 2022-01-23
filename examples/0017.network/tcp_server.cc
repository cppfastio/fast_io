#include<fast_io.h>
#include<fast_io_device.h>

int main()
{
	fast_io::net_service service;
	fast_io::native_socket_file socket(fast_io::tcp_listen(2000));
	for(fast_io::iobuf_socket_file file;;)
		try
		{
			file.reopen(tcp_accept(socket));
			println(file,utc(fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime)));
			file.close();
		}
		catch(...)
		{
		}
}
