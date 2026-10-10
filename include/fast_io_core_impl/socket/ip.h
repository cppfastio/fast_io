#pragma once

namespace fast_io
{

struct ipv4
{
	posix_in_addr address{};
	::std::uint_least16_t port{};
};

struct ipv6
{
	posix_in6_addr address{};
	::std::uint_least16_t port{};
};

struct ip_address
{
	union
	{
		posix_in_addr v4;
		posix_in6_addr v6;
	} address{.v6 = {}};
	bool isv4{};
};

inline constexpr bool operator==(ip_address const &a, ip_address const &b) noexcept
{
	if (a.isv4 != b.isv4)
	{
		return false;
	}
	if (a.isv4)
	{
		for (unsigned i{}; i != 4; ++i)
		{
			if (a.address.v4.address[i] != b.address.v4.address[i])
			{
				return false;
			}
		}
		return true;
	}
	for (unsigned i{}; i != 8; ++i)
	{
		if (a.address.v6.address[i] != b.address.v6.address[i])
		{
			return false;
		}
	}
	return true;
}

struct ip
{
	ip_address address;
	::std::uint_least16_t port{};

	inline constexpr bool is_ipv4() const noexcept
	{
		return address.isv4;
	}
	inline constexpr bool is_ipv6() const noexcept
	{
		return !address.isv4;
	}

	inline explicit constexpr ip() noexcept = default;
	inline explicit constexpr ip(ipv4 add) noexcept
		: address{.address = {.v4 = add.address}, .isv4 = true}, port(add.port)
	{}
	inline explicit constexpr ip(ipv6 add) noexcept
		: address{.address = {.v6 = add.address}}, port(add.port)
	{}
	inline explicit constexpr ip(ip_address addr, ::std::uint_least16_t prt) noexcept
		: address{addr}, port{prt}
	{}
};

} // namespace fast_io
