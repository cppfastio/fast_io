#pragma once

namespace fast_io
{

/*
 * WICG local-network-access address spaces — loopback ⊂ local ⊂ public
 * as a numeric order, so `space > cap` rejects everything broader than
 * the cap. https://wicg.github.io/local-network-access/
 * (`local`/`public` collide with keywords elsewhere only in qualified
 * use — the `_address` suffix keeps the enumerators unambiguous)
 */
enum class ip_address_space : ::std::uint_least8_t
{
	loopback_address = 0, /* this machine — loopback */
	local_address = 1,    /* LAN — RFC1918, link-local, CGN, ULA */
	public_address = 2    /* anything reachable */
};

namespace details
{
/* unwraps ::ffff:a.b.c.d to its embedded v4 — the mapped form is how a
 * dual-stack socket reports a v4 peer; for matching and classification
 * it IS the v4 address */
inline ::fast_io::ip_address lna_unmap_v4_mapped_impl(::fast_io::ip_address const &a) noexcept
{
	if (a.isv4)
	{
		return a;
	}
	auto const *b{reinterpret_cast<unsigned char const *>(__builtin_addressof(a.address.v6))};
	if (__builtin_memcmp(b, "\0\0\0\0\0\0\0\0\0\0", 10) == 0 && b[10] == 0xff && b[11] == 0xff)
	{
		::fast_io::ip_address v4{};
		v4.isv4 = true;
		__builtin_memcpy(v4.address.v4.address, b + 12, 4);
		return v4;
	}
	return a;
}

/* the spec's "Non-public IP address blocks" table: classify by block */
inline ::fast_io::ip_address_space lna_classify_ip_address_impl(
	::fast_io::ip_address const &a0) noexcept
{
	auto const a{::fast_io::details::lna_unmap_v4_mapped_impl(a0)};
	if (a.isv4)
	{
		auto const *b{a.address.v4.address};
		if (b[0] == 127 || b[0] == 0 ||           /* 127/8 loopback, 0/8 null */
			(b[0] == 198 && (b[1] & 0xfe) == 18)) /* 198.18.0.0/15 benchmarking */
		{
			return ::fast_io::ip_address_space::loopback_address;
		}
		if (b[0] == 10 || (b[0] == 172 && (b[1] & 0xf0) == 16) ||
			(b[0] == 192 && b[1] == 168) || (b[0] == 169 && b[1] == 254) ||
			(b[0] == 100 && (b[1] & 0xc0) == 64)) /* RFC1918 + CGN + link-local */
		{
			return ::fast_io::ip_address_space::local_address;
		}
		return ::fast_io::ip_address_space::public_address;
	}
	auto const *b{reinterpret_cast<unsigned char const *>(__builtin_addressof(a.address.v6))};
	bool all_zero{true};
	for (unsigned i{1}; i != 15; ++i)
	{
		if (b[i] != 0)
		{
			all_zero = false;
			break;
		}
	}
	if (all_zero && (b[0] == 0 && (b[15] == 0 || b[15] == 1))) /* :: and ::1 */
	{
		return ::fast_io::ip_address_space::loopback_address;
	}
	if ((b[0] & 0xfe) == 0xfc ||                   /* fc00::/7 ULA */
		(b[0] == 0xfe && (b[1] & 0xc0) == 0x80) || /* fe80::/10 link-local */
		(b[0] == 0xfe && (b[1] & 0xc0) == 0xc0))   /* fec0::/10 site-local */
	{
		return ::fast_io::ip_address_space::local_address;
	}
	return ::fast_io::ip_address_space::public_address;
}
} // namespace details

inline ::fast_io::ip_address_space lna_classify(::fast_io::ip_address const &a) noexcept
{
	return ::fast_io::details::lna_classify_ip_address_impl(a);
}

inline ::fast_io::ip_address_space lna_classify(::fast_io::ip const &v) noexcept
{
	return ::fast_io::details::lna_classify_ip_address_impl(v.address);
}

inline bool lna_allows(::fast_io::ip_address const &peer, ::fast_io::ip_address_space max) noexcept
{
	return ::fast_io::lna_classify(peer) <= max;
}

inline bool lna_allows(::fast_io::ip const &peer, ::fast_io::ip_address_space max) noexcept
{
	return ::fast_io::lna_classify(peer.address) <= max;
}

/* peer-address equality after v4-mapped normalization — ::ffff:a.b.c.d
 * matches its embedded a.b.c.d literal */
inline bool lna_peer_eq(::fast_io::ip_address const &a, ::fast_io::ip_address const &b) noexcept
{
	return ::fast_io::details::lna_unmap_v4_mapped_impl(a) ==
		   ::fast_io::details::lna_unmap_v4_mapped_impl(b);
}

/* what an --lna-style string argument can name: an address space, an
 * ip literal, or a hostname (resolved by the caller — a ".local" mDNS
 * name is a hostname, not an address space, per the spec) */
enum class lna_rule_kind : ::std::uint_least8_t
{
	space,
	literal,
	name
};

struct lna_rule
{
	::fast_io::ip_address_space space{::fast_io::ip_address_space::local_address};
	::fast_io::ip_address address{};
	::fast_io::lna_rule_kind kind{::fast_io::lna_rule_kind::space};
};

namespace details
{
template <::std::integral ch_type>
inline bool lna_str_eq(ch_type const *first, ch_type const *last, char const *lit,
					   ::std::size_t lit_len) noexcept
{
	if (static_cast<::std::size_t>(last - first) != lit_len)
	{
		return false;
	}
	for (::std::size_t i{}; i != lit_len; ++i)
	{
		/* ascii literal — identity mapping for every fast_io char type */
		if (first[i] != static_cast<ch_type>(static_cast<unsigned char>(lit[i])))
		{
			return false;
		}
	}
	return true;
}
} // namespace details

/* parse an access rule: "loopback" | "local" | "public" | an ip literal
 * | anything else is a hostname for the caller to resolve. Bare,
 * shortened and bracketed v6 literals all parse, as do v4-mapped v6 */
template <::std::integral ch_type>
inline ::fast_io::lna_rule lna_parse(ch_type const *first, ch_type const *last) noexcept
{
	::fast_io::lna_rule rule;
	if (details::lna_str_eq(first, last, "loopback", 8))
	{
		rule.space = ::fast_io::ip_address_space::loopback_address;
		return rule;
	}
	if (details::lna_str_eq(first, last, "local", 5))
	{
		rule.space = ::fast_io::ip_address_space::local_address;
		return rule;
	}
	if (details::lna_str_eq(first, last, "public", 6))
	{
		rule.space = ::fast_io::ip_address_space::public_address;
		return rule;
	}
	/* literal: try v4 then v6 through the contiguous in_addr scanners —
	 * the ip_address/ip scan entries are stubs (@todo in addrscn.h), and
	 * a literal must consume the whole string: a trailing word makes the
	 * arg a hostname instead */
	::fast_io::posix_in_addr in4{};
	auto const [it4, ec4]{::fast_io::details::scn_cnt_define_inaddr_impl(first, last, in4)};
	if (ec4 == ::fast_io::freestanding::parse_errc::ok && it4 == last)
	{
		rule.address.address.v4 = in4;
		rule.address.isv4 = true;
		rule.kind = ::fast_io::lna_rule_kind::literal;
		return rule;
	}
	::fast_io::posix_in6_addr in6{};
	auto const [it6, ec6]{
		::fast_io::details::scn_cnt_define_in6addr_impl<true, true, true, false, true>(
			first, last, in6)};
	if (ec6 == ::fast_io::freestanding::parse_errc::ok && it6 == last)
	{
		rule.address.address.v6 = in6;
		rule.kind = ::fast_io::lna_rule_kind::literal;
		return rule;
	}
	rule.kind = ::fast_io::lna_rule_kind::name;
	return rule;
}

template <::fast_io::constructible_to_os_c_str str_type>
inline ::fast_io::lna_rule lna_parse(str_type const &sv) noexcept
{
	if constexpr (requires { sv.size(); })
	{
		return ::fast_io::lna_parse(sv.data(), sv.data() + sv.size());
	}
	else
	{
		auto const *p{sv.c_str()};
		return ::fast_io::lna_parse(p, p + ::fast_io::cstr_len(p));
	}
}

} // namespace fast_io
