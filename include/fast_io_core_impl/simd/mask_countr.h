#pragma once

namespace fast_io
{

namespace details
{

template<bool ctzero,unsigned pos,typename T>
requires (std::same_as<typename ::std::remove_cvref_t<T>::value_type,std::uint_least64_t>)
inline constexpr unsigned vector_mask_countr_recursive_impl(T const& v2) noexcept
{
	constexpr std::uint_least64_t mx{std::numeric_limits<std::uint_least64_t>::max()};
	constexpr unsigned digits{std::numeric_limits<std::uint_least64_t>::digits};
	constexpr unsigned digitspos{digits*pos};
	constexpr std::size_t N{sizeof(T)/sizeof(std::uint_least64_t)};
	static_assert(N!=0);
	std::uint_least64_t element{v2[pos]};
	if constexpr(pos!=N-1)
	{
		if constexpr(ctzero)
		{
			if(!element)
			{
				return vector_mask_countr_recursive_impl<ctzero,pos+1>(v2);
			}
		}
		else
		{
			if(element==mx)
			{
				return vector_mask_countr_recursive_impl<ctzero,pos+1>(v2);
			}
		}
	}
	if constexpr(pos)
	{
		if constexpr(ctzero)
		{

			return digitspos+static_cast<unsigned>(std::countr_zero(element));
		}
		else
		{
			return digitspos+static_cast<unsigned>(std::countr_one(element));
		}
	}
	else
	{
		if constexpr(ctzero)
		{
			return static_cast<unsigned>(std::countr_zero(element));
		}
		else
		{
			return static_cast<unsigned>(std::countr_one(element));
		}
	}
}

template<bool ctzero,std::integral T,std::size_t n>
inline constexpr unsigned vector_mask_countr_common_no_intrinsics_impl(::fast_io::intrinsics::simd_vector<T,n> const& vec) noexcept
{
	constexpr std::size_t N{sizeof(::fast_io::intrinsics::simd_vector<T,n>)/sizeof(std::uint_least64_t)};
	unsigned d{vector_mask_countr_recursive_impl<ctzero,0>(static_cast<::fast_io::intrinsics::simd_vector<std::uint_least64_t,N>>(vec))};
	constexpr unsigned shift{static_cast<unsigned>(std::bit_width(sizeof(T)*::std::numeric_limits<char>::digits))};
	d>>=shift;
#if __has_cpp_attribute(assume)
	[[assume(d<=n)]];
#endif
	return d;
}

inline constexpr bool calculate_can_intrinsics_accelerate_mask_countr(std::size_t sizeofsimdvector) noexcept
{
	if(sizeofsimdvector<=32)
	{
		return ::fast_io::details::calculate_can_simd_vector_run_with_cpu_instruction(sizeofsimdvector);
	}
	return false;
}


template<std::size_t sizeofsimdvector>
inline constexpr bool can_intrinsics_accelerate_mask_countr
{
calculate_can_intrinsics_accelerate_mask_countr(sizeofsimdvector)
};

template<bool ctzero,std::integral T,std::size_t n>
inline
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
constexpr
#endif
unsigned vector_mask_countr_common_intrinsics_impl(::fast_io::intrinsics::simd_vector<T,n> const& vec) noexcept
{
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L
	if (std::is_constant_evaluated())
#endif
	{
		return vector_mask_countr_common_no_intrinsics_impl<ctzero>(vec);
	}
#endif
	unsigned d{};
	if constexpr(sizeof(::fast_io::intrinsics::simd_vector<T,n>)==16)
	{
#if defined(__has_builtin) && __has_cpp_attribute(__gnu__::__vector_size__)
#if defined(__SSE2__) && defined(__x86_64__) && __has_builtin(__builtin_ia32_pmovmskb128)
		using x86_64_v16qi [[__gnu__::__vector_size__ (16)]] = char;
		std::uint_least16_t const value{static_cast<std::uint_least16_t>(__builtin_ia32_pmovmskb128((x86_64_v16qi)vec.value))};
		if constexpr(ctzero)
		{
			d=static_cast<unsigned>(std::countr_zero(value));
		}
		else
		{
			d=static_cast<unsigned>(std::countr_one(value));
		}
#elif defined(__wasm_simd128__) && __has_builtin(__builtin_wasm_bitmask_i8x16)
		using wasmsimd128_i8x16 [[__gnu__::__vector_size__ (16)]] = char;
		::std::uint_least16_t const value{static_cast<::std::uint_least16_t>(__builtin_wasm_bitmask_i8x16(static_cast<wasmsimd128_i8x16>(vec.value)))};
		if constexpr(ctzero)
		{
			d=static_cast<unsigned>(std::countr_zero(value));
		}
		else
		{
			d=static_cast<unsigned>(std::countr_one(value));
		}
#endif
#elif (defined(__x86_64__) || defined(_M_AMD64) || defined(__i386__) || defined(_M_IX86))
		if constexpr(::fast_io::details::cpu_flags::sse2_supported)
		{
			__m128i a = __builtin_bit_cast(__m128i,vec);
			std::uint_least16_t const value{static_cast<std::uint_least16_t>(_mm_movemask_epi8(a,a))};
			if constexpr(ctzero)
			{
				d=static_cast<unsigned>(std::countr_zero(value));
			}
			else
			{
				d=static_cast<unsigned>(std::countr_one(value));
			}
		}
#endif
	}
	else if constexpr(sizeof(::fast_io::intrinsics::simd_vector<T,n>)==32)
	{
#if defined(__has_builtin) && __has_cpp_attribute(__gnu__::__vector_size__)
#if defined(__AVX__) && defined(__x86_64__) && __has_builtin(__builtin_ia32_pmovmskb256)
		using x86_64_v32qi [[__gnu__::__vector_size__ (32)]] = char;
		std::uint_least32_t const value{static_cast<std::uint_least32_t>(__builtin_ia32_pmovmskb256((x86_64_v32qi)vec.value))};
		if constexpr(ctzero)
		{
			d=static_cast<unsigned>(std::countr_zero(value));
		}
		else
		{
			d=static_cast<unsigned>(std::countr_one(value));
		}
#endif
#elif (defined(__x86_64__) || defined(_M_AMD64) || defined(__i386__) || defined(_M_IX86))
		if constexpr(::fast_io::details::cpu_flags::avx2_supported)
		{
			__m256i a = __builtin_bit_cast(__m256i,vec);
			std::uint_least32_t const value{static_cast<std::uint_least32_t>(_mm256_movemask_epi8(a,a))};
			if constexpr(ctzero)
			{
				d=static_cast<unsigned>(std::countr_zero(value));
			}
			else
			{
				d=static_cast<unsigned>(std::countr_one(value));
			}
		}
#endif
	}
	if constexpr(sizeof(T)==1)
	{
		return d;
	}
	else
	{
		constexpr unsigned shift{static_cast<unsigned>(std::bit_width(sizeof(T)*::std::numeric_limits<char>::digits)-3)};
		return d>>shift;
	}
}

template<bool ctzero,std::integral T,std::size_t n>
inline constexpr unsigned vector_mask_countr_common_impl(::fast_io::intrinsics::simd_vector<T,n> const& vec) noexcept
{
	if constexpr(can_intrinsics_accelerate_mask_countr<sizeof(vec)>)
	{
		return vector_mask_countr_common_intrinsics_impl<ctzero>(vec);
	}
	else
	{
		return vector_mask_countr_common_no_intrinsics_impl<ctzero>(vec);
	}
}

}

namespace intrinsics
{

template<typename T,std::size_t n>
inline constexpr auto vector_mask_countr_one(simd_vector<T,n> const& vec) noexcept
{
	return ::fast_io::details::vector_mask_countr_common_impl<false>(vec);
}

template<typename T,std::size_t n>
inline constexpr auto vector_mask_countr_zero(simd_vector<T,n> const& vec) noexcept
{
	return ::fast_io::details::vector_mask_countr_common_impl<true>(vec);
}

}

}