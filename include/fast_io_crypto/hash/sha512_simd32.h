#pragma once

#if __has_cpp_attribute(gnu::always_inline)
[[gnu::always_inline]]
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
	s0.store(wt+round);
}

#if __has_cpp_attribute(gnu::always_inline)
[[gnu::always_inline]]
#endif
inline void sha512_simd32_compute_message_4rounds(::fast_io::intrinsics::simd_vector<std::uint_least64_t,4>& __restrict s1,
	std::uint_least64_t* __restrict w,std::uint_least64_t* __restrict wt,std::size_t round) noexcept
{
/*
s0(w[t])=(w[t-15]>>1) ^ (w[t-15]<<63)^ (w[t-15]>>8) ^ (w[t-15]<<56) ^ (w[t-15]>>7)
s1(w[t])=(w[t-2]>>19) ^ (w[t-2]<<45) ^ (w[t-2]>>61) ^ (w[t-2]<<3)   ^ (w[t-2]>>6)
w[i] = w[i-16] + w[i-7] + s0[i] + s1[i]
wt[r,r+1,r+2,r+3]=wt[r-16,r-15,r-14,r-13]+wt[r-7,r-6,r-5,r-4]
*/
	using namespace ::fast_io::intrinsics;
	simd_vector<std::uint_least64_t,4> s10{s1[2],s1[3],0,0};
	simd_vector<std::uint_least64_t,4> s0;
	s0.load(w+(round-15));

#if 1
	s10=(s10>>19)^(s10<<45)^(s10>>61)^(s10<<3)^(s10>>6);
	s0=(s0>>1)^(s0<<63)^(s0>>8)^(s0<<56)^(s0>>7);
#else
	s10=(((((s10>>42)^s10)>>13)^s10)>>6)^(((s10<<42)^s10)<<3);
	s0=(((((s0>>1)^s0)>>6)^s0)>>1)^(((s0<<7)^s0)<<56);
#endif

	s10.wrap_add_assign(s0);
	s0.load(w+(round-16));
	s10.wrap_add_assign(s0);
	s0.load(w+(round-7));
	s10.wrap_add_assign(s0);
	s1=simd_vector<std::uint_least64_t,4>{0,0,s10[0],s10[1]};
#if 1
	s1=(s1>>19)^(s1<<45)^(s1>>61)^(s1<<3)^(s1>>6);
#else
	s1=(((((s1>>42)^s1)>>13)^s1)>>6)^(((s1<<42)^s1)<<3);
#endif
	s1.wrap_add_assign(s10);
	s1.store(w+round);
	s0.load(::fast_io::details::sha512::K512+round);
	s0.wrap_add_assign(s1);
	s0.store(wt+round);
}

#if __has_cpp_attribute(gnu::flatten)
[[gnu::flatten]]
#endif
inline void sha512_runtime_routine(std::uint_least64_t* __restrict state,std::byte const* __restrict blocks_start,std::byte const* __restrict blocks_last) noexcept
{
	using namespace fast_io::intrinsics;
	using namespace fast_io::details::sha512;

	simd_vector<std::uint_least64_t,4> simd;

	std::uint_least64_t wt[80];
	std::uint_least64_t w[80];
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
		sha512_simd32_byte_swap_message_4rounds(simd,blocks_start,w,wt,0);
		std::uint_least64_t bpc{b^c};
		sha512_scalar_round(wt[0],a,b,d,e,f,g,h,bpc);
		sha512_scalar_round(wt[1],h,a,c,d,e,f,g,bpc);
		sha512_simd32_byte_swap_message_4rounds(simd,blocks_start,w,wt,4);
		sha512_scalar_round(wt[2],g,h,b,c,d,e,f,bpc);
		sha512_scalar_round(wt[3],f,g,a,b,c,d,e,bpc);
		sha512_scalar_round(wt[4],e,f,h,a,b,c,d,bpc);
		sha512_scalar_round(wt[5],d,e,g,h,a,b,c,bpc);
		sha512_simd32_byte_swap_message_4rounds(simd,blocks_start,w,wt,8);
		sha512_scalar_round(wt[6],c,d,f,g,h,a,b,bpc);
		sha512_scalar_round(wt[7],b,c,e,f,g,h,a,bpc);
		sha512_scalar_round(wt[8],a,b,d,e,f,g,h,bpc);
		sha512_scalar_round(wt[9],h,a,c,d,e,f,g,bpc);
		sha512_simd32_byte_swap_message_4rounds(simd,blocks_start,w,wt,12);
		sha512_scalar_round(wt[10],g,h,b,c,d,e,f,bpc);
		sha512_scalar_round(wt[11],f,g,a,b,c,d,e,bpc);
		for(std::uint_fast8_t i{12};i!=76;i+=8)
		{
			std::uint_least64_t const* const p{wt+i};
			sha512_scalar_round(*p,e,f,h,a,b,c,d,bpc);
			sha512_scalar_round(p[1],d,e,g,h,a,b,c,bpc);
			sha512_simd32_compute_message_4rounds(simd,w,wt,i+4);
			sha512_scalar_round(p[2],c,d,f,g,h,a,b,bpc);
			sha512_scalar_round(p[3],b,c,e,f,g,h,a,bpc);
			sha512_scalar_round(p[4],a,b,d,e,f,g,h,bpc);
			sha512_scalar_round(p[5],h,a,c,d,e,f,g,bpc);
			sha512_simd32_compute_message_4rounds(simd,w,wt,i+8);
			sha512_scalar_round(p[6],g,h,b,c,d,e,f,bpc);
			sha512_scalar_round(p[7],f,g,a,b,c,d,e,bpc);
		}
		sha512_scalar_round(wt[76],e,f,h,a,b,c,d,bpc);
		sha512_scalar_round(wt[77],d,e,g,h,a,b,c,bpc);
		sha512_scalar_round(wt[78],c,d,f,g,h,a,b,bpc);
		sha512_scalar_round(wt[79],b,c,e,f,g,h,a,bpc);

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
