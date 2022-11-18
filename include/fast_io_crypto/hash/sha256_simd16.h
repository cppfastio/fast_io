#pragma once

#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#endif
inline void sha256_simd16_byte_swap_message_4rounds(::fast_io::intrinsics::simd_vector<std::uint_least32_t,4>& __restrict s1,
	std::byte const* __restrict blocks_start,std::uint_least32_t* __restrict w,std::uint_least32_t* __restrict wt,std::size_t round) noexcept
{
	using namespace ::fast_io::intrinsics;
	simd_vector<std::uint_least32_t,4> s0;
	s1.load(blocks_start+(round*4u));
	s0.load(K256+round);
	if constexpr(std::endian::native==std::endian::little)
	{
		s1.swap_endian();
	}
	s1.store(w+round);
	s0.wrap_add_assign(s1);
	s0.store(wt+round);
}

#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#endif
inline void sha256_simd16_compute_message_4rounds(::fast_io::intrinsics::simd_vector<std::uint_least32_t,4>& __restrict s1,
	std::uint_least32_t* __restrict w,std::uint_least32_t* __restrict wt,std::size_t round) noexcept
{
/*
s0(w[t])=(w[t-15]>>7) ^ (w[t-15]<<25)^ (w[t-15]>>18) ^ (w[t-15]<<14) ^ (w[t-15]>>3)
s1(w[t])=(w[t-2]>>17) ^ (w[t-2]<<15) ^ (w[t-2]>>19) ^ (w[t-2]<<13)   ^ (w[t-2]>>10)
w[i] = w[i-16] + w[i-7] + s0[i] + s1[i]
wt[r,r+1,r+2,r+3]=wt[r-16,r-15,r-14,r-13]+wt[r-7,r-6,r-5,r-4]
*/
	using namespace ::fast_io::intrinsics;
	simd_vector<std::uint_least32_t,4> s10{s1[2],s1[3],0,0};
	simd_vector<std::uint_least32_t,4> s0;
	s0.load(w+(round-15));
	if constexpr(true)
	{
		s10=(s10>>17)^(s10<<15)^(s10>>19)^(s10<<13)^(s10>>10);
		s0=(s0>>7)^(s0<<25)^(s0>>18)^(s0<<14)^(s0>>3);
	}
	else if constexpr(false)
	{
		s10=(((((s10>>2)^s10)>>7)^s10)>>10)^(((s10<<2)^s10)<<13);
		s0=(((((s0>>11)^s0)>>4)^s0)>>3)^(((s0<<11)^s0)<<14);
	}
	else
	{
		s10=wrap_add((s10>>17),(s10<<15))^wrap_add((s10>>19),(s10<<13))^(s10>>10);
		s0=wrap_add((s0>>7),(s0<<25))^wrap_add((s0>>18),(s0<<14))^(s0>>3);
	}
	s10.wrap_add_assign(s0);
	s0.load(w+(round-16));
	s10.wrap_add_assign(s0);
	s0.load(w+(round-7));
	s10.wrap_add_assign(s0);
	s1=simd_vector<std::uint_least32_t,4>{0,0,s10[0],s10[1]};
	if constexpr(false)
	{
		s1=(s1>>17)^(s1<<15)^(s1>>19)^(s1<<13)^(s1>>10);
	}
	else
	{
		s1=(((((s1>>2)^s1)>>7)^s1)>>10)^(((s1<<2)^s1)<<13);
	}
	s1.wrap_add_assign(s10);
	s1.store(w+round);
	s0.load(::fast_io::details::sha256::K256+round);
	s0.wrap_add_assign(s1);
	s0.store(wt+round);
}

#if defined(__SSE2__) && (!defined(__SHA__) || !defined(__SSSE3__)) && __has_cpp_attribute(__gnu__::__target__) && !defined(__clang__) && defined(__ELF__) && defined(FAST_IO_RUNTIME_DISPATCH)
[[__gnu__::__target__("default")]]
#elif __has_cpp_attribute(__gnu__::__flatten__)
[[__gnu__::__flatten__]]
#endif
inline void sha256_runtime_routine(std::uint_least32_t* __restrict state,std::byte const* __restrict blocks_start,std::byte const* __restrict blocks_last) noexcept
{
	using namespace fast_io::intrinsics;
	using namespace fast_io::details::sha256;

	simd_vector<std::uint_least32_t,4> simd;

	std::uint_least32_t wt[64];
	std::uint_least32_t w[64];
	std::uint_least32_t a{state[0]};
	std::uint_least32_t b{state[1]};
	std::uint_least32_t c{state[2]};
	std::uint_least32_t d{state[3]};
	std::uint_least32_t e{state[4]};
	std::uint_least32_t f{state[5]};
	std::uint_least32_t g{state[6]};
	std::uint_least32_t h{state[7]};

	for(;blocks_start!=blocks_last;blocks_start+=64)
	{
		sha256_simd16_byte_swap_message_4rounds(simd,blocks_start,w,wt,0);
		std::uint_least32_t bpc{b^c};
		sha256_scalar_round(wt[0],a,b,d,e,f,g,h,bpc);
		sha256_scalar_round(wt[1],h,a,c,d,e,f,g,bpc);
		sha256_simd16_byte_swap_message_4rounds(simd,blocks_start,w,wt,4);
		sha256_scalar_round(wt[2],g,h,b,c,d,e,f,bpc);
		sha256_scalar_round(wt[3],f,g,a,b,c,d,e,bpc);
		sha256_scalar_round(wt[4],e,f,h,a,b,c,d,bpc);
		sha256_scalar_round(wt[5],d,e,g,h,a,b,c,bpc);
		sha256_simd16_byte_swap_message_4rounds(simd,blocks_start,w,wt,8);
		sha256_scalar_round(wt[6],c,d,f,g,h,a,b,bpc);
		sha256_scalar_round(wt[7],b,c,e,f,g,h,a,bpc);
		sha256_scalar_round(wt[8],a,b,d,e,f,g,h,bpc);
		sha256_scalar_round(wt[9],h,a,c,d,e,f,g,bpc);
		sha256_simd16_byte_swap_message_4rounds(simd,blocks_start,w,wt,12);
		sha256_scalar_round(wt[10],g,h,b,c,d,e,f,bpc);
		sha256_scalar_round(wt[11],f,g,a,b,c,d,e,bpc);
		for(std::uint_fast8_t i{12};i!=60;i+=16)
		{
			std::uint_least32_t const* const p{wt+i};
			sha256_scalar_round(*p,e,f,h,a,b,c,d,bpc);
			sha256_scalar_round(p[1],d,e,g,h,a,b,c,bpc);
			sha256_simd16_compute_message_4rounds(simd,w,wt,i+4);
			sha256_scalar_round(p[2],c,d,f,g,h,a,b,bpc);
			sha256_scalar_round(p[3],b,c,e,f,g,h,a,bpc);
			sha256_scalar_round(p[4],a,b,d,e,f,g,h,bpc);
			sha256_scalar_round(p[5],h,a,c,d,e,f,g,bpc);
			sha256_simd16_compute_message_4rounds(simd,w,wt,i+8);
			sha256_scalar_round(p[6],g,h,b,c,d,e,f,bpc);
			sha256_scalar_round(p[7],f,g,a,b,c,d,e,bpc);

			sha256_scalar_round(p[8],e,f,h,a,b,c,d,bpc);
			sha256_scalar_round(p[9],d,e,g,h,a,b,c,bpc);
			sha256_simd16_compute_message_4rounds(simd,w,wt,i+12);
			sha256_scalar_round(p[10],c,d,f,g,h,a,b,bpc);
			sha256_scalar_round(p[11],b,c,e,f,g,h,a,bpc);
			sha256_scalar_round(p[12],a,b,d,e,f,g,h,bpc);
			sha256_scalar_round(p[13],h,a,c,d,e,f,g,bpc);
			sha256_simd16_compute_message_4rounds(simd,w,wt,i+16);
			sha256_scalar_round(p[14],g,h,b,c,d,e,f,bpc);
			sha256_scalar_round(p[15],f,g,a,b,c,d,e,bpc);
		}
		sha256_scalar_round(wt[60],e,f,h,a,b,c,d,bpc);
		sha256_scalar_round(wt[61],d,e,g,h,a,b,c,bpc);
		sha256_scalar_round(wt[62],c,d,f,g,h,a,b,bpc);
		sha256_scalar_round(wt[63],b,c,e,f,g,h,a,bpc);

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
