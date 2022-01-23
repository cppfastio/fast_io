#pragma once

#if defined(__AES__)
#include <wmmintrin.h>
#endif

namespace fast_io
{

namespace details::aes
{
inline __m128i aes_128_key_exp_common(__m128i key) noexcept {
	key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
	key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
	key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
	return key;
}

inline __m128i aes_128_key_expansion(__m128i key, __m128i keygened) noexcept {
	keygened = _mm_shuffle_epi32(keygened, _MM_SHUFFLE(3, 3, 3, 3));
	key = aes_128_key_exp_common(key);
	return _mm_xor_si128(key, keygened);
}

inline __m128i aes_128_key_exp(__m128i k, int rcon) noexcept {
	return aes_128_key_expansion(k, _mm_aeskeygenassist_si128(k, rcon));
}

inline __m128i aes_192_key_exp_2(__m128i key, __m128i key2) noexcept {
	key = _mm_shuffle_epi32(key, 0xff);
	key2 = _mm_xor_si128(key2, _mm_slli_si128(key2, 4));
	return _mm_xor_si128(key, key2);
}
inline __m128i aes_192_key_exp(__m128i key, __m128i key2, int I) noexcept {
	return _mm_xor_si128(aes_128_key_exp_common(key), _mm_shuffle_epi32(_mm_aeskeygenassist_si128(key2, I), 0x55));
}

inline __m128i aes_256_key_exp(__m128i key, __m128i key2, int I) noexcept {
	return _mm_xor_si128(aes_128_key_exp_common(key), _mm_shuffle_epi32(_mm_aeskeygenassist_si128(key2, I), 0xff));
}

inline __m128i aes_256_key_exp_2(__m128i key, __m128i key2) noexcept {
	return _mm_xor_si128(aes_128_key_exp_common(key), _mm_shuffle_epi32(_mm_aeskeygenassist_si128(key2, 0x00), 0xff));
}

}

template<std::size_t keysize,bool decrypt=false>
requires (keysize==16||keysize==24||keysize==32)
struct aes
{
	inline static constexpr std::size_t block_size = 16;
	inline static constexpr std::size_t key_size = keysize;
	inline static constexpr std::size_t key_schedule_size = keysize==16?10:(keysize==24?12:15);
	__m128i key_schedule[key_schedule_size];
	explicit aes(std::span<std::byte const,key_size> key_span) noexcept
	{
		using namespace details::aes;
		std::byte const* key{key_span.data()};
		if constexpr(keysize==16)
		{
			key_schedule[0]  = _mm_loadu_si128(reinterpret_cast<__m128i const*>(key));
			key_schedule[1]  = aes_128_key_exp(key_schedule[0], 0x01);
			key_schedule[2]  = aes_128_key_exp(key_schedule[1], 0x02);
			key_schedule[3]  = aes_128_key_exp(key_schedule[2], 0x04);
			key_schedule[4]  = aes_128_key_exp(key_schedule[3], 0x08);
			key_schedule[5]  = aes_128_key_exp(key_schedule[4], 0x10);
			key_schedule[6]  = aes_128_key_exp(key_schedule[5], 0x20);
			key_schedule[7]  = aes_128_key_exp(key_schedule[6], 0x40);
			key_schedule[8]  = aes_128_key_exp(key_schedule[7], 0x80);
			key_schedule[9]  = aes_128_key_exp(key_schedule[8], 0x1B);
			key_schedule[10] = aes_128_key_exp(key_schedule[9], 0x36);
		}
		else if constexpr(keysize==24)
		{
			key_schedule[0] = _mm_loadu_si128(reinterpret_cast<__m128i const*>(key)); // load 16 bytes
			key_schedule[1] = _mm_loadu_si128(reinterpret_cast<__m128i const*>(key + 8)); // load 8 bytes
			key_schedule[1] = _mm_srli_si128(key_schedule[1], 8); // right shift 64 bits
			__m128i temp[2];
			temp[0] = aes_192_key_exp(key_schedule[0], key_schedule[1], 0x01);
			temp[1] = aes_192_key_exp_2(temp[0], key_schedule[1]);
			key_schedule[1] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(key_schedule[1]), reinterpret_cast<__m128d>(temp[0]), 0));
			key_schedule[2] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(temp[0]), reinterpret_cast<__m128d>(temp[1]), 1));
			key_schedule[3] = aes_192_key_exp(temp[0], temp[1], 0x02);
			key_schedule[4] = aes_192_key_exp_2(key_schedule[3], temp[1]);
			temp[0] = aes_192_key_exp(key_schedule[3], key_schedule[4], 0x04);
			temp[1] = aes_192_key_exp_2(temp[0], key_schedule[4]);
			key_schedule[4] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(key_schedule[4]), reinterpret_cast<__m128d>(temp[0]), 0));
			key_schedule[5] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(temp[0]), reinterpret_cast<__m128d>(temp[1]), 1));
			key_schedule[6] = aes_192_key_exp(temp[0], temp[1], 0x08);
			key_schedule[7] = aes_192_key_exp_2(key_schedule[6], temp[1]);
			temp[0] = aes_192_key_exp(key_schedule[6], key_schedule[7], 0x10);
			temp[1] = aes_192_key_exp_2(temp[0], key_schedule[7]);
			key_schedule[7] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(key_schedule[7]), reinterpret_cast<__m128d>(temp[0]), 0));
			key_schedule[8] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(temp[0]), reinterpret_cast<__m128d>(temp[1]), 1));
			key_schedule[9] = aes_192_key_exp(temp[0], temp[1], 0x20);
			key_schedule[10] = aes_192_key_exp_2(key_schedule[9], temp[1]);
			temp[0] = aes_192_key_exp(key_schedule[9], key_schedule[10], 0x40);
			temp[1] = aes_192_key_exp_2(temp[0], key_schedule[10]);
			key_schedule[10] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(key_schedule[10]), reinterpret_cast<__m128d>(temp[0]), 0));
			key_schedule[11] = reinterpret_cast<__m128i>(_mm_shuffle_pd(reinterpret_cast<__m128d>(temp[0]),reinterpret_cast<__m128d>(temp[1]), 1));
			key_schedule[12] = aes_192_key_exp(temp[0], temp[1], 0x80);
			secure_clear(temp,sizeof(temp));
		}
		else
		{
			key_schedule[0] = _mm_loadu_si128(reinterpret_cast<__m128i const*>(key));
			key_schedule[1] = _mm_loadu_si128(reinterpret_cast<__m128i const*>(key + 16));
			key_schedule[2] = aes_256_key_exp(key_schedule[0], key_schedule[1], 0x01);
			key_schedule[3] = aes_256_key_exp_2(key_schedule[1], key_schedule[2]);
			key_schedule[4] = aes_256_key_exp(key_schedule[2], key_schedule[3], 0x02);
			key_schedule[5] = aes_256_key_exp_2(key_schedule[3], key_schedule[4]);
			key_schedule[6] = aes_256_key_exp(key_schedule[4], key_schedule[5], 0x04);
			key_schedule[7] = aes_256_key_exp_2(key_schedule[5], key_schedule[6]);
			key_schedule[8] = aes_256_key_exp(key_schedule[6], key_schedule[7], 0x08);
			key_schedule[9] = aes_256_key_exp_2(key_schedule[7], key_schedule[8]);
			key_schedule[10] = aes_256_key_exp(key_schedule[8], key_schedule[9], 0x10);
			key_schedule[11] = aes_256_key_exp_2(key_schedule[9], key_schedule[10]);
			key_schedule[12] = aes_256_key_exp(key_schedule[10], key_schedule[11], 0x20);
			key_schedule[13] = aes_256_key_exp_2(key_schedule[11], key_schedule[12]);
			key_schedule[14] = aes_256_key_exp(key_schedule[12], key_schedule[13], 0x40);
		}
	}
	void operator()(std::byte const* from,std::size_t blocks,std::byte* to) noexcept
	{

	}
};

}