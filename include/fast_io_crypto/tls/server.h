#pragma once

/*
TLS 1.3 server handshake driver over a byte-stream transport.

Sequence:

  [plaintext phase]
	read ClientHello record(s) (+ optional plaintext CCS)
	require supported_versions offers 0x0304 -- no downgrade, ever
	pick a cipher suite, grab the client's x25519 share
	x25519 -> handshake secrets
  [our flight]
	write ServerHello + compat CCS in plaintext, then seal under the
	server handshake traffic key:
	  EncryptedExtensions, Certificate, CertificateVerify, Finished
  [client second flight]
	plaintext CCS; sealed client Finished under c_hs. A client that
	offered early_data may prepend early-data records we cannot
	decrypt -- those are skipped (0-RTT is not negotiated).
  [application phase]
	master secret -> c_ap (our RX) / s_ap (our TX); Linux SOL_TLS
	offload or the userspace record layer.

The session state is basic_tls_client -- its fields are
direction-neutral (tx_/rx_ keys, pending plaintext, offload flag), so
the post-handshake record pump, KeyUpdate and close_notify machinery
all serve the server unchanged; only the handshake driver differs.
*/

namespace fast_io::tls
{

struct tls_server_config
{
	/* certificate chain, leaf first, DER-encoded */
	::std::byte const *const *certs_der{};
	::std::size_t const *cert_sizes{};
	::std::size_t cert_count{};
	/* private key: PKCS#8, PKCS#1 RSAPrivateKey, or a raw ed25519
	   (32-byte seed / 64-byte expanded) -- see pkey.h */
	::std::byte const *private_key_der{};
	::std::size_t private_key_size{};
	/* kernel record offload (kTLS). false -> always the userspace
	   record AEAD; true -> still falls back when the ulp won't attach */
	bool offload{true};

	/* client authentication: request_client_cert sends
	   CertificateRequest; require_client_cert aborts when the client
	   presents an empty list (certificate_required). client_roots
	   anchors the client chain -- empty disables chain validation
	   (any presented cert is accepted). peer_out receives the
	   presented chain (DER) so the caller can check identity; may be
	   nullptr. */
	bool request_client_cert{};
	bool require_client_cert{};
	::std::byte const *const *client_roots_der{};
	::std::size_t const *client_root_sizes{};
	::std::size_t client_root_count{};
	::fast_io::tls::peer_certificates *peer_out{};

	/* resumption: a 16-byte aes key seals each issued ticket -- the
	   ticket itself is the only server-side state. nullptr disables
	   both ticket issuance and resumption accept. ticket_lifetime is
	   seconds. */
	::std::byte const (*ticket_key)[16]{};
	::std::uint_least32_t ticket_lifetime{7200};
	::std::size_t tickets_to_issue{2};
};

namespace details
{

/* ---------------- ClientHello parse ---------------- */

struct server_client_hello_info
{
	::std::byte const *session_id{};
	::std::size_t session_id_size{};
	::std::byte const *x25519_public_key{}; /* 32 */
	::std::uint_least16_t cipher_suite{};
	::std::uint_least16_t sigalgs[64]{};
	::std::size_t sigalg_count{};
	bool tls13{};
	bool x25519_group{};
	bool x25519_share{};
	bool null_compression{};
	bool has_sigalgs{};
	bool early_data{};
	bool has_sni{};

	/* resumption offer -- identities/binders parsed in pairs, capped */
	struct psk_offer
	{
		::std::byte const *ticket{};
		::std::size_t ticket_size{};
		::std::uint_least32_t obf_age{};
		::std::byte const *binder{};
		::std::size_t binder_size{};
	};
	psk_offer psk_offers[4]{};
	::std::size_t psk_offer_count{};
	::std::size_t binders_offset{}; /* byte offset in the body where the
									   binders u16 vector starts -- the
									   binder transcript truncates there */
	bool has_psk{};
	bool psk_dhe{};
};

/*
parse a ClientHello handshake body (after the 4-byte header).
legacy_version is ignored per rfc8446 4.1.2; the supported_versions
extension is the version gate. The cipher suite is negotiated here in
client preference order -- the first offered suite we implement wins.
Unknown extensions are ignored (they are legal in CH); the ones that
matter are decoded.
*/
inline constexpr bool server_client_hello_parse(server_client_hello_info &info,
												::std::byte const *body, ::std::size_t body_size) noexcept
{
	wire_reader r{body, body + body_size};
	::std::uint_least16_t legacy_version;
	::std::byte const *random;
	if (!r.take_u16(legacy_version) || !r.take_bytes(random, 32))
	{
		return false;
	}
	(void)legacy_version; /* rfc8446: ignored for all purposes */
	if (!r.take_vector8(info.session_id, info.session_id_size))
	{
		return false;
	}
	::std::byte const *suites;
	::std::size_t suites_size;
	if (!r.take_vector16(suites, suites_size))
	{
		return false;
	}
	{
		wire_reader sl{suites, suites + suites_size};
		while (!sl.is_empty())
		{
			::std::uint_least16_t v;
			if (!sl.take_u16(v))
			{
				return false;
			}
			if (v == static_cast<::std::uint_least16_t>(cipher_suite::aes_128_gcm_sha256) ||
				v == static_cast<::std::uint_least16_t>(cipher_suite::aes_256_gcm_sha384) ||
				v == static_cast<::std::uint_least16_t>(cipher_suite::chacha20_poly1305_sha256))
			{
				if (info.cipher_suite == 0)
				{
					info.cipher_suite = v;
				}
			}
		}
	}
	::std::byte const *compressions;
	::std::size_t compressions_size;
	if (!r.take_vector8(compressions, compressions_size))
	{
		return false;
	}
	for (::std::size_t i{}; i != compressions_size; ++i)
	{
		info.null_compression |= (compressions[i] == ::std::byte{});
	}
	::std::byte const *exts_p;
	::std::size_t exts_size;
	if (!r.take_vector16(exts_p, exts_size) || !r.is_empty())
	{
		return false;
	}
	wire_reader exts{exts_p, exts_p + exts_size};
	while (!exts.is_empty())
	{
		::std::uint_least16_t et;
		::std::byte const *ep;
		::std::size_t en;
		if (!exts.take_u16(et) || !exts.take_vector16(ep, en))
		{
			return false;
		}
		wire_reader e{ep, ep + en};
		switch (static_cast<extension_type>(et))
		{
		case extension_type::supported_versions:
		{
			::std::byte const *vs;
			::std::size_t vn;
			if (!e.take_vector8(vs, vn) || !e.is_empty())
			{
				return false;
			}
			for (::std::size_t i{}; i + 2 <= vn; i += 2)
			{
				if ((static_cast<::std::uint_least16_t>(vs[i]) << 8u |
					 static_cast<::std::uint_least16_t>(vs[i + 1])) == protocol_version_tls13)
				{
					info.tls13 = true;
				}
			}
			break;
		}
		case extension_type::supported_groups:
		{
			::std::byte const *gs;
			::std::size_t gn;
			if (!e.take_vector16(gs, gn) || !e.is_empty())
			{
				return false;
			}
			for (::std::size_t i{}; i + 2 <= gn; i += 2)
			{
				if ((static_cast<::std::uint_least16_t>(gs[i]) << 8u |
					 static_cast<::std::uint_least16_t>(gs[i + 1])) ==
					static_cast<::std::uint_least16_t>(named_group::x25519))
				{
					info.x25519_group = true;
				}
			}
			break;
		}
		case extension_type::key_share:
		{
			::std::byte const *ks;
			::std::size_t kn;
			if (!e.take_vector16(ks, kn) || !e.is_empty())
			{
				return false;
			}
			wire_reader shares{ks, ks + kn};
			while (!shares.is_empty())
			{
				::std::uint_least16_t group;
				::std::byte const *key;
				::std::size_t key_size;
				if (!shares.take_u16(group) || !shares.take_vector16(key, key_size))
				{
					return false;
				}
				if (group == static_cast<::std::uint_least16_t>(named_group::x25519) &&
					key_size == 32 && !info.x25519_share)
				{
					info.x25519_share = true;
					info.x25519_public_key = key;
				}
			}
			break;
		}
		case extension_type::signature_algorithms:
		{
			::std::byte const *sa;
			::std::size_t sn;
			if (!e.take_vector16(sa, sn) || !e.is_empty())
			{
				return false;
			}
			info.has_sigalgs = true;
			wire_reader l{sa, sa + sn};
			while (!l.is_empty())
			{
				::std::uint_least16_t v;
				if (!l.take_u16(v))
				{
					return false;
				}
				if (info.sigalg_count != sizeof(info.sigalgs) / sizeof(info.sigalgs[0]))
				{
					info.sigalgs[info.sigalg_count++] = v;
				}
			}
			break;
		}
		case extension_type::server_name:
		{
			info.has_sni = true;
			break;
		}
		case extension_type::early_data:
		{
			info.early_data = true;
			break;
		}
		case extension_type::psk_key_exchange_modes:
		{
			::std::byte const *ms;
			::std::size_t mn;
			if (!e.take_vector8(ms, mn) || !e.is_empty())
			{
				return false;
			}
			for (::std::size_t i{}; i != mn; ++i)
			{
				info.psk_dhe |= (ms[i] == ::std::byte{1}); /* psk_dhe_ke */
			}
			break;
		}
		case extension_type::pre_shared_key:
		{
			/* rfc8446 4.2.11: pre_shared_key MUST be the last
			   extension -- anything after it is a malformed CH */
			info.has_psk = true;
			::std::byte const *ids;
			::std::size_t ids_size;
			if (!e.take_vector16(ids, ids_size))
			{
				return false;
			}
			::std::size_t id_count{};
			{
				wire_reader il{ids, ids + ids_size};
				while (!il.is_empty())
				{
					::std::byte const *ticket;
					::std::size_t ticket_size;
					::std::uint_least32_t obf_age;
					if (!il.take_vector16(ticket, ticket_size) || !il.take_u32(obf_age))
					{
						return false;
					}
					if (id_count != sizeof(info.psk_offers) / sizeof(info.psk_offers[0]))
					{
						info.psk_offers[id_count].ticket = ticket;
						info.psk_offers[id_count].ticket_size = ticket_size;
						info.psk_offers[id_count].obf_age = obf_age;
					}
					++id_count;
				}
			}
			/* the binders vector begins right here in the body --
			   the truncated transcript ends just before its length */
			info.binders_offset = static_cast<::std::size_t>(e.cur - body);
			::std::byte const *binders;
			::std::size_t binders_size;
			if (!e.take_vector16(binders, binders_size) || !e.is_empty() || !exts.is_empty())
			{
				return false;
			}
			::std::size_t binder_count{};
			{
				wire_reader bl{binders, binders + binders_size};
				while (!bl.is_empty())
				{
					::std::byte const *binder;
					::std::size_t binder_size;
					if (!bl.take_vector8(binder, binder_size))
					{
						return false;
					}
					if (binder_count != sizeof(info.psk_offers) / sizeof(info.psk_offers[0]))
					{
						info.psk_offers[binder_count].binder = binder;
						info.psk_offers[binder_count].binder_size = binder_size;
					}
					++binder_count;
				}
			}
			if (binder_count != id_count || id_count == 0)
			{
				return false; /* identities and binders pair off 1:1 */
			}
			info.psk_offer_count = id_count < sizeof(info.psk_offers) / sizeof(info.psk_offers[0])
									   ? id_count
									   : sizeof(info.psk_offers) / sizeof(info.psk_offers[0]);
			break;
		}
		default:
			break; /* unrecognized CH extensions are ignored */
		}
	}
	return true;
}

/* the server's verdict on a parsed ClientHello -- the caller maps it to
   an alert (it owns the socket) */
enum class client_hello_check : ::std::uint_least8_t
{
	ok,
	downgrade,
	bad_compression,
	no_cipher_suite,
	no_key_share,
	missing_signature_algorithms,
	session_id_overflow,
	unexpected_extension
};

inline constexpr client_hello_check
server_client_hello_validate(server_client_hello_info const &info) noexcept
{
	if (info.session_id_size > 32)
	{
		return client_hello_check::session_id_overflow;
	}
	if (!info.tls13)
	{
		/* no supported_versions=0x0304: not a 1.3 client -- never
		   downgrade */
		return client_hello_check::downgrade;
	}
	if (!info.null_compression)
	{
		return client_hello_check::bad_compression;
	}
	if (info.cipher_suite == 0)
	{
		return client_hello_check::no_cipher_suite;
	}
	if (!info.x25519_share)
	{
		/* group advertised but no share would warrant a
		   HelloRetryRequest; for now any missing share fails */
		return client_hello_check::no_key_share;
	}
	if (info.early_data && !info.has_psk)
	{
		/* rfc8446 4.2.10: early_data only exists as a companion of
		   pre_shared_key -- alone it is unexpected_message */
		return client_hello_check::unexpected_extension;
	}
	if (!info.has_sigalgs && !info.has_psk)
	{
		/* rfc8446 4.2.3: a resuming client may omit
		   signature_algorithms (a resumed handshake has no
		   CertificateVerify); without a psk offer it stays
		   mandatory. If the psk fails its binder the driver
		   re-checks before running a cert handshake. */
		return client_hello_check::missing_signature_algorithms;
	}
	return client_hello_check::ok;
}

/* ---------------- ServerHello build ---------------- */

/*
rfc8446 4.1.3: HelloRetryRequest is a ServerHello whose random is
SHA-256("HelloRetryRequest").
*/
inline constexpr ::std::byte hello_retry_random[32]{
	::std::byte{0xcf}, ::std::byte{0x21}, ::std::byte{0xad}, ::std::byte{0x74},
	::std::byte{0xe5}, ::std::byte{0x9a}, ::std::byte{0x61}, ::std::byte{0x11},
	::std::byte{0xbe}, ::std::byte{0x1d}, ::std::byte{0x8c}, ::std::byte{0x02},
	::std::byte{0x1e}, ::std::byte{0x65}, ::std::byte{0xb8}, ::std::byte{0x91},
	::std::byte{0xc2}, ::std::byte{0xa2}, ::std::byte{0x11}, ::std::byte{0x16},
	::std::byte{0x7a}, ::std::byte{0xbb}, ::std::byte{0x8c}, ::std::byte{0x5e},
	::std::byte{0x07}, ::std::byte{0x9e}, ::std::byte{0x09}, ::std::byte{0xe2},
	::std::byte{0xc8}, ::std::byte{0xa8}, ::std::byte{0x33}, ::std::byte{0x9c}};

struct server_hello_params
{
	::std::byte const *random{};     /* 32 */
	::std::byte const *session_id{}; /* echo of the client's */
	::std::size_t session_id_size{};
	cipher_suite suite{};
	::std::byte const *x25519_public_key{}; /* our public, 32 */
	bool hello_retry{};                     /* key_share -> selected_group only */
	bool resumed{};                         /* pre_shared_key{selected} trails */
	::std::uint_least16_t psk_selected{};
};

inline constexpr ::std::size_t server_hello_size(server_hello_params const &params) noexcept
{
	return 2 /* legacy_version */ + 32 /* random */ +
		   1 + params.session_id_size /* session id echo */ +
		   2 /* cipher suite */ + 1 /* compression */ +
		   2 /* ext list len */ +
		   (4 + 2) /* supported_versions: u16 */ +
		   (params.hello_retry ? (4 + 2) /* key_share: u16 selected_group */
							   : (4 + 2 + 2 + 32) /* key_share: group + vec + key */) +
		   (params.resumed ? (4 + 2) : 0) /* pre_shared_key: u16 selected */;
}

/* writes the ServerHello BODY (after the 4-byte handshake header) */
inline constexpr ::std::byte *server_hello_write_body(::std::byte *p, server_hello_params const &params) noexcept
{
	p = wire_put_u16(p, 0x0303); /* legacy_version */
	p = wire_put_bytes(p, params.random, 32);
	*p++ = static_cast<::std::byte>(params.session_id_size);
	p = wire_put_bytes(p, params.session_id, params.session_id_size);
	p = wire_put_u16(p, static_cast<::std::uint_least16_t>(params.suite));
	*p++ = ::std::byte{0}; /* null compression */

	::std::byte *const ext_len{p};
	p += 2;
	::std::byte *const ext_begin{p};
	{
		/* supported_versions: u16 form in ServerHello */
		p = wire_put_u16(p, static_cast<::std::uint_least16_t>(extension_type::supported_versions));
		p = wire_put_u16(p, 2);
		p = wire_put_u16(p, protocol_version_tls13);
	}
	{
		/* key_share: HRR carries only selected_group (a u16); a full
		   ServerHello carries KeyShareEntry { group, u16 vec key } */
		p = wire_put_u16(p, static_cast<::std::uint_least16_t>(extension_type::key_share));
		if (params.hello_retry)
		{
			p = wire_put_u16(p, 2);
			p = wire_put_u16(p, static_cast<::std::uint_least16_t>(named_group::x25519));
		}
		else
		{
			p = wire_put_u16(p, 2 + 2 + 32);
			p = wire_put_u16(p, static_cast<::std::uint_least16_t>(named_group::x25519));
			p = wire_put_u16(p, 32);
			p = wire_put_bytes(p, params.x25519_public_key, 32);
		}
	}
	if (params.resumed)
	{
		/* rfc8446: pre_shared_key must be the LAST ServerHello
		   extension -- a bare u16 selected_identity */
		p = wire_put_u16(p, static_cast<::std::uint_least16_t>(extension_type::pre_shared_key));
		p = wire_put_u16(p, 2);
		p = wire_put_u16(p, params.psk_selected);
	}
	wire_put_u16(ext_len, static_cast<::std::uint_least16_t>(p - ext_begin));
	return p;
}

/* assemble the whole ServerHello record; msg_out/msg_size_out report
   the handshake-message span for the transcript */
inline ::std::size_t server_hello_record_write(
	::std::byte *rec, ::std::size_t cap, server_hello_params const &params,
	::std::byte const **msg_out, ::std::size_t *msg_size_out) noexcept
{
	::std::size_t const body_size{server_hello_size(params)};
	if (body_size + 9 > cap)
	{
		return 0;
	}
	::std::byte *const msg{rec + record_header_size};
	::std::byte *const body{handshake_header_write(
		msg, handshake_type::server_hello, static_cast<::std::uint_least32_t>(body_size))};
	::std::byte *const endp{server_hello_write_body(body, params)};
	::std::size_t const msg_size{static_cast<::std::size_t>(endp - msg)};
	::std::byte *const hdr_end{record_header_write(
		rec, content_type::handshake, static_cast<::std::uint_least16_t>(msg_size))};
	*msg_out = msg;
	*msg_size_out = msg_size;
	return static_cast<::std::size_t>(hdr_end - rec) + msg_size;
}

/*
CertificateRequest body: u8 context_len=0 || u16 ext_len ||
{signature_algorithms: u16 vec of u16}. The extension is required
(rfc8446 4.3.2); certificate_authorities is a hint we leave out.
*/
inline constexpr ::std::uint_least16_t server_cr_sigalgs[10]{
	static_cast<::std::uint_least16_t>(signature_scheme::rsa_pss_rsae_sha256),
	static_cast<::std::uint_least16_t>(signature_scheme::rsa_pss_rsae_sha384),
	static_cast<::std::uint_least16_t>(signature_scheme::rsa_pss_rsae_sha512),
	static_cast<::std::uint_least16_t>(signature_scheme::rsa_pss_pss_sha256),
	static_cast<::std::uint_least16_t>(signature_scheme::rsa_pss_pss_sha384),
	static_cast<::std::uint_least16_t>(signature_scheme::rsa_pss_pss_sha512),
	static_cast<::std::uint_least16_t>(signature_scheme::ecdsa_secp256r1_sha256),
	static_cast<::std::uint_least16_t>(signature_scheme::ecdsa_secp384r1_sha384),
	static_cast<::std::uint_least16_t>(signature_scheme::ecdsa_secp521r1_sha512),
	static_cast<::std::uint_least16_t>(signature_scheme::ed25519)};

inline constexpr ::std::byte *certificate_request_write(::std::byte *msg) noexcept
{
	::std::byte *p{handshake_header_write(msg, handshake_type::certificate_request, 29)};
	*p++ = ::std::byte{0}; /* no request context */
	::std::byte *const ext_len{p};
	p += 2;
	::std::byte *const ext_begin{p};
	p = wire_put_u16(p, static_cast<::std::uint_least16_t>(extension_type::signature_algorithms));
	p = wire_put_u16(p, 2 + sizeof(server_cr_sigalgs));
	p = wire_put_u16(p, static_cast<::std::uint_least16_t>(sizeof(server_cr_sigalgs)));
	for (::std::size_t i{}; i != sizeof(server_cr_sigalgs) / sizeof(server_cr_sigalgs[0]); ++i)
	{
		p = wire_put_u16(p, server_cr_sigalgs[i]);
	}
	wire_put_u16(ext_len, static_cast<::std::uint_least16_t>(p - ext_begin));
	return p;
}

/*
CertificateVerify scheme selection: the leaf cert's SPKI algorithm fixes
the family (ed25519 -> ed25519; rsaEncryption -> rsa_pss_rsae_*;
rsassaPss SPKI -> rsa_pss_pss_*); the client's signature_algorithms list
picks within it, in client preference order. Returns 0 when nothing
overlaps.
*/
inline constexpr ::std::uint_least16_t
tls_server_scheme_pick(der_tlv const &leaf_spki_oid, der_tlv const &leaf_spki_params,
					   ::std::uint_least16_t const *sigalgs, ::std::size_t sigalg_count) noexcept
{
	if (der_oid_eq(leaf_spki_oid, oid::ed25519))
	{
		for (::std::size_t i{}; i != sigalg_count; ++i)
		{
			if (sigalgs[i] == static_cast<::std::uint_least16_t>(signature_scheme::ed25519))
			{
				return sigalgs[i];
			}
		}
		return 0;
	}
	if (der_oid_eq(leaf_spki_oid, oid::ec_public_key))
	{
		/* the params OID pins the curve and its scheme */
		::std::uint_least16_t const want{
			der_oid_eq(leaf_spki_params, oid::secp256r1)
				? static_cast<::std::uint_least16_t>(signature_scheme::ecdsa_secp256r1_sha256)
			: der_oid_eq(leaf_spki_params, oid::secp384r1)
				? static_cast<::std::uint_least16_t>(signature_scheme::ecdsa_secp384r1_sha384)
			: der_oid_eq(leaf_spki_params, oid::secp521r1)
				? static_cast<::std::uint_least16_t>(signature_scheme::ecdsa_secp521r1_sha512)
				: static_cast<::std::uint_least16_t>(0)};
		if (want == 0)
		{
			return 0;
		}
		for (::std::size_t i{}; i != sigalg_count; ++i)
		{
			if (sigalgs[i] == want)
			{
				return sigalgs[i];
			}
		}
		return 0;
	}
	bool const pss_spki{der_oid_eq(leaf_spki_oid, oid::rsassa_pss)};
	if (!pss_spki && !der_oid_eq(leaf_spki_oid, oid::rsa_encryption))
	{
		return 0;
	}
	::std::uint_least16_t const cands[3]{
		pss_spki ? static_cast<::std::uint_least16_t>(signature_scheme::rsa_pss_pss_sha256)
				 : static_cast<::std::uint_least16_t>(signature_scheme::rsa_pss_rsae_sha256),
		pss_spki ? static_cast<::std::uint_least16_t>(signature_scheme::rsa_pss_pss_sha384)
				 : static_cast<::std::uint_least16_t>(signature_scheme::rsa_pss_rsae_sha384),
		pss_spki ? static_cast<::std::uint_least16_t>(signature_scheme::rsa_pss_pss_sha512)
				 : static_cast<::std::uint_least16_t>(signature_scheme::rsa_pss_rsae_sha512)};
	for (::std::size_t i{}; i != sigalg_count; ++i)
	{
		for (::std::size_t j{}; j != 3; ++j)
		{
			if (sigalgs[i] == cands[j])
			{
				return sigalgs[i];
			}
		}
	}
	return 0;
}

/* ---------------- resumption tickets ---------------- */

/*
Stateless ticket: the wire form is nonce12 || aes-128-gcm(blob) || tag
under the config's 16-byte ticket key. The blob pins the suite and the
issue time so a rotated or foreign ticket can never produce a plausible
psk:

	blob = u16 cipher_suite || u64 issue_unix || psk[64]

74 bytes plaintext -> 102 bytes on the wire.
*/
inline constexpr ::std::size_t tls_ticket_blob_size{2 + 8 + 64};
inline constexpr ::std::size_t tls_ticket_size{12 + tls_ticket_blob_size + 16};

inline ::std::size_t tls_ticket_seal(::std::byte *out, ::std::byte const (&key)[16],
									 cipher_suite suite, ::std::uint_least64_t issue_unix,
									 ::std::byte const *psk, ::std::size_t psk_size) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::byte blob[tls_ticket_blob_size];
	::std::byte *p{blob};
	p = wire_put_u16(p, static_cast<::std::uint_least16_t>(suite));
	p = wire_put_u32(p, static_cast<::std::uint_least32_t>(issue_unix >> 32));
	p = wire_put_u32(p, static_cast<::std::uint_least32_t>(issue_unix));
	for (::std::size_t i{}; i != 64; ++i)
	{
		blob[10 + i] = i < psk_size ? psk[i] : ::std::byte{};
	}
	::std::byte nonce[12];
	details::tls_fill_random(nonce, 12);
	::fast_io::freestanding::non_overlapped_copy_n(nonce, 12, out);
	::fast_io::aes_gcm_seal_to_ptr<16>(out + 12, out + 12 + tls_ticket_blob_size,
									   key, nonce, nullptr, 0, blob, tls_ticket_blob_size);
	::fast_io::secure_clear(blob, sizeof(blob));
	return tls_ticket_size;
}

inline bool tls_ticket_open(::std::byte *psk_out, cipher_suite *suite_out,
							::std::uint_least64_t *issue_out,
							::std::byte const (&key)[16],
							::std::byte const *ticket, ::std::size_t ticket_size) noexcept
{
	if (ticket_size != tls_ticket_size)
	{
		return false;
	}
	::std::byte nonce[12];
	::fast_io::freestanding::non_overlapped_copy_n(ticket, 12, nonce);
	::std::byte blob[tls_ticket_blob_size];
	::std::byte tag[16];
	::fast_io::freestanding::non_overlapped_copy_n(ticket + 12 + tls_ticket_blob_size, 16, tag);
	if (!::fast_io::aes_gcm_open_to_ptr<16>(blob, key, nonce, nullptr, 0,
											ticket + 12, tls_ticket_blob_size, tag))
	{
		return false;
	}
	wire_reader r{blob, blob + sizeof(blob)};
	::std::uint_least16_t suite{};
	::std::uint_least32_t hi{}, lo{};
	if (!r.take_u16(suite) || !r.take_u32(hi) || !r.take_u32(lo))
	{
		::fast_io::secure_clear(blob, sizeof(blob));
		return false;
	}
	*suite_out = static_cast<cipher_suite>(suite);
	*issue_out = (static_cast<::std::uint_least64_t>(hi) << 32) | lo;
	::fast_io::freestanding::non_overlapped_copy_n(r.cur, 64, psk_out);
	::fast_io::secure_clear(blob, sizeof(blob));
	return true;
}

/* NewSessionTicket body:
   u32 lifetime || u32 age_add || u8vec nonce || u16vec ticket ||
   u16 exts_len */
inline ::std::byte *new_session_ticket_write(::std::byte *msg,
											 ::std::uint_least32_t lifetime,
											 ::std::uint_least32_t age_add,
											 ::std::uint_least8_t nonce_index,
											 ::std::byte const *ticket,
											 ::std::size_t ticket_size) noexcept
{
	::std::byte *const len_field{msg + 1};
	::std::byte *p{msg + handshake_header_size};
	p = wire_put_u32(p, lifetime);
	p = wire_put_u32(p, age_add);
	*p++ = static_cast<::std::byte>(1);
	*p++ = static_cast<::std::byte>(nonce_index);
	p = wire_put_u16(p, static_cast<::std::uint_least16_t>(ticket_size));
	p = wire_put_bytes(p, ticket, ticket_size);
	p = wire_put_u16(p, 0);
	::std::size_t const body_size{static_cast<::std::size_t>(p - (msg + handshake_header_size))};
	msg[0] = static_cast<::std::byte>(handshake_type::new_session_ticket);
	wire_put_u24(len_field, static_cast<::std::uint_least32_t>(body_size));
	return p;
}

/* ---------------- the driver ---------------- */

/*
Handshake-epoch alert sink: before the handshake traffic key exists
alerts go out plaintext; once the server's s_hs keys are derived they
seal under them (rfc8446 5.4), sharing the flight's sequence counter.
armed flips right after the keys exist -- the CH/SH phase keeps the
plaintext path so a ClientHello rejection still looks like a normal
TLS alert to every client.
*/
struct server_fail_epoch
{
	::std::byte const *key{};
	::std::byte const *iv{};
	::std::uint_least64_t seq{};
	cipher_suite suite{};
	bool armed{};
};

template <typename crypto, typename stmtype>
[[noreturn]] inline void tls_fail_hs(stmtype sock, server_fail_epoch *epoch,
									 alert_description desc) FAST_IO_HERBCEPTIONS_THROWS
{
	if (epoch->armed)
	{
		::std::byte body[2]{desc == alert_description::close_notify
								? ::std::byte{1}
								: ::std::byte{2},
							static_cast<::std::byte>(desc)};
		::std::byte rec[details::record_header_size + 3 + 16];
		::std::size_t const n{crypto::record_seal(
			rec, content_type::alert, body, 2, epoch->suite, epoch->key, epoch->iv, epoch->seq++)};
		FAST_IO_HERBCEPTIONS_TRY
		{
			details::tls_write_full(sock, rec, n);
		}
		FAST_IO_HERBCEPTIONS_CATCH_ALL
		{
		}
		tls_throw_alert(desc);
	}
	details::tls_fail(sock, desc, false);
}

template <typename allocator_type, typename socket_observer_type, typename crypto>
inline void tls_server_handshake(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
								 tls_server_config const *cfg) FAST_IO_HERBCEPTIONS_THROWS
{
	bool const offload{cfg->offload && details::tls_try_offload(client->sock_)};

	/* ---- read the ClientHello (plaintext records; CCS skipped). A CH
	   that offers x25519 in supported_groups but carries no x25519
	   share gets a HelloRetryRequest; the retried CH is then read the
	   same way, checked against the first one ---- */
	::std::byte const *ch_raw{};
	::std::size_t ch_raw_size{};
	::std::byte const *ch_body{};
	::std::size_t ch_body_size{};
	server_client_hello_info chi{};
	bool hrr_sent{};
	::std::byte hrr_transcript[160];
	::std::size_t hrr_transcript_size{};
	::std::byte ch1_hash[64];
	::std::byte ch1_sid[32];
	::std::size_t ch1_sid_size{};
	::std::uint_least16_t ch1_suite{};
	{
		handshake_queue q{};
		for (;;)
		{
			handshake_type mt;
			::std::byte const *body, *raw;
			::std::size_t body_size, raw_size;
			while (handshake_queue_next(__builtin_addressof(q), __builtin_addressof(mt),
										__builtin_addressof(body), __builtin_addressof(body_size),
										__builtin_addressof(raw), __builtin_addressof(raw_size)))
			{
				if (mt != handshake_type::client_hello)
				{
					/* the first flight holds exactly one message */
					details::tls_fail(client->sock_, alert_description::unexpected_message, false);
				}
				ch_raw = raw;
				ch_raw_size = raw_size;
				ch_body = body;
				ch_body_size = body_size;
				goto have_ch;
			}
			{
				::std::byte rec[17408];
				content_type ctype;
				::std::size_t const n{details::tls_read_plaintext_record(
					client->sock_, rec, sizeof(rec), __builtin_addressof(ctype))};
				switch (ctype)
				{
				case content_type::handshake:
					handshake_queue_feed(__builtin_addressof(q), rec, n);
					break;
				case content_type::change_cipher_spec:
					break; /* compat CCS before/around CH */
				case content_type::alert:
					details::tls_throw_peer_alert(rec, n);
				default:
					details::tls_fail(client->sock_, alert_description::unexpected_message, false);
				}
			}
			continue;
		have_ch:;
			/* keep q alive until the transcript has digested ch_raw --
			   on the HRR path the pieces needed past the next feed are
			   extracted right away */
			server_client_hello_info cand{};
			if (!server_client_hello_parse(cand, ch_body, ch_body_size) ||
				cand.session_id_size > sizeof(ch1_sid))
			{
				details::tls_fail(client->sock_, alert_description::decode_error, false);
			}
			if (hrr_sent)
			{
				/* CH2 (rfc8446 4.1.2): the retried CH must keep the
				   session id and the cipher list, must carry the
				   requested share, and must not offer early data */
				bool sid_same{cand.session_id_size == ch1_sid_size};
				for (::std::size_t i{}; sid_same && i != ch1_sid_size; ++i)
				{
					sid_same = (cand.session_id[i] == ch1_sid[i]);
				}
				if (!sid_same || cand.cipher_suite != ch1_suite ||
					!cand.x25519_share || cand.early_data)
				{
					details::tls_fail(client->sock_, alert_description::illegal_parameter, false);
				}
				chi = cand;
				break;
			}
			if (cand.tls13 && cand.null_compression && cand.cipher_suite != 0 &&
				cand.has_sigalgs && !cand.x25519_share && cand.x25519_group)
			{
				/* HelloRetryRequest: digest CH1 for the transcript's
				   message_hash replacement and pin the sid/suite --
				   everything else about CH1 dies with the next
				   queue feed */
				typename crypto::md const hrr_md{
					crypto::md_for(static_cast<cipher_suite>(cand.cipher_suite))};
				typename crypto::hash_ctx h1{hrr_md};
				h1.update(ch_raw, ch_raw + ch_raw_size);
				::fast_io::tls::details::transcript_digest_to_ptr<crypto>(h1, ch1_hash);
				ch1_sid_size = cand.session_id_size;
				::fast_io::freestanding::non_overlapped_copy_n(
					cand.session_id, ch1_sid_size, ch1_sid);
				ch1_suite = cand.cipher_suite;
				::std::byte hrr[160];
				server_hello_params hp{};
				hp.random = hello_retry_random;
				hp.session_id = cand.session_id;
				hp.session_id_size = cand.session_id_size;
				hp.suite = static_cast<cipher_suite>(cand.cipher_suite);
				hp.hello_retry = true;
				::std::byte const *hmsg{};
				::std::size_t hmsg_size{};
				::std::size_t const hrr_size{server_hello_record_write(
					hrr, sizeof(hrr), hp, __builtin_addressof(hmsg), __builtin_addressof(hmsg_size))};
				if (hrr_size == 0 || hmsg_size > sizeof(hrr_transcript))
				{
					details::tls_fail(client->sock_, alert_description::internal_error, false);
				}
				details::tls_write_full(client->sock_, hrr, hrr_size);
				::fast_io::freestanding::non_overlapped_copy_n(hmsg, hmsg_size, hrr_transcript);
				hrr_transcript_size = hmsg_size;
				hrr_sent = true;
				continue;
			}
			chi = cand;
			break;
		}
		switch (server_client_hello_validate(chi))
		{
		case client_hello_check::ok:
			break;
		case client_hello_check::downgrade:
			details::tls_fail(client->sock_, alert_description::protocol_version, false);
		case client_hello_check::bad_compression:
			details::tls_fail(client->sock_, alert_description::illegal_parameter, false);
		case client_hello_check::missing_signature_algorithms:
			details::tls_fail(client->sock_, alert_description::missing_extension, false);
		case client_hello_check::session_id_overflow:
			details::tls_fail(client->sock_, alert_description::decode_error, false);
		case client_hello_check::unexpected_extension:
			details::tls_fail(client->sock_, alert_description::unexpected_message, false);
		default:
			details::tls_fail(client->sock_, alert_description::handshake_failure, false);
		}
		cipher_suite const suite{static_cast<cipher_suite>(chi.cipher_suite)};

		/* our own cert + key: a misconfiguration is our fault, not the
		   client's -- surface it as internal_error */
		if (cfg->cert_count == 0 || cfg->private_key_der == nullptr)
		{
			details::tls_fail(client->sock_, alert_description::internal_error, false);
		}

		::std::byte random[32], sk[32], pk[32];
		details::tls_fill_random(random, 32);
		details::tls_fill_random(sk, 32);
		crypto::x25519_keypair(pk, sk);

		::std::byte shared[32];
		crypto::x25519_shared_secret(shared, chi.x25519_public_key, sk);
		::fast_io::secure_clear(sk, sizeof(sk));
		if (details::tls_all_zero(shared, 32))
		{
			/* rfc8446 4.2.8.1: all-zero X25519 result aborts */
			details::tls_fail(client->sock_, alert_description::illegal_parameter, false);
		}

		typename crypto::md const md{crypto::md_for(suite)};
		::std::size_t const digest_size{crypto::md_digest_size(md)};
		::std::size_t const key_size{::fast_io::tls::details::cipher_suite_key_size(suite)};

		::std::int_least64_t const unix_now{static_cast<::std::int_least64_t>(
			::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime).tv_sec)};

		/* ---- resumption: try each offered ticket under our seal. The
		   binder is an HMAC over the truncated CH -- everything up to
		   the binders vector -- under a finished_key derived from the
		   ticket's psk, so a forged or foreign ticket cannot pass
		   even before its age is checked */
		::std::byte resumed_psk[64];
		::std::uint_least16_t psk_selected{0xffff};
		if (cfg->ticket_key != nullptr && chi.has_psk && chi.psk_dhe && chi.psk_offer_count != 0)
		{
			::std::byte truncated_hash[64];
			{
				typename crypto::hash_ctx th{md};
				if (hrr_sent)
				{
					/* the binder transcript across an HRR is
					   message_hash(CH1) || HRR || truncated CH2 */
					::std::byte mh[handshake_header_size + 64];
					::std::byte *p{handshake_header_write(
						mh, handshake_type::message_hash, static_cast<::std::uint_least32_t>(digest_size))};
					p = wire_put_bytes(p, ch1_hash, digest_size);
					th.update(mh, p);
					th.update(hrr_transcript, hrr_transcript + hrr_transcript_size);
				}
				th.update(ch_raw, ch_raw + handshake_header_size + chi.binders_offset);
				::fast_io::tls::details::transcript_digest_to_ptr<crypto>(th, truncated_hash);
			}
			for (::std::size_t i{}; i != chi.psk_offer_count && psk_selected == 0xffff; ++i)
			{
				auto const &offer{chi.psk_offers[i]};
				cipher_suite ticket_suite{};
				::std::uint_least64_t issued{};
				::std::byte psk[64];
				if (!tls_ticket_open(psk, __builtin_addressof(ticket_suite), __builtin_addressof(issued),
									 *cfg->ticket_key, offer.ticket, offer.ticket_size))
				{
					continue;
				}
				if (ticket_suite != suite || offer.binder_size != digest_size ||
					static_cast<::std::int_least64_t>(issued) + cfg->ticket_lifetime < unix_now)
				{
					continue;
				}
				::std::byte early[64], bkey[64], fk[64], expect[64];
				crypto::hkdf_extract(md, early, nullptr, 0, psk, digest_size);
				::std::byte empty_digest[64];
				{
					typename crypto::hash_ctx eh{md};
					eh.do_final();
					eh.digest_to_byte_ptr(empty_digest);
				}
				::fast_io::tls::details::hkdf_expand_label_to_ptr<crypto>(
					md, bkey, digest_size, early, u8"res binder", 10, empty_digest, digest_size);
				::fast_io::tls::details::hkdf_expand_label_to_ptr<crypto>(
					md, fk, digest_size, bkey, u8"finished", 8, nullptr, 0);
				crypto::hmac(md, expect, fk, digest_size, truncated_hash, digest_size);
				bool same{true};
				for (::std::size_t j{}; j != digest_size; ++j)
				{
					same &= (expect[j] == offer.binder[j]);
				}
				::fast_io::secure_clear(early, sizeof(early));
				::fast_io::secure_clear(bkey, sizeof(bkey));
				::fast_io::secure_clear(fk, sizeof(fk));
				::fast_io::secure_clear(empty_digest, sizeof(empty_digest));
				if (!same)
				{
					::fast_io::secure_clear(psk, sizeof(psk));
					continue;
				}
				::fast_io::freestanding::non_overlapped_copy_n(psk, digest_size, resumed_psk);
				::fast_io::secure_clear(psk, sizeof(psk));
				psk_selected = static_cast<::std::uint_least16_t>(i);
			}
		}

		/* a full handshake needs our cert + a signature scheme the
		   client offered; a resumed one needs neither (rfc8446 4.2.3
		   lets the resuming client omit signature_algorithms -- a
		   failed binder that falls back here must still enforce it) */
		::std::uint_least16_t scheme{};
		::fast_io::tls::details::x509_certificate leaf{};
		if (psk_selected == 0xffff)
		{
			if (!chi.has_sigalgs)
			{
				details::tls_fail(client->sock_, alert_description::missing_extension, false);
			}
			if (!::fast_io::tls::details::x509_certificate_parse(
					leaf, cfg->certs_der[0], cfg->cert_sizes[0]))
			{
				details::tls_fail(client->sock_, alert_description::internal_error, false);
			}
			scheme = tls_server_scheme_pick(leaf.spki_algorithm.oid,
											leaf.spki_algorithm.params,
											chi.sigalgs, chi.sigalg_count);
			if (scheme == 0)
			{
				details::tls_fail(client->sock_, alert_description::handshake_failure, false);
			}
		}

		typename crypto::hash_ctx transcript{md};
		if (hrr_sent)
		{
			/* rfc8446 4.4.1: after HRR the transcript begins with
			   message_hash(ClientHello1) || HelloRetryRequest || CH2 */
			::std::byte mh[handshake_header_size + 64];
			::std::byte *p{handshake_header_write(
				mh, handshake_type::message_hash, static_cast<::std::uint_least32_t>(digest_size))};
			p = wire_put_bytes(p, ch1_hash, digest_size);
			transcript.update(mh, p);
			transcript.update(hrr_transcript, hrr_transcript + hrr_transcript_size);
		}
		transcript.update(ch_raw, ch_raw + ch_raw_size);

		::fast_io::tls::details::key_schedule<crypto> ks{md};
		if (psk_selected != 0xffff)
		{
			/* resumption: the early secret IS the psk */
			ks.init_early_with(resumed_psk, digest_size);
			::fast_io::secure_clear(resumed_psk, sizeof(resumed_psk));
		}
		else
		{
			ks.init_early();
		}
		ks.derive_empty();
		ks.extract_into(shared, 32);
		::fast_io::secure_clear(shared, sizeof(shared));

		::std::byte c_hs[64], s_hs[64];
		/* transcript is CH || SH once the SH below is folded */
		{
			::std::byte sh[512];
			details::server_hello_params shp{};
			shp.random = random;
			shp.session_id = chi.session_id;
			shp.session_id_size = chi.session_id_size;
			shp.suite = suite;
			shp.x25519_public_key = pk;
			shp.resumed = psk_selected != 0xffff;
			shp.psk_selected = psk_selected != 0xffff ? psk_selected : 0;
			::std::byte const *sh_msg{};
			::std::size_t sh_msg_size{};
			::std::size_t const rec_size{details::server_hello_record_write(
				sh, sizeof(sh), shp, __builtin_addressof(sh_msg), __builtin_addressof(sh_msg_size))};
			if (rec_size == 0)
			{
				details::tls_fail(client->sock_, alert_description::internal_error, false);
			}
			details::tls_write_full(client->sock_, sh, rec_size);
			/* compat CCS: plaintext, right after ServerHello */
			::std::byte ccs[6];
			::std::byte *p{details::record_header_write(ccs, content_type::change_cipher_spec, 1)};
			*p++ = ::std::byte{1};
			details::tls_write_full(client->sock_, ccs, static_cast<::std::size_t>(p - ccs));
			transcript.update(sh_msg, sh_msg + sh_msg_size);
		}
		::fast_io::secure_clear(random, sizeof(random));
		::fast_io::secure_clear(pk, sizeof(pk));

		ks.derive_to_ptr(c_hs, u8"c hs traffic", 12, transcript);
		ks.derive_to_ptr(s_hs, u8"s hs traffic", 12, transcript);

		::std::byte key[32], iv[12];
		::std::byte hs_tx_key[32], hs_tx_iv[12], hs_rx_key[32], hs_rx_iv[12];
		::fast_io::tls::details::traffic_key_iv_to_ptr<crypto>(md, key, key_size, iv, s_hs);
		::fast_io::freestanding::non_overlapped_copy_n(key, key_size, hs_tx_key);
		::fast_io::freestanding::non_overlapped_copy_n(iv, 12, hs_tx_iv);
		::fast_io::tls::details::traffic_key_iv_to_ptr<crypto>(md, key, key_size, iv, c_hs);
		::fast_io::freestanding::non_overlapped_copy_n(key, key_size, hs_rx_key);
		::fast_io::freestanding::non_overlapped_copy_n(iv, 12, hs_rx_iv);
		::fast_io::secure_clear(key, sizeof(key));

		/* from here on an abort must seal its alert under s_hs */
		server_fail_epoch epoch{
			.key = hs_tx_key, .iv = hs_tx_iv, .seq = 0, .suite = suite, .armed = true};

		/* ---- our encrypted flight: EE || Certificate || CV || Fin ----
		   built as one handshake octet stream, emitted in <=16K
		   records; the transcript folds each message as produced */
		{
			::std::size_t cert_list_size{3};
			for (::std::size_t i{}; i != cfg->cert_count; ++i)
			{
				cert_list_size += 3 + cfg->cert_sizes[i] + 2;
			}
			auto flight{
				tls_alloc_construct<::fast_io::vector<::std::byte, allocator_type>, allocator_type>(
					client->allocator_handle)};
			flight.reserve(4 + 2 + 4 + 1 + cert_list_size + 4 + 4 + 1024 + 4 + 64);
			auto emit{[&](::std::size_t n) -> ::std::byte * {
				::std::size_t const off{flight.size()};
				flight.resize(off + n);
				return flight.data() + off;
			}};
			::std::size_t transcripted{};
			auto fold{[&]() noexcept {
				transcript.update(flight.data() + transcripted, flight.data() + flight.size());
				transcripted = flight.size();
			}};

			/* EncryptedExtensions: empty extension list */
			{
				::std::byte *p{emit(handshake_header_size + 2)};
				p = handshake_header_write(p, handshake_type::encrypted_extensions, 2);
				wire_put_u16(p, 0);
			}
			fold();

			/* CertificateRequest when client auth is wanted --
			   rfc8446 4.3.2 forbids it on a psk-resumed handshake
			   (post-handshake auth is the mechanism there) */
			if (cfg->request_client_cert && psk_selected == 0xffff)
			{
				certificate_request_write(emit(handshake_header_size + 29));
			}
			fold();

			/* Certificate + CertificateVerify: a resumed handshake is
			   PSK-authenticated, so both are skipped entirely */
			if (psk_selected == 0xffff)
			{
				::std::byte *p{emit(handshake_header_size + 1 + 3 + cert_list_size - 3)};
				p = handshake_header_write(
					p, handshake_type::certificate,
					static_cast<::std::uint_least32_t>(1 + cert_list_size));
				*p++ = ::std::byte{0}; /* no certificate_request_context */
				p = wire_put_u24(p, static_cast<::std::uint_least32_t>(cert_list_size - 3));
				for (::std::size_t i{}; i != cfg->cert_count; ++i)
				{
					p = wire_put_u24(p, static_cast<::std::uint_least32_t>(cfg->cert_sizes[i]));
					p = wire_put_bytes(p, cfg->certs_der[i], cfg->cert_sizes[i]);
					p = wire_put_u16(p, 0); /* no per-entry extensions */
				}
			}
			fold();

			/* CertificateVerify over Hash(CH..Certificate) */
			if (psk_selected == 0xffff)
			{
				::std::byte cv_transcript[64];
				::fast_io::tls::details::transcript_digest_to_ptr<crypto>(transcript, cv_transcript);
				::std::byte covered[::fast_io::tls::details::certificate_verify_content_prefix_size + 64];
				::fast_io::tls::details::certificate_verify_content_write(covered, cv_transcript, digest_size);
				::std::byte salt[64];
				details::tls_fill_random(salt, sizeof(salt));
				::std::byte sig[1024];
				::std::size_t sig_size{};
				if (!crypto::cert_cv_sign(static_cast<signature_scheme>(scheme), covered,
										  ::fast_io::tls::details::certificate_verify_content_prefix_size + digest_size,
										  cfg->private_key_der, cfg->private_key_size,
										  salt, sig, __builtin_addressof(sig_size)))
				{
					tls_fail_hs<crypto>(client->sock_, __builtin_addressof(epoch), alert_description::internal_error);
				}
				::fast_io::secure_clear(salt, sizeof(salt));
				::std::byte *p{emit(handshake_header_size + 2 + 2 + sig_size)};
				p = handshake_header_write(
					p, handshake_type::certificate_verify,
					static_cast<::std::uint_least32_t>(2 + 2 + sig_size));
				p = wire_put_u16(p, scheme);
				p = wire_put_u16(p, static_cast<::std::uint_least16_t>(sig_size));
				p = wire_put_bytes(p, sig, sig_size);
				::fast_io::secure_clear(sig, sizeof(sig));
			}
			fold();

			/* server Finished: verify_data MACs CH..CV */
			{
				::std::byte fin[64];
				::fast_io::tls::details::finished_verify_data_to_ptr<crypto>(md, fin, s_hs, transcript);
				::std::byte *p{emit(handshake_header_size + digest_size)};
				p = handshake_header_write(
					p, handshake_type::finished, static_cast<::std::uint_least32_t>(digest_size));
				p = wire_put_bytes(p, fin, digest_size);
			}
			fold();

			/* ship the flight: <=16K inner plaintext per record; the seq
			   doubles as the epoch's so an alert keeps counting right */
			::std::byte rec[details::record_header_size + 16641 + 16];
			for (::std::size_t off{}; off != flight.size();)
			{
				::std::size_t const chunk{
					flight.size() - off < 16384 ? flight.size() - off : 16384};
				::std::size_t const rec_size{crypto::record_seal(
					rec, content_type::handshake, flight.data() + off, chunk,
					suite, hs_tx_key, hs_tx_iv, epoch.seq++)};
				details::tls_write_full(client->sock_, rec, rec_size);
				off += chunk;
			}
		}

		/* ---- master + application secrets: transcript is CH..serverFin ---- */
		ks.derive_empty();
		{
			::std::byte zero[64]{};
			ks.extract_into(zero, digest_size);
		}
		::std::byte c_ap[64], s_ap[64];
		ks.derive_to_ptr(c_ap, u8"c ap traffic", 12, transcript);
		ks.derive_to_ptr(s_ap, u8"s ap traffic", 12, transcript);

		/* the app traffic keys up front: by the time the client's
		   second flight arrives it has already switched its read epoch
		   to s_ap, so a rejection sealed under s_hs would only ever be
		   an undecryptable record to it */
		details::app_traffic_key_iv tx_ki{}, rx_ki{};
		::fast_io::tls::details::traffic_key_iv_to_ptr<crypto>(md, key, key_size, iv, s_ap);
		::fast_io::freestanding::non_overlapped_copy_n(key, key_size, tx_ki.key);
		::fast_io::freestanding::non_overlapped_copy_n(iv, 12, tx_ki.iv);
		::fast_io::tls::details::traffic_key_iv_to_ptr<crypto>(md, key, key_size, iv, c_ap);
		::fast_io::freestanding::non_overlapped_copy_n(key, key_size, rx_ki.key);
		::fast_io::freestanding::non_overlapped_copy_n(iv, 12, rx_ki.iv);
		::fast_io::secure_clear(key, sizeof(key));
		::fast_io::secure_clear(iv, sizeof(iv));
		epoch.key = tx_ki.key;
		epoch.iv = tx_ki.iv;
		epoch.seq = 0;

		::std::byte fin_copy[handshake_header_size + 64];
		::std::size_t fin_copy_size{};

		/* ---- the client's second flight: CCS, then {Certificate?,
		   CertificateVerify?, Finished}. Certificate/CertificateVerify
		   show up only when we asked; a declined client sends an empty
		   Certificate and omits CertificateVerify (rfc8446 4.4.2) ---- */
		{
			enum class second_flight_state : ::std::uint_least8_t
			{
				want_cert,
				want_cv,
				want_fin
			};
			second_flight_state flight_state{cfg->request_client_cert &&
													 psk_selected == 0xffff
												 ? second_flight_state::want_cert
												 : second_flight_state::want_fin};
			handshake_queue fq{};
			::fast_io::tls::peer_certificates client_peer{};
			::fast_io::tls::details::x509_certificate client_leaf{};
			::std::byte recbuf[17408];
			::std::uint_least64_t hs_rx_seq{};
			::std::size_t early_skipped{};
			for (;;)
			{
				handshake_type mt;
				::std::byte const *body, *raw;
				::std::size_t body_size, raw_size;
				while (handshake_queue_next(__builtin_addressof(fq), __builtin_addressof(mt),
											__builtin_addressof(body), __builtin_addressof(body_size),
											__builtin_addressof(raw), __builtin_addressof(raw_size)))
				{
					switch (mt)
					{
					case handshake_type::end_of_early_data:
						if (!chi.early_data)
						{
							tls_fail_hs<crypto>(client->sock_, __builtin_addressof(epoch), alert_description::unexpected_message);
						}
						transcript.update(raw, raw + raw_size);
						break; /* dropped -- 0-RTT was not negotiated */
					case handshake_type::certificate:
					{
						if (flight_state != second_flight_state::want_cert)
						{
							tls_fail_hs<crypto>(client->sock_, __builtin_addressof(epoch), alert_description::unexpected_message);
						}
						if (!details::certificate_body_parse(
								__builtin_addressof(client_peer), body, body_size))
						{
							tls_fail_hs<crypto>(client->sock_, __builtin_addressof(epoch), alert_description::decode_error);
						}
						if (client_peer.count == 0)
						{
							/* empty list = the client declined to
							   authenticate; rfc8446 forbids CV then */
							if (cfg->require_client_cert)
							{
								tls_fail_hs<crypto>(client->sock_, __builtin_addressof(epoch), alert_description::certificate_required);
							}
							flight_state = second_flight_state::want_fin;
						}
						else
						{
							::fast_io::tls::details::x509_certificate presented[16];
							if (!::fast_io::tls::details::x509_certificate_parse_all(
									presented, 16, client_peer.storage.data(),
									client_peer.offsets, client_peer.sizes, client_peer.count))
							{
								tls_fail_hs<crypto>(client->sock_, __builtin_addressof(epoch), alert_description::bad_certificate);
							}
							if (cfg->client_root_count != 0)
							{
								::std::int_least64_t const now{static_cast<::std::int_least64_t>(
									::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime).tv_sec)};
								switch (::fast_io::tls::details::x509_chain_verify<crypto>(
									presented, client_peer.count,
									cfg->client_roots_der, cfg->client_root_sizes,
									cfg->client_root_count, now))
								{
								case ::fast_io::tls::details::x509_chain_result::ok:
									break;
								case ::fast_io::tls::details::x509_chain_result::expired:
								case ::fast_io::tls::details::x509_chain_result::not_yet_valid:
									tls_fail_hs<crypto>(client->sock_, __builtin_addressof(epoch), alert_description::certificate_expired);
								case ::fast_io::tls::details::x509_chain_result::untrusted:
									tls_fail_hs<crypto>(client->sock_, __builtin_addressof(epoch), alert_description::unknown_ca);
								default:
									tls_fail_hs<crypto>(client->sock_, __builtin_addressof(epoch), alert_description::bad_certificate);
								}
							}
							client_leaf = presented[0];
							if (cfg->peer_out != nullptr)
							{
								*cfg->peer_out = ::std::move(client_peer);
							}
							flight_state = second_flight_state::want_cv;
						}
						transcript.update(raw, raw + raw_size);
						break;
					}
					case handshake_type::certificate_verify:
					{
						if (flight_state != second_flight_state::want_cv)
						{
							tls_fail_hs<crypto>(client->sock_, __builtin_addressof(epoch), alert_description::unexpected_message);
						}
						::fast_io::tls::details::certificate_verify_info cvi{};
						if (!::fast_io::tls::details::certificate_verify_parse(cvi, body, body_size))
						{
							tls_fail_hs<crypto>(client->sock_, __builtin_addressof(epoch), alert_description::decode_error);
						}
						::std::byte cv_transcript[64];
						::fast_io::tls::details::transcript_digest_to_ptr<crypto>(transcript, cv_transcript);
						::std::byte covered[::fast_io::tls::details::certificate_verify_content_prefix_size + 64];
						::fast_io::tls::details::certificate_verify_content_write(
							covered, cv_transcript, digest_size, true);
						switch (crypto::cert_cv_verify(
							cvi.scheme, covered,
							::fast_io::tls::details::certificate_verify_content_prefix_size + digest_size,
							cvi.signature, cvi.signature_size, client_leaf))
						{
						case ::fast_io::tls::details::x509_verify_result::ok:
							break;
						case ::fast_io::tls::details::x509_verify_result::unsupported_algorithm:
							tls_fail_hs<crypto>(client->sock_, __builtin_addressof(epoch), alert_description::illegal_parameter);
						default:
							tls_fail_hs<crypto>(client->sock_, __builtin_addressof(epoch), alert_description::decrypt_error);
						}
						transcript.update(raw, raw + raw_size);
						flight_state = second_flight_state::want_fin;
						break;
					}
					case handshake_type::finished:
					{
						if (flight_state != second_flight_state::want_fin ||
							body_size != digest_size)
						{
							tls_fail_hs<crypto>(client->sock_, __builtin_addressof(epoch),
												flight_state != second_flight_state::want_fin
													? alert_description::unexpected_message
													: alert_description::decode_error);
						}
						/* the client MACs CH..its CertificateVerify --
						   exactly the running transcript now */
						::std::byte expect[64];
						::fast_io::tls::details::finished_verify_data_to_ptr<crypto>(
							md, expect, c_hs, transcript);
						bool same{true};
						for (::std::size_t i{}; i != digest_size; ++i)
						{
							same &= (body[i] == expect[i]);
						}
						if (!same)
						{
							tls_fail_hs<crypto>(client->sock_, __builtin_addressof(epoch), alert_description::decrypt_error);
						}
						/* keep the message bytes: the resumption
						   master secret runs over CH..client Finished
						   and the queue storage dies with this block */
						::fast_io::freestanding::non_overlapped_copy_n(
							raw, raw_size, fin_copy);
						fin_copy_size = raw_size;
						goto established;
					}
					default:
						tls_fail_hs<crypto>(client->sock_, __builtin_addressof(epoch), alert_description::unexpected_message);
					}
				}
				content_type inner{};
				::std::size_t inner_size{};
				if (!details::tls_recv_flight_record<crypto>(
						client->sock_, recbuf, sizeof(recbuf),
						__builtin_addressof(inner), __builtin_addressof(inner_size),
						suite, hs_rx_key, hs_rx_iv, hs_rx_seq))
				{
					if (chi.early_data && early_skipped != 8)
					{
						/* undecryptable records before the client's
						   c_hs epoch are early data -- skip, do not
						   advance the sequence */
						++early_skipped;
						continue;
					}
					tls_fail_hs<crypto>(client->sock_, __builtin_addressof(epoch), alert_description::bad_record_mac);
				}
				if (inner != content_type::change_cipher_spec)
				{
					if (inner_size == 0)
					{
						continue;
					}
					++hs_rx_seq;
				}
				switch (inner)
				{
				case content_type::handshake:
					handshake_queue_feed(__builtin_addressof(fq), recbuf, inner_size);
					break;
				case content_type::change_cipher_spec:
					break;
				case content_type::alert:
					details::tls_throw_peer_alert(recbuf, inner_size);
				default:
					tls_fail_hs<crypto>(client->sock_, __builtin_addressof(epoch), alert_description::unexpected_message);
				}
			}
		established:;
		}

		/* ---- epoch switch: TX <- s_ap, RX <- c_ap (keys derived
		   above -- only the offload install and the session flip
		   remain) ---- */
		if (offload)
		{
			details::ktls_offload_key(client->sock_, details::tls_tx, suite, tx_ki.key, tx_ki.iv);
			details::ktls_offload_key(client->sock_, details::tls_rx, suite, rx_ki.key, rx_ki.iv);
		}
		::std::byte tx_secret[48], rx_secret[48];
		::fast_io::freestanding::non_overlapped_copy_n(s_ap, digest_size, tx_secret);
		::fast_io::freestanding::non_overlapped_copy_n(c_ap, digest_size, rx_secret);
		tls_client_set_established(client, suite, tx_secret, rx_secret, digest_size, offload, tx_ki, rx_ki);
		::fast_io::secure_clear(tx_secret, sizeof(tx_secret));

		/* ---- session tickets: resumption master secret covers
		   CH..client Finished, so the fin message folds first. Each
		   ticket is a sealed blob of {suite, issue time, psk} -- the
		   ticket itself is the only server-side state. NSTs are
		   post-handshake records: they ride the app epoch (inner type
		   handshake) and do NOT enter the transcript */
		if (cfg->ticket_key != nullptr && cfg->tickets_to_issue != 0)
		{
			transcript.update(fin_copy, fin_copy + fin_copy_size);
			::std::byte rms[64];
			ks.derive_to_ptr(rms, u8"res master", 10, transcript);
			::std::byte nst[handshake_header_size + 4 + 4 + 2 + details::tls_ticket_size + 2];
			::std::byte ticket[details::tls_ticket_size];
			::std::byte rec[details::record_header_size + 16641 + 16];
			for (::std::size_t i{}; i != cfg->tickets_to_issue; ++i)
			{
				::std::byte resumption_psk[64];
				::std::byte nonce_i{static_cast<::std::byte>(i)};
				::fast_io::tls::details::hkdf_expand_label_to_ptr<crypto>(
					md, resumption_psk, digest_size, rms,
					u8"resumption", 10, __builtin_addressof(nonce_i), 1);
				details::tls_ticket_seal(
					ticket, *cfg->ticket_key, suite,
					static_cast<::std::uint_least64_t>(unix_now),
					resumption_psk, digest_size);
				::fast_io::secure_clear(resumption_psk, sizeof(resumption_psk));
				::std::byte age_add_b[4];
				details::tls_fill_random(age_add_b, 4);
				wire_reader ar{age_add_b, age_add_b + 4};
				::std::uint_least32_t age_add{};
				ar.take_u32(age_add);
				::std::byte const *const nst_end{details::new_session_ticket_write(
					nst, cfg->ticket_lifetime, age_add,
					static_cast<::std::uint_least8_t>(i), ticket, sizeof(ticket))};
				::std::size_t const nst_size{static_cast<::std::size_t>(nst_end - nst)};
#if defined(__linux__)
				if (offload)
				{
					if constexpr (requires { client->sock_.fd; })
					{
						details::ktls_send_record(client->sock_.fd, content_type::handshake, nst, nst_size);
					}
					else
					{
						::std::size_t const rec_size{crypto::record_seal(
							rec, content_type::handshake, nst, nst_size,
							suite, client->tx_key_, client->tx_iv_, client->tx_seq_++)};
						details::tls_write_full(client->sock_, rec, rec_size);
					}
				}
				else
#endif
				{
					::std::size_t const rec_size{crypto::record_seal(
						rec, content_type::handshake, nst, nst_size,
						suite, client->tx_key_, client->tx_iv_, client->tx_seq_++)};
					details::tls_write_full(client->sock_, rec, rec_size);
				}
			}
			::fast_io::secure_clear(rms, sizeof(rms));
		}
		::fast_io::secure_clear(rx_secret, sizeof(rx_secret));
		::fast_io::secure_clear(&tx_ki, sizeof(tx_ki));
		::fast_io::secure_clear(&rx_ki, sizeof(rx_ki));
		::fast_io::secure_clear(c_ap, sizeof(c_ap));
		::fast_io::secure_clear(s_ap, sizeof(s_ap));
		::fast_io::secure_clear(c_hs, sizeof(c_hs));
		::fast_io::secure_clear(s_hs, sizeof(s_hs));
		::fast_io::secure_clear(hs_tx_key, sizeof(hs_tx_key));
		::fast_io::secure_clear(hs_rx_key, sizeof(hs_rx_key));
		::fast_io::secure_clear(hs_tx_iv, sizeof(hs_tx_iv));
		::fast_io::secure_clear(hs_rx_iv, sizeof(hs_rx_iv));
		::fast_io::secure_clear(&ks, sizeof(ks));
		::fast_io::secure_clear(&transcript, sizeof(transcript));
	}
}
} // namespace details


/* fully-configured server handshake: caller supplies the cert chain and
   private key -- the same basic_tls_io_observer entry point the client
   config uses, overloaded on the config type */
template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto>
inline void handshake_define(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob,
							 tls_server_config const &cfg) FAST_IO_HERBCEPTIONS_THROWS
{
	details::tls_server_handshake(tob.handle, __builtin_addressof(cfg));
}

} // namespace fast_io::tls
