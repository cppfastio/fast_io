#pragma once

namespace fast_io::details::ryu
{
template<std::floating_point floating_type,::fast_io::freestanding::contiguous_iterator Iter>
inline constexpr Iter precise(typename floating_traits<floating_type>::mantissa_type bits,Iter start) noexcept
{
	using floating_trait = floating_traits<floating_type>;
	using mantissa_type = typename floating_trait::mantissa_type;
	using exponent_type = typename floating_trait::exponent_type;
	using signed_exponent_type = std::make_signed_t<exponent_type>;
	mantissa_type const mantissa(bits & ((static_cast<mantissa_type>(1u) << floating_trait::mantissa_bits) - 1u));
	exponent_type const exponent(static_cast<exponent_type>(((bits >> floating_trait::mantissa_bits) & floating_trait::exponent_max)));
	auto const r2(init_rep<floating_type>(mantissa,static_cast<signed_exponent_type>(exponent)));
	auto r2m{r2.m};
	auto r2e{r2.e};


	exponent_type printed_digits(0),available_digits(0);
	bool const negative_r2_e(r2e<0);
	auto result{start};
	if(-52<=r2e)
	{
		exponent_type const idx(negative_r2_e?0:index_for_exponent(static_cast<exponent_type>(r2e)));
		signed_exponent_type const p10bitsmr2e(pow10_bits_for_index(idx)-r2e+8);
		auto const idx_offset(fixed_pow10<>::offset[idx]);
		for(std::size_t i(length_for_index(idx));i--;)
		{
			exponent_type digits(mul_shift_mod_1e9(r2m<<8,fixed_pow10<>::split[idx_offset+i],p10bitsmr2e));
			if(printed_digits)
			{
				append_nine_digits<0>(result,digits);
				result+=9;
				printed_digits+=9;
			}
			else if(digits)[[likely]]
			{
				available_digits=fp_output_unsigned_trans(result,digits);
				result+=available_digits;
				printed_digits = available_digits;
				available_digits = 0;
			}
		}
	}
	if(negative_r2_e&&!available_digits)
	{
		auto abs_e2(-r2e);
		exponent_type const idx(static_cast<exponent_type>(abs_e2)>>4);
		signed_exponent_type j(128+(abs_e2-(idx<<4)));
		exponent_type const of2i(fixed_pow10<>::offset_2[idx]);
		exponent_type const idxp1(fixed_pow10<>::offset_2[idx+1]);
		exponent_type const mb2_idx(fixed_pow10<>::min_block_2[idx]);
		for (exponent_type i(mb2_idx); i < 200; ++i)
		{
			exponent_type const p(of2i+i-mb2_idx);
			if(idxp1<=p)
				break;
			exponent_type digits(mul_shift_mod_1e9(r2m<<8,fixed_pow10<>::split_2[p],j));
			if(printed_digits)
			{
				append_nine_digits<0>(result,digits);
				result+=9;
				printed_digits+=9;
			}
			else if(digits)
			{
				available_digits=fp_output_unsigned_trans(result,digits);
				result+=available_digits;
				printed_digits = available_digits;
				available_digits = 0;
			}
		}
	}
	for(;start<result-1&&result[-1]==0;--result);
	*result=0;
	++result;
	return result;
}

}