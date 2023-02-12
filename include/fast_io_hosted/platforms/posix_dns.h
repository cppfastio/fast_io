#pragma once

namespace fast_io
{

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
posix_addrinfo
{
int              ai_flags{};
int              ai_family{};
int              ai_socktype{};
int              ai_protocol{};
posix_socklen_t  ai_addrlen{};
#if defined(__BIONIC__) || defined(__BSD_VISIBLE) || defined(__CYGWIN__) || defined(__DARWIN_C_LEVEL) || defined(_PICOLIBC__)
char            *ai_canonname{};
posix_sockaddr  *ai_addr{};
#else
posix_sockaddr  *ai_addr{};
char            *ai_canonname{};
#endif
posix_addrinfo  *ai_next{};
};

namespace details
{
extern int libc_getaddrinfo(char const *node, char const* service,
                posix_addrinfo const *hints,
                posix_addrinfo **res) noexcept __asm__("getaddrinfo");

extern void libc_freeaddrinfo(posix_addrinfo *res) noexcept __asm__("freeaddrinfo");
}

class posix_dns_io_observer
{
public:
	using char_type = char;
	using native_handle_type = posix_addrinfo*;
	native_handle_type res{};
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return res;
	}
	explicit constexpr operator bool() noexcept
	{
		return res;
	}
	inline native_handle_type release() noexcept
	{
		native_handle_type temp{res};
		res=nullptr;
		return temp;
	}
};

struct posix_dns_iterator
{
	using native_handle_type = posix_addrinfo*;
	native_handle_type res{};
};

inline constexpr bool operator==(posix_dns_iterator a, ::std::default_sentinel_t) noexcept
{
	return a.res == nullptr;
}

inline constexpr bool operator!=(posix_dns_iterator a, ::std::default_sentinel_t) noexcept
{
	return a.res;
}

inline constexpr posix_dns_io_observer operator*(posix_dns_iterator d) noexcept
{
	return {d.res};
}

inline constexpr posix_dns_iterator& operator++(posix_dns_iterator& d) noexcept
{
	d.res = d.res->ai_next;
	return d;
}

inline constexpr posix_dns_iterator operator++(posix_dns_iterator& d, int) noexcept
{
	posix_dns_iterator temp{d};
	++d;
	return temp;
}

namespace details
{

inline constexpr ::fast_io::ip_address posix_to_ip_address_with_ai_addr_impl(int ai_family,posix_sockaddr const* ai_addr) noexcept
{
	::fast_io::ip_address ret;
	switch(ai_family)
	{
		case AF_INET:
		{
			::fast_io::details::my_memcpy(__builtin_addressof(ret.address.v4.address),__builtin_addressof(reinterpret_cast<posix_sockaddr_in const*>(ai_addr)->sin_addr),sizeof(posix_in_addr));
			ret.isv4=true;
			break;
		}
		case AF_INET6:
		{
			::fast_io::details::my_memcpy(__builtin_addressof(ret.address.v6.address),__builtin_addressof(reinterpret_cast<posix_sockaddr_in6 const*>(ai_addr)->sin6_addr),sizeof(posix_in6_addr));
			break;
		}
	}
	return ret;
}

inline constexpr ::fast_io::ip posix_to_ip_with_ai_addr_impl(int ai_family,posix_sockaddr const* ai_addr,std::uint_least16_t port) noexcept
{
	return ::fast_io::ip{posix_to_ip_address_with_ai_addr_impl(ai_family,ai_addr),port};
}

inline posix_addrinfo* my_getaddrinfo_impl(char const* node,char const* service,posix_addrinfo const* hints)
{
	posix_addrinfo* res{};
	int ec{libc_getaddrinfo(node,service,hints,__builtin_addressof(res))};
	if(ec)
		throw_posix_error(ec);
	return res;
}

inline auto posix_dns_open_internal_impl(char const* node)
{
	constexpr posix_addrinfo info{.ai_family=AF_UNSPEC};
	return my_getaddrinfo_impl(node,nullptr,__builtin_addressof(info));
}

struct posix_dns_open_parameter
{
	inline auto operator()(char const* node_name_c_str)
	{
		return ::fast_io::details::posix_dns_open_internal_impl(node_name_c_str);
	}
};

template<typename T>
requires ::fast_io::constructible_to_os_c_str<T>
inline constexpr auto posix_dns_open_impl(T const& t)
{
	return ::fast_io::posix_api_common(t,posix_dns_open_parameter{});
}

}

inline constexpr ::fast_io::ip to_ip(posix_dns_io_observer d,std::uint_least16_t port) noexcept
{
	return fast_io::details::posix_to_ip_with_ai_addr_impl(d.res->ai_family,d.res->ai_addr,port);
}

inline constexpr ::fast_io::ip_address to_ip_address(posix_dns_io_observer d) noexcept
{
	return fast_io::details::posix_to_ip_address_with_ai_addr_impl(d.res->ai_family,d.res->ai_addr);
}

class
#if __has_cpp_attribute(clang::trivially_relocatable)
[[clang::trivially_relocatable]]
#endif
posix_dns_file:public posix_dns_io_observer
{
public:
	using typename posix_dns_io_observer::char_type;
	using typename posix_dns_io_observer::native_handle_type;
	constexpr posix_dns_file() noexcept = default;
	constexpr posix_dns_file(posix_dns_io_observer) noexcept=delete;
	constexpr posix_dns_file& operator=(posix_dns_io_observer) noexcept=delete;
	template<typename native_hd>
	requires std::same_as<native_handle_type,std::remove_cvref_t<native_hd>>
	explicit constexpr posix_dns_file(native_hd res1) noexcept: posix_dns_io_observer(res1){}
	explicit constexpr posix_dns_file(decltype(nullptr)) noexcept = delete;
	posix_dns_file(char_type const* node,char_type const* service,posix_addrinfo const* hints):posix_dns_io_observer{details::my_getaddrinfo_impl(node,service,hints)}{}
	template<typename T>
	requires ::fast_io::constructible_to_os_c_str<T>
	explicit posix_dns_file(T const& t):posix_dns_io_observer{::fast_io::details::posix_dns_open_impl(t)}{}
	posix_dns_file(posix_dns_file const&)=delete;
	posix_dns_file& operator=(posix_dns_file const&)=delete;
	constexpr posix_dns_file(posix_dns_file&& __restrict other) noexcept: posix_dns_io_observer{other.res}
	{
		other.res=nullptr;
	}
	posix_dns_file& operator=(posix_dns_file&& __restrict other) noexcept
	{
		if(this->res)[[likely]]
			details::libc_freeaddrinfo(this->res);
		this->res=other.res;
		other.res=nullptr;
		return *this;
	}
	void close() noexcept
	{
		if(this->res)[[likely]]
		{
			details::libc_freeaddrinfo(this->res);
			this->res=nullptr;
		}
	}
	~posix_dns_file()
	{
		if(this->res)[[likely]]
			details::libc_freeaddrinfo(this->res);
	}
};

inline constexpr posix_dns_iterator begin(posix_dns_io_observer pdniob) noexcept
{
	return {pdniob.res};
}

inline constexpr posix_dns_iterator cbegin(posix_dns_io_observer pdniob) noexcept
{
	return {pdniob.res};
}

inline constexpr ::std::default_sentinel_t end(posix_dns_io_observer) noexcept
{
	return {};
}

inline constexpr ::std::default_sentinel_t cend(posix_dns_io_observer) noexcept
{
	return {};
}

using native_addrinfo = posix_addrinfo;
using native_dns_io_observer = posix_dns_io_observer;
using native_dns_file = posix_dns_file;
using native_dns_iterator = posix_dns_iterator;

}
