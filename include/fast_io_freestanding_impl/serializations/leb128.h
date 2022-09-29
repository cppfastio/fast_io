#pragma once

namespace fast_io
{

namespace manipulators
{

template<bool issigned,std::size_t bytes>
struct basic_leb128_arbitary_carrier
{
	std::byte buffer[bytes];
};

template<typename value_type>
struct basic_leb128_get_put
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

template<typename T>
requires (::fast_io::details::my_integral<T>||::std::floating_point<T>)
inline constexpr auto leb128_put(T t) noexcept
{
	if constexpr(::fast_io::details::my_unsigned_integral<T>)
	{
		if constexpr(sizeof(T)<=sizeof(unsigned))
		{
			return ::fast_io::manipulators::basic_leb128_get_put<unsigned>{static_cast<unsigned>(t)};
		}
		else if constexpr(sizeof(T)<=sizeof(::std::size_t))
		{
			return ::fast_io::manipulators::basic_leb128_get_put<::std::size_t>{static_cast<::std::size_t>(t)};
		}
		else
		{
			return ::fast_io::manipulators::basic_leb128_get_put<std::remove_cvref_t<T>>{t};
		}
	}
	else if constexpr(::std::floating_point<T>)
	{
		using value_type =
			std::conditional_t<sizeof(T)==sizeof(::std::int_least8_t),
			::std::int_least8_t,
			std::conditional_t<sizeof(T)==sizeof(::std::int_least16_t),
			::std::int_least16_t,
			std::conditional_t<sizeof(T)==sizeof(::std::int_least32_t),
			::std::int_least32_t,
			std::conditional_t<sizeof(T)==sizeof(::std::int_least64_t),
			::std::int_least64_t,
			basic_leb128_arbitary_carrier<true,sizeof(T)>
			>>>>;
		return ::fast_io::manipulators::basic_leb128_get_put<value_type>{std::bit_cast<value_type>(t)};
	}
	else
	{
		if constexpr(sizeof(T)<=sizeof(int))
		{
			return ::fast_io::manipulators::basic_leb128_get_put<int>{static_cast<int>(t)};
		}
		else if constexpr(sizeof(T)<=sizeof(::std::ptrdiff_t))
		{
			return ::fast_io::manipulators::basic_leb128_get_put<::std::ptrdiff_t>{static_cast<::std::ptrdiff_t>(t)};
		}
		else
		{
			return ::fast_io::manipulators::basic_leb128_get_put<std::remove_cvref_t<T>>{t};
		}
	}
}

}

template<std::integral char_type,::fast_io::details::my_integral T>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,
	::fast_io::manipulators::basic_leb128_get_put<T>>) noexcept
{
	constexpr std::size_t digits{std::numeric_limits<T>::digits};
	constexpr std::size_t seven{7};
	constexpr std::size_t digitsdiv7{digits/seven};
	constexpr bool notsevenmul{(digits%seven)!=0};
	constexpr std::size_t urret{notsevenmul+digitsdiv7};
	if constexpr(::fast_io::details::my_unsigned_integral<T>)
	{
		return urret;
	}
	else
	{
		//set a fake value to determine the value later
		return urret*3;
	}
}

namespace details
{

template<std::integral char_type,::fast_io::details::my_integral T>
inline constexpr char_type* pr_rsv_leb128_impl(char_type* iter,T value) noexcept
{
	if constexpr(::fast_io::details::my_unsigned_integral<T>)
	{
		for(;;++iter)
		{
			*iter=static_cast<char_type>(value&0x7f);
			value >>= 7;
			if(!value)
			{
				*iter|=0xf0;
				return ++iter;
			}
		}
	}
	else
	{
		using unsigned_type = ::fast_io::details::my_make_unsigned_t<T>;
		constexpr unsigned_type digits{std::numeric_limits<T>::digits};
		constexpr unsigned_type seven{7};
		constexpr unsigned_type mask1{((~static_cast<unsigned_type>(1))>>7)<<7};
		unsigned_type uvalue{static_cast<unsigned_type>(value)};
		bool isnegative{value<0};
		for(;;++t)
		{
			*t=static_cast<char_type>(uvalue&0x7f);
			uvalue >>= 7;
			if(!uvalue)
			{
				*t|=0xf0;
				return ++t;
			}
		}
	}
}

}

template<std::integral char_type,::fast_io::details::my_integral T>
inline constexpr char_type* print_reserve_define(
	io_reserve_type_t<char_type,
	::fast_io::manipulators::basic_leb128_get_put<T>>,
	char_type *iter,
	::fast_io::manipulators::basic_leb128_get_put<T> v) noexcept
{
	return ::fast_io::details::pr_rsv_leb128_impl(iter,v.reference);
}

}
