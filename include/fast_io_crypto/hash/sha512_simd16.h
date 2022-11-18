#pragma once

#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline void sha512_simd16_byte_swap_message_2rounds(::fast_io::intrinsics::simd_vector<std::uint_least64_t,2>& __restrict s1,
	std::byte const* __restrict blocks_start,std::uint_least64_t* __restrict w,std::uint_least64_t* __restrict wt,std::uint_fast16_t round) noexcept
{
	using namespace ::fast_io::intrinsics;
	simd_vector<std::uint_least64_t,2> s0;
	s1.load(blocks_start+(round<<3u));
	s0.load(::fast_io::details::sha512::K512+round);
	if constexpr(std::endian::native==std::endian::little)
	{
		s1.swap_endian();
	}
	s1.store(w+round);
	s0.wrap_add_assign(s1);
	s0.store(wt);
}

#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline void sha512_simd16_compute_message_2rounds(
	::fast_io::intrinsics::simd_vector<std::uint_least64_t,2>& __restrict s1,
	std::uint_least64_t* __restrict w,std::uint_least64_t* __restrict wt,std::uint_fast8_t round) noexcept
{
	using namespace ::fast_io::intrinsics;
	simd_vector<std::uint_least64_t,2> s0;
	s0.load(w+(round-15));
	if constexpr(true)
	{
		s1=(((((s1>>42)^s1)>>13)^s1)>>6)^(((s1<<42)^s1)<<3);
		s0=(((((s0>>1)^s0)>>6)^s0)>>1)^(((s0<<7)^s0)<<56);
	}
	else if constexpr(false)
	{
		s1=(s1>>19)^(s1<<45)^(s1>>61)^(s1<<3)^(s1>>6);
		s0=(s0>>1)^(s0<<63)^(s0>>8)^(s0<<56)^(s0>>7);
	}
	else
	{
		s1=wrap_add((s1>>19),(s1<<45))^wrap_add((s1>>61),(s1<<3))^(s1>>6);
		s0=wrap_add((s0>>1),(s0<<63))^wrap_add((s0>>8),(s0<<56))^(s0>>7);
	}
	s1.wrap_add_assign(s0);
	s0.load(w+(round-16));
	s1.wrap_add_assign(s0);
	s0.load(w+(round-7));
	s1.wrap_add_assign(s0);
	s1.store(w+round);
	s0.load(::fast_io::details::sha512::K512+round);
	s0.wrap_add_assign(s1);
	s0.store(wt);
}


#if __has_cpp_attribute(__gnu__::__target__) && __has_builtin(__builtin_shufflevector) && defined(__SSE2__) && !defined(__AVX2__) && defined(__ELF__) && defined(FAST_IO_RUNTIME_DISPATCH)
[[__gnu__::__target__("default")]]
#elif __has_cpp_attribute(__gnu__::__flatten__)
[[__gnu__::__flatten__]]
#endif
inline void sha512_runtime_routine(std::uint_least64_t* __restrict state,std::byte const* __restrict blocks_start,std::byte const* __restrict blocks_last) noexcept
{
	using namespace fast_io::intrinsics;
	using namespace fast_io::details::sha512;

	simd_vector<std::uint_least64_t,2> simd;

	std::uint_least64_t w[80];
	std::uint_least64_t wt0[2],wt1[2];
	std::uint_least64_t a{state[0]};
	std::uint_least64_t b{state[1]};
	std::uint_least64_t c{state[2]};
	std::uint_least64_t d{state[3]};
	std::uint_least64_t e{state[4]};
	std::uint_least64_t f{state[5]};
	std::uint_least64_t g{state[6]};
	std::uint_least64_t h{state[7]};

	for(;blocks_start!=blocks_last;blocks_start+=128)
	{
		sha512_simd16_byte_swap_message_2rounds(simd,blocks_start,w,wt0,0);
		sha512_simd16_byte_swap_message_2rounds(simd,blocks_start,w,wt1,2);
		std::uint_least64_t bpc{b^c};
		sha512_scalar_round(wt0[0],a,b,d,e,f,g,h,bpc);
		sha512_scalar_round(wt0[1],h,a,c,d,e,f,g,bpc);

		sha512_simd16_byte_swap_message_2rounds(simd,blocks_start,w,wt0,4);
		sha512_scalar_round(wt1[0],g,h,b,c,d,e,f,bpc);
		sha512_scalar_round(wt1[1],f,g,a,b,c,d,e,bpc);

		sha512_simd16_byte_swap_message_2rounds(simd,blocks_start,w,wt1,6);
		sha512_scalar_round(wt0[0],e,f,h,a,b,c,d,bpc);
		sha512_scalar_round(wt0[1],d,e,g,h,a,b,c,bpc);

		sha512_simd16_byte_swap_message_2rounds(simd,blocks_start,w,wt0,8);
		sha512_scalar_round(wt1[0],c,d,f,g,h,a,b,bpc);
		sha512_scalar_round(wt1[1],b,c,e,f,g,h,a,bpc);

		sha512_simd16_byte_swap_message_2rounds(simd,blocks_start,w,wt1,10);
		sha512_scalar_round(wt0[0],a,b,d,e,f,g,h,bpc);
		sha512_scalar_round(wt0[1],h,a,c,d,e,f,g,bpc);

		sha512_simd16_byte_swap_message_2rounds(simd,blocks_start,w,wt0,12);
		sha512_scalar_round(wt1[0],g,h,b,c,d,e,f,bpc);
		sha512_scalar_round(wt1[1],f,g,a,b,c,d,e,bpc);

		sha512_simd16_byte_swap_message_2rounds(simd,blocks_start,w,wt1,14);
		sha512_scalar_round(wt0[0],e,f,h,a,b,c,d,bpc);
		sha512_scalar_round(wt0[1],d,e,g,h,a,b,c,bpc);

		for(std::uint_fast8_t i{14};i!=78;i+=16)
		{
			sha512_simd16_compute_message_2rounds(simd,w,wt0,i+2);
			sha512_scalar_round(wt1[0],c,d,f,g,h,a,b,bpc);
			sha512_scalar_round(wt1[1],b,c,e,f,g,h,a,bpc);

			sha512_simd16_compute_message_2rounds(simd,w,wt1,i+4);
			sha512_scalar_round(wt0[0],a,b,d,e,f,g,h,bpc);
			sha512_scalar_round(wt0[1],h,a,c,d,e,f,g,bpc);

			sha512_simd16_compute_message_2rounds(simd,w,wt0,i+6);
			sha512_scalar_round(wt1[0],g,h,b,c,d,e,f,bpc);
			sha512_scalar_round(wt1[1],f,g,a,b,c,d,e,bpc);

			sha512_simd16_compute_message_2rounds(simd,w,wt1,i+8);
			sha512_scalar_round(wt0[0],e,f,h,a,b,c,d,bpc);
			sha512_scalar_round(wt0[1],d,e,g,h,a,b,c,bpc);

			sha512_simd16_compute_message_2rounds(simd,w,wt0,i+10);
			sha512_scalar_round(wt1[0],c,d,f,g,h,a,b,bpc);
			sha512_scalar_round(wt1[1],b,c,e,f,g,h,a,bpc);

			sha512_simd16_compute_message_2rounds(simd,w,wt1,i+12);
			sha512_scalar_round(wt0[0],a,b,d,e,f,g,h,bpc);
			sha512_scalar_round(wt0[1],h,a,c,d,e,f,g,bpc);

			sha512_simd16_compute_message_2rounds(simd,w,wt0,i+14);
			sha512_scalar_round(wt1[0],g,h,b,c,d,e,f,bpc);
			sha512_scalar_round(wt1[1],f,g,a,b,c,d,e,bpc);

			sha512_simd16_compute_message_2rounds(simd,w,wt1,i+16);
			sha512_scalar_round(wt0[0],e,f,h,a,b,c,d,bpc);
			sha512_scalar_round(wt0[1],d,e,g,h,a,b,c,bpc);
		}
		sha512_scalar_round(wt1[0],c,d,f,g,h,a,b,bpc);
		sha512_scalar_round(wt1[1],b,c,e,f,g,h,a,bpc);
		a=(*state+=a);
		b=(state[1]+=b);
		c=(state[2]+=c);
		d=(state[3]+=d);
		e=(state[4]+=e);
		f=(state[5]+=f);
		g=(state[6]+=g);
		h=(state[7]+=h);
	}
}
