#pragma once
namespace fast_io::details
{

template<std::size_t sz>
inline constexpr bool supported_lebe_size{sz==8||sz==16||sz==32||sz==64};

}
namespace fast_io::manipulators
{

template<std::size_t sz,typename value_type>
struct basic_lebe_get_integral
{
	value_type* pointer;
};

template<std::size_t sz,typename value_type>
struct basic_lebe_put_integral
{
	using manip_tag = manip_tag_t;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
	[[no_unique_address]]
#endif
#endif
	value_type value;
};


template<::std::endian end,typename value_type>
struct basic_lebe_get_put
{
	using manip_tag = manip_tag_t;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
	[[no_unique_address]]
#endif
#endif
	value_type reference;
};

template<::std::endian en,::std::size_t sz,::fast_io::details::my_integral int_type>
requires (!::std::is_const_v<int_type>&&::fast_io::details::supported_lebe_size<sz>)
inline constexpr auto lebe_get(int_type& t) noexcept
{
	return basic_lebe_get_put<en,basic_lebe_get_integral<sz,int_type>>{{__builtin_addressof(t)}};
}

template<::std::size_t sz,::fast_io::details::my_integral int_type>
requires (!::std::is_const_v<int_type>&&::fast_io::details::supported_lebe_size<sz>)
inline constexpr auto le_get(int_type& t) noexcept
{
	return basic_lebe_get_put<::std::endian::little,basic_lebe_get_integral<sz,int_type>>{{__builtin_addressof(t)}};
}

template<::std::size_t sz,::fast_io::details::my_integral int_type>
requires (!::std::is_const_v<int_type>&&::fast_io::details::supported_lebe_size<sz>)
inline constexpr auto be_get(int_type& t) noexcept
{
	return basic_lebe_get_put<::std::endian::big,basic_lebe_get_integral<sz,int_type>>{{__builtin_addressof(t)}};
}

template<::std::endian en,::std::size_t sz,::fast_io::details::my_integral int_type>
requires ::fast_io::details::supported_lebe_size<sz>
inline constexpr auto lebe_put(int_type t) noexcept((::std::numeric_limits<::fast_io::details::my_make_unsigned_t<::std::remove_cvref_t<int_type>>>::digits)<=sz)
{
	using uint_type = ::fast_io::details::my_make_unsigned_t<::std::remove_cvref_t<int_type>>;
	uint_type u{static_cast<uint_type>(t)};
	if constexpr(sz<(::std::numeric_limits<uint_type>::digits))
	{
		constexpr uint_type mx_value_halfm1{static_cast<uint_type>((static_cast<int_type>(1) << static_cast<int_type>(sz-1)))};
		constexpr uint_type mx_value{static_cast<uint_type>(static_cast<uint_type>(mx_value_halfm1-static_cast<uint_type>(1))+mx_value_halfm1)};
		if(u>mx_value)
			throw_parse_code(::fast_io::parse_code::invalid);
	}
	using proxy_type =
		::std::conditional_t<(sz==64),::std::uint_least64_t,
		::std::conditional_t<(sz==32),::std::uint_least32_t,
		::std::conditional_t<(sz==16),::std::uint_least16_t,
		::std::conditional_t<(sz==8),::std::uint_least8_t,void>>>>;
	return basic_lebe_get_put<en,basic_lebe_put_integral<sz,proxy_type>>{{static_cast<proxy_type>(static_cast<uint_type>(u))}};
}

template<::std::size_t sz,::fast_io::details::my_integral int_type>
requires ::fast_io::details::supported_lebe_size<sz>
inline constexpr auto le_put(int_type t) noexcept(noexcept(lebe_put<::std::endian::little,sz>(t)))
{
	return lebe_put<::std::endian::little,sz>(t);
}

template<::std::size_t sz,::fast_io::details::my_integral int_type>
requires ::fast_io::details::supported_lebe_size<sz>
inline constexpr auto be_put(int_type t) noexcept(noexcept(lebe_put<::std::endian::big,sz>(t)))
{
	return lebe_put<::std::endian::big,sz>(t);
}

}

namespace fast_io
{

namespace details
{

template<::std::endian endn,::fast_io::details::my_unsigned_integral int_type,::std::integral char_type>
inline
#if __cpp_lib_bit_cast >= 201806L && (__cpp_if_consteval >= 202106L ||__cpp_lib_is_constant_evaluated >= 201811L)
constexpr
#endif
auto scan_precise_reserve_define_integer_common_impl(char_type const* iter) noexcept
{
	constexpr std::size_t n{sizeof(int_type)/sizeof(char_type)};
	static_assert(n!=0);
	int_type t;
#if __cpp_lib_bit_cast >= 201806L && (__cpp_if_consteval >= 202106L ||__cpp_lib_is_constant_evaluated >= 201811L)
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L
	if (__builtin_is_constant_evaluated())
#endif
	{
		char_type buffer[n];
		non_overlapped_copy_n(iter,n,buffer);
		t=::std::bit_cast<int_type>(buffer);
	}
	else
#endif
	{
#if !defined(__has_builtin)
		::std::memcpy(__builtin_addressof(t),iter,sizeof(int_type));
#elif __has_builtin(__builtin_memcpy)
		__builtin_memcpy(__builtin_addressof(t),iter,sizeof(int_type));
#else
		::std::memcpy(__builtin_addressof(t),iter,sizeof(int_type));
#endif
	}
	if constexpr(::std::endian::native!=endn)
	{
		t=::fast_io::byte_swap(t);
	}
	return t;
}

template<::std::endian endn,std::size_t n,::fast_io::details::my_integral int_type,::std::integral char_type>
inline constexpr auto scan_precise_reserve_define_integer_impl(char_type const* iter,int_type& t) noexcept
{
	static_assert(n<=64);
	using proxy_type =
		::std::conditional_t<(n==64),::std::uint_least64_t,
		::std::conditional_t<(n==32),::std::uint_least32_t,
		::std::conditional_t<(n==16),::std::uint_least16_t,
		::std::conditional_t<(n==8),::std::uint_least8_t,void>>>>;
	proxy_type const temp{scan_precise_reserve_define_integer_common_impl<endn,proxy_type>(iter)};
	using my_unsigned_type = ::fast_io::details::my_make_unsigned_t<int_type>;
	constexpr std::size_t digits{::std::numeric_limits<my_unsigned_type>::digits};
	if constexpr(digits<n)
	{
		constexpr proxy_type mx_value{::std::numeric_limits<my_unsigned_type>::max()};
		if(mx_value<temp)
		{
			return parse_code::invalid;			
		}
		t=static_cast<int_type>(static_cast<my_unsigned_type>(temp));
		return parse_code::ok;
	}
	else
	{
		t=static_cast<int_type>(static_cast<my_unsigned_type>(temp));
	}
}

template<::std::endian endn,::fast_io::details::my_unsigned_integral uint_type,::std::integral char_type>
inline
#if __cpp_lib_bit_cast >= 201806L && (__cpp_if_consteval >= 202106L ||__cpp_lib_is_constant_evaluated >= 201811L)
constexpr
#endif
char_type* print_reserve_define_integer_common_impl(char_type* iter,uint_type u) noexcept
{
	constexpr std::size_t n{sizeof(uint_type)/sizeof(char_type)};
	static_assert(n!=0);
	if constexpr(::std::endian::native!=endn)
	{
		u=::fast_io::byte_swap(u);
	}
#if __cpp_lib_bit_cast >= 201806L && (__cpp_if_consteval >= 202106L ||__cpp_lib_is_constant_evaluated >= 201811L)
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L
	if (__builtin_is_constant_evaluated())
#endif
	{
		::fast_io::freestanding::array<char_type,n> buffer{::std::bit_cast<::fast_io::freestanding::array<char_type,n>>(u)};
		non_overlapped_copy_n(buffer.data(),n,iter);
	}
	else
#endif
	{
#if !defined(__has_builtin)
		::std::memcpy(iter,__builtin_addressof(u),sizeof(uint_type));
#elif __has_builtin(__builtin_memcpy)
		__builtin_memcpy(iter,__builtin_addressof(u),sizeof(uint_type));
#else
		::std::memcpy(iter,__builtin_addressof(u),sizeof(uint_type));
#endif
	}
	return iter+n;
}


}

template<::std::integral char_type,::std::endian end,::std::size_t sz,::fast_io::details::my_integral int_type>
requires (((sz%(::std::numeric_limits<::std::make_unsigned_t<char_type>>::digits))==0)&&!::std::is_const_v<int_type>)
inline constexpr std::size_t scan_precise_reserve_size(io_reserve_type_t<char_type,
	::fast_io::manipulators::basic_lebe_get_put<end,::fast_io::manipulators::basic_lebe_get_integral<sz,int_type>>>) noexcept
{
	constexpr std::size_t cache{sz/(::std::numeric_limits<::std::make_unsigned_t<char_type>>::digits)};
	return cache;
}

template<::std::endian en,::std::size_t sz,::std::integral char_type,::fast_io::details::my_integral int_type>
requires (((sz%(::std::numeric_limits<::std::make_unsigned_t<char_type>>::digits))==0)&&!::std::is_const_v<int_type>)
inline constexpr auto scan_precise_reserve_define(io_reserve_type_t<char_type,
	::fast_io::manipulators::basic_lebe_get_put<en,::fast_io::manipulators::basic_lebe_get_integral<sz,int_type>>>,
	char_type const* iter,
	::fast_io::manipulators::basic_lebe_get_put<en,::fast_io::manipulators::basic_lebe_get_integral<sz,int_type>> t) noexcept
{
	return ::fast_io::details::scan_precise_reserve_define_integer_impl<en,sz>(iter,*t.reference.pointer);
}

template<::std::integral char_type,::std::endian end,::std::size_t sz,::fast_io::details::my_unsigned_integral int_type>
requires (((sz%(::std::numeric_limits<::std::make_unsigned_t<char_type>>::digits))==0))
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,
	::fast_io::manipulators::basic_lebe_get_put<end,::fast_io::manipulators::basic_lebe_put_integral<sz,int_type>>>) noexcept
{
	constexpr std::size_t cache{sz/(::std::numeric_limits<::std::make_unsigned_t<char_type>>::digits)};
	return cache;
}

template<::std::integral char_type,::std::endian end,::std::size_t sz,::fast_io::details::my_unsigned_integral int_type>
requires (((sz%(::std::numeric_limits<::std::make_unsigned_t<char_type>>::digits))==0))
inline constexpr auto print_reserve_define(io_reserve_type_t<char_type,
	::fast_io::manipulators::basic_lebe_get_put<end,::fast_io::manipulators::basic_lebe_put_integral<sz,int_type>>>,char_type* iter,
	::fast_io::manipulators::basic_lebe_get_put<end,::fast_io::manipulators::basic_lebe_put_integral<sz,int_type>> v) noexcept
{
	return ::fast_io::details::print_reserve_define_integer_common_impl<end>(iter,v.reference.value);
}

}
