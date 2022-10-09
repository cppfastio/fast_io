#pragma once

namespace fast_io::details::chacha
{

template<bool evenround>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
#if __has_cpp_attribute(__gnu__::__flatten__)
[[__gnu__::__flatten__]]
#endif
inline void chacha_simd16_qr4_round_impl(
::fast_io::intrinsics::simd_vector<::std::uint_least32_t,4>& __restrict a,
::fast_io::intrinsics::simd_vector<::std::uint_least32_t,4>& __restrict b,
::fast_io::intrinsics::simd_vector<::std::uint_least32_t,4>& __restrict c,
::fast_io::intrinsics::simd_vector<::std::uint_least32_t,4>& __restrict d
) noexcept
{
	if constexpr(evenround)
	{
		b.value=__builtin_shufflevector(b.value,b.value,3,0,1,2);
		c.value=__builtin_shufflevector(c.value,c.value,2,3,0,1);
		d.value=__builtin_shufflevector(d.value,d.value,1,2,3,0);
		chacha_simd16_qr4_round_impl<0>(a,b,c,d);
		b.value=__builtin_shufflevector(b.value,b.value,1,2,3,0);
		c.value=__builtin_shufflevector(c.value,c.value,2,3,0,1);
		d.value=__builtin_shufflevector(d.value,d.value,3,0,1,2);
	}
	else
	{
		a += b;
		d ^= a;
		d = (d<<16)+(d>>16);

		c += d;
		b ^= c;
		b = (b<<12)+(b>>20);

		a += b;
		d ^= a;
		d = (d<<8)+(d>>24);

		c += d;
		b ^= c;
		b = (b<<7)+(b>>25);
	}

}

#if __has_cpp_attribute(__gnu__::__flatten__)
[[__gnu__::__flatten__]]
#endif
inline void chacha_runtime_routine(::std::byte* __restrict outdata,
	::std::uint_least32_t const* __restrict indata) noexcept
{
	constexpr
		::std::uint_least8_t rounds{20};
	constexpr
		::std::uint_least8_t roundsdiv4{rounds/4};
	::fast_io::intrinsics::simd_vector<::std::uint_least32_t,4> a,b,c,d;
	a.load(indata);
	b.load(indata+4);
	c.load(indata+8);
	d.load(indata+12);
	for(::std::uint_fast8_t i{};i!=roundsdiv4;++i)
	{
		chacha_simd16_qr4_round_impl<0>(a,b,c,d);
		chacha_simd16_qr4_round_impl<1>(a,b,c,d);
		chacha_simd16_qr4_round_impl<0>(a,b,c,d);
		chacha_simd16_qr4_round_impl<1>(a,b,c,d);
	}
	::fast_io::intrinsics::simd_vector<::std::uint_least32_t,4> t;
	t.load(indata);
	a+=t;
	t.load(indata+4);
	b+=t;
	t.load(indata+8);
	c+=t;
	t.load(indata+12);
	d+=t;
	if constexpr(::std::endian::little!=::std::endian::native)
	{
		a.swap_endian();
		b.swap_endian();
		c.swap_endian();
		d.swap_endian();
	}
	a.store(outdata);
	b.store(outdata+4*sizeof(::std::uint_least32_t));
	c.store(outdata+8*sizeof(::std::uint_least32_t));
	d.store(outdata+12*sizeof(::std::uint_least32_t));
}

}