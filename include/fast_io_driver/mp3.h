#pragma once

#include<string.h>

namespace fast_io::mp3
{

inline constexpr ::std::uint_least32_t decode_mp3_safe_int(::std::uint_least32_t nval) noexcept
{
	// nVal has 4 bytes (8-bits each)
	//  - discard most significant bit from each byte
	//  - reverse byte order
	//  - concatenate the 4 * 7-bit nibbles into a 24-bit size.
	return static_cast<::std::uint_least32_t>(nval&UINT32_C(0x7F))+
		static_cast<::std::uint_least32_t>(static_cast<::std::uint_least32_t>(nval>>8)&UINT32_C(0x7C))+
		static_cast<::std::uint_least32_t>(static_cast<::std::uint_least32_t>(nval>>16)&UINT32_C(0x7C))+
		static_cast<::std::uint_least32_t>(static_cast<::std::uint_least32_t>(nval>>24)&UINT32_C(0x7C));
}

struct mp3_duration_result
{
	::std::uint_least64_t duration_in_milliseconds{};//milliseconds
	::fast_io::parse_code code{::fast_io::parse_code::ok};
};

inline mp3_duration_result compute_mp3_duration(void const* first,void const* last) noexcept
{
	::std::byte const* firstbyte{reinterpret_cast<::std::byte const*>(first)};
	::std::byte const* lastbyte{reinterpret_cast<::std::byte const*>(last)};
	constexpr
		::std::size_t mp3headersize{10};
	if(static_cast<::std::size_t>(lastbyte-firstbyte)<mp3headersize)
	{
		return {0,::fast_io::parse_code::end_of_file};
	}
	if(::memcmp(firstbyte,u8"ID3",3)!=0)
	{
		return {0,::fast_io::parse_code::end_of_file};
	}
	firstbyte+=mp3headersize;
	::std::uint_least8_t mp3_header_flag;
	::memcpy(__builtin_addressof(mp3_header_flag),firstbyte+6,1);
	constexpr
		::std::size_t mp3extendedheadersize{6};
	if(mp3_header_flag&UINT32_C(0x40))
	{
		if(static_cast<::std::size_t>(lastbyte-firstbyte)<mp3extendedheadersize)
		{
			return {0,::fast_io::parse_code::end_of_file};
		}
		firstbyte+=mp3extendedheadersize;
	}
	constexpr
		::std::uint_least32_t mp3frameheadersize{10};

	for(;mp3frameheadersize<static_cast<std::size_t>(lastbyte-firstbyte);)
	{
		::std::uint_least32_t nval;
		::memcpy(__builtin_addressof(nval),firstbyte+4,4);
		::std::uint_least32_t datalen{decode_mp3_safe_int(nval)};
		firstbyte+=mp3frameheadersize;
		if(static_cast<std::size_t>(lastbyte-firstbyte)<datalen)
		{
			return {0,::fast_io::parse_code::invalid};
		}
		char8_t flag0;
		::memcpy(__builtin_addressof(flag0),firstbyte+8,1);
		if(!::fast_io::char_category::is_c_upper(flag0))
		{
			return {0,::fast_io::parse_code::ok};
		}

		if(::memcmp(firstbyte,u8"TLEN",4)==0)
		{
			if(static_cast<::std::size_t>(lastbyte-firstbyte)<mp3frameheadersize)
			{
				return {0,::fast_io::parse_code::end_of_file};
			}
			firstbyte+=4;
			::std::uint_least32_t tlen;
			using char8constptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= char8_t const*;
			auto code = ::fast_io::parse_by_scan(reinterpret_cast<char8constptr>(firstbyte),
				reinterpret_cast<char8constptr>(lastbyte),tlen).code;
			if(code != ::fast_io::parse_code::ok)
			{
				return {0,code};
			}
			return {tlen};
		}
		else
		{
			firstbyte+=datalen;
		}
	}
	return {0,::fast_io::parse_code::ok};
}

}
