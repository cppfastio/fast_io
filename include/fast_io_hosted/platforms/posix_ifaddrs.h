#pragma once

#if __has_include(<ifaddrs.h>)
#include <ifaddrs.h>
#endif

namespace fast_io
{

/*
 * posix_ifaddrs mirrors libc's struct ifaddrs with its own declaration,
 * the same trade posix_addrinfo makes: the type exists even where
 * ifaddrs.h does not, and the libc entry points are reached through
 * __asm__-named externs rather than the C header's types.
 */
struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	posix_ifaddrs
{
	posix_ifaddrs *ifa_next;
	char *ifa_name;
	unsigned ifa_flags;
	posix_sockaddr *ifa_addr;
	posix_sockaddr *ifa_netmask;
	union
	{
		posix_sockaddr *ifu_broadaddr;
		posix_sockaddr *ifu_dstaddr;
	} ifa_ifu;
	void *ifa_data;
};

namespace details
{
#ifdef __DARWIN_C_LEVEL
extern int libc_getifaddrs(posix_ifaddrs **ifap) noexcept __asm__("_getifaddrs");

extern void libc_freeifaddrs(posix_ifaddrs *ifa) noexcept __asm__("_freeifaddrs");
#else
extern int libc_getifaddrs(posix_ifaddrs **ifap) noexcept __asm__("getifaddrs");

extern void libc_freeifaddrs(posix_ifaddrs *ifa) noexcept __asm__("freeifaddrs");
#endif

inline posix_ifaddrs *my_getifaddrs_impl() FAST_IO_HERBCEPTIONS_THROWS
{
	posix_ifaddrs *res{};
	if (libc_getifaddrs(__builtin_addressof(res)) == -1)
	{
		throw_posix_error();
	}
	return res;
}
} // namespace details

class posix_ifaddrs_io_observer
{
public:
	using char_type = char;
	using native_handle_type = posix_ifaddrs *;
	native_handle_type res{};
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return res;
	}
	inline explicit constexpr operator bool() noexcept
	{
		return res;
	}
	inline native_handle_type release() noexcept
	{
		native_handle_type temp{res};
		res = nullptr;
		return temp;
	}
};

struct posix_ifaddrs_iterator
{
	using native_handle_type = posix_ifaddrs *;
	native_handle_type res{};
};

inline constexpr bool operator==(posix_ifaddrs_iterator a, ::std::default_sentinel_t) noexcept
{
	return a.res == nullptr;
}

inline constexpr bool operator!=(posix_ifaddrs_iterator a, ::std::default_sentinel_t) noexcept
{
	return a.res;
}

inline constexpr posix_ifaddrs_io_observer operator*(posix_ifaddrs_iterator d) noexcept
{
	return {d.res};
}

inline constexpr posix_ifaddrs_iterator &operator++(posix_ifaddrs_iterator &d) noexcept
{
	d.res = d.res->ifa_next;
	return d;
}

inline constexpr posix_ifaddrs_iterator operator++(posix_ifaddrs_iterator &d, int) noexcept
{
	posix_ifaddrs_iterator temp{d};
	++d;
	return temp;
}

/*
 * Entry accessors. The names deliberately avoid ifa_addr-style member
 * spelling: ifa_broadaddr/ifa_dstaddr are #defines in the real
 * <ifaddrs.h>, and function names must survive its inclusion.
 */
inline constexpr ::fast_io::manipulators::basic_os_c_str<char>
name(posix_ifaddrs_io_observer e) noexcept
{
	return {e.res->ifa_name};
}

inline constexpr unsigned flags(posix_ifaddrs_io_observer e) noexcept
{
	return e.res->ifa_flags;
}

/* ifa_addr's family mapped onto sock_family — getifaddrs also returns
 * link-layer families (AF_PACKET & friends), so callers filter on
 * sock_family::inet/inet6 rather than assuming every entry is an IP */
inline constexpr ::fast_io::sock_family family(posix_ifaddrs_io_observer e) noexcept
{
	auto const *sa{e.res->ifa_addr};
	if (sa == nullptr)
	{
		return ::fast_io::sock_family::unspec;
	}
	if (sa->sa_family == to_posix_sock_family(::fast_io::sock_family::inet))
	{
		return ::fast_io::sock_family::inet;
	}
	if (sa->sa_family == to_posix_sock_family(::fast_io::sock_family::inet6))
	{
		return ::fast_io::sock_family::inet6;
	}
	return ::fast_io::sock_family::unspec;
}

namespace details
{
/* shares the sockaddr→ip conversion from posix_netop.h — only the
 * address half is needed here */
inline ::fast_io::ip_address posix_sockaddr_to_ip_address_impl(posix_sockaddr const *sa) noexcept
{
	return details::posix_sockaddr_to_ip_impl(sa).address;
}
} // namespace details

/* the interface's own address — the getifaddrs ifa_addr field */
inline ::fast_io::ip_address address(posix_ifaddrs_io_observer e) noexcept
{
	return details::posix_sockaddr_to_ip_address_impl(e.res->ifa_addr);
}

inline ::fast_io::ip_address netmask(posix_ifaddrs_io_observer e) noexcept
{
	return details::posix_sockaddr_to_ip_address_impl(e.res->ifa_netmask);
}

/* broadcast and point-to-point destination share the ifa_ifu union */
inline ::fast_io::ip_address broadcast(posix_ifaddrs_io_observer e) noexcept
{
	return details::posix_sockaddr_to_ip_address_impl(e.res->ifa_ifu.ifu_broadaddr);
}

inline ::fast_io::ip_address dstaddr(posix_ifaddrs_io_observer e) noexcept
{
	return details::posix_sockaddr_to_ip_address_impl(e.res->ifa_ifu.ifu_dstaddr);
}

class posix_ifaddrs_file FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE : public posix_ifaddrs_io_observer
{
public:
	using typename posix_ifaddrs_io_observer::char_type;
	using typename posix_ifaddrs_io_observer::native_handle_type;
	inline constexpr posix_ifaddrs_file() noexcept = default;
	inline constexpr posix_ifaddrs_file(posix_ifaddrs_io_observer) noexcept = delete;
	inline constexpr posix_ifaddrs_file &operator=(posix_ifaddrs_io_observer) noexcept = delete;
	template <typename native_hd>
		requires ::std::same_as<native_handle_type, ::std::remove_cvref_t<native_hd>>
	inline explicit constexpr posix_ifaddrs_file(native_hd res1) noexcept
		: posix_ifaddrs_io_observer(res1)
	{
	}
	explicit constexpr posix_ifaddrs_file(decltype(nullptr)) noexcept = delete;
	inline posix_ifaddrs_file(ifaddrs_enumerate_t) FAST_IO_HERBCEPTIONS_THROWS
		: posix_ifaddrs_io_observer{details::my_getifaddrs_impl()}
	{
	}
	inline posix_ifaddrs_file(posix_ifaddrs_file const &) = delete;
	inline posix_ifaddrs_file &operator=(posix_ifaddrs_file const &) = delete;
	inline constexpr posix_ifaddrs_file(posix_ifaddrs_file &&__restrict other) noexcept
		: posix_ifaddrs_io_observer{other.res}
	{
		other.res = nullptr;
	}
	inline posix_ifaddrs_file &operator=(posix_ifaddrs_file &&__restrict other) noexcept
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		if (this->res) [[likely]]
		{
			details::libc_freeifaddrs(this->res);
		}
		this->res = other.res;
		other.res = nullptr;
		return *this;
	}
	inline void close() noexcept
	{
		if (this->res) [[likely]]
		{
			details::libc_freeifaddrs(this->res);
			this->res = nullptr;
		}
	}
	inline ~posix_ifaddrs_file()
	{
		if (this->res) [[likely]]
		{
			details::libc_freeifaddrs(this->res);
		}
	}
};

inline constexpr posix_ifaddrs_iterator begin(posix_ifaddrs_io_observer ob) noexcept
{
	return {ob.res};
}

inline constexpr posix_ifaddrs_iterator cbegin(posix_ifaddrs_io_observer ob) noexcept
{
	return {ob.res};
}

inline constexpr ::std::default_sentinel_t end(posix_ifaddrs_io_observer) noexcept
{
	return {};
}

inline constexpr ::std::default_sentinel_t cend(posix_ifaddrs_io_observer) noexcept
{
	return {};
}

using native_ifaddrs = posix_ifaddrs;
using native_ifaddrs_io_observer = posix_ifaddrs_io_observer;
using native_ifaddrs_file = posix_ifaddrs_file;
using native_ifaddrs_iterator = posix_ifaddrs_iterator;

namespace freestanding
{
template <>
struct is_trivially_copyable_or_relocatable<posix_ifaddrs_file>
{
	inline static constexpr bool value = true;
};

} // namespace freestanding
} // namespace fast_io
