#pragma once

/*
TLS 1.3 client handshake driver over a byte-stream transport.

Sequence:

  [plaintext phase]
	write ClientHello record
	read ServerHello (+ optional plaintext CCS) record(s)
	require supported_versions == 0x0304   -- no downgrade, ever
	x25519 -> handshake secrets
  [encrypted handshake phase]
	read ciphertext records, unwrap in userspace:
	  EncryptedExtensions, [CertificateRequest], Certificate,
	  CertificateVerify, Finished         -- transcript-hashed raw
	verify cert chain + SAN hostname + CV signature + Finished MAC
  [switchover]
	plaintext CCS out; sealed Finished under c_hs traffic key
  [application phase]
	Linux: SOL_TLS offload installs the application traffic keys so the
	kernel owns record AEAD (splice/sendfile capable). Anywhere else the
	userspace record layer keeps serving with the same crypto.
*/

#if defined(__linux__)
#include "ktls.h"
#endif

#include "../../fast_io_dsal/vector.h"
#include "../../fast_io_hosted/white_hole/white_hole.h"

namespace fast_io::tls
{

namespace details
{

/* TLS alert failures are thrown as ::std::tls_alert -- the libherbceptions
   domain whose code packs (level << 8) | description, so the wire value
   is recoverable via herbception_cast or e.code(). */
[[noreturn]] inline void tls_throw_alert(alert_description desc) FAST_IO_HERBCEPTIONS_THROWS
{
	throw throws::std::tls_alert{::std::tls_alert::alert_level::fatal,
								 static_cast<::std::tls_alert::alert_description>(
									 static_cast<::std::uint_least8_t>(desc))};
}

/* rethrow the peer's own alert, preserving its level+description bytes */
[[noreturn]] inline void tls_throw_peer_alert(::std::byte const *alert_body,
											  ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::uint_least8_t level{2}; /* RFC: unknown alerts are fatal */
	::std::uint_least8_t desc{50}; /* decode_error */
	if (n == 2)
	{
		level = static_cast<::std::uint_least8_t>(alert_body[0]);
		desc = static_cast<::std::uint_least8_t>(alert_body[1]);
	}
	throw throws::std::tls_alert{static_cast<::std::tls_alert::alert_level>(level),
								 static_cast<::std::tls_alert::alert_description>(desc)};
}

/* expanded traffic key material for one direction (HKDF output):
   key uses cipher_suite_key_size(suite) bytes, iv all 12 */
struct app_traffic_key_iv
{
	::std::byte key[32]{};
	::std::byte iv[12]{};
};

/* RFC 8446 5.2: TLSCiphertext.length may not exceed 2^14 + 256; the
   full wire record adds the 5-byte header */
inline constexpr ::std::size_t tls_max_ciphertext{(1u << 14u) + 256u};
inline constexpr ::std::size_t tls_max_record{5 + tls_max_ciphertext};

/* decode a record header's payload length; ~size_t{0} on malformed
   (empty or over the TLS cap) */
inline constexpr ::std::size_t tls_record_payload_size(::std::byte const *hdr) noexcept
{
	::std::size_t const clen{
		(static_cast<::std::size_t>(static_cast<::std::uint_least8_t>(hdr[3])) << 8) |
		static_cast<::std::size_t>(static_cast<::std::uint_least8_t>(hdr[4]))};
	if (clen == 0 || clen > tls_max_ciphertext)
	{
		return ~static_cast<::std::size_t>(0);
	}
	return clen;
}

/* frame one buffered ciphertext record: returns its total wire size
   (header + payload) when complete, 0 when more bytes are needed, and
   ~size_t{0} when the length field is malformed. Shared by the sync sw
   read path and every async accumulator. */
inline constexpr ::std::size_t tls_record_frame(::std::byte const *ct,
												::std::size_t have) noexcept
{
	if (have < 5)
	{
		return 0;
	}
	::std::size_t const clen{tls_record_payload_size(ct)};
	if (clen == ~static_cast<::std::size_t>(0))
	{
		return clen;
	}
	return have >= 5 + clen ? 5 + clen : 0;
}

/* remaining bytes of the record under framing: bounds a wire read to
   the record's own end. An accumulator that swallowed the head of the
   NEXT record would lose it when the op completes -- a fresh op starts
   with an empty accumulator, so the wire position must sit on a record
   boundary at every completion */
inline constexpr ::std::size_t tls_record_read_need(::std::byte const *ct,
													::std::size_t ct_have) noexcept
{
	if (ct_have < record_header_size)
	{
		return record_header_size - ct_have;
	}
	::std::size_t const clen{tls_record_payload_size(ct)};
	return clen == ~static_cast<::std::size_t>(0)
			   ? ::std::size_t{0}
			   : record_header_size + clen - ct_have;
}

/* constant-time-ish all-zero test -- the X25519 all-zero shared-secret
   check (rfc8446 4.2.8.1) */
inline constexpr bool tls_all_zero(::std::byte const *p, ::std::size_t n) noexcept
{
	::std::byte acc{};
	for (::std::size_t i{}; i != n; ++i)
	{
		acc |= p[i];
	}
	return acc == ::std::byte{};
}

/* SOL_TLS direction selectors -- used only inside __linux__ offload
   paths, defined unconditionally so the shims compile everywhere */
inline constexpr int tls_tx{1};
inline constexpr int tls_rx{2};

/* transport-neutral whole-buffer I/O for the record layer */

#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
/*
win32 sockets marked no_block at connect stay nonblocking afterwards --
the handshake is a synchronous exchange, so would-block reads/writes
must wait for readiness. WSA events carry it: bind an event for the
direction, WaitForSingleObject until signalled, unbind.
*/
inline constexpr ::std::uint_least32_t tls_wsa_fd_read{0x01u};
inline constexpr ::std::uint_least32_t tls_wsa_fd_write{0x02u};
inline constexpr ::std::uint_least32_t tls_wsa_fd_close{0x20u};
inline constexpr ::std::uint_least32_t tls_wsa_would_block{10035u}; /* WSAEWOULDBLOCK */
inline constexpr ::std::uint_least32_t tls_wsa_infinite{0xFFFFFFFFu};

inline bool tls_would_block(::std::error const &e) noexcept
{
	return e.equivalent(static_cast<::fast_io::freestanding::win32_errc>(tls_wsa_would_block));
}

template <typename stmtype>
inline void tls_wait_for_io(stmtype sock, bool want_write) FAST_IO_HERBCEPTIONS_THROWS
{
	namespace w = ::fast_io::win32;
	::std::size_t ev{w::WSACreateEvent()};
	if (ev == static_cast<::std::size_t>(-1) || ev == 0) [[unlikely]]
	{
		::fast_io::throw_win32_error(static_cast<::std::uint_least32_t>(w::WSAGetLastError()));
	}
	::std::uint_least32_t const mask{
		(want_write ? tls_wsa_fd_write : tls_wsa_fd_read) | tls_wsa_fd_close};
	if (w::WSAEventSelect(sock.hsocket, ev, mask) != 0) [[unlikely]]
	{
		auto const ec{w::WSAGetLastError()};
		w::WSACloseEvent(ev);
		::fast_io::throw_win32_error(static_cast<::std::uint_least32_t>(ec));
	}
	w::WaitForSingleObject(reinterpret_cast<void *>(ev), tls_wsa_infinite);
	w::WSAEventSelect(sock.hsocket, 0, 0);
	w::WSACloseEvent(ev);
}

template <typename stmtype>
inline bool tls_wait_would_block(::std::error const &e, stmtype sock, bool want_write) noexcept
{
	if (!tls_would_block(e))
	{
		return false;
	}
	if constexpr (requires(stmtype s) { s.hsocket; })
	{
		FAST_IO_HERBCEPTIONS_TRY
		{
			tls_wait_for_io(sock, want_write);
			return true;
		}
		FAST_IO_HERBCEPTIONS_CATCH_ALL
		{
		}
	}
	return false;
}
#endif

template <typename stmtype>
inline void tls_read_full(stmtype sock, ::std::byte *buf, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	for (;;)
	{
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
		FAST_IO_HERBCEPTIONS_TRY
		{
#endif
			::fast_io::operations::read_all_bytes(sock, buf, n);
			return;
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
		}
		catch throws(::std::error e)
		{
			if (!tls_wait_would_block(e, sock, false))
			{
				throw throws e;
			}
		}
#endif
	}
}

template <typename stmtype>
inline void tls_write_full(stmtype sock, ::std::byte const *buf, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	for (;;)
	{
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
		FAST_IO_HERBCEPTIONS_TRY
		{
#endif
			::fast_io::operations::write_all_bytes(sock, buf, n);
			return;
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
		}
		catch throws(::std::error e)
		{
			if (!tls_wait_would_block(e, sock, true))
			{
				throw throws e;
			}
		}
#endif
	}
}

template <typename stmtype>
inline ::std::byte const *tls_read_some(stmtype sock, ::std::byte *buf,
										::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	for (;;)
	{
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
		FAST_IO_HERBCEPTIONS_TRY
		{
#endif
			return ::fast_io::operations::read_some_bytes(sock, buf, n);
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
		}
		catch throws(::std::error e)
		{
			if (!tls_wait_would_block(e, sock, false))
			{
				throw throws e;
			}
		}
#endif
	}
}

template <typename stmtype>
inline ::std::byte const *tls_write_some(stmtype sock, ::std::byte const *buf,
										 ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	for (;;)
	{
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
		FAST_IO_HERBCEPTIONS_TRY
		{
#endif
			return ::fast_io::operations::write_some_bytes(sock, buf, n);
#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
		}
		catch throws(::std::error e)
		{
			if (!tls_wait_would_block(e, sock, true))
			{
				throw throws e;
			}
		}
#endif
	}
}
/* entropy through the platform's white_hole device (getrandom /
   RtlGenRandom / ...), not a per-platform hand roll */
inline void tls_fill_random(::std::byte *out, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::u8native_white_hole wh;
	::fast_io::operations::read_all_bytes(wh, out, n);
}

#if defined(__linux__)
/* record offload is only possible where the transport is a linux fd */
template <typename stmtype>
inline constexpr bool tls_transport_is_fd{requires(stmtype s) { s.fd; }};
#endif

/* attach the kernel tls ulp when the platform+transport support it;
   reports whether offload is armed */
template <typename stmtype>
inline bool tls_try_offload(stmtype sock) noexcept
{
#if defined(__linux__)
	if constexpr (tls_transport_is_fd<stmtype>)
	{
		FAST_IO_HERBCEPTIONS_TRY
		{
			ktls_attach(sock.fd);
			return true;
		}
		FAST_IO_HERBCEPTIONS_CATCH_ALL
		{
		}
	}
#endif
	(void)sock;
	return false;
}

/* install one direction's traffic key into the kernel -- reached only
   when offload armed (linux + fd), so the no-fd path is unreachable */
template <typename stmtype>
inline void ktls_offload_key(stmtype sock, ::std::uint_least32_t dir, cipher_suite suite,
							 ::std::byte const *key, ::std::byte const *iv) FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(__linux__)
	if constexpr (tls_transport_is_fd<stmtype>)
	{
		ktls_set_key(sock.fd, static_cast<int>(dir), suite, key, iv, 0);
		return;
	}
#endif
	(void)sock;
	(void)dir;
	(void)suite;
	(void)key;
	(void)iv;
	__builtin_unreachable();
}

/* peer alert -> cxx_std_error on the tls_alert domain; mirrors
   tls_throw_peer_alert's packing ((level << 8) | description) */
inline ::std::cxx_std_error tls_peer_alert_error(::std::byte const *body, ::std::size_t n) noexcept
{
	::std::uint_least8_t level{2}; /* RFC: unknown alerts are fatal */
	::std::uint_least8_t desc{50}; /* decode_error */
	if (n == 2)
	{
		level = static_cast<::std::uint_least8_t>(body[0]);
		desc = static_cast<::std::uint_least8_t>(body[1]);
	}
	return ::fast_io::details::async_make_error(
		::std::tls_alert{static_cast<::std::tls_alert::alert_level>(level),
						 static_cast<::std::tls_alert::alert_description>(desc)});
}

/* locally-raised alert -> cxx_std_error on the tls_alert domain */
inline ::std::cxx_std_error tls_alert_error(::std::uint_least8_t level,
											::std::uint_least8_t desc) noexcept
{
	return ::fast_io::details::async_make_error(
		::std::tls_alert{static_cast<::std::tls_alert::alert_level>(level),
						 static_cast<::std::tls_alert::alert_description>(desc)});
}

} // namespace details

struct tls_client_config
{
	::fast_io::u8cstring_view hostname{};
	/* DER-encoded trust anchors */
	::std::byte const *const *roots{};
	::std::size_t const *root_sizes{};
	::std::size_t root_count{};
	bool check_hostname{true};
	bool check_chain{true};
	/* kernel record offload (kTLS). false -> always the userspace
	   record AEAD; true -> still falls back when the ulp won't attach */
	bool offload{true};
};

/* parsed+stored peer chain */
template <typename allocator_type = ::fast_io::native_global_allocator>
struct basic_peer_certificates
{
	using allocator_handle_type = typename allocator_type::handle_type;
	static inline constexpr bool alloc_with_status{
		allocator_type::has_status};

	::fast_io::vector<::std::byte, allocator_type> storage;
	::std::size_t offsets[16]{};
	::std::size_t sizes[16]{};
	::std::size_t count{};

	inline constexpr basic_peer_certificates() noexcept
		requires(!alloc_with_status)
	= default;
	inline explicit constexpr basic_peer_certificates(allocator_handle_type hdl) noexcept
		requires(alloc_with_status)
		: storage{hdl}
	{
	}
};

using peer_certificates = basic_peer_certificates<>;

/* construct an allocator-parameterized object, binding the handle when
   the allocator carries status and defaulting it otherwise */
template <typename T, typename allocator_type>
inline constexpr T tls_alloc_construct(
	typename allocator_type::handle_type hdl) noexcept
{
	if constexpr (allocator_type::has_status)
	{
		return T{hdl};
	}
	else
	{
		return T{};
	}
}

/*
socket_observer_type is the byte-stream transport the record layer talks
to -- the socket's stream observer (native_socket_io_observer by default:
a posix fd observer on unix, a win32 socket observer on windows). Linux
additionally attempts SOL_TLS offload when the observer is an fd.
*/
/*
the userspace record pump's per-record results -- namespace scope because
the record machinery is plain free functions */
struct sw_record_result
{
	content_type inner{};
	::std::size_t plaintext_size{};
	bool eof{};
};

struct sw_seal_result
{
	::std::size_t wire_size{};
	::std::size_t plaintext_consumed{};
};

template <typename allocator_type = ::fast_io::native_global_allocator,
		  typename socket_observer_type = ::fast_io::native_socket_io_observer,
		  typename crypto = tls_default_crypto>
struct basic_tls_client
{
	using allocator_handle_type = typename allocator_type::handle_type;
	using socket_observer = socket_observer_type;
	static inline constexpr bool alloc_with_status{
		allocator_type::has_status};

	socket_observer_type sock_{};
	allocator_handle_type allocator_handle{};
	cipher_suite suite_{};
	/* current application traffic secrets, needed for KeyUpdate rekey */
	::std::byte tx_secret_[48]{};
	::std::byte rx_secret_[48]{};
	::std::size_t secret_size_{};
	/* application traffic keys/ivs + per-direction record sequence —
	   always kept for KeyUpdate; when offloaded_ is false they drive
	   the userspace record AEAD directly */
	::std::byte tx_key_[32]{}, rx_key_[32]{};
	::std::byte tx_iv_[12]{}, rx_iv_[12]{};
	::std::uint_least64_t tx_seq_{}, rx_seq_{};
	/* decrypted plaintext left over when a record exceeded the caller's
	   buffer -- drained before any new record is read */
	::fast_io::vector<::std::byte, allocator_type> rx_pending_;
	::std::size_t rx_pending_pos_{};
	bool established_{};
	bool offloaded_{};

	inline constexpr basic_tls_client() noexcept = default;
	inline explicit constexpr basic_tls_client(socket_observer_type sock) noexcept
		: sock_{sock},
		  rx_pending_{tls_alloc_construct<::fast_io::vector<::std::byte, allocator_type>,
										  allocator_type>(allocator_handle)}
	{}
	inline constexpr basic_tls_client(socket_observer_type sock,
									  allocator_handle_type hdl) noexcept
		: sock_{sock}, allocator_handle{hdl},
		  rx_pending_{tls_alloc_construct<::fast_io::vector<::std::byte, allocator_type>,
										  allocator_type>(hdl)}
	{}

	basic_tls_client(basic_tls_client const &) = delete;
	basic_tls_client &operator=(basic_tls_client const &) = delete;
	/* move copies the state and wipes the source's secret bytes */
	inline basic_tls_client(basic_tls_client &&other) noexcept
		: sock_{other.sock_}, allocator_handle{other.allocator_handle}, suite_{other.suite_},
		  secret_size_{other.secret_size_}, tx_seq_{other.tx_seq_}, rx_seq_{other.rx_seq_},
		  rx_pending_{::std::move(other.rx_pending_)},
		  rx_pending_pos_{other.rx_pending_pos_},
		  established_{other.established_}, offloaded_{other.offloaded_}
	{
		::fast_io::freestanding::non_overlapped_copy_n(other.tx_secret_, sizeof(other.tx_secret_), tx_secret_);
		::fast_io::freestanding::non_overlapped_copy_n(other.rx_secret_, sizeof(other.rx_secret_), rx_secret_);
		::fast_io::freestanding::non_overlapped_copy_n(other.tx_key_, sizeof(other.tx_key_), tx_key_);
		::fast_io::freestanding::non_overlapped_copy_n(other.rx_key_, sizeof(other.rx_key_), rx_key_);
		::fast_io::freestanding::non_overlapped_copy_n(other.tx_iv_, sizeof(other.tx_iv_), tx_iv_);
		::fast_io::freestanding::non_overlapped_copy_n(other.rx_iv_, sizeof(other.rx_iv_), rx_iv_);
		::fast_io::secure_clear(__builtin_addressof(other), sizeof(other));
	}
	inline basic_tls_client &operator=(basic_tls_client &&other) noexcept
	{
		if (__builtin_addressof(other) == this)
		{
			return *this;
		}
		/* release the old pending storage before the wipe -- the wipe
		   would zero its internal pointers and leak the buffer */
		rx_pending_ = {};
		::fast_io::secure_clear(this, sizeof(*this));
		sock_ = other.sock_;
		allocator_handle = other.allocator_handle;
		suite_ = other.suite_;
		secret_size_ = other.secret_size_;
		tx_seq_ = other.tx_seq_;
		rx_seq_ = other.rx_seq_;
		rx_pending_ = ::std::move(other.rx_pending_);
		rx_pending_pos_ = other.rx_pending_pos_;
		established_ = other.established_;
		offloaded_ = other.offloaded_;
		::fast_io::freestanding::non_overlapped_copy_n(other.tx_secret_, sizeof(other.tx_secret_), tx_secret_);
		::fast_io::freestanding::non_overlapped_copy_n(other.rx_secret_, sizeof(other.rx_secret_), rx_secret_);
		::fast_io::freestanding::non_overlapped_copy_n(other.tx_key_, sizeof(other.tx_key_), tx_key_);
		::fast_io::freestanding::non_overlapped_copy_n(other.rx_key_, sizeof(other.rx_key_), rx_key_);
		::fast_io::freestanding::non_overlapped_copy_n(other.tx_iv_, sizeof(other.tx_iv_), tx_iv_);
		::fast_io::freestanding::non_overlapped_copy_n(other.rx_iv_, sizeof(other.rx_iv_), rx_iv_);
		::fast_io::secure_clear(__builtin_addressof(other), sizeof(other));
		return *this;
	}
};

using tls_client = basic_tls_client<>;

namespace details
{

/* declarations for the client free-function machinery below (mutual
   recursion makes ordering insufficient) */
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline void tls_client_handshake(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
								 tls_client_config const *cfg) FAST_IO_HERBCEPTIONS_THROWS;
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline ::std::size_t tls_client_read_some(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
										  ::std::byte *buf, ::std::size_t buf_size) FAST_IO_HERBCEPTIONS_THROWS;
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline ::std::size_t tls_client_write_some(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
										   ::std::byte const *buf, ::std::size_t buf_size) FAST_IO_HERBCEPTIONS_THROWS;
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline ::std::size_t tls_client_sw_read_some(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
											 ::std::byte *buf, ::std::size_t buf_size) FAST_IO_HERBCEPTIONS_THROWS;
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline ::std::size_t tls_client_sw_write_some(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
											  ::std::byte const *buf, ::std::size_t buf_size) FAST_IO_HERBCEPTIONS_THROWS;
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline void tls_client_process_post_handshake(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
											  ::std::byte const *msgs, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS;
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline ::std::size_t tls_client_rx_pending(basic_tls_client<allocator_type, socket_observer_type, crypto> const *client) noexcept;
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline ::std::size_t tls_client_rx_pending_drain(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
												 ::std::byte *buf, ::std::size_t n) noexcept;
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline ::std::size_t tls_client_rx_pending_drain(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
												 ::fast_io::io_scatter_t const *sc, ::std::size_t nsc) noexcept;
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline void tls_client_rx_pending_stash(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
										::std::byte const *pt, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS;
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline sw_record_result tls_client_sw_open_record(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
												  ::std::byte const *rec, ::std::size_t rec_size,
												  ::std::byte *innerbuf) FAST_IO_HERBCEPTIONS_THROWS;
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline sw_seal_result tls_client_sw_seal_appdata(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
												 ::std::byte *out, ::fast_io::io_scatter_t const *pt,
												 ::std::size_t npt) noexcept;
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline ::std::size_t tls_client_sw_seal_alert(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
											  ::std::byte *out, alert_description desc) noexcept;
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline void tls_client_key_update_received(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
										   ::std::uint_least8_t request) FAST_IO_HERBCEPTIONS_THROWS;
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline void tls_client_set_established(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
									   cipher_suite suite,
									   ::std::byte const *tx_secret, ::std::byte const *rx_secret,
									   ::std::size_t secret_size, bool offloaded,
									   app_traffic_key_iv const &tx_key_iv,
									   app_traffic_key_iv const &rx_key_iv) noexcept;
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline void tls_client_send_alert(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
								  alert_description desc) noexcept;
template <typename allocator_type, typename socket_observer_type, typename crypto>
[[noreturn]] inline void tls_client_fail(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
										 alert_description desc) FAST_IO_HERBCEPTIONS_THROWS;
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline void tls_client_send_close_notify(basic_tls_client<allocator_type, socket_observer_type, crypto> *client) noexcept;

} // namespace details

namespace details
{

template <typename stmtype>
inline void tls_send_alert(stmtype sock, alert_description desc, bool tx_offloaded) noexcept
{
	/* best-effort; nothing to do if this fails too */
	::std::byte const level{desc == alert_description::close_notify ? ::std::byte{1} : ::std::byte{2}};
	::std::byte body[2]{level, static_cast<::std::byte>(desc)};
#if defined(__linux__)
	if (tx_offloaded && requires { sock.fd; })
	{
		FAST_IO_HERBCEPTIONS_TRY
		{
			details::ktls_send_record(sock.fd, content_type::alert, body, 2);
		}
		FAST_IO_HERBCEPTIONS_CATCH_ALL
		{
		}
		return;
	}
#else
	(void)tx_offloaded;
#endif
	::std::byte rec[7];
	::std::byte *p{details::record_header_write(rec, content_type::alert, 2)};
	p = wire_put_bytes(p, body, 2);
	FAST_IO_HERBCEPTIONS_TRY
	{
		details::tls_write_full(sock, rec, static_cast<::std::size_t>(p - rec));
	}
	FAST_IO_HERBCEPTIONS_CATCH_ALL
	{
	}
}

template <typename stmtype>
[[noreturn]] inline void tls_fail(stmtype sock, alert_description desc,
								  bool tx_offloaded) FAST_IO_HERBCEPTIONS_THROWS
{
	tls_send_alert(sock, desc, tx_offloaded);
	tls_throw_alert(desc);
}

/*
read one record during the handshake's encrypted flight. The kernel is
NOT involved yet -- openssl also does the hs epoch in userspace and
only offloads the app epoch. Reads hdr+payload off the wire, AEAD-opens
it under (key,iv,seq), and returns the inner type/content. seq counts
encrypted records only and is advanced by the caller.
*/
template <typename crypto, typename stmtype>
inline bool tls_recv_flight_record(stmtype sock, ::std::byte *buf, ::std::size_t buf_cap,
								   content_type *inner_type, ::std::size_t *inner_size,
								   cipher_suite suite, ::std::byte const *key,
								   ::std::byte const *iv, ::std::uint_least64_t seq) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::byte hdr[record_header_size];
	tls_read_full(sock, hdr, record_header_size);
	wire_reader h{hdr, hdr + record_header_size};
	::std::uint_least8_t t;
	::std::uint_least16_t ver, len;
	if (!h.take_u8(t) || !h.take_u16(ver) || !h.take_u16(len))
	{
		return false;
	}
	(void)ver;
	if (len > buf_cap)
	{
		return false;
	}
	tls_read_full(sock, buf, len);
	if (static_cast<content_type>(t) == content_type::application_data)
	{
		if (!crypto::record_open(buf, *inner_size, *inner_type, hdr, buf, len,
								 suite, key, iv, seq))
		{
			return false;
		}
		return true;
	}
	/* plaintext record (ccs / alert) -- not encrypted, seq untouched */
	*inner_type = static_cast<content_type>(t);
	*inner_size = len;
	return true;
}

/*
read one plaintext record (pre-offload phase). Returns payload size;
ctype gets the record type.
*/
template <typename stmtype>
inline ::std::size_t tls_read_plaintext_record(stmtype sock, ::std::byte *buf, ::std::size_t buf_cap,
											   content_type *ctype) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::byte hdr[record_header_size];
	details::tls_read_full(sock, hdr, record_header_size);
	wire_reader h{hdr, hdr + record_header_size};
	::std::uint_least8_t t;
	::std::uint_least16_t ver, len;
	if (!h.take_u8(t) || !h.take_u16(ver) || !h.take_u16(len))
	{
		details::tls_fail(sock, alert_description::decode_error, false);
	}
	(void)ver;
	if (len > buf_cap)
	{
		details::tls_fail(sock, alert_description::record_overflow, false);
	}
	details::tls_read_full(sock, buf, len);
	*ctype = static_cast<content_type>(t);
	return len;
}

/*
handshake message accumulator: handshake octets may straddle records.
feed() appends record plaintext; next() pulls complete [hdr|body]
messages for transcript hashing.
*/
template <typename allocator_type = ::fast_io::native_global_allocator>
struct basic_handshake_queue
{
	using vector_type = ::fast_io::vector<::std::byte, allocator_type>;
	using allocator_handle_type = typename allocator_type::handle_type;
	static inline constexpr bool alloc_with_status{allocator_type::has_status};

	vector_type pending;
	::std::size_t consumed{};

	inline constexpr basic_handshake_queue() noexcept
		requires(!alloc_with_status)
	= default;
	inline explicit constexpr basic_handshake_queue(allocator_handle_type hdl) noexcept
		requires(alloc_with_status)
		: pending{hdl}
	{
	}
};

using handshake_queue = basic_handshake_queue<>;

inline void handshake_queue_feed(handshake_queue *q,
								 ::std::byte const *data, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	if (q->consumed != 0)
	{
		::fast_io::freestanding::overlapped_copy_n(q->pending.data() + q->consumed,
												   q->pending.size() - q->consumed, q->pending.data());
		q->pending.resize(q->pending.size() - q->consumed);
		q->consumed = 0;
	}
	::std::size_t const old{q->pending.size()};
	q->pending.resize(old + n);
	::fast_io::freestanding::non_overlapped_copy_n(data, n, q->pending.data() + old);
	if (q->pending.size() > (1u << 20u)) /* 1MB handshake cap */
	{
		::fast_io::throw_posix_error(EPROTO);
	}
}

inline bool handshake_queue_next(handshake_queue *q,
								 handshake_type *type, ::std::byte const **body, ::std::size_t *body_size,
								 ::std::byte const **raw, ::std::size_t *raw_size) noexcept
{
	::std::byte const *const p{q->pending.data() + q->consumed};
	::std::size_t const avail{q->pending.size() - q->consumed};
	if (avail < handshake_header_size)
	{
		return false;
	}
	wire_reader r{p, p + avail};
	::std::uint_least8_t t;
	::std::uint_least32_t len;
	if (!r.take_u8(t) || !r.take_u24(len))
	{
		return false;
	}
	if (len > (1u << 20u) || avail < handshake_header_size + len)
	{
		return false;
	}
	*type = static_cast<handshake_type>(t);
	*raw = p;
	*raw_size = handshake_header_size + len;
	*body = p + handshake_header_size;
	*body_size = len;
	q->consumed += *raw_size;
	return true;
}

/* Certificate body: u8 request_context || u24 cert_list of
   {u24 der || u16 extensions}. DERs are copied into peer->storage. */
inline bool certificate_body_parse(::fast_io::tls::peer_certificates *peer,
								   ::std::byte const *body, ::std::size_t body_size) noexcept
{
	wire_reader r{body, body + body_size};
	::std::byte const *ctx;
	::std::size_t ctx_size;
	::std::byte const *list;
	::std::size_t list_size;
	if (!r.take_vector8(ctx, ctx_size) || !r.take_vector24(list, list_size) || !r.empty())
	{
		return false;
	}
	wire_reader l{list, list + list_size};
	while (!l.empty())
	{
		::std::byte const *der;
		::std::size_t der_size;
		::std::byte const *ext;
		::std::size_t ext_size;
		if (!l.take_vector24(der, der_size) || !l.take_vector16(ext, ext_size))
		{
			return false;
		}
		if (peer->count == 16 || der_size == 0)
		{
			return false;
		}
		::std::size_t const off{peer->storage.size()};
		peer->storage.resize(off + der_size);
		::fast_io::freestanding::non_overlapped_copy_n(der, der_size, peer->storage.data() + off);
		peer->offsets[peer->count] = off;
		peer->sizes[peer->count] = der_size;
		++peer->count;
	}
	return true;
}

} // namespace details

/*
the encrypted server flight + client second flight. ch_msg/sh_msg are
the raw handshake octets (with 4-byte headers) already exchanged.
tx_secret_out/rx_secret_out receive the application traffic secrets for
later KeyUpdate rekey.

Transcript rule (rfc8446 4.4.1): CertificateVerify's covered content
hashes CH..Certificate; server Finished's verify_data MACs CH..CV. Both
exclude the message carrying them, so the running hash is snapshotted
before each message is folded in.
*/
template <typename crypto, typename stmtype>
inline void ktls_handshake_flight2(stmtype sock, cipher_suite suite,
								   ::std::byte const *shared_secret,
								   ::std::byte const *ch_msg, ::std::size_t ch_msg_size,
								   ::std::byte const *sh_msg, ::std::size_t sh_msg_size,
								   tls_client_config const *cfg,
								   bool offload,
								   details::app_traffic_key_iv *tx_key_iv_out,
								   details::app_traffic_key_iv *rx_key_iv_out,
								   ::std::byte *tx_secret_out, ::std::byte *rx_secret_out,
								   ::std::size_t *secret_size_out) FAST_IO_HERBCEPTIONS_THROWS
{
	typename crypto::md const md{crypto::md_for(suite)};
	::std::size_t const digest_size{crypto::md_digest_size(md)};
	*secret_size_out = digest_size;

	typename crypto::hash_ctx transcript{md};
	transcript.update(ch_msg, ch_msg + ch_msg_size);
	transcript.update(sh_msg, sh_msg + sh_msg_size);

	::fast_io::tls::details::key_schedule<crypto> ks{md};
	ks.init_early();
	ks.derive_empty();
	ks.extract_into(shared_secret, 32); /* handshake secret */

	::std::byte c_hs[64], s_hs[64];
	ks.derive_to_ptr(c_hs, u8"c hs traffic", 12, transcript);
	ks.derive_to_ptr(s_hs, u8"s hs traffic", 12, transcript);

	::std::byte key[32], iv[12];
	::std::size_t const key_size{::fast_io::tls::details::cipher_suite_key_size(suite)};
	::fast_io::tls::details::traffic_key_iv_to_ptr<crypto>(md, key, key_size, iv, s_hs);
	/* userspace AEAD for the handshake epoch: openssl does the same --
	   only the application epoch goes to the kernel */
	::std::byte hs_rx_key[32], hs_rx_iv[12];
	::fast_io::freestanding::non_overlapped_copy_n(key, key_size, hs_rx_key);
	::fast_io::freestanding::non_overlapped_copy_n(iv, 12, hs_rx_iv);
	::fast_io::secure_clear(key, sizeof(key));

	enum : ::std::uint_least8_t
	{
		want_ee,
		want_cert_or_cr,
		want_cv,
		want_fin,
		flight_done
	} state{want_ee};
	bool cert_request_seen{};
	::std::uint_least64_t hs_rx_seq{};
	::std::byte cr_context[255];
	::std::size_t cr_context_size{};
	/* every allocation lives in these concrete locals -- the rest of the
	   flight is allocator-free protocol work */
	peer_certificates peer{};
	details::handshake_queue q{};
	signature_scheme cv_scheme{};
	::std::byte cv_sig[1024]; /* rsa-8192 pss is the largest advertised */
	::std::size_t cv_sig_size{};
	::std::byte cv_transcript[64]; /* Hash(CH..Certificate) */
	::std::byte recbuf[17408];
	for (;;)
	{
		handshake_type mt;
		::std::byte const *body, *raw;
		::std::size_t body_size, raw_size;
		while (handshake_queue_next(__builtin_addressof(q), __builtin_addressof(mt), __builtin_addressof(body), __builtin_addressof(body_size), __builtin_addressof(raw), __builtin_addressof(raw_size)))
		{
			/* snapshot BEFORE this msg: CV covered and Fin MAC hash only
			   the preceding handshake messages */
			typename crypto::hash_ctx pre{transcript};
			switch (mt)
			{
			case handshake_type::encrypted_extensions:
				if (state != want_ee)
				{
					details::tls_fail(sock, alert_description::unexpected_message, false);
				}
				switch (details::ee_body_check(body, body_size))
				{
				case details::ee_check::ok:
					break;
				case details::ee_check::forbidden_extension:
					details::tls_fail(sock, alert_description::illegal_parameter, false);
				default:
					details::tls_fail(sock, alert_description::decode_error, false);
				}
				state = want_cert_or_cr;
				break;
			case handshake_type::certificate_request:
				if (state != want_cert_or_cr)
				{
					details::tls_fail(sock, alert_description::unexpected_message, false);
				}
				if (!details::cr_context_copy(body, body_size, cr_context,
											  __builtin_addressof(cr_context_size)))
				{
					details::tls_fail(sock, alert_description::decode_error, false);
				}
				cert_request_seen = true;
				break;
			case handshake_type::certificate:
				if (state != want_cert_or_cr)
				{
					details::tls_fail(sock, alert_description::unexpected_message, false);
				}
				if (!details::certificate_body_parse(__builtin_addressof(peer), body, body_size))
				{
					details::tls_fail(sock, alert_description::decode_error, false);
				}
				if (peer.count == 0)
				{
					/* empty client-visible chain is an abort in 1.3 */
					details::tls_fail(sock, alert_description::bad_certificate, false);
				}
				state = want_cv;
				break;
			case handshake_type::certificate_verify:
			{
				if (state != want_cv)
				{
					details::tls_fail(sock, alert_description::unexpected_message, false);
				}
				::fast_io::tls::details::certificate_verify_info cvi;
				if (!::fast_io::tls::details::certificate_verify_parse(cvi, body, body_size))
				{
					details::tls_fail(sock, alert_description::decode_error, false);
				}
				cv_scheme = cvi.scheme;
				if (cvi.signature_size > sizeof(cv_sig))
				{
					details::tls_fail(sock, alert_description::decode_error, false);
				}
				cv_sig_size = cvi.signature_size;
				::fast_io::freestanding::non_overlapped_copy_n(cvi.signature, cvi.signature_size, cv_sig);
				/* pre holds Hash(CH..Certificate) -- what the CV signs */
				::fast_io::tls::details::transcript_digest_to_ptr<crypto>(pre, cv_transcript);
				state = want_fin;
				break;
			}
			case handshake_type::finished:
			{
				if (state != want_fin || body_size != digest_size)
				{
					details::tls_fail(sock, alert_description::decode_error, false);
				}
				/* pre holds Hash(CH..CV) -- what server Finished MACs */
				::std::byte expect[64];
				::fast_io::tls::details::finished_verify_data_to_ptr<crypto>(md, expect, s_hs, pre);
				bool same{true};
				for (::std::size_t i{}; i != digest_size; ++i)
				{
					same &= (body[i] == expect[i]);
				}
				if (!same)
				{
					details::tls_fail(sock, alert_description::decrypt_error, false);
				}
				state = flight_done;
				break;
			}
			default:
				details::tls_fail(sock, alert_description::unexpected_message, false);
			}
			transcript.update(raw, raw + raw_size); /* fold in after processing */
		}
		if (state == flight_done)
		{
			break;
		}
		content_type inner{};
		::std::size_t inner_size{};
		if (!details::tls_recv_flight_record<crypto>(sock, recbuf, sizeof(recbuf), __builtin_addressof(inner), __builtin_addressof(inner_size),
													 suite, hs_rx_key, hs_rx_iv, hs_rx_seq))
		{
			details::tls_fail(sock, alert_description::bad_record_mac, false);
		}
		if (inner == content_type::application_data)
		{
			/* cannot happen: open() never yields outer-type; guard anyway */
			details::tls_fail(sock, alert_description::unexpected_message, false);
		}
		if (inner != content_type::change_cipher_spec)
		{
			/* encrypted records only advance the seq */
			if (inner_size == 0)
			{
				continue; /* empty protected record */
			}
			++hs_rx_seq;
		}
		switch (inner)
		{
		case content_type::handshake:
			handshake_queue_feed(__builtin_addressof(q), recbuf, inner_size);
			break;
		case content_type::change_cipher_spec:
			break; /* compat CCS between epochs; never transcripted */
		case content_type::alert:
			details::tls_throw_peer_alert(recbuf, inner_size);
		default:
			details::tls_fail(sock, alert_description::unexpected_message, false);
		}
	}

	/* ---- verify the certificate chain + leaf hostname ---- */
	::fast_io::tls::details::x509_certificate presented[16];
	if (!::fast_io::tls::details::x509_certificate_parse_all(
			presented, 16, peer.storage.data(), peer.offsets, peer.sizes,
			peer.count))
	{
		details::tls_fail(sock, alert_description::bad_certificate, false);
	}
	if (cfg->check_chain)
	{
		::std::int_least64_t const now{static_cast<::std::int_least64_t>(::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime).tv_sec)};
		switch (::fast_io::tls::details::x509_chain_verify<crypto>(presented, peer.count, cfg->roots, cfg->root_sizes, cfg->root_count, now))
		{
		case ::fast_io::tls::details::x509_chain_result::ok:
			break;
		case ::fast_io::tls::details::x509_chain_result::expired:
		case ::fast_io::tls::details::x509_chain_result::not_yet_valid:
			details::tls_fail(sock, alert_description::certificate_expired, false);
		case ::fast_io::tls::details::x509_chain_result::untrusted:
			details::tls_fail(sock, alert_description::unknown_ca, false);
		default:
			details::tls_fail(sock, alert_description::bad_certificate, false);
		}
	}
	if (cfg->check_hostname && !cfg->hostname.empty() &&
		!::fast_io::tls::details::x509_hostname_match(presented[0], cfg->hostname.data(), cfg->hostname.size()))
	{
		details::tls_fail(sock, alert_description::bad_certificate, false);
	}

	/* ---- CertificateVerify over covered content ---- */
	{
		::std::size_t const covered_size{
			::fast_io::tls::details::certificate_verify_content_prefix_size + digest_size};
		::std::byte covered[::fast_io::tls::details::certificate_verify_content_prefix_size + 64];
		::fast_io::tls::details::certificate_verify_content_write(covered, cv_transcript, digest_size);
		switch (crypto::cert_cv_verify(
			cv_scheme, covered, covered_size, cv_sig, cv_sig_size, presented[0]))
		{
		case ::fast_io::tls::details::x509_verify_result::ok:
			break;
		case ::fast_io::tls::details::x509_verify_result::unsupported_algorithm:
			details::tls_fail(sock, alert_description::illegal_parameter, false);
		default:
			details::tls_fail(sock, alert_description::decrypt_error, false);
		}
	}

	/* ---- master secret + application traffic secrets ---- */
	ks.derive_empty();
	::std::byte zero[64]{};
	ks.extract_into(zero, digest_size); /* master secret */

	::std::byte c_ap[64], s_ap[64];
	ks.derive_to_ptr(c_ap, u8"c ap traffic", 12, transcript);
	ks.derive_to_ptr(s_ap, u8"s ap traffic", 12, transcript);

	/* ---- client second flight ---- */

	/* compat CCS: still plaintext -- must go out BEFORE the TX key is
	   installed or the kernel would encrypt it as a handshake record */
	{
		::std::byte ccs[6];
		::std::byte *p{details::record_header_write(ccs, content_type::change_cipher_spec, 1)};
		*p++ = ::std::byte{1};
		details::tls_write_full(sock, ccs, static_cast<::std::size_t>(p - ccs));
	}
	::fast_io::tls::details::traffic_key_iv_to_ptr<crypto>(md, key, key_size, iv, c_hs);
	::std::byte hs_tx_key[32], hs_tx_iv[12];
	::fast_io::freestanding::non_overlapped_copy_n(key, key_size, hs_tx_key);
	::fast_io::freestanding::non_overlapped_copy_n(iv, 12, hs_tx_iv);
	::fast_io::secure_clear(key, sizeof(key));

	::std::byte flight[4 + 255 + 3 + 4 + 64];
	::std::byte *fp{flight};
	if (cert_request_seen)
	{
		/* empty Certificate echoing the request_context; it is
		   transcripted before the Finished MAC is keyed */
		fp = details::client_cert_echo_write(fp, cr_context, cr_context_size);
		transcript.update(flight, fp);
	}
	::std::byte fin[64];
	::fast_io::tls::details::finished_verify_data_to_ptr<crypto>(md, fin, c_hs, transcript);
	::std::byte const *const fin_start{fp};
	fp = details::finished_message_write(fp, fin, digest_size);
	{
		::std::byte sealed[1024];
		::std::size_t const sealed_size{crypto::record_seal(
			sealed, content_type::handshake, flight, static_cast<::std::size_t>(fp - flight),
			suite, hs_tx_key, hs_tx_iv, 0)};
		details::tls_write_full(sock, sealed, sealed_size);
	}
	transcript.update(fin_start, fp);

	/* epoch switch: TX -> c_ap (after Finished is on the wire), RX -> s_ap.
	   the expanded key+iv are reported either way; the kernel gets them
	   only when the ulp attached (a half-installed offload would leave
	   one direction kernel-encrypted and the other userspace -- a
	   set_key failure is fatal, not a fallback) */
	::fast_io::tls::details::traffic_key_iv_to_ptr<crypto>(md, key, key_size, iv, c_ap);
	::fast_io::freestanding::non_overlapped_copy_n(key, key_size, tx_key_iv_out->key);
	::fast_io::freestanding::non_overlapped_copy_n(iv, 12, tx_key_iv_out->iv);
	if (offload)
	{
		::fast_io::tls::details::ktls_offload_key(sock, details::tls_tx, suite, key, iv);
	}
	::fast_io::tls::details::traffic_key_iv_to_ptr<crypto>(md, key, key_size, iv, s_ap);
	::fast_io::freestanding::non_overlapped_copy_n(key, key_size, rx_key_iv_out->key);
	::fast_io::freestanding::non_overlapped_copy_n(iv, 12, rx_key_iv_out->iv);
	if (offload)
	{
		::fast_io::tls::details::ktls_offload_key(sock, details::tls_rx, suite, key, iv);
	}
	::fast_io::secure_clear(key, sizeof(key));
	::fast_io::secure_clear(iv, sizeof(iv));

	::fast_io::freestanding::non_overlapped_copy_n(c_ap, digest_size, tx_secret_out);
	::fast_io::freestanding::non_overlapped_copy_n(s_ap, digest_size, rx_secret_out);
	::fast_io::secure_clear(hs_rx_key, sizeof(hs_rx_key));
	::fast_io::secure_clear(hs_tx_key, sizeof(hs_tx_key));
	::fast_io::secure_clear(hs_rx_iv, sizeof(hs_rx_iv));
	::fast_io::secure_clear(hs_tx_iv, sizeof(hs_tx_iv));
}

} // namespace fast_io::tls

namespace fast_io::tls::details
{

/*
dispatch the post-SH handshake on the hash implied by the cipher suite.
Returns the negotiated suite's hash size via secret_size_out.
*/
template <typename crypto, typename stmtype>
inline void ktls_handshake_dispatch(stmtype sock, cipher_suite suite,
									::std::byte const *shared_secret,
									::std::byte const *ch_msg, ::std::size_t ch_msg_size,
									::std::byte const *sh_msg, ::std::size_t sh_msg_size,
									tls_client_config const *cfg,
									bool offload,
									app_traffic_key_iv *tx_key_iv_out,
									app_traffic_key_iv *rx_key_iv_out,
									::std::byte *tx_secret_out, ::std::byte *rx_secret_out,
									::std::size_t *secret_size_out) FAST_IO_HERBCEPTIONS_THROWS
{
	ktls_handshake_flight2<crypto>(sock, suite, shared_secret,
								   ch_msg, ch_msg_size, sh_msg, sh_msg_size,
								   cfg, offload, tx_key_iv_out,
								   rx_key_iv_out, tx_secret_out, rx_secret_out, secret_size_out);
}

} // namespace fast_io::tls::details

namespace fast_io::tls
{

namespace details
{

template <typename allocator_type, typename socket_observer_type, typename crypto>
inline void tls_client_handshake(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
								 tls_client_config const *cfg) FAST_IO_HERBCEPTIONS_THROWS
{
	/* attach the tls ulp before anything else -- when the kernel lacks
	   it (ENOPROTOOPT: no module / old kernel / restricted socket) or
	   the transport isn't an fd at all, the userspace record layer
	   serves; the handshake itself is plaintext on the wire either way */
	bool const offload{cfg->offload && details::tls_try_offload(client->sock_)};

	::std::byte session_id[32], random[32], sk[32], pk[32];
	details::tls_fill_random(session_id, 32);
	details::tls_fill_random(random, 32);
	details::tls_fill_random(sk, 32);
	crypto::x25519_keypair(pk, sk);

	::fast_io::tls::details::client_hello_params params{};
	params.hostname = cfg->hostname.data();
	params.hostname_size = cfg->hostname.size();
	params.session_id = session_id;
	params.session_id_size = 32; /* middlebox-compat session id */
	params.random = random;
	params.x25519_public_key = pk;

	::std::byte ch[2048];
	::std::byte const *msg{};
	::std::size_t ch_msg_size{};
	::std::size_t const rec_size{
		::fast_io::tls::details::client_hello_record_write(
			ch, sizeof(ch), params, __builtin_addressof(msg),
			__builtin_addressof(ch_msg_size))};
	if (rec_size == 0)
	{
		details::tls_throw_alert(alert_description::internal_error);
	}
	FAST_IO_HERBCEPTIONS_TRY
	{
		details::tls_write_full(client->sock_, ch, rec_size);
	}
	FAST_IO_HERBCEPTIONS_CATCH_ALL
	{
		details::tls_throw_alert(alert_description::internal_error);
	}

	/* ---- read ServerHello (+ maybe plaintext CCS) ---- */
	::std::byte sh_msg[4096];
	::std::size_t sh_msg_size{};
	{
		/* one wire record, not one message: a <=1.2 server coalesces its
		   first flight (SH + Certificate + ...) into a single plaintext
		   record, so the buffer is the protocol's record cap -- the SH
		   is just the first handshake message inside it */
		::std::byte rec[details::tls_max_ciphertext];
		for (;;)
		{
			content_type ctype;
			::std::size_t const n{details::tls_read_plaintext_record(client->sock_, rec, sizeof(rec), __builtin_addressof(ctype))};
			if (ctype == content_type::change_cipher_spec)
			{
				continue;
			}
			if (ctype == content_type::alert)
			{
				details::tls_throw_peer_alert(rec, n);
			}
			if (ctype != content_type::handshake || n < 4 ||
				rec[0] != static_cast<::std::byte>(handshake_type::server_hello))
			{
				details::tls_fail(client->sock_, alert_description::unexpected_message, false);
			}
			::std::size_t const mlen{
				(static_cast<::std::size_t>(static_cast<::std::uint_least8_t>(rec[1])) << 16u) |
				(static_cast<::std::size_t>(static_cast<::std::uint_least8_t>(rec[2])) << 8u) |
				static_cast<::std::size_t>(static_cast<::std::uint_least8_t>(rec[3]))};
			if (mlen > n - 4 || mlen + 4 > sizeof(sh_msg))
			{
				details::tls_fail(client->sock_, alert_description::decode_error, false);
			}
			/* record bytes past the SH belong to a flight the version
			   check aborts anyway -- only the message itself is kept */
			::fast_io::freestanding::non_overlapped_copy_n(rec, mlen + 4, sh_msg);
			sh_msg_size = mlen + 4;
			break;
		}
	}
	::std::byte const *sh_body{sh_msg + 4};
	::std::size_t sh_body_size{sh_msg_size - 4};
	if (sh_msg[0] != static_cast<::std::byte>(handshake_type::server_hello) ||
		sh_msg_size < 4 ||
		((static_cast<::std::size_t>(sh_msg[1]) << 16u) |
		 (static_cast<::std::size_t>(sh_msg[2]) << 8u) |
		 static_cast<::std::size_t>(sh_msg[3])) != sh_body_size)
	{
		details::tls_fail(client->sock_, alert_description::decode_error, false);
	}
	::fast_io::tls::details::server_hello_info shi{};
	if (!::fast_io::tls::details::server_hello_parse(shi, sh_body, sh_body_size, session_id,
													 32))
	{
		details::tls_fail(client->sock_, alert_description::decode_error, false);
	}
	switch (::fast_io::tls::details::server_hello_validate(shi))
	{
	case ::fast_io::tls::details::server_hello_check::ok:
		break;
	case ::fast_io::tls::details::server_hello_check::hello_retry:
	case ::fast_io::tls::details::server_hello_check::bad_cipher_suite:
		details::tls_fail(client->sock_, alert_description::handshake_failure, false);
	case ::fast_io::tls::details::server_hello_check::downgrade:
		details::tls_fail(client->sock_, alert_description::protocol_version, false);
	default:
		details::tls_fail(client->sock_, alert_description::illegal_parameter, false);
	}
	cipher_suite const suite{static_cast<cipher_suite>(shi.cipher_suite)};

	::std::byte shared[32];
	crypto::x25519_shared_secret(shared, shi.key_share_public_key, sk);
	::fast_io::secure_clear(sk, sizeof(sk));
	if (details::tls_all_zero(shared, 32))
	{
		/* rfc8446 4.2.8.1: all-zero X25519 result aborts */
		details::tls_fail(client->sock_, alert_description::illegal_parameter, false);
	}

	::std::byte tx_secret[48], rx_secret[48];
	::std::size_t secret_size{};
	details::app_traffic_key_iv tx_ki{}, rx_ki{};
	details::ktls_handshake_dispatch<crypto>(client->sock_, suite, shared,
											 msg, ch_msg_size, sh_msg, sh_msg_size,
											 cfg, offload,
											 __builtin_addressof(tx_ki), __builtin_addressof(rx_ki), tx_secret, rx_secret,
											 __builtin_addressof(secret_size));
	::fast_io::secure_clear(shared, sizeof(shared));
	tls_client_set_established(client, suite, tx_secret, rx_secret, secret_size, offload, tx_ki, rx_ki);
	::fast_io::secure_clear(tx_secret, sizeof(tx_secret));
	::fast_io::secure_clear(rx_secret, sizeof(rx_secret));
	::fast_io::secure_clear(&tx_ki, sizeof(tx_ki));
	::fast_io::secure_clear(&rx_ki, sizeof(rx_ki));
}

/*
post-handshake message walk shared by both record pumps: KeyUpdate ->
rekey, NST and friends are dropped (not a resumption client).
*/
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline void tls_client_process_post_handshake(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
											  ::std::byte const *msgs, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	wire_reader r{msgs, msgs + n};
	while (!r.empty())
	{
		::std::uint_least8_t mt;
		::std::uint_least32_t mlen;
		if (!r.take_u8(mt) || !r.take_u24(mlen) || r.remaining() < mlen)
		{
			break; /* split msg -- records stay atomic, so malformed */
		}
		::std::byte const *body{r.cur};
		r.cur += mlen;
		if (static_cast<handshake_type>(mt) == handshake_type::key_update && mlen == 1)
		{
			tls_client_key_update_received(client, static_cast<::std::uint_least8_t>(body[0]));
		}
	}
}

/*
post-handshake record pump. KeyUpdate rotates the direction's traffic
secret and reinstalls the kernel key (new epoch, seq 0); NST is dropped;
alerts map: close_notify -> 0 return (eof), else throw the alert.
*/
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline ::std::size_t tls_client_read_some(basic_tls_client<allocator_type, socket_observer_type, crypto> *client, ::std::byte *buf, ::std::size_t buf_size) FAST_IO_HERBCEPTIONS_THROWS
{
	if (!client->established_)
	{
		/* no handshake was performed -- plain transport passthrough */
		return static_cast<::std::size_t>(
			::fast_io::operations::read_some_bytes(client->sock_, buf, buf_size) - buf);
	}
	if (!client->offloaded_)
	{
		return tls_client_sw_read_some(client, buf, buf_size);
	}
#if defined(__linux__)
	if constexpr (details::tls_transport_is_fd<socket_observer_type>)
	{
		for (;;)
		{
			auto rr{details::ktls_recv_record(client->sock_.fd, buf, buf_size)};
			switch (rr.ctype)
			{
			case content_type::application_data:
				return rr.size;
			case content_type::change_cipher_spec:
				break;
			case content_type::alert:
			{
				if (rr.size == 2 && buf[0] == ::std::byte{1} && buf[1] == ::std::byte{0})
				{
					return 0;
				}
				details::tls_throw_peer_alert(buf, rr.size);
			}
			case content_type::handshake:
				tls_client_process_post_handshake(client, buf, rr.size);
				break;
			default:
				break;
			}
		}
	}
#endif
	__builtin_unreachable();
}

/*
userspace record pump (no kernel offload): drain pending plaintext
first, else read a whole wire record -- 5-byte plaintext header, then
len bytes -- open it under (client->rx_key_, client->rx_iv_, client->rx_seq_) and dispatch on the
inner content type. Records bigger than the caller's buffer stash the
remainder in client->rx_pending_.
*/
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline ::std::size_t tls_client_sw_read_some(basic_tls_client<allocator_type, socket_observer_type, crypto> *client, ::std::byte *buf, ::std::size_t buf_size) FAST_IO_HERBCEPTIONS_THROWS
{
	if (buf_size == 0)
	{
		return 0;
	}
	for (;;)
	{
		if (::std::size_t const take{tls_client_rx_pending_drain(client, buf, buf_size)}; take != 0)
		{
			return take;
		}
		::std::byte rec[details::tls_max_record];
		details::tls_read_full(client->sock_, rec, details::record_header_size);
		::std::size_t const clen{details::tls_record_payload_size(rec)};
		if (clen == ~static_cast<::std::size_t>(0))
		{
			tls_client_fail(client, alert_description::record_overflow);
		}
		details::tls_read_full(client->sock_, rec + details::record_header_size, clen);
		::std::byte inner[details::tls_max_ciphertext];
		auto const rr{tls_client_sw_open_record(client, rec, details::record_header_size + clen, inner)};
		if (rr.eof)
		{
			return 0;
		}
		if (rr.inner != content_type::application_data)
		{
			continue;
		}
		::std::size_t const take{rr.plaintext_size < buf_size ? rr.plaintext_size : buf_size};
		::fast_io::freestanding::non_overlapped_copy_n(inner, take, buf);
		if (rr.plaintext_size > take)
		{
			tls_client_rx_pending_stash(client, inner + take, rr.plaintext_size - take);
		}
		return take;
	}
}

template <typename allocator_type, typename socket_observer_type, typename crypto>
inline ::std::size_t tls_client_rx_pending_drain(basic_tls_client<allocator_type, socket_observer_type, crypto> *client, ::std::byte *buf, ::std::size_t n) noexcept
{
	::std::size_t const pend{tls_client_rx_pending(client)};
	::std::size_t const take{pend < n ? pend : n};
	::fast_io::freestanding::non_overlapped_copy_n(client->rx_pending_.data() + client->rx_pending_pos_,
												   take, buf);
	client->rx_pending_pos_ += take;
	if (client->rx_pending_pos_ == client->rx_pending_.size())
	{
		client->rx_pending_.clear();
		client->rx_pending_pos_ = 0;
	}
	return take;
}

template <typename allocator_type, typename socket_observer_type, typename crypto>
inline ::std::size_t tls_client_rx_pending_drain(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
												 ::fast_io::io_scatter_t const *sc, ::std::size_t nsc) noexcept
{
	::std::size_t done{};
	for (::std::size_t i{}; i != nsc && tls_client_rx_pending(client) != 0; ++i)
	{
		auto const &e{sc[i]};
		::std::size_t const take{tls_client_rx_pending(client) < e.len ? tls_client_rx_pending(client) : e.len};
		::fast_io::freestanding::non_overlapped_copy_n(
			client->rx_pending_.data() + client->rx_pending_pos_, take,
			const_cast<::std::byte *>(static_cast<::std::byte const *>(e.base)));
		client->rx_pending_pos_ += take;
		done += take;
	}
	if (client->rx_pending_pos_ == client->rx_pending_.size())
	{
		client->rx_pending_.clear();
		client->rx_pending_pos_ = 0;
	}
	return done;
}

template <typename allocator_type, typename socket_observer_type, typename crypto>
inline void tls_client_rx_pending_stash(basic_tls_client<allocator_type, socket_observer_type, crypto> *client, ::std::byte const *pt, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::size_t const old{client->rx_pending_.size()};
	client->rx_pending_.resize(old + n);
	::fast_io::freestanding::non_overlapped_copy_n(pt, n, client->rx_pending_.data() + old);
}

/*
open one complete wire record: plaintext CCS and plaintext alerts pass
through unsealed (compat records); outer application_data unwraps AEAD
under the current RX epoch. KeyUpdate rekeys inline via
process_post_handshake. eof reports close_notify (plaintext or inner).
*/
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline sw_record_result tls_client_sw_open_record(basic_tls_client<allocator_type, socket_observer_type, crypto> *client, ::std::byte const *rec, ::std::size_t rec_size,
												  ::std::byte *innerbuf) FAST_IO_HERBCEPTIONS_THROWS
{
	sw_record_result rr{};
	auto const outer{
		static_cast<content_type>(static_cast<::std::uint_least8_t>(rec[0]))};
	::std::size_t const clen{rec_size - details::record_header_size};
	if (outer == content_type::change_cipher_spec)
	{
		rr.inner = outer;
		return rr;
	}
	if (outer == content_type::alert)
	{
		if (clen == 2 && rec[5] == ::std::byte{1} && rec[6] == ::std::byte{0})
		{
			rr.eof = true;
			return rr;
		}
		details::tls_throw_peer_alert(rec + details::record_header_size, clen);
	}
	if (outer != content_type::application_data)
	{
		tls_client_fail(client, alert_description::unexpected_message);
	}
	if (!crypto::record_open(innerbuf, rr.plaintext_size, rr.inner, rec,
							 rec + details::record_header_size, clen,
							 client->suite_, client->rx_key_, client->rx_iv_, client->rx_seq_))
	{
		tls_client_fail(client, alert_description::bad_record_mac);
	}
	++client->rx_seq_;
	switch (rr.inner)
	{
	case content_type::alert:
		if (rr.plaintext_size == 2 && innerbuf[0] == ::std::byte{1} && innerbuf[1] == ::std::byte{0})
		{
			rr.eof = true;
			break;
		}
		details::tls_throw_peer_alert(innerbuf, rr.plaintext_size);
	case content_type::handshake:
		tls_client_process_post_handshake(client, innerbuf, rr.plaintext_size);
		break;
	default:
		break;
	}
	return rr;
}

template <typename allocator_type, typename socket_observer_type, typename crypto>
inline sw_seal_result tls_client_sw_seal_appdata(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
												 ::std::byte *out, ::fast_io::io_scatter_t const *pt, ::std::size_t npt) noexcept
{
	/* gather up to 2^14 plaintext bytes into a precomposed inner
	   (content || type), then one seal -- no double copy */
	::std::byte inner[(1u << 14u) + 1];
	::std::size_t got{};
	for (::std::size_t i{}; i != npt && got < (1u << 14u); ++i)
	{
		auto const &e{pt[i]};
		::std::size_t const room{(1u << 14u) - got};
		::std::size_t const take{e.len < room ? e.len : room};
		__builtin_memcpy(inner + got, e.base, take);
		got += take;
	}
	inner[got] = static_cast<::std::byte>(content_type::application_data);
	return {crypto::record_seal_inner(out, inner, got + 1, client->suite_,
									  client->tx_key_, client->tx_iv_, client->tx_seq_++),
			got};
}

template <typename allocator_type, typename socket_observer_type, typename crypto>
inline void tls_client_key_update_received(basic_tls_client<allocator_type, socket_observer_type, crypto> *client, ::std::uint_least8_t request) FAST_IO_HERBCEPTIONS_THROWS
{
	/* next = HKDF-Expand-Label(secret, "traffic upd", "", hash_size) */
	::std::byte next[64];
	typename crypto::md const md{crypto::md_for(client->suite_)};
	::std::size_t const key_size{::fast_io::tls::details::cipher_suite_key_size(client->suite_)};
	::fast_io::tls::details::hkdf_expand_label_to_ptr<crypto>(
		md, next, client->secret_size_, client->rx_secret_, u8"traffic upd", 11, nullptr, 0);
	::fast_io::freestanding::non_overlapped_copy_n(next, client->secret_size_, client->rx_secret_);
	::fast_io::tls::details::traffic_key_iv_to_ptr<crypto>(
		md, client->rx_key_, key_size, client->rx_iv_, client->rx_secret_);
	if (client->offloaded_)
	{
		details::ktls_offload_key(client->sock_, details::tls_rx, client->suite_, client->rx_key_, client->rx_iv_);
	}
	client->rx_seq_ = 0;
	if (request != 0)
	{
		/* peer asked us to rotate TX too: send our KeyUpdate under the
		   current keys, then rekey */
		::std::byte ku[5];
		::std::byte *p{details::handshake_header_write(ku, handshake_type::key_update, 1)};
		*p++ = ::std::byte{0}; /* update_not_requested */
		::std::size_t const ku_size{static_cast<::std::size_t>(p - ku)};
		if (client->offloaded_)
		{
#if defined(__linux__)
			if constexpr (details::tls_transport_is_fd<socket_observer_type>)
			{
				::fast_io::tls::details::ktls_send_record(client->sock_.fd, content_type::handshake, ku,
														  ku_size);
			}
#endif
		}
		else
		{
			::std::byte rec[details::record_header_size + 16 + sizeof(ku)];
			::std::size_t const rec_size{crypto::record_seal(
				rec, content_type::handshake, ku, ku_size, client->suite_, client->tx_key_, client->tx_iv_, client->tx_seq_++)};
			details::tls_write_full(client->sock_, rec, rec_size);
		}
		::fast_io::tls::details::hkdf_expand_label_to_ptr<crypto>(
			md, next, client->secret_size_, client->tx_secret_, u8"traffic upd", 11, nullptr, 0);
		::fast_io::freestanding::non_overlapped_copy_n(next, client->secret_size_, client->tx_secret_);
		::fast_io::tls::details::traffic_key_iv_to_ptr<crypto>(
			md, client->tx_key_, key_size, client->tx_iv_, client->tx_secret_);
		if (client->offloaded_)
		{
			details::ktls_offload_key(client->sock_, details::tls_tx, client->suite_, client->tx_key_, client->tx_iv_);
		}
		client->tx_seq_ = 0;
	}
	::fast_io::secure_clear(next, sizeof(next));
}

template <typename allocator_type, typename socket_observer_type, typename crypto>
inline ::std::size_t tls_client_sw_write_some(basic_tls_client<allocator_type, socket_observer_type, crypto> *client, ::std::byte const *buf, ::std::size_t buf_size) FAST_IO_HERBCEPTIONS_THROWS
{
	/* one sealed record per call; the 2^14 cap is the wire limit */
	::std::byte rec[details::record_header_size + (1u << 14u) + 1 + 16];
	::fast_io::io_scatter_t const in{buf, buf_size};
	auto const rr{tls_client_sw_seal_appdata(client, rec, __builtin_addressof(in), 1)};
	details::tls_write_full(client->sock_, rec, rr.wire_size);
	return rr.plaintext_consumed;
}

template <typename allocator_type, typename socket_observer_type, typename crypto>
inline ::std::size_t tls_client_write_some(basic_tls_client<allocator_type, socket_observer_type, crypto> *client, ::std::byte const *buf, ::std::size_t buf_size) FAST_IO_HERBCEPTIONS_THROWS
{
	if (!client->established_)
	{
		/* no handshake was performed -- plain transport passthrough */
		return static_cast<::std::size_t>(
			::fast_io::operations::write_some_bytes(client->sock_, buf, buf_size) - buf);
	}
	if (!client->offloaded_)
	{
		return tls_client_sw_write_some(client, buf, buf_size);
	}
	/* offloaded: a plain stream write -- the kernel seals the records */
	return static_cast<::std::size_t>(
		::fast_io::operations::write_some_bytes(client->sock_, buf, buf_size) - buf);
}

template <typename allocator_type, typename socket_observer_type, typename crypto>
inline void tls_client_send_alert(basic_tls_client<allocator_type, socket_observer_type, crypto> *client, alert_description desc) noexcept
{
	if (client->offloaded_)
	{
		details::tls_send_alert(client->sock_, desc, true);
		return;
	}
	/* userspace mode: the alert is sealed under the current TX keys */
	::std::byte const level{desc == alert_description::close_notify ? ::std::byte{1} : ::std::byte{2}};
	::std::byte body[2]{level, static_cast<::std::byte>(desc)};
	::std::byte rec[details::record_header_size + 3 + 16];
	::std::size_t const rec_size{crypto::record_seal(
		rec, content_type::alert, body, 2, client->suite_, client->tx_key_, client->tx_iv_, client->tx_seq_++)};
	FAST_IO_HERBCEPTIONS_TRY
	{
		details::tls_write_full(client->sock_, rec, rec_size);
	}
	FAST_IO_HERBCEPTIONS_CATCH_ALL
	{
	}
}

template <typename allocator_type, typename socket_observer_type, typename crypto>
inline void tls_client_fail(basic_tls_client<allocator_type, socket_observer_type, crypto> *client, alert_description desc) FAST_IO_HERBCEPTIONS_THROWS
{
	tls_client_send_alert(client, desc);
	details::tls_throw_alert(desc);
}

template <typename allocator_type, typename socket_observer_type, typename crypto>
inline void tls_client_send_close_notify(basic_tls_client<allocator_type, socket_observer_type, crypto> *client) noexcept
{
	tls_client_send_alert(client, alert_description::close_notify);
}

/* decrypted plaintext pending delivery (record > caller buffer) */
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline ::std::size_t tls_client_rx_pending(basic_tls_client<allocator_type, socket_observer_type, crypto> const *client) noexcept
{
	return client->rx_pending_.size() - client->rx_pending_pos_;
}

/* seal an alert into one wire record at out (>= 5 + 3 + 16); bumps
   tx_seq_ like a send would */
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline ::std::size_t tls_client_sw_seal_alert(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
											  ::std::byte *out, alert_description desc) noexcept
{
	::std::byte const level{desc == alert_description::close_notify
								? ::std::byte{1}
								: ::std::byte{2}};
	::std::byte const body[2]{level, static_cast<::std::byte>(desc)};
	return crypto::record_seal(out, content_type::alert, body, 2, client->suite_,
							   client->tx_key_, client->tx_iv_, client->tx_seq_++);
}

/* filled by handshake internals; keys are the expanded traffic
   key+iv (key_size/12 bytes used) */
template <typename allocator_type, typename socket_observer_type, typename crypto>
inline void tls_client_set_established(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
									   cipher_suite suite,
									   ::std::byte const *tx_secret, ::std::byte const *rx_secret,
									   ::std::size_t secret_size, bool offloaded,
									   details::app_traffic_key_iv const &tx_key_iv,
									   details::app_traffic_key_iv const &rx_key_iv) noexcept
{
	client->suite_ = suite;
	client->secret_size_ = secret_size;
	client->offloaded_ = offloaded;
	::fast_io::freestanding::non_overlapped_copy_n(tx_secret, secret_size, client->tx_secret_);
	::fast_io::freestanding::non_overlapped_copy_n(rx_secret, secret_size, client->rx_secret_);
	::fast_io::freestanding::non_overlapped_copy_n(tx_key_iv.key, sizeof(tx_key_iv.key), client->tx_key_);
	::fast_io::freestanding::non_overlapped_copy_n(tx_key_iv.iv, sizeof(tx_key_iv.iv), client->tx_iv_);
	::fast_io::freestanding::non_overlapped_copy_n(rx_key_iv.key, sizeof(rx_key_iv.key), client->rx_key_);
	::fast_io::freestanding::non_overlapped_copy_n(rx_key_iv.iv, sizeof(rx_key_iv.iv), client->rx_iv_);
	client->tx_seq_ = client->rx_seq_ = 0;
	client->rx_pending_pos_ = 0;
	client->rx_pending_.clear();
	client->established_ = true;
}

} // namespace details

} // namespace fast_io::tls
