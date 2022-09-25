#pragma once

namespace fast_io::details::md5
{

enum class operation
{
F,G,H,I
};

template<operation op>
inline constexpr auto unit(auto const x,auto const y,auto const z) noexcept
{
	if constexpr(op==operation::F)
	{
		if constexpr(true)
		{
			return (x&(y^z))^z;
		}
		else
		{
			return (x&y) + ((~x)&z);
		}
	}
	else if constexpr(op==operation::G)
	{
		if constexpr(false)
		{
			return (z&(x^y))^y;
		}
		else
		{
			return (z&x) + ((~z)&y);
		}
	}
	else if constexpr(op==operation::H)
		return x^y^z;
	else
	{
		return y^(x|(~z));
	}
}

template<operation op>
inline constexpr void uu(auto& a,auto b,auto c,auto d,auto x,auto s,auto ac) noexcept
{
	x+=ac;
	x+=a;
	x+=unit<op>(b,c,d);
	a=std::rotl(x,s)+b;
}

template<operation op>
inline constexpr void uutmp(auto& tmp,auto& a,auto b,auto d,auto x,auto s,auto ac) noexcept
{
	x+=ac;
	x+=a;
	if constexpr(op==operation::H)
	{
		tmp^=b;
		x+=tmp;
		tmp^=d;
	}
	else
	{
		auto t2{~b};
		d&=t2;
		d^=tmp;
		tmp=t2;
		x+=d;
	}
	a=std::rotl(x,s)+b;
}

inline
#if __cpp_lib_is_constant_evaluated >= 201811L
constexpr
#endif
void md5_main(std::uint_least32_t * __restrict state,std::byte const* __restrict block,std::byte const* __restrict ed) noexcept
{
	std::uint_least32_t a{*state},b{state[1]},c{state[2]},d{state[3]};

	using ul32_may_alias
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= std::uint_least32_t;

	constexpr std::size_t block_size{64};
	std::uint_least32_t x[16];
	std::uint_least32_t tmp;
	for(;block!=ed;block+=block_size)
	{
#if __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if(__builtin_is_constant_evaluated())
#endif
		{
			for(std::size_t j{};j!=16;++j)
			{
				auto dj{block+j*4};
				x[j]=(std::to_integer<std::uint_least32_t>(*dj))|(std::to_integer<std::uint_least32_t>(dj[1])<<8)|
					(std::to_integer<std::uint_least32_t>(dj[2])<<16)|std::to_integer<std::uint_least32_t>(dj[3]<<24);
			}
			uu<operation::F>(a, b, c, d, x[ 0], 7, 0xd76aa478u);
			uu<operation::F>(d, a, b, c, x[ 1], 12, 0xe8c7b756u);
			uu<operation::F>(c, d, a, b, x[ 2], 17, 0x242070dbu);
			uu<operation::F>(b, c, d, a, x[ 3], 22, 0xc1bdceeeu);
			uu<operation::F>(a, b, c, d, x[ 4], 7, 0xf57c0fafu);
			uu<operation::F>(d, a, b, c, x[ 5], 12, 0x4787c62au);
			uu<operation::F>(c, d, a, b, x[ 6], 17, 0xa8304613u);
			uu<operation::F>(b, c, d, a, x[ 7], 22, 0xfd469501u);
			uu<operation::F>(a, b, c, d, x[ 8], 7, 0x698098d8u);
			uu<operation::F>(d, a, b, c, x[ 9], 12, 0x8b44f7afu);
			uu<operation::F>(c, d, a, b, x[10], 17, 0xffff5bb1u);
			uu<operation::F>(b, c, d, a, x[11], 22, 0x895cd7beu);
			uu<operation::F>(a, b, c, d, x[12], 7, 0x6b901122u);
			uu<operation::F>(d, a, b, c, x[13], 12, 0xfd987193u);
			uu<operation::F>(c, d, a, b, x[14], 17, 0xa679438eu);
			uu<operation::F>(b, c, d, a, x[15], 22, 0x49b40821u);
		}
		else
#endif
		{
			ul32_may_alias const* w{reinterpret_cast<ul32_may_alias const*>(block)};
			uu<operation::F>(a, b, c, d, x[ 0]=little_endian(w[0]), 7, 0xd76aa478u);
			uu<operation::F>(d, a, b, c, x[ 1]=little_endian(w[1]), 12, 0xe8c7b756u);
			uu<operation::F>(c, d, a, b, x[ 2]=little_endian(w[2]), 17, 0x242070dbu);
			uu<operation::F>(b, c, d, a, x[ 3]=little_endian(w[3]), 22, 0xc1bdceeeu);
			uu<operation::F>(a, b, c, d, x[ 4]=little_endian(w[4]), 7, 0xf57c0fafu);
			uu<operation::F>(d, a, b, c, x[ 5]=little_endian(w[5]), 12, 0x4787c62au);
			uu<operation::F>(c, d, a, b, x[ 6]=little_endian(w[6]), 17, 0xa8304613u);
			uu<operation::F>(b, c, d, a, x[ 7]=little_endian(w[7]), 22, 0xfd469501u);
			uu<operation::F>(a, b, c, d, x[ 8]=little_endian(w[8]), 7, 0x698098d8u);
			uu<operation::F>(d, a, b, c, x[ 9]=little_endian(w[9]), 12, 0x8b44f7afu);
			uu<operation::F>(c, d, a, b, x[10]=little_endian(w[10]), 17, 0xffff5bb1u);
			uu<operation::F>(b, c, d, a, x[11]=little_endian(w[11]), 22, 0x895cd7beu);
			uu<operation::F>(a, b, c, d, x[12]=little_endian(w[12]), 7, 0x6b901122u);
			uu<operation::F>(d, a, b, c, x[13]=little_endian(w[13]), 12, 0xfd987193u);
			uu<operation::F>(c, d, a, b, x[14]=little_endian(w[14]), 17, 0xa679438eu);
			uu<operation::F>(b, c, d, a, x[15]=little_endian(w[15]), 22, 0x49b40821u);
		}
		/* Round 2 */
		uu<operation::G>(a, b, c, d, x[ 1], 5, 0xf61e2562u);
		uu<operation::G>(d, a, b, c, x[ 6], 9, 0xc040b340u);
		uu<operation::G>(c, d, a, b, x[11], 14, 0x265e5a51u);
		uu<operation::G>(b, c, d, a, x[ 0], 20, 0xe9b6c7aau);
		uu<operation::G>(a, b, c, d, x[ 5], 5, 0xd62f105du);
		uu<operation::G>(d, a, b, c, x[10], 9,  0x2441453u);
		uu<operation::G>(c, d, a, b, x[15], 14, 0xd8a1e681u);
		uu<operation::G>(b, c, d, a, x[ 4], 20, 0xe7d3fbc8u);
		uu<operation::G>(a, b, c, d, x[ 9], 5, 0x21e1cde6u);
		uu<operation::G>(d, a, b, c, x[14], 9, 0xc33707d6u);
		uu<operation::G>(c, d, a, b, x[ 3], 14, 0xf4d50d87u);
		uu<operation::G>(b, c, d, a, x[ 8], 20, 0x455a14edu);
		uu<operation::G>(a, b, c, d, x[13], 5, 0xa9e3e905u);
		uu<operation::G>(d, a, b, c, x[ 2], 9, 0xfcefa3f8u);
		uu<operation::G>(c, d, a, b, x[ 7], 14, 0x676f02d9u);
		uu<operation::G>(b, c, d, a, x[12], 20, 0x8d2a4c8au);

		/* Round 3 */
		tmp=c^d;
		uutmp<operation::H>(tmp, a, b, d, x[ 5], 4, 0xfffa3942u);
		uutmp<operation::H>(tmp, d, a, c, x[ 8], 11, 0x8771f681u);
		uutmp<operation::H>(tmp, c, d, b, x[11], 16, 0x6d9d6122u);
		uutmp<operation::H>(tmp, b, c, a, x[14], 23, 0xfde5380cu);
		uutmp<operation::H>(tmp, a, b, d, x[ 1], 4, 0xa4beea44u);
		uutmp<operation::H>(tmp, d, a, c, x[ 4], 11, 0x4bdecfa9u);
		uutmp<operation::H>(tmp, c, d, b, x[ 7], 16, 0xf6bb4b60u);
		uutmp<operation::H>(tmp, b, c, a, x[10], 23, 0xbebfbc70u);
		uutmp<operation::H>(tmp, a, b, d, x[13], 4, 0x289b7ec6u);
		uutmp<operation::H>(tmp, d, a, c, x[ 0], 11, 0xeaa127fau);
		uutmp<operation::H>(tmp, c, d, b, x[ 3], 16, 0xd4ef3085u);
		uutmp<operation::H>(tmp, b, c, a, x[ 6], 23,  0x4881d05u);
		uutmp<operation::H>(tmp, a, b, d, x[ 9], 4, 0xd9d4d039u);
		uutmp<operation::H>(tmp, d, a, c, x[12], 11, 0xe6db99e5u);
		uutmp<operation::H>(tmp, c, d, b, x[15], 16, 0x1fa27cf8u);
		uutmp<operation::H>(tmp, b, c, a, x[ 2], 23, 0xc4ac5665u);
		
		/* Round 4 */
		if constexpr(false)
		{
			tmp=~c;
			uutmp<operation::I>(tmp, a, b, d, x[ 0], 6, 0xf4292244u);
			uutmp<operation::I>(tmp, d, a, c, x[ 7], 10, 0x432aff97u);
			uutmp<operation::I>(tmp, c, d, b, x[14], 15, 0xab9423a7u);
			uutmp<operation::I>(tmp, b, c, a, x[ 5], 21, 0xfc93a039u);
			uutmp<operation::I>(tmp, a, b, d, x[12], 6, 0x655b59c3u);
			uutmp<operation::I>(tmp, d, a, c, x[ 3], 10, 0x8f0ccc92u);
			uutmp<operation::I>(tmp, c, d, b, x[10], 15, 0xffeff47du);
			uutmp<operation::I>(tmp, b, c, a, x[ 1], 21, 0x85845dd1u);
			uutmp<operation::I>(tmp, a, b, d, x[ 8], 6, 0x6fa87e4fu);
			uutmp<operation::I>(tmp, d, a, c, x[15], 10, 0xfe2ce6e0u);
			uutmp<operation::I>(tmp, c, d, b, x[ 6], 15, 0xa3014314u);
			uutmp<operation::I>(tmp, b, c, a, x[13], 21, 0x4e0811a1u);
			uutmp<operation::I>(tmp, a, b, d, x[ 4], 6, 0xf7537e82u);
			uutmp<operation::I>(tmp, d, a, c, x[11], 10, 0xbd3af235u);
			uutmp<operation::I>(tmp, c, d, b, x[ 2], 15, 0x2ad7d2bbu);
			uutmp<operation::I>(tmp, b, c, a, x[ 9], 21, 0xeb86d391u);
		}
		else
		{
			uu<operation::I>(a, b, c, d, x[ 0], 6, 0xf4292244u);
			uu<operation::I>(d, a, b, c, x[ 7], 10, 0x432aff97u);
			uu<operation::I>(c, d, a, b, x[14], 15, 0xab9423a7u);
			uu<operation::I>(b, c, d, a, x[ 5], 21, 0xfc93a039u);
			uu<operation::I>(a, b, c, d, x[12], 6, 0x655b59c3u);
			uu<operation::I>(d, a, b, c, x[ 3], 10, 0x8f0ccc92u);
			uu<operation::I>(c, d, a, b, x[10], 15, 0xffeff47du);
			uu<operation::I>(b, c, d, a, x[ 1], 21, 0x85845dd1u);
			uu<operation::I>(a, b, c, d, x[ 8], 6, 0x6fa87e4fu);
			uu<operation::I>(d, a, b, c, x[15], 10, 0xfe2ce6e0u);
			uu<operation::I>(c, d, a, b, x[ 6], 15, 0xa3014314u);
			uu<operation::I>(b, c, d, a, x[13], 21, 0x4e0811a1u);
			uu<operation::I>(a, b, c, d, x[ 4], 6, 0xf7537e82u);
			uu<operation::I>(d, a, b, c, x[11], 10, 0xbd3af235u);
			uu<operation::I>(c, d, a, b, x[ 2], 15, 0x2ad7d2bbu);
			uu<operation::I>(b, c, d, a, x[ 9], 21, 0xeb86d391u);
		}

		a=(*state+=a);
		b=(state[1]+=b);
		c=(state[2]+=c);
		d=(state[3]+=d);
	}
}


inline
#if __cpp_lib_is_constant_evaluated >= 201811L
constexpr
#endif
void md5_main_le(std::uint_least32_t * __restrict state,std::byte const* __restrict block,std::byte const* __restrict ed) noexcept
{
	std::uint_least32_t a{*state},b{state[1]},c{state[2]},d{state[3]};

	using ul32_may_alias
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= std::uint_least32_t;

	constexpr std::size_t block_size{64};
	std::uint_least32_t tmp;
	for(;block!=ed;block+=block_size)
	{
		ul32_may_alias const* x{reinterpret_cast<ul32_may_alias const*>(block)};

		/* Round 1 */
		uu<operation::F>(a, b, c, d, x[ 0], 7, 0xd76aa478u);
		uu<operation::F>(d, a, b, c, x[ 1], 12, 0xe8c7b756u);
		uu<operation::F>(c, d, a, b, x[ 2], 17, 0x242070dbu);
		uu<operation::F>(b, c, d, a, x[ 3], 22, 0xc1bdceeeu);
		uu<operation::F>(a, b, c, d, x[ 4], 7, 0xf57c0fafu);
		uu<operation::F>(d, a, b, c, x[ 5], 12, 0x4787c62au);
		uu<operation::F>(c, d, a, b, x[ 6], 17, 0xa8304613u);
		uu<operation::F>(b, c, d, a, x[ 7], 22, 0xfd469501u);
		uu<operation::F>(a, b, c, d, x[ 8], 7, 0x698098d8u);
		uu<operation::F>(d, a, b, c, x[ 9], 12, 0x8b44f7afu);
		uu<operation::F>(c, d, a, b, x[10], 17, 0xffff5bb1u);
		uu<operation::F>(b, c, d, a, x[11], 22, 0x895cd7beu);
		uu<operation::F>(a, b, c, d, x[12], 7, 0x6b901122u);
		uu<operation::F>(d, a, b, c, x[13], 12, 0xfd987193u);
		uu<operation::F>(c, d, a, b, x[14], 17, 0xa679438eu);
		uu<operation::F>(b, c, d, a, x[15], 22, 0x49b40821u);

		/* Round 2 */
		uu<operation::G>(a, b, c, d, x[ 1], 5, 0xf61e2562u);
		uu<operation::G>(d, a, b, c, x[ 6], 9, 0xc040b340u);
		uu<operation::G>(c, d, a, b, x[11], 14, 0x265e5a51u);
		uu<operation::G>(b, c, d, a, x[ 0], 20, 0xe9b6c7aau);
		uu<operation::G>(a, b, c, d, x[ 5], 5, 0xd62f105du);
		uu<operation::G>(d, a, b, c, x[10], 9,  0x2441453u);
		uu<operation::G>(c, d, a, b, x[15], 14, 0xd8a1e681u);
		uu<operation::G>(b, c, d, a, x[ 4], 20, 0xe7d3fbc8u);
		uu<operation::G>(a, b, c, d, x[ 9], 5, 0x21e1cde6u);
		uu<operation::G>(d, a, b, c, x[14], 9, 0xc33707d6u);
		uu<operation::G>(c, d, a, b, x[ 3], 14, 0xf4d50d87u);
		uu<operation::G>(b, c, d, a, x[ 8], 20, 0x455a14edu);
		uu<operation::G>(a, b, c, d, x[13], 5, 0xa9e3e905u);
		uu<operation::G>(d, a, b, c, x[ 2], 9, 0xfcefa3f8u);
		uu<operation::G>(c, d, a, b, x[ 7], 14, 0x676f02d9u);
		uu<operation::G>(b, c, d, a, x[12], 20, 0x8d2a4c8au);

		/* Round 3 */
		tmp=c^d;
		uutmp<operation::H>(tmp, a, b, d, x[ 5], 4, 0xfffa3942u);
		uutmp<operation::H>(tmp, d, a, c, x[ 8], 11, 0x8771f681u);
		uutmp<operation::H>(tmp, c, d, b, x[11], 16, 0x6d9d6122u);
		uutmp<operation::H>(tmp, b, c, a, x[14], 23, 0xfde5380cu);
		uutmp<operation::H>(tmp, a, b, d, x[ 1], 4, 0xa4beea44u);
		uutmp<operation::H>(tmp, d, a, c, x[ 4], 11, 0x4bdecfa9u);
		uutmp<operation::H>(tmp, c, d, b, x[ 7], 16, 0xf6bb4b60u);
		uutmp<operation::H>(tmp, b, c, a, x[10], 23, 0xbebfbc70u);
		uutmp<operation::H>(tmp, a, b, d, x[13], 4, 0x289b7ec6u);
		uutmp<operation::H>(tmp, d, a, c, x[ 0], 11, 0xeaa127fau);
		uutmp<operation::H>(tmp, c, d, b, x[ 3], 16, 0xd4ef3085u);
		uutmp<operation::H>(tmp, b, c, a, x[ 6], 23,  0x4881d05u);
		uutmp<operation::H>(tmp, a, b, d, x[ 9], 4, 0xd9d4d039u);
		uutmp<operation::H>(tmp, d, a, c, x[12], 11, 0xe6db99e5u);
		uutmp<operation::H>(tmp, c, d, b, x[15], 16, 0x1fa27cf8u);
		uutmp<operation::H>(tmp, b, c, a, x[ 2], 23, 0xc4ac5665u);

		/* Round 4 */
		if constexpr(false)
		{
			tmp=~c;
			uutmp<operation::I>(tmp, a, b, d, x[ 0], 6, 0xf4292244u);
			uutmp<operation::I>(tmp, d, a, c, x[ 7], 10, 0x432aff97u);
			uutmp<operation::I>(tmp, c, d, b, x[14], 15, 0xab9423a7u);
			uutmp<operation::I>(tmp, b, c, a, x[ 5], 21, 0xfc93a039u);
			uutmp<operation::I>(tmp, a, b, d, x[12], 6, 0x655b59c3u);
			uutmp<operation::I>(tmp, d, a, c, x[ 3], 10, 0x8f0ccc92u);
			uutmp<operation::I>(tmp, c, d, b, x[10], 15, 0xffeff47du);
			uutmp<operation::I>(tmp, b, c, a, x[ 1], 21, 0x85845dd1u);
			uutmp<operation::I>(tmp, a, b, d, x[ 8], 6, 0x6fa87e4fu);
			uutmp<operation::I>(tmp, d, a, c, x[15], 10, 0xfe2ce6e0u);
			uutmp<operation::I>(tmp, c, d, b, x[ 6], 15, 0xa3014314u);
			uutmp<operation::I>(tmp, b, c, a, x[13], 21, 0x4e0811a1u);
			uutmp<operation::I>(tmp, a, b, d, x[ 4], 6, 0xf7537e82u);
			uutmp<operation::I>(tmp, d, a, c, x[11], 10, 0xbd3af235u);
			uutmp<operation::I>(tmp, c, d, b, x[ 2], 15, 0x2ad7d2bbu);
			uutmp<operation::I>(tmp, b, c, a, x[ 9], 21, 0xeb86d391u);
		}
		else
		{
			uu<operation::I>(a, b, c, d, x[ 0], 6, 0xf4292244u);
			uu<operation::I>(d, a, b, c, x[ 7], 10, 0x432aff97u);
			uu<operation::I>(c, d, a, b, x[14], 15, 0xab9423a7u);
			uu<operation::I>(b, c, d, a, x[ 5], 21, 0xfc93a039u);
			uu<operation::I>(a, b, c, d, x[12], 6, 0x655b59c3u);
			uu<operation::I>(d, a, b, c, x[ 3], 10, 0x8f0ccc92u);
			uu<operation::I>(c, d, a, b, x[10], 15, 0xffeff47du);
			uu<operation::I>(b, c, d, a, x[ 1], 21, 0x85845dd1u);
			uu<operation::I>(a, b, c, d, x[ 8], 6, 0x6fa87e4fu);
			uu<operation::I>(d, a, b, c, x[15], 10, 0xfe2ce6e0u);
			uu<operation::I>(c, d, a, b, x[ 6], 15, 0xa3014314u);
			uu<operation::I>(b, c, d, a, x[13], 21, 0x4e0811a1u);
			uu<operation::I>(a, b, c, d, x[ 4], 6, 0xf7537e82u);
			uu<operation::I>(d, a, b, c, x[11], 10, 0xbd3af235u);
			uu<operation::I>(c, d, a, b, x[ 2], 15, 0x2ad7d2bbu);
			uu<operation::I>(b, c, d, a, x[ 9], 21, 0xeb86d391u);
		}
		a=(*state+=a);
		b=(state[1]+=b);
		c=(state[2]+=c);
		d=(state[3]+=d);
	}
}

class md5
{
public:
	using state_value_type = std::uint_least32_t;
	static inline constexpr std::size_t block_size{64};
	static inline constexpr std::endian hash_endian{std::endian::little};
	static inline constexpr std::size_t state_size{4};
	state_value_type state[state_size];
#if __cpp_lib_is_constant_evaluated >= 201811L
	constexpr
#endif
	void update_blocks(std::byte const* block_start,std::byte const* block_last) noexcept
	{
#if __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if(__builtin_is_constant_evaluated())
#endif
		{
			::fast_io::details::md5::md5_main(this->state,block_start,block_last);
		}
		else
#endif
		{
			if constexpr(std::endian::native==std::endian::little)
			{
				::fast_io::details::md5::md5_main_le(this->state,block_start,block_last);
			}
			else
			{
				::fast_io::details::md5::md5_main(this->state,block_start,block_last);
			}
		}
	}
};

struct md5_initializer
{
	static inline constexpr std::size_t digest_size{16};
	static inline constexpr ::fast_io::details::md5::md5 initialize_value{{0x67452301,0xefcdab89,0x98badcfe,0x10325476}};
	static inline constexpr void digest_to_byte_ptr(std::uint_least32_t const* digest,std::byte* ptr) noexcept
	{
		hash_digest_to_byte_ptr_common<::std::uint_least32_t,digest_size,std::endian::little>(digest,ptr);
	}
};

}

namespace fast_io
{
using md5_context
#ifndef FAST_IO_NO_WARNING_DEPRECATED_CRYPTO_ALGOS
[[deprecated("The weaknesses of MD5 have been exploited in the field, most infamously by the Flame malware in 2012. See wikipedia https://en.wikipedia.org/wiki/MD5")]]
#endif
= ::fast_io::details::basic_md5_sha_context_impl<::fast_io::details::md5::md5,::fast_io::details::md5::md5_initializer,64>;
}
