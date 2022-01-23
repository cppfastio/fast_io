#include<fast_io.h>

int main()
{
	fast_io::net_service service;
	fast_io::native_socket_file socket(tcp_connect(fast_io::ipv4{{127,0,0,1},2000}));
	transmit(fast_io::out(),socket);
}