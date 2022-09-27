#pragma once

inline constexpr auto create_k512scalar() noexcept
{
	constexpr std::size_t n{sizeof(K512)/sizeof(std::uint_least64_t)};
	constexpr std::size_t nsub16{n-16u};
	::fast_io::freestanding::array<std::uint_least64_t,nsub16> a;
	for(std::size_t i{};i!=nsub16;++i)
	{
		a[i]=K512[i+16u];
	}
	return a;
}

inline constexpr auto k512scalar{create_k512scalar()};

#if __has_cpp_attribute(__gnu__::__flatten__)
[[__gnu__::__flatten__]]
#endif
#if __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void sha512_round(std::uint_least64_t T1,std::uint_least64_t a,std::uint_least64_t b,std::uint_least64_t& __restrict d,
				std::uint_least64_t e,std::uint_least64_t f,std::uint_least64_t g,std::uint_least64_t& __restrict h,
				std::uint_least64_t& __restrict bpc,std::uint_least64_t k) noexcept
{
	sha512_scalar_round(T1+k,a,b,d,e,f,g,h,bpc);
}

#if __has_cpp_attribute(__gnu__::__flatten__)
[[__gnu__::__flatten__]]
#endif
inline void sha512_runtime_routine(std::uint_least64_t* __restrict state,std::byte const* __restrict blocks_start,std::byte const* __restrict blocks_last) noexcept
{
	using ul64_may_alias
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= std::uint_least64_t;
	std::uint_least64_t a{state[0]};
	std::uint_least64_t b{state[1]};
	std::uint_least64_t c{state[2]};
	std::uint_least64_t d{state[3]};
	std::uint_least64_t e{state[4]};
	std::uint_least64_t f{state[5]};
	std::uint_least64_t g{state[6]};
	std::uint_least64_t h{state[7]};
	std::uint_least64_t x[16];
	constexpr std::uint_least64_t const* k5_start{k512scalar.element};
	constexpr std::uint_least64_t const* k5_last{k512scalar.element+k512scalar.size()};
	for(;blocks_start!=blocks_last;blocks_start+=128)
	{
		ul64_may_alias const* W{reinterpret_cast<ul64_may_alias const*>(blocks_start)};
		std::uint_least64_t bpc{b^c};
		sha512_round(x[0]=big_endian(W[0]),a,b,d,e,f,g,h,bpc,0x428a2f98d728ae22);
		sha512_round(x[1]=big_endian(W[1]),h,a,c,d,e,f,g,bpc,0x7137449123ef65cd);
		sha512_round(x[2]=big_endian(W[2]),g,h,b,c,d,e,f,bpc,0xb5c0fbcfec4d3b2f);
		sha512_round(x[3]=big_endian(W[3]),f,g,a,b,c,d,e,bpc,0xe9b5dba58189dbbc);
		sha512_round(x[4]=big_endian(W[4]),e,f,h,a,b,c,d,bpc,0x3956c25bf348b538);
		sha512_round(x[5]=big_endian(W[5]),d,e,g,h,a,b,c,bpc,0x59f111f1b605d019);
		sha512_round(x[6]=big_endian(W[6]),c,d,f,g,h,a,b,bpc,0x923f82a4af194f9b);
		sha512_round(x[7]=big_endian(W[7]),b,c,e,f,g,h,a,bpc,0xab1c5ed5da6d8118);
		sha512_round(x[8]=big_endian(W[8]),a,b,d,e,f,g,h,bpc,0xd807aa98a3030242);
		sha512_round(x[9]=big_endian(W[9]),h,a,c,d,e,f,g,bpc,0x12835b0145706fbe);
		sha512_round(x[10]=big_endian(W[10]),g,h,b,c,d,e,f,bpc,0x243185be4ee4b28c);
		sha512_round(x[11]=big_endian(W[11]),f,g,a,b,c,d,e,bpc,0x550c7dc3d5ffb4e2);
		sha512_round(x[12]=big_endian(W[12]),e,f,h,a,b,c,d,bpc,0x72be5d74f27b896f);
		sha512_round(x[13]=big_endian(W[13]),d,e,g,h,a,b,c,bpc,0x80deb1fe3b1696b1);
		sha512_round(x[14]=big_endian(W[14]),c,d,f,g,h,a,b,bpc,0x9bdc06a725c71235);
		sha512_round(x[15]=big_endian(W[15]),b,c,e,f,g,h,a,bpc,0xc19bf174cf692694);
		for(std::uint_least64_t const *k5{k5_start};k5!=k5_last;k5+=16)
		{
			sha512_round((x[0]+=sigma0(x[1])+sigma1(x[14])+x[9]),a,b,d,e,f,g,h,bpc,*k5);
			sha512_round((x[1]+=sigma0(x[2])+sigma1(x[15])+x[10]),h,a,c,d,e,f,g,bpc,k5[1]);
			sha512_round((x[2]+=sigma0(x[3])+sigma1(x[0])+x[11]),g,h,b,c,d,e,f,bpc,k5[2]);
			sha512_round((x[3]+=sigma0(x[4])+sigma1(x[1])+x[12]),f,g,a,b,c,d,e,bpc,k5[3]);
			sha512_round((x[4]+=sigma0(x[5])+sigma1(x[2])+x[13]),e,f,h,a,b,c,d,bpc,k5[4]);
			sha512_round((x[5]+=sigma0(x[6])+sigma1(x[3])+x[14]),d,e,g,h,a,b,c,bpc,k5[5]);
			sha512_round((x[6]+=sigma0(x[7])+sigma1(x[4])+x[15]),c,d,f,g,h,a,b,bpc,k5[6]);
			sha512_round((x[7]+=sigma0(x[8])+sigma1(x[5])+x[0]),b,c,e,f,g,h,a,bpc,k5[7]);
			sha512_round((x[8]+=sigma0(x[9])+sigma1(x[6])+x[1]),a,b,d,e,f,g,h,bpc,k5[8]);
			sha512_round((x[9]+=sigma0(x[10])+sigma1(x[7])+x[2]),h,a,c,d,e,f,g,bpc,k5[9]);
			sha512_round((x[10]+=sigma0(x[11])+sigma1(x[8])+x[3]),g,h,b,c,d,e,f,bpc,k5[10]);
			sha512_round((x[11]+=sigma0(x[12])+sigma1(x[9])+x[4]),f,g,a,b,c,d,e,bpc,k5[11]);
			sha512_round((x[12]+=sigma0(x[13])+sigma1(x[10])+x[5]),e,f,h,a,b,c,d,bpc,k5[12]);
			sha512_round((x[13]+=sigma0(x[14])+sigma1(x[11])+x[6]),d,e,g,h,a,b,c,bpc,k5[13]);
			sha512_round((x[14]+=sigma0(x[15])+sigma1(x[12])+x[7]),c,d,f,g,h,a,b,bpc,k5[14]);
			sha512_round((x[15]+=sigma0(x[0])+sigma1(x[13])+x[8]),b,c,e,f,g,h,a,bpc,k5[15]);
		}
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