#pragma once
/*
Algorithm: fast_io
Author: cqwrteur
Reference: no reference
Credit: Jk jeon
*/
namespace fast_io::details
{

template<
bool showbase,
bool showbase_uppercase,
bool showpos,
bool uppercase,
bool uppercase_e,
typename flt,::std::integral char_type>
inline constexpr char_type* lc_print_rsvhexfloat_define_impl(char_type* iter,flt f,char_type const* decimal_point_base,std::size_t decimal_point_len) noexcept
{
	using trait = iec559_traits<flt>;
	using mantissa_type = typename trait::mantissa_type;
	constexpr std::size_t mbits{trait::mbits};
	constexpr std::size_t ebits{trait::ebits};
	constexpr std::uint_least32_t bias{(static_cast<std::uint_least32_t>(1<<ebits)>>1)-1};
	constexpr mantissa_type exponent_mask{(static_cast<mantissa_type>(1)<<ebits)-1};
	auto [mantissa,exponent,sign] = get_punned_result(f);
	constexpr std::uint_least32_t exponent_mask_u32{static_cast<std::uint_least32_t>(exponent_mask)};
	constexpr std::int_least32_t minus_bias{-static_cast<std::int_least32_t>(bias)};
	constexpr std::uint_least32_t makeup_bits{((mbits / 4 + 1) * 4 - mbits) % 4};//Thanks jk-jeon for the formula
	iter=print_rsv_fp_sign_impl<showpos>(iter,sign);
	if(exponent==exponent_mask_u32)
		return prsv_fp_nan_impl<uppercase>(iter,mantissa);
	if constexpr(showbase)
		iter=print_reserve_show_base_impl<16,showbase_uppercase>(iter);
	if(!mantissa&&!exponent)
		return prsv_fp_hex0p0<uppercase>(iter);
	std::int_least32_t e2{static_cast<std::int_least32_t>(exponent)+minus_bias};
	if(mantissa)
	{
		std::uint_least32_t trailing_zeros_digits{static_cast<std::uint_least32_t>(my_countr_zero_unchecked(mantissa))+makeup_bits};
		constexpr std::uint_least32_t trailing_zeros_mdigits_d4{static_cast<std::uint_least32_t>((mbits+makeup_bits)>>2)};
		std::uint_least32_t trailing_zeros_digits_d4{trailing_zeros_digits>>2};
		std::uint_least32_t trailing_zeros_digits_d4m4{trailing_zeros_digits_d4<<2};
		mantissa<<=makeup_bits;
		mantissa>>=trailing_zeros_digits_d4m4;
		std::uint_least32_t mantissa_len{trailing_zeros_mdigits_d4-trailing_zeros_digits_d4};
		if(exponent==0)
		{
			++e2;
			*iter=char_literal_v<u8'0',char_type>;
		}
		else
		{
			*iter=char_literal_v<u8'1',char_type>;
		}
		++iter;
		if(decimal_point_len==1)[[likely]]
		{
			*iter=*decimal_point_base;
			++iter;
		}
		else
		{
			iter=non_overlapped_copy_n(decimal_point_base,decimal_point_len,iter);
		}
		print_reserve_integral_main_impl<16,uppercase>(iter+=mantissa_len,mantissa,mantissa_len);
	}
	else
	{
		*iter=char_literal_v<u8'1',char_type>;
		++iter;
	}
	*iter=char_literal_v<uppercase_e?u8'P':u8'p',char_type>;
	++iter;
	return with_sign_prt_rsv_exponent_hex_impl<trait::e2hexdigits>(iter,e2);
}

}
