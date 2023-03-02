#pragma once

/*

Time output should strictly following ISO 8601
2020-11-07T12:50:20Z
2023-02-07T15:53:33+08:00

*/

namespace fast_io::details
{

template<bool unchecked=false,::std::integral char_type,my_unsigned_integral U>
inline constexpr char_type* chrono_one_digit_impl(char_type* it,U uv) noexcept
{
	if constexpr(!unchecked)
	{
		if(10u<=uv)[[unlikely]]
			return print_reserve_integral_define<10>(it,uv);
	}
	*it=static_cast<char_type>(uv+arithmetic_char_literal_v<u8'0',char_type>);
	++it;
	return it;
}

template<bool unchecked=false,bool transparent=false,::std::integral char_type,my_integral I>
inline constexpr char_type* chrono_two_digits_impl(char_type* it,I i) noexcept
{
	using unsigned_char_type = my_make_unsigned_t<char_type>;
	if constexpr(my_signed_integral<I>)
	{
		my_make_unsigned_t<I> u{static_cast<my_make_unsigned_t<I>>(i)};
		if(i<0)[[unlikely]]
		{
			u = 0u - u;
			*it=char_literal_v<u8'-',char_type>;
			++it;
		}
		return chrono_two_digits_impl<unchecked>(it,u);
	}
	else
	{
		if constexpr(!unchecked)
		{
			if(100u<=i)[[unlikely]]
				return print_reserve_integral_define<10>(it,i);
		}
		if constexpr(transparent)
		{
			constexpr std::uint_least8_t ten{static_cast<std::uint_least8_t>(10u)};
			if(i<ten)
			{
				*it = char_literal_v<u8' ',char_type>;
				++it;
				*it = static_cast<char_type>(static_cast<unsigned_char_type>(static_cast<unsigned_char_type>(char_literal_v<u8'0',char_type>)+i));
				++it;
				return it;
			}
		}
		return non_overlapped_copy_n(digits_table<char_type,10,false>+(i<<1),2,it);
	}
}

template<::std::integral char_type,::std::signed_integral integ>
inline constexpr char_type* chrono_year_impl(char_type* it,integ i) noexcept
{
	using unsigned_type = my_make_unsigned_t<std::remove_cvref_t<integ>>;
	unsigned_type u{static_cast<unsigned_type>(i)};
	if(i<0)[[unlikely]]
	{
		u = 0u - u;
		*it=char_literal_v<u8'-',char_type>;
		++it;
	}
	if(10000u<=u)[[unlikely]]
		return print_reserve_integral_define<10>(it,u);
	unsigned_type dv(u/100u);
	unsigned_type rmd(u%100u);
	constexpr auto tb{digits_table<char_type,10,false>};
	it=non_overlapped_copy_n(tb+(dv<<1),2,it);
	return non_overlapped_copy_n(tb+(rmd<<1),2,it);
}

// assert(end - begin >= 2)
template <::std::integral char_type, ::std::integral T>
inline constexpr parse_code chrono_scan_two_digits_unsafe_impl(char_type const* begin, T& t) noexcept
{
	T retval{};
	auto ch{ *begin };
	if (!::fast_io::char_category::is_c_digit(ch)) [[unlikely]]
		return parse_code::invalid;
	retval += static_cast<T>(ch - char_literal_v<u8'0', char_type>) * 10;
	++begin;
	ch = *begin;
	if (!::fast_io::char_category::is_c_digit(ch)) [[unlikely]]
		return parse_code::invalid;
	retval += static_cast<T>(ch - char_literal_v<u8'0', char_type>);
	t = retval;
	return parse_code::ok;
}

template <::std::integral char_type, ::std::signed_integral I>
inline constexpr parse_result<char_type const*> chrono_scan_year_impl(char_type const* begin, char_type const* end, I& i) noexcept
{
	I retval{};
	if (end - begin < 20) [[unlikely]]
		return { end, parse_code::invalid };
	[[maybe_unused]] bool sign{};
	if (*begin == char_literal_v<u8'-', char_type>) [[unlikely]]
	{
		sign = true;
		++begin;
	}
	auto itr{ begin };
	for (; itr < begin + 4; ++itr)
		if (!details::char_is_digit<10, char_type>(*itr)) [[unlikely]]
			return { itr, parse_code::invalid };
	if (details::char_is_digit<10, char_type>(*itr)) [[unlikely]]
	{
		auto [itr2, ec] = scan_int_contiguous_define_impl<10, true, false, false>(begin, end, retval);
		if (ec != parse_code::ok) [[unlikely]]
			return { itr, ec };
		begin = itr2;
	}
	else
	{
		retval += static_cast<::std::int_least64_t>(*begin++ - char_literal_v<u8'0', char_type>) * 1000;
		retval += static_cast<::std::int_least64_t>(*begin++ - char_literal_v<u8'0', char_type>) * 100;
		retval += static_cast<::std::int_least64_t>(*begin++ - char_literal_v<u8'0', char_type>) * 10;
		retval += static_cast<::std::int_least64_t>(*begin++ - char_literal_v<u8'0', char_type>);
	}
	i = sign ? -retval : retval;
	return { begin, parse_code::ok };
}

template <::std::integral char_type, my_integral T>
inline constexpr void chrono_scan_decimal_fraction_part_rounding_impl(char_type const* begin, char_type const* end, T& retval) noexcept
{
	if (*begin < char_literal_v<u8'5', char_type>);
	else if (*begin > char_literal_v<u8'5', char_type>)
		++retval;
	else [[unlikely]]
	{
		for (++begin; begin != end; ++begin)
		{
			// xxxxx500000x0000, then round in
			if (*begin != char_literal_v<u8'0', char_type>)
			{
				++retval;
				return;
			}
		}
		// xxxx500000, then it depends on the digit before 5
		if (retval % 2 == 1)
			++retval;
	}
}

template <bool need_rounding = true, ::std::integral char_type, my_integral T>
inline constexpr parse_result<char_type const*> chrono_scan_decimal_fraction_part_never_overflow_impl(char_type const* begin, char_type const* end, T& t) noexcept
{
	if (begin == end) [[unlikely]]
		return { end, parse_code::invalid };
	T retval{};
	constexpr ::std::size_t digitsm1{ ::std::numeric_limits<T>::digits10 };
	auto new_end{ begin + digitsm1 };
	if (new_end > end) new_end = end;
	auto [itr, ec] = scan_int_contiguous_define_impl<10, true, false, true>(begin, new_end, retval);
	if (ec != parse_code::ok) [[unlikely]]
		return { itr, ec };
	if (begin == itr) [[unlikely]]
		return { itr, parse_code::invalid };
	::std::size_t zero_cnt{ digitsm1 - (itr - begin) };
	for (::std::size_t i{}; i < zero_cnt; ++i)
		retval *= 10;
	if constexpr (need_rounding)
	{
		if (itr != end && itr == new_end) [[unlikely]]
		{
			char_type const* digit_end{ skip_digits<10>(itr, end) };
			if (itr != digit_end)
			{
				chrono_scan_decimal_fraction_part_rounding_impl(itr, digit_end, retval);
				itr = digit_end;
			}
		}
	}
	t = retval;
	return { itr, parse_code::ok };
}

}
