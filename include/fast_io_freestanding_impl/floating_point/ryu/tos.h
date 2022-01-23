#pragma once

namespace fast_io::details::ryu
{

template<std::floating_point floating,my_unsigned_integral mantissaType,std::signed_integral exponentType>
inline constexpr unrep<mantissaType,exponentType> init_repm2(mantissaType const& mantissa,exponentType const& exponent)
{
	if constexpr(std::same_as<floating,long double>)
	{
	if(!exponent)
		return {mantissa,1-static_cast<exponentType>(floating_traits<floating>::bias+floating_traits<floating>::mantissa_bits+1)};
	return {mantissa,
		static_cast<exponentType>(exponent-static_cast<exponentType>(floating_traits<floating>::bias+floating_traits<floating>::mantissa_bits+1))};
	}
	else
	{
	if(!exponent)
		return {mantissa,1-static_cast<exponentType>(floating_traits<floating>::bias+floating_traits<floating>::mantissa_bits+2)};
	return {static_cast<mantissaType>((static_cast<mantissaType>(1)<<floating_traits<floating>::mantissa_bits)|mantissa),
		static_cast<exponentType>(exponent-static_cast<exponentType>(floating_traits<floating>::bias+floating_traits<floating>::mantissa_bits+2))};
	}
}


template<bool uppercase_e=false,std::size_t mode=0,bool int_hint=false,
bool is_runtime_decimal_point,std::integral decimal_char_type,
char8_t decimal_point,::fast_io::freestanding::random_access_iterator Iter,std::floating_point F>
inline constexpr Iter output_shortest(
	[[maybe_unused]] compile_time_floating_value<is_runtime_decimal_point,decimal_char_type,decimal_point> decmpt,
	Iter result, F d)
{
	using floating_type = std::remove_cvref_t<F>;
	using char_type = std::remove_reference_t<decltype(*result)>;
	using floating_trait = floating_traits<floating_type>;
	using mantissa_type = typename floating_trait::mantissa_type;
	using exponent_type = typename floating_trait::exponent_type;
	using signed_exponent_type = std::make_signed_t<exponent_type>;
	auto const bits(bit_cast<mantissa_type>(d));
	// Decode bits into sign, mantissa, and exponent.
	bool const sign((bits >> (floating_trait::mantissa_bits + floating_trait::exponent_bits)) & 1u);
	mantissa_type const mantissa(bits & ((static_cast<mantissa_type>(1u) << floating_trait::mantissa_bits) - 1u));
	exponent_type const exponent(static_cast<exponent_type>(((bits >> floating_trait::mantissa_bits) & floating_trait::exponent_max)));
	// Case distinction; exit early for the easy cases.
	if(exponent == floating_trait::exponent_max)
		return easy_case(result,sign,mantissa);
	if(!exponent&&!mantissa)
	{
		if(sign)
		{
			*result=0x2d;
			++result;
		}
		*result=u8'0';
		++result;
		if constexpr(mode==2)
		{
			if constexpr(uppercase_e)
				return my_copy_n(u8"E+00",4,result);
			else
				return my_copy_n(u8"e+00",4,result);
		}
		return result;
	}
	if constexpr(int_hint&&sizeof(F)!=16)//scientific integer hint?? Is that useless?
	{
		auto const r2(init_rep<F,false>(mantissa,static_cast<signed_exponent_type>(exponent)));
		if(-static_cast<std::int32_t>(floating_trait::mantissa_bits)<=r2.e&&r2.e<=0)[[likely]]
		{
			mantissa_type const mask = (static_cast<mantissa_type>(1) << -r2.e) - 1;
			if (!(r2.m & mask))[[likely]]
			{
				auto v2(r2.m>>-r2.e);
				if(sign)
				{
					*result=0x2d;
					++result;
				}
				if constexpr(mode==0)
				{
					if(v2)[[likely]]
					{
						mantissa_type m1(v2%static_cast<mantissa_type>(10000));
						mantissa_type v3(v2/static_cast<mantissa_type>(10000));
						if(m1)[[likely]]//This must be fixed form
							result+=fp_output_unsigned(result,v2);
						else
						{
							for(;;)
							{
								auto const m(v3%10),d(v3/10);
								if(m)
									break;
								v3=d;
							}
							auto const v2_len(chars_len<10,true>(v2));
							auto const v3_len(chars_len<10,true>(v3));
							auto const v3_smaller_than10(v3<10);
							if(v3_len+(v3_smaller_than10?4:5)<v2_len)[[unlikely]]//scientific decision
							{
								if(v3_smaller_than10)
								{
									*result=static_cast<char_type>(v3+u8'0');
									++result;
								}
								else
								{
									fp_output_unsigned(result+1,v3);
									*result=result[1];
									if constexpr(is_runtime_decimal_point)
										result[1]=decmpt.decimal_point;
									else
										result[1]=decimal_point;
									result+=v3_len+1;
								}
								if constexpr(uppercase_e)
									my_copy_n(u8"E+",2,result);
								else
									my_copy_n(u8"e+",2,result);
								result+=2;
								fp_output_two_digits(result,v2_len);
								result+=2;
							}
							else
							{
								fp_output_unsigned_with_len(result,v2,v2_len);
								result+=v2_len;
							}
						}
					}
					else
					{
						*result=u8'0';
						++result;
					}
				}
				else if constexpr(mode==1)		//fixed
					result+=fp_output_unsigned(result,v2);
				else	//scientific
				{
					auto const v2_len(chars_len<10,true>(v2));
					for(;;)
					{
						auto const m(v2%10),d(v2/10);
						if(m)
							break;
						v2=d;
					}
					result+=fp_output_unsigned_point(decmpt,v2,result);
					if constexpr(uppercase_e)
						my_copy_n(u8"E+",2,result);
					else
						my_copy_n(u8"e+",2,result);
					result+=2;
					fp_output_two_digits(result,v2_len-1);
					result+=2;
				}
				return result;
			}
		}
	}
	auto const r2(init_repm2<F>(mantissa,static_cast<signed_exponent_type>(exponent)));
	bool const accept_bounds(!(r2.m&1));
	auto const mv(r2.m<<2);
	exponent_type mm_shift;
#ifdef __SIZEOF_INT128__
	if constexpr(std::same_as<std::remove_cvref_t<F>,long double>)
		mm_shift=(mantissa!=static_cast<mantissa_type>(1)<<(floating_trait::mantissa_bits-1))||exponent==0;
	else
#endif
		mm_shift=(mantissa||static_cast<signed_exponent_type>(exponent)<2);
	//vr,vp,vm
	mantissa_type vr,vp,vm;
	signed_exponent_type e10{};
	bool vm_is_trailing_zeros{},vr_is_trailing_zeros{};
	char8_t last_removed_digit{};
	if(0<=r2.e)
	{
		exponent_type q(log10_pow2(r2.e));
		if constexpr(!std::same_as<floating_type,float>)
			q-=(3<r2.e);
		e10=static_cast<signed_exponent_type>(q);
		signed_exponent_type const k(floating_trait::pow5_inv_bitcount + pow5bits(q) - 1);
		signed_exponent_type const i(-r2.e+static_cast<signed_exponent_type>(q)+k);
#ifdef __SIZEOF_INT128__
		if constexpr(std::same_as<std::remove_cvref_t<F>,long double>)
		{
			auto pow5arr{generic_compute_pow5_inv(q)};
			vr = mul_shift_generic(mv, pow5arr, i);
			vp = mul_shift_generic(mv+2, pow5arr, i);
			vm = mul_shift_generic(mv-1-mm_shift, pow5arr, i);
		}
		else
#endif
		if constexpr(std::same_as<floating_type,double>)
			vr=mul_shift_all(r2.m,pow5<F,true>::inv_split[q],i,vp,vm,mm_shift);
		else if constexpr(std::same_as<floating_type,float>)
		{
			vr = mul_pow5_inv_div_pow2(mv, q, i);
			vp = mul_pow5_inv_div_pow2(mv+2, q, i);
			vm = mul_pow5_inv_div_pow2(mv-1-mm_shift, q, i);
			if (q != 0 && (vp - 1) / 10 <= vm / 10)
			{
				std::int32_t const l = floating_trait::pow5_inv_bitcount + pow5bits(static_cast<std::int32_t>(q - 1)) - 1;
				last_removed_digit = static_cast<char8_t>(mul_pow5_inv_div_pow2(mv, q - 1, -r2.e + static_cast<std::int32_t>(q) - 1 + l) % 10);
			}
		}
		if(q<=floating_trait::floor_log5)//here
		{
			if(!(mv%5))
				vr_is_trailing_zeros=multiple_of_power_of_5(mv,q);
			else if(accept_bounds)
				vm_is_trailing_zeros=multiple_of_power_of_5(mv-1-mm_shift,q);
			else
				vp-=multiple_of_power_of_5(mv+2,q);
		}
	}
	else
	{
		exponent_type abs_e2(static_cast<exponent_type>(-r2.e));
		exponent_type q(log10_pow5(abs_e2));
		if constexpr(!std::same_as<floating_type,float>)
			q-=1<abs_e2;
		signed_exponent_type const signed_q(static_cast<signed_exponent_type>(q));
		e10=signed_q+r2.e;
		signed_exponent_type const i(-r2.e-signed_q);
		signed_exponent_type const k(pow5bits(i)-floating_trait::pow5_bitcount);
		signed_exponent_type j(signed_q-k);
#ifdef __SIZEOF_INT128__
		if constexpr(std::same_as<floating_type,long double>)
		{
			auto pow5arr{generic_compute_pow5(i)};
			vr = mul_shift_generic(mv, pow5arr, j);
			vp = mul_shift_generic(mv+2, pow5arr, j);
			vm = mul_shift_generic(mv-1-mm_shift, pow5arr, j);
		}
		else
#endif
		if constexpr(std::same_as<floating_type,double>)
			vr=mul_shift_all(r2.m,pow5<F,true>::split[i],j,vp,vm,mm_shift);
		else if constexpr(std::same_as<floating_type,float>)
		{
			vr = mul_pow5_div_pow2(mv, static_cast<std::uint32_t>(i), j);
			vp = mul_pow5_div_pow2(mv+2, static_cast<std::uint32_t>(i), j);
			vm = mul_pow5_div_pow2(mv-1-mm_shift, static_cast<std::uint32_t>(i), j);
			if (q != 0 && (vp - 1) / 10 <= vm / 10)
			{
				j = static_cast<std::int32_t>(q) - 1 - (pow5bits(i + 1) - floating_trait::pow5_bitcount);
				last_removed_digit = static_cast<char8_t>(mul_pow5_div_pow2(mv,static_cast<std::uint32_t>(i + 1), j) % 10);
			}
		}
		if(q<2)
		{
			vr_is_trailing_zeros=true;
			if(accept_bounds)
				vm_is_trailing_zeros=mm_shift==1;
			else
				--vp;
		}
		else if(q<floating_trait::bound)
		{
			if constexpr(std::same_as<floating_type,float>)
				vr_is_trailing_zeros=multiple_of_power_of_2(mv,q-1);
			else
				vr_is_trailing_zeros=multiple_of_power_of_2(mv,q);
		}
	}
	signed_exponent_type removed{};
	if(vm_is_trailing_zeros||vr_is_trailing_zeros)
	{
		for(;;)
		{
			mantissa_type const vpdiv10(vp/10);
			mantissa_type const vmdiv10(vm/10);
			auto const vmmod10(static_cast<char8_t>(vm%10));
			if(vpdiv10 <= vmdiv10)
				break;
			mantissa_type const vrdiv10(vr/10);
			auto const vrmod10(static_cast<char8_t>(vr%10));
			vm_is_trailing_zeros&=!vmmod10;
			vr_is_trailing_zeros&=!last_removed_digit;
			last_removed_digit=static_cast<char8_t>(vrmod10);
			vr=vrdiv10;
			vp=vpdiv10;
			vm=vmdiv10;
			++removed;
		}
		if(vm_is_trailing_zeros)
		{
			if constexpr(std::same_as<floating_type,float>)
			{
//https://github.com/ulfjack/ryu/issues/156 I am not sure whether it works. But I am going to adapt it.
				while (vm % 10 == 0)
				{
					vm /= 10;
					vr = vm;
					last_removed_digit = 0;
					++removed;
				}
			}
			else
			{
				for(;;)
				{
					mantissa_type const vmdiv10(vm/10);
					auto const vmmod10(static_cast<char8_t>(vm%10));
					if(vmmod10)
						break;
					mantissa_type const vpdiv10(vp/10);
					mantissa_type const vrdiv10(vr/10);
					auto const vrmod10(vr%10);
					vr_is_trailing_zeros&=!last_removed_digit;
					last_removed_digit=static_cast<char8_t>(vrmod10);
					vr=vrdiv10;
					vp=vpdiv10;
					vm=vmdiv10;
					++removed;
				}
			}
		}
		if(vr_is_trailing_zeros&&last_removed_digit==5&&!(vr&1))
			last_removed_digit=4;
		vr += ((vr==vm&&(!accept_bounds || !vm_is_trailing_zeros))|| 4 < last_removed_digit);
	}
	else
	{
		if constexpr(std::same_as<floating_type,float>)
		{
			for (;;)
			{
				mantissa_type const vpdiv10(vp/10);
				mantissa_type const vmdiv10(vm/10);
				if(vpdiv10<=vmdiv10)
					break;
				mantissa_type const vrdiv10(vr/10);
				auto const vrmod10(vr%10);
				last_removed_digit=vrmod10;
				vr=vrdiv10;
				vp=vpdiv10;
				vm=vmdiv10;
				++removed;
			}
			vr += (vr == vm || last_removed_digit >= 5);
		}
		else
		{
			bool round_up{};
			mantissa_type const vpdiv100(vp/100);
			mantissa_type const vmdiv100(vm/100);
			if(vmdiv100<vpdiv100)
			{
				mantissa_type const vrdiv100(vr/100);
				auto const vrmod100(vr%100);
				round_up=50<=vrmod100;
				vr=vrdiv100;
				vp=vpdiv100;
				vm=vmdiv100;
				removed+=2;
			}
			for (;;)
			{
				mantissa_type const vpdiv10(vp/10);
				mantissa_type const vmdiv10(vm/10);
				if(vpdiv10<=vmdiv10)
					break;
				mantissa_type const vrdiv10(vr/10);
				auto const vrmod10(vr%10);
				round_up=5<=vrmod10;
				vr=vrdiv10;
				vp=vpdiv10;
				vm=vmdiv10;
				++removed;
			}
			vr+=(vr==vm||round_up);
		}
	}
	if(sign)
	{
		*result=0x2d;
		++result;
	}
	if constexpr(mode==0) //general
	{
		std::int32_t olength(static_cast<std::int32_t>(chars_len<10,true>(vr)));	
		std::int32_t const real_exp(static_cast<std::int32_t>(e10 + removed + olength - 1));
		std::uint32_t fixed_length(0),this_case(0);
		if(olength<=real_exp)
		{
			fixed_length=real_exp+1;
			this_case=1;
		}
		else if(0<=real_exp&&real_exp<olength)
		{
			fixed_length=olength+2;
			if(olength==real_exp+1)
				--fixed_length;
			this_case=2;
		}
		else
			fixed_length=static_cast<exponent_type>(-real_exp)+olength+1;
		std::uint32_t scientific_length(olength==1?olength+3:olength+5);
		if(scientific_length<fixed_length)
		{
			result+=fp_output_unsigned_point(decmpt,vr,result);
			return output_exp<uppercase_e,sizeof(mantissa_type)==16>(static_cast<std::int32_t>(real_exp),result);
		}
		switch(this_case)
		{
		case 1:
			fp_output_unsigned(result,vr);
			result+=olength;
			return my_fill_n(result,real_exp+1-olength,u8'0');
		case 2:
		{
			auto a(vr);
			auto eposition(real_exp+1);
			if(olength==eposition)
			{
				fp_output_unsigned(result,a);
				result+=olength;
			}
			else
			{
				fp_output_unsigned(result+1,a);
				my_copy_n(result+1,eposition,result);
				if constexpr(is_runtime_decimal_point)
					result[eposition]=decmpt.decimal_point;
				else
					result[eposition]=decimal_point;
				result+=olength+1;
			}
			return result;
		}
		default:
			if constexpr(is_runtime_decimal_point)
			{
				*result=u8'0';
				*++result=decmpt.decimal_point;
				++result;
			}
			else
			{
				if constexpr(decimal_point==u8'.')
					result=my_copy_n(u8"0.",2,result);
				else if constexpr(decimal_point==u8',')
					result=my_copy_n(u8"0,",2,result);
				else
				{
					*result=u8'0';
					*++result=decimal_point;
					++result;
				}
			}
			result=my_fill_n(result,static_cast<exponent_type>(-real_exp-1),u8'0');
			fp_output_unsigned(result,vr);
			result+=olength;
			return result;
		}
	}
	else if constexpr(mode==1) //fixed
	{
		std::int32_t olength(static_cast<std::int32_t>(chars_len<10,true>(vr)));	
		std::int32_t const real_exp(static_cast<std::int32_t>(e10 + removed + olength - 1));
		if(olength<=real_exp)
		{
			fp_output_unsigned(result,vr);
			result+=olength;
			return my_fill_n(result,real_exp+1-olength,u8'0');	
		}
		else if(0<=real_exp&&real_exp<olength)
		{
			auto a(vr);
			auto eposition(real_exp+1);
			if(olength==eposition)
			{
				fp_output_unsigned(result,a);
				result+=olength;
			}
			else
			{
				fp_output_unsigned(result+1,a);
				my_copy_n(result+1,eposition,result);
				if constexpr(is_runtime_decimal_point)
					result[eposition]=decmpt.decimal_point;
				else
					result[eposition]=decimal_point;
				result+=olength+1;
			}
			return result;
		}
		else
		{
			if constexpr(is_runtime_decimal_point)
			{
				*result=u8'0';
				*++result=decmpt.decimal_point;
				++result;
			}
			else
			{
				if constexpr(decimal_point==u8'.')
					result=my_copy_n(u8"0.",2,result);
				else if constexpr(decimal_point==u8',')
					result=my_copy_n(u8"0,",2,result);
				else
				{
					*result=u8'0';
					*++result=decimal_point;
					++result;
				}
			}
			result=my_fill_n(result,static_cast<exponent_type>(-real_exp-1),u8'0');
			fp_output_unsigned(result,vr);
			result+=olength;
			return result;
		}
	}
	else		//scientific
	{
		auto a(vr);
		std::int32_t real_exp(static_cast<std::int32_t>(e10 + removed - 1));
		if(a<10)
		{
			std::size_t olength(fp_output_unsigned(result,a));
			real_exp+=olength;
			result+=olength;
		}
		else
		{
			std::size_t olength(fp_output_unsigned(result+1,a));
			real_exp+=static_cast<std::int32_t>(olength);
			*result=result[1];
			if constexpr(is_runtime_decimal_point)
				result[1]=decmpt.decimal_point;
			else
				result[1]=decimal_point;
			result+=olength+1;
		}
		return output_exp<uppercase_e,sizeof(mantissa_type)==16>(static_cast<std::int32_t>(real_exp),result);
	}
}

}
