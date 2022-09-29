#pragma once

namespace fast_io
{

namespace manipulators
{

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

template<::fast_io::details::my_integral T>
inline constexpr auto leb128_put(::fast_io::details::my_integral t) noexcept
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
	constexpr std::size_t digits{std::numeric_limits<T>::digits + (::fast_io::details::my_signed_integral<T>)};
	constexpr std::size_t seven{7};
	constexpr std::size_t digitsdiv7{digits/seven};
	constexpr bool notsevenmul{(digits%seven)!=0};
	constexpr std::size_t urret{notsevenmul+digitsdiv7};
	return urret;
}

namespace details
{

template<std::integral char_type,::fast_io::details::my_integral T>
inline constexpr char_type* pr_rsv_leb128_impl(char_type* iter,T value) noexcept
{
	using unsigned_char_type =
		::std::make_unsigned_t<char_type>;
	if constexpr(::fast_io::details::my_unsigned_integral<T>)
	{
		for(;;++iter)
		{
			unsigned_char_type temp{static_cast<unsigned_char_type>(value&0x7f)};
			value >>= 7;
			if(!value)
			{
				temp|=0x80;
				*iter=temp;
				return ++iter;
			}
			*iter=temp;
		}
	}
	else
	{
		for(;;++iter)
		{
			unsigned_char_type temp{static_cast<unsigned_char_type>(value&0x7f)};
			value >>= 7;
			bool iszero{!value};
			if(iszero|(value==-1))
			{
				bool const m{(temp&0x40)!=0};
				if(iszero!=m)
				{
					temp|=0x80;
					*iter=temp;
					return ++iter;
				}
			}
			*iter=temp;
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
