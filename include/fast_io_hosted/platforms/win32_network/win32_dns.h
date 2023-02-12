#pragma once

namespace fast_io
{

template<win32_family fam>
requires (fam==win32_family::ansi_9x||fam==win32_family::wide_nt)
class win32_family_dns_io_observer
{
public:
	using char_type = std::conditional_t<fam==win32_family::wide_nt,char16_t,char>;
	using native_handle_type = ::fast_io::win32::win32_family_addrinfo<fam>*;
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

template<win32_family fam>
requires (fam==win32_family::ansi_9x||fam==win32_family::wide_nt)
struct win32_family_dns_iterator
{
	using native_handle_type = ::fast_io::win32::win32_family_addrinfo<fam>*;
	native_handle_type res{};
};

template<win32_family fam>
inline constexpr bool operator==(win32_family_dns_iterator<fam> a, ::std::default_sentinel_t) noexcept
{
	return a.res == nullptr;
}

template<win32_family fam>
inline constexpr bool operator!=(win32_family_dns_iterator<fam> a, ::std::default_sentinel_t) noexcept
{
	return a.res;
}

template<win32_family fam>
inline constexpr win32_family_dns_io_observer<fam> operator*(win32_family_dns_iterator<fam> d) noexcept
{
	return {d.res};
}

template<win32_family fam>
inline constexpr win32_family_dns_iterator<fam>& operator++(win32_family_dns_iterator<fam>& d) noexcept
{
	d.res = d.res->ai_next;
	return d;
}

template<win32_family fam>
inline constexpr win32_family_dns_iterator<fam> operator++(win32_family_dns_iterator<fam>& d, int) noexcept
{
	auto temp{d};
	++d;
	return temp;
}

namespace details
{
inline constexpr ::fast_io::ip_address win32_to_ip_address_with_ai_addr_impl(int ai_family,posix_sockaddr const* ai_addr) noexcept
{
	::fast_io::ip_address ret;
	switch(ai_family)
	{
	case 2:
	{
		::fast_io::details::my_memcpy(__builtin_addressof(ret.address.v4),__builtin_addressof(reinterpret_cast<posix_sockaddr_in const*>(ai_addr)->sin_addr),sizeof(posix_in_addr));
		ret.isv4=true;
		break;
	}
	case 23:
	{
		::fast_io::details::my_memcpy(__builtin_addressof(ret.address.v6),__builtin_addressof(reinterpret_cast<posix_sockaddr_in6 const*>(ai_addr)->sin6_addr),sizeof(posix_in6_addr));
		break;
	}
	}
	return ret;
}

inline constexpr ::fast_io::ip win32_to_ip_with_ai_addr_impl(int ai_family,posix_sockaddr const* ai_addr,std::uint_least16_t port) noexcept
{
	return ::fast_io::ip{win32_to_ip_address_with_ai_addr_impl(ai_family,ai_addr),port};
}

}

template<win32_family fam>
inline constexpr ::fast_io::ip to_ip(win32_family_dns_io_observer<fam> d,std::uint_least16_t port) noexcept
{
	return ::fast_io::details::win32_to_ip_with_ai_addr_impl(d.res->ai_family,d.res->ai_addr,port);
}

template<win32_family fam>
inline constexpr ::fast_io::ip_address to_ip_address(win32_family_dns_io_observer<fam> d) noexcept
{
	return ::fast_io::details::win32_to_ip_address_with_ai_addr_impl(d.res->ai_family,d.res->ai_addr);
}

namespace details
{

template<win32_family fam>
inline ::fast_io::win32::win32_family_addrinfo<fam>* win32_getaddrinfo_impl(std::conditional_t<fam==win32_family::wide_nt,char16_t,char> const* node,std::conditional_t<fam==win32_family::wide_nt,char16_t,char> const* service,::fast_io::win32::win32_family_addrinfo<fam> const* hints)
{
	::fast_io::win32::win32_family_addrinfo<fam>* res{};
	if constexpr(win32_family::ansi_9x==fam)
	{
		int ret{::fast_io::win32::getaddrinfo(node,service,hints,__builtin_addressof(res))};
		if(ret)
			throw_win32_error(static_cast<std::uint_least32_t>(ret));
	}
	else
	{
		int ret{::fast_io::win32::GetAddrInfoW(node,service,hints,__builtin_addressof(res))};
		if(ret)
			throw_win32_error(static_cast<std::uint_least32_t>(ret));
	}
	return res;
}

template<::fast_io::win32_family fam>
inline void win32_family_freeaddrinfo_impl(::fast_io::win32::win32_family_addrinfo<fam>* paddrinfo)
{
	if constexpr(fam==::fast_io::win32_family::ansi_9x)
	{
		::fast_io::win32::freeaddrinfo(paddrinfo);
	}
	else
	{
		::fast_io::win32::FreeAddrInfoW(paddrinfo);
	}
}


template<win32_family fam>
inline constexpr auto win32_family_dns_open_internal_impl(std::conditional_t<fam==win32_family::wide_nt,char16_t,char> const* node)
{
	constexpr ::fast_io::win32::win32_family_addrinfo<fam> info{.ai_family=0};
	return win32_getaddrinfo_impl<fam>(node,nullptr,__builtin_addressof(info));
}

template<win32_family fam>
struct win32_family_dns_open_parameter
{
	inline auto operator()(std::conditional_t<fam==win32_family::wide_nt,char16_t,char> const* node_name_c_str)
	{
		return ::fast_io::details::win32_family_dns_open_internal_impl<fam>(node_name_c_str);
	}
};

template<win32_family fam,typename T>
requires ::fast_io::constructible_to_os_c_str<T>
inline constexpr auto win32_family_dns_open_impl(T const& t)
{
	return ::fast_io::win32_family_api_common<fam>(t,win32_family_dns_open_parameter<fam>{});
}

}

template<win32_family fam>
class win32_family_dns_file:public win32_family_dns_io_observer<fam>
{
public:
	using typename win32_family_dns_io_observer<fam>::char_type;
	using typename win32_family_dns_io_observer<fam>::native_handle_type;
	constexpr win32_family_dns_file() noexcept = default;
	constexpr win32_family_dns_file(win32_family_dns_io_observer<fam>) noexcept=delete;
	constexpr win32_family_dns_file& operator=(win32_family_dns_io_observer<fam>) noexcept=delete;

	template<typename native_hd>
	requires std::same_as<native_handle_type,std::remove_cvref_t<native_hd>>
	explicit constexpr win32_family_dns_file(native_hd res1) noexcept: win32_family_dns_io_observer<fam>{res1}{}
	explicit constexpr win32_family_dns_file(decltype(nullptr)) noexcept = delete;
	win32_family_dns_file(char_type const* node,char_type const* service,::fast_io::win32::win32_family_addrinfo<fam> const* hints):win32_family_dns_io_observer<fam>{::fast_io::details::win32_getaddrinfo_impl<fam>(node,service,hints)}{}
	template<typename T>
	requires ::fast_io::constructible_to_os_c_str<T>
	explicit win32_family_dns_file(T const& t):win32_family_dns_io_observer<fam>{::fast_io::details::win32_family_dns_open_impl<fam>(t)}{}
	win32_family_dns_file(win32_family_dns_file const&)=delete;
	win32_family_dns_file& operator=(win32_family_dns_file const&)=delete;
	constexpr win32_family_dns_file(win32_family_dns_file&& __restrict other) noexcept: win32_family_dns_io_observer<fam>{other.res}
	{
		other.res=nullptr;
	}
	win32_family_dns_file& operator=(win32_family_dns_file&& __restrict other) noexcept
	{
		if(this->res)[[likely]]
			::fast_io::details::win32_family_freeaddrinfo_impl<fam>(this->res);
		this->res=other.res;
		other.res=nullptr;
		return *this;
	}
	void close() noexcept
	{
		if(this->res)[[likely]]
		{
			::fast_io::details::win32_family_freeaddrinfo_impl<fam>(this->res);
			this->res=nullptr;
		}
	}
	~win32_family_dns_file()
	{
		if(this->res)[[likely]]
			::fast_io::details::win32_family_freeaddrinfo_impl<fam>(this->res);
	}
};

template<win32_family fam>
inline constexpr win32_family_dns_iterator<fam> begin(win32_family_dns_io_observer<fam> pdniob) noexcept
{
	return {pdniob.res};
}

template<win32_family fam>
inline constexpr win32_family_dns_iterator<fam> cbegin(win32_family_dns_io_observer<fam> pdniob) noexcept
{
	return {pdniob.res};
}

template<win32_family fam>
inline constexpr ::std::default_sentinel_t end(win32_family_dns_io_observer<fam>) noexcept
{
	return {};
}

template<win32_family fam>
inline constexpr ::std::default_sentinel_t cend(win32_family_dns_io_observer<fam>) noexcept
{
	return {};
}

using ::fast_io::win32::win32_family_addrinfo;
using ::fast_io::win32::win32_addrinfo_9xa;
using ::fast_io::win32::win32_addrinfo_ntw;
using win32_addrinfo = ::fast_io::win32::win32_family_addrinfo<::fast_io::win32_family::native>;

using win32_dns_io_observer_9xa = win32_family_dns_io_observer<win32_family::ansi_9x>;
using win32_dns_io_observer_ntw = win32_family_dns_io_observer<win32_family::wide_nt>;
using win32_dns_io_observer = win32_family_dns_io_observer<win32_family::native>;

using win32_dns_file_9xa = win32_family_dns_file<win32_family::ansi_9x>;
using win32_dns_file_ntw = win32_family_dns_file<win32_family::wide_nt>;
using win32_dns_file = win32_family_dns_file<win32_family::native>;

using win32_dns_iterator_9xa = win32_family_dns_iterator<win32_family::ansi_9x>;
using win32_dns_iterator_ntw = win32_family_dns_iterator<win32_family::wide_nt>;
using win32_dns_iterator = win32_family_dns_iterator<win32_family::native>;

#if !defined(__WINE__) && !defined(__CYGWIN__)
using native_addrinfo = win32_addrinfo;

#if (defined(_MSC_VER) || (!defined(_WIN32_WINDOWS) && _WIN32_WINNT>=0x0501))
using native_dns_io_observer = win32_dns_io_observer;
using native_dns_file = win32_dns_file;
using native_dns_iterator = win32_dns_iterator;
#endif

#endif

}
