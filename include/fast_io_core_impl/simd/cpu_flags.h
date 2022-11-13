#pragma once

namespace fast_io::details::cpu_flags
{

inline constexpr bool sse2_supported
{
#if defined(__SSE2__) || (defined(_MSC_VER)&&!defined(__clang__)&&(((defined(__x86_64__) || defined(_M_AMD64))&&!defined(_KERNEL_MODE))||defined(__AVX__)))
true
#elif defined(_M_IX86_FP) && defined(_MSC_VER)&&!defined(__clang__)&&!defined(_KERNEL_MODE)
#if _M_IX86_FP == 2
true
#endif
#endif
};

inline constexpr bool sse4_2_supported
{
#if defined(__AVX__) || defined(__SSE4_2__)
true
#endif
};

inline constexpr bool sse3_supported
{
#if defined(__AVX__) || defined(__SSE3__) || defined(__SSE4_2__)
true
#endif
};

inline constexpr bool avx_supported
{
#if defined(__AVX__) || defined(__AVX2__)
true
#endif
};

inline constexpr bool avx2_supported
{
#if defined(__AVX2__)
true
#endif
};

inline constexpr bool avx512bw_supported
{
#if defined(__AVX512BW__)
true
#endif
};

inline constexpr bool avx512vl_supported
{
#if defined(__AVX512VL__)
true
#endif
};

inline constexpr bool avx512dq_supported
{
#if defined(__AVX512DQ__)
true
#endif
};

inline constexpr bool avx512f_supported
{
#if defined(__AVX512F__)
true
#endif
};

inline constexpr bool wasmsimd128_supported
{
#if defined(__wasm_simd128__)
true
#endif
};

inline constexpr bool armneon_supported
{
#if defined(__ARM_NEON) || ((defined(_MSC_VER)&&!defined(__clang__)) && defined(_M_ARM64) &&!defined(_KERNEL_MODE))
true
#endif
};

}
