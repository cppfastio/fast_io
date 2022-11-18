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
inline void sha512_simd32_compute_message_4rounds(
	::fast_io::intrinsics::simd_vector<std::uint_least64_t,2>& __restrict s1,
	std::uint_least64_t* __restrict w,std::uint_least64_t* __restrict wt
	,std::uint_least64_t* __restrict wt1,std::size_t round,
	
	std::uint_least64_t& a,std::uint_least64_t& b,
	std::uint_least64_t& c,std::uint_least64_t& d,
	std::uint_least64_t& e,std::uint_least64_t& f,
	std::uint_least64_t& g,std::uint_least64_t& h,
	std::uint_least64_t& bpc
	) noexcept
{
/*
s0(w[t])=(w[t-15]>>1) ^ (w[t-15]<<63)^ (w[t-15]>>8) ^ (w[t-15]<<56) ^ (w[t-15]>>7)
s1(w[t])=(w[t-2]>>19) ^ (w[t-2]<<45) ^ (w[t-2]>>61) ^ (w[t-2]<<3)   ^ (w[t-2]>>6)
w[i] = w[i-16] + w[i-7] + s0[i] + s1[i]
wt[r,r+1,r+2,r+3]=wt[r-16,r-15,r-14,r-13]+wt[r-7,r-6,r-5,r-4]
*/

	using namespace ::fast_io::intrinsics;
	simd_vector<std::uint_least64_t,4> s0,s2,s0temp,s0temp2;
	simd_vector<std::uint_least64_t,2> s2low,s3,s1temp,s1temp2;
	std::uint_least64_t temp,temp1,T1k,apb;
	if constexpr(false)
	{
		s0.load(w+(round-15));
		s1temp=s1>>19;
		s1temp2=s1<<45;
		s1temp^=s1temp2;
		s1temp2=s1>>61;
		s1temp^=s1temp2;
		s1temp2=s1<<3;
		s1temp^=s1temp2;
		s1temp2=s1>>6;
		s1=s1temp^s1temp2;
		s0temp=s0>>1;
		s0temp2=s0<<63;
		s0temp^=s0temp2;
		s0temp2=s0>>8;
		s0temp^=s0temp2;
		s0temp2=s0<<56;
		s0temp^=s0temp2;
		s0temp2=s0>>7;
		s0=s0temp^s0temp2;
		s2.load(w+(round-16));
		s2.wrap_add_assign(s0);
		s0.load(w+(round-7));
		s2.wrap_add_assign(s0);
		s2low.value=__builtin_shufflevector(s2.value,s2.value,0,1);
		s2low.wrap_add_assign(s1);
		s1.value=__builtin_shufflevector(s2.value,s2.value,2,3);
		s1temp=s2low>>19;
		s1temp2=s2low<<45;
		s1temp^=s1temp2;
		s1temp2=s2low>>61;
		s1temp^=s1temp2;
		s1temp2=s2low<<3;
		s1temp^=s1temp2;
		s1temp2=s2low>>6;
		s3=s1temp^s1temp2;
		s1.wrap_add_assign(s3);
		s2.value=__builtin_shufflevector(s2low.value,s1.value,0,1,2,3);
		s2.store(w+round);
		s0.load(::fast_io::details::sha512::K512+round);
		s0.wrap_add_assign(s2);
		s0.store(wt1);

		T1k=h;
		T1k+=wt[0];
		temp=std::rotr(e,14);
		temp1=std::rotr(e,18);
		temp^=temp1;
		temp1=std::rotr(e,41);
		temp^=temp1;
		T1k+=temp;
		temp=f^g;
		temp&=e;
		temp^=g;
		T1k+=temp;
		temp=std::rotr(a,28);
		temp1=std::rotr(a,34);
		temp^=temp1;
		temp1=std::rotr(a,39);
		h=temp^temp1;
		apb=a^b;
		temp=apb&bpc;
		bpc=apb;
		temp^=b;
		h+=temp;
		bpc=apb;
		d+=T1k;
		h+=T1k;

		T1k=g;
		T1k+=wt[1];
		temp=std::rotr(d,14);
		temp1=std::rotr(d,18);
		temp^=temp1;
		temp1=std::rotr(d,41);
		temp^=temp1;
		T1k+=temp;
		temp=e^f;
		temp&=d;
		temp^=f;
		T1k+=temp;
		temp=std::rotr(h,28);
		temp1=std::rotr(h,34);
		temp^=temp1;
		temp1=std::rotr(h,39);
		g=temp^temp1;
		apb=h^a;
		temp=apb&bpc;
		bpc=apb;
		temp^=a;
		g+=temp;
		bpc=apb;
		c+=T1k;
		g+=T1k;

		T1k=f;
		T1k+=wt[2];
		temp=std::rotr(c,14);
		temp1=std::rotr(c,18);
		temp^=temp1;
		temp1=std::rotr(c,41);
		temp^=temp1;
		T1k+=temp;
		temp=d^e;
		temp&=c;
		temp^=e;
		T1k+=temp;
		temp=std::rotr(g,28);
		temp1=std::rotr(g,34);
		temp^=temp1;
		temp1=std::rotr(g,39);
		f=temp^temp1;
		apb=g^h;
		temp=apb&bpc;
		bpc=apb;
		temp^=h;
		f+=temp;
		bpc=apb;
		b+=T1k;
		f+=T1k;

		T1k=e;
		T1k+=wt[3];
		temp=std::rotr(b,14);
		temp1=std::rotr(b,18);
		temp^=temp1;
		temp1=std::rotr(b,41);
		temp^=temp1;
		T1k+=temp;
		temp=c^d;
		temp&=b;
		temp^=d;
		T1k+=temp;
		temp=std::rotr(f,28);
		temp1=std::rotr(f,34);
		temp^=temp1;
		temp1=std::rotr(f,39);
		e=temp^temp1;
		apb=f^g;
		temp=apb&bpc;
		bpc=apb;
		temp^=g;
		e+=temp;
		bpc=apb;
		a+=T1k;
		e+=T1k;
	}
	else
	{
		s0.load(w+(round-15));
		T1k=h;
		T1k+=wt[0];
		s1temp=s1>>19;
		temp=std::rotr(e,14);
		temp1=std::rotr(e,18);
		s1temp2=s1<<45;
		temp^=temp1;
		temp1=std::rotr(e,41);
		s1temp^=s1temp2;
		temp^=temp1;
		T1k+=temp;
		s1temp2=s1>>61;
		temp=f^g;
		temp&=e;
		s1temp^=s1temp2;
		temp^=g;
		T1k+=temp;
		s1temp2=s1<<3;
		temp=std::rotr(a,28);
		temp1=std::rotr(a,34);
		s1temp^=s1temp2;
		temp^=temp1;
		temp1=std::rotr(a,39);
		s1temp2=s1>>6;
		h=temp^temp1;
		apb=a^b;
		s1=s1temp^s1temp2;
		temp=apb&bpc;
		bpc=apb;
		s0temp=s0>>1;
		temp^=b;
		h+=temp;
		s0temp2=s0<<63;
		bpc=apb;
		d+=T1k;
		s0temp^=s0temp2;
		h+=T1k;
		T1k=g;
		s0temp2=s0>>8;
		T1k+=wt[1];
		temp=std::rotr(d,14);
		s0temp^=s0temp2;
		temp1=std::rotr(d,18);
		temp^=temp1;
		s0temp2=s0<<56;
		temp1=std::rotr(d,41);
		temp^=temp1;
		s0temp^=s0temp2;
		T1k+=temp;
		temp=e^f;
		s0temp2=s0>>7;
		temp&=d;
		temp^=f;
		s0=s0temp^s0temp2;
		T1k+=temp;
		temp=std::rotr(h,28);
		s2.load(w+(round-16));
		temp1=std::rotr(h,34);
		temp^=temp1;
		s2.wrap_add_assign(s0);
		temp1=std::rotr(h,39);
		g=temp^temp1;
		s0.load(w+(round-7));
		apb=h^a;
		temp=apb&bpc;
		s2.wrap_add_assign(s0);
		bpc=apb;
		temp^=a;
		s2low.value=__builtin_shufflevector(s2.value,s2.value,0,1);
		g+=temp;
		bpc=apb;
		s2low.wrap_add_assign(s1);
		c+=T1k;
		g+=T1k;
		s1.value=__builtin_shufflevector(s2.value,s2.value,2,3);
		T1k=f;
		T1k+=wt[2];
		s1temp=s2low>>19;
		temp=std::rotr(c,14);
		temp1=std::rotr(c,18);
		s1temp2=s2low<<45;
		temp^=temp1;
		temp1=std::rotr(c,41);
		s1temp^=s1temp2;
		temp^=temp1;
		T1k+=temp;
		s1temp2=s2low>>61;
		temp=d^e;
		temp&=c;
		s1temp^=s1temp2;
		temp^=e;
		T1k+=temp;
		s1temp2=s2low<<3;
		temp=std::rotr(g,28);
		temp1=std::rotr(g,34);
		s1temp^=s1temp2;
		temp^=temp1;
		temp1=std::rotr(g,39);
		s1temp2=s2low>>6;
		f=temp^temp1;
		apb=g^h;
		s3=s1temp^s1temp2;
		temp=apb&bpc;
		bpc=apb;
		s1.wrap_add_assign(s3);
		temp^=h;
		f+=temp;
		s2.value=__builtin_shufflevector(s2low.value,s1.value,0,1,2,3);
		bpc=apb;
		b+=T1k;
		s2.store(w+round);
		f+=T1k;
		T1k=e;
		s0.load(::fast_io::details::sha512::K512+round);
		T1k+=wt[3];
		temp=std::rotr(b,14);
		s0.wrap_add_assign(s2);
		temp1=std::rotr(b,18);
		temp^=temp1;
		s0.store(wt1);
		temp1=std::rotr(b,41);
		temp^=temp1;
		T1k+=temp;
		temp=c^d;
		temp&=b;
		temp^=d;
		T1k+=temp;
		temp=std::rotr(f,28);
		temp1=std::rotr(f,34);
		temp^=temp1;
		temp1=std::rotr(f,39);
		e=temp^temp1;
		apb=f^g;
		temp=apb&bpc;
		bpc=apb;
		temp^=g;
		e+=temp;
		bpc=apb;
		a+=T1k;
		e+=T1k;
	}
}

#if __has_cpp_attribute(__gnu__::__target__) && __has_builtin(__builtin_shufflevector) && defined(__SSE2__) && !defined(__AVX2__) && defined(__ELF__) && defined(FAST_IO_RUNTIME_DISPATCH)
[[__gnu__::__target__("avx2")]]
#elif __has_cpp_attribute(__gnu__::__flatten__)
[[__gnu__::__flatten__]]
#endif
inline void sha512_runtime_routine(std::uint_least64_t* __restrict state,std::byte const* __restrict blocks_start,std::byte const* __restrict blocks_last) noexcept
{
	using namespace fast_io::intrinsics;
	using namespace fast_io::details::sha512;

	simd_vector<std::uint_least64_t,4> simd;

	std::uint_least64_t wt0[4],wt1[4];
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
		for(std::uint_fast8_t i{16};i!=80;i+=16)
		{
			sha512_simd32_compute_message_4rounds(simd2,w,wt1,wt0,i,
			e,f,g,h,a,b,c,d,bpc);
			sha512_simd32_compute_message_4rounds(simd2,w,wt0,wt1,i+4,
			a,b,c,d,e,f,g,h,bpc);
			sha512_simd32_compute_message_4rounds(simd2,w,wt1,wt0,i+8,
			e,f,g,h,a,b,c,d,bpc);
			sha512_simd32_compute_message_4rounds(simd2,w,wt0,wt1,i+12,
			a,b,c,d,e,f,g,h,bpc);
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
