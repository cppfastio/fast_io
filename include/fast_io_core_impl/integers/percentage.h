#pragma once

namespace fast_io
{

namespace manipulators
{

template <typename T, typename U>
struct percentage_conventional_t
{
	using numerator_type = T;
	using denominator_type = U;
	numerator_type numerator;
	denominator_type denominator;
};

#if 0
template<typename T,typename U>
struct percentage_precision_t
{
	using numerator_type = T;
	using denominator_type = U;
	numerator_type numerator;
	denominator_type denominator;
	::std::size_t precisions;
};
#endif

template <bool upper = false, bool fll = false, bool showpos = false, typename T, typename U>
	requires(::fast_io::details::my_integral<T> && ::fast_io::details::my_integral<U>)
inline constexpr auto percentage_conventional(T num, U deno) noexcept
{
	return ::fast_io::manipulators::scalar_manip_t<
		::fast_io::details::base_mani_flags_cache<10, upper, false, fll, showpos, false,
												  ::fast_io::manipulators::percentage_flag::percent>,
		::fast_io::manipulators::percentage_conventional_t<
			typename ::fast_io::details::integer_alias_type_traits<T>::alias_type,
			typename ::fast_io::details::integer_alias_type_traits<U>::alias_type>>{{num, deno}};
}

template <bool upper = false, bool fll = false, bool showpos = false, typename T, typename U>
	requires(::fast_io::details::my_integral<T> && ::fast_io::details::my_integral<U>)
inline constexpr auto comma_percentage_conventional(T num, U deno) noexcept
{
	return ::fast_io::manipulators::scalar_manip_t<
		::fast_io::details::base_mani_flags_cache<10, upper, false, fll, showpos, true,
												  ::fast_io::manipulators::percentage_flag::percent>,
		::fast_io::manipulators::percentage_conventional_t<
			typename ::fast_io::details::integer_alias_type_traits<T>::alias_type,
			typename ::fast_io::details::integer_alias_type_traits<U>::alias_type>>{{num, deno}};
}

template <::std::size_t base, bool upper = false, bool shbase = false, bool fll = false, bool showpos = false,
		  typename T, typename U>
	requires(::fast_io::details::my_integral<T> && ::fast_io::details::my_integral<U>)
inline constexpr auto base_percentage_conventional(T num, U deno) noexcept
{
	return ::fast_io::manipulators::scalar_manip_t<
		::fast_io::details::base_mani_flags_cache<base, upper, shbase, fll, showpos, false,
												  ::fast_io::manipulators::percentage_flag::percent>,
		::fast_io::manipulators::percentage_conventional_t<
			typename ::fast_io::details::integer_alias_type_traits<T>::alias_type,
			typename ::fast_io::details::integer_alias_type_traits<U>::alias_type>>{{num, deno}};
}

template <::std::size_t base, bool upper = false, bool shbase = false, bool fll = false, bool showpos = false,
		  typename T, typename U>
	requires(::fast_io::details::my_integral<T> && ::fast_io::details::my_integral<U>)
inline constexpr auto comma_base_percentage_conventional(T num, U deno) noexcept
{
	return ::fast_io::manipulators::scalar_manip_t<
		::fast_io::details::base_mani_flags_cache<base, upper, shbase, fll, showpos, true,
												  ::fast_io::manipulators::percentage_flag::percent>,
		::fast_io::manipulators::percentage_conventional_t<
			typename ::fast_io::details::integer_alias_type_traits<T>::alias_type,
			typename ::fast_io::details::integer_alias_type_traits<U>::alias_type>>{{num, deno}};
}

/*
https://www.youtube.com/watch?v=XN2-YEgUMg0
Why Sex Is Binary
Idpols are reactionary anti-scientific trash.
*/

template <bool upper = false, bool fll = false, bool showpos = false, typename T, typename U>
	requires(::fast_io::details::my_integral<T> && ::fast_io::details::my_integral<U>)
inline constexpr auto percentage_sex_ratio(T male, U female) noexcept
{
	return ::fast_io::manipulators::scalar_manip_t<
		::fast_io::details::base_mani_flags_cache<10, upper, false, fll, showpos, false,
												  ::fast_io::manipulators::percentage_flag::sexratio>,
		::fast_io::manipulators::percentage_conventional_t<
			typename ::fast_io::details::integer_alias_type_traits<T>::alias_type,
			typename ::fast_io::details::integer_alias_type_traits<U>::alias_type>>{{male, female}};
}

template <bool upper = false, bool fll = false, bool showpos = false, typename T, typename U>
	requires(::fast_io::details::my_integral<T> && ::fast_io::details::my_integral<U>)
inline constexpr auto comma_percentage_sex_ratio(T male, U female) noexcept
{
	return ::fast_io::manipulators::scalar_manip_t<
		::fast_io::details::base_mani_flags_cache<10, upper, false, fll, showpos, true,
												  ::fast_io::manipulators::percentage_flag::sexratio>,
		::fast_io::manipulators::percentage_conventional_t<
			typename ::fast_io::details::integer_alias_type_traits<T>::alias_type,
			typename ::fast_io::details::integer_alias_type_traits<U>::alias_type>>{{male, female}};
}

template <::std::size_t base, bool upper = false, bool shbase = false, bool fll = false, bool showpos = false,
		  typename T, typename U>
	requires(::fast_io::details::my_integral<T> && ::fast_io::details::my_integral<U>)
inline constexpr auto base_percentage_sex_ratio(T male, U female) noexcept
{
	return ::fast_io::manipulators::scalar_manip_t<
		::fast_io::details::base_mani_flags_cache<base, upper, shbase, fll, showpos, false,
												  ::fast_io::manipulators::percentage_flag::sexratio>,
		::fast_io::manipulators::percentage_conventional_t<
			typename ::fast_io::details::integer_alias_type_traits<T>::alias_type,
			typename ::fast_io::details::integer_alias_type_traits<U>::alias_type>>{{male, female}};
}

template <::std::size_t base, bool upper = false, bool shbase = false, bool fll = false, bool showpos = false,
		  typename T, typename U>
	requires(::fast_io::details::my_integral<T> && ::fast_io::details::my_integral<U>)
inline constexpr auto comma_base_percentage_sex_ratio(T male, U female) noexcept
{
	return ::fast_io::manipulators::scalar_manip_t<
		::fast_io::details::base_mani_flags_cache<base, upper, shbase, fll, showpos, true,
												  ::fast_io::manipulators::percentage_flag::sexratio>,
		::fast_io::manipulators::percentage_conventional_t<
			typename ::fast_io::details::integer_alias_type_traits<T>::alias_type,
			typename ::fast_io::details::integer_alias_type_traits<U>::alias_type>>{{male, female}};
}

} // namespace manipulators

template <::std::integral chartype, ::fast_io::manipulators::scalar_flags flags, typename T, typename U>
	requires(::fast_io::details::my_integral<T> && ::fast_io::details::my_integral<U>)
inline constexpr ::std::size_t print_reserve_size(
	::fast_io::io_reserve_type_t<chartype,
								 ::fast_io::manipulators::scalar_manip_t<
									 flags, ::fast_io::manipulators::percentage_conventional_t<T, U>>>) noexcept
{
	constexpr auto val{
		::fast_io::details::print_integer_reserved_size_cache<flags.base, flags.showbase, flags.showpos, T> + 5u};
	if constexpr (flags.percentage == ::fast_io::manipulators::percentage_flag::percent)
	{
		return val + 1u;
	}
	else if constexpr (flags.percentage == ::fast_io::manipulators::percentage_flag::sexratio)
	{
		constexpr bool showpos{flags.showpos || ::fast_io::details::my_signed_integral<U>};
		constexpr ::std::size_t percentagesexratiocommonsize{val + 4u + static_cast<::std::size_t>(showpos)};
		if constexpr (flags.showbase)
		{
			constexpr ::std::size_t sexratioshowbase{percentagesexratiocommonsize +
													 ::fast_io::details::print_showbase_length<flags.base>};
			return sexratioshowbase;
		}
		else
		{
			return percentagesexratiocommonsize;
		}
		/*:100*/
	}
	else
	{
		return val;
	}
}

namespace details
{

template <::std::size_t base, typename T>
	requires(2 <= base && base <= 36)
inline constexpr T compute_base_ul64_max_val() noexcept
{
	constexpr T ul64max{static_cast<T>(-1)};
	constexpr T ulmxdv{ul64max / base / base};
	T val{1u};
	for (; val <= ulmxdv; val *= base)
		;
	return val;
}

template <::std::size_t base, typename T>
	requires(2 <= base && base <= 36)
inline constexpr T base_ul64_max_val{::fast_io::details::compute_base_ul64_max_val<base, T>()};

template <::std::size_t base, bool uppercase, bool showbase, bool uppercase_showbase, bool full, bool comma,
		  ::std::integral chartype, typename T, typename U>
	requires(::fast_io::details::my_unsigned_integral<T> && ::fast_io::details::my_unsigned_integral<U>)
inline constexpr chartype *prrsv_percentage_main_common_impl(chartype *iter, T numerator, U denominator) noexcept
{
	if constexpr (showbase)
	{
		iter = print_reserve_show_base_impl<base, uppercase_showbase>(iter);
		return prrsv_percentage_main_common_impl<base, uppercase, false, false, full, comma>(iter, numerator,
																							 denominator);
	}
	else
	{
		constexpr ::std::uint_least64_t twodigits{static_cast<T>(base) * static_cast<T>(base)};
		constexpr ::std::uint_least64_t highu{twodigits * twodigits};
		constexpr auto tb{::fast_io::details::digits_table<chartype, base, uppercase>};
		if constexpr (sizeof(T) < sizeof(::std::uint_least64_t))
		{
			::std::uint_least64_t numerator10000low{numerator * static_cast<::std::uint_least64_t>(highu)};
			auto denominatordiv2{denominator >> 1u};
			bool denominatoriseven{!(denominator & 1u)};

			auto quotient{numerator10000low / denominator};
			auto remainder{numerator10000low % denominator};
			if ((denominatordiv2 < remainder) ||
				(remainder == denominatordiv2 && denominatoriseven && ((quotient & 1u) != 0u))) // round
			{
				++quotient; // should be okay
			}
			auto quotientdiv100{quotient / twodigits};
			unsigned quotientmod100{static_cast<unsigned>(quotient % twodigits)};
			if constexpr (full)
			{
				constexpr ::std::size_t toprint{::fast_io::details::cal_max_int_size<T, base>()};
				::fast_io::details::print_reserve_integral_main_impl<base, uppercase>(iter += toprint, quotientdiv100,
																					  toprint);
			}
			else
			{
				iter = ::fast_io::details::print_reserve_integral_define<base, showbase, uppercase_showbase, false,
																		 uppercase, full>(iter, quotientdiv100);
			}
			*iter = ::fast_io::char_literal_v<(comma ? u8',' : u8'.'), chartype>;
			++iter;
			::fast_io::details::non_overlapped_copy_n(tb + (quotientmod100 << 1u), 2u, iter);
			iter += 2u;
		}
		else
		{
			T numerator10000high;
			T numerator10000low;
			if constexpr (sizeof(T) <= sizeof(::std::uint_least64_t))
			{
				numerator10000low = ::fast_io::details::intrinsics::umul(numerator, highu, numerator10000high);
			}
			else
			{
				::std::uint_least64_t numeratorhigh;
				auto numeratorlow{::fast_io::details::intrinsics::unpack_generic(numerator, numeratorhigh)};

				::std::uint_least64_t numlh;
				auto numll = ::fast_io::details::intrinsics::umul(numeratorlow, highu, numlh);

				::std::uint_least64_t numhl;
				auto numhh = ::fast_io::details::intrinsics::umul(numeratorhigh, highu, numhl);
				constexpr ::std::uint_least64_t zero{};

				bool carry{};
				numhl = ::fast_io::intrinsics::addc(numlh, numhl, carry, carry);
				numhh = ::fast_io::intrinsics::addc(numhh, zero, carry, carry);
#if 0
				bool carry{::fast_io::details::intrinsics::add_carry(false,numlh,numhl,numhl)};
				::fast_io::details::intrinsics::add_carry(carry,numhh,zero,numhh);
#endif
				numerator10000low = (static_cast<T>(numhl) << 64u) | numll;
				numerator10000high = static_cast<T>(numhh);
			}
			auto denominatordiv2{denominator >> 1u};
			bool denominatoriseven{!(denominator & 1u)};
			unsigned quotientmod100;
			if (numerator10000high == 0u)
#if __has_cpp_attribute(likely)
				[[likely]]
#endif
			{
				auto quotient{numerator10000low / denominator};
				auto remainder{numerator10000low % denominator};
				if ((denominatordiv2 < remainder) ||
					(remainder == denominatordiv2 && denominatoriseven && ((quotient & 1u) != 0u))) // round
				{
					++quotient; // should be okay
				}
				auto quotientdiv100{quotient / twodigits};
				quotientmod100 = static_cast<unsigned>(quotient % twodigits);
				iter = ::fast_io::details::print_reserve_integral_define<base, showbase, uppercase_showbase, false,
																		 uppercase, full>(iter, quotientdiv100);
			}
			else
			{
				using udivmodtype = decltype(numerator10000low + denominator);
				constexpr T zero{}, one{1u};
				auto [quotientlow, quotienthigh, remainderlow, remainderhigh] =
					::fast_io::intrinsics::udivmod<udivmodtype>(numerator10000low, numerator10000high, denominator,
																zero);

#if __has_cpp_attribute(assume)
				[[assume(remainderhigh == 0)]];
#endif
				if ((denominatordiv2 < remainderlow) ||
					(remainderlow == denominatordiv2 && denominatoriseven && ((quotientlow & 1u) != 0u))) // round
				{
					bool carry{};
					quotientlow = ::fast_io::intrinsics::addc(quotientlow, one, carry, carry);
					quotienthigh = ::fast_io::intrinsics::addc(quotienthigh, zero, carry, carry);
				}
				if (quotienthigh == 0u)
				{
					auto quotientdiv100{quotientlow / twodigits};
					quotientmod100 = static_cast<unsigned>(quotientlow % twodigits);
					iter = ::fast_io::details::print_reserve_integral_define<base, showbase, uppercase_showbase, false,
																			 uppercase, full>(iter, quotientdiv100);
				}
				else
				{
					constexpr auto mxval{::fast_io::details::base_ul64_max_val<base, T>};
					auto [quotientlowlow, quotientlowhigh, remainderlowlow, remainderlowhigh] =
						::fast_io::intrinsics::udivmod<udivmodtype>(quotientlow, quotienthigh, mxval, zero);
#if __has_cpp_attribute(assume)
					[[assume(quotientlowhigh == 0)]];
					[[assume(remainderlowhigh == 0)]];
#endif
					constexpr ::std::size_t toprint{4u};
					if constexpr (full)
					{
						::fast_io::details::print_reserve_integral_main_impl<base, uppercase>(iter += toprint,
																							  quotientlowlow, toprint);
					}
					else
					{
						iter =
							::fast_io::details::print_reserve_integral_define<base, showbase, uppercase_showbase, false,
																			  uppercase, full>(iter, quotientlowlow);
					}
					auto quotientdiv100{remainderlowlow / twodigits};
					quotientmod100 = static_cast<unsigned>(remainderlowlow % twodigits);

					constexpr ::std::size_t tdigitsm2{::fast_io::details::cal_max_int_size<T, base>() - toprint};
					::fast_io::details::print_reserve_integral_main_impl<base, uppercase>(iter += tdigitsm2,
																						  quotientdiv100, tdigitsm2);
				}
			}
			*iter = ::fast_io::char_literal_v<(comma ? u8',' : u8'.'), chartype>;
			++iter;
			::fast_io::details::non_overlapped_copy_n(tb + (quotientmod100 << 1u), 2u, iter);
			iter += 2u;
		}
		return iter;
	}
}

template <::std::size_t base, bool uppercase, ::std::integral chartype>
inline constexpr chartype *prrsv_percentage_denominatorzero_impl(chartype *iter) noexcept
{
	if constexpr (23u < base)
	{
		if constexpr (uppercase)
		{
			if constexpr (::std::same_as<chartype, char>)
			{
				iter = ::fast_io::details::copy_string_literal("#NAN", iter);
			}
			else if constexpr (::std::same_as<chartype, wchar_t>)
			{
				iter = ::fast_io::details::copy_string_literal(L"#NAN", iter);
			}
			else if constexpr (::std::same_as<chartype, char16_t>)
			{
				iter = ::fast_io::details::copy_string_literal(u"#NAN", iter);
			}
			else if constexpr (::std::same_as<chartype, char32_t>)
			{
				iter = ::fast_io::details::copy_string_literal(U"#NAN", iter);
			}
			else if constexpr (::std::same_as<chartype, char8_t>)
			{
				iter = ::fast_io::details::copy_string_literal(u8"#NAN", iter);
			}
		}
		else
		{
			if constexpr (::std::same_as<chartype, char>)
			{
				iter = ::fast_io::details::copy_string_literal("#nan", iter);
			}
			else if constexpr (::std::same_as<chartype, wchar_t>)
			{
				iter = ::fast_io::details::copy_string_literal(L"#nan", iter);
			}
			else if constexpr (::std::same_as<chartype, char16_t>)
			{
				iter = ::fast_io::details::copy_string_literal(u"#nan", iter);
			}
			else if constexpr (::std::same_as<chartype, char32_t>)
			{
				iter = ::fast_io::details::copy_string_literal(U"#nan", iter);
			}
			else if constexpr (::std::same_as<chartype, char8_t>)
			{
				iter = ::fast_io::details::copy_string_literal(u8"#nan", iter);
			}
		}
	}
	else
	{
		if constexpr (uppercase)
		{
			if constexpr (::std::same_as<chartype, char>)
			{
				iter = ::fast_io::details::copy_string_literal("NAN", iter);
			}
			else if constexpr (::std::same_as<chartype, wchar_t>)
			{
				iter = ::fast_io::details::copy_string_literal(L"NAN", iter);
			}
			else if constexpr (::std::same_as<chartype, char16_t>)
			{
				iter = ::fast_io::details::copy_string_literal(u"NAN", iter);
			}
			else if constexpr (::std::same_as<chartype, char32_t>)
			{
				iter = ::fast_io::details::copy_string_literal(U"NAN", iter);
			}
			else if constexpr (::std::same_as<chartype, char8_t>)
			{
				iter = ::fast_io::details::copy_string_literal(u8"NAN", iter);
			}
		}
		else
		{
			if constexpr (::std::same_as<chartype, char>)
			{
				iter = ::fast_io::details::copy_string_literal("nan", iter);
			}
			else if constexpr (::std::same_as<chartype, wchar_t>)
			{
				iter = ::fast_io::details::copy_string_literal(L"nan", iter);
			}
			else if constexpr (::std::same_as<chartype, char16_t>)
			{
				iter = ::fast_io::details::copy_string_literal(u"nan", iter);
			}
			else if constexpr (::std::same_as<chartype, char32_t>)
			{
				iter = ::fast_io::details::copy_string_literal(U"nan", iter);
			}
			else if constexpr (::std::same_as<chartype, char8_t>)
			{
				iter = ::fast_io::details::copy_string_literal(u8"nan", iter);
			}
		}
	}
	return iter;
}

template <::std::size_t base, bool uppercase, bool showbase, bool uppercase_showbase, bool full, bool showpos,
		  bool comma, ::std::integral chartype, typename T>
	requires(::fast_io::details::my_integral<T>)
inline constexpr chartype *prrsv_percentage_denominatorzero_sexratio_impl(chartype *iter, T numerator) noexcept
{
	iter =
		::fast_io::details::print_reserve_integral_define<base, showbase, uppercase_showbase, showpos, uppercase, full>(
			iter, numerator);
	if constexpr (showbase && base != 10)
	{
		if constexpr (comma)
		{
			if constexpr (showpos)
			{
				if constexpr (::std::same_as<chartype, char>)
				{
					iter = ::fast_io::details::copy_string_literal(",00:+", iter);
				}
				else if constexpr (::std::same_as<chartype, wchar_t>)
				{
					iter = ::fast_io::details::copy_string_literal(L",00:+", iter);
				}
				else if constexpr (::std::same_as<chartype, char16_t>)
				{
					iter = ::fast_io::details::copy_string_literal(u",00:+", iter);
				}
				else if constexpr (::std::same_as<chartype, char32_t>)
				{
					iter = ::fast_io::details::copy_string_literal(U",00:+", iter);
				}
				else
				{
					iter = ::fast_io::details::copy_string_literal(u8",00:+", iter);
				}
			}
			else
			{
				if constexpr (::std::same_as<chartype, char>)
				{
					iter = ::fast_io::details::copy_string_literal(",00:", iter);
				}
				else if constexpr (::std::same_as<chartype, wchar_t>)
				{
					iter = ::fast_io::details::copy_string_literal(L",00:", iter);
				}
				else if constexpr (::std::same_as<chartype, char16_t>)
				{
					iter = ::fast_io::details::copy_string_literal(u",00:", iter);
				}
				else if constexpr (::std::same_as<chartype, char32_t>)
				{
					iter = ::fast_io::details::copy_string_literal(U",00:", iter);
				}
				else
				{
					iter = ::fast_io::details::copy_string_literal(u8",00:", iter);
				}
			}
		}
		else
		{
			if constexpr (showpos)
			{
				if constexpr (::std::same_as<chartype, char>)
				{
					iter = ::fast_io::details::copy_string_literal(".00:+", iter);
				}
				else if constexpr (::std::same_as<chartype, wchar_t>)
				{
					iter = ::fast_io::details::copy_string_literal(L".00:+", iter);
				}
				else if constexpr (::std::same_as<chartype, char16_t>)
				{
					iter = ::fast_io::details::copy_string_literal(u".00:+", iter);
				}
				else if constexpr (::std::same_as<chartype, char32_t>)
				{
					iter = ::fast_io::details::copy_string_literal(U".00:+", iter);
				}
				else
				{
					iter = ::fast_io::details::copy_string_literal(u8".00:+", iter);
				}
			}
			else
			{
				if constexpr (::std::same_as<chartype, char>)
				{
					iter = ::fast_io::details::copy_string_literal(".00:", iter);
				}
				else if constexpr (::std::same_as<chartype, wchar_t>)
				{
					iter = ::fast_io::details::copy_string_literal(L".00:", iter);
				}
				else if constexpr (::std::same_as<chartype, char16_t>)
				{
					iter = ::fast_io::details::copy_string_literal(u".00:", iter);
				}
				else if constexpr (::std::same_as<chartype, char32_t>)
				{
					iter = ::fast_io::details::copy_string_literal(U".00:", iter);
				}
				else
				{
					iter = ::fast_io::details::copy_string_literal(u8".00:", iter);
				}
			}
		}
		iter = ::fast_io::details::print_reserve_show_base_impl<base, uppercase_showbase>(iter);
		*iter = ::fast_io::char_literal_v<u8'0', chartype>;
		++iter;
	}
	else
	{
		if constexpr (comma)
		{
			if constexpr (showpos)
			{
				if constexpr (::std::same_as<chartype, char>)
				{
					iter = ::fast_io::details::copy_string_literal(",00:+0", iter);
				}
				else if constexpr (::std::same_as<chartype, wchar_t>)
				{
					iter = ::fast_io::details::copy_string_literal(L",00:+0", iter);
				}
				else if constexpr (::std::same_as<chartype, char16_t>)
				{
					iter = ::fast_io::details::copy_string_literal(u",00:+0", iter);
				}
				else if constexpr (::std::same_as<chartype, char32_t>)
				{
					iter = ::fast_io::details::copy_string_literal(U",00:+0", iter);
				}
				else
				{
					iter = ::fast_io::details::copy_string_literal(u8",00:+0", iter);
				}
			}
			else
			{
				if constexpr (::std::same_as<chartype, char>)
				{
					iter = ::fast_io::details::copy_string_literal(",00:0", iter);
				}
				else if constexpr (::std::same_as<chartype, wchar_t>)
				{
					iter = ::fast_io::details::copy_string_literal(L",00:0", iter);
				}
				else if constexpr (::std::same_as<chartype, char16_t>)
				{
					iter = ::fast_io::details::copy_string_literal(u",00:0", iter);
				}
				else if constexpr (::std::same_as<chartype, char32_t>)
				{
					iter = ::fast_io::details::copy_string_literal(U",00:0", iter);
				}
				else
				{
					iter = ::fast_io::details::copy_string_literal(u8",00:0", iter);
				}
			}
		}
		else
		{
			if constexpr (showpos)
			{
				if constexpr (::std::same_as<chartype, char>)
				{
					iter = ::fast_io::details::copy_string_literal(".00:+0", iter);
				}
				else if constexpr (::std::same_as<chartype, wchar_t>)
				{
					iter = ::fast_io::details::copy_string_literal(L".00:+0", iter);
				}
				else if constexpr (::std::same_as<chartype, char16_t>)
				{
					iter = ::fast_io::details::copy_string_literal(u".00:+0", iter);
				}
				else if constexpr (::std::same_as<chartype, char32_t>)
				{
					iter = ::fast_io::details::copy_string_literal(U".00:+0", iter);
				}
				else
				{
					iter = ::fast_io::details::copy_string_literal(u8".00:+0", iter);
				}
			}
			else
			{
				if constexpr (::std::same_as<chartype, char>)
				{
					iter = ::fast_io::details::copy_string_literal(".00:0", iter);
				}
				else if constexpr (::std::same_as<chartype, wchar_t>)
				{
					iter = ::fast_io::details::copy_string_literal(L".00:0", iter);
				}
				else if constexpr (::std::same_as<chartype, char16_t>)
				{
					iter = ::fast_io::details::copy_string_literal(u".00:0", iter);
				}
				else if constexpr (::std::same_as<chartype, char32_t>)
				{
					iter = ::fast_io::details::copy_string_literal(U".00:0", iter);
				}
				else
				{
					iter = ::fast_io::details::copy_string_literal(u8".00:0", iter);
				}
			}
		}
	}
	return iter;
}

template <::std::size_t base, bool uppercase, bool showbase, bool uppercase_showbase, bool full, bool showpos,
		  bool comma, ::std::integral chartype, typename T, typename U>
	requires(::fast_io::details::my_integral<T> && ::fast_io::details::my_integral<U>)
inline constexpr chartype *prrsv_percentage_sexratio_signed_impl(chartype *iter, T numerator, U denominator,
																 bool denominatorisnegative) noexcept
{
	if constexpr (sizeof(T) < sizeof(::std::uint_least32_t) && !full)
	{
		return prrsv_percentage_sexratio_signed_impl<base, uppercase, showbase, uppercase_showbase, full, showpos,
													 comma>(iter, static_cast<::std::uint_least32_t>(numerator),
															denominator, denominatorisnegative);
	}
	else
	{
		iter = prrsv_percentage_main_common_impl<base, uppercase, showbase, uppercase_showbase, full, comma>(
			iter, numerator, denominator);
		*iter = ::fast_io::char_literal_v<u8':', chartype>;
		++iter;
		if constexpr (showpos)
		{
			*iter = denominatorisnegative ? (::fast_io::char_literal_v<u8'-', chartype>)
										  : (::fast_io::char_literal_v<u8'+', chartype>);
			++iter;
		}
		else
		{
			if (denominatorisnegative)
			{
				*iter = ::fast_io::char_literal_v<u8'-', chartype>;
				++iter;
			}
		}
		if constexpr (showbase && base != 10)
		{
			iter = ::fast_io::details::print_reserve_show_base_impl<base, uppercase_showbase>(iter);
		}
		if constexpr (::std::same_as<chartype, char>)
		{
			iter = ::fast_io::details::copy_string_literal("100", iter);
		}
		else if constexpr (::std::same_as<chartype, wchar_t>)
		{
			iter = ::fast_io::details::copy_string_literal(L"100", iter);
		}
		else if constexpr (::std::same_as<chartype, char16_t>)
		{
			iter = ::fast_io::details::copy_string_literal(u"100", iter);
		}
		else if constexpr (::std::same_as<chartype, char32_t>)
		{
			iter = ::fast_io::details::copy_string_literal(U"100", iter);
		}
		else
		{
			iter = ::fast_io::details::copy_string_literal(u8"100", iter);
		}
		return iter;
	}
}

template <::std::size_t base, bool uppercase, bool showbase, bool uppercase_showbase, bool full, bool showpos,
		  bool comma, ::std::integral chartype, typename T, typename U>
	requires(::fast_io::details::my_integral<T> && ::fast_io::details::my_integral<U>)
inline constexpr chartype *prrsv_percentage_sexratio_unsigned_impl(chartype *iter, T numerator, U denominator) noexcept
{
	iter = prrsv_percentage_main_common_impl<base, uppercase, showbase, uppercase_showbase, full, comma>(
		iter, numerator, denominator);
	*iter = ::fast_io::char_literal_v<u8':', chartype>;
	++iter;
	if constexpr (showpos)
	{
		*iter = ::fast_io::char_literal_v<u8'+', chartype>;
		++iter;
	}
	if constexpr (showbase && base != 10)
	{
		iter = ::fast_io::details::print_reserve_show_base_impl<base, uppercase_showbase>(iter);
	}
	if constexpr (::std::same_as<chartype, char>)
	{
		iter = ::fast_io::details::copy_string_literal("100", iter);
	}
	else if constexpr (::std::same_as<chartype, wchar_t>)
	{
		iter = ::fast_io::details::copy_string_literal(L"100", iter);
	}
	else if constexpr (::std::same_as<chartype, char16_t>)
	{
		iter = ::fast_io::details::copy_string_literal(u"100", iter);
	}
	else if constexpr (::std::same_as<chartype, char32_t>)
	{
		iter = ::fast_io::details::copy_string_literal(U"100", iter);
	}
	else
	{
		iter = ::fast_io::details::copy_string_literal(u8"100", iter);
	}
	return iter;
}

template <::std::size_t base, bool uppercase, bool showbase, bool uppercase_showbase, bool full, bool showpos,
		  bool comma, ::fast_io::manipulators::percentage_flag percent, ::std::integral chartype, typename T,
		  typename U>
	requires(::fast_io::details::my_integral<T> && ::fast_io::details::my_integral<U>)
inline constexpr chartype *prrsv_percentage_conventional_impl(chartype *iter, T numerator, U denominator) noexcept
{
	using unsignednumeratortype = ::fast_io::details::my_make_unsigned_t<T>;
	using unsigneddenominatortype = ::fast_io::details::my_make_unsigned_t<U>;
	if (denominator == 0)
	{
		if constexpr (::fast_io::manipulators::percentage_flag::sexratio == percent)
		{
			return prrsv_percentage_denominatorzero_sexratio_impl<base, uppercase, showbase, uppercase_showbase, full,
																  showpos, comma>(iter, numerator);
		}
		else
		{
			return prrsv_percentage_denominatorzero_impl<base, uppercase>(iter);
		}
	}
	if constexpr (::fast_io::details::my_signed_integral<T> || ::fast_io::details::my_signed_integral<U>)
	{
		bool isnegative, denominatorisnegative;
		unsignednumeratortype unsignednum{static_cast<unsignednumeratortype>(numerator)};
		if constexpr (::fast_io::details::my_signed_integral<T>)
		{
			isnegative = numerator < 0;
			if (isnegative)
			{
				constexpr unsignednumeratortype zero{};
				unsignednum = zero - unsignednum;
			}
		}
		else
		{
			isnegative = false;
		}
		unsigneddenominatortype unsignedden{static_cast<unsigneddenominatortype>(denominator)};
		if constexpr (::fast_io::details::my_signed_integral<U>)
		{
			denominatorisnegative = denominator < 0;
			if (denominatorisnegative)
			{
				constexpr unsigneddenominatortype zero{};
				unsignedden = zero - unsignedden;
				if constexpr (percent != ::fast_io::manipulators::percentage_flag::sexratio)
				{
					isnegative = !isnegative;
				}
			}
		}
		if constexpr (showpos)
		{
			chartype sign;
			if (isnegative)
			{
				sign = ::fast_io::char_literal_v<u8'-', chartype>;
			}
			else
			{
				sign = ::fast_io::char_literal_v<u8'+', chartype>;
			}
			*iter = sign;
			++iter;
		}
		else
		{
			if (isnegative)
			{
				*iter = ::fast_io::char_literal_v<u8'-', chartype>;
				++iter;
			}
		}
		if constexpr (percent == ::fast_io::manipulators::percentage_flag::sexratio &&
					  ::fast_io::details::my_signed_integral<U>)
		{
			return prrsv_percentage_sexratio_signed_impl<base, uppercase, showbase, uppercase_showbase, full, showpos,
														 comma>(iter, unsignednum, unsignedden, denominatorisnegative);
		}
		else
		{
			return prrsv_percentage_conventional_impl<base, uppercase, showbase, uppercase_showbase, full, showpos,
													  comma, percent>(iter, unsignednum, unsignedden);
		}
	}
	else if constexpr (sizeof(T) < sizeof(::std::uint_least32_t) && !full)
	{
		return prrsv_percentage_conventional_impl<base, uppercase, showbase, uppercase_showbase, full, showpos, comma,
												  percent>(iter, static_cast<::std::uint_least32_t>(numerator),
														   denominator);
	}
	else if constexpr (percent == ::fast_io::manipulators::percentage_flag::sexratio)
	{
		return prrsv_percentage_sexratio_unsigned_impl<base, uppercase, showbase, uppercase_showbase, full, showpos,
													   comma>(iter, numerator, denominator);
	}
	else
	{
		iter = ::fast_io::details::prrsv_percentage_main_common_impl<base, uppercase, showbase, uppercase_showbase,
																	 full, comma>(iter, numerator, denominator);
		if constexpr (percent == ::fast_io::manipulators::percentage_flag::percent)
		{
			*iter = ::fast_io::char_literal_v<u8'%', chartype>;
			++iter;
		}
		return iter;
	}
}

} // namespace details

template <::std::integral chartype, ::fast_io::manipulators::scalar_flags flags, typename T, typename U>
	requires(::fast_io::details::my_integral<T> && ::fast_io::details::my_integral<U>)
inline constexpr chartype *print_reserve_define(
	::fast_io::io_reserve_type_t<chartype, ::fast_io::manipulators::scalar_manip_t<
											   flags, ::fast_io::manipulators::percentage_conventional_t<T, U>>>,
	chartype *iter,
	::fast_io::manipulators::scalar_manip_t<flags, ::fast_io::manipulators::percentage_conventional_t<T, U>> v) noexcept
{
	return ::fast_io::details::prrsv_percentage_conventional_impl<flags.base, flags.uppercase, flags.showbase,
																  flags.uppercase_showbase, flags.full, flags.showpos,
																  flags.comma, flags.percentage>(
		iter, v.reference.numerator, v.reference.denominator);
}

} // namespace fast_io
