#pragma once

/*

Time output should strictly following ISO 8601
2020-11-07T12:50:20Z

*/

namespace fast_io::details
{

template<bool unchecked=false,::fast_io::freestanding::random_access_iterator Iter,my_unsigned_integral U>
inline constexpr Iter chrono_one_digit_impl(Iter it,U uv) noexcept
{
	if constexpr(!unchecked)
	{
		if(10u<=uv)[[unlikely]]
			return print_reserve_integral_define<10>(it,uv);
	}
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
	*it=static_cast<char_type>(uv+char_literal_v<u8'0',char_type>);
	++it;
	return it;
}

template<bool unchecked=false,bool transparent=false,::fast_io::freestanding::random_access_iterator Iter,my_integral I>
inline constexpr Iter chrono_two_digits_impl(Iter it,I i) noexcept
{
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
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
		constexpr auto tb{get_shared_inline_constexpr_base_table<char_type,10,false>().element};
		return non_overlapped_copy_n(tb[i].element,2,it);
	}
}

template<::fast_io::freestanding::random_access_iterator Iter,std::signed_integral integ>
inline constexpr Iter chrono_year_impl(Iter it,integ i) noexcept
{
	using unsigned_type = my_make_unsigned_t<std::remove_cvref_t<integ>>;
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
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
	constexpr auto tb{get_shared_inline_constexpr_base_table<char_type,10,false>().element};
	it=non_overlapped_copy_n(tb[dv].element,2,it);
	return non_overlapped_copy_n(tb[rmd].element,2,it);
}


}