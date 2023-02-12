#pragma once

namespace fast_io
{

using win32_socklen_t = int;

template<win32_family family,std::integral ch_type>
class basic_win32_family_socket_io_observer
{
public:
	using char_type = ch_type;
	using native_handle_type = std::uintptr_t;
	native_handle_type hsocket{};
	constexpr native_handle_type release() noexcept
	{
		native_handle_type temp{hsocket};
		hsocket={};
		return temp;
	}
	constexpr native_handle_type native_handle() const noexcept
	{
		return hsocket;
	}
};

template<win32_family family,std::integral char_type>
inline constexpr basic_win32_family_socket_io_observer<family,char_type> io_value_handle(basic_win32_family_socket_io_observer<family,char_type> value) noexcept
{
	return value;
}

namespace win32::details
{
inline std::uint_least32_t win32_socket_write_simple_impl(std::uintptr_t socket, void const* data,std::uint_least32_t len)
{
	wsabuf buffer{len,const_cast<char*>(reinterpret_cast<char const*>(data))};
	std::uint_least32_t sent{};
	if(::fast_io::win32::WSASend(socket,__builtin_addressof(buffer),1,__builtin_addressof(sent),0,nullptr,nullptr))
		throw_win32_error(static_cast<std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	return sent;
}

inline std::size_t win32_socket_write_impl(std::uintptr_t socket, void const* data,std::size_t to_write)
{
	if constexpr(sizeof(std::uint_least32_t)<sizeof(std::size_t))
	{
		std::size_t written{};
		for(;to_write;)
		{
			std::uint_least32_t to_write_this_round{UINT_LEAST32_MAX};
			if(to_write<static_cast<std::size_t>(UINT_LEAST32_MAX))
				to_write_this_round=static_cast<std::uint_least32_t>(to_write);
			std::uint_least32_t number_of_bytes_written{win32_socket_write_simple_impl(socket,data,to_write_this_round)};
			written+=number_of_bytes_written;
			if(number_of_bytes_written<to_write_this_round)
				break;
			to_write-=to_write_this_round;
		}
		return written;
	}
	else
	{
		return win32_socket_write_simple_impl(socket,data,static_cast<std::uint_least32_t>(to_write));
	}
}
inline std::size_t win32_socket_read_impl(std::uintptr_t socket, void* data,std::size_t to_read)
{
	if constexpr(sizeof(int)<sizeof(std::size_t)||(sizeof(int)==sizeof(std::size_t)))
	{
		constexpr std::size_t intmax{static_cast<std::size_t>(static_cast<unsigned>(std::numeric_limits<int>::max()))};
		if(intmax<to_read)
			to_read=intmax;
	}
	int recved{::fast_io::win32::recv(socket,reinterpret_cast<char*>(data),static_cast<int>(static_cast<unsigned>(to_read)),0)};
	if(recved==-1)
		throw_win32_error(static_cast<std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	return static_cast<std::size_t>(static_cast<int>(recved));
}

inline io_scatter_status_t win32_socket_scatter_read_impl(std::uintptr_t socket,io_scatter_t* scatter,std::size_t len)
{
	if(len==0)
		return {0,0,0};
	io_scatter_t sc0{*scatter};
	return {win32_socket_read_impl(socket,const_cast<void*>(sc0.base),sc0.len),1,0};
}

inline io_scatter_status_t win32_socket_scatter_write_impl(std::uintptr_t socket,io_scatter_t* scatter,std::size_t len)
{
	std::size_t written{};
	for(std::size_t i{};i!=len;++i)
	{
		io_scatter_t sc{scatter[i]};
		std::size_t written_this_round{win32_socket_write_impl(socket,sc.base,sc.len)};
		written+=written_this_round;
		if(written_this_round!=sc.len)[[unlikely]]
			return {written,i,written_this_round};
	}
	return {written,len,0};
}

inline void posix_connect_win32_socket_impl(std::uintptr_t hsocket,void const* addr,int addrlen)
{
	if(::fast_io::win32::WSAConnect(hsocket,addr,addrlen,nullptr,nullptr,nullptr,nullptr))
		throw_win32_error(static_cast<std::uint_least32_t>(WSAGetLastError()));
}

inline void posix_bind_win32_socket_impl(std::uintptr_t hsocket,void const* addr,int addrlen)
{
	if(::fast_io::win32::bind(hsocket,addr,addrlen)==-1)
		throw_win32_error(static_cast<std::uint_least32_t>(WSAGetLastError()));
}

inline void posix_listen_win32_socket_impl(std::uintptr_t hsocket,int backlog)
{
	if(::fast_io::win32::listen(hsocket,backlog)==-1)
		throw_win32_error(static_cast<std::uint_least32_t>(WSAGetLastError()));
}

inline std::uintptr_t posix_accept_win32_socket_impl(std::uintptr_t hsocket,void* addr,int* addrlen)
{
	std::uintptr_t accepted_socket{::fast_io::win32::WSAAccept(hsocket,addr,addrlen,nullptr,0)};
	if(accepted_socket==static_cast<std::uintptr_t>(-1))
		throw_win32_error(static_cast<std::uint_least32_t>(WSAGetLastError()));
	return accepted_socket;
}

}

template<win32_family family,std::integral ch_type,::std::contiguous_iterator Iter>
[[nodiscard]]
inline constexpr Iter read(basic_win32_family_socket_io_observer<family,ch_type> sockiob,Iter first,Iter last) 
{
	return win32::details::win32_socket_read_impl(sockiob.hsocket,::std::to_address(first),sizeof(*first)*static_cast<std::size_t>(::std::to_address(last)-::std::to_address(first)))/sizeof(*first)+first;
}

template<win32_family family,std::integral ch_type,::std::contiguous_iterator Iter>
inline constexpr Iter write(basic_win32_family_socket_io_observer<family,ch_type> sockiob,Iter first,Iter last)
{
	return win32::details::win32_socket_write_impl(sockiob.hsocket,::std::to_address(first),sizeof(*first)*static_cast<std::size_t>(::std::to_address(last)-::std::to_address(first)))/sizeof(*first)+first;
}

template<win32_family family,std::integral ch_type>
[[nodiscard]]
inline constexpr io_scatter_status_t scatter_read(basic_win32_family_socket_io_observer<family,ch_type> sockiob,io_scatters_t scatters)
{
	return win32::details::win32_socket_scatter_read_impl(sockiob.hsocket,const_cast<io_scatter_t*>(scatters.base),scatters.len);
}

template<win32_family family,std::integral ch_type>
inline constexpr io_scatter_status_t scatter_write(basic_win32_family_socket_io_observer<family,ch_type> sockiob,io_scatters_t scatters)
{
	return win32::details::win32_socket_scatter_write_impl(sockiob.hsocket, const_cast<io_scatter_t*>(scatters.base),scatters.len);
}

template<win32_family family,std::integral ch_type>
inline void posix_connect(basic_win32_family_socket_io_observer<family,ch_type> sockiob,void const* remote_address,win32_socklen_t address_struct_size)
{
	win32::details::posix_connect_win32_socket_impl(sockiob.hsocket,remote_address,address_struct_size);
}

template<win32_family family,std::integral ch_type>
inline void posix_bind(basic_win32_family_socket_io_observer<family,ch_type> h,void const* addr,win32_socklen_t addrlen)
{
	win32::details::posix_bind_win32_socket_impl(h.hsocket,addr,addrlen);
}

template<win32_family family,std::integral ch_type>
inline void posix_listen(basic_win32_family_socket_io_observer<family,ch_type> h,int backlog)
{
	win32::details::posix_listen_win32_socket_impl(h.hsocket,backlog);
}

namespace win32::details
{

inline std::uintptr_t win32_duphsocket(std::uintptr_t s)
{
	return reinterpret_cast<std::uintptr_t>(win32_dup_impl(reinterpret_cast<void*>(s)));
}
inline std::uintptr_t win32_dup2hsocket(std::uintptr_t handle,std::uintptr_t newhandle)
{
	auto temp{win32_duphsocket(handle)};
	if(newhandle)[[likely]]
		closesocket(newhandle);
	return temp;
}

}

inline constexpr int to_win32_sock_family(sock_family dom) noexcept
{
	switch(dom)
	{
	case sock_family::local:
		return 1;
	case sock_family::inet:
		return 2;
	case sock_family::implink:
		return 3;
	case sock_family::pup:
		return 4;
	case sock_family::chaos:
		return 5;
	case sock_family::ipx:
		return 6;
	case sock_family::iso:
		return 7;
	case sock_family::ecma:
		return 8;
	case sock_family::datakit:
		return 9;
	case sock_family::ccitt:
		return 10;
	case sock_family::sna:
		return 11;
	case sock_family::decnet:
		return 12;
	case sock_family::dli:
		return 13;
	case sock_family::lat:
		return 14;
	case sock_family::hylink:
		return 15;
	case sock_family::appletalk:
		return 16;
	case sock_family::netbios:
		return 17;
	case sock_family::voiceview:
		return 18;
	case sock_family::firefox:
		return 19;
	case sock_family::ban:
		return 21;
	case sock_family::atm:
		return 22;
	case sock_family::inet6:
		return 23;
	case sock_family::cluster:
		return 24;
	case sock_family::ieee12844:
		return 25;
	case sock_family::irda:
		return 26;
	case sock_family::netdes:
		return 28;
	case sock_family::tcnprocess:
		return 29;
	case sock_family::tcnmessage:
		return 30;
	case sock_family::iclfxbm:
		return 31;
	case sock_family::bluetooth:
		return 32;
	case sock_family::can:
		return 65537;
	default:
		return 0;
	}
}


inline constexpr int to_win32_sock_protocol(sock_protocol prot) noexcept
{
	switch(prot)
	{
	case sock_protocol::ip:
		return 0;
	case sock_protocol::hopopts:
		return 0;
	case sock_protocol::icmp:
		return 1;
	case sock_protocol::igmp:
		return 2;
	case sock_protocol::ggp:
		return 3;
	case sock_protocol::ipv4:
		return 4;
	case sock_protocol::st:
		return 5;
	case sock_protocol::tcp:
		return 6;
	case sock_protocol::cbt:
		return 7;
	case sock_protocol::egp:
		return 8;
	case sock_protocol::igp:
		return 9;
	case sock_protocol::pup:
		return 12;
	case sock_protocol::udp:
		return 17;
	case sock_protocol::idp:
		return 22;
	case sock_protocol::rdp:
		return 27;
	case sock_protocol::ipv6:
		return 41;
	case sock_protocol::routing:
		return 43;
	case sock_protocol::fragment:
		return 44;
	case sock_protocol::icmpv6:
		return 58;
	case sock_protocol::none:
		return 59;
	case sock_protocol::dstopts:
		return 60;
	case sock_protocol::nd:
		return 77;
	case sock_protocol::iclfxbm:
		return 78;
	case sock_protocol::pim:
		return 103;
	case sock_protocol::pgm:
		return 113;
	case sock_protocol::l2tp:
		return 115;
	case sock_protocol::sctp:
		return 132;
	case sock_protocol::raw:
		return 255;
	default:
		return -1;
	}
}

inline constexpr int to_win32_sock_type(sock_type dom) noexcept
{
	switch(dom)
	{
		case sock_type::stream:
			return 1;
		case sock_type::dgram:
			return 2;
		case sock_type::seqpacket:
			return 5;
		case sock_type::raw:
			return 3;
		case sock_type::rdm:
			return 4;
		default:
			return -1;
	};
}

inline constexpr std::uint_least32_t to_win32_sock_open_mode(open_mode m) noexcept
{
	std::uint_least32_t flags{};
	if((m&open_mode::no_block)==open_mode::no_block)
		flags|=0x01;
#if defined(_WIN32_WINDOWS) || _WIN32_WINNT >= 0x0602
//this flag only supports after windows 7 sp1. So we start supporting this flag from windows 8
	if((m&open_mode::inherit)!=open_mode::inherit)
		flags|=0x80;
#endif
	return flags;
}

inline constexpr std::uint_least32_t to_win32_sock_open_mode_9xa(open_mode m) noexcept
{
	std::uint_least32_t flags{};
	if((m&open_mode::no_block)==open_mode::no_block)
		flags|=0x01;
	return flags;
}


#if !defined(__CYGWIN__) && !defined(__WINE__)

inline constexpr int to_native_sock_family(sock_family dom) noexcept
{
	return to_win32_sock_family(dom);
}

inline constexpr int to_native_sock_type(sock_type dom) noexcept
{
	return to_win32_sock_type(dom);
}

inline constexpr int to_native_sock_protocol(sock_protocol prot) noexcept
{
	return to_win32_sock_protocol(prot);
}

inline constexpr std::uint_least32_t to_native_sock_open_mode(open_mode m) noexcept
{
	return to_win32_sock_open_mode(m);
}

#endif

namespace win32::details
{

template<win32_family family>
inline std::uintptr_t open_win32_socket_raw_impl(int af,int tp,int prt,std::uint_least32_t dwflags)
{
	if constexpr(family==win32_family::wide_nt)
	{
		std::uintptr_t ret{::fast_io::win32::WSASocketW(af,tp,prt,nullptr,0,dwflags)};
		if(ret==UINTPTR_MAX)
			throw_win32_error(static_cast<std::uint_least32_t>(WSAGetLastError()));
		return ret;
	}
	else
	{
		std::uintptr_t ret{::fast_io::win32::WSASocketA(af,tp,prt,nullptr,0,dwflags)};
		if(ret==UINTPTR_MAX)
			throw_win32_error(static_cast<std::uint_least32_t>(WSAGetLastError()));
		return ret;
	}
}

template<win32_family family>
inline std::uintptr_t open_win32_socket_raw_om_custom_only_impl(int af,int tp,int prt,open_mode om)
{
	if constexpr(family==::fast_io::win32_family::wide_nt)
	{
		return open_win32_socket_raw_impl<family>(af,tp,prt,to_win32_sock_open_mode(om));
	}
	else
	{
		return open_win32_socket_raw_impl<family>(af,tp,prt,to_win32_sock_open_mode_9xa(om));
	}
}

template<win32_family family>
inline std::uintptr_t open_win32_socket_impl(sock_family d,sock_type t,open_mode m,sock_protocol p)
{
	return open_win32_socket_raw_om_custom_only_impl<family>(to_win32_sock_family(d),to_win32_sock_type(t),to_win32_sock_protocol(p),m);
}

}

struct
#if __has_cpp_attribute(clang::trivially_relocatable)
[[clang::trivially_relocatable]]
#endif
win32_socket_factory
{
	using native_handle_type = std::uintptr_t;
	std::uintptr_t hsocket{};
	explicit constexpr win32_socket_factory(std::uintptr_t v) noexcept:hsocket(v){}
	win32_socket_factory(win32_socket_factory const&)=delete;
	win32_socket_factory& operator=(win32_socket_factory const&)=delete;
	~win32_socket_factory()
	{
		if(hsocket)[[likely]]
			win32::closesocket(hsocket);
	}
};


template<win32_family family,std::integral ch_type>
inline win32_socket_factory posix_accept(basic_win32_family_socket_io_observer<family,ch_type> h,void* addr,win32_socklen_t* addrlen)
{
	return win32_socket_factory{win32::details::posix_accept_win32_socket_impl(h.hsocket,addr,addrlen)};
}

template<win32_family family,std::integral ch_type>
inline win32_socket_factory tcp_accept(basic_win32_family_socket_io_observer<family,ch_type> h)
{
	return win32_socket_factory{win32::details::posix_accept_win32_socket_impl(h.hsocket,nullptr,nullptr)};
}

template<win32_family family,std::integral ch_type>
class
#if __has_cpp_attribute(clang::trivially_relocatable)
[[clang::trivially_relocatable]]
#endif
basic_win32_family_socket_file : public basic_win32_family_socket_io_observer<family,ch_type>
{
public:
	using typename basic_win32_family_socket_io_observer<family,ch_type>::char_type;
	using typename basic_win32_family_socket_io_observer<family,ch_type>::native_handle_type;
	constexpr basic_win32_family_socket_file() noexcept=default;

	constexpr basic_win32_family_socket_file(basic_win32_family_socket_io_observer<family,ch_type>) noexcept=delete;
	constexpr basic_win32_family_socket_file& operator=(basic_win32_family_socket_io_observer<family,ch_type>) noexcept=delete;

	template<typename native_hd>
	requires std::same_as<native_handle_type,std::remove_cvref_t<native_hd>>
	explicit constexpr basic_win32_family_socket_file(native_hd hsocket1) noexcept: basic_win32_family_socket_io_observer<family,ch_type>{hsocket1}
	{}
	explicit constexpr basic_win32_family_socket_file(decltype(nullptr)) noexcept=delete;
	basic_win32_family_socket_file(io_dup_t,basic_win32_family_socket_io_observer<family,ch_type> wsiob):basic_win32_family_socket_io_observer<family,ch_type>{win32::details::win32_duphsocket(wsiob.hsocket)}
	{}
	basic_win32_family_socket_file(sock_family d,sock_type t,open_mode m,sock_protocol p)
		:basic_win32_family_socket_file<family,ch_type>{win32::details::open_win32_socket_impl<family>(d,t,m,p)}
	{}

	basic_win32_family_socket_file(basic_win32_family_socket_file const& dp):basic_win32_family_socket_io_observer<family,char_type>{win32::details::win32_duphsocket(dp.hsocket)}
	{
	}
	basic_win32_family_socket_file& operator=(basic_win32_family_socket_file const& dp)
	{
		this->hsocket=win32::details::win32_dup2hsocket(dp.hsocket,this->hsocket);
		return *this;
	}
	constexpr basic_win32_family_socket_file(basic_win32_family_socket_file&& __restrict b) noexcept : basic_win32_family_socket_io_observer<family,char_type>{b.hsocket}
	{
		b.hsocket = 0;
	}
	basic_win32_family_socket_file& operator=(basic_win32_family_socket_file&& __restrict b) noexcept
	{
		if(this->hsocket)[[likely]]
			::fast_io::win32::closesocket(this->hsocket);
		this->hsocket=b.hsocket;
		b.hsocket = 0;
		return *this;
	}
	constexpr void reset(native_handle_type newhsocket=0) noexcept
	{
		if(this->hsocket)[[likely]]
			::fast_io::win32::closesocket(this->hsocket);
		this->hsocket=newhsocket;
	}
	void close()
	{
		if(this->hsocket)[[likely]]
		{
			auto ret{::fast_io::win32::closesocket(this->hsocket)};
			this->hsocket=0;//POSIX standard says we should never call close(2) again even close syscall fails
			if(ret)
				throw_win32_error(static_cast<std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
		}
	}

	explicit constexpr basic_win32_family_socket_file(win32_socket_factory&& other) noexcept : basic_win32_family_socket_io_observer<family,char_type>{other.hsocket}
	{
		other.hsocket=0;
	}
	~basic_win32_family_socket_file()
	{
		if(this->hsocket)[[likely]]
			::fast_io::win32::closesocket(this->hsocket);
	}
};


namespace details
{

template<win32_family family>
inline std::uintptr_t win32_family_tcp_connect_v4_impl(ipv4 v4,open_mode m)
{
	basic_win32_family_socket_file<family,char> soc(sock_family::inet,sock_type::stream,m,sock_protocol::tcp);
	constexpr auto inet{to_win32_sock_family(sock_family::inet)};
	posix_sockaddr_in in{.sin_family=inet,.sin_port=big_endian(static_cast<std::uint_least16_t>(v4.port)),.sin_addr=v4.address};
	posix_connect(soc,__builtin_addressof(in),sizeof(in));
	return soc.release();
}


template<win32_family family>
inline std::uintptr_t win32_family_tcp_connect_v6_impl(ipv6 v6,open_mode m)
{
	basic_win32_family_socket_file<family,char> soc(sock_family::inet6,sock_type::stream,m,sock_protocol::tcp);
	constexpr auto inet6{to_win32_sock_family(sock_family::inet6)};
	posix_sockaddr_in6 in6{.sin6_family=inet6,.sin6_port=big_endian(static_cast<std::uint_least16_t>(v6.port)),.sin6_addr=v6.address};
	posix_connect(soc,__builtin_addressof(in6),sizeof(in6));
	return soc.release();
}

template<win32_family family>
inline std::uintptr_t win32_family_tcp_connect_ip_impl(ip v,open_mode m)
{
	basic_win32_family_socket_file<family,char> soc(v.address.isv4?sock_family::inet:sock_family::inet6,sock_type::stream,m,sock_protocol::tcp);
	if(v.address.isv4)
	{
		constexpr auto inet{to_win32_sock_family(sock_family::inet)};
		posix_sockaddr_in in{.sin_family=inet,.sin_port=big_endian(v.port),.sin_addr=v.address.address.v4};
		posix_connect(soc,__builtin_addressof(in),sizeof(in));
	}
	else
	{
		constexpr auto inet6{to_win32_sock_family(sock_family::inet6)};
		posix_sockaddr_in6 in6{.sin6_family=inet6,.sin6_port=big_endian(v.port),.sin6_addr=v.address.address.v6};
		posix_connect(soc,__builtin_addressof(in6),sizeof(in6));
	}
	return soc.release();
}

inline void win32_tcp_listen_common_impl(std::uintptr_t hsocket,std::uint_least16_t port)
{
	constexpr auto inet{to_win32_sock_family(sock_family::inet)};
	posix_sockaddr_in in{.sin_family=inet,.sin_port=big_endian(port),.sin_addr={}};
	::fast_io::win32::details::posix_bind_win32_socket_impl(hsocket,__builtin_addressof(in),sizeof(in));
	::fast_io::win32::details::posix_listen_win32_socket_impl(hsocket,128);
}

template<win32_family family>
inline std::uintptr_t win32_tcp_listen_impl(std::uint_least16_t port,open_mode m)
{
	basic_win32_family_socket_file<family,char> soc(sock_family::inet,sock_type::stream,m,sock_protocol::tcp);
	win32_tcp_listen_common_impl(soc.hsocket,port);
	return soc.release();
}

}

template<win32_family family>
inline win32_socket_factory win32_family_tcp_connect(ipv4 v4,open_mode m=open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_v4_impl<family>(v4,m)};
}

inline win32_socket_factory win32_tcp_connect_9xa(ipv4 v4,open_mode m=open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_v4_impl<win32_family::ansi_9x>(v4,m)};
}

inline win32_socket_factory win32_tcp_connect_ntw(ipv4 v4,open_mode m=open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_v4_impl<win32_family::wide_nt>(v4,m)};
}

inline win32_socket_factory win32_tcp_connect(ipv4 v4,open_mode m=open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_v4_impl<win32_family::native>(v4,m)};
}


template<win32_family family>
inline win32_socket_factory win32_family_tcp_connect(ipv6 v6,open_mode m=open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_v6_impl<family>(v6,m)};
}

inline win32_socket_factory win32_tcp_connect_9xa(ipv6 v6,open_mode m=open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_v6_impl<win32_family::ansi_9x>(v6,m)};
}

inline win32_socket_factory win32_tcp_connect_ntw(ipv6 v6,open_mode m=open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_v6_impl<win32_family::wide_nt>(v6,m)};
}

inline win32_socket_factory win32_tcp_connect(ipv6 v6,open_mode m=open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_v6_impl<win32_family::native>(v6,m)};
}

inline win32_socket_factory win32_tcp_connect_9xa(ip v,open_mode m=open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_ip_impl<win32_family::ansi_9x>(v,m)};
}

inline win32_socket_factory win32_tcp_connect_ntw(ip v,open_mode m=open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_ip_impl<win32_family::wide_nt>(v,m)};
}

inline win32_socket_factory win32_tcp_connect(ip v,open_mode m=open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_ip_impl<win32_family::native>(v,m)};
}


template<win32_family family>
inline win32_socket_factory win32_family_tcp_listen(std::uint_least16_t port,open_mode m=open_mode{})
{
	return win32_socket_factory{details::win32_tcp_listen_impl<family>(port,m)};
}

inline win32_socket_factory win32_tcp_listen_ntw(std::uint_least16_t port,open_mode m=open_mode{})
{
	return win32_socket_factory{details::win32_tcp_listen_impl<win32_family::wide_nt>(port,m)};
}

inline win32_socket_factory win32_tcp_listen_9xa(std::uint_least16_t port,open_mode m=open_mode{})
{
	return win32_socket_factory{details::win32_tcp_listen_impl<win32_family::ansi_9x>(port,m)};
}

inline win32_socket_factory win32_tcp_listen(std::uint_least16_t port,open_mode m=open_mode{})
{
	return win32_socket_factory{details::win32_tcp_listen_impl<win32_family::native>(port,m)};
}

template<std::integral ch_type>
using basic_win32_socket_io_observer_9xa = basic_win32_family_socket_io_observer<win32_family::ansi_9x,ch_type>;

using win32_socket_io_observer_9xa=basic_win32_socket_io_observer_9xa<char>;
using wwin32_socket_io_observer_9xa=basic_win32_socket_io_observer_9xa<wchar_t>;
using u8win32_socket_io_observer_9xa=basic_win32_socket_io_observer_9xa<char8_t>;
using u16win32_socket_io_observer_9xa=basic_win32_socket_io_observer_9xa<char16_t>;
using u32win32_socket_io_observer_9xa=basic_win32_socket_io_observer_9xa<char32_t>;

template<std::integral ch_type>
using basic_win32_socket_io_observer_ntw = basic_win32_family_socket_io_observer<win32_family::wide_nt,ch_type>;

using win32_socket_io_observer_ntw=basic_win32_socket_io_observer_ntw<char>;
using wwin32_socket_io_observer_ntw=basic_win32_socket_io_observer_ntw<wchar_t>;
using u8win32_socket_io_observer_ntw=basic_win32_socket_io_observer_ntw<char8_t>;
using u16win32_socket_io_observer_ntw=basic_win32_socket_io_observer_ntw<char16_t>;
using u32win32_socket_io_observer_ntw=basic_win32_socket_io_observer_ntw<char32_t>;

template<std::integral ch_type>
using basic_win32_socket_io_observer = basic_win32_family_socket_io_observer<win32_family::native,ch_type>;

using win32_socket_io_observer=basic_win32_socket_io_observer<char>;
using wwin32_socket_io_observer=basic_win32_socket_io_observer<wchar_t>;
using u8win32_socket_io_observer=basic_win32_socket_io_observer<char8_t>;
using u16win32_socket_io_observer=basic_win32_socket_io_observer<char16_t>;
using u32win32_socket_io_observer=basic_win32_socket_io_observer<char32_t>;

template<std::integral ch_type>
using basic_win32_socket_file_9xa = basic_win32_family_socket_file<win32_family::ansi_9x,ch_type>;

using win32_socket_file_9xa=basic_win32_socket_file_9xa<char>;
using wwin32_socket_file_9xa=basic_win32_socket_file_9xa<wchar_t>;
using u8win32_socket_file_9xa=basic_win32_socket_file_9xa<char8_t>;
using u16win32_socket_file_9xa=basic_win32_socket_file_9xa<char16_t>;
using u32win32_socket_file_9xa=basic_win32_socket_file_9xa<char32_t>;

template<std::integral ch_type>
using basic_win32_socket_file_ntw = basic_win32_family_socket_file<win32_family::wide_nt,ch_type>;

using win32_socket_file_ntw=basic_win32_socket_file_ntw<char>;
using wwin32_socket_file_ntw=basic_win32_socket_file_ntw<wchar_t>;
using u8win32_socket_file_ntw=basic_win32_socket_file_ntw<char8_t>;
using u16win32_socket_file_ntw=basic_win32_socket_file_ntw<char16_t>;
using u32win32_socket_file_ntw=basic_win32_socket_file_ntw<char32_t>;

template<std::integral ch_type>
using basic_win32_socket_file = basic_win32_family_socket_file<win32_family::native,ch_type>;

using win32_socket_file=basic_win32_socket_file<char>;
using wwin32_socket_file=basic_win32_socket_file<wchar_t>;
using u8win32_socket_file=basic_win32_socket_file<char8_t>;
using u16win32_socket_file=basic_win32_socket_file<char16_t>;
using u32win32_socket_file=basic_win32_socket_file<char32_t>;

#if !defined(__CYGWIN__) && !defined(__WINE__)

template<std::integral ch_type>
using basic_native_socket_io_observer = basic_win32_socket_io_observer<ch_type>;
template<std::integral ch_type>
using basic_native_socket_file = basic_win32_socket_file<ch_type>;
using native_socklen_t = win32_socklen_t;

inline win32_socket_factory tcp_connect(ipv4 v4,open_mode m=open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_v4_impl<win32_family::native>(v4,m)};
}
inline win32_socket_factory tcp_connect(ipv6 v6,open_mode m=open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_v6_impl<win32_family::native>(v6,m)};
}
inline win32_socket_factory tcp_connect(ip v,open_mode m=open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_ip_impl<win32_family::native>(v,m)};
}

inline win32_socket_factory tcp_listen(std::uint_least16_t port,open_mode m=open_mode{})
{
	return win32_socket_factory{details::win32_tcp_listen_impl<win32_family::native>(port,m)};
}

#endif

namespace freestanding
{

template<win32_family fm,std::integral char_type>
struct is_trivially_relocatable<basic_win32_family_socket_file<fm,char_type>>
{
	inline static constexpr bool value = true;
};

template<win32_family fm,std::integral char_type>
struct is_zero_default_constructible<basic_win32_family_socket_file<fm,char_type>>
{
	inline static constexpr bool value = true;
};

}

}

