#pragma once

namespace fast_io::tls::cipher_suite
{


struct cipher_suite_type
{
	::fast_io::freestanding::array<std::byte,2> array{};
};

inline constexpr bool operator==(cipher_suite_type const& a,cipher_suite_type const& b)
{
	return a.array==b.array;
}
inline constexpr bool operator!=(cipher_suite_type const& a,cipher_suite_type const& b)
{
	return a.array!=b.array;
}

/*

No TLS support before 1.2
TLS 1.2
rfc5246
page 74
https://tools.ietf.org/html/rfc5246
*/

inline constexpr cipher_suite_type tls_rsa_with_null_null                {{ std::byte(0x00),std::byte(0x00) }};
inline constexpr cipher_suite_type tls_rsa_with_null_md5                 {{ std::byte(0x00),std::byte(0x01) }};
inline constexpr cipher_suite_type tls_rsa_with_null_sha                 {{ std::byte(0x00),std::byte(0x02) }};
inline constexpr cipher_suite_type tls_rsa_with_null_sha256              {{ std::byte(0x00),std::byte(0x3B) }};
inline constexpr cipher_suite_type tls_rsa_with_rc4_128_md5              {{ std::byte(0x00),std::byte(0x04) }};
inline constexpr cipher_suite_type tls_rsa_with_rc4_128_sha              {{ std::byte(0x00),std::byte(0x05) }};
inline constexpr cipher_suite_type tls_rsa_with_3des_ede_cbc_sha         {{ std::byte(0x00),std::byte(0x0A) }};
inline constexpr cipher_suite_type tls_rsa_with_aes_128_cbc_sha          {{ std::byte(0x00),std::byte(0x2F) }};
inline constexpr cipher_suite_type tls_rsa_with_aes_256_cbc_sha          {{ std::byte(0x00),std::byte(0x35) }};
inline constexpr cipher_suite_type tls_rsa_with_aes_128_cbc_sha256       {{ std::byte(0x00),std::byte(0x3C) }};
inline constexpr cipher_suite_type tls_rsa_with_aes_256_cbc_sha256       {{ std::byte(0x00),std::byte(0x3D) }};

inline constexpr cipher_suite_type tls_dh_dss_with_3des_ede_cbc_sha      {{ std::byte(0x00),std::byte(0x0D) }};
inline constexpr cipher_suite_type tls_dh_rsa_with_3des_ede_cbc_sha      {{ std::byte(0x00),std::byte(0x10) }};
inline constexpr cipher_suite_type tls_dhe_dss_with_3des_ede_cbc_sha     {{ std::byte(0x00),std::byte(0x13) }};
inline constexpr cipher_suite_type tls_dhe_rsa_with_3des_ede_cbc_sha     {{ std::byte(0x00),std::byte(0x16) }};
inline constexpr cipher_suite_type tls_dh_dss_with_aes_128_cbc_sha       {{ std::byte(0x00),std::byte(0x30) }};
inline constexpr cipher_suite_type tls_dh_rsa_with_aes_128_cbc_sha       {{ std::byte(0x00),std::byte(0x31) }};
inline constexpr cipher_suite_type tls_dhe_dss_with_aes_128_cbc_sha      {{ std::byte(0x00),std::byte(0x32) }};
inline constexpr cipher_suite_type tls_dhe_rsa_with_aes_128_cbc_sha      {{ std::byte(0x00),std::byte(0x33) }};
inline constexpr cipher_suite_type tls_dh_dss_with_aes_256_cbc_sha       {{ std::byte(0x00),std::byte(0x36) }};
inline constexpr cipher_suite_type tls_dh_rsa_with_aes_256_cbc_sha       {{ std::byte(0x00),std::byte(0x37) }};
inline constexpr cipher_suite_type tls_dhe_dss_with_aes_256_cbc_sha      {{ std::byte(0x00),std::byte(0x38) }};
inline constexpr cipher_suite_type tls_dhe_rsa_with_aes_256_cbc_sha      {{ std::byte(0x00),std::byte(0x39) }};
inline constexpr cipher_suite_type tls_dh_dss_with_aes_128_cbc_sha256    {{ std::byte(0x00),std::byte(0x3e) }};
inline constexpr cipher_suite_type tls_dh_rsa_with_aes_128_cbc_sha256    {{ std::byte(0x00),std::byte(0x3f) }};
inline constexpr cipher_suite_type tls_dhe_dss_with_aes_128_cbc_sha256   {{ std::byte(0x00),std::byte(0x40) }};
inline constexpr cipher_suite_type tls_dhe_rsa_with_aes_128_cbc_sha256   {{ std::byte(0x00),std::byte(0x67) }};
inline constexpr cipher_suite_type tls_dh_dss_with_aes_256_cbc_sha256    {{ std::byte(0x00),std::byte(0x68) }};
inline constexpr cipher_suite_type tls_dh_rsa_with_aes_256_cbc_sha256    {{ std::byte(0x00),std::byte(0x69) }};
inline constexpr cipher_suite_type tls_dhe_dss_with_aes_256_cbc_sha256   {{ std::byte(0x00),std::byte(0x6a) }};
inline constexpr cipher_suite_type tls_dhe_rsa_with_aes_256_cbc_sha256   {{ std::byte(0x00),std::byte(0x6b) }};
/*
TLS 1.3
rfc
https://tools.ietf.org/html/rfc8446#page-133
              +------------------------------+-------------+
              | Description                  | Value       |
              +------------------------------+-------------+
              | TLS_AES_128_GCM_SHA256       | {0x13,0x01} |
              |                              |             |
              | TLS_AES_256_GCM_SHA384       | {0x13,0x02} |
              |                              |             |
              | TLS_CHACHA20_POLY1305_SHA256 | {0x13,0x03} |
              |                              |             |
              | TLS_AES_128_CCM_SHA256       | {0x13,0x04} |
              |                              |             |
              | TLS_AES_128_CCM_8_SHA256     | {0x13,0x05} |
              +------------------------------+-------------+
*/

inline constexpr cipher_suite_type tls_aes_128_gcm_sha256{{std::byte(0x13),std::byte(0x01)}};
inline constexpr cipher_suite_type tls_aes_256_gcm_sha256{{std::byte(0x13),std::byte(0x02)}};
inline constexpr cipher_suite_type tls_chacha20_poly1305_sha256{{std::byte(0x13),std::byte(0x03)}};
inline constexpr cipher_suite_type tls_aes_128_ccm_sha256{{std::byte(0x13),std::byte(0x04)}};
inline constexpr cipher_suite_type tls_aes_128_ccm_8_sha256{{std::byte(0x13),std::byte(0x05)}};

template<output_stream output>
inline constexpr void print_define(output& outp,cipher_suite_type const& e)
{
	if(e==tls_rsa_with_null_null)
		print_freestanding(outp,u8"TLS_RSA_WITH_NULL_NULL{0x00,0x00}");
	else if(e==tls_rsa_with_null_md5)
		print_freestanding(outp,u8"TLS_RSA_WITH_NULL_MD5{0x00,0x01}");
	else if(e==tls_rsa_with_null_sha)
		print_freestanding(outp,u8"TLS_RSA_WITH_NULL_SHA{0x00,0x02}");
	else if(e==tls_rsa_with_null_sha256)
		print_freestanding(outp,u8"TLS_RSA_WITH_NULL_SHA256{0x00,0x3B}");
	else if(e==tls_rsa_with_rc4_128_md5)
		print_freestanding(outp,u8"TLS_RSA_WITH_RC4_128_MD5{0x00,0x04}");
	else if(e==tls_rsa_with_rc4_128_sha)
		print_freestanding(outp,u8"TLS_RSA_WITH_RC4_128_SHA{0x00,0x05}");
	else if(e==tls_rsa_with_3des_ede_cbc_sha)
		print_freestanding(outp,u8"TLS_RSA_WITH_3DES_EDE_CBC_SHA{0x00,0x0A}");	
	else if(e==tls_rsa_with_aes_128_cbc_sha)
		print_freestanding(outp,u8"TLS_RSA_WITH_AES_128_CBC_SHA{0x00,0x2F}");
	else if(e==tls_rsa_with_aes_256_cbc_sha)
		print_freestanding(outp,u8"TLS_RSA_WITH_AES_256_CBC_SHA{0x00,0x35}");
	else if(e==tls_rsa_with_aes_128_cbc_sha256)
		print_freestanding(outp,u8"TLS_RSA_WITH_AES_128_CBC_SHA256{0x00,0x3C}");
	else if(e==tls_rsa_with_aes_256_cbc_sha256)
		print_freestanding(outp,u8"TLS_RSA_WITH_AES_256_CBC_SHA256{0x00,0x3D}");
	else if(e==tls_dh_dss_with_3des_ede_cbc_sha)
		print_freestanding(outp,u8"TLS_DH_DSS_WITH_3DES_EDE_CBC_SHA{0x00,0x0D}");
	else if(e==tls_dh_rsa_with_3des_ede_cbc_sha)
		print_freestanding(outp,u8"TLS_DH_RSA_WITH_3DES_EDE_CBC_SHA{0x00,0x10}");
	else if(e==tls_dhe_dss_with_3des_ede_cbc_sha)
		print_freestanding(outp,u8"TLS_DHE_RSS_WITH_3DES_EDE_CBC_SHA{0x00,0x13}");
	else if(e==tls_dhe_rsa_with_3des_ede_cbc_sha)
		print_freestanding(outp,u8"TLS_DHE_RSS_WITH_3DES_EDE_CBC_SHA{0x00,0x16}");
	else if(e==tls_dh_dss_with_aes_128_cbc_sha)
		print_freestanding(outp,u8"TLS_DH_DSS_WITH_AES_128_CBC_SHA{0x00,0x30}");
	else if(e==tls_dh_rsa_with_aes_128_cbc_sha)
		print_freestanding(outp,u8"TLS_DH_RSA_WITH_AES_128_CBC_SHA{0x00,0x31}");
	else if(e==tls_dhe_dss_with_aes_128_cbc_sha)
		print_freestanding(outp,u8"TLS_DHE_DSS_WITH_AES_128_CBC_SHA{0x00,0x32}");
	else if(e==tls_dhe_rsa_with_aes_128_cbc_sha)
		print_freestanding(outp,u8"TLS_DHE_RSA_WITH_AES_128_CBC_SHA{0x00,0x33}");
	else if(e==tls_dh_dss_with_aes_256_cbc_sha)
		print_freestanding(outp,u8"TLS_DH_DSS_WITH_AES_256_CBC_SHA{0x00,0x36}");
	else if(e==tls_dh_rsa_with_aes_256_cbc_sha)
		print_freestanding(outp,u8"TLS_DH_RSA_WITH_AES_256_CBC_SHA{0x00,0x37}");
	else if(e==tls_dhe_dss_with_aes_256_cbc_sha)
		print_freestanding(outp,u8"TLS_DHE_DSS_WITH_AES_256_CBC_SHA{0x00,0x38}");
	else if(e==tls_dhe_rsa_with_aes_256_cbc_sha)
		print_freestanding(outp,u8"TLS_DHE_RSA_WITH_AES_256_CBC_SHA{0x00,0x39}");
	else if(e==tls_dh_dss_with_aes_128_cbc_sha256)
		print_freestanding(outp,u8"TLS_DH_DSS_WITH_AES_128_CBC_SHA256{0x00,0x3e}");
	else if(e==tls_dh_rsa_with_aes_128_cbc_sha256)
		print_freestanding(outp,u8"TLS_DH_RSA_WITH_AES_128_CBC_SHA256{0x00,0x3f}");
	else if(e==tls_dhe_dss_with_aes_128_cbc_sha256)
		print_freestanding(outp,u8"TLS_DHE_DSS_WITH_AES_128_CBC_SHA256{0x00,0x40}");
	else if(e==tls_dhe_rsa_with_aes_128_cbc_sha256)
		print_freestanding(outp,u8"TLS_DHE_RSA_WITH_AES_128_CBC_SHA256{0x00,0x67}");
	else if(e==tls_dh_dss_with_aes_256_cbc_sha256)
		print_freestanding(outp,u8"TLS_DH_DSS_WITH_AES_256_CBC_SHA256{0x00,0x68}");
	else if(e==tls_dh_rsa_with_aes_256_cbc_sha256)
		print_freestanding(outp,u8"TLS_DH_RSA_WITH_AES_256_CBC_SHA256{0x00,0x69}");
	else if(e==tls_dhe_dss_with_aes_256_cbc_sha256)
		print_freestanding(outp,u8"TLS_DHE_DSS_WITH_AES_256_CBC_SHA256{0x00,0x6a}");
	else if(e==tls_dhe_rsa_with_aes_256_cbc_sha256)
		print_freestanding(outp,u8"TLS_DHE_RSA_WITH_AES_256_CBC_SHA256{0x00,0x6b}");
	else if(e==tls_aes_128_gcm_sha256)
		print_freestanding(outp,u8"TLS_AES_128_GCM_SHA256{0x13,0x01}");
	else if(e==tls_aes_256_gcm_sha256)
		print_freestanding(outp,u8"TLS_AES_256_GCM_SHA256{0x13,0x02}");
	else if(e==tls_chacha20_poly1305_sha256)
		print_freestanding(outp,u8"TLS_CHACHA20_POLY1305_SHA256{0x13,0x03}");
	else if(e==tls_aes_128_ccm_sha256)
		print_freestanding(outp,u8"TLS_AES_128_CCM_SHA256{0x13,0x04}");
	else if(e==tls_aes_128_ccm_8_sha256)
		print_freestanding(outp,u8"TLS_AES_128_CCM_8_SHA256{0x13,0x05}");
	else
		print_freestanding(outp,u8"Unknown TLS cipher suite{",e.array.front(),fast_io::manipulators::chvw(u8','),e.array.back(),fast_io::manipulators::chvw(u8'}'));
}

}