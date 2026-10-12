#include <fast_io.h>
#include <fast_io_device.h>
#include <string_view>

using namespace fast_io::io;

namespace fi = ::fast_io;
namespace fop = ::fast_io::operations;

/*
 * Async HTTP/1.1 GET. The request is print()ed into the socket's output
 * buffer; the input side's tie handling flushes it asynchronously before
 * the first read — no explicit flush needed, same ordering as sync.
 */
static fi::io_async_task<> fetch(fi::io_async_observer sched, fi::u8iobuf_socket_file sock,
								 fi::u8native_file out, char const *host) throws
{
	/* host goes straight into the Host: header — the string_filters
	 * guard validates the hostname grammar and throws before any
	 * byte is written, so a hostile argv string cannot inject headers */
	co_await fi::async_print(sched, {}, sock,
							 u8"GET / HTTP/1.1\r\n"
							 "Host:",
							 ::fast_io::mnp::code_cvt(
								 ::fast_io::mnp::string_filters::host(
									 ::fast_io::mnp::os_c_str(host))),
							 u8"\r\n"
							 "User-agent:whatever\r\n"
							 "Accept-Type:*/*\r\n"
							 "Connection:close\r\n\r\n");
	fi::u8http_header_buffer hdr{};
	co_await fi::async_scan(sched, {}, sock, hdr);
	using namespace std::string_view_literals;
	for (auto [key, value] : line_generator(hdr))
	{
		if (::std::u8string_view(key) == u8"Content-Length"sv)
		{
			auto content_length{::fast_io::u8to<::std::uint_least64_t>(value)};
			co_await fop::async_transmit_all_bytes(sched, {}, out, {}, sock, {},
												   fi::size_t_opt{content_length});
			co_return;
		}
	}
}

int main(int argc, char const **argv)
try
{
	if (argc < 2)
	{
		if (argc == 0)
		{
			return 1;
		}
		fi::perr("Usage: ", ::fast_io::mnp::os_c_str(*argv), " <domain>\n");
		return 1;
	}
	fi::net_service service;
	fi::io_async_scheduler sched{fi::io_async};
	fi::u8iobuf_socket_file sock{fi::tcp_connect(
		fi::to_ip(fi::native_dns_file(::fast_io::mnp::os_c_str(argv[1])), 80),
		fi::open_mode::no_block)};
	fi::u8native_file out{u8"index.html", fi::open_mode::out | fi::open_mode::no_block};

	fi::io_async_task<> t{
		fetch(sched, ::std::move(sock), ::std::move(out), argv[1])};
	t.resume();
	while (!t.done())
	{
		fi::io_async_wait(sched);
	}
	t.rethrow_if_error();
}
catch throws(::std::error e)
{
	fi::perrln(e);
	return 1;
}
