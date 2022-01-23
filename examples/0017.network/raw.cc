#include<fast_io.h>

int main()
{
	fast_io::net_service service;
	fast_io::native_socket_file sock(fast_io::sock_family::inet,fast_io::sock_type::stream,fast_io::open_mode{},fast_io::sock_protocol::ip);
	constexpr fast_io::posix_sockaddr_in in{.sin_family=to_native_sock_family(fast_io::sock_family::inet),.sin_port=fast_io::big_endian(static_cast<std::uint16_t>(100)),.sin_addr={192,168,0,1}};
	posix_connect(sock,std::addressof(in),sizeof(in));
}
