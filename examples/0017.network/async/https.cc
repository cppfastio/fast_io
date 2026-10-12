/*
https async example: fetch a page over TLS 1.3.
	./https www.google.com
native_tls is used: schannel on Windows, the fast_io TLS 1.3 client
elsewhere (u8iobuf_native_tls_socket_file). The
handshake is synchronous by design -- it is userspace protocol work,
not a device op, and runs before the scheduler exists. Record-layer
I/O rides io_async: io_uring on Linux (kTLS recvmsg+cmsg reads),
win32_file on Windows.
*/
#include <fast_io.h>
#include <fast_io_hosted_crypto.h>
#include <fast_io_device.h>

static ::fast_io::io_async_task<> fetch(::fast_io::io_async_observer sched,
										::fast_io::u8cstring_view host,
										::fast_io::tls::u8iobuf_native_tls_socket_file tls) throws
{
	co_await ::fast_io::io::async_print(sched, {}, tls,
										u8"GET / HTTP/1.1\r\n"
										u8"Host:",
										host,
										u8"\r\n"
										u8"User-agent:whatever\r\n"
										u8"Accept-Type:*/*\r\n"
										u8"Connection:close\r\n\r\n");

	fast_io::u8http_header_buffer buffer;
	co_await ::fast_io::io::async_scan(sched, {}, tls, buffer);
	std::uint_least64_t content_length{};
	bool has_content_length{}, chunked{};
	for (auto [key, value] : line_generator(buffer))
	{
		if (::fast_io::u8string_view(key) == ::fast_io::u8string_view(u8"Content-Length"))
		{
			content_length = ::fast_io::u8to<std::uint_least64_t>(value);
			has_content_length = true;
		}
		else if (::fast_io::u8string_view(key) == ::fast_io::u8string_view(u8"Transfer-Encoding") &&
				 ::fast_io::u8string_view(value) == ::fast_io::u8string_view(u8"chunked"))
		{
			chunked = true;
		}
	}
	fast_io::u8native_file nf(u8"index.html", fast_io::open_mode::out | fast_io::open_mode::no_block);
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
			co_await ::fast_io::io::async_scan(sched, {}, tls,
											   ::fast_io::mnp::hex_get(chunk_size),
											   ::fast_io::mnp::scan_skippers::crlf());
			if (chunk_size == 0)
			{
				break;
			}
			co_await ::fast_io::operations::async_transmit_all_bytes(sched, {}, nf, {}, tls, {},
																	 chunk_size);
		}
	}
	else
	{
		co_await ::fast_io::operations::async_transmit_all_bytes(sched, {}, nf, {}, tls, {},
				{content_length,has_content_length});
	}
}

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
			::fast_io::mnp::string_filters::host(::fast_io::mnp::os_c_str(reinterpret_cast<char8_t const *>(argv[1])))};

		::fast_io::net_service service;
		::fast_io::tls::u8iobuf_native_tls_socket_file tls{
			::fast_io::tcp_connect(
				::fast_io::to_ip(::fast_io::native_dns_file{host}, 443),
				::fast_io::open_mode::no_block)};
		/* sync handshake before the pump starts -- nothing to block yet */
		::fast_io::operations::handshake(tls.handle, host);

		::fast_io::io_async_scheduler sched{::fast_io::io_async};
		auto task{fetch(sched, host, ::std::move(tls))};
		for (task.resume(); !task.done(); ::fast_io::io_async_wait(sched))
		{
		}
		task.rethrow_if_error();
	}
	catch throws(::std::error e)
	{
		perrln(e);
		return 1;
	}
}
