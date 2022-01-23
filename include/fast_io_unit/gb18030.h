#pragma once

namespace fast_io
{

/*
Referenced from
GB18030与Unicode编码转换算法.程小刚，郭韧
https://max.book118.com/html/2019/0128/6114222054002004.shtm
*/

namespace details::codecvt::gb18030
{

inline constexpr char32_t linear_18030(char32_t a,char32_t b,char32_t c,char32_t d) noexcept
{
	return ((a*static_cast<char32_t>(10)+b)*static_cast<char32_t>(126)+c)*static_cast<char32_t>(10)+d;
}

inline constexpr char32_t linear_18030_base{linear_18030(0x81,0x30,0x81,0x30)};

inline constexpr char32_t linear(char32_t x) noexcept
{
	return linear_18030((x>>24)&0xff,(x>>16)&0xff,(x>>8)&0xff,x&0xff);
}

inline constexpr char32_t gb18030_ranges[13][4]{
{0x0452,0x2010,linear(0x8130D330),linear(0x8136A531)},
{0x2643,0x2E81,linear(0x8137A839),linear(0x8138FD38)},
{0x361B,0x3918,linear(0x8230A633),linear(0x8230F237)},
{0x3CE1,0x4056,linear(0x8231D438),linear(0x8232AF32)},
{0x4160,0x4337,linear(0x8232C937),linear(0x8232F837)},
{0x44D7,0x464C,linear(0x8233A339),linear(0x8233C931)},
{0x478E,0x4947,linear(0x8233E838),linear(0x82349638)},
{0x49B8,0x4C77,linear(0x8234A131),linear(0x8234E733)},
{0x9FA6,0xD800,linear(0x82358F33),linear(0x8336C738)},
{0xE865,0xF92C,linear(0x8336D030),linear(0x84308534)},
{0xFA2A,0xFE30,linear(0x84309C38),linear(0x84318537)},
{0xFFE6,0x10000,linear(0x8431A234),linear(0x8431A439)},
{0x10000,0x110000,linear(0x90308130),linear(0xE3329A35)}};

template<typename T>
requires (sizeof(T)==1)
inline constexpr std::size_t get_gb18030_invalid_code_units(T* p_dst) noexcept
{
	*p_dst=static_cast<T>(0x84);
	p_dst[1]=static_cast<T>(0x31);
	p_dst[2]=static_cast<T>(0xA4);
	p_dst[3]=static_cast<T>(0x37);
	return 4;
}

inline constexpr char32_t lookup_uni_to_gb18030_tb[32781]{
#include"gb18030_impl/gb18030_tb.h"
};
inline constexpr char32_t lookup_gb18030_to_uni4_tb[5974]{
#include"gb18030_impl/gb18030_uni4_tb.h"
};

inline constexpr char32_t lookup_gb18030_to_uni2_tb[23940]{
#include"gb18030_impl/gb18030_uni2_tb.h"
};

template<typename T>
requires (sizeof(T)==1)
inline constexpr std::size_t lookup_uni_to_gb18030(char32_t cdpt, T* p_dst) noexcept
{
	char32_t v{lookup_uni_to_gb18030_tb[cdpt]};
	char16_t v2{static_cast<char16_t>(v)};
	if(static_cast<char32_t>(v2)==v)
	{
		if constexpr((!std::is_volatile_v<T>)&&(std::endian::native==std::endian::little||std::endian::native==std::endian::big))
		{
#if __cpp_lib_is_constant_evaluated>=201811L
			if(!std::is_constant_evaluated())
			{
				if constexpr(std::endian::native==std::endian::big)
					v2=byte_swap(v2);
				::fast_io::details::my_memcpy(p_dst,&v2,2);
			}
			else
#endif
			{
				p_dst[0]=static_cast<T>(v2&0xFF);
				p_dst[1]=static_cast<T>(v2>>8);
			}
		}
		else
		{
			p_dst[0]=static_cast<T>(v2&0xFF);
			p_dst[1]=static_cast<T>(v2>>8);
		}
		return 2;
	}
	if constexpr((!std::is_volatile_v<T>)&&(std::endian::native==std::endian::little||std::endian::native==std::endian::big))
	{
#if __cpp_lib_is_constant_evaluated>=201811L
		if(!std::is_constant_evaluated())
		{
			if constexpr(std::endian::native==std::endian::big)
				v=byte_swap(v);
			::fast_io::details::my_memcpy(p_dst,&v,4);
		}
		else
#endif
		{
			p_dst[0]=static_cast<T>(v&0xFF);
			p_dst[1]=static_cast<T>((v>>8)&0xFF);
			p_dst[2]=static_cast<T>((v>>16)&0xFF);
			p_dst[3]=static_cast<T>(v>>24);
		}
	}
	else
	{
		p_dst[0]=static_cast<T>(v&0xFF);
		p_dst[1]=static_cast<T>((v>>8)&0xFF);
		p_dst[2]=static_cast<T>((v>>16)&0xFF);
		p_dst[3]=static_cast<T>(v>>24);
	}
	return 4;
}

template<typename T>
requires (sizeof(T)==1)
inline constexpr std::size_t get_gb18030_code_units_unhappy(char32_t u32, T* p_dst) noexcept
{
	if(u32<0x110000)[[likely]]
	{
		if(u32<0x0452)
			return lookup_uni_to_gb18030(u32-128,p_dst);
		char32_t sum{128};
		for(std::size_t i{};i!=13;++i)
		{
			auto const& e{gb18030_ranges[i]};
			auto const e0{e[0]};
			auto const e1{e[1]};
			char32_t diff{static_cast<char32_t>(e1-e0)};
			sum+=diff;
			if(static_cast<char32_t>(u32-e0)<diff)
			{
				char32_t e2{e[2]};
				char32_t gb{u32-e0+e2};
				gb-=linear_18030_base;
				p_dst[3]=static_cast<T>(0x30+gb%10); gb/=10;
				p_dst[2]=static_cast<T>(0x81+gb%126); gb/=126;
				p_dst[1]=static_cast<T>(0x30+gb%10); gb/=10;
				*p_dst=static_cast<T>(0x81+gb);
				return 4;
			}
			else if(static_cast<char32_t>(u32-e1)<static_cast<char32_t>(gb18030_ranges[i+1][0]-e1))
				return lookup_uni_to_gb18030(u32-sum,p_dst);
		}
	}
	return get_gb18030_invalid_code_units(p_dst);
}

template<typename T>
requires (sizeof(T)==1)
inline constexpr std::size_t get_gb18030_code_units(char32_t cdpt, T* p_dst) noexcept
{
	if(cdpt<0x80)[[likely]]
	{
		*p_dst=static_cast<T>(static_cast<std::make_unsigned_t<T>>(cdpt));
		return 1;
	}
	return get_gb18030_code_units_unhappy(cdpt,p_dst);
}

inline constexpr char32_t lookup_gb18030_to_uni4_func(char32_t index) noexcept
{
	if(index>=5974)[[unlikely]]
		return 0xFFFD;
	return lookup_gb18030_to_uni4_tb[index];
}

inline constexpr char32_t utf32cp_by_gb18030_index(char32_t index) noexcept
{
	char32_t sum{linear_18030_base+820};
	for(std::size_t i{};i!=13;++i)
	{
		auto const& e{gb18030_ranges[i]};
		auto const e0{e[0]};
		auto const e1{e[1]};
		sum+=static_cast<char32_t>(e1-e0);
		auto const e2{e[2]};
		auto const e3{e[3]};
		char32_t diff{static_cast<char32_t>(e3-e2)};
		if(static_cast<char32_t>(index-e2)<=diff)
			return index-e[2]+e[0];
		else if(e3<index&&i!=12&&index<gb18030_ranges[i+1][2])
			return lookup_gb18030_to_uni4_func(index-sum);	
	}
	return 0xFFFD;
}

template<typename T>
requires (sizeof(T)==1)
inline constexpr gb18030_advance_unchecked_result<T> gb18030_advance_unchecked(T const* src) noexcept
{
	char8_t const src0{static_cast<char8_t>(*src)};
	if(src0<0x80)
		return {src0,1};
	if(src0==0x80)
		return {0x20AC,1};
	if(src0==0xFF)
		return {0xFFFD,1};
	char8_t const src1{static_cast<char8_t>(src[1])};
	if(src1==0x7F)
		return {0xFFFD,2};
	else if((static_cast<char8_t>(src1-static_cast<char8_t>(0x40))<static_cast<char8_t>(0xFF-0x40)))
	{
		char32_t c{src1};
		if(src1>0x7F)
			--c;
		return {lookup_gb18030_to_uni2_tb[static_cast<char32_t>((static_cast<char32_t>(src0)-0x81)*190u+(c-0x40))],2};
	}
	else if((src1<0x30)|(src1==0xFF))
		return {0xFFFD,2};
	char8_t const src2{static_cast<char8_t>(src[2])};
	if(static_cast<char8_t>(0xFF-0x81)<=static_cast<char8_t>(src2-static_cast<char8_t>(0x81)))
		return {0xFFFD,3};
	char8_t const src3{static_cast<char8_t>(src[3])};
	if(static_cast<char8_t>(0x40-0x30)<=static_cast<char8_t>(src3-static_cast<char8_t>(0x30)))
		return {0xFFFD,4};
	return {utf32cp_by_gb18030_index(linear_18030(src0,src1,src2,src3)),4};
}

template<typename T>
requires (sizeof(T)==1)
inline constexpr gb18030_advance_unchecked_result<T> gb18030_advance(T const* src,std::size_t sz) noexcept
{
	char8_t const src0{static_cast<char8_t>(*src)};
	if(src0<0x80)
		return {src0,1};
	if(src0==0x80)
		return {0x20AC,1};
	if(src0==0xFF)
		return {0xFFFD,1};
	if(sz<2)
		return {0,static_cast<char8_t>(-1)};
	char8_t const src1{static_cast<char8_t>(src[1])};
	if(src1==0x7F)
		return {0xFFFD,2};
	else if((static_cast<char8_t>(src1-static_cast<char8_t>(0x40))<static_cast<char8_t>(0xFF-0x40)))
	{
		char32_t c{src1};
		if(0x7F<src1)
			--c;
		return {lookup_gb18030_to_uni2_tb[static_cast<char32_t>((static_cast<char32_t>(src0)-0x81)*190u+(c-0x40))],2};
	}
	else if((src1<0x30)|(src1==0xFF))
		return {0xFFFD,2};
	if(sz<4)
		return {0,static_cast<char8_t>(-1)};
	char8_t const src2{static_cast<char8_t>(src[2])};
	if(static_cast<char8_t>(0xFF-0x81)<=static_cast<char8_t>(src2-static_cast<char8_t>(0x81)))
		return {0xFFFD,3};
	char8_t const src3{static_cast<char8_t>(src[3])};
	if(static_cast<char8_t>(0x40-0x30)<=static_cast<char8_t>(src3-static_cast<char8_t>(0x30)))
		return {0xFFFD,4};
	return {utf32cp_by_gb18030_index(linear_18030(src0,src1,src2,src3)),4};
}

}

}
