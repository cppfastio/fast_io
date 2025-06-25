#pragma once

namespace fast_io
{

using win32_socklen_t = int;

template <win32_family family, ::std::integral ch_type>
class basic_win32_family_socket_io_observer
{
public:
	using char_type = ch_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
	using native_handle_type = ::std::size_t;
	native_handle_type hsocket{};
	inline constexpr native_handle_type release() noexcept
	{
		native_handle_type temp{hsocket};
		hsocket = {};
		return temp;
	}
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return hsocket;
	}
};

template <win32_family family, ::std::integral char_type>
inline constexpr basic_win32_family_socket_io_observer<family, char_type>
io_stream_ref_define(basic_win32_family_socket_io_observer<family, char_type> value) noexcept
{
	return value;
}

template <win32_family family, ::std::integral char_type>
inline constexpr basic_win32_family_socket_io_observer<family, char>
io_bytes_stream_ref_define(basic_win32_family_socket_io_observer<family, char_type> value) noexcept
{
	return {value.hsocket};
}

namespace win32::details
{
inline ::std::byte const *win32_socket_write_bytes_impl(::std::size_t socket, ::std::byte const *first,
														::std::byte const *last)
{
	wsabuf buffer{::fast_io::details::read_write_bytes_compute<::std::uint_least32_t>(first, last),
				  const_cast<char *>(reinterpret_cast<char const *>(first))};
	::std::uint_least32_t sent{};
	if (::fast_io::win32::WSASend(socket, __builtin_addressof(buffer), 1, __builtin_addressof(sent), 0, nullptr,
								  nullptr))
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	}
	return first + sent;
}

inline ::std::byte *win32_socket_read_bytes_impl(::std::size_t socket, ::std::byte *first, ::std::byte *last)
{
	int recved{::fast_io::win32::recv(socket, reinterpret_cast<char *>(first),
									  ::fast_io::details::read_write_bytes_compute<::std::int_least32_t>(first, last),
									  0)};
	if (recved == -1)
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	}
	return first + recved;
}

inline void posix_connect_win32_socket_impl(::std::size_t hsocket, void const *addr, int addrlen)
{
	if (::fast_io::win32::WSAConnect(hsocket, addr, addrlen, nullptr, nullptr, nullptr, nullptr))
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	}
}

inline void posix_bind_win32_socket_impl(::std::size_t hsocket, void const *addr, int addrlen)
{
	if (::fast_io::win32::bind(hsocket, addr, addrlen) == -1)
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	}
}

inline void posix_listen_win32_socket_impl(::std::size_t hsocket, int backlog)
{
	if (::fast_io::win32::listen(hsocket, backlog) == -1)
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	}
}

inline ::std::size_t posix_accept_win32_socket_impl(::std::size_t hsocket, void *addr, int *addrlen)
{
	::std::size_t accepted_socket{::fast_io::win32::WSAAccept(hsocket, addr, addrlen, nullptr, 0)};
	if (accepted_socket == static_cast<::std::size_t>(-1))
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	}
	return accepted_socket;
}

inline ::std::ptrdiff_t posix_recvfrom_win32_socket_impl(::std::size_t hsocket, void *buf, ::std::size_t len, int flags, void *src_addr, int *addrlen)
{
	if (len > INT_MAX)
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(87u));
	}

	auto soc{::fast_io::win32::recvfrom(hsocket, reinterpret_cast<char *>(buf), static_cast<int>(len), flags, src_addr, addrlen)};
	if (soc == -1)
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	}
	return soc;
}

inline ::std::ptrdiff_t posix_sendto_win32_socket_impl(::std::size_t hsocket, void const *msg, ::std::size_t len, int flags, void const *to, int tolen)
{
	if (len > INT_MAX)
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(87u));
	}

	auto soc{::fast_io::win32::sendto(hsocket, reinterpret_cast<char const *>(msg), static_cast<int>(len), flags, to, tolen)};

	if (soc == -1)
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	}
	return soc;
}
} // namespace win32::details

template <win32_family family, ::std::integral ch_type>
inline ::std::byte *read_some_bytes_underflow_define(basic_win32_family_socket_io_observer<family, ch_type> wiob,
													 ::std::byte *first, ::std::byte *last)
{
	return ::fast_io::win32::details::win32_socket_read_bytes_impl(wiob.hsocket, first, last);
}

template <win32_family family, ::std::integral ch_type>
inline ::std::byte const *write_some_bytes_overflow_define(basic_win32_family_socket_io_observer<family, ch_type> wiob,
														   ::std::byte const *first, ::std::byte const *last)
{
	return ::fast_io::win32::details::win32_socket_write_bytes_impl(wiob.hsocket, first, last);
}

template <win32_family family, ::std::integral ch_type>
inline void posix_connect(basic_win32_family_socket_io_observer<family, ch_type> sockiob, void const *remote_address,
						  win32_socklen_t address_struct_size)
{
	win32::details::posix_connect_win32_socket_impl(sockiob.hsocket, remote_address, address_struct_size);
}

template <win32_family family, ::std::integral ch_type>
inline void posix_bind(basic_win32_family_socket_io_observer<family, ch_type> h, void const *addr,
					   win32_socklen_t addrlen)
{
	win32::details::posix_bind_win32_socket_impl(h.hsocket, addr, addrlen);
}

template <win32_family family, ::std::integral ch_type>
inline void posix_listen(basic_win32_family_socket_io_observer<family, ch_type> h, int backlog)
{
	win32::details::posix_listen_win32_socket_impl(h.hsocket, backlog);
}

template <win32_family family, ::std::integral ch_type>
inline ::std::ptrdiff_t posix_recvfrom(basic_win32_family_socket_io_observer<family, ch_type> h, void *buf, ::std::size_t len,
									   int flags, void *src_addr, win32_socklen_t *addrlen)
{
	return win32::details::posix_recvfrom_win32_socket_impl(h.hsocket, buf, len, flags, src_addr, addrlen);
}

template <win32_family family, ::std::integral ch_type>
inline ::std::ptrdiff_t posix_sendto(basic_win32_family_socket_io_observer<family, ch_type> h, void const *msg, ::std::size_t len,
									 int flags, void const *to, win32_socklen_t tolen)
{
	return win32::details::posix_sendto_win32_socket_impl(h.hsocket, msg, len, flags, to, tolen);
}

namespace win32::details
{

inline ::std::size_t win32_duphsocket(::std::size_t s)
{
	return reinterpret_cast<::std::size_t>(win32_dup_impl(reinterpret_cast<void *>(s)));
}
inline ::std::size_t win32_dup2hsocket(::std::size_t handle, ::std::size_t newhandle)
{
	auto temp{win32_duphsocket(handle)};
	if (newhandle) [[likely]]
	{
		::fast_io::win32::closesocket(newhandle);
	}
	return temp;
}

} // namespace win32::details

inline constexpr int to_win32_sock_family(sock_family dom) noexcept
{
	switch (dom)
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
	switch (prot)
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
	switch (dom)
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

inline constexpr ::std::uint_least32_t to_win32_sock_open_mode(open_mode m) noexcept
{
	::std::uint_least32_t flags{};
	if ((m & open_mode::no_block) == open_mode::no_block)
	{
		flags |= 0x01;
	}
#if !defined(_WIN32_WINDOWS) || _WIN32_WINNT >= 0x0602
	// this flag only supports after windows 7 sp1. So we start supporting this flag from windows 8
	if ((m & open_mode::inherit) != open_mode::inherit)
	{
		flags |= 0x80;
	}
#endif
	return flags;
}

inline constexpr ::std::uint_least32_t to_win32_sock_open_mode_9xa(open_mode m) noexcept
{
	::std::uint_least32_t flags{};
	if ((m & open_mode::no_block) == open_mode::no_block)
	{
		flags |= 0x01;
	}
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

inline constexpr ::std::uint_least32_t to_native_sock_open_mode(open_mode m) noexcept
{
	return to_win32_sock_open_mode(m);
}

#endif

namespace win32::details
{

template <win32_family family>
inline ::std::size_t open_win32_socket_raw_impl(int af, int tp, int prt, ::std::uint_least32_t dwflags)
{
	if constexpr (family == win32_family::wide_nt)
	{
		::std::size_t ret{::fast_io::win32::WSASocketW(af, tp, prt, nullptr, 0, dwflags)};
		if (ret == UINTPTR_MAX)
		{
			throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
		}
		return ret;
	}
	else
	{
		::std::size_t ret{::fast_io::win32::WSASocketA(af, tp, prt, nullptr, 0, dwflags)};
		if (ret == UINTPTR_MAX)
		{
			throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
		}
		return ret;
	}
}

template <win32_family family>
inline ::std::size_t open_win32_socket_raw_om_custom_only_impl(int af, int tp, int prt, open_mode om)
{
	if constexpr (family == ::fast_io::win32_family::wide_nt)
	{
		return open_win32_socket_raw_impl<family>(af, tp, prt, to_win32_sock_open_mode(om));
	}
	else
	{
		return open_win32_socket_raw_impl<family>(af, tp, prt, to_win32_sock_open_mode_9xa(om));
	}
}

template <win32_family family>
inline ::std::size_t open_win32_socket_impl(sock_family d, sock_type t, open_mode m, sock_protocol p)
{
	return open_win32_socket_raw_om_custom_only_impl<family>(to_win32_sock_family(d), to_win32_sock_type(t),
															 to_win32_sock_protocol(p), m);
}

} // namespace win32::details

struct win32_socket_factory FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
{
	using native_handle_type = ::std::size_t;
	::std::size_t hsocket{};
	inline explicit constexpr win32_socket_factory(::std::size_t v) noexcept
		: hsocket(v)
	{}
	inline win32_socket_factory(win32_socket_factory const &) = delete;
	inline win32_socket_factory &operator=(win32_socket_factory const &) = delete;
	inline ~win32_socket_factory()
	{
		if (hsocket) [[likely]]
		{
			::fast_io::win32::closesocket(hsocket);
		}
	}
};

template <win32_family family, ::std::integral ch_type>
inline win32_socket_factory posix_accept(basic_win32_family_socket_io_observer<family, ch_type> h, void *addr,
										 win32_socklen_t *addrlen)
{
	return win32_socket_factory{::fast_io::win32::details::posix_accept_win32_socket_impl(h.hsocket, addr, addrlen)};
}

template <win32_family family, ::std::integral ch_type>
inline win32_socket_factory tcp_accept(basic_win32_family_socket_io_observer<family, ch_type> h)
{
	return win32_socket_factory{::fast_io::win32::details::posix_accept_win32_socket_impl(h.hsocket, nullptr, nullptr)};
}

template <win32_family family, ::std::integral ch_type>
class basic_win32_family_socket_file FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE : public basic_win32_family_socket_io_observer<family, ch_type>
{
public:
	using typename basic_win32_family_socket_io_observer<family, ch_type>::char_type;
	using typename basic_win32_family_socket_io_observer<family, ch_type>::input_char_type;
	using typename basic_win32_family_socket_io_observer<family, ch_type>::output_char_type;
	using typename basic_win32_family_socket_io_observer<family, ch_type>::native_handle_type;
	inline explicit constexpr basic_win32_family_socket_file() noexcept = default;

	inline constexpr basic_win32_family_socket_file(basic_win32_family_socket_io_observer<family, ch_type>) noexcept = delete;
	inline constexpr basic_win32_family_socket_file &
	operator=(basic_win32_family_socket_io_observer<family, ch_type>) noexcept = delete;

	template <typename native_hd>
		requires ::std::same_as<native_handle_type, ::std::remove_cvref_t<native_hd>>
	inline explicit constexpr basic_win32_family_socket_file(native_hd hsocket1) noexcept
		: basic_win32_family_socket_io_observer<family, ch_type>{hsocket1}
	{
	}

	inline explicit constexpr basic_win32_family_socket_file(decltype(nullptr)) noexcept = delete;
	inline basic_win32_family_socket_file(io_dup_t, basic_win32_family_socket_io_observer<family, ch_type> wsiob)
		: basic_win32_family_socket_io_observer<family, ch_type>{
			  ::fast_io::win32::details::win32_duphsocket(wsiob.hsocket)}
	{
	}

	inline basic_win32_family_socket_file(sock_family d, sock_type t, open_mode m, sock_protocol p)
		: basic_win32_family_socket_file<family, ch_type>{
			  ::fast_io::win32::details::open_win32_socket_impl<family>(d, t, m, p)}
	{
	}

	inline basic_win32_family_socket_file(basic_win32_family_socket_file const &dp)
		: basic_win32_family_socket_io_observer<family, char_type>{
			  ::fast_io::win32::details::win32_duphsocket(dp.hsocket)}
	{
	}

	inline basic_win32_family_socket_file &operator=(basic_win32_family_socket_file const &dp)
	{
		if (__builtin_addressof(dp) == this) [[unlikely]]
		{
			return *this;
		}
		this->hsocket = ::fast_io::win32::details::win32_dup2hsocket(dp.hsocket, this->hsocket);
		return *this;
	}

	inline constexpr basic_win32_family_socket_file(basic_win32_family_socket_file &&__restrict b) noexcept
		: basic_win32_family_socket_io_observer<family, char_type>{b.hsocket}
	{
		b.hsocket = 0;
	}

	inline basic_win32_family_socket_file &operator=(basic_win32_family_socket_file &&__restrict b) noexcept
	{
		if (__builtin_addressof(b) == this) [[unlikely]]
		{
			return *this;
		}
		if (this->hsocket) [[likely]]
		{
			::fast_io::win32::closesocket(this->hsocket);
		}
		this->hsocket = b.hsocket;
		b.hsocket = 0;
		return *this;
	}

	inline constexpr void reset(native_handle_type newhsocket = 0) noexcept
	{
		if (this->hsocket) [[likely]]
		{
			::fast_io::win32::closesocket(this->hsocket);
		}
		this->hsocket = newhsocket;
	}

	inline void close()
	{
		if (this->hsocket) [[likely]]
		{
			auto ret{::fast_io::win32::closesocket(this->hsocket)};
			this->hsocket = 0; // POSIX standard says we should never call close(2) again even close syscall fails
			if (ret)
			{
				throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
			}
		}
	}

	inline explicit constexpr basic_win32_family_socket_file(win32_socket_factory &&other) noexcept
		: basic_win32_family_socket_io_observer<family, char_type>{other.hsocket}
	{
		other.hsocket = 0;
	}

	inline ~basic_win32_family_socket_file()
	{
		if (this->hsocket) [[likely]]
		{
			::fast_io::win32::closesocket(this->hsocket);
		}
	}
};

namespace details
{

template <win32_family family>
inline ::std::size_t win32_family_tcp_connect_v4_impl(ipv4 v4, open_mode m)
{
	basic_win32_family_socket_file<family, char> soc(sock_family::inet, sock_type::stream, m, sock_protocol::tcp);
	constexpr auto inet{to_win32_sock_family(sock_family::inet)};
	posix_sockaddr_in in{.sin_family = inet,
						 .sin_port = big_endian(static_cast<::std::uint_least16_t>(v4.port)),
						 .sin_addr = v4.address};
	posix_connect(soc, __builtin_addressof(in), sizeof(in));
	return soc.release();
}

template <win32_family family>
inline ::std::size_t win32_family_tcp_connect_v6_impl(ipv6 v6, open_mode m)
{
	basic_win32_family_socket_file<family, char> soc(sock_family::inet6, sock_type::stream, m, sock_protocol::tcp);
	constexpr auto inet6{to_win32_sock_family(sock_family::inet6)};
	posix_sockaddr_in6 in6{.sin6_family = inet6,
						   .sin6_port = big_endian(static_cast<::std::uint_least16_t>(v6.port)),
						   .sin6_addr = v6.address};
	posix_connect(soc, __builtin_addressof(in6), sizeof(in6));
	return soc.release();
}

template <win32_family family>
inline ::std::size_t win32_family_tcp_connect_ip_impl(ip v, open_mode m)
{
	basic_win32_family_socket_file<family, char> soc(v.address.isv4 ? sock_family::inet : sock_family::inet6,
													 sock_type::stream, m, sock_protocol::tcp);
	if (v.address.isv4)
	{
		constexpr auto inet{to_win32_sock_family(sock_family::inet)};
		posix_sockaddr_in in{.sin_family = inet, .sin_port = big_endian(v.port), .sin_addr = v.address.address.v4};
		posix_connect(soc, __builtin_addressof(in), sizeof(in));
	}
	else
	{
		constexpr auto inet6{to_win32_sock_family(sock_family::inet6)};
		posix_sockaddr_in6 in6{
			.sin6_family = inet6, .sin6_port = big_endian(v.port), .sin6_addr = v.address.address.v6};
		posix_connect(soc, __builtin_addressof(in6), sizeof(in6));
	}
	return soc.release();
}

inline void win32_tcp_listen_common_impl(::std::size_t hsocket, ::std::uint_least16_t port)
{
	constexpr auto inet{to_win32_sock_family(sock_family::inet)};
	posix_sockaddr_in in{.sin_family = inet, .sin_port = big_endian(port), .sin_addr = {}};
	::fast_io::win32::details::posix_bind_win32_socket_impl(hsocket, __builtin_addressof(in), sizeof(in));
	::fast_io::win32::details::posix_listen_win32_socket_impl(hsocket, 128);
}

template <win32_family family>
inline ::std::size_t win32_tcp_listen_impl(::std::uint_least16_t port, open_mode m)
{
	basic_win32_family_socket_file<family, char> soc(sock_family::inet, sock_type::stream, m, sock_protocol::tcp);
	win32_tcp_listen_common_impl(soc.hsocket, port);
	return soc.release();
}

} // namespace details

template <win32_family family>
inline win32_socket_factory win32_family_tcp_connect(ipv4 v4, open_mode m = open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_v4_impl<family>(v4, m)};
}

inline win32_socket_factory win32_tcp_connect_9xa(ipv4 v4, open_mode m = open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_v4_impl<win32_family::ansi_9x>(v4, m)};
}

inline win32_socket_factory win32_tcp_connect_ntw(ipv4 v4, open_mode m = open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_v4_impl<win32_family::wide_nt>(v4, m)};
}

inline win32_socket_factory win32_tcp_connect(ipv4 v4, open_mode m = open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_v4_impl<win32_family::native>(v4, m)};
}

template <win32_family family>
inline win32_socket_factory win32_family_tcp_connect(ipv6 v6, open_mode m = open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_v6_impl<family>(v6, m)};
}

inline win32_socket_factory win32_tcp_connect_9xa(ipv6 v6, open_mode m = open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_v6_impl<win32_family::ansi_9x>(v6, m)};
}

inline win32_socket_factory win32_tcp_connect_ntw(ipv6 v6, open_mode m = open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_v6_impl<win32_family::wide_nt>(v6, m)};
}

inline win32_socket_factory win32_tcp_connect(ipv6 v6, open_mode m = open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_v6_impl<win32_family::native>(v6, m)};
}

inline win32_socket_factory win32_tcp_connect_9xa(ip v, open_mode m = open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_ip_impl<win32_family::ansi_9x>(v, m)};
}

inline win32_socket_factory win32_tcp_connect_ntw(ip v, open_mode m = open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_ip_impl<win32_family::wide_nt>(v, m)};
}

inline win32_socket_factory win32_tcp_connect(ip v, open_mode m = open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_ip_impl<win32_family::native>(v, m)};
}

template <win32_family family>
inline win32_socket_factory win32_family_tcp_listen(::std::uint_least16_t port, open_mode m = open_mode{})
{
	return win32_socket_factory{details::win32_tcp_listen_impl<family>(port, m)};
}

inline win32_socket_factory win32_tcp_listen_ntw(::std::uint_least16_t port, open_mode m = open_mode{})
{
	return win32_socket_factory{details::win32_tcp_listen_impl<win32_family::wide_nt>(port, m)};
}

inline win32_socket_factory win32_tcp_listen_9xa(::std::uint_least16_t port, open_mode m = open_mode{})
{
	return win32_socket_factory{details::win32_tcp_listen_impl<win32_family::ansi_9x>(port, m)};
}

inline win32_socket_factory win32_tcp_listen(::std::uint_least16_t port, open_mode m = open_mode{})
{
	return win32_socket_factory{details::win32_tcp_listen_impl<win32_family::native>(port, m)};
}

template <::std::integral ch_type>
using basic_win32_socket_io_observer_9xa = basic_win32_family_socket_io_observer<win32_family::ansi_9x, ch_type>;

using win32_socket_io_observer_9xa = basic_win32_socket_io_observer_9xa<char>;
using wwin32_socket_io_observer_9xa = basic_win32_socket_io_observer_9xa<wchar_t>;
using u8win32_socket_io_observer_9xa = basic_win32_socket_io_observer_9xa<char8_t>;
using u16win32_socket_io_observer_9xa = basic_win32_socket_io_observer_9xa<char16_t>;
using u32win32_socket_io_observer_9xa = basic_win32_socket_io_observer_9xa<char32_t>;

template <::std::integral ch_type>
using basic_win32_socket_io_observer_ntw = basic_win32_family_socket_io_observer<win32_family::wide_nt, ch_type>;

using win32_socket_io_observer_ntw = basic_win32_socket_io_observer_ntw<char>;
using wwin32_socket_io_observer_ntw = basic_win32_socket_io_observer_ntw<wchar_t>;
using u8win32_socket_io_observer_ntw = basic_win32_socket_io_observer_ntw<char8_t>;
using u16win32_socket_io_observer_ntw = basic_win32_socket_io_observer_ntw<char16_t>;
using u32win32_socket_io_observer_ntw = basic_win32_socket_io_observer_ntw<char32_t>;

template <::std::integral ch_type>
using basic_win32_socket_io_observer = basic_win32_family_socket_io_observer<win32_family::native, ch_type>;

using win32_socket_io_observer = basic_win32_socket_io_observer<char>;
using wwin32_socket_io_observer = basic_win32_socket_io_observer<wchar_t>;
using u8win32_socket_io_observer = basic_win32_socket_io_observer<char8_t>;
using u16win32_socket_io_observer = basic_win32_socket_io_observer<char16_t>;
using u32win32_socket_io_observer = basic_win32_socket_io_observer<char32_t>;

template <::std::integral ch_type>
using basic_win32_socket_file_9xa = basic_win32_family_socket_file<win32_family::ansi_9x, ch_type>;

using win32_socket_file_9xa = basic_win32_socket_file_9xa<char>;
using wwin32_socket_file_9xa = basic_win32_socket_file_9xa<wchar_t>;
using u8win32_socket_file_9xa = basic_win32_socket_file_9xa<char8_t>;
using u16win32_socket_file_9xa = basic_win32_socket_file_9xa<char16_t>;
using u32win32_socket_file_9xa = basic_win32_socket_file_9xa<char32_t>;

template <::std::integral ch_type>
using basic_win32_socket_file_ntw = basic_win32_family_socket_file<win32_family::wide_nt, ch_type>;

using win32_socket_file_ntw = basic_win32_socket_file_ntw<char>;
using wwin32_socket_file_ntw = basic_win32_socket_file_ntw<wchar_t>;
using u8win32_socket_file_ntw = basic_win32_socket_file_ntw<char8_t>;
using u16win32_socket_file_ntw = basic_win32_socket_file_ntw<char16_t>;
using u32win32_socket_file_ntw = basic_win32_socket_file_ntw<char32_t>;

template <::std::integral ch_type>
using basic_win32_socket_file = basic_win32_family_socket_file<win32_family::native, ch_type>;

using win32_socket_file = basic_win32_socket_file<char>;
using wwin32_socket_file = basic_win32_socket_file<wchar_t>;
using u8win32_socket_file = basic_win32_socket_file<char8_t>;
using u16win32_socket_file = basic_win32_socket_file<char16_t>;
using u32win32_socket_file = basic_win32_socket_file<char32_t>;

#if !defined(__CYGWIN__) && !defined(__WINE__)

template <::std::integral ch_type>
using basic_native_socket_io_observer = basic_win32_socket_io_observer<ch_type>;
template <::std::integral ch_type>
using basic_native_socket_file = basic_win32_socket_file<ch_type>;
using native_socklen_t = win32_socklen_t;

inline win32_socket_factory tcp_connect(ipv4 v4, open_mode m = open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_v4_impl<win32_family::native>(v4, m)};
}
inline win32_socket_factory tcp_connect(ipv6 v6, open_mode m = open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_v6_impl<win32_family::native>(v6, m)};
}
inline win32_socket_factory tcp_connect(ip v, open_mode m = open_mode{})
{
	return win32_socket_factory{details::win32_family_tcp_connect_ip_impl<win32_family::native>(v, m)};
}

inline win32_socket_factory tcp_listen(::std::uint_least16_t port, open_mode m = open_mode{})
{
	return win32_socket_factory{details::win32_tcp_listen_impl<win32_family::native>(port, m)};
}

#endif

namespace freestanding
{
template <>
struct is_trivially_copyable_or_relocatable<win32_socket_factory>
{
	inline static constexpr bool value = true;
};

template <win32_family fm, ::std::integral char_type>
struct is_zero_default_constructible<basic_win32_family_socket_io_observer<fm, char_type>>
{
	inline static constexpr bool value = true;
};

template <win32_family fm, ::std::integral char_type>
struct is_trivially_copyable_or_relocatable<basic_win32_family_socket_file<fm, char_type>>
{
	inline static constexpr bool value = true;
};

template <win32_family fm, ::std::integral char_type>
struct is_zero_default_constructible<basic_win32_family_socket_file<fm, char_type>>
{
	inline static constexpr bool value = true;
};

} // namespace freestanding

} // namespace fast_io
