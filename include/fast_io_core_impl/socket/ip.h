#pragma once

namespace fast_io
{

struct ipv4
{
	posix_in_addr address{};
	std::uint_least16_t port{};
};

struct ipv6
{
	posix_in6_addr address{};
	std::uint_least16_t port{};
};

struct ip_address
{
	union
	{
		posix_in_addr v4;
		posix_in6_addr v6;
	}address{.v6={}};
	bool isv4{};
};

struct ip
{
	ip_address address;
	std::uint_least16_t port{};

	inline constexpr bool is_ipv4() const noexcept
	{
		return address.isv4;
	}
	inline constexpr bool is_ipv6() const noexcept
	{
		return !address.isv4;
	}

	explicit constexpr ip() noexcept = default;
	explicit constexpr ip(ipv4 add):address{.address={.v4=add.address},.isv4=true},port(add.port){}
	explicit constexpr ip(ipv6 add):address{.address={.v6=add.address}},port(add.port){}
	explicit constexpr ip(ip_address addr,::std::uint_least16_t prt):address{addr},port{prt}{}
};

}
