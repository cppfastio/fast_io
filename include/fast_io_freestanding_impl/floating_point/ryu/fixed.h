#pragma once

namespace fast_io::details::ryu
{

template<my_integral mantissaType,std::integral exponentType>
struct unrep
{
	using mantissa_type = mantissaType;
	using exponent_type = exponentType;
	mantissa_type m;
	exponent_type e;
};
template<std::unsigned_integral T>
inline constexpr T index_for_exponent(T e){return (e+15)>>4;}

template<std::unsigned_integral T>
inline constexpr T pow10_bits_for_index(T idx){return (idx<<4)+120;}


template<typename T>
inline constexpr std::uint32_t mul_shift_mod_1e9(std::uint64_t m, ::fast_io::freestanding::array<T,3> const& mul, std::size_t j)
{
	uint128_t b1(mul_extend(m,mul[1]));
	b1+=high(mul_extend(m,mul[0]));
	uint128_t s1(mul_extend(m,mul[2]));
	s1+=high(b1);
	s1>>=j-128;
	uint128_t constexpr mulb(construct_unsigned_extension(static_cast<std::uint64_t>(0x31680A88F8953031),static_cast<std::uint64_t>(0x89705F4136B4A597)));
	return static_cast<std::uint32_t>(s1)-1000000000*static_cast<std::uint32_t>(low(mul_high(s1,mulb))>>29);
}

template<typename T>
inline constexpr std::uint32_t pow5_factor(T value)
{
	for (std::uint32_t count{};value;++count)
	{
		if (value%5)
			return count;
		value/=5;
	}
	return 0;
}



// Returns true if value is divisible by 5^p.
template<typename T>
inline constexpr bool multiple_of_power_of_5(T value,std::uint32_t p)
{
	// The author tried a case distinction on p, but there was no performance difference.
	return p<=pow5_factor(value);
}

inline constexpr uint32_t log10_pow2(std::uint64_t e)
{
	return static_cast<std::uint32_t> (((static_cast<std::uint64_t>(e)) * 169464822037455ull) >> 49);
}
template<std::unsigned_integral T>
inline constexpr std::size_t length_for_index(T idx){return (log10_pow2(idx<<4)+25)/9;}

template<std::integral T>
inline constexpr uint32_t log10_pow5(T e)
{
	// The first value this approximation fails for is 5^2621 which is just greater than 10^1832.
	return static_cast<uint32_t> (((static_cast<uint64_t>(e)) * 196742565691928ull) >> 48);
}
/*
template<bool controller,std::unsigned_integral T>
inline constexpr ::fast_io::freestanding::array<fast_io::uint128_t,2> compute_pow5(T v)
{
	std::uint32_t const base(v/56);
	std::uint32_t const base2(base*56);
	::fast_io::freestanding::array<std::uint64_t,4> const& mul(pow5<long double,controller>::split[base]);
	if(v==base2)
		return {construct_unsigned_extension(mul.front(),mul[1]),construct_unsigned_extension(mul[2],mul[3])};
	else
	{
		std::uint32_t const offset(v - base2);
		::fast_io::freestanding::array<std::uint64_t,2> const &m = pow5<long double,controller>::[offset];
		const uint32_t delta = pow5bits(i) - pow5bits(base2);
		const uint32_t corr = (uint32_t) ((POW5_ERRORS[i / 32] >> (2 * (i % 32))) & 3);
		mul_128_256_shift(m, mul, delta, corr, result);
	}
//		return pow5<long double,controller>::;
//	pow5<long double,true>::inv_split[q]
}

template<std::unsigned_integral T>
inline constexpr ::fast_io::freestanding::array<fast_io::uint128_t,2> compute_pow5_inv(T v)
{
}
*/


template<::fast_io::freestanding::random_access_iterator Iter,my_unsigned_integral mantissaType>
inline constexpr auto easy_case(Iter result,bool sign, mantissaType const& mantissa)
{
	if (mantissa)
		return my_copy_n(u8"nan",3,result);
	if (sign)
		return my_copy_n(u8"-inf",4,result);
	return my_copy_n(u8"inf",3,result);
}

template<std::floating_point floating,bool ignore_exp0=false,my_unsigned_integral mantissaType,std::signed_integral exponentType>
inline constexpr unrep<mantissaType,exponentType> init_rep(mantissaType const& mantissa,exponentType const& exponent)
{
	if constexpr(sizeof(floating)==16)
	{
		if constexpr(!ignore_exp0)
		{
			if(!exponent)
				return {mantissa,1-static_cast<exponentType>(floating_traits<floating>::bias+floating_traits<floating>::mantissa_bits)};
		}
		return {mantissa,static_cast<exponentType>(exponent-static_cast<exponentType>(floating_traits<floating>::bias+floating_traits<floating>::mantissa_bits))};
	}	
	else
	{
		if constexpr(!ignore_exp0)
		{
			if(!exponent)
				return {mantissa,1-static_cast<exponentType>(floating_traits<floating>::bias+floating_traits<floating>::mantissa_bits)};
		}
		return {static_cast<mantissaType>((static_cast<mantissaType>(1)<<floating_traits<floating>::mantissa_bits)|mantissa),
			static_cast<exponentType>(exponent-static_cast<exponentType>(floating_traits<floating>::bias+floating_traits<floating>::mantissa_bits))};
	}
}

template<bool uppercase_e=false,bool four_digits=false,std::signed_integral T,::fast_io::freestanding::random_access_iterator Iter>
requires std::same_as<T,std::int32_t>
inline constexpr Iter output_exp(T exp,Iter result)
{
	if constexpr(uppercase_e)
		*result=0x41;
	else
		*result=0x65;
	++result;
	if(exp<0)
	{
		*result=0x2d;
		++result;
		exp=-exp;
	}
	else
	{
		*result=0x2b;
		++result;
	}
	using char_type = std::remove_reference_t<decltype(*result)>;
	std::make_unsigned_t<T> unsigned_exp(exp);
#ifdef __OPTIMIZE_SIZE__
	std::size_t len{2};
	if constexpr(four_digits)
	{
		if(1000<=unsigned_exp)[[unlikely]]
			len=4;
		else if(100<=unsigned_exp)[[unlikely]]
			len=3;
	}
	else
	{
		if(100<=unsigned_exp)[[unlikely]]
			len=3;
	}
	auto str{result+len};
	for(std::size_t i{};i!=len;++i)
	{
		std::make_unsigned_t<T> const temp(unsigned_exp/10);
		char_type const res(unsigned_exp%10);
		*--str=u8'0'+res;
		unsigned_exp=temp;
	}
	return result+len;
#elif defined(FAST_IO_OPTIMIZE_TIME)
	if constexpr(four_digits)
	{
	if(1000<=unsigned_exp)[[unlikely]]
		return my_copy_n(jiaendu::static_tables<char_type>::table4[unsigned_exp].data(),4,result);
	}
	if(100<=unsigned_exp)[[unlikely]]
		return my_copy_n(jiaendu::static_tables<char_type>::table3[unsigned_exp].data(),3,result);
	return my_copy_n(jiaendu::static_tables<char_type>::table2[unsigned_exp].data(),2,result);
#else
	constexpr auto tb(get_shared_inline_constexpr_base_table<char_type,10,false>().data());
	if constexpr(four_digits)
	{
	if(1000<=unsigned_exp)[[unlikely]]
	{
		auto u{unsigned_exp%100u};
		auto v{unsigned_exp/100u};
		my_copy_n(tb[v].data(),2,result);
		my_copy_n(tb[u].data(),2,result+2);
		return result+4;
	}
	}
	if(100<=unsigned_exp)[[unlikely]]
	{
		auto u{unsigned_exp%100u};
		*result=static_cast<char8_t>(unsigned_exp/100u)+u8'0';
		my_copy_n(tb[u].data(),2,result+1);
		return result+3;
	}
	return my_copy_n(tb[unsigned_exp].data(),2,result);
#endif
}

template<char32_t dec,bool scientific = false,bool uppercase_e=false,::fast_io::freestanding::random_access_iterator Iter,std::floating_point F>
inline constexpr auto output_fixed(Iter result, F d,std::size_t precision)
{
	using floating_trait = floating_traits<F>;
	using mantissa_type = typename floating_trait::mantissa_type;
	using exponent_type = typename floating_trait::exponent_type;
	using signed_exponent_type = std::make_signed_t<exponent_type>;
	using char_type = std::remove_reference_t<decltype(*result)>;
	auto const bits(bit_cast<mantissa_type>(d));
	// Decode bits into sign, mantissa, and exponent.
	bool const sign((bits >> (floating_trait::mantissa_bits + floating_trait::exponent_bits)) & 1u);
	mantissa_type const mantissa(bits & ((static_cast<mantissa_type>(1u) << floating_trait::mantissa_bits) - 1u));
	exponent_type const exponent(static_cast<exponent_type>(((bits >> floating_trait::mantissa_bits) & floating_trait::exponent_max)));
	// Case distinction; exit early for the easy cases.
	if(exponent == floating_trait::exponent_max)
		return easy_case(result,sign,mantissa);
	auto start(result);
	if(!exponent&&!mantissa)
	{
		if(sign)
		{
			*result=0x2d;
			++result;
		}
		*result=0x30;
		++result;
		if (precision!=0)
		{
			*result=dec;
			++result;
			result=my_fill_n(result,precision,0x30);
			if constexpr(scientific)
			{
				if constexpr(uppercase_e)
					return my_copy_n(u8"E+00",4,result);
				else
					return my_copy_n(u8"e+00",4,result);
			}
		}
		return result;
	}
	auto const r2(init_rep<F>(mantissa,static_cast<signed_exponent_type>(exponent)));
	if (sign)
	{
		*result=0x2d;
		++result;
	}
	bool const negative_r2_e(r2.e<0);
	if constexpr(scientific)
	{
		std::size_t const scientific_precision(precision+1);
		exponent_type digits(0),printed_digits(0),available_digits(0);
		signed_exponent_type exp(0);
		if(-52<=r2.e)
		{
			exponent_type const idx(negative_r2_e?0:index_for_exponent(static_cast<exponent_type>(r2.e)));
			signed_exponent_type const p10bitsmr2e(pow10_bits_for_index(idx)-r2.e+8);
			auto const idx_offset(fixed_pow10<>::offset[idx]);
			for(std::size_t i(length_for_index(idx));i--;)
			{
				digits=mul_shift_mod_1e9(r2.m<<8,fixed_pow10<>::split[idx_offset+i],p10bitsmr2e);
				if(printed_digits)
				{
					if (precision<9)
					{
						available_digits=9;
						break;
					}
					else if(scientific_precision < printed_digits + 9)
					{
						available_digits=9;
						break;
					}
					my_fill(result,output_base_number_impl<10,false,false,dec>(result+9,digits),0x30);
					result+=9;
					printed_digits+=9;
				}
				else if(digits)
				{
					available_digits = chars_len<10,true>(digits);
					exp = static_cast<signed_exponent_type>(i*9 + available_digits - 1);
					if(scientific_precision < available_digits)
						break;
					if (precision!=0)
						output_base_number_impl<10,false,true,dec>(result+=available_digits+1,digits);
					else
					{
						*result=static_cast<char_type>(0x30+digits);
						++result;
					}
					printed_digits = available_digits;
					available_digits = 0;
				}
			}
		}
		if(negative_r2_e&&!available_digits)
		{
			auto abs_e2(-r2.e);
			exponent_type const idx(static_cast<exponent_type>(abs_e2)>>4);
			signed_exponent_type j(128+(abs_e2-(idx<<4)));
			exponent_type const of2i(fixed_pow10<>::offset_2[idx]);
			exponent_type const idxp1(fixed_pow10<>::offset_2[idx+1]);
			exponent_type const mb2_idx(fixed_pow10<>::min_block_2[idx]);
			for (exponent_type i(mb2_idx); i < 200; ++i)
			{
				exponent_type const p(of2i+i-mb2_idx);
				digits=(idxp1<=p)?0:mul_shift_mod_1e9(r2.m<<8,fixed_pow10<>::split_2[p],j);
				if(printed_digits)
				{
					if (precision<9)
					{
						available_digits=9;
						break;
					}
					else if(scientific_precision < printed_digits + 9)
					{
						available_digits=9;
						break;
					}
					my_fill(result,output_base_number_impl<10,false,false,dec>(result+9,digits),0x30);
					result+=9;
					printed_digits+=9;
				}
				else if(digits)
				{
					available_digits=chars_len<10,true>(digits);
					exp = static_cast<signed_exponent_type> (available_digits -(i + 1) * 9 - 1);
					if (scientific_precision<available_digits)
						break;
					if (precision!=0)
						output_base_number_impl<10,false,true,dec>(result+=available_digits+1,digits);
					else
					{
						*result=static_cast<char_type>(0x30+digits);
						++result;
					}
					printed_digits = available_digits;
					available_digits = 0;
				}
			}
		}
		exponent_type const maximum(scientific_precision - printed_digits);
		exponent_type lastdigit(0);
		for(exponent_type k(maximum);k<available_digits;++k)
		{
			lastdigit = digits%10;
			digits /= 10;
		}
		std::size_t round_up(0);
		if(lastdigit!=5)
			round_up = 5 < lastdigit;
		else
		{
			signed_exponent_type const rexp (static_cast<signed_exponent_type> (scientific_precision - exp));
			signed_exponent_type const required_twos(-r2.e - rexp);
			bool trailing_zeros(required_twos <= 0 || (required_twos < 60 && multiple_of_power_of_2(r2.m, static_cast<exponent_type>(required_twos))));
			if (rexp < 0)
			{
				signed_exponent_type required_fives = -rexp;
				trailing_zeros = trailing_zeros && multiple_of_power_of_5(r2.m, static_cast<exponent_type>(required_fives));
			}
			round_up = trailing_zeros ? 2 : 1;
		}
		if(printed_digits)
		{
			if(digits)
			{
				my_fill(result,output_base_number_impl<10,false,false,dec>(result+maximum,digits),0x30);
				result+=maximum;
			}
			else
				result=my_fill_n(result,maximum,0x30);
		}
		else
		{
			if (precision!=0)
			{
				my_fill(result,output_base_number_impl<10,false,true,dec>(result+maximum+1,digits),0x30);
				result+=maximum+1;
			}
			else
			{
				*result = 0x30 + digits;
				++result;
			}
		}
		if(round_up)
		{
			std::size_t round_index(result-start);
			while(round_index--)
			{
				auto c(start[round_index]);
				if (c == 0x2d)
				{
					start[round_index+1] = 0x31;
					++exp;
					break;
				}
				if (precision==0)
				{
					if (c == 0x39)
					{
						start[round_index] = 0x30;
						round_up = 1;
						continue;
					}
					else
					{
						if (round_up==2&&!(c&1))
							break;
						start[round_index]=c+1;
						break;
					}
				}
				else
				{
					if (c == dec)
						continue;
					else if (c == 0x39)
					{
						start[round_index] = 0x30;
						round_up = 1;
						continue;
					}
					else
					{
						if (round_up==2&&!(c&1))
							break;
						start[round_index]=c+1;
						break;
					}
				}
			}
			if(round_index==static_cast<std::size_t>(-1))
			{
				start[round_index+1] = 0x31;
				++exp;
			}
		}
		return output_exp<uppercase_e>(exp,result);
	}
	else
	{
		bool nonzero(false);
		if(-52<=r2.e)
		{
			exponent_type const idx(negative_r2_e?0:index_for_exponent(static_cast<exponent_type>(r2.e)));
			signed_exponent_type const p10bitsmr2e(pow10_bits_for_index(idx)-r2.e+8);
			for(std::size_t i(length_for_index(idx));i--;)
			{
				exponent_type digits(mul_shift_mod_1e9(r2.m<<8,fixed_pow10<>::split[fixed_pow10<>::offset[idx]+i],p10bitsmr2e));
				if(nonzero)
				{
					my_fill(result,output_base_number_impl<10,false,false,dec>(result+9,digits),0x30);
					result+=9;
				}
				else if(digits)
				{
					output_base_number_impl<10,false,false,dec>(result+=chars_len<10,true>(digits),digits);
					nonzero = true;
				}
			}
		}
		if(!nonzero)
		{
			*result=0x30;
			++result;
		}
		if (precision!=0)
		{
			*result=dec;
			++result;
		}
		if(negative_r2_e)
		{
			auto abs_e2(-r2.e);
			exponent_type const idx(static_cast<exponent_type>(abs_e2)>>4);
			std::size_t const blocks(precision/9+1);
			std::size_t round_up(0);
			std::size_t i(0);
			auto const mb2_idx(fixed_pow10<>::min_block_2[idx]);
			if (blocks<=mb2_idx)
			{
				i=blocks;
				result=my_fill_n(result,precision,0x30);
			}
			else if(i<mb2_idx)
				result=my_fill_n(result,9*(i=mb2_idx),0x30);
			signed_exponent_type j(128+(abs_e2-(idx<<4)));
			auto const of2i(fixed_pow10<>::offset_2[idx]);
			for(;i<blocks;++i)
			{
				exponent_type p(of2i+i-mb2_idx);
				exponent_type digits(mul_shift_mod_1e9(r2.m<<8,fixed_pow10<>::split_2[p],j));
				if (fixed_pow10<>::offset_2[idx+1]<=p)
				{
					result=my_fill_n(result,precision-9*i,0x30);
					break;
				}
				if(i+1<blocks)
				{
					my_fill(result,output_base_number_impl<10,false,false,dec>(result+9,digits),0x30);
					result+=9;
				}
				else
				{
					exponent_type const maximum(precision-9*i);
					exponent_type lastdigit(0);
					for(exponent_type k(maximum);k<9;++k)
					{
						lastdigit = digits%10;
						digits /= 10;
					}
					if(lastdigit!=5)
						round_up=lastdigit>5;
					else
					{
						auto const required_twos(static_cast<signed_exponent_type>(abs_e2-precision-1));
						if(required_twos<=0||(required_twos<60&&multiple_of_power_of_2(r2.m,static_cast<exponent_type>(required_twos))))
							round_up = 2;
						else
							round_up = 1;
					}
					if(maximum)
					{
						my_fill(result,output_base_number_impl<10,false,false,dec>(result+maximum,digits),0x30);
						result+=maximum;
					}
					break;
				}
			}
			if(round_up)
			{
				std::size_t round_index(result-start);
				if (precision!=0)
				{
					std::size_t dot_index(0);
					while(round_index--)
					{
						auto c(start[round_index]);
						if (c == 0x2d)
						{
							start[round_index+1] = 0x31;
							if(dot_index)
							{
								start[dot_index] = 0x30;
								start[dot_index+1] = dec;
							}
							*result=0x30;
							return ++result;
						}
						if (c == dec)
						{
							dot_index = round_index;
							continue;
						}
						else if (c == 0x39)
						{
							start[round_index] = 0x30;
							round_up = 1;
							continue;
						}
						if (round_up!=2||c&1)
							start[round_index]=c+1;
						return result;
					}
					*start=0x31;
					if(dot_index)
					{
						start[dot_index] = 0x30;
						start[dot_index+1] = dec;
					}
				}
				else
				{
					while(round_index--)
					{
						auto c(start[round_index]);
						if (c == 0x2d)
						{
							start[round_index+1] = 0x31;
							*result=0x30;
							return ++result;
						}
						if (c == 0x39)
						{
							start[round_index] = 0x30;
							round_up = 1;
							continue;
						}
						if (round_up!=2||c&1)
							start[round_index]=c+1;
						return result;
					}
					*start=0x31;
				}
				*result=0x30;
				++result;
			}
			return result;
		}
		return my_fill_n(result,precision,0x30);
	}
}

}
