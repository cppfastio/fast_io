#pragma once

namespace fast_io
{

struct win32_socket_event_guard_t
{
	using native_handle_type = ::std::size_t;

	native_handle_type curr_handle{};

	inline constexpr win32_socket_event_guard_t() noexcept = default;

	inline constexpr win32_socket_event_guard_t(native_handle_type handle) noexcept : curr_handle(handle)
	{}

	inline constexpr win32_socket_event_guard_t(win32_socket_event_guard_t const &) = delete;
	inline constexpr win32_socket_event_guard_t &operator=(win32_socket_event_guard_t const &) = delete;

	inline constexpr win32_socket_event_guard_t(win32_socket_event_guard_t &&other) noexcept : curr_handle{other.curr_handle}
	{
		other.curr_handle = {};
	}

	inline constexpr win32_socket_event_guard_t &operator=(win32_socket_event_guard_t &&other) noexcept
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}

		if (curr_handle) [[likely]]
		{
			::fast_io::win32::WSACloseEvent(curr_handle);
		}

		curr_handle = other.curr_handle;
		other.curr_handle = {};

		return *this;
	}

	inline constexpr native_handle_type release() noexcept
	{
		native_handle_type temp{curr_handle};
		curr_handle = {};
		return temp;
	}

	inline constexpr native_handle_type native_handle() const noexcept
	{
		return curr_handle;
	}

	inline constexpr native_handle_type native_handle() noexcept
	{
		return curr_handle;
	}

	inline constexpr ~win32_socket_event_guard_t()
	{
		if (curr_handle) [[likely]]
		{
			::fast_io::win32::WSACloseEvent(curr_handle);
		}
	}
};

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
inline ::std::byte const *win32_socket_write_bytes_impl(::std::size_t socket, ::std::byte const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	wsabuf buffer{::fast_io::details::read_write_bytes_compute<::std::uint_least32_t>(first, count),
				  const_cast<char *>(reinterpret_cast<char const *>(first))};
	::std::uint_least32_t sent{};
	if (::fast_io::win32::WSASend(socket, __builtin_addressof(buffer), 1, __builtin_addressof(sent), 0, nullptr,
								  nullptr))
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	}
	return first + sent;
}

inline ::std::byte *win32_socket_read_bytes_impl(::std::size_t socket, ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	int recved{::fast_io::win32::recv(socket, reinterpret_cast<char *>(first),
									  ::fast_io::details::read_write_bytes_compute<::std::int_least32_t>(first, count),
									  0)};
	if (recved == -1)
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	}
	return first + recved;
}

inline void posix_connect_win32_socket_impl(::std::size_t hsocket, void const *addr, int addrlen)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if (::fast_io::win32::WSAConnect(hsocket, addr, addrlen, nullptr, nullptr, nullptr, nullptr))
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	}
}

inline void posix_bind_win32_socket_impl(::std::size_t hsocket, void const *addr, int addrlen)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if (::fast_io::win32::bind(hsocket, addr, addrlen) == -1)
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	}
}

inline void posix_listen_win32_socket_impl(::std::size_t hsocket, int backlog)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if (::fast_io::win32::listen(hsocket, backlog) == -1)
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	}
}

inline void posix_setsockopt_win32_socket_impl(::std::size_t hsocket, int level, int optname,
											   void const *optval, int optlen)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if (::fast_io::win32::setsockopt(hsocket, level, optname, optval, optlen) == -1)
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	}
}

inline void posix_getpeername_win32_socket_impl(::std::size_t hsocket, void *addr, int *addrlen)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if (::fast_io::win32::getpeername(hsocket, addr, addrlen) == -1)
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	}
}

inline ::std::size_t posix_accept_win32_socket_impl(::std::size_t hsocket, void *addr, int *addrlen)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::std::size_t accepted_socket{::fast_io::win32::WSAAccept(hsocket, addr, addrlen, nullptr, 0)};
	if (accepted_socket == static_cast<::std::size_t>(-1))
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	}
	return accepted_socket;
}

inline ::std::ptrdiff_t posix_recvfrom_win32_socket_impl(::std::size_t hsocket, void *buf, ::std::size_t len, int flags, void *src_addr, int *addrlen)
	FAST_IO_HERBCEPTIONS_THROWS
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
	FAST_IO_HERBCEPTIONS_THROWS
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
													 ::std::byte *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::details::win32_socket_read_bytes_impl(wiob.hsocket, first, count);
}

template <win32_family family, ::std::integral ch_type>
inline ::std::byte const *write_some_bytes_overflow_define(basic_win32_family_socket_io_observer<family, ch_type> wiob,
														   ::std::byte const *first, ::std::size_t count)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::details::win32_socket_write_bytes_impl(wiob.hsocket, first, count);
}

template <win32_family family, ::std::integral ch_type>
inline void posix_connect(basic_win32_family_socket_io_observer<family, ch_type> sockiob, void const *remote_address,
						  win32_socklen_t address_struct_size)
	FAST_IO_HERBCEPTIONS_THROWS
{
	win32::details::posix_connect_win32_socket_impl(sockiob.hsocket, remote_address, address_struct_size);
}

template <win32_family family, ::std::integral ch_type>
inline void posix_bind(basic_win32_family_socket_io_observer<family, ch_type> h, void const *addr,
					   win32_socklen_t addrlen)
	FAST_IO_HERBCEPTIONS_THROWS
{
	win32::details::posix_bind_win32_socket_impl(h.hsocket, addr, addrlen);
}

template <win32_family family, ::std::integral ch_type>
inline void posix_listen(basic_win32_family_socket_io_observer<family, ch_type> h, int backlog)
	FAST_IO_HERBCEPTIONS_THROWS
{
	win32::details::posix_listen_win32_socket_impl(h.hsocket, backlog);
}

template <win32_family family, ::std::integral ch_type>
inline ::std::ptrdiff_t posix_recvfrom(basic_win32_family_socket_io_observer<family, ch_type> h, void *buf, ::std::size_t len,
									   int flags, void *src_addr, win32_socklen_t *addrlen)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32::details::posix_recvfrom_win32_socket_impl(h.hsocket, buf, len, flags, src_addr, addrlen);
}

template <win32_family family, ::std::integral ch_type>
inline ::std::ptrdiff_t posix_sendto(basic_win32_family_socket_io_observer<family, ch_type> h, void const *msg, ::std::size_t len,
									 int flags, void const *to, win32_socklen_t tolen)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32::details::posix_sendto_win32_socket_impl(h.hsocket, msg, len, flags, to, tolen);
}

namespace win32::details
{

inline ::std::size_t win32_duphsocket(::std::size_t s)
	FAST_IO_HERBCEPTIONS_THROWS
{
	// Duplicate a SOCKET using WSADuplicateSocket + WSASocket, per MSDN guidance.
	::fast_io::win32::wsaprotocol_infoa info{};
	::std::uint_least32_t const pid{::fast_io::win32::GetCurrentProcessId()};
	if (::fast_io::win32::WSADuplicateSocketA(s, pid, __builtin_addressof(info)) != 0)
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	}
	::std::size_t dup{
		::fast_io::win32::WSASocketA(info.iAddressFamily, info.iSocketType, info.iProtocol, __builtin_addressof(info), 0, 0)};
	if (dup == UINTPTR_MAX)
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	}
	return dup;
}
inline ::std::size_t win32_dup2hsocket(::std::size_t handle, ::std::size_t newhandle)
	FAST_IO_HERBCEPTIONS_THROWS
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
	FAST_IO_HERBCEPTIONS_THROWS
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

/* FIONBIO, matching posix O_NONBLOCK: overlapped sockets stay capable of
 * overlapped I/O, and calls like WSAAccept report WSAEWOULDBLOCK instead
 * of blocking. Kept out of open_win32_socket_raw_impl so callers that
 * need a synchronous connect() can defer it past the handshake. */
inline void win32_socket_apply_no_block(::std::size_t hsocket)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::std::uint_least32_t nonblock{1u};
	if (::fast_io::win32::ioctlsocket(hsocket, static_cast<long>(0x8004667Eu),
									  __builtin_addressof(nonblock)) != 0) [[unlikely]]
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	}
}

template <win32_family family>
inline ::std::size_t open_win32_socket_raw_om_custom_only_impl(int af, int tp, int prt, open_mode om)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::std::size_t ret;
	if constexpr (family == ::fast_io::win32_family::wide_nt)
	{
		ret = open_win32_socket_raw_impl<family>(af, tp, prt, to_win32_sock_open_mode(om));
	}
	else
	{
		ret = open_win32_socket_raw_impl<family>(af, tp, prt, to_win32_sock_open_mode_9xa(om));
	}
	if ((om & open_mode::no_block) == open_mode::no_block)
	{
		win32_socket_apply_no_block(ret);
	}
	return ret;
}

template <win32_family family>
inline ::std::size_t open_win32_socket_impl(sock_family d, sock_type t, open_mode m, sock_protocol p)
	FAST_IO_HERBCEPTIONS_THROWS
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
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_socket_factory{::fast_io::win32::details::posix_accept_win32_socket_impl(h.hsocket, addr, addrlen)};
}

template <win32_family family, ::std::integral ch_type>
inline win32_socket_factory tcp_accept(basic_win32_family_socket_io_observer<family, ch_type> h)
	FAST_IO_HERBCEPTIONS_THROWS
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
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_win32_family_socket_io_observer<family, ch_type>{
			  ::fast_io::win32::details::win32_duphsocket(wsiob.hsocket)}
	{
	}

	inline basic_win32_family_socket_file(sock_family d, sock_type t, open_mode m, sock_protocol p)
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_win32_family_socket_file<family, ch_type>{
			  ::fast_io::win32::details::open_win32_socket_impl<family>(d, t, m, p)}
	{
	}

	inline basic_win32_family_socket_file(basic_win32_family_socket_file const &dp)
		FAST_IO_HERBCEPTIONS_THROWS
		: basic_win32_family_socket_io_observer<family, char_type>{
			  ::fast_io::win32::details::win32_duphsocket(dp.hsocket)}
	{
	}

	inline basic_win32_family_socket_file &operator=(basic_win32_family_socket_file const &dp)
		FAST_IO_HERBCEPTIONS_THROWS
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
		FAST_IO_HERBCEPTIONS_THROWS
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

/* Socket creation for connect(): to_win32_sock_open_mode already sets
 * WSA_FLAG_OVERLAPPED when no_block is requested, so the socket is
 * IOCP-capable; FIONBIO is deferred until after connect() — a
 * nonblocking socket reports WSAEWOULDBLOCK from the handshake instead
 * of completing it */
template <win32_family family>
inline ::std::size_t open_win32_socket_connect_impl(sock_family d, open_mode m)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if constexpr (family == ::fast_io::win32_family::wide_nt)
	{
		return ::fast_io::win32::details::open_win32_socket_raw_impl<family>(to_win32_sock_family(d),
																			 to_win32_sock_type(sock_type::stream),
																			 to_win32_sock_protocol(sock_protocol::tcp),
																			 to_win32_sock_open_mode(m));
	}
	else
	{
		return ::fast_io::win32::details::open_win32_socket_raw_impl<family>(to_win32_sock_family(d),
																			 to_win32_sock_type(sock_type::stream),
																			 to_win32_sock_protocol(sock_protocol::tcp),
																			 to_win32_sock_open_mode_9xa(m));
	}
}

template <win32_family family>
inline ::std::size_t win32_family_tcp_connect_v4_impl(ipv4 v4, open_mode m)
	FAST_IO_HERBCEPTIONS_THROWS
{
	basic_win32_family_socket_file<family, char> soc{
		open_win32_socket_connect_impl<family>(sock_family::inet, m)};
	constexpr auto inet{to_win32_sock_family(sock_family::inet)};
	posix_sockaddr_in in{.sin_family = inet,
						 .sin_port = big_endian(static_cast<::std::uint_least16_t>(v4.port)),
						 .sin_addr = v4.address};
	posix_connect(soc, __builtin_addressof(in), sizeof(in));
	if ((m & open_mode::no_block) == open_mode::no_block)
	{
		::fast_io::win32::details::win32_socket_apply_no_block(soc.hsocket);
	}
	return soc.release();
}

template <win32_family family>
inline ::std::size_t win32_family_tcp_connect_v6_impl(ipv6 v6, open_mode m)
	FAST_IO_HERBCEPTIONS_THROWS
{
	basic_win32_family_socket_file<family, char> soc{
		open_win32_socket_connect_impl<family>(sock_family::inet6, m)};
	constexpr auto inet6{to_win32_sock_family(sock_family::inet6)};
	posix_sockaddr_in6 in6{.sin6_family = inet6,
						   .sin6_port = big_endian(static_cast<::std::uint_least16_t>(v6.port)),
						   .sin6_addr = v6.address};
	posix_connect(soc, __builtin_addressof(in6), sizeof(in6));
	if ((m & open_mode::no_block) == open_mode::no_block)
	{
		::fast_io::win32::details::win32_socket_apply_no_block(soc.hsocket);
	}
	return soc.release();
}

template <win32_family family>
inline ::std::size_t win32_family_tcp_connect_ip_impl(ip v, open_mode m)
	FAST_IO_HERBCEPTIONS_THROWS
{
	basic_win32_family_socket_file<family, char> soc{
		open_win32_socket_connect_impl<family>(v.address.isv4 ? sock_family::inet : sock_family::inet6, m)};
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
	if ((m & open_mode::no_block) == open_mode::no_block)
	{
		::fast_io::win32::details::win32_socket_apply_no_block(soc.hsocket);
	}
	return soc.release();
}

inline void win32_tcp_listen_common_impl(::std::size_t hsocket, ::std::uint_least16_t port)
	FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr auto inet{to_win32_sock_family(sock_family::inet)};
	posix_sockaddr_in in{.sin_family = inet, .sin_port = big_endian(port), .sin_addr = {}};
	::fast_io::win32::details::posix_bind_win32_socket_impl(hsocket, __builtin_addressof(in), sizeof(in));
	::fast_io::win32::details::posix_listen_win32_socket_impl(hsocket, 128);
}

inline ::fast_io::ip win32_sockaddr_to_ip_impl(posix_sockaddr const *sa) noexcept
{
	::fast_io::ip ret;
	if (sa == nullptr)
	{
		return ret;
	}
	if (sa->sa_family == to_win32_sock_family(::fast_io::sock_family::inet))
	{
		auto const *in{reinterpret_cast<posix_sockaddr_in const *>(sa)};
		::fast_io::details::my_memcpy(__builtin_addressof(ret.address.address.v4.address),
									  __builtin_addressof(in->sin_addr), sizeof(posix_in_addr));
		ret.address.isv4 = true;
		ret.port = big_endian(in->sin_port);
	}
	else if (sa->sa_family == to_win32_sock_family(::fast_io::sock_family::inet6))
	{
		auto const *in6{reinterpret_cast<posix_sockaddr_in6 const *>(sa)};
		::fast_io::details::my_memcpy(__builtin_addressof(ret.address.address.v6.address),
									  __builtin_addressof(in6->sin6_addr), sizeof(posix_in6_addr));
		ret.port = big_endian(in6->sin6_port);
	}
	return ret;
}

inline void win32_tcp_listen_v6_common_impl(::std::size_t hsocket, ipv6 v6)
	FAST_IO_HERBCEPTIONS_THROWS
{
	/* winsock IPPROTO_IPV6 = 41, IPV6_V6ONLY = 27 — the v6 socket stays
	 * v6-only so a companion v4 listener owns the v4 traffic */
	constexpr int one{1};
	::fast_io::win32::details::posix_setsockopt_win32_socket_impl(
		hsocket, 41, 27, __builtin_addressof(one), static_cast<int>(sizeof(one)));
	constexpr auto inet6{to_win32_sock_family(sock_family::inet6)};
	posix_sockaddr_in6 in6{
		.sin6_family = inet6, .sin6_port = big_endian(v6.port), .sin6_addr = v6.address};
	::fast_io::win32::details::posix_bind_win32_socket_impl(hsocket, __builtin_addressof(in6),
															sizeof(in6));
	::fast_io::win32::details::posix_listen_win32_socket_impl(hsocket, 128);
}

template <win32_family family>
inline ::std::size_t win32_tcp_listen_v6_impl(ipv6 v6, open_mode m)
	FAST_IO_HERBCEPTIONS_THROWS
{
	basic_win32_family_socket_file<family, char> soc(sock_family::inet6, sock_type::stream, m,
													 sock_protocol::tcp);
	win32_tcp_listen_v6_common_impl(soc.hsocket, v6);
	return soc.release();
}

/* dual-stack: winsock calls it DualMode — an AF_INET6 socket with
 * IPV6_V6ONLY cleared accepts both families on one socket, matching
 * node's server.listen(port) bound to ::. No v6 support or a refused
 * V6ONLY clear falls back to the v4 socket */
template <win32_family family>
inline ::std::size_t win32_tcp_listen_dual_impl(::std::uint_least16_t port, open_mode m)
	FAST_IO_HERBCEPTIONS_THROWS
{
	try
	{
		basic_win32_family_socket_file<family, char> soc(sock_family::inet6, sock_type::stream, m,
														 sock_protocol::tcp);
		constexpr int zero{};
		::fast_io::win32::details::posix_setsockopt_win32_socket_impl(
			soc.hsocket, 41, 27, __builtin_addressof(zero), static_cast<int>(sizeof(zero)));
		constexpr auto inet6{to_win32_sock_family(sock_family::inet6)};
		posix_sockaddr_in6 in6{.sin6_family = inet6, .sin6_port = big_endian(port), .sin6_addr = {}};
		::fast_io::win32::details::posix_bind_win32_socket_impl(soc.hsocket, __builtin_addressof(in6),
																sizeof(in6));
		::fast_io::win32::details::posix_listen_win32_socket_impl(soc.hsocket, 128);
		return soc.release();
	}
	catch throws(::std::error)
	{
	}
	basic_win32_family_socket_file<family, char> soc(sock_family::inet, sock_type::stream, m,
													 sock_protocol::tcp);
	win32_tcp_listen_common_impl(soc.hsocket, port);
	return soc.release();
}

template <win32_family family>
inline ::std::size_t win32_tcp_listen_impl(::std::uint_least16_t port, open_mode m)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_tcp_listen_dual_impl<family>(port, m);
}

} // namespace details

template <win32_family family>
inline win32_socket_factory win32_family_tcp_connect(ipv4 v4, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_socket_factory{details::win32_family_tcp_connect_v4_impl<family>(v4, m)};
}

inline win32_socket_factory win32_tcp_connect_9xa(ipv4 v4, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_socket_factory{details::win32_family_tcp_connect_v4_impl<win32_family::ansi_9x>(v4, m)};
}

inline win32_socket_factory win32_tcp_connect_ntw(ipv4 v4, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_socket_factory{details::win32_family_tcp_connect_v4_impl<win32_family::wide_nt>(v4, m)};
}

inline win32_socket_factory win32_tcp_connect(ipv4 v4, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_socket_factory{details::win32_family_tcp_connect_v4_impl<win32_family::native>(v4, m)};
}

template <win32_family family>
inline win32_socket_factory win32_family_tcp_connect(ipv6 v6, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_socket_factory{details::win32_family_tcp_connect_v6_impl<family>(v6, m)};
}

inline win32_socket_factory win32_tcp_connect_9xa(ipv6 v6, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_socket_factory{details::win32_family_tcp_connect_v6_impl<win32_family::ansi_9x>(v6, m)};
}

inline win32_socket_factory win32_tcp_connect_ntw(ipv6 v6, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_socket_factory{details::win32_family_tcp_connect_v6_impl<win32_family::wide_nt>(v6, m)};
}

inline win32_socket_factory win32_tcp_connect(ipv6 v6, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_socket_factory{details::win32_family_tcp_connect_v6_impl<win32_family::native>(v6, m)};
}

inline win32_socket_factory win32_tcp_connect_9xa(ip v, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_socket_factory{details::win32_family_tcp_connect_ip_impl<win32_family::ansi_9x>(v, m)};
}

inline win32_socket_factory win32_tcp_connect_ntw(ip v, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_socket_factory{details::win32_family_tcp_connect_ip_impl<win32_family::wide_nt>(v, m)};
}

inline win32_socket_factory win32_tcp_connect(ip v, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_socket_factory{details::win32_family_tcp_connect_ip_impl<win32_family::native>(v, m)};
}

template <win32_family family>
inline win32_socket_factory win32_family_tcp_listen(::std::uint_least16_t port, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_socket_factory{details::win32_tcp_listen_impl<family>(port, m)};
}

inline win32_socket_factory win32_tcp_listen_ntw(::std::uint_least16_t port, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_socket_factory{details::win32_tcp_listen_impl<win32_family::wide_nt>(port, m)};
}

inline win32_socket_factory win32_tcp_listen_9xa(::std::uint_least16_t port, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_socket_factory{details::win32_tcp_listen_impl<win32_family::ansi_9x>(port, m)};
}

inline win32_socket_factory win32_tcp_listen(::std::uint_least16_t port, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_socket_factory{details::win32_tcp_listen_impl<win32_family::native>(port, m)};
}

template <win32_family family>
inline win32_socket_factory win32_family_tcp_listen(ipv6 v6, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_socket_factory{details::win32_tcp_listen_v6_impl<family>(v6, m)};
}

inline win32_socket_factory win32_tcp_listen_ntw(ipv6 v6, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_socket_factory{details::win32_tcp_listen_v6_impl<win32_family::wide_nt>(v6, m)};
}

inline win32_socket_factory win32_tcp_listen_9xa(ipv6 v6, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_socket_factory{details::win32_tcp_listen_v6_impl<win32_family::ansi_9x>(v6, m)};
}

inline win32_socket_factory win32_tcp_listen(ipv6 v6, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_socket_factory{details::win32_tcp_listen_v6_impl<win32_family::native>(v6, m)};
}

template <win32_family fam, ::std::integral ch_type>
inline void getpeername(basic_win32_family_socket_io_observer<fam, ch_type> h, void *addr,
						int *addrlen) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::details::posix_getpeername_win32_socket_impl(h.hsocket, addr, addrlen);
}

template <win32_family fam, ::std::integral ch_type>
inline ::fast_io::ip getpeername(basic_win32_family_socket_io_observer<fam, ch_type> h)
	FAST_IO_HERBCEPTIONS_THROWS
{
	posix_sockaddr_storage sa{};
	int addrlen{static_cast<int>(sizeof(sa))};
	::fast_io::win32::details::posix_getpeername_win32_socket_impl(
		h.hsocket, __builtin_addressof(sa), __builtin_addressof(addrlen));
	return details::win32_sockaddr_to_ip_impl(
		reinterpret_cast<posix_sockaddr const *>(__builtin_addressof(sa)));
}
/* WICG local-network-access on the peer: classify the connected
 * socket's source address into its address space, and the containment
 * check `peer_space <= max` as one call */
template <win32_family fam, ::std::integral ch_type>
inline ::fast_io::ip_address_space lna_classify(
	basic_win32_family_socket_io_observer<fam, ch_type> h) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::lna_classify(getpeername(h).address);
}
template <win32_family fam, ::std::integral ch_type>
inline bool lna_allows(basic_win32_family_socket_io_observer<fam, ch_type> h,
					   ::fast_io::ip_address_space max) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::lna_classify(h) <= max;
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
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_socket_factory{details::win32_family_tcp_connect_v4_impl<win32_family::native>(v4, m)};
}
inline win32_socket_factory tcp_connect(ipv6 v6, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_socket_factory{details::win32_family_tcp_connect_v6_impl<win32_family::native>(v6, m)};
}
inline win32_socket_factory tcp_connect(ip v, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return win32_socket_factory{details::win32_family_tcp_connect_ip_impl<win32_family::native>(v, m)};
}

namespace operations
{

/*
 * async_connect convenience overloads: build the peer sockaddr from a
 * fast_io address exactly like tcp_connect does, then forward to the raw
 * pointer-and-length form. The backend copies the address at submission,
 * so the local does not need to outlive the call.
 */
template <typename async_scheduler_type, typename streamtype>
inline auto async_connect(async_scheduler_type &&scheduler,
						  ::fast_io::posix_statx_timestamp_opt timeout, streamtype &&stm,
						  ipv4 v4) noexcept
{
	constexpr auto inet{to_win32_sock_family(sock_family::inet)};
	posix_sockaddr_in in{.sin_family = inet,
						 .sin_port = big_endian(static_cast<::std::uint_least16_t>(v4.port)),
						 .sin_addr = v4.address};
	return async_connect(::fast_io::freestanding::forward<async_scheduler_type>(scheduler),
						 timeout, ::fast_io::freestanding::forward<streamtype>(stm),
						 __builtin_addressof(in), sizeof(in));
}

template <typename async_scheduler_type, typename streamtype>
inline auto async_connect(async_scheduler_type &&scheduler,
						  ::fast_io::posix_statx_timestamp_opt timeout, streamtype &&stm,
						  ipv6 v6) noexcept
{
	constexpr auto inet6{to_win32_sock_family(sock_family::inet6)};
	posix_sockaddr_in6 in6{.sin6_family = inet6,
						   .sin6_port = big_endian(static_cast<::std::uint_least16_t>(v6.port)),
						   .sin6_addr = v6.address};
	return async_connect(::fast_io::freestanding::forward<async_scheduler_type>(scheduler),
						 timeout, ::fast_io::freestanding::forward<streamtype>(stm),
						 __builtin_addressof(in6), sizeof(in6));
}

template <typename async_scheduler_type, typename streamtype>
inline auto async_connect(async_scheduler_type &&scheduler,
						  ::fast_io::posix_statx_timestamp_opt timeout, streamtype &&stm,
						  ip v) noexcept
{
	if (v.is_ipv4())
	{
		return async_connect(::fast_io::freestanding::forward<async_scheduler_type>(scheduler),
							 timeout, ::fast_io::freestanding::forward<streamtype>(stm),
							 ipv4{.address = v.address.address.v4, .port = v.port});
	}
	return async_connect(::fast_io::freestanding::forward<async_scheduler_type>(scheduler),
						 timeout, ::fast_io::freestanding::forward<streamtype>(stm),
						 ipv6{.address = v.address.address.v6, .port = v.port});
}

} // namespace operations

inline win32_socket_factory tcp_listen(::std::uint_least16_t port, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
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
