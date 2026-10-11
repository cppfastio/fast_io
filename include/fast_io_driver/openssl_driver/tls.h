#pragma once

/*
OpenSSL-backed TLS client: an SSL* driven through a pair of memory
BIOs so it works over any fast_io byte stream (posix fd, win32 socket,
or anything else satisfying the _bytes ops).

This is a driver header: include it explicitly when you want the
OpenSSL backend. It is never selected automatically -- OpenSSL's
record layer is a blocking-call API that cannot compose with io_uring
submission or kTLS offload, and it needs -lssl -lcrypto at link time.
*/

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "../../fast_io_dsal/impl/misc/push_macros.h"

namespace fast_io::tls
{

template <typename socket_observer_type = ::fast_io::native_socket_io_observer>
struct basic_ossl_tls_client;

namespace details
{

template <typename socket_observer_type>
inline void ossl_tls_free(basic_ossl_tls_client<socket_observer_type> *client) noexcept;

}

/*
the client aggregate: transport observer + SSL handle + its owning
CTX. Plain public data, same shape as basic_tls_client.
*/
template <typename socket_observer_type>
struct basic_ossl_tls_client
{
	using socket_observer = socket_observer_type;

	socket_observer_type sock_{};
	::SSL_CTX *ctx_{};
	::SSL *ssl_{};
	::BIO *rbio_{}; /* ssl reads ciphertext from here (we feed it) */
	::BIO *wbio_{}; /* ssl writes ciphertext here (we drain it) */
	bool established_{};

	inline constexpr basic_ossl_tls_client() noexcept = default;
	inline explicit constexpr basic_ossl_tls_client(socket_observer_type sock) noexcept
		: sock_{sock}
	{
	}
	basic_ossl_tls_client(basic_ossl_tls_client const &) = delete;
	basic_ossl_tls_client &operator=(basic_ossl_tls_client const &) = delete;
	inline constexpr basic_ossl_tls_client(basic_ossl_tls_client &&other) noexcept
		: sock_{other.sock_}, ctx_{other.ctx_}, ssl_{other.ssl_},
		  rbio_{other.rbio_}, wbio_{other.wbio_}, established_{other.established_}
	{
		other.ssl_ = nullptr;
		other.ctx_ = nullptr;
		other.rbio_ = nullptr;
		other.wbio_ = nullptr;
		other.established_ = false;
	}
	inline basic_ossl_tls_client &operator=(basic_ossl_tls_client &&other) noexcept
	{
		if (__builtin_addressof(other) == this)
		{
			return *this;
		}
		details::ossl_tls_free(this);
		sock_ = other.sock_;
		ctx_ = other.ctx_;
		ssl_ = other.ssl_;
		rbio_ = other.rbio_;
		wbio_ = other.wbio_;
		established_ = other.established_;
		other.ssl_ = nullptr;
		other.ctx_ = nullptr;
		other.rbio_ = nullptr;
		other.wbio_ = nullptr;
		other.established_ = false;
		return *this;
	}
	inline ~basic_ossl_tls_client()
	{
		details::ossl_tls_free(this);
	}
};

namespace details
{

/* drain the earliest OpenSSL queue entry and map it: fatal-alert
   reasons pack the wire description at SSL_AD_REASON_OFFSET +
   description, ERR_LIB_SYS carries a raw errno, anything else degrades
   to alert internal_error */
[[noreturn]] inline void ossl_tls_throw() FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(__HERBCEPTIONS__)
	unsigned long const code{ERR_get_error()};
	int const reason{ERR_GET_REASON(code)};
	if (reason > SSL_AD_REASON_OFFSET)
	{
		throw throws::std::tls_alert{
			::std::tls_alert::alert_level::fatal,
			static_cast<::std::tls_alert::alert_description>(reason - SSL_AD_REASON_OFFSET)};
	}
	switch (reason)
	{
	case SSL_R_CERTIFICATE_VERIFY_FAILED:
		throw throws::std::tls_alert{::std::tls_alert::alert_level::fatal,
									 ::std::tls_alert::alert_description::bad_certificate};
	case SSL_R_UNEXPECTED_EOF_WHILE_READING:
	case SSL_R_UNEXPECTED_MESSAGE:
		throw throws::std::tls_alert{::std::tls_alert::alert_level::fatal,
									 ::std::tls_alert::alert_description::unexpected_message};
	default:
		break;
	}
	if (ERR_GET_LIB(code) == ERR_LIB_SYS && reason > 0)
	{
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
		/* the queue carries GetLastError/WSA codes here, not errno */
		::fast_io::herbceptions::throws_win32_errc_with_value(
			static_cast<::std::uint_least32_t>(reason));
#else
		::fast_io::herbceptions::throws_errc_with_value(reason);
#endif
	}
	throw throws::std::tls_alert{::std::tls_alert::alert_level::fatal,
								 ::std::tls_alert::alert_description::internal_error};
#else
	::fast_io::fast_terminate();
#endif
}

/* SSL_free frees the attached BIOs too */
template <typename socket_observer_type>
inline void ossl_tls_free(basic_ossl_tls_client<socket_observer_type> *client) noexcept
{
	if (client->ssl_ != nullptr)
	{
		SSL_free(client->ssl_);
		client->ssl_ = nullptr;
	}
	if (client->ctx_ != nullptr)
	{
		SSL_CTX_free(client->ctx_);
		client->ctx_ = nullptr;
	}
	client->rbio_ = nullptr;
	client->wbio_ = nullptr;
}

/* drain every ciphertext octet openssl produced into the transport */
template <typename socket_observer_type>
inline void ossl_tls_flush_wbio(basic_ossl_tls_client<socket_observer_type> *client) FAST_IO_HERBCEPTIONS_THROWS
{
	for (;;)
	{
		char buf[16384];
		int const n{BIO_read(client->wbio_, buf, sizeof(buf))};
		if (n <= 0)
		{
			return;
		}
		tls_write_full(client->sock_, reinterpret_cast<::std::byte const *>(buf),
					   static_cast<::std::size_t>(n));
	}
}

/* run one SSL_connect step; pump ciphertext both ways until it
   completes or fails */
template <typename socket_observer_type>
inline void ossl_tls_drive(basic_ossl_tls_client<socket_observer_type> *client, int ret) FAST_IO_HERBCEPTIONS_THROWS
{
	for (;;)
	{
		ossl_tls_flush_wbio(client);
		if (ret == 1)
		{
			return;
		}
		int const err{SSL_get_error(client->ssl_, ret)};
		if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE)
		{
			ossl_tls_throw();
		}
		::std::byte buf[16384];
		auto const e{::fast_io::operations::read_some_bytes(client->sock_, buf, sizeof(buf))};
		if (e == buf)
		{
			::fast_io::throw_posix_error(EPIPE);
		}
		if (BIO_write(client->rbio_, buf, static_cast<int>(e - buf)) <= 0)
		{
			ossl_tls_throw();
		}
		ret = SSL_connect(client->ssl_);
	}
}

template <typename socket_observer_type>
inline void tls_client_handshake(basic_ossl_tls_client<socket_observer_type> *client,
								 tls_client_config const *cfg) FAST_IO_HERBCEPTIONS_THROWS
{
	if (client->ssl_ != nullptr)
	{
		return;
	}
	::SSL_CTX *ctx{SSL_CTX_new(TLS_client_method())};
	if (ctx == nullptr)
	{
		ossl_tls_throw();
	}
	SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION);
	SSL_CTX_set_max_proto_version(ctx, TLS1_3_VERSION);
	::SSL *ssl{SSL_new(ctx)};
	if (ssl == nullptr)
	{
		SSL_CTX_free(ctx);
		ossl_tls_throw();
	}
	::BIO *rbio{BIO_new(BIO_s_mem())};
	::BIO *wbio{BIO_new(BIO_s_mem())};
	if (rbio == nullptr || wbio == nullptr)
	{
		BIO_free(rbio);
		BIO_free(wbio);
		SSL_free(ssl);
		SSL_CTX_free(ctx);
		ossl_tls_throw();
	}
	SSL_set_bio(ssl, rbio, wbio);
	client->ctx_ = ctx;
	client->ssl_ = ssl;
	client->rbio_ = rbio;
	client->wbio_ = wbio;

	if (cfg->check_hostname && !cfg->hostname.is_empty())
	{
		char host[256];
		::std::size_t const n{cfg->hostname.size() < sizeof(host) - 1 ? cfg->hostname.size() : sizeof(host) - 1};
		__builtin_memcpy(host, cfg->hostname.data(), n);
		host[n] = 0;
		SSL_set_tlsext_host_name(ssl, host);
		X509_VERIFY_PARAM *vp{SSL_get0_param(ssl)};
		X509_VERIFY_PARAM_set1_host(vp, host, 0);
	}
	if (cfg->check_chain)
	{
		if (SSL_CTX_set_default_verify_paths(ctx) != 1)
		{
			ossl_tls_throw();
		}
		SSL_set_verify(ssl, SSL_VERIFY_PEER, nullptr);
	}
	else
	{
		SSL_set_verify(ssl, SSL_VERIFY_NONE, nullptr);
	}

	int const ret{SSL_connect(ssl)};
	ossl_tls_drive(client, ret);
	client->established_ = true;
}

template <typename socket_observer_type>
inline ::std::size_t tls_client_read_some(basic_ossl_tls_client<socket_observer_type> *client,
										  ::std::byte *buf, ::std::size_t buf_size) FAST_IO_HERBCEPTIONS_THROWS
{
	if (!client->established_)
	{
		return static_cast<::std::size_t>(
			::fast_io::operations::read_some_bytes(client->sock_, buf, buf_size) - buf);
	}
	for (;;)
	{
		::std::size_t got{};
		int const ret{SSL_read_ex(client->ssl_, buf, buf_size, __builtin_addressof(got))};
		if (ret == 1)
		{
			return got;
		}
		int const err{SSL_get_error(client->ssl_, ret)};
		if (err == SSL_ERROR_ZERO_RETURN)
		{
			return 0; /* close_notify received */
		}
		if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE)
		{
			ossl_tls_throw();
		}
		ossl_tls_flush_wbio(client);
		auto const e{::fast_io::operations::read_some_bytes(client->sock_, buf, buf_size)};
		if (e == buf)
		{
			::fast_io::throw_posix_error(EPIPE);
		}
		if (BIO_write(client->rbio_, buf, static_cast<int>(e - buf)) <= 0)
		{
			ossl_tls_throw();
		}
	}
}

template <typename socket_observer_type>
inline ::std::size_t tls_client_write_some(basic_ossl_tls_client<socket_observer_type> *client,
										   ::std::byte const *buf, ::std::size_t buf_size) FAST_IO_HERBCEPTIONS_THROWS
{
	if (!client->established_)
	{
		return static_cast<::std::size_t>(
			::fast_io::operations::write_some_bytes(client->sock_, buf, buf_size) - buf);
	}
	for (;;)
	{
		::std::size_t done{};
		int const ret{SSL_write_ex(client->ssl_, buf, buf_size, __builtin_addressof(done))};
		if (ret == 1)
		{
			ossl_tls_flush_wbio(client);
			return done;
		}
		int const err{SSL_get_error(client->ssl_, ret)};
		if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE)
		{
			ossl_tls_throw();
		}
		ossl_tls_flush_wbio(client);
		::std::byte tmp[16384];
		auto const e{::fast_io::operations::read_some_bytes(client->sock_, tmp, sizeof(tmp))};
		if (e == tmp)
		{
			::fast_io::throw_posix_error(EPIPE);
		}
		if (BIO_write(client->rbio_, tmp, static_cast<int>(e - tmp)) <= 0)
		{
			ossl_tls_throw();
		}
	}
}

template <typename socket_observer_type>
inline void tls_client_send_close_notify(basic_ossl_tls_client<socket_observer_type> *client) noexcept
{
	if (client->ssl_ == nullptr)
	{
		return;
	}
	FAST_IO_HERBCEPTIONS_TRY
	{
		SSL_shutdown(client->ssl_);
		ossl_tls_flush_wbio(client);
	}
	FAST_IO_HERBCEPTIONS_CATCH_ALL
	{
	}
}

} // namespace details

template <::std::integral ch_type, typename socket_observer_type = ::fast_io::native_socket_io_observer>
struct basic_ossl_tls_io_observer
{
	using char_type = ch_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
	using native_handle_type = basic_ossl_tls_client<socket_observer_type> *;
	native_handle_type handle{};

	inline constexpr native_handle_type native_handle() const noexcept
	{
		return handle;
	}
	inline explicit constexpr operator bool() const noexcept
	{
		return handle != nullptr;
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{handle};
		handle = nullptr;
		return temp;
	}
};

template <::std::integral ch_type, typename socket_observer_type>
inline constexpr basic_ossl_tls_io_observer<ch_type, socket_observer_type>
io_stream_ref_define(basic_ossl_tls_io_observer<ch_type, socket_observer_type> other) noexcept
{
	return other;
}

template <::std::integral ch_type, typename socket_observer_type>
inline constexpr basic_ossl_tls_io_observer<char, socket_observer_type>
io_bytes_stream_ref_define(basic_ossl_tls_io_observer<ch_type, socket_observer_type> other) noexcept
{
	return {other.handle};
}

template <::std::integral ch_type, typename socket_observer_type>
inline ::std::byte *read_some_bytes_underflow_define(basic_ossl_tls_io_observer<ch_type, socket_observer_type> tob,
													 ::std::byte *first, ::std::size_t count) FAST_IO_HERBCEPTIONS_THROWS
{
	return first + details::tls_client_read_some(tob.handle, first, count);
}

template <::std::integral ch_type, typename socket_observer_type>
inline ::std::byte const *write_some_bytes_overflow_define(basic_ossl_tls_io_observer<ch_type, socket_observer_type> tob,
														   ::std::byte const *first, ::std::size_t count) FAST_IO_HERBCEPTIONS_THROWS
{
	return first + details::tls_client_write_some(tob.handle, first, count);
}

template <::std::integral ch_type, typename socket_observer_type>
inline void handshake_define(basic_ossl_tls_io_observer<ch_type, socket_observer_type> tob,
							 tls_client_config cfg) FAST_IO_HERBCEPTIONS_THROWS
{
	details::tls_client_handshake(tob.handle, __builtin_addressof(cfg));
}

template <::std::integral ch_type, typename socket_observer_type>
inline void handshake_define(basic_ossl_tls_io_observer<ch_type, socket_observer_type> tob,
							 ::fast_io::u8cstring_view hostname) FAST_IO_HERBCEPTIONS_THROWS
{
	tls_client_config cfg{};
	cfg.hostname = hostname;
	details::tls_client_handshake(tob.handle, __builtin_addressof(cfg));
}

template <::std::integral ch_type, typename socket_observer_type>
inline void tls_close_notify(basic_ossl_tls_io_observer<ch_type, socket_observer_type> tob) noexcept
{
	details::tls_client_send_close_notify(tob.handle);
}

/* owning bundle, same shape as basic_tls: socket member owns the
   transport, client borrows it */
template <typename socket_type, typename allocator_type = ::fast_io::native_global_allocator>
struct basic_ossl_tls
{
	using char_type = typename socket_type::char_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
	using socket_observer_type =
		decltype(::fast_io::io_stream_ref_define(::std::declval<socket_type &>()));
	using client_type = basic_ossl_tls_client<socket_observer_type>;

	socket_type socket;
	client_type client;

	inline constexpr basic_ossl_tls() noexcept
		requires(::std::is_default_constructible_v<socket_type>)
	= default;

	template <typename... Args>
		requires(::std::constructible_from<socket_type, Args...>)
	inline explicit constexpr basic_ossl_tls(Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(socket_type(::std::forward<Args>(args)...))
		: socket(::std::forward<Args>(args)...), client{::fast_io::io_stream_ref_define(socket)}
	{
	}

	basic_ossl_tls(basic_ossl_tls const &) = delete;
	basic_ossl_tls &operator=(basic_ossl_tls const &) = delete;
	inline constexpr basic_ossl_tls(basic_ossl_tls &&other) noexcept
		: socket(::std::move(other.socket)), client(::std::move(other.client))
	{
	}
	inline basic_ossl_tls &operator=(basic_ossl_tls &&other) noexcept
	{
		if (__builtin_addressof(other) == this)
		{
			return *this;
		}
		socket = ::std::move(other.socket);
		client = ::std::move(other.client);
		return *this;
	}
};

template <typename socket_type, typename allocator_type>
inline constexpr basic_ossl_tls_io_observer<typename socket_type::char_type,
											typename basic_ossl_tls<socket_type, allocator_type>::socket_observer_type>
io_stream_ref_define(basic_ossl_tls<socket_type, allocator_type> &t) noexcept
{
	return {__builtin_addressof(t.client)};
}

template <typename socket_type, typename allocator_type>
inline constexpr basic_ossl_tls_io_observer<char,
											typename basic_ossl_tls<socket_type, allocator_type>::socket_observer_type>
io_bytes_stream_ref_define(basic_ossl_tls<socket_type, allocator_type> &t) noexcept
{
	return {__builtin_addressof(t.client)};
}

template <::std::integral ch_type, typename allocator_type = ::fast_io::native_global_allocator>
using basic_ossl_tls_socket_file = basic_ossl_tls<basic_native_socket_file<ch_type>, allocator_type>;

using ossl_tls_socket_file = basic_ossl_tls_socket_file<char>;
using u8ossl_tls_socket_file = basic_ossl_tls_socket_file<char8_t>;

template <::std::integral ch_type, typename allocator_type = ::fast_io::native_global_allocator>
using basic_iobuf_ossl_tls_socket_file =
	basic_iobuf<basic_ossl_tls_socket_file<ch_type, allocator_type>, allocator_type>;

using iobuf_ossl_tls_socket_file = basic_iobuf_ossl_tls_socket_file<char>;
using u8iobuf_ossl_tls_socket_file = basic_iobuf_ossl_tls_socket_file<char8_t>;

} // namespace fast_io::tls

#include "../../fast_io_dsal/impl/misc/pop_macros.h"
