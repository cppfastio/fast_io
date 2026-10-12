#pragma once

/*
TLS 1.3 (rfc8446) protocol constants. Only 1.3 is implemented; no
version downgrade is permitted anywhere in this module.
*/

#include "cipher_suite.h"

/*
-D FAST_IO_TLS_FORCE_FAST_IO=1 pins everything to the fast_io userspace
implementations: the native backend ignores schannel, and the crypto
backend ignores provider libraries (openssl/gnutls) even where their
headers are visible -- e.g. wine's schannel still shells out to gnutls,
which a wine user may not provide.
*/
#if !defined(FAST_IO_TLS_FORCE_FAST_IO)
#define FAST_IO_TLS_FORCE_FAST_IO 0
#endif

namespace fast_io::tls
{

/* record layer content_type */
enum class content_type : ::std::uint_least8_t
{
	change_cipher_spec = 20,
	alert = 21,
	handshake = 22,
	application_data = 23,
};

/* Handshake.msg_type */
enum class handshake_type : ::std::uint_least8_t
{
	client_hello = 1,
	server_hello = 2,
	new_session_ticket = 4,
	end_of_early_data = 5,
	encrypted_extensions = 8,
	certificate = 11,
	certificate_request = 13,
	certificate_verify = 15,
	finished = 20,
	key_update = 24,
	message_hash = 254,
};

/* Extension.extension_type */
enum class extension_type : ::std::uint_least16_t
{
	server_name = 0,
	max_fragment_length = 1,
	status_request = 5,
	supported_groups = 10,
	signature_algorithms = 13,
	application_layer_protocol_negotiation = 16,
	pre_shared_key = 41,
	early_data = 42,
	supported_versions = 43,
	cookie = 44,
	psk_key_exchange_modes = 45,
	certificate_authorities = 47,
	signature_algorithms_cert = 50,
	key_share = 51,
};

/* NamedGroup; only x25519 is implemented */
enum class named_group : ::std::uint_least16_t
{
	secp256r1 = 0x0017,
	secp384r1 = 0x0018,
	secp521r1 = 0x0019,
	x25519 = 0x001d,
	x448 = 0x001e,
};

/* SignatureScheme */
enum class signature_scheme : ::std::uint_least16_t
{
	/* RSASSA-PKCS1-v1_5: allowed for certificates only, never in
	   CertificateVerify under TLS 1.3 */
	rsa_pkcs1_sha256 = 0x0401,
	rsa_pkcs1_sha384 = 0x0501,
	rsa_pkcs1_sha512 = 0x0601,
	ecdsa_secp256r1_sha256 = 0x0403,
	ecdsa_secp384r1_sha384 = 0x0503,
	ecdsa_secp521r1_sha512 = 0x0603,
	ed25519 = 0x0807,
	rsa_pss_rsae_sha256 = 0x0804,
	rsa_pss_rsae_sha384 = 0x0805,
	rsa_pss_rsae_sha512 = 0x0806,
	rsa_pss_pss_sha256 = 0x0809,
	rsa_pss_pss_sha384 = 0x080a,
	rsa_pss_pss_sha512 = 0x080b,
};

/* AlertDescription */
enum class alert_description : ::std::uint_least8_t
{
	close_notify = 0,
	unexpected_message = 10,
	bad_record_mac = 20,
	record_overflow = 22,
	handshake_failure = 40,
	bad_certificate = 42,
	unsupported_certificate = 43,
	certificate_expired = 45,
	certificate_unknown = 46,
	illegal_parameter = 47,
	unknown_ca = 48,
	decode_error = 50,
	decrypt_error = 51,
	protocol_version = 70,
	internal_error = 80,
	missing_extension = 109,
	unsupported_extension = 110,
	unrecognized_name = 112,
	unknown_psk_identity = 115,
	certificate_required = 116,
	no_application_protocol = 120,
};

inline constexpr ::std::uint_least16_t protocol_version_tls13{0x0304};

} // namespace fast_io::tls
