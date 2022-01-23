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

inline constexpr std::uint_least32_t B2U32(std::byte val, std::uint8_t sh) noexcept
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


#if defined(FAST_IO_ARM_SHA) && ( defined(__arm__) || defined(__aarch32__) || defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM) )

inline void sha256_arm_function(std::uint_least32_t* __restrict state,std::byte const* __restrict data,std::byte const* __restrict block_last) noexcept
{
	constexpr std::size_t block_size{64};
	uint32x4_t STATE0, STATE1, ABEF_SAVE, CDGH_SAVE;
	uint32x4_t MSG0, MSG1, MSG2, MSG3;
	uint32x4_t TMP0, TMP1, TMP2;

	STATE0 = vld1q_u32(state));
	STATE1 = vld1q_u32(state+4);
	for(;data!=block_last;data+=block_size)
	{
		ABEF_SAVE = STATE0;
		CDGH_SAVE = STATE1;

		/* Load message */
		MSG0 = vld1q_u32(reinterpret_cast<std::uint_least32_t const *>(data +  0));
		MSG1 = vld1q_u32(reinterpret_cast<std::uint_least32_t const *>(data + 16));
		MSG2 = vld1q_u32(reinterpret_cast<std::uint_least32_t const *>(data + 32));
		MSG3 = vld1q_u32(reinterpret_cast<std::uint_least32_t const *>(data + 48));

		/* Reverse for little endian */
		MSG0 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG0)));
		MSG1 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG1)));
		MSG2 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG2)));
		MSG3 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG3)));

		TMP0 = vaddq_u32(MSG0, vld1q_u32(K256);

		/* Rounds 0-3 */
		MSG0 = vsha256su0q_u32(MSG0, MSG1);
		TMP2 = STATE0;
		TMP1 = vaddq_u32(MSG1, vld1q_u32(K256+4);
		STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
		STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
		MSG0 = vsha256su1q_u32(MSG0, MSG2, MSG3);

		/* Rounds 4-7 */
		MSG1 = vsha256su0q_u32(MSG1, MSG2);
		TMP2 = STATE0;
		TMP0 = vaddq_u32(MSG2, vld1q_u32(K256+8);
		STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
		STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
		MSG1 = vsha256su1q_u32(MSG1, MSG3, MSG0);

		/* Rounds 8-11 */
		MSG2 = vsha256su0q_u32(MSG2, MSG3);
		TMP2 = STATE0;
		TMP1 = vaddq_u32(MSG3, vld1q_u32(K256+12);
		STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
		STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
		MSG2 = vsha256su1q_u32(MSG2, MSG0, MSG1);

		/* Rounds 12-15 */
		MSG3 = vsha256su0q_u32(MSG3, MSG0);
		TMP2 = STATE0;
		TMP0 = vaddq_u32(MSG0, vld1q_u32(K256+16);
		STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
		STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
		MSG3 = vsha256su1q_u32(MSG3, MSG1, MSG2);

		/* Rounds 16-19 */
		MSG0 = vsha256su0q_u32(MSG0, MSG1);
		TMP2 = STATE0;
		TMP1 = vaddq_u32(MSG1, vld1q_u32(K256+20);
		STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
		STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
		MSG0 = vsha256su1q_u32(MSG0, MSG2, MSG3);

		/* Rounds 20-23 */
		MSG1 = vsha256su0q_u32(MSG1, MSG2);
		TMP2 = STATE0;
		TMP0 = vaddq_u32(MSG2, vld1q_u32(K256+24);
		STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
		STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
		MSG1 = vsha256su1q_u32(MSG1, MSG3, MSG0);

		/* Rounds 24-27 */
		MSG2 = vsha256su0q_u32(MSG2, MSG3);
		TMP2 = STATE0;
		TMP1 = vaddq_u32(MSG3, vld1q_u32(K256+28);
		STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
		STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
		MSG2 = vsha256su1q_u32(MSG2, MSG0, MSG1);

		/* Rounds 28-31 */
		MSG3 = vsha256su0q_u32(MSG3, MSG0);
		TMP2 = STATE0;
		TMP0 = vaddq_u32(MSG0, vld1q_u32(K256+32);
		STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
		STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
		MSG3 = vsha256su1q_u32(MSG3, MSG1, MSG2);

		/* Rounds 32-35 */
		MSG0 = vsha256su0q_u32(MSG0, MSG1);
		TMP2 = STATE0;
		TMP1 = vaddq_u32(MSG1, vld1q_u32(K256+38);
		STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
		STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
		MSG0 = vsha256su1q_u32(MSG0, MSG2, MSG3);

		/* Rounds 36-39 */
		MSG1 = vsha256su0q_u32(MSG1, MSG2);
		TMP2 = STATE0;
		TMP0 = vaddq_u32(MSG2, vld1q_u32(K256+42);
		STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
		STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
		MSG1 = vsha256su1q_u32(MSG1, MSG3, MSG0);

		/* Rounds 40-43 */
		MSG2 = vsha256su0q_u32(MSG2, MSG3);
		TMP2 = STATE0;
		TMP1 = vaddq_u32(MSG3, vld1q_u32(K256+46);
		STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
		STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
		MSG2 = vsha256su1q_u32(MSG2, MSG0, MSG1);

		/* Rounds 44-47 */
		MSG3 = vsha256su0q_u32(MSG3, MSG0);
		TMP2 = STATE0;
		TMP0 = vaddq_u32(MSG0, vld1q_u32(K256+50);
		STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
		STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
		MSG3 = vsha256su1q_u32(MSG3, MSG1, MSG2);

		/* Rounds 48-51 */
		TMP2 = STATE0;
		TMP1 = vaddq_u32(MSG1, vld1q_u32(K256+54);
		STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
		STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);

		/* Rounds 52-55 */
		TMP2 = STATE0;
		TMP0 = vaddq_u32(MSG2, vld1q_u32(K256+58);
		STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
		STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);

		/* Rounds 56-59 */
		TMP2 = STATE0;
		TMP1 = vaddq_u32(MSG3, vld1q_u32(K256+60);
		STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
		STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);

		/* Rounds 60-63 */
		TMP2 = STATE0;
		STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
		STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);

		/* Combine state */
		STATE0 = vaddq_u32(STATE0, ABEF_SAVE);
		STATE1 = vaddq_u32(STATE1, CDGH_SAVE);
	}
	vst1q_u32(state, STATE0);
	vst1q_u32(state+4, STATE1);
}
#endif

inline void sha256_do_function(std::uint_least32_t* __restrict state,std::byte const* __restrict block,std::byte const* __restrict blocks_last) noexcept
{
#if defined(__has_builtin)
#if __has_builtin(__builtin_ia32_sha256rnds2)&& \
__has_builtin(__builtin_ia32_pshufd)&& \
__has_builtin(__builtin_ia32_palignr128) && \
__has_builtin(__builtin_ia32_sha256msg1) && \
__has_builtin(__builtin_ia32_sha256msg2) && \
__has_builtin(__builtin_ia32_pshufb128) && (!defined(__clang__)||(defined(__SSE4_2__)&&defined(__SHA__)))
	constexpr std::size_t block_size{64};
	using ::fast_io::intrinsics::simd_vector;
	simd_vector<int,4> state0st{static_cast<int>(state[5]),static_cast<int>(state[4]),
				static_cast<int>(state[1]),static_cast<int>(state[0])};
	simd_vector<int,4> state1st{static_cast<int>(state[7]),static_cast<int>(state[6]),
				static_cast<int>(state[3]),static_cast<int>(state[2])};
	simd_vector<int,4> state0=state0st;
	simd_vector<int,4> state1=state1st;
	simd_vector<int,4> msg,msg0,msg1,msg2,msg3;
	constexpr int compiler_magic{
#if defined(__clang__)
4		//clang and gcc disagree with __builtin_ia32_palignr128
#else
32
#endif
	};
	for(;block!=blocks_last;block+=block_size)
	{
		/* Rounds 0-3 */
		msg0.load(block);
		msg0.swap_endian();
		{
		//0xE9B5DBA5B5C0FBCFULL, 0x71374491428A2F98ULL
		constexpr simd_vector<int,4> constant{1116352408,1899447441,-1245643825,-373957723};
		msg=wrap_add(msg0,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg = __builtin_ia32_pshufd(msg, 0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);

		/* Rounds 4-7 */
		msg1.load(block+16);
		msg1.swap_endian();
		{
		//0xAB1C5ED5923F82A4ULL, 0x59F111F13956C25BULL
		constexpr simd_vector<int,4> constant{961987163,1508970993,-1841331548,-1424204075};
		msg=wrap_add(msg1,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg = __builtin_ia32_pshufd(msg, 0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);
		msg0 = __builtin_ia32_sha256msg1(msg0,msg1);

		/* Rounds 8-11 */
		msg2.load(block+32);
		msg2.swap_endian();
		{
			constexpr simd_vector<int,4> constant{-670586216,310598401,607225278,1426881987};
			msg = wrap_add(msg2,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg = __builtin_ia32_pshufd(msg, 0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);
		msg1 = __builtin_ia32_sha256msg1(msg1,msg2);

		/* Rounds 12-15 */
		msg3.load(block+48);
		msg3.swap_endian();
		{
			constexpr simd_vector<int,4> constant{1925078388,-2132889090,-1680079193,-1046744716};
			msg = wrap_add(msg3,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg0.wrap_add_assign(__builtin_ia32_palignr128(
					static_cast<simd_vector<long long,2>>(msg3),
					static_cast<simd_vector<long long,2>>(msg2),compiler_magic));
		msg0=__builtin_ia32_sha256msg2(msg0,msg3);
		msg = __builtin_ia32_pshufd(msg, 0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);	
		msg2 = __builtin_ia32_sha256msg1(msg2,msg3);

		/* Rounds 16-19 */
		{
		constexpr simd_vector<int,4> constant{-459576895,-272742522,264347078,604807628};
		msg = wrap_add(msg0,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);

		msg1.wrap_add_assign(__builtin_ia32_palignr128(static_cast<simd_vector<long long,2>>(msg0),
					static_cast<simd_vector<long long,2>>(msg3),compiler_magic));

		msg1=__builtin_ia32_sha256msg2(msg1,msg0);
		msg = __builtin_ia32_pshufd(msg, 0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);		
		msg3 = __builtin_ia32_sha256msg1(msg3,msg0);

		/* Rounds 20-23 */
		{
			constexpr simd_vector<int,4> constant{770255983,1249150122,1555081692,1996064986};
			msg = wrap_add(msg1,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg2.wrap_add_assign(__builtin_ia32_palignr128(static_cast<simd_vector<long long,2>>(msg1),
					static_cast<simd_vector<long long,2>>(msg0),compiler_magic));
		msg2=__builtin_ia32_sha256msg2(msg2,msg1);
		msg = __builtin_ia32_pshufd(msg, 0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);		
		msg0 = __builtin_ia32_sha256msg1(msg0,msg1);

		/* Rounds 24-27 */
		{
			constexpr simd_vector<int,4> constant{-1740746414,-1473132947,-1341970488,-1084653625};
			msg = wrap_add(msg2,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg3.wrap_add_assign(__builtin_ia32_palignr128(static_cast<simd_vector<long long,2>>(msg2),
					static_cast<simd_vector<long long,2>>(msg1),compiler_magic));
		msg3=__builtin_ia32_sha256msg2(msg3,msg2);
		msg = __builtin_ia32_pshufd(msg, 0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);		
		msg1 = __builtin_ia32_sha256msg1(msg1,msg2);

		/* Rounds 28-31 */
		{
			constexpr simd_vector<int,4> constant{-958395405,-710438585,113926993,338241895};
			msg = wrap_add(msg3,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg0.wrap_add_assign(__builtin_ia32_palignr128(static_cast<simd_vector<long long,2>>(msg3),
					static_cast<simd_vector<long long,2>>(msg2),compiler_magic));
		msg0=__builtin_ia32_sha256msg2(msg0,msg3);
		msg = __builtin_ia32_pshufd(msg, 0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);		
		msg2 = __builtin_ia32_sha256msg1(msg2,msg3);

		/* Rounds 32-35 */
		{
			constexpr simd_vector<int,4> constant{666307205,773529912,1294757372,1396182291};
			msg = wrap_add(msg0,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg1.wrap_add_assign(__builtin_ia32_palignr128(static_cast<simd_vector<long long,2>>(msg0),
					static_cast<simd_vector<long long,2>>(msg3),compiler_magic));
		msg1=__builtin_ia32_sha256msg2(msg1,msg0);
		msg = __builtin_ia32_pshufd(msg, 0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);		
		msg3 = __builtin_ia32_sha256msg1(msg3,msg0);

		/* Rounds 36-39 */
		{
			constexpr simd_vector<int,4> constant{1695183700,1986661051,-2117940946,-1838011259};
			msg = wrap_add(msg1,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg2.wrap_add_assign(__builtin_ia32_palignr128(static_cast<simd_vector<long long,2>>(msg1),
					static_cast<simd_vector<long long,2>>(msg0),compiler_magic));
		msg2 = __builtin_ia32_sha256msg2(msg2,msg1);
		msg = __builtin_ia32_pshufd(msg,0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);
		msg0 = __builtin_ia32_sha256msg1(msg0,msg1);

		/* Rounds 40-43 */
		{
			constexpr simd_vector<int,4> constant{-1564481375,-1474664885,-1035236496,-949202525};
			msg = wrap_add(msg2,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg3.wrap_add_assign(__builtin_ia32_palignr128(static_cast<simd_vector<long long,2>>(msg2),
					static_cast<simd_vector<long long,2>>(msg1),compiler_magic));
		msg3 = __builtin_ia32_sha256msg2(msg3,msg2);
		msg = __builtin_ia32_pshufd(msg,0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);
		msg1 = __builtin_ia32_sha256msg1(msg1,msg2);

		/* Rounds 44-47 */
		{
			constexpr simd_vector<int,4> constant{-778901479,-694614492,-200395387,275423344};
			msg = wrap_add(msg3,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg0.wrap_add_assign(__builtin_ia32_palignr128(static_cast<simd_vector<long long,2>>(msg3),
					static_cast<simd_vector<long long,2>>(msg2),compiler_magic));
		msg0 = __builtin_ia32_sha256msg2(msg0,msg3);
		msg = __builtin_ia32_pshufd(msg,0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);
		msg2 = __builtin_ia32_sha256msg1(msg2,msg3);

		/* Rounds 48-51 */
		{
			constexpr simd_vector<int,4> constant{430227734,506948616,659060556,883997877};
			msg = wrap_add(msg0,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg1.wrap_add_assign(__builtin_ia32_palignr128(static_cast<simd_vector<long long,2>>(msg0),
					static_cast<simd_vector<long long,2>>(msg3),compiler_magic));
		msg1 = __builtin_ia32_sha256msg2(msg1,msg0);
		msg = __builtin_ia32_pshufd(msg,0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);
		msg3 = __builtin_ia32_sha256msg1(msg3,msg0);

		/* Rounds 52-55 */
		{
			constexpr simd_vector<int,4> constant{958139571,1322822218,1537002063,1747873779};
			msg = wrap_add(msg1,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg2.wrap_add_assign(__builtin_ia32_palignr128(static_cast<simd_vector<long long,2>>(msg1),
					static_cast<simd_vector<long long,2>>(msg0),compiler_magic));
		msg2 = __builtin_ia32_sha256msg2(msg2,msg1);
		msg = __builtin_ia32_pshufd(msg,0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);

		/* Rounds 56-59 */
		{
			constexpr simd_vector<int,4> constant{1955562222,2024104815,-2067236844,-1933114872};
			msg = wrap_add(msg2,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg3.wrap_add_assign(__builtin_ia32_palignr128(static_cast<simd_vector<long long,2>>(msg2),
					static_cast<simd_vector<long long,2>>(msg1),compiler_magic));
		msg3 = __builtin_ia32_sha256msg2(msg3,msg2);
		msg = __builtin_ia32_pshufd(msg,0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);

		/* Rounds 60-63 */
		{
			constexpr simd_vector<int,4> constant{-1866530822,-1538233109,-1090935817,-965641998};
			msg = wrap_add(msg3,constant);
		}
		state1 = __builtin_ia32_sha256rnds2(state1,state0,msg);
		msg = __builtin_ia32_pshufd(msg,0x0E);
		state0 = __builtin_ia32_sha256rnds2(state0,state1,msg);


		/* Combine state  */
		state0st.wrap_add_assign(state0);
		state0 = state0st;
		state1st.wrap_add_assign(state1);
		state1 = state1st;
	}
	state[0]=static_cast<std::uint_least32_t>(state0[3]);
	state[1]=static_cast<std::uint_least32_t>(state0[2]);
	state[2]=static_cast<std::uint_least32_t>(state1[3]);
	state[3]=static_cast<std::uint_least32_t>(state1[2]);
	state[4]=static_cast<std::uint_least32_t>(state0[1]);
	state[5]=static_cast<std::uint_least32_t>(state0[0]);
	state[6]=static_cast<std::uint_least32_t>(state1[1]);
	state[7]=static_cast<std::uint_least32_t>(state1[0]);
#else
	sha256_do_constexpr_function(state,block,blocks_last);
#endif
#elif (defined(_MSC_VER)&&!defined(__clang__)) && (defined(__AVX__) || (defined(__SSE_4_2__)&&defined(__SHA__)) )
	constexpr std::size_t block_size{64};
	__m128i STATE0, STATE1;
	__m128i MSG, TMP;
	__m128i MSG0, MSG1, MSG2, MSG3;
	__m128i ABEF_SAVE, CDGH_SAVE;
	__m128i const MASK = _mm_set_epi64x(0x0c0d0e0f08090a0bULL, 0x0405060700010203ULL);

	/* Load initial values */
	TMP = _mm_loadu_si128((__m128i const*) state);
	STATE1 = _mm_loadu_si128((__m128i const*) (state+4));

	TMP = _mm_shuffle_epi32(TMP, 0xB1);          /* CDAB */
	STATE1 = _mm_shuffle_epi32(STATE1, 0x1B);    /* EFGH */
	STATE0 = _mm_alignr_epi8(TMP, STATE1, 8);    /* ABEF */
	STATE1 = _mm_blend_epi16(STATE1, TMP, 0xF0); /* CDGH */
	for(;block!=blocks_last;block+=block_size)
	{
		/* Save current state */
		ABEF_SAVE = STATE0;
		CDGH_SAVE = STATE1;

		/* Rounds 0-3 */
		MSG = _mm_loadu_si128((__m128i const*) block);
		MSG0 = _mm_shuffle_epi8(MSG, MASK);
		MSG = _mm_add_epi32(MSG0, _mm_set_epi64x(0xE9B5DBA5B5C0FBCFULL, 0x71374491428A2F98ULL));
		STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
		MSG = _mm_shuffle_epi32(MSG, 0x0E);
		STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);

		/* Rounds 4-7 */
		MSG1 = _mm_loadu_si128((__m128i const*) (block+16));
		MSG1 = _mm_shuffle_epi8(MSG1, MASK);
		MSG = _mm_add_epi32(MSG1, _mm_set_epi64x(0xAB1C5ED5923F82A4ULL, 0x59F111F13956C25BULL));
		STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
		MSG = _mm_shuffle_epi32(MSG, 0x0E);
		STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
		MSG0 = _mm_sha256msg1_epu32(MSG0, MSG1);

		/* Rounds 8-11 */
		MSG2 = _mm_loadu_si128((__m128i const*) (block+32));
		MSG2 = _mm_shuffle_epi8(MSG2, MASK);
		MSG = _mm_add_epi32(MSG2, _mm_set_epi64x(0x550C7DC3243185BEULL, 0x12835B01D807AA98ULL));
		STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
		MSG = _mm_shuffle_epi32(MSG, 0x0E);
		STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
		MSG1 = _mm_sha256msg1_epu32(MSG1, MSG2);


		/* Rounds 12-15 */
		MSG3 = _mm_loadu_si128((__m128i const*) (block+48));
		MSG3 = _mm_shuffle_epi8(MSG3, MASK);
		MSG = _mm_add_epi32(MSG3, _mm_set_epi64x(0xC19BF1749BDC06A7ULL, 0x80DEB1FE72BE5D74ULL));
		STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
		TMP = _mm_alignr_epi8(MSG3, MSG2, 4);
		MSG0 = _mm_add_epi32(MSG0, TMP);
		MSG0 = _mm_sha256msg2_epu32(MSG0, MSG3);
		MSG = _mm_shuffle_epi32(MSG, 0x0E);
		STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
		MSG2 = _mm_sha256msg1_epu32(MSG2, MSG3);

		/* Rounds 16-19 */
		MSG = _mm_add_epi32(MSG0, _mm_set_epi64x(0x240CA1CC0FC19DC6ULL, 0xEFBE4786E49B69C1ULL));
		STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
		TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
		MSG1 = _mm_add_epi32(MSG1, TMP);
		MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
		MSG = _mm_shuffle_epi32(MSG, 0x0E);
		STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
		MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

		/* Rounds 20-23 */
		MSG = _mm_add_epi32(MSG1, _mm_set_epi64x(0x76F988DA5CB0A9DCULL, 0x4A7484AA2DE92C6FULL));
		STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
		TMP = _mm_alignr_epi8(MSG1, MSG0, 4);
		MSG2 = _mm_add_epi32(MSG2, TMP);
		MSG2 = _mm_sha256msg2_epu32(MSG2, MSG1);
		MSG = _mm_shuffle_epi32(MSG, 0x0E);
		STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
		MSG0 = _mm_sha256msg1_epu32(MSG0, MSG1);

		/* Rounds 24-27 */
		MSG = _mm_add_epi32(MSG2, _mm_set_epi64x(0xBF597FC7B00327C8ULL, 0xA831C66D983E5152ULL));
		STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
		TMP = _mm_alignr_epi8(MSG2, MSG1, 4);
		MSG3 = _mm_add_epi32(MSG3, TMP);
		MSG3 = _mm_sha256msg2_epu32(MSG3, MSG2);
		MSG = _mm_shuffle_epi32(MSG, 0x0E);
		STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
		MSG1 = _mm_sha256msg1_epu32(MSG1, MSG2);

		/* Rounds 28-31 */
		MSG = _mm_add_epi32(MSG3, _mm_set_epi64x(0x1429296706CA6351ULL,  0xD5A79147C6E00BF3ULL));
		STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
		TMP = _mm_alignr_epi8(MSG3, MSG2, 4);
		MSG0 = _mm_add_epi32(MSG0, TMP);
		MSG0 = _mm_sha256msg2_epu32(MSG0, MSG3);
		MSG = _mm_shuffle_epi32(MSG, 0x0E);
		STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
		MSG2 = _mm_sha256msg1_epu32(MSG2, MSG3);

		/* Rounds 32-35 */
		MSG = _mm_add_epi32(MSG0, _mm_set_epi64x(0x53380D134D2C6DFCULL, 0x2E1B213827B70A85ULL));
		STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
		TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
		MSG1 = _mm_add_epi32(MSG1, TMP);
		MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
		MSG = _mm_shuffle_epi32(MSG, 0x0E);
		STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
		MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

		/* Rounds 36-39 */
		MSG = _mm_add_epi32(MSG1, _mm_set_epi64x(0x92722C8581C2C92EULL, 0x766A0ABB650A7354ULL));
		STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
		TMP = _mm_alignr_epi8(MSG1, MSG0, 4);
		MSG2 = _mm_add_epi32(MSG2, TMP);
		MSG2 = _mm_sha256msg2_epu32(MSG2, MSG1);
		MSG = _mm_shuffle_epi32(MSG, 0x0E);
		STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
		MSG0 = _mm_sha256msg1_epu32(MSG0, MSG1);

		/* Rounds 40-43 */
		MSG = _mm_add_epi32(MSG2, _mm_set_epi64x(0xC76C51A3C24B8B70ULL, 0xA81A664BA2BFE8A1ULL));
		STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
		TMP = _mm_alignr_epi8(MSG2, MSG1, 4);
		MSG3 = _mm_add_epi32(MSG3, TMP);
		MSG3 = _mm_sha256msg2_epu32(MSG3, MSG2);
		MSG = _mm_shuffle_epi32(MSG, 0x0E);
		STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
		MSG1 = _mm_sha256msg1_epu32(MSG1, MSG2);

		/* Rounds 44-47 */
		MSG = _mm_add_epi32(MSG3, _mm_set_epi64x(0x106AA070F40E3585ULL, 0xD6990624D192E819ULL));
		STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
		TMP = _mm_alignr_epi8(MSG3, MSG2, 4);
		MSG0 = _mm_add_epi32(MSG0, TMP);
		MSG0 = _mm_sha256msg2_epu32(MSG0, MSG3);
		MSG = _mm_shuffle_epi32(MSG, 0x0E);
		STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
		MSG2 = _mm_sha256msg1_epu32(MSG2, MSG3);

		/* Rounds 48-51 */
		MSG = _mm_add_epi32(MSG0, _mm_set_epi64x(0x34B0BCB52748774CULL, 0x1E376C0819A4C116ULL));
		STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
		TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
		MSG1 = _mm_add_epi32(MSG1, TMP);
		MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
		MSG = _mm_shuffle_epi32(MSG, 0x0E);
		STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
		MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

		/* Rounds 52-55 */
		MSG = _mm_add_epi32(MSG1, _mm_set_epi64x(0x682E6FF35B9CCA4FULL, 0x4ED8AA4A391C0CB3ULL));
		STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
		TMP = _mm_alignr_epi8(MSG1, MSG0, 4);
		MSG2 = _mm_add_epi32(MSG2, TMP);
		MSG2 = _mm_sha256msg2_epu32(MSG2, MSG1);
		MSG = _mm_shuffle_epi32(MSG, 0x0E);
		STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);

		/* Rounds 56-59 */
		MSG = _mm_add_epi32(MSG2, _mm_set_epi64x(0x8CC7020884C87814ULL, 0x78A5636F748F82EEULL));
		STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
		TMP = _mm_alignr_epi8(MSG2, MSG1, 4);
		MSG3 = _mm_add_epi32(MSG3, TMP);
		MSG3 = _mm_sha256msg2_epu32(MSG3, MSG2);
		MSG = _mm_shuffle_epi32(MSG, 0x0E);
		STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);

		/* Rounds 60-63 */
		MSG = _mm_add_epi32(MSG3, _mm_set_epi64x(0xC67178F2BEF9A3F7ULL, 0xA4506CEB90BEFFFAULL));
		STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
		MSG = _mm_shuffle_epi32(MSG, 0x0E);
		STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);

		/* Combine state  */
		STATE0 = _mm_add_epi32(STATE0, ABEF_SAVE);
		STATE1 = _mm_add_epi32(STATE1, CDGH_SAVE);
	}
	TMP = _mm_shuffle_epi32(STATE0, 0x1B);       /* FEBA */
	STATE1 = _mm_shuffle_epi32(STATE1, 0xB1);    /* DCHG */
	STATE0 = _mm_blend_epi16(TMP, STATE1, 0xF0); /* DCBA */
	STATE1 = _mm_alignr_epi8(STATE1, TMP, 8);    /* ABEF */
	/* Save state */
	_mm_storeu_si128((__m128i*) (state), STATE0);
	_mm_storeu_si128((__m128i*) (state+4), STATE1);
#else
	sha256_do_constexpr_function(state,block,blocks_last);
#endif
}

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
			::fast_io::details::sha256::sha256_do_function(this->state,blocks_start,blocks_last);
		}
	}
};

struct sha224_initializer
{
	static inline constexpr std::size_t digest_size{28};
	static inline constexpr ::fast_io::details::sha256::sha256 initialize_value{0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939, 0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4};
	static inline constexpr void digest_to_byte_ptr(std::uint_least32_t const* digest,std::byte* ptr) noexcept
	{
		hash_digest_to_byte_ptr_common<::std::uint_least32_t,digest_size,std::endian::big>(digest,ptr);
	}
};

struct sha256_initializer
{
	static inline constexpr std::size_t digest_size{32};
	static inline constexpr ::fast_io::details::sha256::sha256 initialize_value{0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
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
