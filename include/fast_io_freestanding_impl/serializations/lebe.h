#pragma once

namespace fast_io::details
{

template<::std::size_t sz>
inline constexpr bool supported_lebe_size{sz==8||sz==16||sz==32||sz==64||sz==128};

#ifdef __SIZEOF_INT128__
using pesudo_int128type = __uint128_t;
#else
struct pesudo_int128type
{
	::std::uint_least64_t uarr[2];
};
#endif

template<typename T>
inline constexpr ::std::size_t cal_size_lebe_floatsize() noexcept
{
	using nocvref = ::std::remove_cvref_t<T>;
	if constexpr(::std::same_as<nocvref,float>)
	{
		return 32;
	}
	else if constexpr(::std::same_as<nocvref,double>)
	{
		return 64;
	}
	else if constexpr(::std::same_as<nocvref,long double>)
	{
		return 128;
	}
	else if constexpr(sizeof(nocvref)==sizeof(::std::uint_least8_t))
	{
		return 8;
	}
	else if constexpr(sizeof(nocvref)==sizeof(::std::uint_least16_t))
	{
		return 16;
	}
	else if constexpr(sizeof(nocvref)==sizeof(::std::uint_least32_t))
	{
		return 32;
	}
	else if constexpr(sizeof(nocvref)==sizeof(::std::uint_least64_t))
	{
		return 64;
	}
	else if constexpr(sizeof(nocvref)==sizeof(pesudo_int128type))
	{
		return 128;
	}
	else
	{
		return 0;
	}
}

template<typename floattype>
inline constexpr ::std::size_t size_lebe_float_size{::fast_io::details::cal_size_lebe_floatsize<floattype>()};

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
		::std::conditional_t<(sz==128),::fast_io::details::pesudo_int128type,
		::std::conditional_t<(sz==64),::std::uint_least64_t,
		::std::conditional_t<(sz==32),::std::uint_least32_t,
		::std::conditional_t<(sz==16),::std::uint_least16_t,
		::std::conditional_t<(sz==8),::std::uint_least8_t,void>>>>>;
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

template<::std::endian en,::std::floating_point T>
inline constexpr auto iec559_lebe_put(T t)
{
	constexpr
		bool isiec559{::std::numeric_limits<T>::is_iec559};
	using nocvref = ::std::remove_cvref_t<T>;

	if constexpr(::std::same_as<nocvref,float>)
	{
		if constexpr(isiec559)
		{
			return ::fast_io::manipulators::lebe_put<en,32>(::std::bit_cast<::std::uint_least32_t>(static_cast<float>(t)));
		}
		else
		{
#if __STDCPP_FLOAT32_T__
			return ::fast_io::manipulators::lebe_put<en,32>(::std::bit_cast<::std::uint_least32_t>(static_cast<_Float32>(t)));
#else
static_assert(isiec559,"float is not iec60559");
#endif
		}
	}
	else if constexpr(::std::same_as<nocvref,double>)
	{
		if constexpr(isiec559)
		{
			return ::fast_io::manipulators::lebe_put<en,64>(::std::bit_cast<::std::uint_least64_t>(static_cast<double>(t)));
		}
		else
		{
#if __STDCPP_FLOAT64_T__
			return ::fast_io::manipulators::lebe_put<en,64>(::std::bit_cast<::std::uint_least64_t>(static_cast<_Float64>(t)));
#else
static_assert(isiec559,"double is not iec60559");
#endif
		}
	}
	else if constexpr(::std::same_as<nocvref,long double>)
	{
#if __STDCPP_FLOAT128_T__
		using proxy_type = ::fast_io::details::pesudo_int128type;
		return ::fast_io::manipulators::basic_lebe_get_put<en,basic_lebe_put_integral<128,proxy_type>>{{::std::bit_cast<::fast_io::details::pesudo_int128type>(static_cast<_Float128>(t))}};
#else
static_assert(sizeof(double)!=sizeof(long)&&isiec559,"long double is not iec60559 or sizeof(double) == sizeof(long double)");
#endif
	}
	else if constexpr(sizeof(T)==sizeof(::std::uint_least8_t))
	{
		return ::fast_io::manipulators::lebe_put<en,8>(::std::bit_cast<::std::uint_least8_t>(static_cast<nocvref>(t)));
	}
	else if constexpr(sizeof(T)==sizeof(::std::uint_least16_t))
	{
		return ::fast_io::manipulators::lebe_put<en,16>(::std::bit_cast<::std::uint_least16_t>(static_cast<nocvref>(t)));
	}
	else if constexpr(sizeof(T)==sizeof(::std::uint_least32_t))
	{
		return ::fast_io::manipulators::lebe_put<en,32>(::std::bit_cast<::std::uint_least32_t>(static_cast<nocvref>(t)));
	}
	else if constexpr(sizeof(T)==sizeof(::std::uint_least64_t))
	{
		return ::fast_io::manipulators::lebe_put<en,64>(::std::bit_cast<::std::uint_least64_t>(static_cast<nocvref>(t)));
	}
	else if constexpr(sizeof(T)==sizeof(::fast_io::details::pesudo_int128type))
	{
		using proxy_type = ::fast_io::details::pesudo_int128type;
		return ::fast_io::manipulators::basic_lebe_get_put<en,basic_lebe_put_integral<128,proxy_type>>{{::std::bit_cast<::fast_io::details::pesudo_int128type>(static_cast<nocvref>(t))}};
	}
}

/*
WASM standard uses this format.
*/
template<::std::floating_point T>
inline constexpr auto iec559_le_put(T t)
{
	return ::fast_io::manipulators::iec559_lebe_put<::std::endian::little>(t);
}

template<::std::floating_point T>
inline constexpr auto iec559_be_put(T t)
{
	return ::fast_io::manipulators::iec559_lebe_put<::std::endian::big>(t);
}

template<::std::endian en,::std::floating_point T>
requires (!::std::is_const_v<T>)
inline constexpr auto iec559_lebe_get(T& t) noexcept
{
	using nocvreft = ::std::remove_cvref_t<T>;
	return ::fast_io::manipulators::basic_lebe_get_put<en,
		::fast_io::manipulators::basic_lebe_get_integral<::fast_io::details::cal_size_lebe_floatsize<nocvreft>(),T>>{{__builtin_addressof(t)}};
}

template<::std::floating_point T>
inline constexpr auto iec559_le_get(T& t)
{
	return ::fast_io::manipulators::iec559_lebe_get<::std::endian::little>(t);
}

template<::std::floating_point T>
inline constexpr auto iec559_be_get(T& t)
{
	return ::fast_io::manipulators::iec559_lebe_get<::std::endian::big>(t);
}

}

namespace fast_io
{

namespace details
{

template<typename int_type>
struct parse_lebeget_int_result
{
	bool failed;
	int_type itp;
};

template<::std::endian endn,typename int_type,::std::integral char_type>
requires (1<sizeof(char_type))
inline
#if __cpp_lib_bit_cast >= 201806L && (__cpp_if_consteval >= 202106L ||__cpp_lib_is_constant_evaluated >= 201811L)
constexpr
#endif
parse_lebeget_int_result<int_type> scan_precise_reserve_define_integer_common_largechartype_impl(char_type const* iter) noexcept
{
	constexpr std::size_t n{sizeof(int_type)};
	static_assert(n!=0);
	int_type t;

	using unsigned_char_type = ::std::make_unsigned_t<char_type>;
	constexpr
		unsigned_char_type mx{static_cast<unsigned_char_type>(::std::numeric_limits<char unsigned>::max())};

	char unsigned buffer[n];
	for(auto bit{buffer},bie{buffer+n};bit!=bie;++bit)
	{
		auto val{static_cast<unsigned_char_type>(*iter)};
		if(mx < val)
		{
			return {true,t};
		}
		*bit=static_cast<char unsigned>(val);
		++iter;
	}
#if __cpp_lib_bit_cast >= 201806L
	t=::std::bit_cast<int_type>(buffer);
#elif !defined(__has_builtin)
	::std::memcpy(__builtin_addressof(t),buffer,n);
#elif __has_builtin(__builtin_memcpy)
	__builtin_memcpy(__builtin_addressof(t),buffer,n);
#else
	::std::memcpy(__builtin_addressof(t),buffer,n);
#endif
	if constexpr(::std::endian::native!=endn)
	{
		if constexpr(::fast_io::details::my_unsigned_integral<int_type>)
		{
			t=::fast_io::byte_swap(t);
		}
		else
		{
			::std::uint_least64_t low{t.low};
			::std::uint_least64_t high{t.high};
			t.low=::fast_io::byte_swap(high);
			t.high=::fast_io::byte_swap(low);
		}
	}
	return {false,t};
}

template<::std::endian endn,typename int_type,::std::integral char_type>
requires (sizeof(char_type)==1)
inline
#if __cpp_lib_bit_cast >= 201806L && (__cpp_if_consteval >= 202106L ||__cpp_lib_is_constant_evaluated >= 201811L)
constexpr
#endif
auto scan_precise_reserve_define_integer_common_impl(char_type const* iter) noexcept
{
	constexpr std::size_t n{sizeof(int_type)};
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
		if constexpr(::fast_io::details::my_unsigned_integral<int_type>)
		{
			t=::fast_io::byte_swap(t);
		}
		else
		{
			::std::uint_least64_t low{t.low};
			::std::uint_least64_t high{t.high};
			t.low=::fast_io::byte_swap(high);
			t.high=::fast_io::byte_swap(low);
		}
	}
	return t;
}

template<::std::endian endn,std::size_t n,::fast_io::details::my_integral int_type,::std::integral char_type>
inline constexpr ::std::conditional_t<(1<sizeof(char_type)),::fast_io::parse_code,void> scan_precise_reserve_define_integer_impl(char_type const* iter,int_type& t) noexcept
{
	static_assert(n<=128);
	using proxy_type =
		::std::conditional_t<(n==128),::fast_io::details::pesudo_int128type,
		::std::conditional_t<(n==64),::std::uint_least64_t,
		::std::conditional_t<(n==32),::std::uint_least32_t,
		::std::conditional_t<(n==16),::std::uint_least16_t,
		::std::conditional_t<(n==8),::std::uint_least8_t,void>>>>>;
	proxy_type temp;
	if constexpr(1<sizeof(char_type))
	{
		auto [cd,tp] = scan_precise_reserve_define_integer_common_largechartype_impl<endn,proxy_type>(iter);
		if(cd)
		{
			return parse_code::invalid;
		}
		temp = tp;
	}
	else
	{
		temp = scan_precise_reserve_define_integer_common_impl<endn,proxy_type>(iter);
	}
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
	if constexpr(1<sizeof(char_type))
	{
		return parse_code::ok;
	}
}

template<::std::endian endn,::std::floating_point flttypef,::std::integral char_type>
inline constexpr ::std::conditional_t<(1<sizeof(char_type)),::fast_io::parse_code,void> scan_precise_reserve_define_lebe_float_get_impl(char_type const* iter,flttypef& t) noexcept
{
	using flttype =
		::std::remove_cvref_t<flttypef>;
	constexpr
		::std::size_t n{::fast_io::details::size_lebe_float_size<flttype>};
	using proxy_type =
		::std::conditional_t<(n==128),::fast_io::details::pesudo_int128type,
		::std::conditional_t<(n==64),::std::uint_least64_t,
		::std::conditional_t<(n==32),::std::uint_least32_t,
		::std::conditional_t<(n==16),::std::uint_least16_t,
		::std::conditional_t<(n==8),::std::uint_least8_t,void>>>>>;
	proxy_type temp;
	if constexpr(1<sizeof(char_type))
	{
		auto [cd,tp] = scan_precise_reserve_define_integer_common_largechartype_impl<endn,proxy_type>(iter);
		if(cd)
		{
			return parse_code::invalid;
		}
		temp = tp;
	}
	else
	{
		temp = scan_precise_reserve_define_integer_common_impl<endn,proxy_type>(iter);
	}
	constexpr
		bool isiec559{::std::numeric_limits<flttype>::is_iec559};
	if constexpr(::std::same_as<flttype,float>)
	{
		if constexpr(isiec559)
		{
			t=static_cast<flttypef>(std::bit_cast<float>(temp));
		}
		else
		{
#if __STDCPP_FLOAT32_T__
			t=static_cast<flttypef>(std::bit_cast<_Float32>(temp));
#else
static_assert(isiec559,"float is not iec60559");
#endif
		}
	}
	else if constexpr(::std::same_as<flttype,double>)
	{
		if constexpr(isiec559)
		{
			t=static_cast<flttypef>(std::bit_cast<double>(temp));
		}
		else
		{
#if __STDCPP_FLOAT64_T__
			t=static_cast<flttypef>(std::bit_cast<_Float64>(temp));
#else
static_assert(isiec559,"double is not iec60559");
#endif
		}
	}
	else if constexpr(::std::same_as<flttype,long double>)
	{
#if __STDCPP_FLOAT128_T__
			t=static_cast<flttypef>(std::bit_cast<_Float128>(temp));
#else
static_assert(sizeof(double)!=sizeof(long)&&isiec559,"long double is not iec60559 or sizeof(double) == sizeof(long double)");
#endif
	}
	else
	{
		t=static_cast<flttypef>(std::bit_cast<flttype>(temp));
	}
	if constexpr(1<sizeof(char_type))
	{
		return parse_code::ok;
	}
}

template<::std::endian endn,typename uint_type,::std::integral char_type>
inline
#if __cpp_lib_bit_cast >= 201806L && (__cpp_if_consteval >= 202106L ||__cpp_lib_is_constant_evaluated >= 201811L)
constexpr
#endif
char_type* print_reserve_define_integer_lebe_common_impl(char_type* iter,uint_type u) noexcept
{
	constexpr std::size_t n{sizeof(uint_type)};
	static_assert(n!=0);
	if constexpr(::std::endian::native!=endn)
	{
		if constexpr(::fast_io::details::my_unsigned_integral<uint_type>)
		{
			u=::fast_io::byte_swap(u);
		}
		else
		{
			::std::uint_least64_t low{u.low};
			::std::uint_least64_t high{u.high};
			u.low=::fast_io::byte_swap(high);
			u.high=::fast_io::byte_swap(low);
		}
	}
#if __cpp_lib_bit_cast >= 201806L && (__cpp_if_consteval >= 202106L ||__cpp_lib_is_constant_evaluated >= 201811L)
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L
	if (__builtin_is_constant_evaluated())
#endif
	{
		::fast_io::freestanding::array<char unsigned,n> buffer{::std::bit_cast<::fast_io::freestanding::array<char unsigned,n>>(u)};
		non_overlapped_copy_n(buffer.data(),n,iter);
	}
	else
#endif
	{
		if constexpr(1<sizeof(uint_type))
		{
			auto buffer{::std::bit_cast<::fast_io::freestanding::array<char unsigned,n>>(u)};
			non_overlapped_copy_n(buffer.data(),n,iter);
		}
		else
		{
#if !defined(__has_builtin)
		::std::memcpy(iter,__builtin_addressof(u),n);
#elif __has_builtin(__builtin_memcpy)
		__builtin_memcpy(iter,__builtin_addressof(u),n);
#else
		::std::memcpy(iter,__builtin_addressof(u),n);
#endif
		}
	}
	return iter+n;
}


}

template<::std::integral char_type,::std::endian end,::std::size_t sz,typename int_type>
requires (((sz%(::std::numeric_limits<char unsigned>::digits))==0)&&!::std::is_const_v<int_type>
	&&(::fast_io::details::my_integral<int_type>||
	(::std::floating_point<int_type>&&(::fast_io::details::size_lebe_float_size<int_type>)==sz)))
inline constexpr std::size_t scan_precise_reserve_size(io_reserve_type_t<char_type,
	::fast_io::manipulators::basic_lebe_get_put<end,::fast_io::manipulators::basic_lebe_get_integral<sz,int_type>>>) noexcept
{
	constexpr std::size_t cache{sz/(::std::numeric_limits<char unsigned>::digits)};
	return cache;
}

template<::std::endian en,::std::size_t sz,::std::integral char_type,typename int_type>
requires (((sz%(::std::numeric_limits<char unsigned>::digits))==0)&&!::std::is_const_v<int_type>
	&&(::fast_io::details::my_integral<int_type>||
	(::std::floating_point<int_type>&&(::fast_io::details::size_lebe_float_size<int_type>)==sz)))
inline constexpr auto scan_precise_reserve_define(io_reserve_type_t<char_type,
	::fast_io::manipulators::basic_lebe_get_put<en,::fast_io::manipulators::basic_lebe_get_integral<sz,int_type>>>,
	char_type const* iter,
	::fast_io::manipulators::basic_lebe_get_put<en,::fast_io::manipulators::basic_lebe_get_integral<sz,int_type>> t) noexcept
{
	if constexpr(::std::floating_point<int_type>)
	{
		return ::fast_io::details::scan_precise_reserve_define_lebe_float_get_impl<en>(iter,*t.reference.pointer);
	}
	else
	{
		return ::fast_io::details::scan_precise_reserve_define_integer_impl<en,sz>(iter,*t.reference.pointer);
	}
}

template<::std::integral char_type,::std::endian end,::std::size_t sz,typename int_type>
requires (((sz%(::std::numeric_limits<char unsigned>::digits))==0)&&
	(::fast_io::details::my_unsigned_integral<int_type>||::std::same_as<int_type,::fast_io::details::pesudo_int128type>))
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,
	::fast_io::manipulators::basic_lebe_get_put<end,::fast_io::manipulators::basic_lebe_put_integral<sz,int_type>>>) noexcept
{
	constexpr std::size_t cache{sz/(::std::numeric_limits<char unsigned>::digits)};
	return cache;
}

template<::std::integral char_type,::std::endian end,::std::size_t sz,typename int_type>
requires (((sz%(::std::numeric_limits<char unsigned>::digits))==0)&&
	(::fast_io::details::my_unsigned_integral<int_type>||::std::same_as<int_type,::fast_io::details::pesudo_int128type>))
inline constexpr auto print_reserve_define(io_reserve_type_t<char_type,
	::fast_io::manipulators::basic_lebe_get_put<end,::fast_io::manipulators::basic_lebe_put_integral<sz,int_type>>>,char_type* iter,
	::fast_io::manipulators::basic_lebe_get_put<end,::fast_io::manipulators::basic_lebe_put_integral<sz,int_type>> v) noexcept
{
	return ::fast_io::details::print_reserve_define_integer_lebe_common_impl<end>(iter,v.reference.value);
}

}
