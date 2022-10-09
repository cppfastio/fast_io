#pragma once

#include"scalar.h"
#if (defined(__GNUC__)  || defined(__clang__)) && (defined(__SSE__)||defined(__wasm_simd128__))
#if __has_builtin(__builtin_shufflevector)
#include"simd16.h"
#else
#include"runtime.h"
#endif
#else
#include"runtime.h"
#endif


namespace fast_io
{
#if 0
struct chacha
{
	using state_value_type = std::uint_least32_t;
	static inline constexpr std::size_t block_size{64};
	static inline constexpr std::endian cipher_endian{std::endian::little};
	static inline constexpr std::size_t state_size{16};
	state_value_type state[state_size];
#if __cpp_lib_is_constant_evaluated >= 201811L
	constexpr
#endif
	void update_block(::std::byte* __restrict outdata,
	::std::uint_least32_t const* __restrict indata) noexcept
	{
		
	}
}
#endif
}
