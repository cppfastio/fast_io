#include <fast_io.h>

int main()
{
	using namespace ::fast_io::iomnp;
	try
	{
		fast_io::net_service service;
		fast_io::native_socket_file socket(tcp_connect(fast_io::ipv4{{127, 0, 0, 1}, 2000}));
		print(socket, "Hello World\n");
		fast_io::operations::transmit_all_bytes(fast_io::out(), {}, socket, {}, {});
	}
	catch throws(::std::error e)
	{
		perrln(e);
		return 1;
	}
}
