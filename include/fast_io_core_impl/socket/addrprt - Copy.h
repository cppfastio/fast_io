#pragma once

namespace fast_io
{

#if 0
namespace manipulators
{

template<ip_flags flags,typename iptype>
inline constexpr ip_manip_t<flags,iptype> ip_generic(iptype ipaddr) noexcept
{
	return {};
}

}
#endif

namespace details
{

template<::std::integral char_type>
inline constexpr char_type* prtrsv_inaddr_common_define_impl(char_type* it,posix_in_addr inaddr) noexcept
{
	auto j{inaddr.address};
	it=print_reserve_integral_define<10>(it,*j);
	++j;
	for(auto e{inaddr.address+4};j!=e;++j)
	{
		*it=::fast_io::char_literal_v<u8'.',char_type>;
		++it;
		it=print_reserve_integral_define<10>(it,*j);
	}
	return it;
}

template<bool shorten,bool uppercase,bool showv6bracket,::std::integral char_type>
inline constexpr char_type* prtrsv_in6addr_common_define_impl(char_type* it,posix_in6_addr inaddr) noexcept
{
	if constexpr(showv6bracket)
	{
		*it = ::fast_io::char_literal_v<u8'[',char_type>;
		++it;
		it = prtrsv_in6addr_common_define_impl<shorten,uppercase,false>(it,inaddr);
		*it = ::fast_io::char_literal_v<u8']',char_type>;
		++it;
	}
	else if constexpr(shorten)
	{
		//::std::size_t maximum_zeros{};
	}
	else
	{
		auto start{inaddr.address};
		for(auto i{start},e{start+8};i!=e;++i)
		{
			if(i!=start)
			{
				*it=::fast_io::char_literal_v<u8':',char_type>;
				++it;
			}
			it=print_reserve_integral_define<16,false,false,false,uppercase,false>(it,*i);
		}
	}
	return it;
}

template<::std::integral char_type,::std::unsigned_integral porttype>
inline constexpr char_type* prtrsv_ipport_define_impl(char_type* it,ipv4 v) noexcept
{
	*it=::fast_io::char_literal_v<u8':',char_type>;
	++it;
	return ::fast_io::details::print_reserve_integral_define<10>(it,v.port);
}

template<::std::integral char_type>
inline constexpr char_type* prtrsv_ipport_zero_define_impl(char_type* it) noexcept
{
	if constexpr(::std::same_as<char_type,char>)
	{
		return copy_string_literal(it,":0");
	}
	else if constexpr(::std::same_as<char_type,wchar_t>)
	{
		return copy_string_literal(it,L":0");
	}
	else if constexpr(::std::same_as<char_type,char16_t>)
	{
		return copy_string_literal(it,u":0");
	}
	else if constexpr(::std::same_as<char_type,char32_t>)
	{
		return copy_string_literal(it,U":0");
	}
	else
	{
		return copy_string_literal(it,u8":0");
	}
}

#if 0
template<bool shorten=true,bool uppercase=false,::std::integral char_type>
inline constexpr char_type* prtrsv_ipv6_define_impl(char_type* it,ipv6 v) noexcept
{
	return prtrsv_ipport_define_impl(prtrsv_inaddr6_define_impl<shorten,uppercase>(it,v.address),v.port);
}

#endif
template<bool showport,::std::integral char_type>
inline constexpr char_type* prtrsv_inaddr_define_impl(char_type* it,::fast_io::posix_in_addr inadd) noexcept
{
	it = prtrsv_inaddr_common_define_impl(it,inadd);
	if constexpr(showport)
	{
		it = prtrsv_ipport_zero_define_impl(it);
	}
	return it;
}

template<bool showport,::std::integral char_type>
inline constexpr char_type* prtrsv_ipv4_define_impl(char_type* it,::fast_io::ipv4 v) noexcept
{
	it = prtrsv_inaddr_common_define_impl(it,v.address);
	if constexpr(showport)
	{
		it = prtrsv_ipport_define_impl(it,v.port);
	}
	return it;
}

template<bool v6shorten,bool uppercase,bool showv6bracket,bool showport,::std::integral char_type>
inline constexpr char_type* prtrsv_inaddr6_define_impl(char_type* it,::fast_io::posix_in6_addr in6add) noexcept
{
	it = prtrsv_in6addr_common_define_impl<v6shorten,uppercase,showport?true:showv6bracket>(it,in6add);
	if constexpr(showport)
	{
		it = prtrsv_ipport_zero_define_impl(it);
	}
	return it;
}

template<bool v6shorten,bool uppercase,bool showv6bracket,bool showport,::std::integral char_type>
inline constexpr char_type* prtrsv_ipv6_define_impl(char_type* it,::fast_io::ipv6 v) noexcept
{
	it = prtrsv_inaddr6_define_impl<v6shorten,uppercase,showport?true:showv6bracket>(it,v.address);
	if constexpr(showport)
	{
		it = prtrsv_ipport_define_impl(it,v.port);
	}
	return it;
}

template<bool v6shorten,bool uppercase,bool showv6bracket,bool showport,::std::integral char_type>
inline constexpr char_type* prtrsv_ip_define_impl(char_type* it,::fast_io::ip v) noexcept
{
	if(v.isv4)
	{
		it = prtrsv_inaddr_define_impl<false>(it,v.address.v4);
	}
	else
	{
		it = prtrsv_inaddr6_define_impl<v6shorten,uppercase,showport?true:showv6bracket,false>(it,v.address.v6);
	}
	if constexpr(showport)
	{
		it = prtrsv_ipport_define_impl(it,v.port);
	}
	return it;
}

template<typename T>
concept inaddrnocvrefimpl = ::std::same_as<T,::fast_io::posix_in_addr>||
			::std::same_as<T,::fast_io::posix_in6_addr>;

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
inline constexpr ::fast_io::ip_manip_t<
	::fast_io::details::inaddrimpl<iptype>?
	::fast_io::manipulators::ip_default_inaddr_flags
	:
	::fast_io::manipulators::ip_default_flags,::std::remove_cvref_t<iptype>> print_alias_define(io_alias_t,iptype t) noexcept
{
	return {t};
}

template<::std::integral char_type,ip_flags flags,::fast_io::details::iptypesimpl iptype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::std::size_t
	print_reserve_size(::fast_io::io_reserve_type_t<char_type,
		::fast_io::ip_manip_t<flags,iptype>>) noexcept
{
	using nocvreftype = ::std::remove_cvref_t<iptype>;


	constexpr
		::std::size_t portsize{::fast_io::details::print_integer_reserved_size_cache<10,false,false,::std::uint_least16_t>};

	constexpr
		::std::size_t v4intsize{::fast_io::details::print_integer_reserved_size_cache<10,false,false,char unsigned>};

	constexpr
		::std::size_t v4totalsize{(v4size+1)*4-1};

	constexpr
		::std::size_t v4sizewithport{v4totalsize+portsize+1};


	if constexpr(::std::same_as<nocvreftype,::fast_io::posix_in_addr>||
			::std::same_as<T,::fast_io::ipv4>)
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
			::std::size_t v6totalsize{(v6size+1)*8-1};

		constexpr
			::std::size_t v6totalsizebracket{v6totalsize+2};

		constexpr
			::std::size_t v6sizewithport{v6totalsizebracket+2};

		static_assert(v4intsize<v6totalsize&&v4sizewithport<v6sizewithport);

		if constexpr(flags.showport)
		{
			return v6sizewithport;
		}
		else if constexpr(flags.showv6bracket)
		{
			return v6totalsizebracket;
		}
		else
		{
			return v6totalsize;
		}
	}
}

template<::std::integral char_type,ip_flags flags,::fast_io::details::iptypesimpl iptype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr char_type* print_reserve_define(::fast_io::io_reserve_type_t<char_type,
		::fast_io::ip_manip_t<flags,iptype>>,char_type* iter,::fast_io::ip_manip_t<flags,iptype> val) noexcept
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
		return ::fast_io::details::prtrsv_inaddr6_define_impl<flags.v6shorten,flags.uppercase,
			flags.showport?true:flags.showv6bracket,
			flags.showport>(iter,val.reference);
	}
	else if constexpr(::std::same_as<iptype,::fast_io::ipv6>)
	{
		return ::fast_io::details::prtrsv_ipv6_define_impl<flags.v6shorten,flags.uppercase,
			flags.showport?true:flags.showv6bracket,
			flags.showport>(iter,val.reference);
	}
	else
	{
		return ::fast_io::details::prtrsv_ip_define_impl<flags.v6shorten,flags.uppercase,
			flags.showport?true:flags.showv6bracket,
			flags.showport>(iter,val.reference);
	}
}

}
