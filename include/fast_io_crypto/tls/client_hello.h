#pragma once

namespace fast_io::tls
{

struct client_hello
{
std::uint_least8_t handshake_type=0x01;
::fast_io::freestanding::array<std::uint_least8_t,3> size={0x00,0x01,0xFC};
std::uint_least16_t protocal_version=0x0303; 	//TLS v1.2. IT MUST BE 0x0303 even for TLS v1.3 to prevent downgrade attack.
::fast_io::freestanding::array<std::uint_least8_t,32> random{};
std::uint_least8_t session_id_length{32};
::fast_io::freestanding::array<std::uint_least8_t,32> session_id{};
::fast_io::freestanding::array<std::uint_least8_t,2> cipher_suite_length={0x00,0x02};
cipher_suite::cipher_suite_type cipher_suite{cipher_suite::tls_aes_256_gcm_sha256};
std::uint_least8_t number_of_compression_length={0x01};
std::uint_least8_t compression_method={};
};

}