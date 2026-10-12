/*
https server example: serve a fixed page over TLS 1.3.
	./https_server cert.pem key.pem [port]
key.pem may be PKCS#8, PKCS#1 (RSA) or raw ed25519 -- see tls/pkey.h.
The listener is the userspace TLS 1.3 implementation (kTLS offloaded on
Linux where SOL_TLS attaches); the peer side can be openssl s_client,
curl, or the fast_io https client.
*/
#include <fast_io.h>
#include <fast_io_hosted_crypto.h>

int main(int argc, char const **argv)
{
	using namespace ::fast_io::io;
	if (argc < 3)
	{
		if (argc == 0)
		{
			return 1;
		}
		perr("Usage: ", ::fast_io::mnp::os_c_str(*argv), " <cert.pem> <key.pem> [client_ca.pem]\n");
		return 1;
	}
	try
	{
		/* cert chain (leaf first) -- reuses the trust-store PEM loader */
		::fast_io::tls::root_store certs;
		::fast_io::tls::details::root_store_load_pem_file(
			__builtin_addressof(certs), ::fast_io::mnp::os_c_str(argv[1]));
		if (certs.sizes.is_empty())
		{
			perrln("no CERTIFICATE blocks in ", ::fast_io::mnp::os_c_str(argv[1]));
			return 1;
		}
		/* private key: first PEM block that decodes */
		::std::byte key_der[8192];
		::std::size_t key_size{};
		{
			::fast_io::native_file_loader kf{::fast_io::mnp::os_c_str(argv[2])};
			char8_t const *cur{reinterpret_cast<char8_t const *>(kf.data())};
			char8_t const *const end{cur + kf.size()};
			char8_t const *label, *b64;
			::std::size_t label_size, b64_size;
			while (key_size == 0 &&
				   ::fast_io::tls::details::pem_next_block(cur, end, label, label_size, b64, b64_size))
			{
				key_size = ::fast_io::tls::details::pem_decode_block(
					key_der, sizeof(key_der), b64, b64_size);
			}
		}
		if (key_size == 0)
		{
			perrln("no decodable private key block in ", ::fast_io::mnp::os_c_str(argv[2]));
			return 1;
		}
		::fast_io::tls::tls_server_config cfg{};
		cfg.certs_der = certs.ptrs.data();
		cfg.cert_sizes = certs.sizes.data();
		cfg.cert_count = certs.sizes.size();
		cfg.private_key_der = key_der;
		cfg.private_key_size = key_size;
		/* resumption: a per-process ticket key seals NewSessionTickets --
		   restart rotates it, so old tickets simply stop decrypting */
		::std::byte ticket_key[16];
		::fast_io::tls::details::tls_fill_random(ticket_key, 16);
		cfg.ticket_key = &ticket_key;
		/* optional client-CA bundle: requires + verifies a client cert */
		::fast_io::tls::root_store client_ca;
		::fast_io::tls::peer_certificates peer_chain;
		if (argc > 3)
		{
			::fast_io::tls::details::root_store_load_pem_file(
				__builtin_addressof(client_ca), ::fast_io::mnp::os_c_str(argv[3]));
			cfg.request_client_cert = true;
			cfg.require_client_cert = true;
			cfg.client_roots_der = client_ca.ptrs.data();
			cfg.client_root_sizes = client_ca.sizes.data();
			cfg.client_root_count = client_ca.sizes.size();
			cfg.peer_out = __builtin_addressof(peer_chain);
		}

		::fast_io::net_service service;
		::fast_io::native_socket_file socket(
			::fast_io::tcp_listen(4433));
		perrln("listening -- connect with: openssl s_client -connect localhost:4433 -tls1_3");
		for (;;)
		{
			try
			{
				::fast_io::tls::u8iobuf_tls_socket_file tls{::fast_io::tcp_accept(socket)};
				::fast_io::operations::handshake(tls.handle, cfg);
				::fast_io::u8http_header_buffer buffer;
				scan(tls, buffer);
				print(tls,
					  u8"HTTP/1.1 200 OK\r\n"
					  u8"Content-Type: text/plain\r\n"
					  u8"Connection: close\r\n\r\n"
					  u8"hello from fast_io tls server\n");
				::fast_io::operations::output_stream_buffer_flush(tls);
				::fast_io::tls::tls_close_notify(::fast_io::operations::io_stream_ref(tls.handle));
			}
			catch throws(::std::error e)
			{
				perrln(e);
			}
		}
	}
	catch throws(::std::error e)
	{
		perrln(e);
		return 1;
	}
}
