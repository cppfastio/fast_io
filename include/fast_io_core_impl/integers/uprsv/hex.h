#pragma once

namespace fast_io::details::uprsv::hexbase
{

template<bool isfirst,bool uppercase,::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr char_type* uprsv_hex_handle8_impl(char_type *iter,::std::uint_least32_t first4) noexcept
{
	constexpr
		::std::uint_least32_t onex2{UINT32_C(0x100)};
	constexpr
		::std::uint_least32_t mask8{onex2-UINT32_C(1)};
	constexpr
		::std::uint_least32_t onex4{UINT32_C(0x10000)};
	constexpr
		::std::uint_least32_t onex6{UINT32_C(0x1000000)};

	constexpr
		auto const *digitstb{digits_table<char_type,16,uppercase>};
	constexpr
		::std::size_t tocopybytes{sizeof(char_type)*2u};
	if constexpr(isfirst)
	{
		if (first4 < onex4)
		{
			if(first4 < onex2)
			{
				bool const lessthan16{first4<UINT32_C(0x10)};
				::fast_io::details::intrinsics::typed_memcpy(iter,digitstb+lessthan16+(first4<<1),tocopybytes);
				iter+=1+(!lessthan16);
			}
			else
			{
				::std::uint_least32_t const f0{static_cast<::std::uint_least32_t>(first4>>8)};
				::std::uint_least32_t const f2{static_cast<::std::uint_least32_t>(f0&mask8)};
				bool const lessthan16{f0<UINT32_C(0x10)};
				::fast_io::details::intrinsics::typed_memcpy(iter,digitstb+lessthan16+(f0<<1),tocopybytes);
				iter+=1+(!lessthan16);
				::fast_io::details::intrinsics::typed_memcpy(iter,digitstb+f2,tocopybytes);
				iter+=2;
			}
		}
		else
		{
			if(first4 < onex6)
			{
				::std::uint_least32_t const f0{static_cast<::std::uint_least32_t>(first4>>16)};
				::std::uint_least32_t const f2{static_cast<::std::uint_least32_t>((first4>>8)&mask8)};
				::std::uint_least32_t const f4{static_cast<::std::uint_least32_t>(first4&mask8)};

				bool const lessthan16{f0<UINT32_C(0x10)};
				::fast_io::details::intrinsics::typed_memcpy(iter,digitstb+lessthan16+(f0<<1),tocopybytes);
				iter+=1+(!lessthan16);
				::fast_io::details::intrinsics::typed_memcpy(iter,digitstb+(f2<<1),tocopybytes);
				::fast_io::details::intrinsics::typed_memcpy(iter+2,digitstb+(f4<<1),tocopybytes);
				iter+=4;
			}
			else
			{
				::std::uint_least32_t const f0{static_cast<::std::uint_least32_t>(first4>>24)};
				::std::uint_least32_t const f2{static_cast<::std::uint_least32_t>((first4>>16)&mask8)};
				::std::uint_least32_t const f4{static_cast<::std::uint_least32_t>((first4>>8)&mask8)};
				::std::uint_least32_t const f6{static_cast<::std::uint_least32_t>(first4&mask8)};

				bool const lessthan16{f0<UINT32_C(0x10)};
				::fast_io::details::intrinsics::typed_memcpy(iter,digitstb+lessthan16+(f0<<1),tocopybytes);
				iter+=1+(!lessthan16);
				::fast_io::details::intrinsics::typed_memcpy(iter,digitstb+(f2<<1),tocopybytes);
				::fast_io::details::intrinsics::typed_memcpy(iter+2,digitstb+(f4<<1),tocopybytes);
				::fast_io::details::intrinsics::typed_memcpy(iter+4,digitstb+(f6<<1),tocopybytes);
				iter+=6;
			}
		}
	}
	else
	{
		::std::uint_least32_t const f0{static_cast<::std::uint_least32_t>(first4>>24)};
		::std::uint_least32_t const f2{static_cast<::std::uint_least32_t>((first4>>16)&mask8)};
		::std::uint_least32_t const f4{static_cast<::std::uint_least32_t>((first4>>8)&mask8)};
		::std::uint_least32_t const f6{static_cast<::std::uint_least32_t>(first4&mask8)};
		::fast_io::details::intrinsics::typed_memcpy(iter,digitstb+(f0<<1),tocopybytes);
		::fast_io::details::intrinsics::typed_memcpy(iter+2,digitstb+(f2<<1),tocopybytes);
		::fast_io::details::intrinsics::typed_memcpy(iter+4,digitstb+(f4<<1),tocopybytes);
		::fast_io::details::intrinsics::typed_memcpy(iter+6,digitstb+(f6<<1),tocopybytes);
		iter+=8;
	}
	return iter;
}

template<bool uppercase,::std::integral char_type>
inline constexpr char_type* uprsv_hex32_impl(char_type *iter,::std::uint_least32_t value) noexcept
{
	return uprsv_hex_handle8_impl<true,uppercase>(iter,value);
}

template<bool uppercase,::std::integral char_type>
inline constexpr char_type* uprsv_hex64_impl(char_type *iter,::std::uint_least64_t value) noexcept
{
	::std::uint_least32_t high;
	::std::uint_least32_t low{::fast_io::details::intrinsics::unpack_generic(value,high)};
	::std::uint_least32_t const first4{high?high:low};
	iter = uprsv_hex_handle8_impl<true,uppercase>(iter,first4);
	if(high)
	{
		iter = uprsv_hex_handle8_impl<false,uppercase>(iter,low);
	}
	return iter;
}

#ifdef __SIZEOF_INT128__

template<bool uppercase,::std::integral char_type>
inline constexpr char_type* uprsv_hex64_notfirst_impl(char_type *iter,::std::uint_least64_t value) noexcept
{
	::std::uint_least32_t high;
	::std::uint_least32_t low{::fast_io::details::intrinsics::unpack_generic(value,high)};
	iter = uprsv_hex_handle8_impl<false,uppercase>(iter,high);
	iter = uprsv_hex_handle8_impl<false,uppercase>(iter,low);
	return iter;
}

template<bool uppercase,::std::integral char_type>
inline constexpr char_type* uprsv_hex128_impl(char_type *iter,__uint128_t value) noexcept
{
	::std::uint_least64_t high;
	::std::uint_least64_t low{::fast_io::details::intrinsics::unpack_generic(value,high)};
	::std::uint_least64_t const first8{high?high:low};
	iter = uprsv_hex64_impl<uppercase>(iter,first8);
	if(high)
	{
		iter = uprsv_hex64_notfirst_impl<uppercase>(iter,low);
	}
	return iter;
}
#endif

}
