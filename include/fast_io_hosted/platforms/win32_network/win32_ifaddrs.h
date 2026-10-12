#pragma once

namespace fast_io
{

/*
 * getifaddrs yields one node per (interface, address) pair; the win32
 * equivalent nests a unicast-address list under each adapter. The entry
 * is therefore a pair — adapter supplies name/flags, the unicast node
 * supplies the socket address — and the iterator flattens the nesting
 * so posix iteration code carries over unchanged.
 */
struct win32_ifaddrs
{
	::fast_io::win32::win32_ip_adapter_addresses *adapter{};
	::fast_io::win32::win32_ip_adapter_unicast_address *unicast{};
};

namespace details
{
/* next address in the flattened walk: unicast->next, then the next
 * adapter's first unicast, skipping adapters that have none. A null
 * unicast on entry means "take this adapter's first unicast", which
 * doubles as the begin() walk start. */
inline win32_ifaddrs win32_ifaddrs_advance_impl(win32_ifaddrs e) noexcept
{
	if (e.adapter == nullptr)
	{
		return {};
	}
	e.unicast = e.unicast == nullptr ? e.adapter->first_unicast_address : e.unicast->next;
	while (e.unicast == nullptr)
	{
		e.adapter = e.adapter->next;
		if (e.adapter == nullptr)
		{
			return {};
		}
		e.unicast = e.adapter->first_unicast_address;
	}
	return e;
}

/* GAA fills a caller-supplied buffer and reports the size through
 * SizePointer — size-query then query-again, retrying when adapters
 * appear between the two calls. The buffer comes off the process heap
 * through win32_heapalloc_allocator's try api, matching how the win32
 * apis it feeds report failure */
inline ::fast_io::win32::win32_ip_adapter_addresses *win32_getadaptersaddresses_impl()
	FAST_IO_HERBCEPTIONS_THROWS
{
	/* GAA_FLAG_SKIP_ANYCAST | SKIP_MULTICAST | SKIP_DNS_SERVER —
	 * friendly names and unicast addresses stay */
	constexpr ::std::uint_least32_t gaa_skip_flags{0x000eu};
	::std::uint_least32_t sz{};
	for (unsigned tries{}; tries != 4; ++tries)
	{
		auto const ret{::fast_io::win32::GetAdaptersAddresses(0u, gaa_skip_flags, nullptr, nullptr,
															  __builtin_addressof(sz))};
		if (ret == 232u) /* ERROR_NO_DATA — nothing to enumerate */
		{
			return nullptr;
		}
		if (ret != 111u) /* anything but ERROR_BUFFER_OVERFLOW is fatal */
		{
			throw_win32_error(ret);
		}
		auto *buf{static_cast<::fast_io::win32::win32_ip_adapter_addresses *>(
			::fast_io::win32_heapalloc_allocator::allocate_conditional_zero_try(sz, false))};
		auto const ret2{::fast_io::win32::GetAdaptersAddresses(0u, gaa_skip_flags, nullptr, buf,
															   __builtin_addressof(sz))};
		if (ret2 == 0u) /* ERROR_SUCCESS */
		{
			return buf;
		}
		::fast_io::win32_heapalloc_allocator::deallocate(buf);
		if (ret2 != 111u)
		{
			throw_win32_error(ret2);
		}
	}
	throw_win32_error(8u); /* ERROR_NOT_ENOUGH_MEMORY — retries exhausted */
}
} // namespace details

template <win32_family fam>
	requires(fam == win32_family::ansi_9x || fam == win32_family::wide_nt)
class win32_family_ifaddrs_io_observer
{
public:
	using char_type = ::std::conditional_t<fam == win32_family::wide_nt, char16_t, char>;
	using native_handle_type = win32_ifaddrs;
	native_handle_type res{};
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return res;
	}
	inline explicit constexpr operator bool() noexcept
	{
		return res.adapter != nullptr;
	}
	inline native_handle_type release() noexcept
	{
		native_handle_type temp{res};
		res = {};
		return temp;
	}
};

template <win32_family fam>
	requires(fam == win32_family::ansi_9x || fam == win32_family::wide_nt)
struct win32_family_ifaddrs_iterator
{
	using native_handle_type = win32_ifaddrs;
	native_handle_type res{};
};

template <win32_family fam>
inline constexpr bool operator==(win32_family_ifaddrs_iterator<fam> a,
								 ::std::default_sentinel_t) noexcept
{
	return a.res.adapter == nullptr;
}

template <win32_family fam>
inline constexpr bool operator!=(win32_family_ifaddrs_iterator<fam> a,
								 ::std::default_sentinel_t) noexcept
{
	return a.res.adapter != nullptr;
}

template <win32_family fam>
inline constexpr win32_family_ifaddrs_io_observer<fam>
operator*(win32_family_ifaddrs_iterator<fam> d) noexcept
{
	return {d.res};
}

template <win32_family fam>
inline constexpr win32_family_ifaddrs_iterator<fam> &
operator++(win32_family_ifaddrs_iterator<fam> &d) noexcept
{
	d.res = details::win32_ifaddrs_advance_impl(d.res);
	return d;
}

template <win32_family fam>
inline constexpr win32_family_ifaddrs_iterator<fam>
operator++(win32_family_ifaddrs_iterator<fam> &d, int) noexcept
{
	win32_family_ifaddrs_iterator<fam> temp{d};
	++d;
	return temp;
}

/* entry accessors — same names as the posix side */
template <win32_family fam>
inline constexpr ::fast_io::manipulators::basic_os_c_str<
	typename win32_family_ifaddrs_io_observer<fam>::char_type>
name(win32_family_ifaddrs_io_observer<fam> e) noexcept
{
	if constexpr (fam == win32_family::wide_nt)
	{
		return {e.res.adapter->friendly_name};
	}
	else
	{
		return {e.res.adapter->adapter_name};
	}
}

template <win32_family fam>
inline constexpr ::std::uint_least32_t flags(win32_family_ifaddrs_io_observer<fam> e) noexcept
{
	return e.res.adapter->flags;
}

/* unicast sockaddr's family mapped onto sock_family — the win32
 * enumeration only returns unicast addresses, but a null sockaddr is
 * still possible */
template <win32_family fam>
inline constexpr ::fast_io::sock_family family(win32_family_ifaddrs_io_observer<fam> e) noexcept
{
	auto const *u{e.res.unicast};
	auto const *sa{u == nullptr ? nullptr : u->address.lpSockaddr};
	if (sa == nullptr)
	{
		return ::fast_io::sock_family::unspec;
	}
	if (sa->sa_family == to_win32_sock_family(::fast_io::sock_family::inet))
	{
		return ::fast_io::sock_family::inet;
	}
	if (sa->sa_family == to_win32_sock_family(::fast_io::sock_family::inet6))
	{
		return ::fast_io::sock_family::inet6;
	}
	return ::fast_io::sock_family::unspec;
}

namespace details
{
/* shares the sockaddr→ip conversion from socket_file.h — only the
 * address half is needed here */
inline ::fast_io::ip_address win32_sockaddr_to_ip_address_impl(posix_sockaddr const *sa) noexcept
{
	return details::win32_sockaddr_to_ip_impl(sa).address;
}

inline void win32_prefix_mask_impl(unsigned char *mask, unsigned prefixlen) noexcept
{
	unsigned const full{prefixlen >> 3};
	unsigned const rem{prefixlen & 7u};
	for (unsigned i{}; i != full; ++i)
	{
		mask[i] = 0xffu;
	}
	if (rem != 0)
	{
		mask[full] = static_cast<unsigned char>(0xffu << (8u - rem));
	}
}
} // namespace details

template <win32_family fam>
inline ::fast_io::ip_address address(win32_family_ifaddrs_io_observer<fam> e) noexcept
{
	auto const *u{e.res.unicast};
	return details::win32_sockaddr_to_ip_address_impl(u == nullptr ? nullptr : u->address.lpSockaddr);
}

/* no sockaddr for the mask on win32 — each unicast entry carries
 * OnLinkPrefixLength instead, so the mask bytes are computed */
template <win32_family fam>
inline ::fast_io::ip_address netmask(win32_family_ifaddrs_io_observer<fam> e) noexcept
{
	::fast_io::ip_address ret;
	auto const *u{e.res.unicast};
	auto const *sa{u == nullptr ? nullptr : u->address.lpSockaddr};
	if (sa == nullptr)
	{
		return ret;
	}
	unsigned const pl{static_cast<unsigned>(u->on_link_prefix_length)};
	if (sa->sa_family == to_win32_sock_family(::fast_io::sock_family::inet) && pl <= 32)
	{
		unsigned char mask[4]{};
		details::win32_prefix_mask_impl(mask, pl);
		::fast_io::details::my_memcpy(__builtin_addressof(ret.address.v4.address), mask, 4);
		ret.isv4 = true;
	}
	else if (sa->sa_family == to_win32_sock_family(::fast_io::sock_family::inet6) && pl <= 128)
	{
		unsigned char mask[16]{};
		details::win32_prefix_mask_impl(mask, pl);
		::fast_io::details::my_memcpy(__builtin_addressof(ret.address.v6.address), mask, 16);
	}
	return ret;
}

/* broadcast/dstaddr have no GetAdaptersAddresses counterpart — the
 * posix-side accessors deliberately do not appear here */

template <win32_family fam>
	requires(fam == win32_family::ansi_9x || fam == win32_family::wide_nt)
class win32_family_ifaddrs_file FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
	: public win32_family_ifaddrs_io_observer<fam>
{
public:
	using typename win32_family_ifaddrs_io_observer<fam>::char_type;
	using typename win32_family_ifaddrs_io_observer<fam>::native_handle_type;
	inline constexpr win32_family_ifaddrs_file() noexcept = default;
	inline constexpr win32_family_ifaddrs_file(win32_family_ifaddrs_io_observer<fam>) noexcept = delete;
	inline constexpr win32_family_ifaddrs_file &
	operator=(win32_family_ifaddrs_io_observer<fam>) noexcept = delete;
	explicit constexpr win32_family_ifaddrs_file(decltype(nullptr)) noexcept = delete;
	inline win32_family_ifaddrs_file(ifaddrs_enumerate_t) FAST_IO_HERBCEPTIONS_THROWS
		: win32_family_ifaddrs_io_observer<fam>{{details::win32_getadaptersaddresses_impl(),
												 nullptr}}
	{
	}
	inline win32_family_ifaddrs_file(win32_family_ifaddrs_file const &) = delete;
	inline win32_family_ifaddrs_file &operator=(win32_family_ifaddrs_file const &) = delete;
	inline constexpr win32_family_ifaddrs_file(win32_family_ifaddrs_file &&__restrict other) noexcept
		: win32_family_ifaddrs_io_observer<fam>{other.res}
	{
		other.res = {};
	}
	inline win32_family_ifaddrs_file &
	operator=(win32_family_ifaddrs_file &&__restrict other) noexcept
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		if (this->res.adapter != nullptr) [[likely]]
		{
			::fast_io::win32_heapalloc_allocator::deallocate(this->res.adapter);
		}
		this->res = other.res;
		other.res = {};
		return *this;
	}
	inline void close() noexcept
	{
		if (this->res.adapter != nullptr) [[likely]]
		{
			::fast_io::win32_heapalloc_allocator::deallocate(this->res.adapter);
			this->res = {};
		}
	}
	inline ~win32_family_ifaddrs_file()
	{
		if (this->res.adapter != nullptr) [[likely]]
		{
			::fast_io::win32_heapalloc_allocator::deallocate(this->res.adapter);
		}
	}
};

template <win32_family fam>
inline constexpr win32_family_ifaddrs_iterator<fam>
begin(win32_family_ifaddrs_io_observer<fam> ob) noexcept
{
	return {details::win32_ifaddrs_advance_impl({ob.res.adapter, nullptr})};
}

template <win32_family fam>
inline constexpr win32_family_ifaddrs_iterator<fam>
cbegin(win32_family_ifaddrs_io_observer<fam> ob) noexcept
{
	return {details::win32_ifaddrs_advance_impl({ob.res.adapter, nullptr})};
}

template <win32_family fam>
inline constexpr ::std::default_sentinel_t
end(win32_family_ifaddrs_io_observer<fam>) noexcept
{
	return {};
}

template <win32_family fam>
inline constexpr ::std::default_sentinel_t
cend(win32_family_ifaddrs_io_observer<fam>) noexcept
{
	return {};
}

using native_ifaddrs = win32_ifaddrs;
using native_ifaddrs_io_observer = win32_family_ifaddrs_io_observer<win32_family::native>;
using native_ifaddrs_file = win32_family_ifaddrs_file<win32_family::native>;
using native_ifaddrs_iterator = win32_family_ifaddrs_iterator<win32_family::native>;

namespace freestanding
{
template <win32_family fam>
struct is_trivially_copyable_or_relocatable<win32_family_ifaddrs_file<fam>>
{
	inline static constexpr bool value = true;
};

} // namespace freestanding
} // namespace fast_io
