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

template<::fast_io::details::my_integral T>
inline constexpr auto leb128_get(T & t) noexcept
{
	return basic_leb128_get_put<T*>{ &t };
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
				*iter=temp;
				return ++iter;
			}
			*iter=temp|0x80;
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
					*iter=temp;
					return ++iter;
				}
			}
			*iter=temp|0x80;
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

struct leb128_scan_state_t
{
	std::uint_least64_t group_count{};
};

template <std::integral char_type, typename I>
inline constexpr
io_type_t<leb128_scan_state_t> scan_context_type(io_reserve_type_t<char_type, manipulators::basic_leb128_get_put<I>>) noexcept
{
	return {};
}

template <std::integral char_type, ::fast_io::details::my_signed_integral I>
inline constexpr parse_result<char_type const*> scan_context_define(
	io_reserve_type_t<char_type, manipulators::basic_leb128_get_put<I*>>,
	leb128_scan_state_t& state,
	char_type const* begin, char_type const* end,
	manipulators::basic_leb128_get_put<I*> t) noexcept
{
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	using U = std::make_unsigned_t<I>;
	U tmp{};
	std::uint_least64_t cnt{ state.group_count };
	if (cnt == 0)
		*t.reference = 0;
	for (; begin != end; cnt += 7)
	{
		bool sign = (static_cast<unsigned_char_type>(*begin) & 0x80) != 0;
		std::int_fast8_t byte = static_cast<unsigned_char_type>(*begin) & 0x7f;
		++begin;
		constexpr auto digits{ std::numeric_limits<U>::digits };
		// TODO: optimize
		if (cnt > digits - 7 &&
			(
				cnt > digits ||
				(!(byte & 0x40) && byte >= (1u << (digits % 7))) ||
				((byte & 0x40) && (-byte & 0x7f) >= (1u << (digits % 7)))
				)
			) [[unlikely]]
			return { begin, parse_code::overflow };
		tmp |= static_cast<U>(byte) << cnt;
		if (!sign)
		{
			if (byte & 0x40 && cnt < digits - 7)
			{
				tmp |= static_cast<U>(-1) << (cnt + 7);
			}
			*t.reference |= tmp;
			return { begin, parse_code::ok };
		}
	}
	*t.reference |= tmp;
	state.group_count = cnt;
	return { begin, parse_code::partial };
}

template <std::integral char_type, ::fast_io::details::my_unsigned_integral U>
inline constexpr parse_result<char_type const*> scan_context_define(
	io_reserve_type_t<char_type, manipulators::basic_leb128_get_put<U*>>,
	leb128_scan_state_t& state,
	char_type const* begin, char_type const* end,
	manipulators::basic_leb128_get_put<U*> t) noexcept
{
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	U tmp{};
	std::uint_least64_t cnt{ state.group_count };
	if (cnt == 0)
		*t.reference = 0;
	for (; begin != end; cnt += 7)
	{
		bool sign = static_cast<unsigned_char_type>(*begin) & 0x80;
		std::uint_fast8_t byte = static_cast<unsigned_char_type>(*begin) & 0x7f;
		++begin;
		constexpr auto digits{ std::numeric_limits<U>::digits };
		if (cnt > digits - 7 && (cnt > digits || byte >= (1u << (digits % 7)))) [[unlikely]]
			return { begin, parse_code::overflow };
		tmp |= static_cast<U>(byte) << cnt;
		if (!sign)
		{
			*t.reference |= tmp;
			return { begin, parse_code::ok };
		}
	}
	*t.reference |= tmp;
	state.group_count = cnt;
	return { begin, parse_code::partial };
}

template <std::integral char_type, typename I>
inline constexpr parse_code scan_context_eof_define(io_reserve_type_t<char_type, manipulators::basic_leb128_get_put<I>>, leb128_scan_state_t& state, manipulators::basic_leb128_get_put<I> t) noexcept
{
	return parse_code::end_of_file;
}

template <std::integral char_type, ::fast_io::details::my_signed_integral I>
inline constexpr
parse_result<char_type const*> scan_contiguous_define(
	io_reserve_type_t<char_type, manipulators::basic_leb128_get_put<I*>>,
	char_type const* begin, char_type const* end,
	manipulators::basic_leb128_get_put<I*> t) noexcept
{
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	using U = std::make_unsigned_t<I>;
	U tmp{};
	std::uint_least64_t cnt{};
	*t.reference = 0;
	for (; begin != end; cnt += 7)
	{
		bool sign = (static_cast<unsigned_char_type>(*begin) & 0x80) != 0;
		std::int_fast8_t byte = static_cast<unsigned_char_type>(*begin) & 0x7f;
		++begin;
		constexpr auto digits{ std::numeric_limits<U>::digits };
		// TODO: optimize
		if (cnt > digits - 7 &&
			(
				cnt > digits ||
				(!(byte & 0x40) && byte >= (1u << (digits % 7))) ||
				((byte & 0x40) && (-byte & 0x7f) >= (1u << (digits % 7)))
				)
			) [[unlikely]]
			return { begin, parse_code::overflow };
		tmp |= static_cast<U>(byte) << cnt;
		if (!sign)
		{
			if (byte & 0x40 && cnt < digits - 7)
			{
				tmp |= static_cast<U>(-1) << (cnt + 7);
			}
			*t.reference |= tmp;
			return { begin, parse_code::ok };
		}
	}
	return { begin, parse_code::invalid };
}

template <std::integral char_type, ::fast_io::details::my_unsigned_integral U>
inline constexpr
parse_result<char_type const*> scan_contiguous_define(
	io_reserve_type_t<char_type, manipulators::basic_leb128_get_put<U*>>,
	char_type const* begin, char_type const* end,
	manipulators::basic_leb128_get_put<U*> t) noexcept
{
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	U tmp{};
	std::uint_least64_t cnt{};
	*t.reference = 0;
	for (; begin != end; cnt += 7)
	{
		bool sign = static_cast<unsigned_char_type>(*begin) & 0x80;
		std::uint_fast8_t byte = static_cast<unsigned_char_type>(*begin) & 0x7f;
		++begin;
		constexpr auto digits{ std::numeric_limits<U>::digits };
		if (cnt > digits - 7 && (cnt > digits || byte >= (1u << (digits % 7)))) [[unlikely]]
			return { begin, parse_code::overflow };
		tmp |= static_cast<U>(byte) << cnt;
		if (!sign)
		{
			*t.reference |= tmp;
			return { begin, parse_code::ok };
		}
	}
	return { begin, parse_code::invalid };
}


}
