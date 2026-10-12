/*
https example: fetch a page over TLS 1.3.
	./https www.google.com
native_tls resolves to the OS/backend TLS for the build -- schannel on
Windows, openssl when its headers are visible, fast_io's own TLS 1.3
client everywhere else. Use fast_io::tls::u8iobuf_tls_socket_file
directly for the fast_io implementation.
*/
#include <fast_io.h>
#include <fast_io_hosted_crypto.h>

int main(int argc, char const **argv)
{
	using namespace ::fast_io::io;
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
		::fast_io::u8cstring_view const host{
			::fast_io::mnp::os_c_str(reinterpret_cast<char8_t const *>(argv[1]))};

		::fast_io::net_service service;
		::fast_io::tls::u8iobuf_native_tls_socket_file tls{
			::fast_io::tcp_connect(::fast_io::to_ip(::fast_io::native_dns_file{::fast_io::mnp::os_c_str(argv[1])}, 443))};
		::fast_io::operations::handshake(tls.handle, host);

		print(tls, u8"GET / HTTP/1.1\r\n"
				   u8"Host:",
			  ::fast_io::mnp::code_cvt(
				  ::fast_io::mnp::string_filters::host(
					  ::fast_io::mnp::os_c_str(host))),
			  u8"\r\n"
			  u8"User-agent:whatever\r\n"
			  u8"Accept-Type:*/*\r\n"
			  u8"Connection:close\r\n\r\n");

		fast_io::u8http_header_buffer buffer;
		scan(tls, buffer);
		std::uint_least64_t content_length{};
		bool chunked{};
		for (auto [key, value] : line_generator(buffer))
		{
			if (::fast_io::u8string_view(key) == ::fast_io::u8string_view(u8"Content-Length"))
			{
				content_length = ::fast_io::u8to<std::uint_least64_t>(value);
			}
			else if (::fast_io::u8string_view(key) == ::fast_io::u8string_view(u8"Transfer-Encoding") &&
					 ::fast_io::u8string_view(value) == ::fast_io::u8string_view(u8"chunked"))
			{
				chunked = true;
			}
		}
		fast_io::u8native_file nf(u8"index.html", fast_io::open_mode::out);
		if (chunked)
		{
			/*
			Transfer-Encoding: chunked
				<hex size> CRLF <data> CRLF ... 0 CRLF CRLF
			mnp::scan_skippers::crlf() consumes the CRLF between the size line and the chunk body;
			the CRLF trailing each body is skipped by hex_get's whitespace skip.
			*/
			for (;;)
			{
				std::uint_least64_t chunk_size{};
				scan(tls, ::fast_io::mnp::hex_get(chunk_size), ::fast_io::mnp::scan_skippers::crlf());
				if (chunk_size == 0)
				{
					break;
				}
				::fast_io::operations::transmit_all_bytes(nf, {}, tls, {}, chunk_size);
			}
		}
		else if (content_length)
		{
			::fast_io::operations::transmit_all_bytes(nf, {}, tls, {}, content_length);
		}
		else
		{
			/* Connection: close -- transmit until the peer's close_notify (eof) */
			::fast_io::operations::transmit_all_bytes(nf, {}, tls, {}, {});
		}
	}
	catch throws(::std::error e)
	{
		perrln(e);
		return 1;
	}
}
