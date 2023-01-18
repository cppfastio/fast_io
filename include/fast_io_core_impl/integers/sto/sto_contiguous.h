#pragma once
#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(push)
#pragma warning( disable : 4061 )
#endif
#include"sto_generate_base_tb.h"

namespace fast_io
{

namespace details
{

inline constexpr bool sto_use_table{true};

template<char8_t base,std::integral char_type>
requires (2<=base&&base<=36)
inline constexpr bool char_digit_to_literal(my_make_unsigned_t<char_type>& ch) noexcept
{
	using unsigned_char_type = my_make_unsigned_t<char_type>;
	constexpr bool ebcdic{::fast_io::details::is_ebcdic<char_type>};
	if constexpr(::std::same_as<char_type,wchar_t>&&::fast_io::details::wide_is_none_utf_endian)
	{
		ch=static_cast<char_type>(::fast_io::byte_swap(static_cast<unsigned_char_type>(ch)));
	}
	if constexpr(base<=10)
	{
		constexpr unsigned_char_type base_char_type(base);
		if constexpr(ebcdic)
			ch-=static_cast<unsigned_char_type>(240);
		else
			ch-=static_cast<unsigned_char_type>(u8'0');
		return base_char_type<=ch;
	}
	else
	{
		if constexpr(sto_use_table)
		{
			constexpr char8_t mx{std::numeric_limits<char8_t>::max()};
			constexpr bool use_partial{mx<(std::numeric_limits<unsigned_char_type>::max())||std::numeric_limits<char8_t>::digits!=8};
			if constexpr(use_partial)
			{
				constexpr std::size_t n{sto_base_tb<ebcdic,use_partial,base>.size()};
				static_assert(n<=mx);
				constexpr char8_t v{static_cast<char8_t>(n)};
				if(v<ch)
				{
					return true;
				}
			}
			char8_t ret{sto_base_tb<ebcdic,use_partial,base>[static_cast<unsigned_char_type>(ch)]};
			if(ret==mx)
			{
				return true;
			}
			ch=static_cast<unsigned_char_type>(ret);
			return false;
		}
		else if constexpr(ebcdic)
		{

			if constexpr(base<=19)
			{
				constexpr unsigned_char_type mns{base-10};
				unsigned_char_type ch2(ch);
				ch2-=0xC1;
				unsigned_char_type ch3(ch);
				ch3-=0x81;
				ch-=0xF0;
				if(ch2<mns)
					ch=ch2+static_cast<unsigned_char_type>(10);
				else if(ch3<mns)
					ch=ch3+static_cast<unsigned_char_type>(10);
				else if(10<=ch)
					return true;
				return false;
			}
			else if constexpr(base<=28)
			{
				constexpr unsigned_char_type mns{base-19};
				unsigned_char_type ch2(ch);
				ch2-=0xC1;
				unsigned_char_type ch3(ch);
				ch3-=0x81;
				unsigned_char_type ch4(ch);
				ch2-=0xD1;
				unsigned_char_type ch5(ch);
				ch3-=0x91;
				ch-=0xF0;
				if(ch4<mns)
					ch=ch4+static_cast<unsigned_char_type>(19);
				else if(ch5<mns)
					ch=ch5+static_cast<unsigned_char_type>(19);
				else if(ch2<9)
					ch=ch2+static_cast<unsigned_char_type>(10);
				else if(ch3<9)
					ch=ch3+static_cast<unsigned_char_type>(10);
				else if(10<=ch)
					return true;
				return false;
			}
			else
			{
				constexpr unsigned_char_type mns{base-27};
				unsigned_char_type ch2(ch);
				ch2-=0xC1;
				unsigned_char_type ch3(ch);
				ch3-=0x81;
				unsigned_char_type ch4(ch);
				ch4-=0xD1;
				unsigned_char_type ch5(ch);
				ch5-=0x91;
				unsigned_char_type ch6(ch);
				ch6-=0xE2;
				unsigned_char_type ch7(ch);
				ch7-=0xA2;
				ch-=0xF0;
				if(ch6<mns)
					ch=ch6+static_cast<unsigned_char_type>(27);
				else if(ch7<mns)
					ch=ch7+static_cast<unsigned_char_type>(27);
				else if(ch4<9)
					ch=ch4+static_cast<unsigned_char_type>(19);
				else if(ch5<9)
					ch=ch5+static_cast<unsigned_char_type>(19);
				else if(ch2<9)
					ch=ch2+static_cast<unsigned_char_type>(10);
				else if(ch3<9)
					ch=ch3+static_cast<unsigned_char_type>(10);
				else if(10<=ch)
					return true;
				return false;
			}
		}
		else
		{
			constexpr unsigned_char_type mns{base-10};
			unsigned_char_type ch2(ch);
			ch2-=u8'A';
			unsigned_char_type ch3(ch);
			ch3-=u8'a';
			ch-=u8'0';
			if(ch2<mns)
				ch=ch2+static_cast<unsigned_char_type>(10);
			else if(ch3<mns)
				ch=ch3+static_cast<unsigned_char_type>(10);
			else if(10<=ch)
				return true;
			return false;
		}
	}
}

template<char8_t base,std::integral char_type>
requires (2<=base&&base<=36)
inline constexpr bool char_is_digit(my_make_unsigned_t<char_type> ch) noexcept
{
	using unsigned_char_type = my_make_unsigned_t<char_type>;
	constexpr bool ebcdic{::fast_io::details::is_ebcdic<char_type>};
	constexpr unsigned_char_type base_char_type(base);
	if constexpr(::std::same_as<char_type,wchar_t>&&::fast_io::details::wide_is_none_utf_endian)
	{
		ch=static_cast<char_type>(::fast_io::byte_swap(static_cast<unsigned_char_type>(ch)));
	}
	if constexpr(base<=10)
	{
		if constexpr(ebcdic)
			ch-=static_cast<unsigned_char_type>(240);
		else
			ch-=static_cast<unsigned_char_type>(u8'0');
		return ch<base_char_type;
	}
	else
	{
		if constexpr(sto_use_table)
		{
			constexpr char8_t mx{std::numeric_limits<char8_t>::max()};
			constexpr bool use_partial{mx<(std::numeric_limits<unsigned_char_type>::max())||std::numeric_limits<char8_t>::digits!=8};
			if constexpr(use_partial)
			{
				constexpr std::size_t n{sto_base_tb<ebcdic,use_partial,base>.size()};
				static_assert(n<=mx);
				constexpr char8_t v{static_cast<char8_t>(n)};
				if(v<ch)
				{
					return false;
				}
			}
			auto ret{sto_base_tb<ebcdic,use_partial,base>[static_cast<unsigned_char_type>(ch)]};
			if(ret==mx)
			{
				return false;
			}
			return true;
		}
		else if constexpr(ebcdic)
		{
			if constexpr(base<=19)
			{
				constexpr unsigned_char_type mns{base-10};
				unsigned_char_type ch2(ch);
				ch2-=0xC1;
				unsigned_char_type ch3(ch);
				ch3-=0x81;
				ch-=0xF0;
				return (ch2<mns)|(ch3<mns)|(ch<10u);
			}
			else if constexpr(base<=28)
			{
				constexpr unsigned_char_type mns{base-19};
				unsigned_char_type ch2(ch);
				ch2-=0xC1;
				unsigned_char_type ch3(ch);
				ch3-=0x81;
				unsigned_char_type ch4(ch);
				ch2-=0xD1;
				unsigned_char_type ch5(ch);
				ch3-=0x91;
				ch-=0xF0;
				return (ch4<mns)|(ch5<mns)|(ch2<mns)|(ch3<9)|(ch<10u);
			}
			else
			{
				constexpr unsigned_char_type mns{base-27};
				unsigned_char_type ch2(ch);
				ch2-=0xC1;
				unsigned_char_type ch3(ch);
				ch3-=0x81;
				unsigned_char_type ch4(ch);
				ch4-=0xD1;
				unsigned_char_type ch5(ch);
				ch5-=0x91;
				unsigned_char_type ch6(ch);
				ch6-=0xE2;
				unsigned_char_type ch7(ch);
				ch7-=0xA2;
				ch-=0xF0;
				return (ch6<mns)|(ch7<mns)|(ch4<9u)|(ch5<9u)|(ch2<9u)|(ch3<9u)|(ch<10u);
			}
		}
		else
		{
			constexpr unsigned_char_type mns{base-10};
			unsigned_char_type ch2(ch);
			ch2-=u8'A';
			unsigned_char_type ch3(ch);
			ch3-=u8'a';
			ch-=u8'0';
			return (ch2<mns)|(ch3<mns)|(ch<10u);
		}
	}
}

template<std::integral char_type>
inline constexpr char_type const* find_none_zero_simd_impl(char_type const* first,char_type const* last) noexcept;

struct simd_parse_result
{
	std::size_t digits;
	fast_io::parse_code code;
};

inline constexpr char unsigned simd16_shift_table[32]{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

#if defined(__SSE4_1__) && defined(__x86_64__)

template<bool char_execharset>
inline std::uint_least32_t detect_length(char unsigned const* buffer) noexcept
{
	constexpr char8_t zero_constant{char_execharset?static_cast<char8_t>('0'):u8'0'};
	constexpr char8_t v176_constant{static_cast<char8_t>((zero_constant+static_cast<char8_t>(128))&255u)};
#if (defined(__GNUC__) || defined(__clang__)) && !defined(__INTEL_COMPILER)
	using namespace fast_io::intrinsics;
	x86_64_v16qu chunk;
	__builtin_memcpy(__builtin_addressof(chunk),buffer,16);
	x86_64_v16qu const v176{v176_constant,v176_constant,v176_constant,v176_constant,v176_constant,v176_constant,v176_constant,v176_constant,
				v176_constant,v176_constant,v176_constant,v176_constant,v176_constant,v176_constant,v176_constant,v176_constant};
	x86_64_v16qu const t0{chunk-v176};
	x86_64_v16qs const minus118{-118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118};
	x86_64_v16qs const mask{(x86_64_v16qs)t0<minus118};
	std::uint_least16_t v{static_cast<std::uint_least16_t>(__builtin_ia32_pmovmskb128((x86_64_v16qi)mask))};
#else
	__m128i chunk = _mm_loadu_si128(reinterpret_cast<__m128i const*>(buffer));
	__m128i const t0 = _mm_sub_epi8(chunk, _mm_set1_epi8(v176_constant));
	__m128i const mask = _mm_cmplt_epi8(t0, _mm_set1_epi8(-118));
	std::uint_least16_t v{static_cast<std::uint_least16_t>(_mm_movemask_epi8(mask))};
#endif
	return static_cast<std::uint_least32_t>(std::countr_one(v));
}

template<bool char_execharset>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline std::size_t sse_skip_long_overflow_digits(char unsigned const* buffer,char unsigned const* buffer_end) noexcept
{
	auto it{buffer+16};
	for(;16<=buffer_end-it;it+=16)
	{
		auto new_length{detect_length<char_execharset>(it)};
		if(new_length!=16)
			return static_cast<std::size_t>(it-buffer+new_length);
	};
	if(buffer_end==it)
		return static_cast<std::size_t>(it-buffer);
	return static_cast<std::size_t>(buffer_end-buffer+detect_length<char_execharset>(buffer_end-16));
}

template<bool char_execharset,bool less_than_64_bits>
#if __has_cpp_attribute(__gnu__::__hot__)
[[__gnu__::__hot__]]
#endif
inline simd_parse_result sse_parse(char unsigned const* buffer,char unsigned const* buffer_end,std::uint_least64_t &res) noexcept
{
	constexpr char8_t zero_constant{char_execharset?static_cast<char8_t>('0'):u8'0'};
	constexpr char8_t v176_constant{static_cast<char8_t>((zero_constant+static_cast<char8_t>(128))&255u)};
	using fast_io::parse_code;
#if (defined(__GNUC__) || defined(__clang__)) && !defined(__INTEL_COMPILER)
	using namespace fast_io::intrinsics;
	x86_64_v16qu chunk;
	__builtin_memcpy(__builtin_addressof(chunk),buffer,16);
	x86_64_v16qu const v176{v176_constant,v176_constant,v176_constant,v176_constant,v176_constant,v176_constant,v176_constant,v176_constant,
				v176_constant,v176_constant,v176_constant,v176_constant,v176_constant,v176_constant,v176_constant,v176_constant};
	x86_64_v16qu const t0{chunk-v176};
	x86_64_v16qs const minus118{-118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118, -118};
	x86_64_v16qs const mask{(x86_64_v16qs)t0<minus118};
	std::uint_least16_t v{static_cast<std::uint_least16_t>(__builtin_ia32_pmovmskb128((x86_64_v16qi)mask))};
	std::uint_least32_t digits{static_cast<std::uint_least32_t>(std::countr_one(v))};
	if(digits==0)
		return {0,parse_code::invalid};
	x86_64_v16qu const zeros{zero_constant,zero_constant,zero_constant,zero_constant,zero_constant,zero_constant,zero_constant,zero_constant,
		zero_constant,zero_constant,zero_constant,zero_constant,zero_constant,zero_constant,zero_constant,zero_constant};
	chunk-=zeros;
	x86_64_v16qi shuffle_mask;
	__builtin_memcpy(__builtin_addressof(shuffle_mask),simd16_shift_table+digits,sizeof(x86_64_v16qi));
	chunk=(x86_64_v16qu)__builtin_ia32_pshufb128((x86_64_v16qi)chunk,shuffle_mask);
	chunk=(x86_64_v16qu)__builtin_ia32_pmaddubsw128((x86_64_v16qi)chunk,x86_64_v16qi{10,1,10,1,10,1,10,1,10,1,10,1,10,1,10,1});
	chunk=(x86_64_v16qu)__builtin_ia32_pmaddwd128((x86_64_v8hi)chunk,x86_64_v8hi{100,1,100,1,100,1,100,1});
	chunk=(x86_64_v16qu)__builtin_ia32_packusdw128((x86_64_v4si)chunk,(x86_64_v4si)chunk);
	chunk=(x86_64_v16qu)__builtin_ia32_pmaddwd128((x86_64_v8hi)chunk,x86_64_v8hi{10000,1,10000,1,0,0,0,0});
	std::uint_least64_t chunk0;
	__builtin_memcpy(__builtin_addressof(chunk0),__builtin_addressof(chunk),sizeof(chunk0));
#else
	__m128i chunk = _mm_loadu_si128(reinterpret_cast<__m128i const*>(buffer));
	__m128i const t0 = _mm_sub_epi8(chunk, _mm_set1_epi8(v176_constant));
	__m128i const mask = _mm_cmplt_epi8(t0, _mm_set1_epi8(-118));
	std::uint_least16_t v{static_cast<std::uint_least16_t>(_mm_movemask_epi8(mask))};
	std::uint_least32_t digits{static_cast<std::uint_least32_t>(std::countr_one(v))};
	if(digits==0)
		return {0,parse_code::invalid};
	chunk = _mm_sub_epi8(chunk, _mm_set1_epi8(zero_constant));
	chunk = _mm_shuffle_epi8(chunk,_mm_loadu_si128(reinterpret_cast<__m128i const*>(simd16_shift_table+digits)));
	chunk = _mm_maddubs_epi16(chunk, _mm_set_epi8(1,10,1,10,1,10,1,10,1,10,1,10,1,10,1,10));
	chunk = _mm_madd_epi16(chunk, _mm_set_epi16(1,100,1,100,1,100,1,100));
	chunk = _mm_packus_epi32(chunk, chunk);
	chunk = _mm_madd_epi16(chunk,_mm_set_epi16(0,0,0,0,1,10000,1,10000));
	std::uint_least64_t chunk0;
	std::memcpy(__builtin_addressof(chunk0),__builtin_addressof(chunk),sizeof(chunk0));
#endif
	std::uint_least64_t result{static_cast<std::uint_least64_t>(((chunk0 & 0xffffffff) * UINT64_C(100000000)) + (chunk0 >> 32))};
	if(digits==16)[[unlikely]]
	{
		if constexpr(less_than_64_bits)
		{
			//std::uint_least32_t can never have 16 digits
			return {sse_skip_long_overflow_digits<char_execharset>(buffer+16,buffer_end)+16,parse_code::overflow};
		}
		else
		{
			std::size_t digits1{detect_length<char_execharset>(buffer+16)};
//18446744073709551615 20 digits
			switch(digits1)
			{
			case 3:
			{
				res=result*UINT16_C(1000)+((buffer[16]-zero_constant)*UINT16_C(100)+(buffer[17]-zero_constant)*UINT16_C(10)
				+(buffer[18]-zero_constant));
				return {19,parse_code::ok};
			}
			case 2:
			{
				res=result*UINT16_C(100)+((buffer[16]-zero_constant)*UINT16_C(10)+static_cast<std::uint_least64_t>(buffer[17]-zero_constant));
				return {18,parse_code::ok};
			}
			case 1:
			{
				res=result*UINT16_C(10)+(buffer[16]-zero_constant);
				return {17,parse_code::ok};
			}
			case 0:
			{
				res=result;
				return {16,parse_code::ok};
			}
			case 4:
			{
				constexpr std::uint_least64_t risky_value{UINT_LEAST64_MAX/UINT64_C(10000)};
				constexpr std::uint_fast16_t risky_mod{UINT_LEAST64_MAX%UINT64_C(10000)};
				if(result>risky_value)
					return {20,parse_code::overflow};
				std::uint_fast16_t partial{
				static_cast<std::uint_fast16_t>(
				static_cast<std::uint_fast16_t>(buffer[16]-zero_constant)*UINT16_C(1000)+
				static_cast<std::uint_fast8_t>(buffer[17]-zero_constant)*UINT16_C(100)+
				static_cast<std::uint_fast8_t>(buffer[18]-zero_constant)*UINT16_C(10)+
				static_cast<std::uint_fast8_t>(buffer[19]-zero_constant))};
				if(result==risky_value&&risky_mod<partial)
					return {20,parse_code::overflow};
				res=result*UINT16_C(10000)+partial;
				return {20,parse_code::ok};
			}
			case 16:
			{
				digits1=sse_skip_long_overflow_digits<char_execharset>(buffer+16,buffer_end);
				[[fallthrough]];
			}
			default:
			{
				return {digits1+16,parse_code::overflow};
			}
			}
		}
	}
	res=result;
	return {digits,parse_code::ok};
}

#endif

template<char8_t base,::std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr char_type const* skip_digits(char_type const* first,char_type const* last) noexcept
{
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	for(;first!=last&&char_is_digit<base,char_type>(static_cast<unsigned_char_type>(*first));++first);
	return first;
}

template<char8_t base,::std::integral char_type,my_unsigned_integral T>
#if defined(__SSE4_1__) && __has_cpp_attribute(__gnu__::__cold__) && defined(__x86_64__)
[[__gnu__::__cold__]]
#endif
inline constexpr parse_result<char_type const*> scan_int_contiguous_none_simd_space_part_define_impl(char_type const* first,char_type const* last,T& res) noexcept
{
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	using unsigned_type = my_make_unsigned_t<std::remove_cvref_t<T>>;
	constexpr unsigned_char_type base_char_type{base};
	constexpr unsigned_type risky_uint_max{static_cast<unsigned_type>(-1)};
	constexpr unsigned_type risky_value{risky_uint_max/base};
	constexpr unsigned_char_type risky_digit(risky_uint_max%base);
	constexpr bool isspecialbase{base==2||base==4||base==16};
	constexpr std::size_t max_size{details::cal_max_int_size<unsigned_type,base>()-(!isspecialbase)};
	std::size_t const diff{static_cast<std::size_t>(last-first)};
	std::size_t mn_val{max_size};
	if(diff<mn_val)
		mn_val=diff;
	auto first_phase_last{first+mn_val};
	for(;first!=first_phase_last;++first)
	{
		unsigned_char_type ch{static_cast<unsigned_char_type>(*first)};
		if(char_digit_to_literal<base,char_type>(ch))[[unlikely]]
			break;
		res*=base_char_type;
		res+=ch;
	}
	bool overflow{};
	if(first!=last)[[likely]]
	{
		unsigned_char_type ch{static_cast<unsigned_char_type>(*first)};
		if constexpr(isspecialbase)
		{
			if(char_is_digit<base,char_type>(ch))
			{
				++first;
				first=skip_digits<base>(first,last);
				overflow=true;
			}
		}
		else
		{
			if(!char_digit_to_literal<base,char_type>(ch))[[unlikely]]
			{
				overflow=res>risky_value||(risky_value==res&&ch>risky_digit);
				if(!overflow)
				{
					res*=base_char_type;
					res+=ch;
				}
				++first;
				if(first!=last&&char_is_digit<base,char_type>(static_cast<unsigned_char_type>(*first)))
				{
					++first;
					first=skip_digits<base>(first,last);
					overflow=true;
				}
			}
		}
	}
	return {first,(overflow?(parse_code::overflow):(parse_code::ok))};
}

inline constexpr parse_code ongoing_parse_code{static_cast<parse_code>(std::numeric_limits<char unsigned>::max())};

template<char8_t base,std::integral char_type>
inline constexpr parse_result<char_type const*> scan_shbase_impl(char_type const* first,char_type const* last) noexcept
{
	if(first==last||*first!=char_literal_v<u8'0',char_type>)
	{
		return {first,parse_code::invalid};
	}
	if((++first)==last)
	{
		return {first,parse_code::invalid};
	}
	if constexpr(base==2||base==3||base==16)
	{
		auto ch{*first};
		if((ch!=char_literal_v<(base==2?u8'B':(base==3?u8't':u8'X')),char_type>)&
			(ch!=char_literal_v<(base==2?u8'b':(base==3?u8't':u8'x')),char_type>))
		{
			return {first,parse_code::invalid};
		}
		++first;
	}
	else
	{
		if(*first!=char_literal_v<u8'[',char_type>)
		{
			return {first,parse_code::invalid};
		}
		++first;
		if((++first)==last)
		{
			return {first,parse_code::invalid};
		}
		constexpr auto digit0{char_literal_v<
		u8'0'+(base<10?base:base/10),char_type>};
		if(*first!=digit0)
		{
			return {first,parse_code::invalid};
		}
		if((++first)==last)
		{
			return {first,parse_code::invalid};
		}
		if constexpr(10<base)
		{
			constexpr auto digit1{char_literal_v<
			u8'0'+(base%10),char_type>};
			if(*first!=digit1)
			{
				return {first,parse_code::invalid};
			}
			if((++first)==last)
			{
				return {first,parse_code::invalid};
			}
		}
		if(*first!=char_literal_v<u8']',char_type>)
		{
			return {first,parse_code::invalid};
		}
	}
	return {first,ongoing_parse_code};
}

template<std::integral char_type>
inline constexpr char_type const* skip_hexdigits(char_type const* first,char_type const* last) noexcept;

template<char8_t base,bool shbase=false,bool skipzero=false,::std::integral char_type,my_integral T>
inline constexpr parse_result<char_type const*> scan_int_contiguous_none_space_part_define_impl(char_type const* first,char_type const* last,T& t) noexcept
{
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	[[maybe_unused]] bool sign{};
	if constexpr(my_signed_integral<T>)
	{
		if(first==last)
			return {first,parse_code::invalid};
		constexpr auto minus_sign{char_literal_v<u8'-',char_type>};
		if((sign=(minus_sign==*first)))
			++first;
		if constexpr(shbase&&base!=10)
		{
			if constexpr(base==8)
			{
				if(first==last||*first!=char_literal_v<u8'0',char_type>)
				{
					return {first,parse_code::invalid};
				}
				++first;
			}
			else
			{
				auto phase_ret = scan_shbase_impl<base>(first,last);
				if(phase_ret.code!=ongoing_parse_code)
				{
					return phase_ret;
				}
				first=phase_ret.iter;
			}
		}
	}
	constexpr auto zero{char_literal_v<u8'0',char_type>};
	if(first!=last)
	{
		auto first_ch{*first};
		if(!char_is_digit<base,char_type>(static_cast<unsigned_char_type>(first_ch)))[[unlikely]]
			return {first,parse_code::invalid};
		else if(first_ch==zero)
		{
			if constexpr(skipzero)
			{
				++first;
				first=::fast_io::details::find_none_zero_simd_impl(first,last);
				if(first==last)
				{
					t=0;
					return {first,parse_code::ok};
				}
			}
			else
			{
				++first;
				if((first==last)||(!char_is_digit<base,char_type>(static_cast<unsigned_char_type>(*first))))
				{
					t={};
					return {first,parse_code::ok};
				}
				return {first,parse_code::invalid};
			}
		}
	}
	using unsigned_type = my_make_unsigned_t<std::remove_cvref_t<T>>;
	unsigned_type res{};
	char_type const* it;
#if defined(__SSE4_1__) && defined(__x86_64__)
	if constexpr(base==10&&sizeof(char_type)==1&&sizeof(unsigned_type)<=sizeof(std::uint_least64_t))
	{
		if(
#if __cpp_lib_is_constant_evaluated >= 201811L
		!std::is_constant_evaluated()&&
#endif
		last-first>=32)[[likely]]
		{
			constexpr bool smaller_than_uint64{sizeof(unsigned_type)<sizeof(std::uint_least64_t)};
			std::uint_least64_t temp{};
			auto [digits,ec]=sse_parse<is_ebcdic<char_type>,smaller_than_uint64>(reinterpret_cast<char unsigned const*>(first),reinterpret_cast<char unsigned const*>(last),temp);
			it=first+digits;
			if(ec!=parse_code::ok)[[unlikely]]
				return {it,ec};
			if constexpr(smaller_than_uint64)
			{
				constexpr unsigned_type umax{static_cast<unsigned_type>(-1)};
				if(temp>umax)
					return {it,parse_code::overflow};
				res=static_cast<unsigned_type>(temp);
			}
			else
			{
				res=temp;
			}
		}
		else[[unlikely]]
		{
			auto [it2,ec]=scan_int_contiguous_none_simd_space_part_define_impl<base>(first,last,res);
			if(ec!=parse_code::ok)
				return {it2,ec};
			it=it2;
		}
	}
	else
#endif
	{
		auto [it2,ec]=scan_int_contiguous_none_simd_space_part_define_impl<base>(first,last,res);
		if(ec!=parse_code::ok)
			return {it2,ec};
		it=it2;
	}
	if constexpr(my_signed_integral<T>)
	{
		constexpr unsigned_type umax{static_cast<unsigned_type>(-1)};
		constexpr unsigned_type imax{umax>>1};
		if(res>(static_cast<my_make_unsigned_t<T>>(imax)+sign))
			return {it,parse_code::overflow};
		if(sign)
			t=static_cast<T>(static_cast<unsigned_type>(0)-res);
		else
			t=static_cast<T>(res);
	}
	else
	{
		t=res;
	}
	return {it,parse_code::ok};
}

template<char8_t base,bool noskipws,bool shbase,bool skipzero,::std::integral char_type,details::my_integral T>
inline constexpr parse_result<char_type const*> scan_int_contiguous_define_impl(char_type const* first,char_type const* last,T& t) noexcept
{
	if constexpr(!noskipws)
	{
		first=::fast_io::details::find_space_common_impl<false,true>(first,last);
		if(first==last)
			return {first,parse_code::end_of_file};
	}
	if constexpr(my_unsigned_integral<T>)
	{
		if constexpr(shbase&&base!=10)
		{
			if constexpr(base==8)
			{
				if(first==last||*first!=char_literal_v<u8'0',char_type>)
				{
					return {first,parse_code::invalid};
				}
				++first;
			}
			else
			{
				auto phase_ret = scan_shbase_impl<base>(first,last);
				if(phase_ret.code!=ongoing_parse_code)
				{
					return phase_ret;
				}
				first=phase_ret.iter;
			}
		}
	}
	return scan_int_contiguous_none_space_part_define_impl<base,((shbase&&base!=10)&&my_signed_integral<T>),skipzero>(first,last,t);
}
}

enum class scan_integral_context_phase:std::uint_least8_t
{
space,
sign,
prefix,
zero,
zero_skip,
zero_invalid,
digit,
overflow
};

namespace details
{
template<char8_t base,std::integral char_type,
	::fast_io::details::my_integral T>
inline constexpr auto scan_context_type_impl_int() noexcept
{
	using unsigned_type = details::my_make_unsigned_t<std::remove_cvref_t<T>>;
	constexpr std::size_t max_size{(::fast_io::details::print_integer_reserved_size_cache<base,false,
	::fast_io::details::my_signed_integral<T>,unsigned_type>)+2};
	struct scan_integer_context
	{
		::fast_io::freestanding::array<char_type,max_size> buffer;
		std::uint_least8_t size{};
		scan_integral_context_phase integer_phase{};
		inline constexpr void reset() noexcept
		{
			size=0;
			integer_phase=scan_integral_context_phase::space;
		}
	};
	return io_type_t<scan_integer_context>{};
}
}


namespace details
{

template<::std::integral char_type>
inline constexpr parse_result<char_type const*> sc_int_ctx_space_phase(char_type const* first,char_type const* last) noexcept
{
	first=::fast_io::details::find_space_common_impl<false,true>(first,last);
	if(first==last)
		return {first,parse_code::partial};
	return {first,ongoing_parse_code};
}

template<bool allow_negative,bool allow_positive,typename State,std::integral char_type>
inline constexpr parse_result<char_type const*> sc_int_ctx_sign_phase(State& st,char_type const* first,char_type const* last) noexcept
{
	if(first==last)
	{
		st.integer_phase=scan_integral_context_phase::sign;
		return {first,parse_code::partial};
	}
	if constexpr(allow_negative)
	{
		if constexpr(allow_positive)
		{
			auto ch{*first};
			if(ch==char_literal_v<u8'-',char_type>)
			{
				*st.buffer.data()=ch;
				st.size=1;
				++first;
			}
			else if(ch==char_literal_v<u8'+',char_type>)
			{
				++first;
			}
		}
		else
		{
			if(*first==char_literal_v<u8'-',char_type>)
			{
				*st.buffer.data()=char_literal_v<u8'-',char_type>;
				st.size=1;
				++first;
			}
		}
	}
	else
	{
		if constexpr(allow_positive)
		{
			auto ch{*first};
			if(ch==char_literal_v<u8'+',char_type>)
			{
				++first;
			}
		}
	}
	return {first,ongoing_parse_code};
}

template<char8_t base,std::integral char_type>
requires (base!=10)
inline constexpr parse_result<char_type const*> sc_int_ctx_prefix_phase(
	std::uint_least8_t& sz,char_type const* first,char_type const* last) noexcept
{
	if(first==last)
	{
		return {first,parse_code::partial};
	}
	if constexpr(base==8)
	{
		if(*first!=char_literal_v<u8'0',char_type>)
		{
			return {first,parse_code::invalid};
		}

	}
	else
	{
		std::uint_least8_t size_cache{sz};
		if(size_cache==0)
		{
			if(*first!=char_literal_v<u8'0',char_type>)
			{
				return {first,parse_code::invalid};
			}
			if((++first)==last)
			{
				sz=1;
				return {first,parse_code::partial};
			}
			if constexpr(base!=2&&base!=3&&base!=16)
			{
				size_cache=1;
			}
		}
		if constexpr(base==2||base==3||base==16)
		{
			auto ch{*first};
			if((ch==char_literal_v<(base==2?u8'B':(base==3?u8't':u8'X')),char_type>)|
				(ch==char_literal_v<(base==2?u8'b':(base==3?u8't':u8'x')),char_type>))
			{
				sz=0;
				++first;
				return {first,ongoing_parse_code};
			}
			else
			{
				return {first,parse_code::invalid};
			}
		}
		else
		{
			if(size_cache==1)
			{
				if(*first!=char_literal_v<u8'[',char_type>)
				{
					return {first,parse_code::invalid};
				}
				if((++first)==last)
				{
					sz=2;
					return {first,parse_code::partial};
				}
			}
			constexpr auto digit0{char_literal_v<
			u8'0'+(base<10?base:base/10),char_type>};
			if(size_cache==2)
			{
				if(*first!=digit0)
				{
					return {first,parse_code::invalid};
				}
				if((++first)==last)
				{
					sz=3;
					return {first,parse_code::partial};
				}
			}
			if constexpr(10<base)
			{
				constexpr auto digit1{char_literal_v<
				u8'0'+(base%10),char_type>};
				if(size_cache==3)
				{
					if(*first!=digit1)
					{
						return {first,parse_code::invalid};
					}
					if((++first)==last)
					{
						sz=4;
						return {first,parse_code::partial};
					}
				}
			}
			constexpr
				::std::uint_least8_t last_index{base<10?3:4};
			if(size_cache==last_index)
			{
				if(*first!=char_literal_v<u8']',char_type>)
				{
					return {first,parse_code::invalid};
				}
				sz=0;
				++first;
			}
		}
	}
	return {first,ongoing_parse_code};
}

template<char8_t base,bool skipzero,std::integral char_type>
inline constexpr parse_result<char_type const*> sc_int_ctx_zero_phase(scan_integral_context_phase& integer_phase,char_type const* first,char_type const* last) noexcept
{
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	if(first==last)
	{
		integer_phase=scan_integral_context_phase::zero;
		return {first,parse_code::partial};
	}
	constexpr auto zero{char_literal_v<u8'0',char_type>};
	auto first_ch{*first};
	if(!char_is_digit<base,char_type>(static_cast<unsigned_char_type>(first_ch)))[[unlikely]]
	{
		return {first,parse_code::invalid};
	}
	else if(first_ch==zero)[[unlikely]]
	{
		++first;
		if constexpr(skipzero)
		{
			first=find_none_zero_simd_impl(first,last);
		}
		if(first==last)
		{
			if constexpr(skipzero)
			{
				integer_phase=scan_integral_context_phase::zero_skip;
			}
			else
			{
				integer_phase=scan_integral_context_phase::zero_invalid;
			}
			return {first,parse_code::partial};
		}
		if(!char_is_digit<base,char_type>(static_cast<unsigned_char_type>(*first)))
		{
			return {first,parse_code::ok};
		}
		if constexpr(!skipzero)
		{
			return {first,parse_code::invalid};
		}
	}
	return {first,ongoing_parse_code};
}


template<char8_t base,std::integral char_type,typename State,my_integral T>
inline constexpr parse_result<char_type const*> sc_int_ctx_digit_phase(State& st,char_type const* first,char_type const* last,T& t) noexcept
{
	auto it{skip_digits<base>(first,last)};
	std::size_t const diff{st.buffer.size()-static_cast<std::size_t>(st.size)};
	std::size_t const first_it_diff{static_cast<std::size_t>(it-first)};
	if(first_it_diff<diff)
	{
		auto start{st.buffer.data()+st.size};
		auto e{non_overlapped_copy_n(first,first_it_diff,start)};
		st.size+=static_cast<std::uint_least8_t>(first_it_diff);
		if(it==last)
		{
			st.integer_phase=scan_integral_context_phase::digit;
			return {it,parse_code::partial};
		}
		if(st.size==0)
		{
			t={};
			return {it,parse_code::ok};
		}
		auto [p,ec]=scan_int_contiguous_none_space_part_define_impl<base>(st.buffer.data(),e,t);
		return {p-start+first,ec};
	}
	else
	{
		if(it==last)
		{
			st.integer_phase=scan_integral_context_phase::overflow;
			return {it,parse_code::partial};
		}
		else
		{
			return {it,parse_code::overflow};
		}
	}
}

template<char8_t base,std::integral char_type>
inline constexpr parse_result<char_type const*> sc_int_ctx_zero_invalid_phase(char_type const* first,char_type const* last) noexcept
{
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	if(first==last)
		return {first,parse_code::partial};
	++first;
	if(!char_is_digit<base,char_type>(static_cast<unsigned_char_type>(*first)))
	{
		return {first,parse_code::ok};
	}
	return {first,parse_code::invalid};
}

template<char8_t base,std::integral char_type>
inline constexpr parse_result<char_type const*> sc_int_ctx_skip_digits_phase(char_type const* first,char_type const* last) noexcept
{
	first=skip_digits<base>(first,last);
	return {first,(first==last)?parse_code::partial:parse_code::invalid};
}

template<char8_t base,bool noskipws,bool shbase,bool skipzero,typename State,std::integral char_type,my_integral T>
inline constexpr parse_result<char_type const*> scan_context_define_parse_impl(State& st,char_type const* first,char_type const* last,T& t) noexcept
{
	auto phase{st.integer_phase};
#if __has_cpp_attribute(assume)
	if constexpr(noskipws)
	{
		[[assume(phase!=scan_integral_context_phase::space)]];
	}
	if constexpr(my_unsigned_integral<T>)
	{
		[[assume(phase!=scan_integral_context_phase::sign)]];
	}
	if constexpr(!shbase||base==10)
	{
		[[assume(phase!=scan_integral_context_phase::prefix)]];
	}
	if constexpr(skipzero)
	{
		[[assume(phase!=scan_integral_context_phase::zero_invalid)]];
	}
	else
	{
		[[assume(phase!=scan_integral_context_phase::zero_skip)]];
	}
#endif
	switch(phase)
	{
	case scan_integral_context_phase::space:
	{
		if constexpr(!noskipws)
		{
			auto phase_ret = sc_int_ctx_space_phase(first,last);
			if(phase_ret.code!=ongoing_parse_code)
			{
				return phase_ret;
			}
			first=phase_ret.iter;
		}
		[[fallthrough]];
	}
	case scan_integral_context_phase::sign:
	{
		if constexpr(my_signed_integral<T>)
		{
			auto phase_ret = sc_int_ctx_sign_phase<true,false>(st,first,last);
			if(phase_ret.code!=ongoing_parse_code)
			{
				return phase_ret;
			}
			first=phase_ret.iter;
		}
		[[fallthrough]];
	}
	case scan_integral_context_phase::prefix:
	{
		if constexpr(shbase&&base!=10)
		{
			st.integer_phase=scan_integral_context_phase::prefix;
			auto phase_ret = sc_int_ctx_prefix_phase<base>(st.size,first,last);
			if(phase_ret.code!=ongoing_parse_code)
			{
				return phase_ret;
			}
			first=phase_ret.iter;
		}
		[[fallthrough]];
	}
	case scan_integral_context_phase::zero:
	case scan_integral_context_phase::zero_skip:
	{
		auto phase_ret=sc_int_ctx_zero_phase<base,skipzero>(st.integer_phase,first,last);
		if(phase_ret.code!=ongoing_parse_code)
		{
			if constexpr(skipzero)
			{
				if(phase_ret.code==parse_code::ok)
				{
					t={};
				}
				else if(phase_ret.code==parse_code::invalid&&phase==scan_integral_context_phase::zero_skip)
				{
					t={};
					phase_ret.code=parse_code::ok;
				}
			}
			else
			{
				if(phase_ret.code==parse_code::ok)
				{
					t={};
				}
			}
			return phase_ret;
		}
		first=phase_ret.iter;
		[[fallthrough]];
	}
	case scan_integral_context_phase::digit:
	{
		return sc_int_ctx_digit_phase<base>(st,first,last,t);
	}
	case scan_integral_context_phase::zero_invalid:
	{
		if constexpr(skipzero)
		{
			return {first,parse_code::invalid};
		}
		else
		{
			auto phase_ret=sc_int_ctx_zero_invalid_phase<base>(first,last);
			if(phase_ret.code==parse_code::ok)
			{
				t={};
			}
			return phase_ret;
		}
	}
	default:
	{
		return sc_int_ctx_skip_digits_phase<base>(first,last);
	}
	}
}

template<char8_t base,bool noskipws,bool shbase,bool skipzero,typename State,my_integral T>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr parse_code scan_context_eof_define_parse_impl(State& st,T& t) noexcept
{
	auto phase{st.integer_phase};
#if __has_cpp_attribute(assume)
	if constexpr(!skipzero)
	{
		[[assume(phase!=scan_integral_context_phase::zero_skip)]];
	}
#endif
	switch(phase)
	{
	case scan_integral_context_phase::space:
	{
		if constexpr(noskipws)
		{
			return parse_code::invalid;
		}
		else
		{
			return parse_code::end_of_file;
		}
	}
	case scan_integral_context_phase::digit:
		return scan_int_contiguous_none_space_part_define_impl<base>(st.buffer.data(),st.buffer.data()+st.size,t).code;
	case scan_integral_context_phase::overflow:
		return parse_code::overflow;
	case scan_integral_context_phase::zero_skip:
	case scan_integral_context_phase::zero_invalid:
	{
		t={};
		return parse_code::ok;
	}
	default:
		return parse_code::invalid;
	}
}

}


namespace manipulators
{

template<typename char_type>
struct ch_get_t
{
	using manip_tag = manip_tag_t;
	char_type& reference;
};

template<::fast_io::details::my_integral T>
inline constexpr ch_get_t<T&> ch_get(T& reference) noexcept
{
	return {reference};
}

template<std::size_t bs,bool noskipws=false,bool skipzero=false,::fast_io::details::my_integral scalar_type>
inline constexpr scalar_manip_t<::fast_io::details::base_scan_mani_flags_cache<bs,noskipws,false,skipzero>,scalar_type&> base_get(scalar_type& t) noexcept
{
	return {t};
}

template<bool noskipws=false,bool skipzero=false,::fast_io::details::my_integral scalar_type>
inline constexpr scalar_manip_t<::fast_io::details::base_scan_mani_flags_cache<2,noskipws,false,skipzero>,scalar_type&> bin_get(scalar_type& t) noexcept
{
	return {t};
}

template<bool noskipws=false,bool skipzero=false,::fast_io::details::my_integral scalar_type>
inline constexpr scalar_manip_t<::fast_io::details::base_scan_mani_flags_cache<16,noskipws,false,skipzero>,scalar_type&> hex_get(scalar_type& t) noexcept
{
	return {t};
}

template<bool noskipws=false,::fast_io::details::my_unsigned_integral scalar_type>
inline constexpr scalar_manip_t<::fast_io::details::base_scan_mani_flags_cache<16,noskipws,true,true>,scalar_type&> addrvw_get(scalar_type& t) noexcept
{
	return {t};
}

}

template<details::my_integral T>
inline constexpr ::fast_io::manipulators::scalar_manip_t<::fast_io::details::base_scan_mani_flags_cache<10,false,false,false>,T&> scan_alias_define(io_alias_t,T& t) noexcept
{
	return {t};
}

template<std::integral char_type,manipulators::scalar_flags flags,details::my_integral T>
inline constexpr auto scan_context_type(io_reserve_type_t<char_type,::fast_io::manipulators::scalar_manip_t<flags,T&>>) noexcept
{
	return details::scan_context_type_impl_int<flags.base,char_type,T>();
}

template<::std::integral char_type,manipulators::scalar_flags flags,details::my_integral T>
inline constexpr parse_result<char_type const*> scan_contiguous_define(io_reserve_type_t<char_type,::fast_io::manipulators::scalar_manip_t<flags,T&>>,char_type const* begin,char_type const* end,::fast_io::manipulators::scalar_manip_t<flags,T&> t) noexcept
{
	return details::scan_int_contiguous_define_impl<flags.base,flags.noskipws,flags.showbase,flags.full>(begin,end,t.reference);
}

template<::std::integral char_type,manipulators::scalar_flags flags,typename State,details::my_integral T>
inline constexpr parse_result<char_type const*> scan_context_define(io_reserve_type_t<char_type,::fast_io::manipulators::scalar_manip_t<flags,T&>>,State& state,char_type const* begin,char_type const* end,::fast_io::manipulators::scalar_manip_t<flags,T&> t) noexcept
{
	return details::scan_context_define_parse_impl<flags.base,flags.noskipws,flags.showbase,flags.full>(state,begin,end,t.reference);
}

template<::std::integral char_type,manipulators::scalar_flags flags,typename State,details::my_integral T>
inline constexpr parse_code scan_context_eof_define(io_reserve_type_t<char_type,::fast_io::manipulators::scalar_manip_t<flags,T&>>,State& state,::fast_io::manipulators::scalar_manip_t<flags,T&> t) noexcept
{
	return details::scan_context_eof_define_parse_impl<flags.base,flags.noskipws,flags.showbase,flags.full>(state,t.reference);
}

namespace details
{
template<::std::integral char_type>
inline constexpr parse_result<char_type const*> ch_get_context_impl(char_type const* first,char_type const* last,char_type& t) noexcept
{
	first=::fast_io::details::find_space_common_impl<false,true>(first,last);
	if(first==last)
		return {first,parse_code::partial};
	t=*first;
	++first;
	return {first,parse_code::ok};
}
}

template<std::integral char_type>
inline constexpr io_type_t<details::empty> scan_context_type(io_reserve_type_t<char_type,manipulators::ch_get_t<char_type&>>) noexcept
{
	return {};
}

template<std::integral char_type>
inline constexpr parse_result<char_type const*> scan_context_define(io_reserve_type_t<char_type,manipulators::ch_get_t<char_type&>>,details::empty,char_type const* begin,char_type const* end,manipulators::ch_get_t<char_type&> t) noexcept
{
	return details::ch_get_context_impl(begin,end,t.reference);
}

template<std::integral char_type>
inline constexpr parse_code scan_context_eof_define(io_reserve_type_t<char_type,manipulators::ch_get_t<char_type&>>,details::empty,manipulators::ch_get_t<char_type&>) noexcept
{
	return parse_code::end_of_file;
}

}
#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning (pop)
#endif
