#pragma once

namespace fast_io::details::fp
{

template<bool uppercase_e=false,bool four_digits=false,::fast_io::freestanding::random_access_iterator Iter>
inline constexpr Iter output_exp(std::int32_t exp,Iter result)
{
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
	if constexpr(uppercase_e)
	{
		if constexpr(std::same_as<char_type,char>)
			*result='E';
		else if constexpr(std::same_as<char_type,wchar_t>)
			*result=L'E';
		else
			*result=u8'E';
	}
	else
	{
		if constexpr(std::same_as<char_type,char>)
			*result='e';
		else if constexpr(std::same_as<char_type,wchar_t>)
			*result=L'e';
		else
			*result=u8'e';
	}
	++result;
	if(exp<0)
	{
		if constexpr(std::same_as<char_type,char>)
			*result='-';
		else if constexpr(std::same_as<char_type,wchar_t>)
			*result=L'-';
		else
			*result=u8'-';
		exp=-exp;
	}
	else
	{
		if constexpr(std::same_as<char_type,char>)
			*result='+';
		else if constexpr(std::same_as<char_type,wchar_t>)
			*result=L'+';
		else
			*result=u8'+';
	}
	++result;
	std::uint32_t unsigned_exp(exp);
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
		std::uint32_t const temp(static_cast<std::uint32_t>(unsigned_exp/10));
		char_type const res(static_cast<char_type>(unsigned_exp%10));
		if constexpr(std::same_as<char_type,char>)
			*--str=static_cast<char_type>('0'+res);
		else if constexpr(std::same_as<char_type,wchar_t>)
			*--str=static_cast<char_type>(L'0'+res);
		else
			*--str=static_cast<char_type>(u8'0'+res);
		unsigned_exp=temp;
	}
	return result+len;
#elif defined(FAST_IO_OPTIMIZE_TIME)
	if constexpr(four_digits)
	{
	if(1000<=unsigned_exp)[[unlikely]]
		return non_overlapped_copy_n(jiaendu::static_tables<char_type>::table4[unsigned_exp].data(),4,result);
	}
	if(100<=unsigned_exp)[[unlikely]]
		return non_overlapped_copy_n(jiaendu::static_tables<char_type>::table3[unsigned_exp].data(),3,result);
	return non_overlapped_copy_n(jiaendu::static_tables<char_type>::table2[unsigned_exp].data(),2,result);
#else
	constexpr auto tb(get_shared_inline_constexpr_base_table<char_type,10,false>().data());
	if constexpr(four_digits)
	{
	if(1000<=unsigned_exp)[[unlikely]]
	{
		auto u{unsigned_exp%100u};
		auto v{unsigned_exp/100u};
		non_overlapped_copy_n(tb[v].data(),2,result);
		non_overlapped_copy_n(tb[u].data(),2,result+2);
		return result+4;
	}
	}
	if(100<=unsigned_exp)[[unlikely]]
	{
		auto u{unsigned_exp%100u};
		if constexpr(std::same_as<char_type,char>)
			*result=static_cast<char_type>(unsigned_exp/100u+'0');
		else if constexpr(std::same_as<char_type,wchar_t>)
			*result=static_cast<char_type>(unsigned_exp/100u+L'0');
		else
			*result=static_cast<char_type>(unsigned_exp/100u+u8'0');
		non_overlapped_copy_n(tb[u].data(),2,result+1);
		return result+3;
	}
	return non_overlapped_copy_n(tb[unsigned_exp].data(),2,result);
#endif
}

template<::fast_io::freestanding::random_access_iterator Iter,my_unsigned_integral mantissaType>
inline constexpr Iter nan_inf_case(Iter result,bool sign, mantissaType mantissa) noexcept
{
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
	if (mantissa)
	{
		if constexpr(std::same_as<char,char_type>)
			return copy_string_literal("nan",result);
		else if constexpr(std::same_as<wchar_t,char_type>)
			return copy_string_literal(L"nan",result);
		else if constexpr(std::same_as<char16_t,char_type>)
			return copy_string_literal(u"nan",result);
		else if constexpr(std::same_as<char32_t,char_type>)
			return copy_string_literal(U"nan",result);
		else
			return copy_string_literal(u8"nan",result);
	}
	if (sign)
	{
		if constexpr(std::same_as<char,char_type>)
			return copy_string_literal("-inf",result);
		else if constexpr(std::same_as<wchar_t,char_type>)
			return copy_string_literal(L"-inf",result);
		else if constexpr(std::same_as<char16_t,char_type>)
			return copy_string_literal(u"-inf",result);
		else if constexpr(std::same_as<char32_t,char_type>)
			return copy_string_literal(U"-inf",result);
		else
			return copy_string_literal(u8"-inf",result);
	}
	if constexpr(std::same_as<char,char_type>)
		return copy_string_literal("inf",result);
	else if constexpr(std::same_as<wchar_t,char_type>)
		return copy_string_literal(L"inf",result);
	else if constexpr(std::same_as<char16_t,char_type>)
		return copy_string_literal(u"inf",result);
	else if constexpr(std::same_as<char32_t,char_type>)
		return copy_string_literal(U"inf",result);
	else
		return copy_string_literal(u8"inf",result);
}


template<::fast_io::freestanding::random_access_iterator Iter>
inline constexpr Iter sign_case(Iter result,bool sign) noexcept
{
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
	if(sign)
	{
		if constexpr(std::same_as<char,char_type>)
			*result='-';
		else if constexpr(std::same_as<wchar_t,char_type>)
			*result=L'-';
		else
			*result=u8'-';
		++result;
	}
	return result;
}

template<manipulators::floating_representation flt_mode,bool uppercase_e,::fast_io::freestanding::random_access_iterator Iter>
inline constexpr Iter zero_case(Iter result,bool sign) noexcept
{
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
	if constexpr(flt_mode==manipulators::floating_representation::scientific)
	{
		if constexpr(uppercase_e)
		{
			if(sign)
			{
				if constexpr(std::same_as<char,char_type>)
					return copy_string_literal("-0E+00",result);
				else if constexpr(std::same_as<wchar_t,char_type>)
					return copy_string_literal(L"-0E+00",result);
				else if constexpr(std::same_as<char16_t,char_type>)
					return copy_string_literal(u"-0E+00",result);
				else if constexpr(std::same_as<char32_t,char_type>)
					return copy_string_literal(U"-0E+00",result);
				else
					return copy_string_literal(u8"-0E+00",result);
			}
			if constexpr(std::same_as<char,char_type>)
				return copy_string_literal("0E+00",result);
			else if constexpr(std::same_as<wchar_t,char_type>)
				return copy_string_literal(L"0E+00",result);
			else if constexpr(std::same_as<char16_t,char_type>)
				return copy_string_literal(u"0E+00",result);
			else if constexpr(std::same_as<char32_t,char_type>)
				return copy_string_literal(U"0E+00",result);
			else
				return copy_string_literal(u8"0E+00",result);
		}
		else
		{
			if(sign)
			{
				if constexpr(std::same_as<char,char_type>)
					return copy_string_literal("-0e+00",result);
				else if constexpr(std::same_as<wchar_t,char_type>)
					return copy_string_literal(L"-0e+00",result);
				else if constexpr(std::same_as<char16_t,char_type>)
					return copy_string_literal(u"-0e+00",result);
				else if constexpr(std::same_as<char32_t,char_type>)
					return copy_string_literal(U"-0e+00",result);
				else
					return copy_string_literal(u8"-0e+00",result);
			}
			if constexpr(std::same_as<char,char_type>)
				return copy_string_literal("0e+00",result);
			else if constexpr(std::same_as<wchar_t,char_type>)
				return copy_string_literal(L"0e+00",result);
			else if constexpr(std::same_as<char16_t,char_type>)
				return copy_string_literal(u"0e+00",result);
			else if constexpr(std::same_as<char32_t,char_type>)
				return copy_string_literal(U"0e+00",result);
			else
				return copy_string_literal(u8"0e+00",result);
		}
	}
	else if constexpr(flt_mode==manipulators::floating_representation::general&&uppercase_e)
	{
		result=sign_case(result,sign);
		if constexpr(std::same_as<char,char_type>)
			*result='0';
		else if constexpr(std::same_as<wchar_t,char_type>)
			*result=L'0';
		else
			*result=u8'0';
		++result;
		return result;
	}
	else
		return zero_case<manipulators::floating_representation::general,true>(result,sign);
}

template<manipulators::floating_representation flt_mode,bool uppercase_e,char8_t decimal_point,::fast_io::freestanding::random_access_iterator Iter,my_unsigned_integral mantissa_type>
inline constexpr Iter integer_case_no_sign(Iter result,mantissa_type v2) noexcept
{
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
	if constexpr(flt_mode==manipulators::floating_representation::general)
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
						if constexpr(std::same_as<char_type,char>)
							*result=static_cast<char_type>(v3+'0');
						else if constexpr(std::same_as<char_type,wchar_t>)
							*result=static_cast<char_type>(v3+L'0');
						else
							*result=static_cast<char_type>(v3+u8'0');
						++result;
					}
					else
					{
						fp_output_unsigned(result+1,v3);
						*result=result[1];
						if constexpr(decimal_point==u8'.')
						{
							if constexpr(std::same_as<char_type,char>)
								result[1]='.';
							else if constexpr(std::same_as<char_type,wchar_t>)
								result[1]=L'.';
							else
								result[1]=u8'.';
						}
						else if constexpr(decimal_point==u8',')
						{
							if constexpr(std::same_as<char_type,char>)
								result[1]=',';
							else if constexpr(std::same_as<char_type,wchar_t>)
								result[1]=L',';
							else
								result[1]=u8',';
						}
						result+=v3_len+1;
					}
					if constexpr(uppercase_e)
					{
						if constexpr(std::same_as<char,char_type>)
							result=copy_string_literal("E+",result);
						else if constexpr(std::same_as<wchar_t,char_type>)
							result=copy_string_literal(L"E+",result);
						else if constexpr(std::same_as<char16_t,char_type>)
							result=copy_string_literal(u"E+",result);
						else if constexpr(std::same_as<char32_t,char_type>)
							result=copy_string_literal(U"E+",result);
						else
							result=copy_string_literal(u8"E+",result);
					}
					else
					{
						if constexpr(std::same_as<char,char_type>)
							result=copy_string_literal("e+",result);
						else if constexpr(std::same_as<wchar_t,char_type>)
							result=copy_string_literal(L"e+",result);
						else if constexpr(std::same_as<char16_t,char_type>)
							result=copy_string_literal(u"e+",result);
						else if constexpr(std::same_as<char32_t,char_type>)
							result=copy_string_literal(U"e+",result);
						else
							result=copy_string_literal(u8"e+",result);
					}
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
			if constexpr(std::same_as<char_type,char>)
				*result='0';
			else if constexpr(std::same_as<char_type,wchar_t>)
				*result=L'0';
			else
				*result=u8'0';
			++result;
		}
	}
	else if constexpr(flt_mode==manipulators::floating_representation::fixed)
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
		result+=fp_output_unsigned_point_no_dcm<decimal_point>(v2,result);
		if constexpr(uppercase_e)
		{
			if constexpr(std::same_as<char,char_type>)
				result=copy_string_literal("E+",result);
			else if constexpr(std::same_as<wchar_t,char_type>)
				result=copy_string_literal(L"E+",result);
			else if constexpr(std::same_as<char16_t,char_type>)
				result=copy_string_literal(u"E+",result);
			else if constexpr(std::same_as<char32_t,char_type>)
				result=copy_string_literal(U"E+",result);
			else
				result=copy_string_literal(u8"E+",result);
		}
		else
		{
			if constexpr(std::same_as<char,char_type>)
				result=copy_string_literal("e+",result);
			else if constexpr(std::same_as<wchar_t,char_type>)
				result=copy_string_literal(L"e+",result);
			else if constexpr(std::same_as<char16_t,char_type>)
				result=copy_string_literal(u"e+",result);
			else if constexpr(std::same_as<char32_t,char_type>)
				result=copy_string_literal(U"e+",result);
			else
				result=copy_string_literal(u8"e+",result);
		}
		fp_output_two_digits(result,v2_len-1);
		result+=2;
	}
	return result;
}

template<manipulators::floating_representation flt_mode,bool uppercase_e,char8_t decimal_point,::fast_io::freestanding::random_access_iterator Iter,my_unsigned_integral muint>
inline constexpr Iter integer_case(Iter result,bool sign,muint v2) noexcept
{
	return integer_case_no_sign<flt_mode,uppercase_e,decimal_point>(sign_case(result,sign),v2);
}

template<manipulators::floating_representation flt_mode,bool uppercase_e,char8_t decimal_point,::fast_io::freestanding::random_access_iterator Iter,my_unsigned_integral mantissa_type,
		my_signed_integral exponent_type>
inline constexpr Iter normal_case_no_sign(Iter result,mantissa_type vr,exponent_type e10) noexcept
{
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
	if constexpr(flt_mode==manipulators::floating_representation::general) //general
	{
		std::int32_t olength(static_cast<std::int32_t>(chars_len<10,true>(vr)));	
		std::int32_t const real_exp(static_cast<std::int32_t>(e10 
/*
		+ removed
*/
		 + olength - 1));
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
			result+=fp_output_unsigned_point_no_dcm<decimal_point>(vr,result);
			return output_exp<uppercase_e,sizeof(mantissa_type)==16>(static_cast<std::int32_t>(real_exp),result);
		}
		switch(this_case)
		{
		case 1:
			fp_output_unsigned(result,vr);
			result+=olength;
			if constexpr(std::same_as<char_type,char>)
				return my_fill_n(result,real_exp+1-olength,'0');
			else if constexpr(std::same_as<char_type,wchar_t>)
				return my_fill_n(result,real_exp+1-olength,L'0');
			else if constexpr(std::same_as<char_type,char16_t>)
				return my_fill_n(result,real_exp+1-olength,u'0');
			else if constexpr(std::same_as<char_type,char32_t>)
				return my_fill_n(result,real_exp+1-olength,U'0');
			else
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
				if constexpr(decimal_point==u8'.')
				{
				if constexpr(std::same_as<char_type,char>)
					result[eposition]='.';
				else if constexpr(std::same_as<char_type,wchar_t>)
					result[eposition]=L'.';
				else
					result[eposition]=u8'.';
				}
				else
				{
				if constexpr(std::same_as<char_type,char>)
					result[eposition]=',';
				else if constexpr(std::same_as<char_type,wchar_t>)
					result[eposition]=L',';
				else
					result[eposition]=u8',';
				}
				result+=olength+1;
			}
			return result;
		}
		default:
			if constexpr(decimal_point==u8'.')
			{
			if constexpr(std::same_as<char_type,char>)
				result=copy_string_literal("0.",result);
			else if constexpr(std::same_as<char_type,wchar_t>)
				result=copy_string_literal(L"0.",result);
			else if constexpr(std::same_as<char_type,char16_t>)
				result=copy_string_literal(u"0.",result);
			else if constexpr(std::same_as<char_type,char32_t>)
				result=copy_string_literal(U"0.",result);
			else
				result=copy_string_literal(u8"0.",result);
			}
			else
			{
			if constexpr(std::same_as<char_type,char>)
				result=copy_string_literal("0,",result);
			else if constexpr(std::same_as<char_type,wchar_t>)
				result=copy_string_literal(L"0,",result);
			else if constexpr(std::same_as<char_type,char16_t>)
				result=copy_string_literal(u"0,",result);
			else if constexpr(std::same_as<char_type,char32_t>)
				result=copy_string_literal(U"0,",result);
			else
				result=copy_string_literal(u8"0,",result);
			}
			if constexpr(std::same_as<char_type,char>)
				result=my_fill_n(result,static_cast<exponent_type>(-real_exp-1),'0');
			else if constexpr(std::same_as<char_type,wchar_t>)
				result=my_fill_n(result,static_cast<exponent_type>(-real_exp-1),L'0');
			else if constexpr(std::same_as<char_type,char16_t>)
				result=my_fill_n(result,static_cast<exponent_type>(-real_exp-1),u'0');
			else if constexpr(std::same_as<char_type,char32_t>)
				result=my_fill_n(result,static_cast<exponent_type>(-real_exp-1),U'0');
			else
				result=my_fill_n(result,static_cast<exponent_type>(-real_exp-1),u8'0');
			fp_output_unsigned(result,vr);
			result+=olength;
			return result;
		}
	}
	else if constexpr(flt_mode==manipulators::floating_representation::fixed) //fixed
	{
		std::int32_t olength(static_cast<std::int32_t>(chars_len<10,true>(vr)));	
		std::int32_t const real_exp(static_cast<std::int32_t>(e10 
/*
		+ removed
*/

		 + olength - 1));
		if(olength<=real_exp)
		{
			fp_output_unsigned(result,vr);
			result+=olength;
			if constexpr(std::same_as<char_type,char>)
				return my_fill_n(result,real_exp+1-olength,'0');
			else if constexpr(std::same_as<char_type,wchar_t>)
				return my_fill_n(result,real_exp+1-olength,L'0');
			else if constexpr(std::same_as<char_type,char16_t>)
				return my_fill_n(result,real_exp+1-olength,u'0');
			else if constexpr(std::same_as<char_type,char32_t>)
				return my_fill_n(result,real_exp+1-olength,U'0');
			else
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
				if constexpr(std::same_as<char_type,char>)
					result[eposition]=decimal_point;
				else if constexpr(std::same_as<char_type,wchar_t>)
					result[eposition]=decimal_point;
				else if constexpr(std::same_as<char_type,char16_t>)
					result[eposition]=decimal_point;
				else if constexpr(std::same_as<char_type,char32_t>)
					result[eposition]=decimal_point;
				else
					result[eposition]=decimal_point;
				result+=olength+1;
			}
			return result;
		}
		else
		{
			if constexpr(decimal_point==u8'.')
			{
				if constexpr(std::same_as<char_type,char>)
					result=copy_string_literal("0.",result);
				else if constexpr(std::same_as<char_type,wchar_t>)
					result=copy_string_literal(L"0.",result);
				else if constexpr(std::same_as<char_type,char16_t>)
					result=copy_string_literal(u"0.",result);
				else if constexpr(std::same_as<char_type,char32_t>)
					result=copy_string_literal(U"0.",result);
				else
					result=copy_string_literal(u8"0.",result);
			}
			else if constexpr(decimal_point==u8',')
			{
				if constexpr(std::same_as<char_type,char>)
					result=copy_string_literal("0,",result);
				else if constexpr(std::same_as<char_type,wchar_t>)
					result=copy_string_literal(L"0,",result);
				else if constexpr(std::same_as<char_type,char16_t>)
					result=copy_string_literal(u"0,",result);
				else if constexpr(std::same_as<char_type,char32_t>)
					result=copy_string_literal(U"0,",result);
				else
					result=copy_string_literal(u8"0,",result);
			}
			if constexpr(std::same_as<char_type,char>)
				result=my_fill_n(result,static_cast<exponent_type>(-real_exp-1),'0');
			else if constexpr(std::same_as<char_type,wchar_t>)
				result=my_fill_n(result,static_cast<exponent_type>(-real_exp-1),L'0');
			else if constexpr(std::same_as<char_type,char16_t>)
				result=my_fill_n(result,static_cast<exponent_type>(-real_exp-1),u'0');
			else if constexpr(std::same_as<char_type,char32_t>)
				result=my_fill_n(result,static_cast<exponent_type>(-real_exp-1),U'0');
			else
				result=my_fill_n(result,static_cast<exponent_type>(-real_exp-1),u8'0');
			fp_output_unsigned(result,vr);
			result+=olength;
			return result;
		}
	}
	else		//scientific
	{
		std::int32_t real_exp(static_cast<std::int32_t>(e10
/*
		+ removed
*/

		 - 1));
		if(vr<10)
		{
			std::uint32_t olength(static_cast<std::uint32_t>(fp_output_unsigned(result,vr)));
			real_exp+=olength;
			result+=olength;
		}
		else
		{
			std::uint32_t olength(static_cast<std::uint32_t>(fp_output_unsigned(result+1,vr)));
			real_exp+=olength;
			*result=result[1];
			if constexpr(decimal_point==u8'.')
			{
			if constexpr(std::same_as<char_type,char>)
				result[1]='.';
			else if constexpr(std::same_as<char_type,wchar_t>)
				result[1]=L'.';
			else
				result[1]=u8'.';
			}
			else
			{
			if constexpr(std::same_as<char_type,char>)
				result[1]=',';
			else if constexpr(std::same_as<char_type,wchar_t>)
				result[1]=L',';
			else
				result[1]=u8',';
			}
			result+=olength+1;
		}
		return output_exp<uppercase_e,sizeof(mantissa_type)==16>(static_cast<std::int32_t>(real_exp),result);
	}
}

template<manipulators::floating_representation flt_mode,bool uppercase_e,char8_t decimal_point,::fast_io::freestanding::random_access_iterator Iter,my_unsigned_integral muint,
		my_signed_integral exp_type>
inline constexpr Iter normal_case(Iter result,bool sign,muint vr,exp_type e10) noexcept
{
	return normal_case_no_sign<flt_mode,uppercase_e,decimal_point>(sign_case(result,sign),vr,e10);
}

}