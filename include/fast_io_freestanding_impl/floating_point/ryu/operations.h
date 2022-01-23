#pragma once

namespace fast_io::details::ryu
{

// It seems to be slightly faster to avoid uint128_t here, although the
// generated code for uint128_t looks slightly nicer.
constexpr inline std::uint32_t mul_shift_32(std::uint32_t const m, std::uint64_t const factor, std::int32_t const shift)
{
  // The casts here help MSVC to avoid calls to the __allmul library
  // function.
	std::uint32_t const factor_low{static_cast<std::uint32_t>(factor)};
	std::uint32_t const factor_high{static_cast<std::uint32_t>(factor >> 32)};
	std::uint64_t const bits0{static_cast<std::uint64_t>(m) * factor_low};
	std::uint64_t const bits1{static_cast<std::uint64_t>(m) * factor_high};

	if constexpr(sizeof(std::uintptr_t)<8)
	{
		// On 32-bit platforms we can avoid a 64-bit shift-right since we only
		// need the upper 32 bits of the result and the shift value is > 32.
		std::uint32_t const bits0_high(static_cast<std::uint32_t>(bits0 >> 32));
		std::uint32_t bits1_low(static_cast<std::uint32_t>(bits1));
		std::uint32_t bits1_high(static_cast<std::uint32_t>(bits1 >> 32));
		bits1_low += bits0_high;
		bits1_high += (bits1_low < bits0_high);
		std::int32_t const s(shift - 32);
		return (bits1_high << (32 - s)) | (bits1_low >> s);
 	}
	else
	{
		std::uint64_t const sum((bits0 >> 32) + bits1);
		std::uint64_t const shifted_sum(sum >> (shift - 32));
		return static_cast<std::uint32_t>(shifted_sum);
	}
}

constexpr inline std::uint32_t mul_pow5_inv_div_pow2(std::uint32_t const m, std::uint32_t const q, std::int32_t const j)
{
	return mul_shift_32(m, pow5<float,true>::inv_split[q], j);
}

constexpr inline std::uint32_t mul_pow5_div_pow2(std::uint32_t const m, std::uint32_t const i, std::int32_t const j)
{
	return mul_shift_32(m, pow5<float,true>::split[i], j);
}
template<std::integral U>
inline constexpr std::int32_t log2pow5(U e)
{
	return static_cast<int32_t>(((static_cast<std::uint32_t>(e) * 1217359) >> 19));
}

template<bool generic=false>
inline constexpr auto pow5bits(std::uint32_t e)
{
	if constexpr(generic)
		return static_cast<std::uint32_t> (((e * 163391164108059ull) >> 46) + 1);
	else
		return static_cast<std::int32_t>(log2pow5(e)+1);
}
#ifdef __SIZEOF_INT128__

constexpr inline ::fast_io::freestanding::array<std::uint64_t,4> mul_128_256_shift(
		::fast_io::freestanding::array<std::uint64_t,2> const& a,::fast_io::freestanding::array<std::uint64_t,4> const& b,
		std::uint32_t const shift,std::uint32_t const corr)
{
	__uint128_t a0{a.front()};
	__uint128_t a1{a[1]};
	__uint128_t const b00(a0 * b[0]); // 0
	__uint128_t const b01(a0 * b[1]); // 64
	__uint128_t const b02(a0 * b[2]); // 128
	__uint128_t const b03(a0 * b[3]); // 196
	__uint128_t const b10(a1 * b[0]); // 64
	__uint128_t const b11(a1 * b[1]); // 128
	__uint128_t const b12(a1 * b[2]); // 196
	__uint128_t const b13(a1 * b[3]); // 256

	__uint128_t const s0(b00);       // 0   x
	__uint128_t const s1(b01 + b10); // 64  x
	__uint128_t const c1(s1 < b01);  // 196 x
	__uint128_t const s2(b02 + b11); // 128 x
	__uint128_t const c2(s2 < b02);  // 256 x
	__uint128_t const s3(b03 + b12); // 196 x
	__uint128_t const c3(s3 < b03);  // 324 x

	__uint128_t const p0(s0 + (s1 << 64));                                // 0
	__uint128_t const d0(p0 < b00);                                       // 128
	__uint128_t const q1(s2 + (s1 >> 64) + (s3 << 64));                   // 128
	__uint128_t const d1(q1 < s2);                                        // 256
	__uint128_t const p1(q1 + (c1 << 64) + d0);                           // 128
	__uint128_t const d2(p1 < q1);                                        // 256
	__uint128_t const p2(b13 + (s3 >> 64) + c2 + (c3 << 64) + d1 + d2);   // 256
	if (shift < 128)
	{
		__uint128_t const r0{corr + ((p0 >> shift) | (p1 << (128 - shift)))};
		__uint128_t const r1{((p1 >> shift) | (p2 << (128 - shift))) + (r0 < corr)};
		return {static_cast<std::uint64_t>(r0),static_cast<std::uint64_t>(r0>>64),
		static_cast<std::uint64_t>(r1),static_cast<std::uint64_t>(r1>>64)};
	}
	else if (shift == 128)
	{
		__uint128_t const r0{corr + p1};
		__uint128_t const r1{p2 + (r0 < corr)};
		return {static_cast<std::uint64_t>(r0),static_cast<std::uint64_t>(r0>>64),
		static_cast<std::uint64_t>(r1),static_cast<std::uint64_t>(r1>>64)};
	}
	else
	{
		__uint128_t const r0{corr + ((p1 >> (shift - 128)) | (p2 << (256 - shift)))};
		__uint128_t const r1{(p2 >> (shift - 128)) + (r0 < corr)};
		return {static_cast<std::uint64_t>(r0),static_cast<std::uint64_t>(r0>>64),
		static_cast<std::uint64_t>(r1),static_cast<std::uint64_t>(r1>>64)};
	}
}

constexpr inline ::fast_io::freestanding::array<std::uint64_t,4> generic_compute_pow5_inv(std::uint32_t const i)
{
	constexpr std::uint32_t pow5_table_size{static_cast<std::uint32_t>(pow5<long double,true>::table.size())};
	std::uint32_t const base((i + pow5_table_size - 1) / pow5_table_size);
	std::uint32_t const base2 (base * pow5_table_size);
	auto const& mul{pow5<long double,true>::inv_split[base]}; // 1/5^base2
	if (i == base2)
		return {mul.front() + 1,mul[1],mul[2],mul[3]};
	else
	{
		std::uint32_t const offset(base2 - i);
		 // 5^offset
		std::uint32_t const delta(pow5bits<true>(base2) - pow5bits<true>(i));
		std::uint32_t const corr(static_cast<std::uint32_t> ((pow5<long double,true>::errors[i / 32] >> (2 * (i % 32))) & 3) + 1);
		return mul_128_256_shift(pow5<long double,true>::table[offset], mul, delta, corr);
	}
}
constexpr inline ::fast_io::freestanding::array<std::uint64_t,4> generic_compute_pow5(std::uint32_t const i)
{
	constexpr std::uint32_t pow5_table_size{static_cast<std::uint32_t>(pow5<long double,true>::table.size())};
	std::uint32_t const base(i / pow5_table_size);
	std::uint32_t const base2(base * pow5_table_size);
	auto const& mul{pow5<long double,true>::split[base]}; // 1/5^base2
	if (i == base2)
		return mul;
	else
	{
		std::uint32_t const offset(i-base2);
		 // 5^offset
		std::uint32_t const delta(pow5bits<true>(i)-pow5bits<true>(base2));
		std::uint32_t const corr(static_cast<std::uint32_t> ((pow5<long double,true>::errors[i / 32] >> (2 * (i % 32))) & 3));
		return mul_128_256_shift(pow5<long double,true>::table[offset], mul, delta, corr);
	}
}

constexpr __uint128_t mul_shift_generic(__uint128_t const m,::fast_io::freestanding::array<std::uint64_t,4> const& mul,std::int32_t const j)
{
	::fast_io::freestanding::array<std::uint64_t,2> a{static_cast<std::uint64_t>(m),static_cast<std::uint64_t>(m>>64)};
	auto res{mul_128_256_shift(a, mul, j, 0)};
	return (static_cast<__uint128_t>(res[1])<<64)|res.front();
}

#endif
template<std::unsigned_integral T,std::size_t muldiff=sizeof(T)*8>
requires std::same_as<T,std::uint64_t>
inline constexpr T mul_shift(T m, ::fast_io::freestanding::array<T,2> const& mul, std::size_t j)
{
#if defined(_MSC_VER) && defined(_M_X64)
	if constexpr(std::same_as<T,std::uint64_t>)
	{
		// m is maximum 55 bits
		std::uint64_t high1;                                   // 128
		std::uint64_t low1{_umul128(m, mul[1], __builtin_addressof(high1))}; // 64
		std::uint64_t high0;                                   // 64
		_umul128(m, mul.front(), __builtin_addressof(high0));                       // 0
		std::uint64_t const sum{high0 + low1};
		if (sum < high0)
			++high1; // overflow into high1
		return __shiftright128(sum, high1, static_cast<unsigned char>(j - 64));
	}
	else
	{
#endif
		return low((mul_extend(m,mul.back())+high(mul_extend(m,mul.front())))>>(j-muldiff));
#if defined(_MSC_VER) && defined(_M_X64)
	}
#endif
}
template<std::unsigned_integral T,std::size_t muldiff=sizeof(T)*8>
requires std::same_as<T,std::uint32_t>
inline constexpr T mul_shift(T m, std::uint64_t mul, std::size_t j)
{
	return (static_cast<std::uint64_t>(m)*mul)>>(j-muldiff);
}

template<my_unsigned_integral T,typename P,typename M>
requires (sizeof(T)==16||sizeof(T)==8)
inline constexpr auto mul_shift_all(T m, P const& mul,std::size_t j,M& vp,M& vm,std::uint32_t mmshift)
{
	if constexpr(sizeof(T)==16)
	{
		auto const m4(m<<2);
		vp = mul_shift_generic(m4 + 2, mul, j);
		vm = mul_shift_generic(m4 - 1 - mmshift, mul, j);
		return mul_shift_generic(m4, mul, j);
	}
	else
	{
		auto const m4(m<<2);
		vp = mul_shift(m4 + 2, mul, j);
		vm = mul_shift(m4 - 1 - mmshift, mul, j);
		return mul_shift(m4, mul, j);
	}
}

template<my_unsigned_integral T>
inline constexpr bool multiple_of_power_of_2(T value,std::size_t p) {
// return __builtin_ctz(value) >= p;
	return !(static_cast<T>(value) & ((static_cast<T>(1)<<p) - 1));
}

}