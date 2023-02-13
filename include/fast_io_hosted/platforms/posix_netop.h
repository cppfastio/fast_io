#pragma once

namespace fast_io
{

template<std::integral ch_type>
inline void posix_connect(basic_posix_io_observer<ch_type> h,void const* addr,posix_socklen_t addrlen)
{
	details::posix_connect_posix_socket_impl(h.fd,addr,addrlen);
}

template<std::integral ch_type>
inline void posix_bind(basic_posix_io_observer<ch_type> h,void const* addr,posix_socklen_t addrlen)
{
	details::posix_bind_posix_socket_impl(h.fd,addr,addrlen);
}

template<std::integral ch_type>
inline void posix_listen(basic_posix_io_observer<ch_type> h,int backlog)
{
	details::posix_listen_posix_socket_impl(h.fd,backlog);
}

namespace details
{

inline int posix_tcp_connect_v4_impl(ipv4 v4,open_mode m)
{
	posix_file soc(sock_family::inet,sock_type::stream,m,sock_protocol::tcp);
	constexpr auto inet{to_posix_sock_family(sock_family::inet)};
	posix_sockaddr_in in{.sin_family=inet,.sin_port=big_endian(static_cast<std::uint_least16_t>(v4.port)),.sin_addr=v4.address};
	posix_connect(soc,__builtin_addressof(in),sizeof(in));
	return soc.release();
}

inline int posix_tcp_connect_v6_impl(ipv6 v6,open_mode m)
{
	posix_file soc(sock_family::inet6,sock_type::stream,m,sock_protocol::tcp);
	constexpr auto inet6{to_posix_sock_family(sock_family::inet6)};
	posix_sockaddr_in6 in6{.sin6_family=inet6,.sin6_port=big_endian(static_cast<std::uint_least16_t>(v6.port)),.sin6_addr=v6.address};
	posix_connect(soc,__builtin_addressof(in6),sizeof(in6));
	return soc.release();
}

inline int posix_tcp_connect_ip_impl(ip v,open_mode m)
{
	posix_file soc(v.address.isv4?sock_family::inet:sock_family::inet6,sock_type::stream,m,sock_protocol::tcp);
	if(v.address.isv4)
	{
		constexpr auto inet{to_posix_sock_family(sock_family::inet)};
		posix_sockaddr_in in{.sin_family=inet,.sin_port=big_endian(v.port),.sin_addr=v.address.address.v4};
		posix_connect(soc,__builtin_addressof(in),sizeof(in));
	}
	else
	{
		constexpr auto inet6{to_posix_sock_family(sock_family::inet6)};
		posix_sockaddr_in6 in6{.sin6_family=inet6,.sin6_port=big_endian(v.port),.sin6_addr=v.address.address.v6};
		posix_connect(soc,__builtin_addressof(in6),sizeof(in6));
	}
	return soc.release();
}

inline int posix_tcp_listen_impl(std::uint_least16_t port,open_mode m)
{
	posix_file soc(sock_family::inet,sock_type::stream,m,sock_protocol::tcp);
	constexpr auto inet{to_posix_sock_family(sock_family::inet)};
	posix_sockaddr_in in{.sin_family=inet,.sin_port=big_endian(port),.sin_addr={}};
	posix_bind_posix_socket_impl(soc.fd,__builtin_addressof(in),sizeof(in));
	posix_listen_posix_socket_impl(soc.fd,128);
	return soc.release();
}

}

template<std::integral ch_type>
inline posix_file_factory posix_accept(basic_posix_io_observer<ch_type> h,void* addr, posix_socklen_t* addrlen)
{
	return posix_file_factory{details::posix_accept_posix_socket_impl(h.fd,addr,addrlen)};
}

template<std::integral ch_type>
inline posix_file_factory tcp_accept(basic_posix_io_observer<ch_type> h)
{
	return posix_file_factory{details::posix_accept_posix_socket_impl(h.fd,nullptr,nullptr)};
}

inline posix_file_factory posix_tcp_connect(ipv4 v4,open_mode m=open_mode{})
{
	return posix_file_factory{details::posix_tcp_connect_v4_impl(v4,m)};
}

inline posix_file_factory posix_tcp_connect(ipv6 v6,open_mode m=open_mode{})
{
	return posix_file_factory{details::posix_tcp_connect_v6_impl(v6,m)};
}

inline posix_file_factory posix_tcp_connect(ip v,open_mode m=open_mode{})
{
	return posix_file_factory{details::posix_tcp_connect_ip_impl(v,m)};
}

inline posix_file_factory tcp_connect(ipv4 v4,open_mode m=open_mode{})
{
	return posix_file_factory{details::posix_tcp_connect_v4_impl(v4,m)};
}
inline posix_file_factory tcp_connect(ipv6 v6,open_mode m=open_mode{})
{
	return posix_file_factory{details::posix_tcp_connect_v6_impl(v6,m)};
}
inline posix_file_factory tcp_connect(ip v,open_mode m=open_mode{})
{
	return posix_file_factory{details::posix_tcp_connect_ip_impl(v,m)};
}
inline posix_file_factory tcp_listen(std::uint_least16_t port,open_mode m=open_mode{})
{
	return posix_file_factory{details::posix_tcp_listen_impl(port,m)};
}

template<std::integral ch_type>
using basic_native_socket_io_observer = basic_posix_io_observer<ch_type>;
template<std::integral ch_type>
using basic_native_socket_file = basic_posix_file<ch_type>;
using net_service = posix_empty_network_service;
}
