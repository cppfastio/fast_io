#pragma once

namespace fast_io
{


namespace details
{

template<::std::integral char_type>
inline constexpr char_type* prtrsv_inaddr_common_define_impl(char_type* it,char unsigned const * __restrict start) noexcept
{
	for(auto i{start},e{start+4};i!=e;++i)
	{
		if(i!=start)
		{
			*it=::fast_io::char_literal_v<u8'.',char_type>;
			++it;
		}
		it=print_reserve_integral_define<10>(it,*i);
	}
	return it;
}

template<bool shorten,bool uppercase,bool showv6bracket,bool full,::std::integral char_type>
inline constexpr char_type* prtrsv_in6addr_common_define_impl(char_type *it,::std::uint_least16_t const * __restrict start) noexcept
{
	if constexpr(showv6bracket)
	{
		*it = ::fast_io::char_literal_v<u8'[',char_type>;
		++it;
		it = prtrsv_in6addr_common_define_impl<shorten,uppercase,false,full>(it,start);
		*it = ::fast_io::char_literal_v<u8']',char_type>;
		++it;
	}
	else if constexpr(shorten)
	{
		auto e{start+8};
		auto maxposition{e},maxposition_end{e};
		for(auto i{start};i!=e;)
		{
			if(*i==0)
			{
				auto j{i+1};
				for(;j!=e&&*j==0;++j);
				::std::size_t diff{static_cast<::std::size_t>(j-i)};
				if(1<diff)
				{
					::std::size_t max_last{static_cast<::std::size_t>(maxposition_end-maxposition)};
					if(max_last<diff)
					{
						maxposition = i;
						maxposition_end = j;
					}
				}
				i = j;
			}
			else
			{
				++i;
			}
		}
		for(auto i{start},e{start+8};i!=e;)
		{
			if(maxposition==i)
			{
				if constexpr(::std::same_as<char_type,char>)
				{
					it = copy_string_literal("::",it);
				}
				else if constexpr(::std::same_as<char_type,wchar_t>)
				{
					it = copy_string_literal(L"::",it);
				}
				else if constexpr(::std::same_as<char_type,char16_t>)
				{
					it = copy_string_literal(u"::",it);
				}
				else if constexpr(::std::same_as<char_type,char32_t>)
				{
					it = copy_string_literal(U"::",it);
				}
				else
				{
					it = copy_string_literal(u8"::",it);
				}
				i = maxposition_end;
			}
			else
			{
				if(i!=start&&i!=maxposition_end)
				{
					*it=::fast_io::char_literal_v<u8':',char_type>;
					++it;
				}
				it=print_reserve_integral_define<16,false,false,false,uppercase,full>(it,::fast_io::big_endian(*i));
				++i;
			}
		}
	}
	else
	{
		for(auto i{start},e{start+8};i!=e;++i)
		{
			if(i!=start)
			{
				*it=::fast_io::char_literal_v<u8':',char_type>;
				++it;
			}
			it=print_reserve_integral_define<16,false,false,false,uppercase,full>(it,::fast_io::big_endian(*i));
		}
	}
	return it;
}

template<::std::integral char_type,::std::unsigned_integral porttype>
inline constexpr char_type* prtrsv_ipport_define_impl(char_type* it,porttype port) noexcept
{
	*it=::fast_io::char_literal_v<u8':',char_type>;
	++it;
	return ::fast_io::details::print_reserve_integral_define<10>(it,port);
}

template<::std::integral char_type>
inline constexpr char_type* prtrsv_ipport_zero_define_impl(char_type* it) noexcept
{
	if constexpr(::std::same_as<char_type,char>)
	{
		return copy_string_literal(":0",it);
	}
	else if constexpr(::std::same_as<char_type,wchar_t>)
	{
		return copy_string_literal(L":0",it);
	}
	else if constexpr(::std::same_as<char_type,char16_t>)
	{
		return copy_string_literal(u":0",it);
	}
	else if constexpr(::std::same_as<char_type,char32_t>)
	{
		return copy_string_literal(U":0",it);
	}
	else
	{
		return copy_string_literal(u8":0",it);
	}
}

template<bool showport,::std::integral char_type>
inline constexpr char_type* prtrsv_inaddr_define_impl(char_type* it,::fast_io::posix_in_addr v) noexcept
{
	it = prtrsv_inaddr_common_define_impl(it,v.address);
	if constexpr(showport)
	{
		it = prtrsv_ipport_zero_define_impl(it);
	}
	return it;
}

template<bool showport,::std::integral char_type>
inline constexpr char_type* prtrsv_ipv4_define_impl(char_type* it,::fast_io::ipv4 v) noexcept
{
	it = prtrsv_inaddr_common_define_impl(it,v.address.address);
	if constexpr(showport)
	{
		it = prtrsv_ipport_define_impl(it,v.port);
	}
	return it;
}

template<bool v6shorten,bool uppercase,bool showv6bracket,bool showport,bool full,::std::integral char_type>
inline constexpr char_type* prtrsv_inaddr6_define_impl(char_type* it,::fast_io::posix_in6_addr v) noexcept
{
	it = prtrsv_in6addr_common_define_impl<v6shorten,uppercase,showport?true:showv6bracket,full>(it,v.address);
	if constexpr(showport)
	{
		it = prtrsv_ipport_zero_define_impl(it);
	}
	return it;
}

template<bool v6shorten,bool uppercase,bool showv6bracket,bool showport,bool full,::std::integral char_type>
inline constexpr char_type* prtrsv_ipv6_define_impl(char_type* it,::fast_io::ipv6 v) noexcept
{
	it = prtrsv_in6addr_common_define_impl<v6shorten,uppercase,showport?true:showv6bracket,full>(it,v.address.address);
	if constexpr(showport)
	{
		it = prtrsv_ipport_define_impl(it,v.port);
	}
	return it;
}

template<bool v6shorten,bool uppercase,bool showv6bracket,bool showport,bool full,::std::integral char_type>
inline constexpr char_type* prtrsv_ip_address_define_impl(char_type *it,::fast_io::ip_address v) noexcept
{
	if constexpr(showport)
	{
		it = prtrsv_ip_address_define_impl<v6shorten,uppercase,showv6bracket,false,full>(it,v);
		return prtrsv_ipport_zero_define_impl(it);
	}
	else
	{
		if(v.isv4)
		{
			it = prtrsv_inaddr_common_define_impl(it,v.address.v4.address);
		}
		else
		{
			it = prtrsv_in6addr_common_define_impl<v6shorten,uppercase,showport?true:showv6bracket,full>(it,v.address.v6.address);
		}
		return it;
	}
}

template<bool v6shorten,bool uppercase,bool showv6bracket,bool showport,bool full,::std::integral char_type>
inline constexpr char_type* prtrsv_ip_define_impl(char_type* it,::fast_io::ip v) noexcept
{
	it = prtrsv_ip_address_define_impl<v6shorten,uppercase,(showport?true:showv6bracket),false,full>(it,v.address);
	if constexpr(showport)
	{
		it = prtrsv_ipport_define_impl(it,v.port);
	}
	return it;
}

template<typename T>
concept inaddrnocvrefimpl = ::std::same_as<T,::fast_io::posix_in_addr>||
			::std::same_as<T,::fast_io::posix_in6_addr>||
			::std::same_as<T,::fast_io::ip_address>;

template<typename T>
concept iptypesnocvrefimpl = ::fast_io::details::inaddrnocvrefimpl<T>||
			::std::same_as<T,::fast_io::ipv4>||
			::std::same_as<T,::fast_io::ipv6>||
			::std::same_as<T,::fast_io::ip>;

template<typename T>
concept inaddrimpl = ::fast_io::details::inaddrnocvrefimpl<::std::remove_cvref_t<T>>;

template<typename T>
concept iptypesimpl = ::fast_io::details::iptypesnocvrefimpl<::std::remove_cvref_t<T>>;

}

template<::fast_io::details::iptypesimpl iptype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::manipulators::ip_manip_t<
	::fast_io::details::inaddrimpl<iptype>?
	::fast_io::manipulators::ip_default_inaddr_flags
	:
	::fast_io::manipulators::ip_default_flags,::std::remove_cvref_t<iptype>> print_alias_define(io_alias_t,iptype t) noexcept
{
	return {t};
}

template<::std::integral char_type,::fast_io::manipulators::ip_flags flags,::fast_io::details::iptypesimpl iptype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::std::size_t
	print_reserve_size(::fast_io::io_reserve_type_t<char_type,
		::fast_io::manipulators::ip_manip_t<flags,iptype>>) noexcept
{
	using nocvreftype = ::std::remove_cvref_t<iptype>;


	constexpr
		::std::size_t portsize{::fast_io::details::print_integer_reserved_size_cache<10,false,false,::std::uint_least16_t>};

	constexpr
		::std::size_t v4intsize{::fast_io::details::print_integer_reserved_size_cache<10,false,false,char unsigned>};

	constexpr
		::std::size_t v4totalsize{(v4intsize+1)*4-1};

	constexpr
		::std::size_t v4sizewithport{v4totalsize+portsize+1};

	constexpr
		::std::size_t prefixlength{v4intsize+portsize+1};


	if constexpr(::std::same_as<nocvreftype,::fast_io::posix_in_addr>||
			::std::same_as<nocvreftype,::fast_io::ipv4>)
	{
		if constexpr(flags.showport)
		{
			return v4sizewithport;
		}
		else
		{
			return v4totalsize;
		}
	}
	else
	{
		constexpr
			::std::size_t v6intsize{portsize};

		constexpr
			::std::size_t v6totalsize{(v6intsize+1)*8-1};

		constexpr
			::std::size_t v6totalsizebracket{v6totalsize+2};

		constexpr
			::std::size_t v6sizewithport{v6totalsizebracket+2+portsize};

		static_assert(v4intsize<v6totalsize&&v4sizewithport<v6sizewithport);

		if constexpr(flags.showport)
		{
			return v6sizewithport;
		}
		else if constexpr(flags.v6bracket)
		{
			return v6totalsizebracket;
		}
		else
		{
			return v6totalsize;
		}
	}
}

template<::std::integral char_type,::fast_io::manipulators::ip_flags flags,::fast_io::details::iptypesimpl iptype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr char_type* print_reserve_define(::fast_io::io_reserve_type_t<char_type,
		::fast_io::manipulators::ip_manip_t<flags,iptype>>,char_type* iter,::fast_io::manipulators::ip_manip_t<flags,iptype> val) noexcept
{
	using nocvreftype = ::std::remove_cvref_t<iptype>;
	if constexpr(::std::same_as<iptype,::fast_io::posix_in_addr>)
	{
		return ::fast_io::details::prtrsv_inaddr_define_impl<flags.showport>(iter,val.reference);
	}
	else if constexpr(::std::same_as<iptype,::fast_io::ipv4>)
	{
		return ::fast_io::details::prtrsv_ipv4_define_impl<flags.showport>(iter,val.reference);
	}
	else if constexpr(::std::same_as<iptype,::fast_io::posix_in6_addr>)
	{
		return ::fast_io::details::prtrsv_inaddr6_define_impl<flags.v6shorten,flags.v6uppercase,
			flags.showport?true:flags.v6bracket,flags.showport,
			flags.v6full>(iter,val.reference);
	}
	else if constexpr(::std::same_as<iptype,::fast_io::ipv6>)
	{
		return ::fast_io::details::prtrsv_ipv6_define_impl<flags.v6shorten,flags.v6uppercase,
			flags.showport?true:flags.v6bracket,flags.showport,
			flags.v6full>(iter,val.reference);
	}
	else if constexpr(::std::same_as<iptype,::fast_io::ip_address>)
	{
		return ::fast_io::details::prtrsv_ip_address_define_impl<flags.v6shorten,flags.v6uppercase,
			flags.showport?true:flags.v6bracket,flags.showport,
			flags.v6full>(iter,val.reference);
	}
	else
	{
		return ::fast_io::details::prtrsv_ip_define_impl<flags.v6shorten,flags.v6uppercase,
			flags.showport?true:flags.v6bracket,flags.showport,
			flags.v6full>(iter,val.reference);
	}
}


namespace manipulators
{

template<ip_flags flags,::fast_io::details::iptypesimpl iptype>
inline constexpr ip_manip_t<flags,::std::remove_cvref_t<iptype>> ip_generic(iptype ipaddr) noexcept
{
	return {ipaddr};
}

}

#if 0
enum class scan_ipv4_context_phase : ::std::uint_least8_t
{
	addr0,
	dot0,
	addr1,
	dot1,
	addr2,
	dot2,
	addr3
};

enum class scan_ipv6_context_phase : ::std::uint_least8_t
{
	nothing,
};

enum class scan_integral_context_phase : ::std::uint_least8_t;
template <typename phase_t, ::std::integral char_type>
struct ip_scan_state_t
{
	// TODO: to find out why +1 is needed here
	static inline constexpr auto max_size{ ::fast_io::details::print_integer_reserved_size_cache<10, false, false, ::std::uint_least16_t> + 1 };
	::fast_io::freestanding::array<char_type, max_size> buffer;
	scan_integral_context_phase integer_phase{};
	::std::uint_least8_t size{};
	phase_t ip_phase{};
	static inline constexpr ::std::uint_least8_t port_mark = -1;
	static inline constexpr ::std::uint_least8_t port = -2;
};
template <::std::integral char_type>
using ipv4_scan_state_t = ip_scan_state_t<scan_ipv4_context_phase, char_type>;
template <::std::integral char_type>
using ipv6_scan_state_t = ip_scan_state_t<scan_ipv6_context_phase, char_type>;
template <::std::integral char_type>
using ip_port_scan_state_t = ip_scan_state_t<::std::uint_least8_t, char_type>;

namespace details {

template <::std::integral char_type>
inline constexpr parse_result<char_type const*> scn_cnt_define_inaddr_impl(char_type const* begin, char_type const* end, posix_in_addr& t) noexcept
{
	if (end - begin < 7) [[unlikely]]
		return { begin, parse_code::invalid };
	auto [itr0, ec0] = scan_int_contiguous_define_impl<10, true, false, false>(begin, begin + 3, t.address[0]);
	if (ec0 != parse_code::ok) [[unlikely]]
		return { itr0, ec0 };
	if constexpr (sizeof(decltype(t.address[0])) != 1)
	{
		if (t.address[0] >= 256u) [[unlikely]]
			return { itr0, parse_code::overflow };
	}
	begin = itr0;
	if (begin == end || *begin != char_literal_v<u8'.', char_type>) [[unlikely]]
		return { begin, parse_code::invalid };
	++begin;
	auto [itr1, ec1] = scan_int_contiguous_define_impl<10, true, false, false>(begin, begin + 3, t.address[1]);
	if (ec1 != parse_code::ok) [[unlikely]]
		return { itr1, ec1 };
	if constexpr (sizeof(decltype(t.address[1])) != 1)
	{
		if (t.address[1] >= 256u) [[unlikely]]
			return { itr1, parse_code::overflow };
	}
	begin = itr1;
	if (begin == end || *begin != char_literal_v<u8'.', char_type>) [[unlikely]]
		return { begin, parse_code::invalid };
	++begin;
	if (end - begin < 3) [[unlikely]]
		return { begin, parse_code::invalid };
	auto [itr2, ec2] = scan_int_contiguous_define_impl<10, true, false, false>(begin, begin + 3, t.address[2]);
	if (ec2 != parse_code::ok) [[unlikely]]
		return { itr2, ec2 };
	if constexpr (sizeof(decltype(t.address[2])) != 1)
	{
		if (t.address[2] >= 256u) [[unlikely]]
			return { itr2, parse_code::overflow };
	}
	begin = itr2;
	if (begin == end || *begin != char_literal_v<u8'.', char_type>) [[unlikely]]
		return { begin, parse_code::invalid };
	++begin;
	auto [itr3, ec3] = scan_int_contiguous_define_impl<10, true, false, false>(begin, end, t.address[3]);
	if (ec3 != parse_code::ok) [[unlikely]]
		return { itr3, ec3 };
	if constexpr (sizeof(decltype(t.address[3])) != 1)
	{
		if (t.address[3] >= 256u) [[unlikely]]
			return { itr3, parse_code::overflow };
	}
	begin = itr3;
	return { begin, parse_code::ok };
}

template <::std::integral char_type>
inline constexpr parse_result<char_type const*> scn_ctx_define_inaddr_impl(ipv4_scan_state_t<char_type>& state, char_type const* begin, char_type const* end, posix_in_addr& t) noexcept
{
	switch (state.ip_phase)
	{
	case scan_ipv4_context_phase::addr0:
	{
		if (begin == end)
			return { begin, parse_code::partial };
		auto [itr, ec] = scan_context_define_parse_impl<10, false, false, false>(state, begin, end, t.address[0]);
		if (ec != parse_code::ok) [[unlikely]]
			return { itr, ec };
		if constexpr (sizeof(decltype(t.address[0])) != 1)
		{
			if (t.address[0] >= 256) [[unlikely]]
				return { itr, parse_code::overflow };
		}
		state.size = 0;
		state.integer_phase = scan_integral_context_phase::zero;
		begin = itr;
		state.ip_phase = scan_ipv4_context_phase::dot0;
		[[fallthrough]];
	}
	case scan_ipv4_context_phase::dot0:
	{
		if (begin == end)
			return { begin, parse_code::partial };
		if (*begin != char_literal_v<u8'.', char_type>) [[unlikely]]
			return { begin, parse_code::invalid };
		++begin;
		state.ip_phase = scan_ipv4_context_phase::addr1;
		[[fallthrough]];
	}
	case scan_ipv4_context_phase::addr1:
	{
		if (begin == end)
			return { begin, parse_code::partial };
		auto [itr, ec] = scan_context_define_parse_impl<10, true, false, false>(state, begin, end, t.address[1]);
		if (ec != parse_code::ok) [[unlikely]]
			return { itr, ec };
		if constexpr (sizeof(decltype(t.address[1])) != 1)
		{
			if (t.address[1] >= 256) [[unlikely]]
				return { itr, parse_code::overflow };
		}
		state.size = 0;
		state.integer_phase = scan_integral_context_phase::zero;
		begin = itr;
		state.ip_phase = scan_ipv4_context_phase::dot1;
		[[fallthrough]];
	}
	case scan_ipv4_context_phase::dot1:
	{
		if (begin == end)
			return { begin, parse_code::partial };
		if (*begin != char_literal_v<u8'.', char_type>) [[unlikely]]
			return { begin, parse_code::invalid };
		++begin;
		state.ip_phase = scan_ipv4_context_phase::addr2;
		[[fallthrough]];
	}
	case scan_ipv4_context_phase::addr2:
	{
		if (begin == end)
			return { begin, parse_code::partial };
		auto [itr, ec] = scan_context_define_parse_impl<10, true, false, false>(state, begin, end, t.address[2]);
		if (ec != parse_code::ok) [[unlikely]]
			return { itr, ec };
		if constexpr (sizeof(decltype(t.address[2])) != 1)
		{
			if (t.address[2] >= 256) [[unlikely]]
				return { itr, parse_code::overflow };
		}
		state.size = 0;
		state.integer_phase = scan_integral_context_phase::zero;
		begin = itr;
		state.ip_phase = scan_ipv4_context_phase::dot2;
		[[fallthrough]];
	}
	case scan_ipv4_context_phase::dot2:
	{
		if (begin == end)
			return { begin, parse_code::partial };
		if (*begin != char_literal_v<u8'.', char_type>) [[unlikely]]
			return { begin, parse_code::invalid };
		++begin;
		state.ip_phase = scan_ipv4_context_phase::addr3;
		[[fallthrough]];
	}
	case scan_ipv4_context_phase::addr3:
	{
		if (begin == end)
			return { begin, parse_code::partial };
		auto [itr, ec] = scan_context_define_parse_impl<10, true, false, false>(state, begin, end, t.address[3]);
		if (ec != parse_code::ok) [[unlikely]]
			return { itr, ec };
		if constexpr (sizeof(decltype(t.address[3])) != 1)
		{
			if (t.address[3] >= 256) [[unlikely]]
				return { itr, parse_code::overflow };
		}
		begin = itr;
		return { begin, parse_code::ok };
	}
	}
#ifdef __has_builtin
#if __has_builtin(__builtin_unreachable)
	__builtin_unreachable();
#endif
#endif
}

template <::std::integral char_type>
inline constexpr parse_result<char_type const*> scn_cnt_define_port_impl(char_type const* begin, char_type const* end, ::std::uint_least16_t& t) noexcept
{
	if (begin == end) [[unlikely]]
		return { begin, parse_code::invalid };
	if (*begin != char_literal_v<u8':', char_type>) [[unlikely]]
		return { begin, parse_code::invalid };
	++begin;
	return scan_int_contiguous_define_impl<10, true, false, false>(begin, end, t);
}

template <::std::integral char_type>
inline constexpr parse_result<char_type const*> scn_ctx_define_port_impl(ip_port_scan_state_t<char_type>& state, char_type const* begin, char_type const* end, ::std::uint_least16_t& t) noexcept
{
	switch (state.ip_phase)
	{
	case state.port_mark:
		if (begin == end)
			return { begin, parse_code::partial };
		if (*begin != char_literal_v<u8':', char_type>) [[unlikely]]
			return { begin, parse_code::invalid };
		++begin;
		state.ip_phase = state.port;
		[[fallthrough]];
	case state.port:
		if (begin == end)
			return { begin, parse_code::partial };
		return scan_context_define_parse_impl<10, true, false, false>(state, begin, end, t);
	default:;
#ifdef __has_builtin
#if __has_builtin(__builtin_unreachable)
		__builtin_unreachable();
#endif
#endif
	}
}

template <typename state_t, ::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr parse_result<char_type const*> scn_ctx_define_port_type_general_impl(state_t& state, char_type const* begin, char_type const* end, ::std::uint_least16_t& t) noexcept
{
	auto new_state{ ::std::bit_cast<ip_port_scan_state_t<char_type>>(state) };
	auto result{ scn_ctx_define_port_impl<char_type>(new_state, begin, end, t) };
	state = ::std::bit_cast<state_t>(new_state);
	return result;
}

template <bool v6shorten, bool v6uppercase, bool v6bracket, bool v6full, ::std::integral char_type>
inline constexpr parse_result<char_type const*> scn_cnt_define_in6addr_impl(char_type const* begin, char_type const* end, posix_in6_addr& t) noexcept
{
}

}

namespace manipulators
{

inline constexpr ip_flags ip_default_scan_flags{.v6uppercase=true,.showport=true};
inline constexpr ip_flags ip_default_scan_inaddr_flags{.v6uppercase=true};

template<ip_flags flags, ::fast_io::details::iptypesimpl iptype>
inline constexpr ip_manip_t<flags, ::std::remove_cvref_t<iptype>*> ip_scan_generic(iptype& ipaddr) noexcept
{
	return { __builtin_addressof(ipaddr) };
}

}

template<::fast_io::details::iptypesimpl iptype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::manipulators::ip_manip_t<
	::fast_io::details::inaddrimpl<iptype> ?
	::fast_io::manipulators::ip_default_scan_inaddr_flags
	:
	::fast_io::manipulators::ip_default_scan_flags, ::std::remove_cvref_t<iptype>*> scan_alias_define(io_alias_t, iptype& t) noexcept
{
	return { __builtin_addressof(t) };
}

template<::std::integral char_type, ::fast_io::manipulators::ip_flags flags, ::fast_io::details::iptypesimpl iptype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr parse_result<char_type const*> scan_contiguous_define(::fast_io::io_reserve_type_t<char_type,
	::fast_io::manipulators::ip_manip_t<flags, iptype*>>, char_type const* begin, char_type const* end, ::fast_io::manipulators::ip_manip_t<flags, iptype*> val) noexcept
{
	if constexpr (::std::same_as<iptype, ::fast_io::posix_in_addr>)
	{
		static_assert(flags.showport == false, "please use fast_io::ipv4 to store a port");
		return ::fast_io::details::scn_cnt_define_inaddr_impl(begin, end, *val.reference);
	}
	else if constexpr (::std::same_as<iptype, ::fast_io::ipv4>)
	{
		auto result{ ::fast_io::details::scn_cnt_define_inaddr_impl(begin, end, val.reference->address) };
		if (result.code != parse_code::ok) [[unlikely]]
			return result;
		begin = result.iter;
		return ::fast_io::details::scn_cnt_define_port_impl(begin, end, val.reference->port);
	}
	else if constexpr (::std::same_as<iptype, ::fast_io::posix_in6_addr>)
	{
		static_assert(flags.showport == false, "please use fast_io::ipv6 to store a port");
//		return ::fast_io::details::prtrsv_inaddr6_define_impl<flags.v6shorten, flags.v6uppercase,
//			flags.showport ? true : flags.v6bracket, flags.v6full,
//			flags.showport>(iter, val.reference);
		return {};
	}
	else if constexpr (::std::same_as<iptype, ::fast_io::ipv6>)
	{
		//return ::fast_io::details::prtrsv_ipv6_define_impl<flags.v6shorten, flags.v6uppercase,
		//	flags.showport ? true : flags.v6bracket, flags.v6full,
		//	flags.showport>(iter, val.reference);
		return {};
	}
	else if constexpr (::std::same_as<iptype, ::fast_io::ip_address>)
	{
		//return ::fast_io::details::prtrsv_ip_address_define_impl<flags.v6shorten, flags.v6uppercase,
		//	flags.showport ? true : flags.v6bracket, flags.v6full,
		//	flags.showport>(iter, val.reference);
		return {};
	}
	else
	{
		//return ::fast_io::details::prtrsv_ip_define_impl<flags.v6shorten, flags.v6uppercase,
		//	flags.showport ? true : flags.v6bracket, flags.v6full,
		//	flags.showport>(iter, val.reference);
		return {};
	}
}

template<::std::integral char_type, ::fast_io::manipulators::ip_flags flags>
inline constexpr io_type_t<ipv4_scan_state_t<char_type>> scan_context_type(io_reserve_type_t<char_type, ::fast_io::manipulators::ip_manip_t<flags, posix_in_addr*>>) noexcept
{
	static_assert(flags.showport == false, "please use fast_io::ipv4 to store a port");
	return {};
}

template<::std::integral char_type, ::fast_io::manipulators::ip_flags flags>
inline constexpr parse_result<char_type const*> scan_context_define(::fast_io::io_reserve_type_t<char_type,
	::fast_io::manipulators::ip_manip_t<flags, posix_in_addr*>>, ipv4_scan_state_t<char_type>& state, char_type const* begin, char_type const* end, ::fast_io::manipulators::ip_manip_t<flags, posix_in_addr*> t) noexcept
{
	return details::scn_ctx_define_inaddr_impl(state, begin, end, *t.reference);
}

template<::std::integral char_type, ::fast_io::manipulators::ip_flags flags>
inline constexpr parse_code scan_context_eof_define(::fast_io::io_reserve_type_t<char_type,
	::fast_io::manipulators::ip_manip_t<flags, posix_in_addr*>>, ipv4_scan_state_t<char_type>& state, ::fast_io::manipulators::ip_manip_t<flags, posix_in_addr*> t) noexcept
{
	if (state.ipv4_phase != scan_ipv4_context_phase::addr3)
		return parse_code::end_of_file;
	else
		return details::scan_int_contiguous_none_space_part_define_impl<10>(state.buffer.data(), state.buffer.data() + state.size, t.reference->address[3]).code;
}

template<::std::integral char_type, ::fast_io::manipulators::ip_flags flags>
inline constexpr io_type_t<ipv4_scan_state_t<char_type>> scan_context_type(io_reserve_type_t<char_type, ::fast_io::manipulators::ip_manip_t<flags, ipv4*>>) noexcept
{
	return {};
}

template<::std::integral char_type, ::fast_io::manipulators::ip_flags flags>
inline constexpr parse_result<char_type const*> scan_context_define(::fast_io::io_reserve_type_t<char_type,
	::fast_io::manipulators::ip_manip_t<flags, ipv4*>>, ipv4_scan_state_t<char_type>& state, char_type const* begin, char_type const* end, ::fast_io::manipulators::ip_manip_t<flags, ipv4*> t) noexcept
{
	switch (state.ip_phase)
	{
	case scan_ipv4_context_phase::addr0:
	case scan_ipv4_context_phase::dot0:
	case scan_ipv4_context_phase::addr1:
	case scan_ipv4_context_phase::dot1:
	case scan_ipv4_context_phase::addr2:
	case scan_ipv4_context_phase::dot2:
	case scan_ipv4_context_phase::addr3:
	{
		auto result{ details::scn_ctx_define_inaddr_impl(state, begin, end, t.reference->address) };
		if (result.code != parse_code::ok)
			return result;
		begin = result.iter;
		state.integer_phase = scan_integral_context_phase::zero;
		state.size = 0;
		state.ip_phase = ::std::bit_cast<scan_ipv4_context_phase>(state.port_mark);
		[[fallthrough]];
	}
	// clang bug for std::bit_cast is not a constexpr expression
#if !defined(__clang__)
	case ::std::bit_cast<scan_ipv4_context_phase>(state.port_mark):
	case ::std::bit_cast<scan_ipv4_context_phase>(state.port):
		return { details::scn_ctx_define_port_type_general_impl(state, begin, end, t.reference->port) };
	default:;
#else
	default:
		if (state.ip_phase == ::std::bit_cast<scan_ipv4_context_phase>(state.port_mark) ||
			state.ip_phase == ::std::bit_cast<scan_ipv4_context_phase>(state.port)) [[likely]]
			return { details::scn_ctx_define_port_type_general_impl(state, begin, end, t.reference->port) };
#endif
#ifdef __has_builtin
#if __has_builtin(__builtin_unreachable)
		__builtin_unreachable();
#endif
#endif
	}
}

template<::std::integral char_type, ::fast_io::manipulators::ip_flags flags>
inline constexpr parse_code scan_context_eof_define(::fast_io::io_reserve_type_t<char_type,
	::fast_io::manipulators::ip_manip_t<flags, ipv4*>>, ipv4_scan_state_t<char_type>& state, ::fast_io::manipulators::ip_manip_t<flags, ipv4*> t) noexcept
{
	if (state.ip_phase != ::std::bit_cast<scan_ipv4_context_phase>(state.port))
		return parse_code::end_of_file;
	else
		return details::scan_int_contiguous_none_space_part_define_impl<10>(state.buffer.data(), state.buffer.data() + state.size, t.reference->port).code;
}


#endif

}
