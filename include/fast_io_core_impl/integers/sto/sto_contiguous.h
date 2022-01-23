#pragma once

namespace fast_io
{

namespace details
{

template<char8_t base,std::integral char_type>
requires (2<=base&&base<=36)
inline constexpr bool char_digit_to_literal(my_make_unsigned_t<char_type>& ch) noexcept
{
	using unsigned_char_type = my_make_unsigned_t<char_type>;
	constexpr bool ebcdic{exec_charset_is_ebcdic<char_type>()};
	constexpr unsigned_char_type base_char_type(base);
	if constexpr(base<=10)
	{
		if constexpr(ebcdic)
			ch-=static_cast<unsigned_char_type>(240);
		else
			ch-=static_cast<unsigned_char_type>(u8'0');
		return base_char_type<=ch;
	}
	else
	{
		if constexpr(ebcdic)
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
	constexpr bool ebcdic{exec_charset_is_ebcdic<char_type>()};
	constexpr unsigned_char_type base_char_type(base);
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
		if constexpr(ebcdic)
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

struct simd_parse_result
{
	std::size_t digits;
	fast_io::parse_code code;
};

#if defined(__SSE4_1__) && defined(__x86_64__)

template<bool char_execharset>
inline std::uint32_t detect_length(char unsigned const* buffer) noexcept
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
	std::uint16_t v{static_cast<std::uint16_t>(__builtin_ia32_pmovmskb128((x86_64_v16qi)mask))};
#else
	__m128i chunk = _mm_loadu_si128(reinterpret_cast<__m128i const*>(buffer));
	__m128i const t0 = _mm_sub_epi8(chunk, _mm_set1_epi8(v176_constant));
	__m128i const mask = _mm_cmplt_epi8(t0, _mm_set1_epi8(-118));
	std::uint16_t v{static_cast<std::uint16_t>(_mm_movemask_epi8(mask))};
#endif
	return static_cast<std::uint32_t>(std::countr_one(v));
}

template<bool char_execharset>
#if __has_cpp_attribute(gnu::cold)
[[gnu::cold]]
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


inline constexpr char unsigned sse_shift_table[32]{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

template<bool char_execharset,bool less_than_64_bits>
#if __has_cpp_attribute(gnu::hot)
[[gnu::hot]]
#endif
inline simd_parse_result sse_parse(char unsigned const* buffer,char unsigned const* buffer_end,std::uint64_t &res) noexcept
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
	std::uint16_t v{static_cast<std::uint16_t>(__builtin_ia32_pmovmskb128((x86_64_v16qi)mask))};
	std::uint32_t digits{static_cast<std::uint32_t>(std::countr_one(v))};
	if(digits==0)
		return {0,parse_code::invalid};
	x86_64_v16qu const zeros{zero_constant,zero_constant,zero_constant,zero_constant,zero_constant,zero_constant,zero_constant,zero_constant,
		zero_constant,zero_constant,zero_constant,zero_constant,zero_constant,zero_constant,zero_constant,zero_constant};
	chunk-=zeros;
	x86_64_v16qi shuffle_mask;
	__builtin_memcpy(__builtin_addressof(shuffle_mask),sse_shift_table+digits,sizeof(x86_64_v16qi));
	chunk=(x86_64_v16qu)__builtin_ia32_pshufb128((x86_64_v16qi)chunk,shuffle_mask);
	chunk=(x86_64_v16qu)__builtin_ia32_pmaddubsw128((x86_64_v16qi)chunk,x86_64_v16qi{10,1,10,1,10,1,10,1,10,1,10,1,10,1,10,1});
	chunk=(x86_64_v16qu)__builtin_ia32_pmaddwd128((x86_64_v8hi)chunk,x86_64_v8hi{100,1,100,1,100,1,100,1});
	chunk=(x86_64_v16qu)__builtin_ia32_packusdw128((x86_64_v4si)chunk,(x86_64_v4si)chunk);
	chunk=(x86_64_v16qu)__builtin_ia32_pmaddwd128((x86_64_v8hi)chunk,x86_64_v8hi{10000,1,10000,1,0,0,0,0});
	std::uint64_t chunk0;
	__builtin_memcpy(__builtin_addressof(chunk0),__builtin_addressof(chunk),sizeof(chunk0));
#else
	__m128i chunk = _mm_loadu_si128(reinterpret_cast<__m128i const*>(buffer));
	__m128i const t0 = _mm_sub_epi8(chunk, _mm_set1_epi8(v176_constant));
	__m128i const mask = _mm_cmplt_epi8(t0, _mm_set1_epi8(-118));
	std::uint16_t v{static_cast<std::uint16_t>(_mm_movemask_epi8(mask))};
	std::uint32_t digits{static_cast<std::uint32_t>(std::countr_one(v))};
	if(digits==0)
		return {0,parse_code::invalid};
	chunk = _mm_sub_epi8(chunk, _mm_set1_epi8(zero_constant));
	chunk = _mm_shuffle_epi8(chunk,_mm_loadu_si128(reinterpret_cast<__m128i const*>(sse_shift_table+digits)));
	chunk = _mm_maddubs_epi16(chunk, _mm_set_epi8(1,10,1,10,1,10,1,10,1,10,1,10,1,10,1,10));
	chunk = _mm_madd_epi16(chunk, _mm_set_epi16(1,100,1,100,1,100,1,100));
	chunk = _mm_packus_epi32(chunk, chunk);
	chunk = _mm_madd_epi16(chunk,_mm_set_epi16(0,0,0,0,1,10000,1,10000));
	std::uint64_t chunk0;
	std::memcpy(__builtin_addressof(chunk0),__builtin_addressof(chunk),sizeof(chunk0));
#endif
	std::uint64_t result{static_cast<std::uint64_t>(((chunk0 & 0xffffffff) * 100000000ULL) + (chunk0 >> 32))};
	if(digits==16)[[unlikely]]
	{
		if constexpr(less_than_64_bits)
		{
			//std::uint32_t can never have 16 digits
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
				res=result*1000ULL+static_cast<std::uint64_t>(buffer[16]-zero_constant)*100ULL+static_cast<std::uint64_t>(buffer[17]-zero_constant)*10ULL
				+static_cast<std::uint64_t>(buffer[18]-zero_constant);
				return {19,parse_code::ok};
			}
			case 2:
			{
				res=result*100+static_cast<std::uint64_t>(buffer[16]-zero_constant)*10ULL+static_cast<std::uint64_t>(buffer[17]-zero_constant);
				return {18,parse_code::ok};
			}
			case 1:
			{
				res=result*10+static_cast<std::uint64_t>(buffer[16]-zero_constant);
				return {17,parse_code::ok};
			}
			case 0:
			{
				res=result;
				return {16,parse_code::ok};
			}
			case 4:
			{
				constexpr std::uint64_t risky_value{UINT64_MAX/10000ULL};
				constexpr std::uint64_t risky_mod{UINT64_MAX%10000ULL};
				if(result>risky_value)
					return {20,parse_code::overflow};
				std::uint64_t partial{static_cast<std::uint64_t>(buffer[16]-zero_constant)*1000ULL+static_cast<std::uint64_t>(buffer[17]-zero_constant)*100ULL
				+static_cast<std::uint64_t>(buffer[18]-zero_constant)*10ULL+static_cast<std::uint64_t>(buffer[19]-zero_constant)};
				if(result==risky_value&&risky_mod<partial)
					return {20,parse_code::overflow};
				res=result*10000ULL+partial;
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

template<char8_t base,::fast_io::freestanding::random_access_iterator Iter,my_unsigned_integral T>
#if defined(__SSE4_1__) && __has_cpp_attribute(gnu::cold) && defined(__x86_64__)
[[gnu::cold]]
#endif
inline constexpr parse_result<Iter> scan_int_contiguous_none_simd_space_part_define_impl(Iter first,Iter last,T& res) noexcept
{
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	using unsigned_type = my_make_unsigned_t<std::remove_cvref_t<T>>;
	constexpr unsigned_char_type base_char_type{base};
	constexpr std::size_t max_size{details::cal_max_int_size<unsigned_type,base>()-1};
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
		if(!char_digit_to_literal<base,char_type>(ch))[[unlikely]]
		{
			constexpr unsigned_type risky_uint_max{static_cast<unsigned_type>(-1)};
			constexpr unsigned_type risky_value{risky_uint_max/base};
			constexpr unsigned_char_type risky_digit(risky_uint_max%base);
			overflow=res>risky_value||(risky_value==res&&ch>risky_digit);;
			if(!overflow)
			{
				res*=base_char_type;
				res+=ch;
			}
			for(++first;first!=last&&char_is_digit<base,char_type>(static_cast<unsigned_char_type>(*first));++first)
				overflow|=true;
		}
	}
	return {first,(overflow?(parse_code::overflow):(parse_code::ok))};
}

template<char8_t base,::fast_io::freestanding::random_access_iterator Iter,my_integral T>
inline constexpr parse_result<Iter> scan_int_contiguous_none_space_part_define_impl(Iter first,Iter last,T& t) noexcept
{
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	[[maybe_unused]] bool sign{};
	if constexpr(my_signed_integral<T>)
	{
		if(first==last)
			return {first,parse_code::invalid};
		constexpr auto minus_sign{char_literal_v<u8'-',char_type>};
		if((sign=(minus_sign==*first)))
			++first;
	}
	constexpr auto zero{char_literal_v<u8'0',char_type>};
	if(first!=last)
	{
		auto first_ch{*first};
		if(!char_is_digit<base,char_type>(static_cast<unsigned_char_type>(first_ch)))[[unlikely]]
			return {first,parse_code::invalid};
		else if(first_ch==zero)
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
	using unsigned_type = my_make_unsigned_t<std::remove_cvref_t<T>>;
	unsigned_type res{};
	Iter it;
#if defined(__SSE4_1__) && defined(__x86_64__)
	if constexpr(base==10&&sizeof(char_type)==1&&sizeof(unsigned_type)<=sizeof(std::uint64_t))
	{
		if(
#if __cpp_lib_is_constant_evaluated >= 201811L
		!std::is_constant_evaluated()&&
#endif
		last-first>=32)[[likely]]
		{
			constexpr bool smaller_than_uint64{sizeof(unsigned_type)<sizeof(std::uint64_t)};
			std::uint64_t temp{};
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
		t=res;
	return {it,parse_code::ok};
}

template<char8_t base,::fast_io::freestanding::random_access_iterator Iter,details::my_integral T>
inline constexpr parse_result<Iter> scan_int_contiguous_define_impl(Iter first,Iter last,T& t) noexcept
{
	for(;first!=last&&::fast_io::char_category::is_c_space(*first);++first);
	if(first==last)
		return {first,parse_code::end_of_file};
	return scan_int_contiguous_none_space_part_define_impl<base>(first,last,t);
}
}

enum class scan_integral_context_phase:std::uint_fast8_t
{
space,
sign,
zero,
zero_invalid,
digit,
overflow
};

namespace details
{
template<char8_t base,std::integral char_type,details::my_integral T>
inline constexpr auto scan_context_type_impl_int() noexcept
{
	using unsigned_type = details::my_make_unsigned_t<std::remove_cvref_t<T>>;
	constexpr std::size_t max_size{details::cal_max_int_size<unsigned_type,base>()+2};
	struct scan_integer_context
	{
		::fast_io::freestanding::array<char_type,max_size> buffer;
		std::uint_fast8_t size{};
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

template<char8_t base,::fast_io::freestanding::input_iterator Iter>
inline constexpr Iter scan_skip_all_digits_impl(Iter first,Iter last) noexcept
{
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	for(;first!=last&&char_is_digit<base,char_type>(static_cast<unsigned_char_type>(*first));++first);
	return first;
}

template<char8_t base,typename State,::fast_io::freestanding::input_iterator Iter,my_integral T>
inline constexpr parse_result<Iter> scan_context_define_parse_impl(State& st,Iter first,Iter last,T& t) noexcept
{
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	auto phase{st.integer_phase};
	switch(phase)
	{
	case scan_integral_context_phase::space:
	{
		for(;first!=last&&::fast_io::char_category::is_c_space(*first);++first);
		if(first==last)
			return {first,parse_code::partial};
		[[fallthrough]];
	}
	case scan_integral_context_phase::sign:
	{
		if constexpr(my_signed_integral<T>)
		{
			if(first==last)
			{
				st.integer_phase=scan_integral_context_phase::sign;
				return {first,parse_code::partial};
			}
			constexpr auto minus{char_literal_v<u8'-',char_type>};
			if(*first==minus)
			{
				*st.buffer.data()=*first;
				st.size=1;
				++first;
			}
		}
		[[fallthrough]];
	}
	case scan_integral_context_phase::zero:
	{
		if(first==last)
		{
			st.integer_phase=scan_integral_context_phase::zero;
			return {first,parse_code::partial};
		}
		constexpr auto zero{char_literal_v<u8'0',char_type>};
		{
			auto first_ch{*first};
			if(!char_is_digit<base,char_type>(static_cast<unsigned_char_type>(first_ch)))[[unlikely]]
			{
				return {first,parse_code::invalid};
			}
			else if(first_ch==zero)[[unlikely]]
			{
				++first;
				if(first==last)
				{
					st.integer_phase=scan_integral_context_phase::zero_invalid;
					return {first,parse_code::partial};
				}
				if(!char_is_digit<base,char_type>(static_cast<unsigned_char_type>(*first)))
				{
					t={};
					return {first,parse_code::ok};
				}
				return {first,parse_code::invalid};
			}
		}
		[[fallthrough]];
	}
	case scan_integral_context_phase::digit:
	{
		auto it{scan_skip_all_digits_impl<base>(first,last)};
		std::size_t const diff{st.buffer.size()-static_cast<std::size_t>(st.size)};
		std::size_t const first_it_diff{static_cast<std::size_t>(it-first)};
		if(first_it_diff<diff)
		{
			auto start{st.buffer.data()+st.size};
			auto e{non_overlapped_copy_n(first,first_it_diff,start)};
			st.size+=static_cast<std::uint_fast8_t>(first_it_diff);
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
		break;
	}
	case scan_integral_context_phase::zero_invalid:
	{
		if(first==last)
			return {first,parse_code::partial};
		++first;
		if(!char_is_digit<base,char_type>(static_cast<unsigned_char_type>(*first)))
		{
			t={};
			return {first,parse_code::ok};
		}
		return {first,parse_code::invalid};
	}
	default:
	{
		first=scan_skip_all_digits_impl<base>(first,last);
		return {first,(first==last)?parse_code::partial:parse_code::invalid};
	}
	}
}

template<char8_t base,typename State,my_integral T>
#if __has_cpp_attribute(gnu::cold)
[[gnu::cold]]
#endif
inline constexpr parse_code scan_context_eof_define_parse_impl(State& st,T& t) noexcept
{
	switch(st.integer_phase)
	{
	case scan_integral_context_phase::space:
		return parse_code::end_of_file;
	case scan_integral_context_phase::digit:
		return scan_int_contiguous_none_space_part_define_impl<base>(st.buffer.data(),st.buffer.data()+st.size,t).code;
	case scan_integral_context_phase::overflow:
		return parse_code::overflow;
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

template<std::size_t bs,bool noskipws=false,::fast_io::details::my_integral scalar_type>
inline constexpr scalar_manip_t<::fast_io::details::base_scan_mani_flags_cache<bs,noskipws>,scalar_type&> base_get(scalar_type& t) noexcept
{
	return {t};
}

template<bool noskipws=false,::fast_io::details::my_integral scalar_type>
inline constexpr scalar_manip_t<::fast_io::details::base_scan_mani_flags_cache<2,noskipws>,scalar_type&> bin_get(scalar_type& t) noexcept
{
	return {t};
}

template<bool noskipws=false,::fast_io::details::my_integral scalar_type>
inline constexpr scalar_manip_t<::fast_io::details::base_scan_mani_flags_cache<16,noskipws>,scalar_type&> hex_get(scalar_type& t) noexcept
{
	return {t};
}

}

template<details::my_integral T>
inline constexpr ::fast_io::manipulators::scalar_manip_t<::fast_io::details::base_scan_mani_flags_cache<10,false>,T&> scan_alias_define(io_alias_t,T& t) noexcept
{
	return {t};
}

template<std::integral char_type,manipulators::scalar_flags flags,details::my_integral T>
inline constexpr auto scan_context_type(io_reserve_type_t<char_type,::fast_io::manipulators::scalar_manip_t<flags,T&>>) noexcept
{
	return details::scan_context_type_impl_int<flags.base,char_type,T>();
}

template<::fast_io::freestanding::random_access_iterator Iter,manipulators::scalar_flags flags,details::my_integral T>
inline constexpr parse_result<Iter> scan_contiguous_define(io_reserve_type_t<::fast_io::freestanding::iter_value_t<Iter>,::fast_io::manipulators::scalar_manip_t<flags,T&>>,Iter begin,Iter end,::fast_io::manipulators::scalar_manip_t<flags,T&> t) noexcept
{
	if constexpr(::fast_io::freestanding::contiguous_iterator<Iter>&&!std::is_pointer_v<Iter>)
	{
		auto [it,ec] = details::scan_int_contiguous_define_impl<flags.base>(::fast_io::freestanding::to_address(begin),::fast_io::freestanding::to_address(end),t.reference);
		return {it-::fast_io::freestanding::to_address(begin)+begin,ec};
	}
	else
		return details::scan_int_contiguous_define_impl<flags.base>(begin,end,t.reference);
}

template<::fast_io::freestanding::input_iterator Iter,manipulators::scalar_flags flags,typename State,details::my_integral T>
inline constexpr parse_result<Iter> scan_context_define(io_reserve_type_t<::fast_io::freestanding::iter_value_t<Iter>,::fast_io::manipulators::scalar_manip_t<flags,T&>>,State& state,Iter begin,Iter end,::fast_io::manipulators::scalar_manip_t<flags,T&> t) noexcept
{
	return details::scan_context_define_parse_impl<flags.base>(state,begin,end,t.reference);
}

template<std::integral char_type,manipulators::scalar_flags flags,typename State,details::my_integral T>
inline constexpr parse_code scan_context_eof_define(io_reserve_type_t<char_type,::fast_io::manipulators::scalar_manip_t<flags,T&>>,State& state,::fast_io::manipulators::scalar_manip_t<flags,T&> t) noexcept
{
	return details::scan_context_eof_define_parse_impl<flags.base>(state,t.reference);
}

namespace details
{
template<::fast_io::freestanding::input_iterator Iter>
inline constexpr parse_result<Iter> ch_get_context_impl(Iter first,Iter last,::fast_io::freestanding::iter_value_t<Iter>& t) noexcept
{
	for(;first!=last&&::fast_io::char_category::is_c_space(*first);++first);
	if(first==last)[[unlikely]]
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

template<::fast_io::freestanding::input_iterator Iter>
inline constexpr parse_result<Iter> scan_context_define(io_reserve_type_t<::fast_io::freestanding::iter_value_t<Iter>,manipulators::ch_get_t<::fast_io::freestanding::iter_value_t<Iter>&>>,details::empty,Iter begin,Iter end,manipulators::ch_get_t<::fast_io::freestanding::iter_value_t<Iter>&> t) noexcept
{
	return details::ch_get_context_impl(begin,end,t.reference);
}

template<std::integral char_type>
inline constexpr parse_code scan_context_eof_define(io_reserve_type_t<char_type,manipulators::ch_get_t<char_type&>>,details::empty,manipulators::ch_get_t<char_type&>) noexcept
{
	return parse_code::end_of_file;
}

}