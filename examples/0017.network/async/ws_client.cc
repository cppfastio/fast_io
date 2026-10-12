/*
 * RFC 6455 websocket client on fast_io's async socket layer.
 *
 * Connect, run the client upgrade handshake (random 16-byte nonce ->
 * base64 Sec-WebSocket-Key, verify the server's sha1 accept value),
 * then send a masked text frame + a ping and print what comes back.
 * The client direction MUST mask — websocket_encode_frame_header_masked
 * plus websocket_apply_mask on the payload.
 *
 * Usage: ./ws_client [host] [port] [message]
 * defaults: 127.0.0.1 2000 "hello"
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

/* entropy through the platform white-hole (getrandom/RtlGenRandom) */
static void fill_random(::std::byte *out, ::std::size_t n) throws
{
	fi::u8native_white_hole wh;
	fop::read_all_bytes(wh, out, n);
}

/* masked frame out: header (with the key) + xored payload, one op */
static fi::io_async_task<> ws_send_masked(fi::io_async_observer sched,
										  fi::u8iobuf_socket_file &sock, bool fin,
										  fi::websocket_opcode op, ::std::byte *data,
										  ::std::size_t len) throws
{
	::std::byte key[4];
	fill_random(key, sizeof(key));
	::std::byte hdrbytes[14];
	auto const hn{fi::websocket_encode_frame_header_masked(hdrbytes, fin, op, len, key)};
	fi::websocket_apply_mask(data, len, key);
	fi::io_scatter_t const sc[]{{hdrbytes, hn}, {data, len}};
	co_await fop::async_scatter_pwrite_all_bytes(sched, {}, sock, sc, 2, {});
}

/* read one server frame: header via the scan manip, then payload */
static fi::io_async_task<fi::websocket_frame_header>
ws_recv_frame(fi::io_async_observer sched, fi::u8iobuf_socket_file &sock,
			  fi::u8string &payload) throws
{
	fi::websocket_frame_header fh{};
	co_await fi::io::async_scan(sched, {}, sock, fi::mnp::websocket_frame_header_get(fh));
	payload.resize(fh.payload_length);
	if (fh.payload_length != 0)
	{
		co_await fop::async_pread_all_bytes(sched, {}, sock,
											reinterpret_cast<::std::byte *>(payload.data()),
											payload.size(), {});
	}
	co_return fh;
}

static fi::io_async_task<> run(fi::io_async_observer sched, fi::ip const &server,
							   fi::u8string_view host_header, fi::u8string_view message) throws
{
	fi::u8iobuf_socket_file sock{
		fi::u8native_socket_file{fi::tcp_connect(server, fi::open_mode::no_block)}};

	/* --- handshake --- */
	::std::byte nonce[16];
	fill_random(nonce, sizeof(nonce));
	char8_t key[24];
	fi::websocket_b64_encode(key, nonce, sizeof(nonce));
	co_await fi::io::async_print(
		sched, {}, sock, u8"GET / HTTP/1.1\r\nHost: ", host_header,
		u8"\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: ",
		fi::u8string_view{key, sizeof(key)}, u8"\r\nSec-WebSocket-Version: 13\r\n\r\n");
	co_await fop::async_output_stream_flush(sched, {}, sock);

	fi::u8http_header_buffer hdr;
	co_await fi::io::async_scan(sched, {}, sock, hdr);
	auto const c{hdr.code()};
	fi::u8string_view const code{c.data(), c.size()};
	fi::u8string_view accept{};
	for (auto [k, v] : fi::line_generator(hdr))
	{
		fi::u8string_view const name{k.data(), k.size()};
		if (ascii_ieq(name, u8"sec-websocket-accept"))
		{
			accept = fi::u8string_view{v.data(), v.size()};
		}
	}
	char8_t want[fi::websocket_accept_key_size];
	fi::websocket_accept_key(want, key, sizeof(key));
	if (code != fi::u8string_view{u8"101"} ||
		__builtin_memcmp(accept.data(), want, sizeof(want)) != 0)
	{
		perrln("bad handshake");
		co_return;
	}
	perr("upgraded\n");

	/* --- frames --- */
	fi::u8string buf{message};
	co_await ws_send_masked(sched, sock, true, fi::websocket_opcode::text,
							reinterpret_cast<::std::byte *>(buf.data()), buf.size());
	fi::u8string payload;
	{
		auto const fh{co_await ws_recv_frame(sched, sock, payload)};
		perr("echo: ", fh.payload_length, " bytes\n");
	}
	buf = u8"pi";
	co_await ws_send_masked(sched, sock, true, fi::websocket_opcode::ping,
							reinterpret_cast<::std::byte *>(buf.data()), buf.size());
	{
		auto const fh{co_await ws_recv_frame(sched, sock, payload)};
		perr("pong: opcode ", static_cast<int>(fh.opcode), " ", fh.payload_length,
			 " bytes\n");
	}
	co_await ws_send_masked(sched, sock, true, fi::websocket_opcode::close, nullptr, 0);
	{
		auto const fh{co_await ws_recv_frame(sched, sock, payload)};
		perr("closed: opcode ", static_cast<int>(fh.opcode), "\n");
	}
}

int main(int argc, char const **argv)
try
{
	char const *host{argc > 1 ? argv[1] : "127.0.0.1"};
	::std::uint_least16_t port{2000};
	if (argc > 2)
	{
		auto const plen{__builtin_strlen(argv[2])};
		auto const *p{reinterpret_cast<char8_t const *>(argv[2])};
		fi::basic_ibuffer_view<char8_t> ibv{p, p + plen};
		fi::scan(ibv, port);
	}
	char const *msg{argc > 3 ? argv[3] : "hello"};

	fi::net_service service;
	fi::io_async_scheduler sched{fi::io_async};

	fi::ip server{};
	bool resolved{};
	fi::native_dns_file dns{fi::mnp::os_c_str(host)};
	for (auto const ent : dns)
	{
		server = fi::to_ip(ent, port);
		resolved = true;
		break;
	}
	if (!resolved)
	{
		perrln("cannot resolve ", fi::mnp::os_c_str(host));
		return 1;
	}

	auto const hlen{__builtin_strlen(host)}, mlen{__builtin_strlen(msg)};
	auto const *h{reinterpret_cast<char8_t const *>(host)};
	auto const *m{reinterpret_cast<char8_t const *>(msg)};
	auto task{run(sched, server, fi::u8string_view{h, hlen}, fi::u8string_view{m, mlen})};
	for (task.resume(); !task.done(); fi::io_async_wait(sched))
	{
	}
	task.rethrow_if_error();
}
catch throws(::std::error e)
{
	perrln(e);
	return 1;
}
