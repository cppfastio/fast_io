#pragma once

/*
fast_io's own uprsv (uint print reserve) algorithm
Referenced from xjkp's MyItoA algorithm:
https://gitee.com/xjkp2283572185/MyStd/blob/master/MyIntegral/MyItoA.hpp
*/

#include"dec.h"
#include"hex.h"

namespace fast_io::details::uprsv
{

template<::std::size_t base,bool uppercase,::std::integral char_type,::fast_io::details::my_unsigned_integral uinttype>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr char_type* uprsv_main(char_type *iter,uinttype value) noexcept
{
	if constexpr(base==10)
	{
		using namespace ::fast_io::details::uprsv::decbase;
		if constexpr(sizeof(uinttype)<=sizeof(::std::uint_least32_t))
		{
			return uprsv32_impl(iter,static_cast<::std::uint_least32_t>(value));
		}
		else
#ifdef __SIZEOF_INT128__
		if constexpr(sizeof(uinttype)<=sizeof(::std::uint_least64_t))
#endif
		{
			return uprsv64_impl(iter,static_cast<::std::uint_least64_t>(value));
		}
#ifdef __SIZEOF_INT128__
		else 
		{
			return uprsv128_impl(iter,static_cast<__uint128_t>(value));
		}
#endif
	}
	else
	{
		using namespace ::fast_io::details::uprsv::hexbase;
		if constexpr(sizeof(uinttype)<=sizeof(::std::uint_least32_t))
		{
			return uprsv_hex32_impl<uppercase>(iter,static_cast<::std::uint_least32_t>(value));
		}
		else
#ifdef __SIZEOF_INT128__
		if constexpr(sizeof(uinttype)<=sizeof(::std::uint_least64_t))
#endif
		{
			return uprsv_hex64_impl<uppercase>(iter,static_cast<::std::uint_least64_t>(value));
		}
#ifdef __SIZEOF_INT128__
		else 
		{
			return uprsv_hex128_impl<uppercase>(iter,static_cast<__uint128_t>(value));
		}
#endif
	}
}

}
