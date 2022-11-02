#pragma once

namespace fast_io
{

namespace details
{

inline constexpr bool calculate_can_simd_vector_run_with_cpu_instruction(std::size_t sizeofsimdvector) noexcept
{
	if(sizeofsimdvector==16)
	{
#if (defined(__SSE__) && defined(__x86_64__)) || defined(__wasm_simd128__)
		return true;
#endif
	}
	else if(sizeofsimdvector==32)
	{
#if defined(__AVX__) && defined(__x86_64__)
		return true;
#endif
	}
	else if(sizeofsimdvector==64)
	{
#if defined(__AVX512F__) && defined(__x86_64__)
		return true;
#endif
	}
	return false;
}

template<std::size_t sizeofsimdvector>
inline constexpr bool can_simd_vector_run_with_cpu_instruction
{
calculate_can_simd_vector_run_with_cpu_instruction(sizeofsimdvector)
};

inline constexpr std::size_t optimal_simd_vector_run_with_cpu_instruction_size
{
can_simd_vector_run_with_cpu_instruction<64>?64:
(can_simd_vector_run_with_cpu_instruction<32>?32:
(can_simd_vector_run_with_cpu_instruction<16>?16:0))
};

template<unsigned pos,typename T>
inline constexpr bool is_all_zeros_recursive_impl(T const& v2) noexcept
{
	constexpr std::uint_least64_t mx{std::numeric_limits<std::uint_least64_t>::max()};
	constexpr unsigned digits{std::numeric_limits<std::uint_least64_t>::digits};
	constexpr unsigned digitspos{digits*pos};
	constexpr std::size_t N{sizeof(T)/sizeof(std::uint_least64_t)};
	static_assert(N!=0);
	std::uint_least64_t element{v2[pos]};
	if constexpr(pos+2==N)
	{
		constexpr unsigned posp1{pos+1};
		return !element&&!v2[posp1];
	}
	else if constexpr(pos!=N-1)
	{
		return !element&&is_all_zeros_recursive_impl<pos+1>(v2);
	}
	else
	{
		return !element;
	}
}

template<std::integral T,std::size_t n>
inline
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
constexpr
#endif
bool is_all_zeros_impl(::fast_io::intrinsics::simd_vector<T,n> const& vec) noexcept
{
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L
	if (std::is_constant_evaluated())
#endif
	{
		return is_all_zeros_recursive_impl<0>(vec);
	}
#endif
	constexpr std::size_t szofvec{sizeof(::fast_io::intrinsics::simd_vector<T,n>)};

	if constexpr(sizeof(::fast_io::intrinsics::simd_vector<T,n>)==16)
	{
#if defined(__has_builtin)
#if defined(__x86_64__) && defined(__SSE4_1__) && __has_builtin(__builtin_ia32_pmovmskb128)
		using x86_64_v2di [[__gnu__::__vector_size__ (16)]] = long long;
		return __builtin_ia32_ptestz128((x86_64_v2di)vec.value,(x86_64_v2di)vec.value);
#elif defined(__x86_64__) && __has_builtin(__builtin_ia32_pmovmskb128)
		using x86_64_v16qi [[__gnu__::__vector_size__ (16)]] = char;
		return !__builtin_ia32_pmovmskb128((x86_64_v16qi)vec.value);
#elif defined(__wasm_simd128__) && __has_builtin(__builtin_wasm_bitmask_i8x16)
		using wasmsimd128_i8x16 [[__gnu__::__vector_size__ (16)]] = char;
		return !__builtin_wasm_bitmask_i8x16(static_cast<wasmsimd128_i8x16>(vec.value));
#endif
#endif
	}
	else if constexpr(sizeof(::fast_io::intrinsics::simd_vector<T,n>)==32)
	{
#if defined(__has_builtin)
#if defined(__AVX2__) && defined(__x86_64__) && __has_builtin(__builtin_ia32_ptestz256)
		using x86_64_v4di [[__gnu__::__vector_size__ (32)]] = long long;
		return __builtin_ia32_ptestz256((x86_64_v4di)vec.value,(x86_64_v4di)vec.value);
#elif defined(__AVX2__) && defined(__x86_64__) && __has_builtin(__builtin_ia32_pmovmskb256)
		using x86_64_v32qi [[__gnu__::__vector_size__ (32)]] = char;
		return !__builtin_ia32_pmovmskb256((x86_64_v32qi)vec.value);
#endif
#endif
	}
	constexpr std::size_t N{sizeof(::fast_io::intrinsics::simd_vector<T,n>)/sizeof(std::uint_least64_t)};
	return is_all_zeros_recursive_impl<0>(static_cast<::fast_io::intrinsics::simd_vector<std::uint_least64_t,N>>(vec));
}

}

namespace intrinsics
{

template<typename T,std::size_t N>
inline constexpr bool is_all_zeros(simd_vector<T,N> const& vec) noexcept
{
	return ::fast_io::details::is_all_zeros_impl(vec);
}
}

}