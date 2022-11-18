#pragma once
/*
https://github.com/noloader/SHA-Intrinsics/blob/master/sha256-x86.c
*/

namespace fast_io::details::sha256
{

inline constexpr std::uint_least32_t K256[]
{
0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5,
0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3,
0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC,
0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7,
0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13,
0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3,
0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5,
0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208,
0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2
};

inline constexpr auto Sigma0(auto x) noexcept
{
	return std::rotr(x,2)^std::rotr(x,13)^std::rotr(x,22);
}
inline constexpr auto Sigma1(auto x) noexcept
{
	return std::rotr(x,6)^std::rotr(x,11)^std::rotr(x,25);
}
inline constexpr auto sigma0(auto x) noexcept
{
	return std::rotr(x,7)^std::rotr(x,18)^(x>>3);
}
inline constexpr auto sigma1(auto x) noexcept
{
	return std::rotr(x,17)^std::rotr(x,19)^(x>>10);
}
inline constexpr auto Ch(auto x,auto y,auto z) noexcept
{
	return ((y^z)&x)^z;
}
inline constexpr auto Maj(auto x,auto y,auto z) noexcept
{
	return (x&y)^(x&z)^(y&z);
}

#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void sha256_scalar_round(std::uint_least32_t T1k,std::uint_least32_t a,std::uint_least32_t b,std::uint_least32_t& __restrict d,
				std::uint_least32_t e,std::uint_least32_t f,std::uint_least32_t g,std::uint_least32_t& __restrict h,std::uint_least32_t& __restrict bpc) noexcept
{
	T1k+=h+Sigma1(e)+Ch(e,f,g);
	h=Sigma0(a);
	std::uint_least32_t apb{a^b};
	h+=(apb&bpc)^b;
	bpc=apb;
	d+=T1k;
	h+=T1k;
}

inline constexpr std::uint_least32_t B2U32(std::byte val, std::uint_least8_t sh) noexcept
{
	return (static_cast<std::uint_least32_t>(val)) << sh;
}

inline 
#if __cpp_lib_is_constant_evaluated >= 201811L
constexpr
#endif
void sha256_do_constexpr_function(std::uint_least32_t* __restrict state,std::byte const* __restrict data,std::byte const* __restrict blocks_last) noexcept
{
	std::uint_least32_t a{state[0]}, b{state[1]}, c{state[2]}, d{state[3]}, e{state[4]}, f{state[5]}, g{state[6]}, h{state[7]}, s0, s1, T1, T2;
	std::uint_least32_t X[16];
	using namespace fast_io::details::sha256;
	while(data!=blocks_last)
	{
		std::uint_least32_t i{};
		for (; i < 16; ++i)
		{
#if __cpp_lib_is_constant_evaluated >= 201811L
			if (std::is_constant_evaluated())
			{
				X[i] = B2U32(data[0], 24) | B2U32(data[1], 16) | B2U32(data[2], 8) | B2U32(data[3], 0);
			}
			else
#endif
			{
				std::uint_least32_t value;
				::fast_io::details::my_memcpy(__builtin_addressof(value),data,4);
				X[i] = big_endian(value);
			}
			data += 4;

			T1 = h;
			T1 += Sigma1(e);
			T1 += Ch(e, f, g);
			T1 += K256[i];
			T1 += X[i];

			T2 = Sigma0(a);
			T2 += Maj(a, b, c);

			h = g;
			g = f;
			f = e;
			e = d + T1;
			d = c;
			c = b;
			b = a;
			a = T1 + T2;
		}

		for (; i < 64; ++i)
		{
			s0 = X[(i + 1) & 0x0f];
			s0 = sigma0(s0);
			s1 = X[(i + 14) & 0x0f];
			s1 = sigma1(s1);

			T1 = X[i & 0xf] += s0 + s1 + X[(i + 9) & 0xf];
			T1 += h + Sigma1(e) + Ch(e, f, g) + K256[i];
			T2 = Sigma0(a) + Maj(a, b, c);
			h = g;
			g = f;
			f = e;
			e = d + T1;
			d = c;
			c = b;
			b = a;
			a = T1 + T2;
		}

		a=(state[0] += a);
		b=(state[1] += b);
		c=(state[2] += c);
		d=(state[3] += d);
		e=(state[4] += e);
		f=(state[5] += f);
		g=(state[6] += g);
		h=(state[7] += h);
	}
}

#if (!defined(_MSC_VER) || defined(__clang__))
#if defined(__SSE2__) && (!defined(__SHA__) || !defined(__SSSE3__)) && __has_cpp_attribute(__gnu__::__target__) && !defined(__clang__) && defined(__ELF__) && defined(FAST_IO_RUNTIME_DISPATCH)
#include"sha256_x86_sha_extensions.h"
#include"sha256_simd16.h"
#elif __has_builtin(__builtin_ia32_sha256rnds2)&& \
__has_builtin(__builtin_ia32_pshufd)&& \
__has_builtin(__builtin_ia32_palignr128) && \
__has_builtin(__builtin_ia32_sha256msg1) && \
__has_builtin(__builtin_ia32_sha256msg2) && \
__has_builtin(__builtin_ia32_pshufb128) && (!defined(__clang__)||(defined(__SSE4_2__)&&defined(__SHA__)))
#include"sha256_x86_sha_extensions.h"
#elif defined(__SSE2__)  || defined(__wasm_simd128__)
#include"sha256_simd16.h"
#else
#include"sha256_scalar.h"
#endif
#elif (defined(_MSC_VER)&&!defined(__clang__)) && (defined(__AVX__) || (defined(__SSE_4_2__)&&defined(__SHA__)) )
#include"sha256_x86_sha_extensions_msvc.h"
#else
#include"sha256_scalar.h"
#endif

class sha256
{
public:
	using state_value_type = std::uint_least32_t;
	static inline constexpr std::size_t block_size{64};
	static inline constexpr std::endian hash_endian{std::endian::big};
	static inline constexpr std::size_t state_size{8};
	state_value_type state[state_size]{0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
	constexpr
#endif
	void update_blocks(std::byte const* blocks_start,std::byte const* blocks_last) noexcept
	{
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if(__builtin_is_constant_evaluated())
#endif
		{
			::fast_io::details::sha256::sha256_do_constexpr_function(this->state,blocks_start,blocks_last);
		}
		else
#endif
		{
			::fast_io::details::sha256::sha256_runtime_routine(this->state,blocks_start,blocks_last);
		}
	}
};

struct sha224_initializer
{
	static inline constexpr std::size_t digest_size{28};
	static inline constexpr ::fast_io::details::sha256::sha256 initialize_value{{0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939, 0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4}};
	static inline constexpr void digest_to_byte_ptr(std::uint_least32_t const* digest,std::byte* ptr) noexcept
	{
		hash_digest_to_byte_ptr_common<::std::uint_least32_t,digest_size,std::endian::big>(digest,ptr);
	}
};

struct sha256_initializer
{
	static inline constexpr std::size_t digest_size{32};
	static inline constexpr ::fast_io::details::sha256::sha256 initialize_value{{0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19}};
	static inline constexpr void digest_to_byte_ptr(std::uint_least32_t const* digest,std::byte* ptr) noexcept
	{
		hash_digest_to_byte_ptr_common<::std::uint_least32_t,digest_size,std::endian::big>(digest,ptr);
	}
};

}

namespace fast_io
{
using sha224_context = ::fast_io::details::basic_md5_sha_context_impl<::fast_io::details::sha256::sha256,::fast_io::details::sha256::sha224_initializer,64>;
using sha256_context = ::fast_io::details::basic_md5_sha_context_impl<::fast_io::details::sha256::sha256,::fast_io::details::sha256::sha256_initializer,64>;
}
