#pragma once

namespace fast_io::details
{

template <bool comma, ::std::integral char_type>
inline constexpr char_type *prsv_fp_zero_precision_impl(char_type *iter, ::std::size_t precision) noexcept
{
	*iter = ::fast_io::char_literal_v<u8'0', char_type>;
	++iter;
	if (!precision)
	{
		return iter;
	}
	*iter = ::fast_io::char_literal_v<(comma ? u8',' : u8'.'), char_type>;
	++iter;
	return ::fast_io::details::prsv_fill_zero_impl(iter, precision);
}

namespace ryu
{

template <bool showpos, bool uppercase, bool comma, ::std::integral char_type, ::std::floating_point flt>
inline constexpr char_type *float642fixed_buffered_n(char_type *iter, flt f, ::std::size_t precision) noexcept
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
		return ::fast_io::details::prsv_fp_zero_precision_impl<comma>(iter, precision);
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

#if 0
  int index = 0;
  bool nonzero = false;
  if (ieeeSign) {
    result[index++] = '-';
  }
  if (e2 >= -52) {
    const uint32_t idx = e2 < 0 ? 0 : indexForExponent((uint32_t) e2);
    const uint32_t p10bits = pow10BitsForIndex(idx);
    const int32_t len = (int32_t) lengthForIndex(idx);
#ifdef RYU_DEBUG
    __builtin_printf("idx=%u\n", idx);
    __builtin_printf("len=%d\n", len);
#endif
    for (int32_t i = len - 1; i >= 0; --i) {
      const uint32_t j = p10bits - e2;
      // Temporary: j is usually around 128, and by shifting a bit, we push it to 128 or above, which is
      // a slightly faster code path in mulShift_mod1e9. Instead, we can just increase the multipliers.
      const uint32_t digits = mulShift_mod1e9(m2 << 8, POW10_SPLIT[POW10_OFFSET[idx] + i], (int32_t) (j + 8));
      if (nonzero) {
        append_nine_digits(digits, result + index);
        index += 9;
      } else if (digits != 0) {
        const uint32_t olength = decimalLength9(digits);
        append_n_digits(olength, digits, result + index);
        index += olength;
        nonzero = true;
      }
    }
  }
  if (!nonzero) {
    result[index++] = '0';
  }
  if (precision > 0) {
    result[index++] = '.';
  }
#ifdef RYU_DEBUG
  __builtin_printf("e2=%d\n", e2);
#endif
  if (e2 < 0) {
    const int32_t idx = -e2 / 16;
#ifdef RYU_DEBUG
    __builtin_printf("idx=%d\n", idx);
#endif
    const uint32_t blocks = precision / 9 + 1;
    // 0 = don't round up; 1 = round up unconditionally; 2 = round up if odd.
    int roundUp = 0;
    uint32_t i = 0;
    if (blocks <= MIN_BLOCK_2[idx]) {
      i = blocks;
      memset(result + index, '0', precision);
      index += precision;
    } else if (i < MIN_BLOCK_2[idx]) {
      i = MIN_BLOCK_2[idx];
      memset(result + index, '0', 9 * i);
      index += 9 * i;
    }
    for (; i < blocks; ++i) {
      const int32_t j = ADDITIONAL_BITS_2 + (-e2 - 16 * idx);
      const uint32_t p = POW10_OFFSET_2[idx] + i - MIN_BLOCK_2[idx];
      if (p >= POW10_OFFSET_2[idx + 1]) {
        // If the remaining digits are all 0, then we might as well use memset.
        // No rounding required in this case.
        const uint32_t fill = precision - 9 * i;
        memset(result + index, '0', fill);
        index += fill;
        break;
      }
      // Temporary: j is usually around 128, and by shifting a bit, we push it to 128 or above, which is
      // a slightly faster code path in mulShift_mod1e9. Instead, we can just increase the multipliers.
      uint32_t digits = mulShift_mod1e9(m2 << 8, POW10_SPLIT_2[p], j + 8);
#ifdef RYU_DEBUG
      __builtin_printf("digits=%u\n", digits);
#endif
      if (i < blocks - 1) {
        append_nine_digits(digits, result + index);
        index += 9;
      } else {
        const uint32_t maximum = precision - 9 * i;
        uint32_t lastDigit = 0;
        for (uint32_t k = 0; k < 9 - maximum; ++k) {
          lastDigit = digits % 10;
          digits /= 10;
        }
#ifdef RYU_DEBUG
        __builtin_printf("lastDigit=%u\n", lastDigit);
#endif
        if (lastDigit != 5) {
          roundUp = lastDigit > 5;
        } else {
          // Is m * 10^(additionalDigits + 1) / 2^(-e2) integer?
          const int32_t requiredTwos = -e2 - (int32_t) precision - 1;
          const bool trailingZeros = requiredTwos <= 0
            || (requiredTwos < 60 && multipleOfPowerOf2(m2, (uint32_t) requiredTwos));
          roundUp = trailingZeros ? 2 : 1;
#ifdef RYU_DEBUG
          __builtin_printf("requiredTwos=%d\n", requiredTwos);
          __builtin_printf("trailingZeros=%s\n", trailingZeros ? "true" : "false");
#endif
        }
        if (maximum > 0) {
          append_c_digits(maximum, digits, result + index);
          index += maximum;
        }
        break;
      }
    }
#ifdef RYU_DEBUG
    __builtin_printf("roundUp=%d\n", roundUp);
#endif
    if (roundUp != 0) {
      int roundIndex = index;
      int dotIndex = 0; // '.' can't be located at index 0
      while (true) {
        --roundIndex;
        char c;
        if (roundIndex == -1 || (c = result[roundIndex], c == '-')) {
          result[roundIndex + 1] = '1';
          if (dotIndex > 0) {
            result[dotIndex] = '0';
            result[dotIndex + 1] = '.';
          }
          result[index++] = '0';
          break;
        }
        if (c == '.') {
          dotIndex = roundIndex;
          continue;
        } else if (c == '9') {
          result[roundIndex] = '0';
          roundUp = 1;
          continue;
        } else {
          if (roundUp == 2 && c % 2 == 0) {
            break;
          }
          result[roundIndex] = c + 1;
          break;
        }
      }
    }
  } else {
    memset(result + index, '0', precision);
    index += precision;
  }
#endif
	return iter;
}

} // namespace ryu

} // namespace fast_io::details
