#pragma once

/*
Linux kernel TLS (kTLS) plumbing. The kernel does record-layer AEAD
once keys are installed with setsockopt(SOL_TLS, TLS_TX/TLS_RX); the
handshake stays in userspace. Struct layouts mirror <linux/tls.h> --
they are stable uapi and duplicated here so this header does not pull
kernel headers into users who only parse it.

TLS 1.3 only: info.version is always 0x0304 and the record protection
profile (implicit nonce = salt || iv, per-record seq XORed in) is the
TLS 1.3 one the kernel applies for that version.
*/

#if defined(__linux__)

namespace fast_io::tls::details
{

inline constexpr int sol_tls{282};
inline constexpr int ipproto_tcp{6};
inline constexpr int tcp_ulp{31};
inline constexpr int tls_set_record_type{1};
inline constexpr int tls_get_record_type{2};

inline constexpr ::std::uint_least16_t tls_1_3_version{0x0304};

inline constexpr ::std::uint_least16_t tls_cipher_aes_gcm_128{51};
inline constexpr ::std::uint_least16_t tls_cipher_aes_gcm_256{52};
inline constexpr ::std::uint_least16_t tls_cipher_chacha20_poly1305{54};

/* ---- kernel uapi structs (ABI-stable, mirrored) ---- */

struct ktls_crypto_info
{
	::std::uint_least16_t version;
	::std::uint_least16_t cipher_type;
};

struct ktls_crypto_info_aes_gcm_128
{
	ktls_crypto_info info;
	::std::uint_least8_t iv[8];
	::std::uint_least8_t key[16];
	::std::uint_least8_t salt[4];
	::std::uint_least8_t rec_seq[8];
};

struct ktls_crypto_info_aes_gcm_256
{
	ktls_crypto_info info;
	::std::uint_least8_t iv[8];
	::std::uint_least8_t key[32];
	::std::uint_least8_t salt[4];
	::std::uint_least8_t rec_seq[8];
};

struct ktls_crypto_info_chacha20_poly1305
{
	ktls_crypto_info info;
	::std::uint_least8_t iv[12];
	::std::uint_least8_t key[32];
	__extension__ ::std::uint_least8_t salt[0]; /* extended layout; unused for chacha */
	::std::uint_least8_t rec_seq[8];
};

struct ktls_iovec
{
	void *base;
	::std::size_t len;
};

struct ktls_msghdr
{
	void *name;
	::std::uint_least32_t namelen;
	ktls_iovec *iov;
	::std::size_t iovlen;
	void *control;
	::std::size_t controllen;
	::std::int_least32_t flags;
};

struct ktls_cmsghdr
{
	::std::size_t len;
	::std::int_least32_t level;
	::std::int_least32_t type;
};

inline constexpr ::std::size_t ktls_cmsg_align{sizeof(::std::size_t)};
inline constexpr ::std::size_t ktls_cmsg_space_data{sizeof(::std::uint_least8_t)};

inline constexpr ::std::size_t ktls_cmsg_align_up(::std::size_t n) noexcept
{
	return (n + ktls_cmsg_align - 1) & ~(ktls_cmsg_align - 1);
}
inline constexpr ::std::size_t ktls_cmsg_space{ktls_cmsg_align_up(sizeof(ktls_cmsghdr)) + ktls_cmsg_align_up(ktls_cmsg_space_data)};
inline constexpr ::std::size_t ktls_cmsg_len{ktls_cmsg_align_up(sizeof(ktls_cmsghdr)) + ktls_cmsg_space_data};

/* fill a crypto_info blob: suite selects key/cipher shape; iv12 is the
   12-byte static iv from the key schedule; seq is the starting record
   sequence number (big-endian on the wire). returns blob size. */
inline constexpr ::std::size_t ktls_fill_crypto_info(void *blob,
													 cipher_suite suite,
													 ::std::byte const *key,
													 ::std::byte const *iv12,
													 ::std::uint_least64_t seq) noexcept
{
	switch (suite)
	{
	case cipher_suite::aes_128_gcm_sha256:
	{
		auto &ci{*static_cast<ktls_crypto_info_aes_gcm_128 *>(blob)};
		ci.info = {tls_1_3_version, tls_cipher_aes_gcm_128};
		::fast_io::details::non_overlapped_copy_n(key, 16, reinterpret_cast<::std::byte *>(ci.key));
		::fast_io::details::non_overlapped_copy_n(iv12, 4, reinterpret_cast<::std::byte *>(ci.salt));
		::fast_io::details::non_overlapped_copy_n(iv12 + 4, 8, reinterpret_cast<::std::byte *>(ci.iv));
		wire_put_u64(reinterpret_cast<::std::byte *>(ci.rec_seq), seq);
		return sizeof(ci);
	}
	case cipher_suite::aes_256_gcm_sha384:
	{
		auto &ci{*static_cast<ktls_crypto_info_aes_gcm_256 *>(blob)};
		ci.info = {tls_1_3_version, tls_cipher_aes_gcm_256};
		::fast_io::details::non_overlapped_copy_n(key, 32, reinterpret_cast<::std::byte *>(ci.key));
		::fast_io::details::non_overlapped_copy_n(iv12, 4, reinterpret_cast<::std::byte *>(ci.salt));
		::fast_io::details::non_overlapped_copy_n(iv12 + 4, 8, reinterpret_cast<::std::byte *>(ci.iv));
		wire_put_u64(reinterpret_cast<::std::byte *>(ci.rec_seq), seq);
		return sizeof(ci);
	}
	default:
	{
		auto &ci{*static_cast<ktls_crypto_info_chacha20_poly1305 *>(blob)};
		ci.info = {tls_1_3_version, tls_cipher_chacha20_poly1305};
		::fast_io::details::non_overlapped_copy_n(key, 32, reinterpret_cast<::std::byte *>(ci.key));
		::fast_io::details::non_overlapped_copy_n(iv12, 12, reinterpret_cast<::std::byte *>(ci.iv));
		wire_put_u64(reinterpret_cast<::std::byte *>(ci.rec_seq), seq);
		return sizeof(ci);
	}
	}
}

inline constexpr ::std::size_t ktls_crypto_info_blob_size{sizeof(ktls_crypto_info_aes_gcm_256)};

/*
install one direction's traffic key. level SOL_TLS / optname TLS_TX or
TLS_RX. `throw`s the posix errno on failure.
*/
/*
attach the tls ulp to the tcp socket. The tls module registers itself
under TCP_ULP "tls"; until this setsockopt the socket has no SOL_TLS
handler and every SOL_TLS setsockopt returns ENOPROTOOPT.
*/
inline void ktls_attach(int fd) FAST_IO_HERBCEPTIONS_THROWS
{
	char8_t const ulp_name[]{u8"tls"};
#if defined(__NR_setsockopt)
	system_call_throw_error(system_call<__NR_setsockopt, int>(fd, ipproto_tcp, tcp_ulp,
															  reinterpret_cast<::std::byte const *>(ulp_name),
															  sizeof(ulp_name)));
#else
	if (::fast_io::noexcept_call(::setsockopt, fd, ipproto_tcp, tcp_ulp,
								 reinterpret_cast<char const *>(ulp_name), sizeof(ulp_name)) == -1)
	{
		::fast_io::throw_posix_error();
	}
#endif
}

inline void ktls_set_key(int fd, int optname, cipher_suite suite,
						 ::std::byte const *key, ::std::byte const *iv12,
						 ::std::uint_least64_t seq) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::byte blob[ktls_crypto_info_blob_size];
	::std::size_t const n{ktls_fill_crypto_info(blob, suite, key, iv12, seq)};
#if defined(__NR_setsockopt)
	system_call_throw_error(system_call<__NR_setsockopt, int>(fd, sol_tls, optname, blob, n));
#else
	if (::fast_io::noexcept_call(::setsockopt, fd, sol_tls, optname, blob, n) == -1)
	{
		::fast_io::throw_posix_error();
	}
#endif
}

/*
receive one TLS record's plaintext. The kernel decrypts and reports the
true content type in a TLS_GET_RECORD_TYPE cmsg. Returns payload bytes
written into buf; ctype gets the record type. May return 0 on
close_notify-style EOF ordering.
*/
struct ktls_recv_result
{
	::std::size_t size;
	content_type ctype;
};

inline ktls_recv_result ktls_recv_record(int fd, ::std::byte *buf, ::std::size_t buf_size) FAST_IO_HERBCEPTIONS_THROWS
{
	ktls_recv_result res{0, content_type::application_data};
	alignas(::std::size_t)::std::byte control[ktls_cmsg_space];
	ktls_iovec iov{buf, buf_size};
	ktls_msghdr msg{};
	msg.iov = &iov;
	msg.iovlen = 1;
	msg.control = control;
	msg.controllen = sizeof(control);
	::std::ptrdiff_t ret;
#if defined(__NR_recvmsg)
	ret = system_call<__NR_recvmsg, ::std::ptrdiff_t>(fd, &msg, 0);
	system_call_throw_error(ret);
#else
	using msghdr_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= struct ::msghdr *;
	ret = ::fast_io::noexcept_call(::recvmsg, fd, reinterpret_cast<msghdr_alias_ptr>(&msg), 0);
	if (ret == -1)
	{
		::fast_io::throw_posix_error();
	}
#endif
	res.size = static_cast<::std::size_t>(ret);
	if ((msg.flags & 0x8 /* MSG_CTRUNC */) != 0)
	{
		::fast_io::throw_posix_error(EMSGSIZE);
	}
	/* walk control messages for TLS_GET_RECORD_TYPE */
	::std::size_t off{};
	while (off + sizeof(ktls_cmsghdr) <= msg.controllen)
	{
		auto const &cmsg{*reinterpret_cast<ktls_cmsghdr const *>(control + off)};
		if (cmsg.len < sizeof(ktls_cmsghdr))
		{
			break;
		}
		if (cmsg.level == sol_tls && cmsg.type == tls_get_record_type &&
			cmsg.len >= sizeof(ktls_cmsghdr) + 1)
		{
			res.ctype = static_cast<content_type>(
				*reinterpret_cast<::std::uint_least8_t const *>(control + off + sizeof(ktls_cmsghdr)));
		}
		off += ktls_cmsg_align_up(cmsg.len);
	}
	return res;
}

/*
fill a sendmsg msghdr carrying a TLS_SET_RECORD_TYPE cmsg; control must
point at ktls_cmsg_space bytes. Shared by the synchronous send path and
the io_uring async op, whose msghdr must outlive submission.
*/
inline void ktls_fill_send_msghdr(ktls_msghdr &msg, ktls_iovec &iov, void *control,
								  content_type ctype, ::std::byte const *data,
								  ::std::size_t data_size) noexcept
{
	auto &cmsg{*reinterpret_cast<ktls_cmsghdr *>(control)};
	cmsg.len = ktls_cmsg_len;
	cmsg.level = sol_tls;
	cmsg.type = tls_set_record_type;
	*reinterpret_cast<::std::uint_least8_t *>(static_cast<::std::byte *>(control) +
											  sizeof(ktls_cmsghdr)) =
		static_cast<::std::uint_least8_t>(ctype);
	iov = {const_cast<void *>(static_cast<void const *>(data)), data_size};
	msg = {};
	msg.iov = __builtin_addressof(iov);
	msg.iovlen = 1;
	msg.control = control;
	msg.controllen = ktls_cmsg_space;
}

/*
send one record with an explicit inner content type (handshake, alert).
Without the cmsg the kernel tags the record as application_data.
*/
inline void ktls_send_record(int fd, content_type ctype,
							 ::std::byte const *data, ::std::size_t data_size) FAST_IO_HERBCEPTIONS_THROWS
{
	alignas(::std::size_t)::std::byte control[ktls_cmsg_space];
	ktls_iovec iov{};
	ktls_msghdr msg{};
	ktls_fill_send_msghdr(msg, iov, control, ctype, data, data_size);
	::std::ptrdiff_t ret;
#if defined(__NR_sendmsg)
	ret = system_call<__NR_sendmsg, ::std::ptrdiff_t>(fd, &msg, 0);
	system_call_throw_error(ret);
#else
	using msghdr_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= struct ::msghdr const *;
	ret = ::fast_io::noexcept_call(::sendmsg, fd, reinterpret_cast<msghdr_alias_ptr>(&msg), 0);
	if (ret == -1)
	{
		::fast_io::throw_posix_error();
	}
#endif
	if (ret <= 0)
	{
		::fast_io::throw_posix_error(EPIPE);
	}
}

} // namespace fast_io::tls::details

#endif
