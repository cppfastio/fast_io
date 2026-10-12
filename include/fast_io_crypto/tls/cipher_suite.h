#pragma once

/*
TLS 1.3 cipher suites (rfc8446 B.4). Only the AEAD suites are listed;
nothing below TLS 1.3 is negotiated by this module. All three are
supported by Linux kTLS (aes-gcm via tls12_crypto_info_aes_gcm_*,
chacha20-poly1305 via tls12_crypto_info_chacha20_poly1305).
*/

namespace fast_io::tls
{

enum class cipher_suite : ::std::uint_least16_t
{
	aes_128_gcm_sha256 = 0x1301,
	aes_256_gcm_sha384 = 0x1302,
	chacha20_poly1305_sha256 = 0x1303,
};

namespace details
{

inline constexpr char8_t const *cipher_suite_name(cipher_suite cs) noexcept
{
	switch (cs)
	{
	case cipher_suite::aes_128_gcm_sha256:
		return u8"TLS_AES_128_GCM_SHA256";
	case cipher_suite::aes_256_gcm_sha384:
		return u8"TLS_AES_256_GCM_SHA384";
	case cipher_suite::chacha20_poly1305_sha256:
		return u8"TLS_CHACHA20_POLY1305_SHA256";
	default:
		return u8"TLS_UNKNOWN_CIPHER_SUITE";
	}
}

/* key length for the suite's bulk cipher */
inline constexpr ::std::size_t cipher_suite_key_size(cipher_suite cs) noexcept
{
	return cs == cipher_suite::aes_256_gcm_sha384 ? 32u : (cs == cipher_suite::aes_128_gcm_sha256 ? 16u : 32u);
}

} // namespace details

} // namespace fast_io::tls

namespace fast_io
{

/* dynamic_reserve_printable for cipher_suite: "0x1301 TLS_AES_128_GCM_SHA256" */
template <::std::integral char_type>
inline constexpr ::std::size_t print_reserve_size(::fast_io::io_reserve_type_t<char_type, ::fast_io::tls::cipher_suite>,
												  ::fast_io::tls::cipher_suite cs) noexcept
{
	return ::fast_io::cstr_len(::fast_io::tls::details::cipher_suite_name(cs)) + 7;
}

template <::std::integral char_type>
inline constexpr char_type *print_reserve_define(::fast_io::io_reserve_type_t<char_type, ::fast_io::tls::cipher_suite>,
												 char_type *ptr, ::fast_io::tls::cipher_suite cs) noexcept
{
	char8_t const *name{::fast_io::tls::details::cipher_suite_name(cs)};
	unsigned const v{static_cast<unsigned>(cs)};
	constexpr char8_t hexdig[]{u8"0123456789abcdef"};
	*ptr++ = ::fast_io::char_literal_v<u8'0', char_type>;
	*ptr++ = ::fast_io::char_literal_v<u8'x', char_type>;
	for (unsigned s{12u};; s -= 4u)
	{
		*ptr++ = static_cast<char_type>(hexdig[(v >> s) & 0xfu]);
		if (s == 0)
		{
			break;
		}
	}
	*ptr++ = ::fast_io::char_literal_v<u8' ', char_type>;
	return ::fast_io::details::non_overlapped_copy_n(name, ::fast_io::cstr_len(name), ptr);
}

} // namespace fast_io
