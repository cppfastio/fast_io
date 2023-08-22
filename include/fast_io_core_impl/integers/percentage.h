#pragma once

namespace fast_io
{

namespace manipulators
{

template<typename T,typename U>
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

template<bool showpos=false,typename T,typename U>
requires (::fast_io::details::my_integral<T>&&::fast_io::details::my_integral<U>)
inline constexpr auto percentage_conventional(T num,U deno) noexcept
{
	return ::fast_io::manipulators::scalar_manip_t<
	::fast_io::details::base_mani_flags_cache<10,false,false,showpos,false>,
	::fast_io::manipulators::percentage_precision_t<
	typename ::fast_io::details::integer_alias_type_traits<T>::alias_type,
	typename ::fast_io::details::integer_alias_type_traits<U>::alias_type>>{{num,deno}};
}

template<bool showpos=false,typename T,typename U>
requires (::fast_io::details::my_integral<T>&&::fast_io::details::my_integral<U>)
inline constexpr auto comma_percentage_conventional(T num,U deno) noexcept
{
	return ::fast_io::manipulators::scalar_manip_t<
	::fast_io::details::base_mani_flags_cache<10,false,false,showpos,true>,
	::fast_io::manipulators::percentage_precision_t<
	typename ::fast_io::details::integer_alias_type_traits<T>::alias_type,
	typename ::fast_io::details::integer_alias_type_traits<U>::alias_type>>{{num,deno}};
}

}

template<::std::integral chartype,::fast_io::manipulators::scalar_flags flags,typename T,typename U>
requires (::fast_io::details::my_integral<T>&&::fast_io::details::my_integral<U>)
inline constexpr ::std::size_t print_reserve_size(::fast_io::io_reserve_type_t<chartype,
	::fast_io::manipulators::scalar_manip_t<flags,::fast_io::manipulators::percentage_conventional_t<T,U>>>) noexcept
{
	return ::fast_io::details::print_integer_reserved_size_cache<flags.base,flags.showbase,flags.showpos,T> + 5u;
}

namespace details
{

template<::std::size_t base,bool uppercase,bool showpos,::std::integral chartype,typename T,typename U>
requires (::fast_io::details::my_integral<T>&&::fast_io::details::my_integral<U>)
inline constexpr chartype* prrsv_percentage_conventional_impl(chartype *iter,T numerator,U denominator) noexcept
{
	using unsignednumeratortype = ::fast_io::details::my_make_unsigned_t<T>;
	using unsigneddenominatortype = ::fast_io::details::my_make_unsigned_t<U>;
	if constexpr(::fast_io::details::my_signed_integral<T>||::fast_io::details::my_signed_integral<U>)
	{
		bool isnegative{numerator<0};
		unsignednumeratortype unsignednum{static_cast<unsignednumeratortype>(numerator)};
		if(isnegative)
		{
			constexpr
				unsignednumeratortype zero{};
			unsignednum=zero-unsignednum;
		}
		unsigneddenominatortype unsignedden{static_cast<unsigneddenominatortype>(denominator)};
		if(denominator<0)
		{
			constexpr
				unsigneddenominatortype zero{};
			unsignedden=zero-unsignedden;
			isnegative=!isnegative;
		}
		if constexpr(showpos)
		{
			char_type sign;
			if(isnegative)
			{
				sign=::fast_io::details::char_literal_v<u8'-',chartype>
			}
			else
			{
				sign=::fast_io::details::char_literal_v<u8'+',chartype>;
			}
			*iter=sign;
			++iter;
		}
		else
		{
			if(isnegative)
			{
				*iter=::fast_io::details::char_literal_v<u8'-',chartype>;
				++iter;
			}
		}
		return prrsv_percentage_conventional_impl(iter,unsignednum,unsignedden);
	}
	else if constexpr(sizeof(T)<sizeof(::std::uint_least32_t))
	{
		return prrsv_percentage_conventional_impl(iter,static_cast<::std::uint_least32_t>(numerator),denominator) noexcept
	}
	else
	{
		if(denominator==0)	//denominator cannot be zero. so we print out nan
		{
			if constexpr(uppercase)
			{
				if constexpr(::std::same_as<chartype,char>)
				{
					iter=::fast_io::details::copy_string_literal("NAN",iter);
				}
				else if constexpr(::std::same_as<chartype,wchar_t>)
				{
					iter=::fast_io::details::copy_string_literal(L"NAN",iter);
				}
				else if constexpr(::std::same_as<chartype,char16_t>)
				{
					iter=::fast_io::details::copy_string_literal(u"NAN",iter);
				}
				else if constexpr(::std::same_as<chartype,char32_t>)
				{
					iter=::fast_io::details::copy_string_literal(U"NAN",iter);
				}
				else if constexpr(::std::same_as<chartype,char8_t>)
				{
					iter=::fast_io::details::copy_string_literal(u8"NAN",iter);
				}
			}
			else
			{
				if constexpr(::std::same_as<chartype,char>)
				{
					iter=::fast_io::details::copy_string_literal("nan",iter);
				}
				else if constexpr(::std::same_as<chartype,wchar_t>)
				{
					iter=::fast_io::details::copy_string_literal(L"nan",iter);
				}
				else if constexpr(::std::same_as<chartype,char16_t>)
				{
					iter=::fast_io::details::copy_string_literal(u"nan",iter);
				}
				else if constexpr(::std::same_as<chartype,char32_t>)
				{
					iter=::fast_io::details::copy_string_literal(U"nan",iter);
				}
				else if constexpr(::std::same_as<chartype,char8_t>)
				{
					iter=::fast_io::details::copy_string_literal(u8"nan",iter);
				}
			}
		}
		constexpr
			T highu{static_cast<T>(base)*static_cast<T>(base)};
		T numerator10000high;
		auto numerator10000low{::fast_io::intrinsics::umulh(T,highu,high)};
		auto denominatordiv2{denominator>>1u};
		bool denominatorisodd{denominator&1u};
		if(numerator10000high==0u)
#if __has_cpp_attribute(likely)
		[[likely]]
#endif
		{
			auto quotient{numerator10000low/denominator};
			auto remainder{numerator10000low%denominator};
			if(remainder)
		}
		else
		{

		}
		return iter;
#if 0
		constexpr
			unsignednumeratortype un100max{static_cast<unsignednumeratortype>(::std::numeric_limits<U>::max()/static_cast<U>(100u))};
		decltype(numerator*100u) numerator100;

		numerator100=numerator*100u;//pesudo code
		auto quotient{numerator100/denominator};
#endif
	}
}
	
}

template<::std::integral chartype,::fast_io::manipulators::scalar_flags flags,typename T,typename U>
requires (::fast_io::details::my_integral<T>&&::fast_io::details::my_integral<U>)
inline constexpr chartype* print_reserve_define(::fast_io::io_reserve_type_t<chartype,
	::fast_io::manipulators::scalar_manip_t<flags,::fast_io::manipulators::percentage_conventional_t<T,U>>>,
	chartype* iter,
	::fast_io::manipulators::scalar_manip_t<flags,::fast_io::manipulators::percentage_conventional_t<T,U>> v) noexcept
{
	return iter;
}

}
