#pragma once

namespace fast_io::details::ryu
{

template<std::floating_point floating_type>
inline constexpr typename floating_traits<floating_type>::mantissa_type me10_to_me2(typename floating_traits<floating_type>::mantissa_type m10,std::uint32_t ue10,std::uint32_t m10digits,std::int32_t dot_index,std::int32_t e_index,std::uint32_t index,bool exp_negative)
{
	using floating_trait = floating_traits<floating_type>;
	using mantissa_type = typename floating_trait::mantissa_type;
	using exponent_type = typename floating_trait::exponent_type;
	constexpr exponent_type real_bits{floating_trait::exponent_bits+floating_trait::mantissa_bits+1};
	std::int32_t e10(static_cast<std::int32_t>(ue10));
	if(exp_negative)
		e10=-e10;
	if(e_index==-1)
		e_index=index;
	if(dot_index==-1)
		dot_index=index;
	e10-=dot_index<e_index?e_index-dot_index-1:0;
	constexpr exponent_type maximum_representable_e2{(1<<floating_trait::exponent_bits)-1};
	if((static_cast<std::int32_t>(m10digits+e10)<floating_trait::minimum_exp)||(!m10))
		return {};
	if(floating_trait::maximum_exp<static_cast<std::int32_t>(m10digits+e10))
		return static_cast<mantissa_type>(static_cast<mantissa_type>(maximum_representable_e2) << floating_trait::mantissa_bits);
	bool trailing_zeros{};
	std::int32_t e2(static_cast<std::int32_t>(std::bit_width(m10))+e10-(2+floating_trait::mantissa_bits));
	mantissa_type m2{};
	if(e10<0)
	{
		auto const p5bme10(pow5bits(-e10));
		e2-=p5bme10;
		auto j{e2-e10+p5bme10-1+floating_trait::pow5_inv_bitcount};
#ifdef __SIZEOF_INT128__
		if constexpr(std::same_as<floating_type,long double>)
			m2=mul_shift_generic(m10, generic_compute_pow5_inv(-e10), j);
		else
#endif
		if constexpr(std::same_as<floating_type,float>)
			m2=mul_pow5_inv_div_pow2(m10,-e10,j);
		else
			m2=mul_shift(m10,pow5<floating_type,true>::inv_split[-e10],j);
		trailing_zeros=multiple_of_power_of_5(m10,-e10);
	}
	else
	{
		e2+=log2pow5(e10);
		auto j{e2-e10-pow5bits(e10)+floating_trait::pow5_bitcount};
#ifdef __SIZEOF_INT128__
		if constexpr(std::same_as<floating_type,long double>)
			m2=mul_shift_generic(m10, generic_compute_pow5(e10),j);
		else
#endif
		if constexpr(std::same_as<floating_type,float>)
			m2=mul_pow5_div_pow2(m10,e10,j);
		else
			m2=mul_shift(m10,pow5<floating_type,true>::split[e10],j);
		trailing_zeros = e2 < e10 || (e2 - e10 < static_cast<std::int32_t>(real_bits) && multiple_of_power_of_2(m10, e2 - e10));
	}
	std::int32_t ieee_e2(e2 + (floating_trait::bias-1) + std::bit_width(m2));
	if(ieee_e2<0)
		ieee_e2=0;
	if(static_cast<std::int32_t>(maximum_representable_e2)<=ieee_e2)[[unlikely]]
	{
		return static_cast<mantissa_type>(static_cast<mantissa_type>(maximum_representable_e2) << floating_trait::mantissa_bits);
	}
	std::int32_t shift((!ieee_e2?1:ieee_e2)-e2-(floating_trait::bias+floating_trait::mantissa_bits));
	trailing_zeros &= !(m2 & ((static_cast<mantissa_type>(1) << (shift - 1)) - 1));
	bool last_removed_bit((m2>>(shift-1))&1);
	bool round_up((last_removed_bit) && (!trailing_zeros || ((m2 >> shift) & 1)));
	mantissa_type ieee_m2((m2 >> shift) + round_up);
	if(std::same_as<floating_type,float>)
	{
		ieee_m2 &= ((static_cast<mantissa_type>(1) << floating_trait::mantissa_bits) - 1);
		if (ieee_m2 == 0 && round_up)
			++ieee_e2;
	}
	else
	{
		if(ieee_m2 == (static_cast<mantissa_type>(1) << (floating_trait::mantissa_bits + 1)))
			++ieee_e2;
		ieee_m2&=((static_cast<mantissa_type>(1) << floating_trait::mantissa_bits) - 1);
	}
	return (static_cast<mantissa_type>(ieee_e2) << floating_trait::mantissa_bits)|
		(((m2 >> shift) + round_up) & ((static_cast<mantissa_type>(1) << floating_trait::mantissa_bits) - 1));
}

/*
https://baseconvert.com/ieee-754-floating-point
*/
template<char32_t decimal_point,std::floating_point F,typename It_First,typename It_Second>
inline constexpr F input_floating(It_First iter,It_Second ed)
{
	using char_type = std::remove_cvref_t<decltype(*iter)>;
	using floating_type = std::remove_cvref_t<F>;
	using floating_trait = floating_traits<floating_type>;
	using mantissa_type = typename floating_trait::mantissa_type;
	using exponent_type = typename floating_trait::exponent_type;
	using unsigned_char_type = std::make_unsigned_t<char_type>;
	using signed_exponent_type = std::make_signed_t<exponent_type>;
	constexpr unsigned_char_type decimal_point_value_after_minus_zero{static_cast<unsigned_char_type>(static_cast<unsigned_char_type>(decimal_point)-u8'0')}; 
	//.(46)-48: static_cast<unsigned_char_type>(-2)
	//-(45)-48: static_cast<unsigned_char_type>(-3)
	//'E'(69)-48: 21
	//'e'(101)-48: 53
	//We do not support EBCDIC
	constexpr exponent_type real_bits{floating_trait::exponent_bits+floating_trait::mantissa_bits+1};
	bool negative{};
	if(*iter==u8'-')
	{
		negative=true;
		if(++iter==ed)
			return bit_cast<F>(static_cast<mantissa_type>(static_cast<mantissa_type>(negative) << (real_bits-1)));
	}
	else if(*iter==u8'+')[[unlikely]]
	{
		if(++iter==ed)
			return {};
	}
	
	for(;iter!=ed&&*iter==u8'0';++iter);
	exponent_type m10digits{};
	mantissa_type m10{};
	signed_exponent_type dot_index{-1};
	exponent_type index{};
	stack_arbitary_int<1024> buffer;//To change with long double
	for(;iter!=ed&&m10digits!=floating_trait::digits10;++iter)
	{
		unsigned_char_type const ch(*iter-u8'0');
		if(0x9<ch)[[unlikely]]
		{
			if(ch==decimal_point_value_after_minus_zero)
			{
				if(dot_index!=-1)
#ifdef __cpp_exceptions
					throw fast_io_text_error("malformed input");
#else
					fast_terminate();
#endif
				dot_index=index;
				++index;
				continue;
			}
			else
				break;

		}
		
		m10*=10;
		if(m10+=ch)[[likely]]
		{
			++m10digits;
			buffer.push_back_unchecked(ch);
		}
		++index;
	}
	std::size_t extra_e10{};
	bool need_verify{};
	bool non_zero_remain{};
	if(m10digits==floating_trait::digits10)[[unlikely]]
	{
		bool find_decimal_point{};
		if(dot_index==-1)
		{
			for(;iter!=ed&&*iter==u8'0';++iter)
				++extra_e10;
			if(iter!=ed&&*iter==decimal_point)
			{
				++iter;
				find_decimal_point=true;
			}
		}
		std::size_t zero{0};
		for(;iter!=ed&&*iter==u8'0';++iter)
			++zero;
		if(iter!=ed&&static_cast<unsigned_char_type>(*iter-u8'1')<9)[[unlikely]]
		{
			need_verify=true;
			std::size_t t_m10e{extra_e10+m10digits+zero};
			if(floating_trait::required_buffer_size<t_m10e)
			{
				buffer.final_zeros();
				non_zero_remain=true;
			}
			else
			{
//				::debug_println(extra_e10+zero);
				buffer.fillnz(extra_e10+zero);
			}
			if(dot_index==-1&&!find_decimal_point)
			{
				for(;iter!=ed&&!buffer.full();++iter)
				{
					unsigned_char_type const ch(*iter-u8'0');
					if(9<ch)[[unlikely]]
						break;
					buffer.push_back_unchecked(ch);
					++extra_e10;
				}
				if(buffer.full())[[unlikely]]
				{
					for(;iter!=ed;++iter)
					{
						unsigned_char_type const ch(*iter-u8'0');
						if(9<ch)[[unlikely]]
							break;
						++extra_e10;
						non_zero_remain|=ch;
					}
				}
				if(iter!=ed&&*iter==decimal_point)
					++iter;
			}
			for(;iter!=ed&&!buffer.full();++iter)
			{
				unsigned_char_type const ch(*iter-u8'0');
				if(9<ch)[[unlikely]]
					break;
				buffer.push_back_unchecked(ch);
			}
			if(buffer.full())
				for(;iter!=ed;++iter)
				{
					unsigned_char_type const ch(*iter-u8'0');
					if(9<ch)[[unlikely]]
						break;
					non_zero_remain|=ch;
				}
		}
	}
	signed_exponent_type e_index{-1};
	bool exp_negative{};
	exponent_type ue10{};
	std::size_t ue10digits{};
	if(iter!=ed&&((*iter==u8'e')||(*iter==u8'E')))
	{
		e_index=index;
		if(++iter==ed)[[unlikely]]
			throw_malformed_input();
		++index;
		if((*iter==u8'+')||(*iter==u8'-'))[[likely]]
		{
			exp_negative=(*iter==u8'-');
			if(++iter==ed)[[unlikely]]
				throw_malformed_input();
			++index;
		}
		for(;iter!=ed&&*iter==u8'0';++iter)
			++index;
		for(;iter!=ed;++iter)
		{
			unsigned_char_type const ch(*iter-u8'0');
			if(0x9<ch)[[unlikely]]
				break;
			ue10*=10;
			ue10+=ch;
			++ue10digits;
			++index;
		}
	}
	if((6<ue10digits)|((ue10+=extra_e10)<extra_e10))[[unlikely]]
		throw_input_overflow_error();
	auto fl{me10_to_me2<F>(m10,ue10,m10digits,dot_index,e_index,index,exp_negative)};
	if(need_verify)[[unlikely]]
	{
		auto cl{me10_to_me2<F>(m10+1,ue10,m10digits,dot_index,e_index,index,exp_negative)};
		if(fl==cl)
			return bit_cast<F>(((static_cast<mantissa_type>(negative)) << (real_bits-1))|fl);
		else
		{

			if constexpr(std::same_as<double,floating_type>)
			{
				stack_arbitary_int<1024> fl_buffer;
				fl_buffer.position=precise<floating_type>(fl,fl_buffer.data())-fl_buffer.data();
				stack_arbitary_int<1024> cl_buffer;
				cl_buffer.position=precise<floating_type>(cl,cl_buffer.data())-cl_buffer.data();
//				::debug_println("before:\ncl_buffer (LARGER):",cl_buffer,"\nbuffer:",buffer,"\nfl_buffer (SMALLER):",fl_buffer);
				std::size_t cl_buffer_size{cl_buffer.size()};
				bool const roundup{cl_buffer.digits.front()==1&&fl_buffer.digits.front()==9};
				if(roundup)
					--cl_buffer_size;
				std::size_t larger_buffer_size;
				if(cl_buffer_size<fl_buffer.size())
				{
					larger_buffer_size=fl_buffer.size();
					::fast_io::details::my_memset(cl_buffer.data()+cl_buffer.size(),0,fl_buffer.size()-cl_buffer_size);
					cl_buffer.position+=fl_buffer.size()-cl_buffer_size;
				}
				else
				{
					larger_buffer_size=cl_buffer_size;
					::fast_io::details::my_memset(fl_buffer.data()+fl_buffer.size(),0,cl_buffer_size-fl_buffer.size());
					fl_buffer.position+=cl_buffer_size-fl_buffer.size();
				}
				if(larger_buffer_size<buffer.size())
				{
					if(!non_zero_remain)
					{
						std::size_t i{larger_buffer_size};
						for(;i!=buffer.size();++i)
							if(buffer.digits[i])
							{
								non_zero_remain=true;
								break;
							}
					}
					buffer.position=larger_buffer_size;
				}
				else
				{
					std::size_t const to_set{larger_buffer_size-buffer.size()};
					::fast_io::details::my_memset(buffer.data()+buffer.size(),0,to_set);
					buffer.position+=to_set;
				}
//				::debug_println("\n\nafter:\ncl_buffer (LARGER):",cl_buffer,"\nbuffer:",buffer,"\nfl_buffer (SMALLER):",fl_buffer);

				if(!fake_minus_assignment(cl_buffer,buffer))
					return bit_cast<F>(((static_cast<mantissa_type>(negative)) << (real_bits-1)) | cl);
				if(!fake_minus_assignment(buffer,fl_buffer))
					return bit_cast<F>(((static_cast<mantissa_type>(negative)) << (real_bits-1)) | fl);
//				::debug_println("\n\nafter:\ncl_buffer (LARGER):",cl_buffer,"\nbuffer:",buffer,"\nfl_buffer (SMALLER):",fl_buffer);
				int res{memcmp(cl_buffer.data()+roundup,buffer.data(),larger_buffer_size)};
//				::debug_println("res=",res);
				if(res<0)
					return bit_cast<F>(((static_cast<mantissa_type>(negative)) << (real_bits-1)) | cl);
				else if(0<res)
					return bit_cast<F>(((static_cast<mantissa_type>(negative)) << (real_bits-1)) | fl);
				else
				{
					if(non_zero_remain||fl&1)
						return bit_cast<F>(((static_cast<mantissa_type>(negative)) << (real_bits-1)) | cl);
					return bit_cast<F>(((static_cast<mantissa_type>(negative)) << (real_bits-1)) | fl);
				}
			}
			else
			{
				throw fast_io_text_error("ryu to do with multiprecisions");
			}
		}
	}
	else
		return bit_cast<F>(((static_cast<mantissa_type>(negative)) << (real_bits-1)) | fl);
}

}
