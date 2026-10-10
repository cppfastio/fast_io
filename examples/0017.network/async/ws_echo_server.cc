/*
 * RFC 6455 websocket echo server on fast_io's async socket layer.
 *
 * Same two-coroutine shape as tcp_echo_server.cc: one accept coroutine
 * detaches a session coroutine per connection. Each session scans the
 * HTTP request head, answers 101 Switching Protocols with the sha1'd
 * Sec-WebSocket-Accept, then runs the frame loop: text/binary echo,
 * ping -> pong, close -> reply + end. Clients MUST mask; an unmasked
 * frame gets a 1002 protocol close.
 *
 * Usage: ./ws_echo_server [port]   (default 2000)
 */
#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_crypto.h>
#include <fast_io_dsal/string.h>
#include <fast_io_dsal/string_view.h>

namespace fi = ::fast_io;
namespace fop = ::fast_io::operations;
using ::fast_io::io::perr;
using ::fast_io::io::perrln;

static bool ascii_ieq(fi::u8string_view a, fi::u8string_view b) noexcept
{
	if (a.size() != b.size())
	{
		return false;
	}
	for (::std::size_t i{}; i != a.size(); ++i)
	{
		auto const x{static_cast<char8_t>(a[i] & ~0x20)}, y{static_cast<char8_t>(b[i] & ~0x20)};
		if (x != y)
		{
			return false;
		}
	}
	return true;
}

/* comma-list header token match — Connection: keep-alive, Upgrade */
static bool has_token(fi::u8string_view list, fi::u8string_view tok) noexcept
{
	for (::std::size_t off{}; off < list.size();)
	{
		::std::size_t comma{list.size()};
		for (::std::size_t i{off}; i != list.size(); ++i)
		{
			if (list[i] == char8_t{','})
			{
				comma = i;
				break;
			}
		}
		auto seg{fi::u8string_view{list.data() + off, comma - off}};
		while (!seg.empty() && (seg.front() == char8_t{' '} || seg.front() == char8_t{'\t'}))
		{
			seg.remove_prefix(1);
		}
		while (!seg.empty() && (seg.back() == char8_t{' '} || seg.back() == char8_t{'\t'}))
		{
			seg.remove_suffix(1);
		}
		if (ascii_ieq(seg, tok))
		{
			return true;
		}
		off = comma == list.size() ? list.size() : comma + 1;
	}
	return false;
}

/* one ws frame out: encoded header + payload in a single scatter op —
 * the server direction is never masked */
static fi::io_async_task<> ws_send_frame(fi::io_async_observer sched,
										 fi::u8iobuf_socket_file &sock, bool fin,
										 fi::websocket_opcode op, ::std::byte const *data,
										 ::std::size_t len) throws
{
	::std::byte hdrbytes[10];
	auto const hn{fi::websocket_encode_frame_header(hdrbytes, fin, op, len)};
	fi::io_scatter_t const sc[]{{hdrbytes, hn}, {data, len}};
	co_await fop::async_scatter_pwrite_all_bytes(sched, {}, sock, sc, 2, {});
}

static fi::io_async_task<> ws_send_close(fi::io_async_observer sched,
										 fi::u8iobuf_socket_file &sock,
										 ::std::uint_least16_t code) throws
{
	::std::byte const reason[2]{static_cast<::std::byte>(code >> 8),
								static_cast<::std::byte>(code)};
	co_await ws_send_frame(sched, sock, true, fi::websocket_opcode::close, reason, 2);
}

/*
 * Session: scan the request head, verify the upgrade tuple, answer 101,
 * then frames forever. Escaping herbceptions (disconnect, malformed
 * frames) destroy the detached frame — nobody to report to.
 */
static fi::io_async_task<> ws_session(fi::io_async_observer sched,
									  fi::u8iobuf_socket_file sock) throws
{
	fi::u8http_header_buffer hdr;
	co_await fi::io::async_scan(sched, {}, sock, hdr);
	auto const m{hdr.request()};
	fi::u8string_view method{m.data(), m.size()};
	auto const c{hdr.code()};
	fi::u8string_view target{c.data(), c.size()};

	fi::u8string_view upgrade{}, connection{}, wskey{}, wsver{};
	for (auto [key, value] : fi::line_generator(hdr))
	{
		fi::u8string_view const k{key.data(), key.size()};
		fi::u8string_view const v{value.data(), value.size()};
		if (ascii_ieq(k, u8"upgrade"))
		{
			upgrade = v;
		}
		else if (ascii_ieq(k, u8"connection"))
		{
			connection = v;
		}
		else if (ascii_ieq(k, u8"sec-websocket-key"))
		{
			wskey = v;
		}
		else if (ascii_ieq(k, u8"sec-websocket-version"))
		{
			wsver = v;
		}
	}

	if (method != fi::u8string_view{u8"GET"} || !ascii_ieq(upgrade, u8"websocket") ||
		!has_token(connection, u8"upgrade") || wskey.empty() ||
		wsver != fi::u8string_view{u8"13"})
	{
		co_await fi::io::async_print(
			sched, {}, sock,
			u8"HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: close\r\n\r\n");
		co_await fop::async_output_stream_flush(sched, {}, sock);
		co_return;
	}

	char8_t accept[fi::websocket_accept_key_size];
	fi::websocket_accept_key(accept, wskey.data(), wskey.size());
	co_await fi::io::async_print(
		sched, {}, sock,
		u8"HTTP/1.1 101 Switching Protocols\r\n"
		u8"Upgrade: websocket\r\n"
		u8"Connection: Upgrade\r\n"
		u8"Sec-WebSocket-Accept: ",
		fi::u8string_view{accept, fi::websocket_accept_key_size}, u8"\r\n\r\n");
	co_await fop::async_output_stream_flush(sched, {}, sock);

	constexpr ::std::size_t max_payload{16u << 20};
	fi::u8string payload;
	for (;;)
	{
		fi::websocket_frame_header fh{};
		co_await fi::io::async_scan(sched, {}, sock,
									fi::mnp::websocket_frame_header_get(fh));
		if (!fh.mask || fh.payload_length > max_payload)
		{
			co_await ws_send_close(
				sched, sock,
				static_cast<::std::uint_least16_t>(!fh.mask ? 1002 : 1009));
			co_return;
		}
		payload.resize(fh.payload_length);
		auto const pdata{reinterpret_cast<::std::byte *>(payload.data())};
		co_await fop::async_pread_all_bytes(sched, {}, sock, pdata, payload.size(), {});
		fi::websocket_apply_mask(pdata, payload.size(), fh.mask_key);
		switch (fh.opcode)
		{
		case fi::websocket_opcode::text:
		case fi::websocket_opcode::binary:
		case fi::websocket_opcode::continuation:
			co_await ws_send_frame(sched, sock, fh.fin, fh.opcode, pdata, payload.size());
			break;
		case fi::websocket_opcode::ping:
			co_await ws_send_frame(sched, sock, true, fi::websocket_opcode::pong, pdata,
								   payload.size());
			break;
		case fi::websocket_opcode::close:
			co_await ws_send_frame(sched, sock, true, fi::websocket_opcode::close, pdata,
								   payload.size());
			co_return;
		default: /* pong — ignore */
			break;
		}
	}
}

static fi::io_async_task<> accept_loop(fi::io_async_observer sched,
									   fi::u8native_socket_io_observer listener) throws
{
	for (;;)
	{
		/* accepted sockets need async capability: no_block marks the
		 * socket overlapped on win32; posix schedulers need nothing */
		ws_session(sched,
				   fi::u8iobuf_socket_file{co_await fop::async_accept(
					   sched, {}, listener, fi::open_mode::no_block)})
			.detach();
	}
}

int main(int argc, char const **argv)
try
{
	::std::uint_least16_t port{2000};
	if (argc > 1)
	{
		try
		{
			auto const plen{__builtin_strlen(argv[1])};
			auto const *p{reinterpret_cast<char8_t const *>(argv[1])};
			fi::basic_ibuffer_view<char8_t> ibv{p, p + plen};
			fi::scan(ibv, port);
		}
		catch throws(::std::error)
		{
			perrln("Usage: ", fi::mnp::os_c_str(*argv), " [port]");
			return 1;
		}
	}
	fi::net_service service;
	fi::io_async_scheduler scheduler{fi::io_async};

	perr("Websocket echo server listening on port ", port, "\n");
	/* the listener binds once — a bind failure is a startup error, not a
	 * session error; the supervisor only rebuilds the accept coroutine
	 * when a mid-run failure kills it */
	fi::u8native_socket_file listener{fi::tcp_listen(port, fi::open_mode::no_block)};
	for (;;)
	{
		try
		{
			auto acceptor{accept_loop(scheduler, listener)};
			acceptor.resume();
			while (!acceptor.done())
			{
				fi::io_async_wait(scheduler);
			}
			acceptor.rethrow_if_error();
		}
		catch throws(::std::error e)
		{
			perrln("acceptor: ", e, " — restarting");
		}
	}
}
catch throws(::std::error e)
{
	perrln(e);
	return 1;
}
