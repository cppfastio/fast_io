#pragma once
/*
https://en.wikipedia.org/wiki/Salsa20#ChaCha_variant
*/
namespace fast_io
{

namespace details::chacha
{

inline constexpr void chachaqr(::std::uint_least32_t& __restrict a,
			::std::uint_least32_t& __restrict b,
			::std::uint_least32_t& __restrict c,
			::std::uint_least32_t& __restrict d) noexcept
{
	a += b;
	d ^= a;
	d = ::std::rotl(d,16);

	c += d;
	b ^= c;
	b = ::std::rotl(b,12);

	a += b;
	d ^= a;
	d = ::std::rotl(d, 8);

	c += d;
	b ^= c;
	b = ::std::rotl(b, 7);
}

#if __has_cpp_attribute(__gnu__::__flatten__)
[[__gnu__::__flatten__]]
#endif
inline
#if (__cpp_lib_is_constant_evaluated >=201811L || __cpp_if_consteval >= 202106L) && __cpp_lib_bit_cast >= 201806L
constexpr
#endif
void chacha_main_routine(::std::byte* __restrict outdata,
	::std::uint_least32_t const* __restrict indata) noexcept
{
	constexpr
		::std::size_t n{16};
	::std::uint_least32_t x[n];
#if __cpp_lib_is_constant_evaluated >=201811L || __cpp_if_consteval >= 202106L
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if(std::is_constant_evaluated())
#endif
	{
		for(std::size_t i{};i!=n;++i)
			x[i]=indata[i];
	}
	else
#endif
	{
		__builtin_memcpy(x,indata,sizeof(x));
	}
	constexpr
		::std::uint_least8_t rounds{20};
	constexpr
		::std::uint_least8_t roundsdiv2{rounds/2};
	for (::std::uint_fast8_t i{}; i != roundsdiv2; ++i)
	{
		// Odd round
		chachaqr(x[0], x[4], x[ 8], x[12]); // column 0
		chachaqr(x[1], x[5], x[ 9], x[13]); // column 1
		chachaqr(x[2], x[6], x[10], x[14]); // column 2
		chachaqr(x[3], x[7], x[11], x[15]); // column 3
		// Even round
		chachaqr(x[0], x[5], x[10], x[15]); // diagonal 1 (main diagonal)
		chachaqr(x[1], x[6], x[11], x[12]); // diagonal 2
		chachaqr(x[2], x[7], x[ 8], x[13]); // diagonal 3
		chachaqr(x[3], x[4], x[ 9], x[14]); // diagonal 4
	}

	for(::std::size_t i{};i!=n;++i)
	{
		std::uint_least32_t res{x[i]+indata[i]};
		if constexpr(::std::endian::little!=::std::endian::native)
		{
			res=::fast_io::byte_swap(res);
		}
#if __cpp_lib_is_constant_evaluated >=201811L || __cpp_if_consteval >= 202106L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if(std::is_constant_evaluated())
#endif
		{
			auto v{::std::bit_cast<::fast_io::freestanding::array<::std::byte,sizeof(::std::uint_least32_t)>>(res)};
			for(::std::size_t j{};j!=sizeof(::std::uint_least32_t);++j)
			{
				outdata[i*sizeof(::std::uint_least32_t)+j]=v[j];
			}
		}
		else
#endif
		{
			__builtin_memcpy(outdata+i*sizeof(::std::uint_least32_t),__builtin_addressof(res),sizeof(::std::uint_least32_t));
		}
	}

}

}

}