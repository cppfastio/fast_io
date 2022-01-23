#pragma once
#include<asio.hpp>
#include"asio_driver/socket_io_observer.h"

namespace fast_io::asio_driver
{

template<std::integral char_type>
using basic_ip_tcp_socket_io_observer = basic_socket_io_observer<asio::ip::tcp::socket,char_type>;

using ip_tcp_socket_io_observer=basic_ip_tcp_socket_io_observer<char>;
using ibuf_ip_tcp_socket_io_observer=fast_io::basic_ibuf<basic_ip_tcp_socket_io_observer<char>>;

using u8ip_tcp_socket_io_observer=basic_ip_tcp_socket_io_observer<char8_t>;
using u8ibuf_ip_tcp_socket_io_observer=fast_io::basic_ibuf<basic_ip_tcp_socket_io_observer<char8_t>>;

using wip_tcp_socket_io_observer=basic_ip_tcp_socket_io_observer<wchar_t>;
using wibuf_ip_tcp_socket_io_observer=fast_io::basic_ibuf<basic_ip_tcp_socket_io_observer<wchar_t>>;

}