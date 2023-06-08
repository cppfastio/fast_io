#include<fast_io.h>
#include<fast_io_device.h>
#include<string_view>

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
	fast_io::u8iobuf_socket_file socket{fast_io::tcp_connect(fast_io::to_ip(fast_io::native_dns_file(::fast_io::mnp::os_c_str(argv[1])),80))};
	print(socket,u8"GET / HTTP/1.1\r\n"
	"Host:",::fast_io::mnp::code_cvt_os_c_str(argv[1]),//this may be dangerous but here we just show the example
	u8"\r\n"
	"User-agent:whatever\r\n"
	"Accept-Type:*/*\r\n"
	"Connection:close\r\n\r\n");
	fast_io::u8http_header_buffer buffer;
	scan(socket,buffer);
	using namespace std::string_view_literals;
	for(auto [key,value]:line_generator(buffer))
	{
		if(std::u8string_view(key)==u8"Content-Length"sv)
		{
			auto content_length{::fast_io::u8to<std::uint_least64_t>(value)};
			fast_io::u8native_file nf(u8"index.html",fast_io::open_mode::out);
			::fast_io::operations::transmit_bytes_some(nf,socket,content_length);
			return 0;
		}
	}
}
