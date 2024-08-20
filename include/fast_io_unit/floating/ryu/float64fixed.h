#pragma once

namespace fast_io::details
{

template<::std::integral char_type>
inline constexpr char_type* prsv_fill_zero_impl(char_type* iter, ::std::size_t n) noexcept
{
	auto ed{iter+n};
	for(;iter!=ed;++iter)
		*iter=::fast_io::char_literal_v<u8'0', char_type>;
	return ed;
}

template<bool comma, ::std::integral char_type>
inline constexpr char_type* prsv_fp_zero_precision_impl(char_type* iter, ::std::size_t precision) noexcept
{
	*iter=::fast_io::char_literal_v<u8'0', char_type>;
	++iter;
	if (!precision)
	{
		return iter;
	}
	*iter=::fast_io::char_literal_v<(comma?u8',':u8'.'), char_type>;
	++iter;
	return ::fast_io::details::prsv_fill_zero_impl(iter, precision);
}

namespace ryu
{

template<bool showpos, bool uppercase, bool comma, ::std::integral char_type, ::std::floating_point flt>
inline constexpr char_type* float642fixed_buffered_n(char_type* iter, flt f, ::std::size_t precision) noexcept
{
	using trait = ::fast_io::details::iec559_traits<flt>;
	using mantissa_type = typename trait::mantissa_type;
	constexpr ::std::size_t mbits{trait::mbits};
	constexpr ::std::size_t ebits{trait::ebits};
	constexpr ::std::uint_least32_t bias{(static_cast<::std::uint_least32_t>(1 << ebits) >> 1) - 1};
	constexpr ::std::int_least32_t exponent_bias{bias + mbits};
	constexpr mantissa_type exponent_mask{(static_cast<mantissa_type>(1) << ebits) - 1};
	constexpr mantissa_type mflags{static_cast<mantissa_type>(static_cast<mantissa_type>(1) << mbits)};
	constexpr ::std::uint_least32_t exponent_mask_u32{static_cast<::std::uint_least32_t>(exponent_mask)};
	auto [mantissa, exponent, sign] = ::fast_io::details::get_punned_result(f);
	iter = print_rsv_fp_sign_impl<showpos>(iter, sign);
	if (exponent == exponent_mask_u32)
	{
		return ::fast_io::details::prsv_fp_nan_impl<uppercase>(iter, mantissa);
	}
	if (!mantissa && !exponent)
	{
		return ::fast_io::details::prsv_fp_zero_precision_impl<comma>(iter,precision);
	}
	::std::int_least32_t e2;
	::std::uint_least64_t m2;
	if (e2 == 0) [[unlikely]]
	{
		constexpr ::std::int_least32_t e2bias{1 - static_cast<::std::int_least32_t>(exponent_bias)};
		e2 = e2bias;
	}
	else
	{
		auto e2_temp{e2};
		e2 -= exponent_bias;
		m2 |= mflags;
	}
	return iter;
}

}

}
