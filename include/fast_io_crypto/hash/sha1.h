#pragma once

namespace fast_io::details::sha1
{
//from https://github.com/vog/sha1/blob/master/sha1.cpp
inline constexpr std::uint_least32_t blk(std::uint_least32_t* __restrict block,std::size_t const i) noexcept
{
	return std::rotl(block[(i+13)&15] ^ block[(i+8)&15] ^ block[(i+2)&15] ^ block[i], 1);
}

inline constexpr void R0(std::uint_least32_t* __restrict block, std::uint_least32_t const v, std::uint_least32_t &w, std::uint_least32_t const x, std::uint_least32_t const y, std::uint_least32_t &z, std::size_t i) noexcept
{
	z += ((w&(x^y))^y) + block[i] + 0x5a827999 + std::rotl(v, 5);
	w = std::rotl(w, 30);
}

inline constexpr void R1(std::uint_least32_t* __restrict block, std::uint_least32_t const v, std::uint_least32_t &w, std::uint_least32_t const x, std::uint_least32_t const y, std::uint_least32_t &z, std::size_t i) noexcept
{
	block[i] = blk(block, i);
	z += ((w&(x^y))^y) + block[i] + 0x5a827999 + std::rotl(v, 5);
	w = std::rotl(w, 30);
}


inline constexpr void R2(std::uint_least32_t* __restrict block, std::uint_least32_t const v, std::uint_least32_t &w, std::uint_least32_t const x, std::uint_least32_t const y, std::uint_least32_t &z, std::size_t i) noexcept
{
	block[i] = blk(block, i);
	z += (w^x^y) + block[i] + 0x6ed9eba1 + std::rotl(v, 5);
	w = std::rotl(w, 30);
}


inline constexpr void R3(std::uint_least32_t* __restrict block, std::uint_least32_t const v, std::uint_least32_t &w, std::uint_least32_t const x, std::uint_least32_t const y, std::uint_least32_t &z, std::size_t i) noexcept
{
	block[i] = blk(block, i);
	z += (((w|x)&y)|(w&x)) + block[i] + 0x8f1bbcdc + std::rotl(v, 5);
	w = std::rotl(w, 30);
}


inline constexpr void R4(std::uint_least32_t* __restrict block, std::uint_least32_t const v, std::uint_least32_t &w, std::uint_least32_t const x, std::uint_least32_t const y, std::uint_least32_t &z, std::size_t i) noexcept
{
	block[i] = blk(block, i);
	z += (w^x^y) + block[i] + 0xca62c1d6 + std::rotl(v, 5);
	w = std::rotl(w, 30);
}


inline 
#if __cpp_lib_is_constant_evaluated >= 201811L
constexpr
#endif
void sha1_do_constexpr_function(std::uint_least32_t* __restrict state,std::byte const* __restrict blocks_start,std::size_t blocks_bytes) noexcept
{
	std::uint_least32_t a{state[0]}, b{state[1]}, c{state[2]}, d{state[3]}, e{state[4]};
	std::uint_least32_t X[16];
	constexpr std::size_t block_size{64};
	for(std::byte const* data(blocks_start),*ed(blocks_start+blocks_bytes);data!=ed;data+=block_size)
	{
#if __cpp_lib_is_constant_evaluated >= 201811L
		if(std::is_constant_evaluated())
		{
			for(std::size_t j{};j!=16;++j)
			{
				auto dj{data+j*4};
				X[j]=(std::to_integer<std::uint_least32_t>(*dj)<<24)|(std::to_integer<std::uint_least32_t>(dj[1])<<16)|
					(std::to_integer<std::uint_least32_t>(dj[2])<<8)|std::to_integer<std::uint_least32_t>(dj[3]);
			}
		}
		else
#endif
		{
			my_memcpy(X,data,block_size);
			if constexpr(std::endian::big!=std::endian::native)
			{
				for(auto& ele : X)
					ele=big_endian(ele);
			}
		}
		/* 4 rounds of 20 operations each. Loop unrolled. */
		R0(X, a, b, c, d, e,  0);
		R0(X, e, a, b, c, d,  1);
		R0(X, d, e, a, b, c,  2);
		R0(X, c, d, e, a, b,  3);
		R0(X, b, c, d, e, a,  4);
		R0(X, a, b, c, d, e,  5);
		R0(X, e, a, b, c, d,  6);
		R0(X, d, e, a, b, c,  7);
		R0(X, c, d, e, a, b,  8);
		R0(X, b, c, d, e, a,  9);
		R0(X, a, b, c, d, e, 10);
		R0(X, e, a, b, c, d, 11);
		R0(X, d, e, a, b, c, 12);
		R0(X, c, d, e, a, b, 13);
		R0(X, b, c, d, e, a, 14);
		R0(X, a, b, c, d, e, 15);
		R1(X, e, a, b, c, d,  0);
		R1(X, d, e, a, b, c,  1);
		R1(X, c, d, e, a, b,  2);
		R1(X, b, c, d, e, a,  3);
		R2(X, a, b, c, d, e,  4);
		R2(X, e, a, b, c, d,  5);
		R2(X, d, e, a, b, c,  6);
		R2(X, c, d, e, a, b,  7);
		R2(X, b, c, d, e, a,  8);
		R2(X, a, b, c, d, e,  9);
		R2(X, e, a, b, c, d, 10);
		R2(X, d, e, a, b, c, 11);
		R2(X, c, d, e, a, b, 12);
		R2(X, b, c, d, e, a, 13);
		R2(X, a, b, c, d, e, 14);
		R2(X, e, a, b, c, d, 15);
		R2(X, d, e, a, b, c,  0);
		R2(X, c, d, e, a, b,  1);
		R2(X, b, c, d, e, a,  2);
		R2(X, a, b, c, d, e,  3);
		R2(X, e, a, b, c, d,  4);
		R2(X, d, e, a, b, c,  5);
		R2(X, c, d, e, a, b,  6);
		R2(X, b, c, d, e, a,  7);
		R3(X, a, b, c, d, e,  8);
		R3(X, e, a, b, c, d,  9);
		R3(X, d, e, a, b, c, 10);
		R3(X, c, d, e, a, b, 11);
		R3(X, b, c, d, e, a, 12);
		R3(X, a, b, c, d, e, 13);
		R3(X, e, a, b, c, d, 14);
		R3(X, d, e, a, b, c, 15);
		R3(X, c, d, e, a, b,  0);
		R3(X, b, c, d, e, a,  1);
		R3(X, a, b, c, d, e,  2);
		R3(X, e, a, b, c, d,  3);
		R3(X, d, e, a, b, c,  4);
		R3(X, c, d, e, a, b,  5);
		R3(X, b, c, d, e, a,  6);
		R3(X, a, b, c, d, e,  7);
		R3(X, e, a, b, c, d,  8);
		R3(X, d, e, a, b, c,  9);
		R3(X, c, d, e, a, b, 10);
		R3(X, b, c, d, e, a, 11);
		R4(X, a, b, c, d, e, 12);
		R4(X, e, a, b, c, d, 13);
		R4(X, d, e, a, b, c, 14);
		R4(X, c, d, e, a, b, 15);
		R4(X, b, c, d, e, a,  0);
		R4(X, a, b, c, d, e,  1);
		R4(X, e, a, b, c, d,  2);
		R4(X, d, e, a, b, c,  3);
		R4(X, c, d, e, a, b,  4);
		R4(X, b, c, d, e, a,  5);
		R4(X, a, b, c, d, e,  6);
		R4(X, e, a, b, c, d,  7);
		R4(X, d, e, a, b, c,  8);
		R4(X, c, d, e, a, b,  9);
		R4(X, b, c, d, e, a, 10);
		R4(X, a, b, c, d, e, 11);
		R4(X, e, a, b, c, d, 12);
		R4(X, d, e, a, b, c, 13);
		R4(X, c, d, e, a, b, 14);
		R4(X, b, c, d, e, a, 15);

		/* Add the working vars back into state[] */
		a=(state[0] += a);
		b=(state[1] += b);
		c=(state[2] += c);
		d=(state[3] += d);
		e=(state[4] += e);
	}
}

#if defined(FAST_IO_ARM_SHA) && ( defined(__arm__) || defined(__aarch32__) || defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM) )
inline void sha1_arm_function(std::uint_least32_t* __restrict state,std::byte const* __restrict blocks_start,std::size_t blocks_bytes) noexcept
{
	uint32x4_t ABCD, ABCD_SAVED;
	uint32x4_t TMP0, TMP1;
	uint32x4_t MSG0, MSG1, MSG2, MSG3;
	uint_least32_t   E0, E0_SAVED, E1;

	ABCD = vld1q_u32(state);

	for(auto data(blocks),ed(blocks+blocks_bytes);data!=ed;data+=block_size)
	{
		ABCD_SAVED = ABCD;
		E0_SAVED = E0;

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

		TMP0 = vaddq_u32(MSG0, vdupq_n_u32(0x5A827999));
		TMP1 = vaddq_u32(MSG1, vdupq_n_u32(0x5A827999));

		/* Rounds 0-3 */
		E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
		ABCD = vsha1cq_u32(ABCD, E0, TMP0);
		TMP0 = vaddq_u32(MSG2, vdupq_n_u32(0x5A827999));
		MSG0 = vsha1su0q_u32(MSG0, MSG1, MSG2);

		/* Rounds 4-7 */
		E0 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
		ABCD = vsha1cq_u32(ABCD, E1, TMP1);
		TMP1 = vaddq_u32(MSG3, vdupq_n_u32(0x5A827999));
		MSG0 = vsha1su1q_u32(MSG0, MSG3);
		MSG1 = vsha1su0q_u32(MSG1, MSG2, MSG3);

		/* Rounds 8-11 */
		E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
		ABCD = vsha1cq_u32(ABCD, E0, TMP0);
		TMP0 = vaddq_u32(MSG0, vdupq_n_u32(0x5A827999));
		MSG1 = vsha1su1q_u32(MSG1, MSG0);
		MSG2 = vsha1su0q_u32(MSG2, MSG3, MSG0);

		/* Rounds 12-15 */
		E0 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
		ABCD = vsha1cq_u32(ABCD, E1, TMP1);
		TMP1 = vaddq_u32(MSG1, vdupq_n_u32(0x6ED9EBA1));
		MSG2 = vsha1su1q_u32(MSG2, MSG1);
		MSG3 = vsha1su0q_u32(MSG3, MSG0, MSG1);

		/* Rounds 16-19 */
		E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
		ABCD = vsha1cq_u32(ABCD, E0, TMP0);
		TMP0 = vaddq_u32(MSG2, vdupq_n_u32(0x6ED9EBA1));
		MSG3 = vsha1su1q_u32(MSG3, MSG2);
		MSG0 = vsha1su0q_u32(MSG0, MSG1, MSG2);

		/* Rounds 20-23 */
		E0 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
		ABCD = vsha1pq_u32(ABCD, E1, TMP1);
		TMP1 = vaddq_u32(MSG3, vdupq_n_u32(0x6ED9EBA1));
		MSG0 = vsha1su1q_u32(MSG0, MSG3);
		MSG1 = vsha1su0q_u32(MSG1, MSG2, MSG3);

		/* Rounds 24-27 */
		E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
		ABCD = vsha1pq_u32(ABCD, E0, TMP0);
		TMP0 = vaddq_u32(MSG0, vdupq_n_u32(0x6ED9EBA1));
		MSG1 = vsha1su1q_u32(MSG1, MSG0);
		MSG2 = vsha1su0q_u32(MSG2, MSG3, MSG0);

		/* Rounds 28-31 */
		E0 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
		ABCD = vsha1pq_u32(ABCD, E1, TMP1);
		TMP1 = vaddq_u32(MSG1, vdupq_n_u32(0x6ED9EBA1));
		MSG2 = vsha1su1q_u32(MSG2, MSG1);
		MSG3 = vsha1su0q_u32(MSG3, MSG0, MSG1);

		/* Rounds 32-35 */
		E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
		ABCD = vsha1pq_u32(ABCD, E0, TMP0);
		TMP0 = vaddq_u32(MSG2, vdupq_n_u32(0x8F1BBCDC));
		MSG3 = vsha1su1q_u32(MSG3, MSG2);
		MSG0 = vsha1su0q_u32(MSG0, MSG1, MSG2);

		/* Rounds 36-39 */
		E0 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
		ABCD = vsha1pq_u32(ABCD, E1, TMP1);
		TMP1 = vaddq_u32(MSG3, vdupq_n_u32(0x8F1BBCDC));
		MSG0 = vsha1su1q_u32(MSG0, MSG3);
		MSG1 = vsha1su0q_u32(MSG1, MSG2, MSG3);

		/* Rounds 40-43 */
		E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
		ABCD = vsha1mq_u32(ABCD, E0, TMP0);
		TMP0 = vaddq_u32(MSG0, vdupq_n_u32(0x8F1BBCDC));
		MSG1 = vsha1su1q_u32(MSG1, MSG0);
		MSG2 = vsha1su0q_u32(MSG2, MSG3, MSG0);

		/* Rounds 44-47 */
		E0 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
		ABCD = vsha1mq_u32(ABCD, E1, TMP1);
		TMP1 = vaddq_u32(MSG1, vdupq_n_u32(0x8F1BBCDC));
		MSG2 = vsha1su1q_u32(MSG2, MSG1);
		MSG3 = vsha1su0q_u32(MSG3, MSG0, MSG1);

		/* Rounds 48-51 */
		E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
		ABCD = vsha1mq_u32(ABCD, E0, TMP0);
		TMP0 = vaddq_u32(MSG2, vdupq_n_u32(0x8F1BBCDC));
		MSG3 = vsha1su1q_u32(MSG3, MSG2);
		MSG0 = vsha1su0q_u32(MSG0, MSG1, MSG2);

		/* Rounds 52-55 */
		E0 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
		ABCD = vsha1mq_u32(ABCD, E1, TMP1);
		TMP1 = vaddq_u32(MSG3, vdupq_n_u32(0xCA62C1D6));
		MSG0 = vsha1su1q_u32(MSG0, MSG3);
		MSG1 = vsha1su0q_u32(MSG1, MSG2, MSG3);

		/* Rounds 56-59 */
		E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
		ABCD = vsha1mq_u32(ABCD, E0, TMP0);
		TMP0 = vaddq_u32(MSG0, vdupq_n_u32(0xCA62C1D6));
		MSG1 = vsha1su1q_u32(MSG1, MSG0);
		MSG2 = vsha1su0q_u32(MSG2, MSG3, MSG0);

		/* Rounds 60-63 */
		E0 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
		ABCD = vsha1pq_u32(ABCD, E1, TMP1);
		TMP1 = vaddq_u32(MSG1, vdupq_n_u32(0xCA62C1D6));
		MSG2 = vsha1su1q_u32(MSG2, MSG1);
		MSG3 = vsha1su0q_u32(MSG3, MSG0, MSG1);

		/* Rounds 64-67 */
		E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
		ABCD = vsha1pq_u32(ABCD, E0, TMP0);
		TMP0 = vaddq_u32(MSG2, vdupq_n_u32(0xCA62C1D6));
		MSG3 = vsha1su1q_u32(MSG3, MSG2);
		MSG0 = vsha1su0q_u32(MSG0, MSG1, MSG2);

		/* Rounds 68-71 */
		E0 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
		ABCD = vsha1pq_u32(ABCD, E1, TMP1);
		TMP1 = vaddq_u32(MSG3, vdupq_n_u32(0xCA62C1D6));
		MSG0 = vsha1su1q_u32(MSG0, MSG3);

		/* Rounds 72-75 */
		E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
		ABCD = vsha1pq_u32(ABCD, E0, TMP0);

		/* Rounds 76-79 */
		E0 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
		ABCD = vsha1pq_u32(ABCD, E1, TMP1);

		/* Combine state */
		E0 += E0_SAVED;
		ABCD = vaddq_u32(ABCD_SAVED, ABCD);
	}
	vst1q_u32(state, ABCD);
	state[4] = E0;
}
#endif

inline void sha1_do_function(std::uint_least32_t* __restrict state,std::byte const* __restrict blocks_start,std::size_t blocks_bytes) noexcept
{
	[[maybe_unused]] constexpr std::size_t block_size{64};
#if defined(__has_builtin)
#if __has_builtin(__builtin_ia32_sha1rnds4)&& \
__has_builtin(__builtin_ia32_sha1nexte)&& \
__has_builtin(__builtin_ia32_sha1msg1) && \
__has_builtin(__builtin_ia32_pshufb128) && (!defined(__clang__)||(defined(__SSE4_2__)&&defined(__SHA__)))
	using namespace ::fast_io::intrinsics;
	constexpr simd_vector<char,16> mask{15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0};
	simd_vector<int,4> abcdstate{static_cast<int>(state[3]),static_cast<int>(state[2]),
				static_cast<int>(state[1]),static_cast<int>(*state)};
	simd_vector<int,4> estate{0,0,0,static_cast<int>(state[4])};
	simd_vector<int,4> abcd=abcdstate;
	simd_vector<int,4> e0=estate;
	simd_vector<int,4> msg0,msg1,msg2,msg3;
	for(auto block(blocks_start),ed(blocks_start+blocks_bytes);block!=ed;block+=block_size)
	{
		// Rounds 0-3
		msg0.load(block);
		msg0.shuffle(mask);
		e0.wrap_add_assign(msg0);
		simd_vector<int,4> e1=abcd;
		abcd = __builtin_ia32_sha1rnds4(abcd,e0,0);

		// Rounds 4-7
		msg1.load(block+16);
		msg1.shuffle(mask);
		e1 = __builtin_ia32_sha1nexte(e1,msg1);
		e0 = abcd;
		abcd = __builtin_ia32_sha1rnds4(abcd,e1,0);
		msg0 = __builtin_ia32_sha1msg1(msg0,msg1);

		// Rounds 8-11
		msg2.load(block+32);
		msg2.shuffle(mask);
		e0 = __builtin_ia32_sha1nexte(e0,msg2);
		e1 = abcd;
		abcd = __builtin_ia32_sha1rnds4(abcd,e0,0);
		msg1 = __builtin_ia32_sha1msg1(msg1,msg2);
		msg0^=msg2;

		// Rounds 12-15
		msg3.load(block+48);
		msg3.shuffle(mask);
		e1 = __builtin_ia32_sha1nexte(e1,msg3);
		e0 = abcd;
		msg0 = __builtin_ia32_sha1msg2(msg0,msg3);
		abcd = __builtin_ia32_sha1rnds4(abcd,e1,0);
		msg2 = __builtin_ia32_sha1msg1(msg2,msg3);
		msg1^=msg3;

		// Rounds 16-19
		e0 = __builtin_ia32_sha1nexte(e0,msg0);
		e1 = abcd;
		msg1 = __builtin_ia32_sha1msg2(msg1,msg0);
		abcd = __builtin_ia32_sha1rnds4(abcd,e0,0);
		msg3 = __builtin_ia32_sha1msg1(msg3,msg0);
		msg2^=msg0;

		// Rounds 20-23
		e1 = __builtin_ia32_sha1nexte(e1,msg1);
		e0 = abcd;
		msg2 = __builtin_ia32_sha1msg2(msg2,msg1);
		abcd = __builtin_ia32_sha1rnds4(abcd,e1,1);
		msg0 = __builtin_ia32_sha1msg1(msg0,msg1);
		msg3^=msg1;

		// Rounds 24-27
		e0 = __builtin_ia32_sha1nexte(e0,msg2);
		e1 = abcd;
		msg3 = __builtin_ia32_sha1msg2(msg3,msg2);
		abcd = __builtin_ia32_sha1rnds4(abcd,e0,1);
		msg1 = __builtin_ia32_sha1msg1(msg1,msg2);
		msg0^=msg2;

		// Rounds 28-31
		e1 = __builtin_ia32_sha1nexte(e1,msg3);
		e0 = abcd;
		msg0 = __builtin_ia32_sha1msg2(msg0,msg3);
		abcd = __builtin_ia32_sha1rnds4(abcd,e1,1);
		msg2 = __builtin_ia32_sha1msg1(msg2,msg3);
		msg1^=msg3;

		// Rounds 32-35
		e0 = __builtin_ia32_sha1nexte(e0,msg0);
		e1 = abcd;
		msg1 = __builtin_ia32_sha1msg2(msg1,msg0);
		abcd = __builtin_ia32_sha1rnds4(abcd,e0,1);
		msg3 = __builtin_ia32_sha1msg1(msg3,msg0);
		msg2^=msg0;

		// Rounds 36-39
		e1 = __builtin_ia32_sha1nexte(e1,msg1);
		e0 = abcd;
		msg2 = __builtin_ia32_sha1msg2(msg2,msg1);
		abcd = __builtin_ia32_sha1rnds4(abcd,e1,1);
		msg0 = __builtin_ia32_sha1msg1(msg0,msg1);
		msg3^=msg1;

		// Rounds 40-43
		e0 = __builtin_ia32_sha1nexte(e0,msg2);
		e1 = abcd;
		msg3 = __builtin_ia32_sha1msg2(msg3,msg2);
		abcd = __builtin_ia32_sha1rnds4(abcd,e0,2);
		msg1 = __builtin_ia32_sha1msg1(msg1,msg2);
		msg0^=msg2;

		// Rounds 44-47
		e1 = __builtin_ia32_sha1nexte(e1,msg3);
		e0 = abcd;
		msg0 = __builtin_ia32_sha1msg2(msg0,msg3);
		abcd = __builtin_ia32_sha1rnds4(abcd,e1,2);
		msg2 = __builtin_ia32_sha1msg1(msg2,msg3);
		msg1^=msg3;

		// Rounds 48-51
		e0 = __builtin_ia32_sha1nexte(e0,msg0);
		e1 = abcd;
		msg1 = __builtin_ia32_sha1msg2(msg1,msg0);
		abcd = __builtin_ia32_sha1rnds4(abcd,e0,2);
		msg3 = __builtin_ia32_sha1msg1(msg3,msg0);
		msg2^=msg0;

		// Rounds 52-55
		e1 = __builtin_ia32_sha1nexte(e1,msg1);
		e0 = abcd;
		msg2 = __builtin_ia32_sha1msg2(msg2,msg1);
		abcd = __builtin_ia32_sha1rnds4(abcd,e1,2);
		msg0 = __builtin_ia32_sha1msg1(msg0,msg1);
		msg3^=msg1;

		// Rounds 56-59
		e0 = __builtin_ia32_sha1nexte(e0,msg2);
		e1 = abcd;
		msg3 = __builtin_ia32_sha1msg2(msg3,msg2);
		abcd = __builtin_ia32_sha1rnds4(abcd,e0,2);
		msg1 = __builtin_ia32_sha1msg1(msg1,msg2);
		msg0^=msg2; 

		// Rounds 60-63
		e1 = __builtin_ia32_sha1nexte(e1,msg3);
		e0 = abcd;
		msg0 = __builtin_ia32_sha1msg2(msg0,msg3);
		abcd = __builtin_ia32_sha1rnds4(abcd,e1,3);
		msg2 = __builtin_ia32_sha1msg1(msg2,msg3);
		msg1^=msg3; 

		// Rounds 64-67
		e0 = __builtin_ia32_sha1nexte(e0,msg0);
		e1 = abcd;
		msg1 = __builtin_ia32_sha1msg2(msg1,msg0);
		abcd = __builtin_ia32_sha1rnds4(abcd,e0,3);
		msg3 = __builtin_ia32_sha1msg1(msg3,msg0);
		msg2^=msg0; 

		// Rounds 68-71
		e1 = __builtin_ia32_sha1nexte(e1,msg1);
		e0=abcd;
		msg2 = __builtin_ia32_sha1msg2(msg2,msg1);
		abcd = __builtin_ia32_sha1rnds4(abcd,e1,3);
		msg3^=msg1; 


		// Rounds 72-75
		e0 = __builtin_ia32_sha1nexte(e0,msg2);
		e1 = abcd;
		msg3 = __builtin_ia32_sha1msg2(msg3,msg2);
		abcd = __builtin_ia32_sha1rnds4(abcd,e0,3);

		// Rounds 76-79
		e1 = __builtin_ia32_sha1nexte(e1,msg3);
		e0 = abcd;
		abcd = __builtin_ia32_sha1rnds4(abcd,e1,3);

		// Add values back to state
		e0 = ( estate = __builtin_ia32_sha1nexte(e0, estate));
		abcdstate.wrap_add_assign(abcd);
		abcd = abcdstate;
	}
	simd_vector<int,4>{abcd[3],abcd[2],abcd[1],abcd[0]}.store(state);
	state[4]=static_cast<std::uint_least32_t>(e0.back());
#elif defined(FAST_IO_ARM_SHA) && ( defined(__arm__) || defined(__aarch32__) || defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM) )
	sha1_arm_function(state,blocks_start,blocks_bytes);
#else
	sha1_do_constexpr_function(state,blocks_start,blocks_bytes);
#endif
#elif (defined(_MSC_VER)&&!defined(__clang__)) && (defined(__AVX__) || (defined(__SSE_4_2__)&&defined(__SHA__)) )
//https://stackoverflow.com/questions/21107350/how-can-i-access-sha-intrinsic
	__m128i ABCD, ABCD_SAVE, E0, E0_SAVE, E1;
	__m128i MASK, MSG0, MSG1, MSG2, MSG3;
//Clearly it is a violation of strict aliasing rule.
//However, the library will mark them as mayalias. We can safely use them.

	// Load initial values
	ABCD = _mm_loadu_si128((__m128i*) state);
	E0 = _mm_set_epi32(state[4], 0, 0, 0);
	ABCD = _mm_shuffle_epi32(ABCD, 0x1B);
	MASK = _mm_set_epi64x(0x0001020304050607ULL, 0x08090a0b0c0d0e0fULL);
	for(auto block(blocks_start),ed(blocks_start+blocks_bytes);block!=ed;block+=block_size)
	{
		// Save current hash
		ABCD_SAVE = ABCD;
		E0_SAVE = E0;

		// Rounds 0-3
		MSG0 = _mm_loadu_si128((__m128i*) block);
		MSG0 = _mm_shuffle_epi8(MSG0, MASK);
		E0 = _mm_add_epi32(E0, MSG0);
		E1 = ABCD;
		ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 0);

		// Rounds 4-7
		MSG1 = _mm_loadu_si128((__m128i*) (block+16));
		MSG1 = _mm_shuffle_epi8(MSG1, MASK);
		E1 = _mm_sha1nexte_epu32(E1, MSG1);
		E0 = ABCD;
		ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 0);
		MSG0 = _mm_sha1msg1_epu32(MSG0, MSG1);

		// Rounds 8-11
		MSG2 = _mm_loadu_si128((__m128i*) (block+32));
		MSG2 = _mm_shuffle_epi8(MSG2, MASK);
		E0 = _mm_sha1nexte_epu32(E0, MSG2);
		E1 = ABCD;
		ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 0);
		MSG1 = _mm_sha1msg1_epu32(MSG1, MSG2);
		MSG0 = _mm_xor_si128(MSG0, MSG2);

		// Rounds 12-15
		MSG3 = _mm_loadu_si128((__m128i*) (block+48));
		MSG3 = _mm_shuffle_epi8(MSG3, MASK);
		E1 = _mm_sha1nexte_epu32(E1, MSG3);
		E0 = ABCD;
		MSG0 = _mm_sha1msg2_epu32(MSG0, MSG3);
		ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 0);
		MSG2 = _mm_sha1msg1_epu32(MSG2, MSG3);
		MSG1 = _mm_xor_si128(MSG1, MSG3);

		// Rounds 16-19
		E0 = _mm_sha1nexte_epu32(E0, MSG0);
		E1 = ABCD;
		MSG1 = _mm_sha1msg2_epu32(MSG1, MSG0);
		ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 0);
		MSG3 = _mm_sha1msg1_epu32(MSG3, MSG0);
		MSG2 = _mm_xor_si128(MSG2, MSG0);

		// Rounds 20-23
		E1 = _mm_sha1nexte_epu32(E1, MSG1);
		E0 = ABCD;
		MSG2 = _mm_sha1msg2_epu32(MSG2, MSG1);
		ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 1);
		MSG0 = _mm_sha1msg1_epu32(MSG0, MSG1);
		MSG3 = _mm_xor_si128(MSG3, MSG1);

		// Rounds 24-27
		E0 = _mm_sha1nexte_epu32(E0, MSG2);
		E1 = ABCD;
		MSG3 = _mm_sha1msg2_epu32(MSG3, MSG2);
		ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 1);
		MSG1 = _mm_sha1msg1_epu32(MSG1, MSG2);
		MSG0 = _mm_xor_si128(MSG0, MSG2);

		// Rounds 28-31
		E1 = _mm_sha1nexte_epu32(E1, MSG3);
		E0 = ABCD;
		MSG0 = _mm_sha1msg2_epu32(MSG0, MSG3);
		ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 1);
		MSG2 = _mm_sha1msg1_epu32(MSG2, MSG3);
		MSG1 = _mm_xor_si128(MSG1, MSG3);

		// Rounds 32-35
		E0 = _mm_sha1nexte_epu32(E0, MSG0);
		E1 = ABCD;
		MSG1 = _mm_sha1msg2_epu32(MSG1, MSG0);
		ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 1);
		MSG3 = _mm_sha1msg1_epu32(MSG3, MSG0);
		MSG2 = _mm_xor_si128(MSG2, MSG0);

		// Rounds 36-39
		E1 = _mm_sha1nexte_epu32(E1, MSG1);
		E0 = ABCD;
		MSG2 = _mm_sha1msg2_epu32(MSG2, MSG1);
		ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 1);
		MSG0 = _mm_sha1msg1_epu32(MSG0, MSG1);
		MSG3 = _mm_xor_si128(MSG3, MSG1);

		// Rounds 40-43
		E0 = _mm_sha1nexte_epu32(E0, MSG2);
		E1 = ABCD;
		MSG3 = _mm_sha1msg2_epu32(MSG3, MSG2);
		ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 2);
		MSG1 = _mm_sha1msg1_epu32(MSG1, MSG2);
		MSG0 = _mm_xor_si128(MSG0, MSG2);

		// Rounds 44-47
		E1 = _mm_sha1nexte_epu32(E1, MSG3);
		E0 = ABCD;
		MSG0 = _mm_sha1msg2_epu32(MSG0, MSG3);
		ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 2);
		MSG2 = _mm_sha1msg1_epu32(MSG2, MSG3);
		MSG1 = _mm_xor_si128(MSG1, MSG3);

		// Rounds 48-51
		E0 = _mm_sha1nexte_epu32(E0, MSG0);
		E1 = ABCD;
		MSG1 = _mm_sha1msg2_epu32(MSG1, MSG0);
		ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 2);
		MSG3 = _mm_sha1msg1_epu32(MSG3, MSG0);
		MSG2 = _mm_xor_si128(MSG2, MSG0);

		// Rounds 52-55
		E1 = _mm_sha1nexte_epu32(E1, MSG1);
		E0 = ABCD;
		MSG2 = _mm_sha1msg2_epu32(MSG2, MSG1);
		ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 2);
		MSG0 = _mm_sha1msg1_epu32(MSG0, MSG1);
		MSG3 = _mm_xor_si128(MSG3, MSG1);

		// Rounds 56-59
		E0 = _mm_sha1nexte_epu32(E0, MSG2);
		E1 = ABCD;
		MSG3 = _mm_sha1msg2_epu32(MSG3, MSG2);
		ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 2);
		MSG1 = _mm_sha1msg1_epu32(MSG1, MSG2);
		MSG0 = _mm_xor_si128(MSG0, MSG2);

		// Rounds 60-63
		E1 = _mm_sha1nexte_epu32(E1, MSG3);
		E0 = ABCD;
		MSG0 = _mm_sha1msg2_epu32(MSG0, MSG3);
		ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 3);
		MSG2 = _mm_sha1msg1_epu32(MSG2, MSG3);
		MSG1 = _mm_xor_si128(MSG1, MSG3);

		// Rounds 64-67
		E0 = _mm_sha1nexte_epu32(E0, MSG0);
		E1 = ABCD;
		MSG1 = _mm_sha1msg2_epu32(MSG1, MSG0);
		ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 3);
		MSG3 = _mm_sha1msg1_epu32(MSG3, MSG0);
		MSG2 = _mm_xor_si128(MSG2, MSG0);

		// Rounds 68-71
		E1 = _mm_sha1nexte_epu32(E1, MSG1);
		E0 = ABCD;
		MSG2 = _mm_sha1msg2_epu32(MSG2, MSG1);
		ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 3);
		MSG3 = _mm_xor_si128(MSG3, MSG1);

		// Rounds 72-75
		E0 = _mm_sha1nexte_epu32(E0, MSG2);
		E1 = ABCD;
		MSG3 = _mm_sha1msg2_epu32(MSG3, MSG2);
		ABCD = _mm_sha1rnds4_epu32(ABCD, E0, 3);

		// Rounds 76-79
		E1 = _mm_sha1nexte_epu32(E1, MSG3);
		E0 = ABCD;
		ABCD = _mm_sha1rnds4_epu32(ABCD, E1, 3);
		// Add values back to state
		E0 = _mm_sha1nexte_epu32(E0, E0_SAVE);
		ABCD = _mm_add_epi32(ABCD, ABCD_SAVE);
	}
	// Save state
	ABCD = _mm_shuffle_epi32(ABCD, 0x1B);
	_mm_storeu_si128((__m128i*) state, ABCD);
	state[4] = _mm_extract_epi32(E0, 3);
#elif defined(FAST_IO_ARM_SHA) && ( defined(__arm__) || defined(__aarch32__) || defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM) )
	sha1_arm_function(state,blocks_start,blocks_bytes);
#else
	sha1_do_constexpr_function(state,blocks_start,blocks_bytes);
#endif
}

class sha1
{
public:
	using state_value_type = std::uint_least32_t;
	static inline constexpr std::size_t block_size{64};
	static inline constexpr std::endian hash_endian{std::endian::big};
	static inline constexpr std::size_t state_size{5};
	state_value_type state[state_size];
#if __cpp_lib_is_constant_evaluated >= 201811L
	constexpr
#endif
	void update_blocks(std::byte const* __restrict blocks_start,std::byte const* __restrict blocks_last) noexcept
	{
		std::size_t const blocks_bytes{static_cast<std::size_t>(blocks_last-blocks_start)};
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
		if consteval
#else
		if(__builtin_is_constant_evaluated())
#endif
		{
			::fast_io::details::sha1::sha1_do_constexpr_function(this->state,blocks_start,blocks_bytes);
		}
		else
#endif
		{
			::fast_io::details::sha1::sha1_do_function(this->state,blocks_start,blocks_bytes);
		}
	}
};

struct sha1_initializer
{
	static inline constexpr std::size_t digest_size{20};
	static inline constexpr ::fast_io::details::sha1::sha1 initialize_value{{0x67452301,0xefcdab89,0x98badcfe,0x10325476,0xc3d2e1f0}};
	static inline constexpr void digest_to_byte_ptr(std::uint_least32_t const* digest,std::byte* ptr) noexcept
	{
		hash_digest_to_byte_ptr_common<::std::uint_least32_t,digest_size,std::endian::big>(digest,ptr);
	}
};

}

namespace fast_io
{
using sha1_context
#ifndef FAST_IO_NO_WARNING_DEPRECATED_CRYPTO_ALGOS
[[deprecated("SHA1 is no longer a secure algorithm. See wikipedia https://en.wikipedia.org/wiki/SHA-1")]]
#endif
= ::fast_io::details::basic_md5_sha_context_impl<::fast_io::details::sha1::sha1,::fast_io::details::sha1::sha1_initializer,64>;
}
