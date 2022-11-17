#pragma once

#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#endif
inline void sha512_simd32_byte_swap_message_4rounds(::fast_io::intrinsics::simd_vector<std::uint_least64_t,4>& __restrict s1,
	std::byte const* __restrict blocks_start,std::uint_least64_t* __restrict w,std::uint_least64_t* __restrict wt,std::size_t round) noexcept
{
	using namespace ::fast_io::intrinsics;
	simd_vector<std::uint_least64_t,4> s0;
	s1.load(blocks_start+(round*8u));
	s0.load(K512+round);
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
#endif
inline void sha512_simd32_compute_message_4rounds(::fast_io::intrinsics::simd_vector<std::uint_least64_t,2>& __restrict s1,
	std::uint_least64_t* __restrict w,std::uint_least64_t* __restrict wt,std::size_t round) noexcept
{
/*
s0(w[t])=(w[t-15]>>1) ^ (w[t-15]<<63)^ (w[t-15]>>8) ^ (w[t-15]<<56) ^ (w[t-15]>>7)
s1(w[t])=(w[t-2]>>19) ^ (w[t-2]<<45) ^ (w[t-2]>>61) ^ (w[t-2]<<3)   ^ (w[t-2]>>6)
w[i] = w[i-16] + w[i-7] + s0[i] + s1[i]
wt[r,r+1,r+2,r+3]=wt[r-16,r-15,r-14,r-13]+wt[r-7,r-6,r-5,r-4]
*/
	using namespace ::fast_io::intrinsics;
	simd_vector<std::uint_least64_t,4> s0;
	s0.load(w+(round-15));

#if 0
	s1=(s1>>19)^(s1<<45)^(s1>>61)^(s1<<3)^(s1>>6);
	s0=(s0>>1)^(s0<<63)^(s0>>8)^(s0<<56)^(s0>>7);
#else
	s1=(((((s1>>42)^s1)>>13)^s1)>>6)^(((s1<<42)^s1)<<3);
	s0=(((((s0>>1)^s0)>>6)^s0)>>1)^(((s0<<7)^s0)<<56);
#endif

	simd_vector<std::uint_least64_t,4> s2;
	s2.load(w+(round-16));//s2=w[i-16]
	s2.wrap_add_assign(s0);//s2=s0[i]+w[i-16]
	s0.load(w+(round-7));//s0=w[i-7]
	s2.wrap_add_assign(s0);//s2=w[i-16]+w[i-7]+s0[i]

	simd_vector<std::uint_least64_t,2> s2low{__builtin_shufflevector(s2.value,s2.value,0,1)};

	s2low.wrap_add_assign(s1);//s2low=w[i-16]+w[i-7]+s0[i]+s1[i]
	s1.value=__builtin_shufflevector(s2.value,s2.value,2,3);

	simd_vector<std::uint_least64_t,2> s3{s2low};
#if 0
	s3=(s3>>19)^(s3<<45)^(s3>>61)^(s3<<3)^(s3>>6);
#else
	s3=(((((s3>>42)^s3)>>13)^s3)>>6)^(((s3<<42)^s3)<<3);
#endif

	s1.wrap_add_assign(s3);
	s2.value=__builtin_shufflevector(s2low.value,s1.value,0,1,2,3);
	s2.store(w+round);
	s0.load(::fast_io::details::sha512::K512+round);
	s0.wrap_add_assign(s2);
	s0.store(wt);
}

#if __has_cpp_attribute(__gnu__::__target__) && __has_builtin(__builtin_shufflevector) && defined(__SSE2__) && !defined(__AVX2__) && defined(__ELF__)
[[__gnu__::__target__("avx2")]]
#elif __has_cpp_attribute(__gnu__::__flatten__)
[[__gnu__::__flatten__]]
#endif
inline void sha512_runtime_routine(std::uint_least64_t* __restrict state,std::byte const* __restrict blocks_start,std::byte const* __restrict blocks_last) noexcept
{
	using namespace fast_io::intrinsics;
	using namespace fast_io::details::sha512;

	simd_vector<std::uint_least64_t,4> simd;

	std::uint_least64_t w[80];
	std::uint_least64_t wt0[4],wt1[4];
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
		sha512_simd32_byte_swap_message_4rounds(simd,blocks_start,w,wt0,0);
		sha512_simd32_byte_swap_message_4rounds(simd,blocks_start,w,wt1,4);
		std::uint_least64_t bpc{b^c};
		sha512_scalar_round(wt0[0],a,b,d,e,f,g,h,bpc);
		sha512_scalar_round(wt0[1],h,a,c,d,e,f,g,bpc);
		sha512_scalar_round(wt0[2],g,h,b,c,d,e,f,bpc);
		sha512_scalar_round(wt0[3],f,g,a,b,c,d,e,bpc);
		sha512_simd32_byte_swap_message_4rounds(simd,blocks_start,w,wt0,8);
		sha512_scalar_round(wt1[0],e,f,h,a,b,c,d,bpc);
		sha512_scalar_round(wt1[1],d,e,g,h,a,b,c,bpc);
		sha512_scalar_round(wt1[2],c,d,f,g,h,a,b,bpc);
		sha512_scalar_round(wt1[3],b,c,e,f,g,h,a,bpc);
		sha512_simd32_byte_swap_message_4rounds(simd,blocks_start,w,wt1,12);
		sha512_scalar_round(wt0[0],a,b,d,e,f,g,h,bpc);
		sha512_scalar_round(wt0[1],h,a,c,d,e,f,g,bpc);
		sha512_scalar_round(wt0[2],g,h,b,c,d,e,f,bpc);
		sha512_scalar_round(wt0[3],f,g,a,b,c,d,e,bpc);
		simd_vector<std::uint_least64_t,2> simd2{__builtin_shufflevector(simd.value,simd.value,2,3)};
		for(std::uint_fast8_t i{12};i!=76;i+=16)
		{
			sha512_simd32_compute_message_4rounds(simd2,w,wt0,i+4);
			sha512_scalar_round(wt1[0],e,f,h,a,b,c,d,bpc);
			sha512_scalar_round(wt1[1],d,e,g,h,a,b,c,bpc);
			sha512_scalar_round(wt1[2],c,d,f,g,h,a,b,bpc);
			sha512_scalar_round(wt1[3],b,c,e,f,g,h,a,bpc);
			sha512_simd32_compute_message_4rounds(simd2,w,wt1,i+8);
			sha512_scalar_round(wt0[0],a,b,d,e,f,g,h,bpc);
			sha512_scalar_round(wt0[1],h,a,c,d,e,f,g,bpc);
			sha512_scalar_round(wt0[2],g,h,b,c,d,e,f,bpc);
			sha512_scalar_round(wt0[3],f,g,a,b,c,d,e,bpc);

			sha512_simd32_compute_message_4rounds(simd2,w,wt0,i+12);
			sha512_scalar_round(wt1[0],e,f,h,a,b,c,d,bpc);
			sha512_scalar_round(wt1[1],d,e,g,h,a,b,c,bpc);
			sha512_scalar_round(wt1[2],c,d,f,g,h,a,b,bpc);
			sha512_scalar_round(wt1[3],b,c,e,f,g,h,a,bpc);
			sha512_simd32_compute_message_4rounds(simd2,w,wt1,i+16);
			sha512_scalar_round(wt0[0],a,b,d,e,f,g,h,bpc);
			sha512_scalar_round(wt0[1],h,a,c,d,e,f,g,bpc);
			sha512_scalar_round(wt0[2],g,h,b,c,d,e,f,bpc);
			sha512_scalar_round(wt0[3],f,g,a,b,c,d,e,bpc);

		}
		sha512_scalar_round(wt1[0],e,f,h,a,b,c,d,bpc);
		sha512_scalar_round(wt1[1],d,e,g,h,a,b,c,bpc);
		sha512_scalar_round(wt1[2],c,d,f,g,h,a,b,bpc);
		sha512_scalar_round(wt1[3],b,c,e,f,g,h,a,bpc);

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
