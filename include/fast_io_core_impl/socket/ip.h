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

struct ip
{
	union
	{
		posix_in_addr v4;
		posix_in6_addr v6;
	}address{.v6={}};
	std::uint_least16_t port{};
	bool isv4{};
	constexpr ip() noexcept = default;
	explicit constexpr ip(ipv4 add):address{.v4=add.address},port(add.port),isv4{true}{}
	explicit constexpr ip(ipv6 add):address{.v6=add.address},port(add.port){}
};


}
