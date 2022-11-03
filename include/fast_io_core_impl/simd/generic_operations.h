#pragma once

#if !defined(_MSC_VER)
#include <immintrin.h>
#endif

namespace fast_io
{

namespace intrinsics
{
template<typename T,std::size_t N>
struct
#if defined(_MSC_VER)
__declspec(intrin_type) __declspec(align(sizeof(T)*N/2))
#endif
simd_vector;
}
namespace details
{

namespace cpu_flags
{

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

}

template<typename T,typename Func>
inline constexpr void generic_simd_operation_impl(T& selfval,T const& other,Func func) noexcept
{
	constexpr std::size_t N{std::remove_cvref_t<T>::size()};
	for(std::size_t i{};i!=N;++i)
	{
		func(selfval.value[i],other.value[i]);
	}
}

template<typename T,typename Func>
inline constexpr void generic_simd_self_op_impl(T& selfval,Func func) noexcept
{
	constexpr std::size_t N{std::remove_cvref_t<T>::size()};
	for(std::size_t i{};i!=N;++i)
	{
		func(selfval.value[i]);
	}
}

template<typename T,typename Func>
inline constexpr std::remove_cvref_t<T> generic_simd_self_create_op_impl(T const& selfval,Func func) noexcept
{
	std::remove_cvref_t<T> newval;
	constexpr std::size_t N{std::remove_cvref_t<T>::size()};
	for(std::size_t i{};i!=N;++i)
	{
		newval.value[i]=func(selfval.value[i]);
	}
	return newval;
}

template<typename T,typename Func>
inline constexpr std::remove_cvref_t<T> generic_simd_create_op_impl(T const& a,T const& b,Func func) noexcept
{
	constexpr std::size_t N{std::remove_cvref_t<T>::size()};
	std::remove_cvref_t<T> newval;
	for(std::size_t i{};i!=N;++i)
	{
		newval.value[i]=func(a.value[i],b.value[i]);
	}
	return newval;
}

template<typename T>
inline constexpr auto create_value_mx() noexcept
{
	::fast_io::freestanding::array<std::uint_least8_t,sizeof(T)> arr;
	for(auto& e : arr)
	{
		e=std::numeric_limits<std::uint_least8_t>::max();
	}
	return std::bit_cast<T>(arr);
}

template<typename T,typename Func>
inline constexpr T generic_simd_comparision_common_impl(T const& a,
	T const& b,Func func) noexcept
{
	using value_type = std::remove_cvref_t<T>::value_type;
	return ::fast_io::details::generic_simd_create_op_impl(a,b,[&](value_type va,value_type vb)->value_type
	{
		bool t{func(va,vb)};
		constexpr auto mx{create_value_mx<value_type>()};
		if(t)
		{
			return mx;
		}
		return {};	
	});
}

template<typename T,std::size_t N>
inline constexpr ::fast_io::intrinsics::simd_vector<T,N> all_zero_simd_vector_mask{};

template<typename T,std::size_t N>
inline constexpr ::fast_io::intrinsics::simd_vector<T,N> all_one_simd_vector_mask{create_value_mx<::fast_io::intrinsics::simd_vector<T,N>>()};

template<typename T,std::size_t N>
inline constexpr ::fast_io::intrinsics::simd_vector<T,N> wrap_add_common(::fast_io::intrinsics::simd_vector<T,N> const& a,::fast_io::intrinsics::simd_vector<T,N> const& b) noexcept
{
#if defined(__x86_64__) || defined(_M_X64)
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
		using vec_type = ::fast_io::intrinsics::simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16)
		{
			if constexpr(std::integral<T>)
			{
				__m128i amm = __builtin_bit_cast(__m128i,a);
				__m128i bmm = __builtin_bit_cast(__m128i,b);
				if constexpr(sizeof(T)==1)
				{
					return __builtin_bit_cast(vec_type,_mm_add_epi8(amm,bmm));
				}
				else if constexpr(sizeof(T)==2)
				{
					return __builtin_bit_cast(vec_type,_mm_add_epi16(amm,bmm));
				}
				else if constexpr(sizeof(T)==4)
				{
					return __builtin_bit_cast(vec_type,_mm_add_epi32(amm,bmm));
				}
				else if constexpr(sizeof(T)==8)
				{
					return __builtin_bit_cast(vec_type,_mm_add_epi64(amm,bmm));
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m128h amm = __builtin_bit_cast(__m128h,a);
					__m128h bmm = __builtin_bit_cast(__m128h,b);
					return __builtin_bit_cast(vec_type,_mm_add_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m128 amm = __builtin_bit_cast(__m128,a);
					__m128 bmm = __builtin_bit_cast(__m128,b);
					return __builtin_bit_cast(vec_type,_mm_add_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m128d amm = __builtin_bit_cast(__m128d,a);
					__m128d bmm = __builtin_bit_cast(__m128d,b);
					return __builtin_bit_cast(vec_type,_mm_add_pd(amm,bmm));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==32&&::fast_io::details::cpu_flags::avx2_supported)
		{
			if constexpr(std::integral<T>)
			{
				__m256i amm = __builtin_bit_cast(__m256i,a);
				__m256i bmm = __builtin_bit_cast(__m256i,b);
				if constexpr(sizeof(T)==1)
				{
					return __builtin_bit_cast(vec_type,_mm256_add_epi8(amm,bmm));
				}
				else if constexpr(sizeof(T)==2)
				{
					return __builtin_bit_cast(vec_type,_mm256_add_epi16(amm,bmm));
				}
				else if constexpr(sizeof(T)==4)
				{
					return __builtin_bit_cast(vec_type,_mm256_add_epi32(amm,bmm));
				}
				else if constexpr(sizeof(T)==8)
				{
					return __builtin_bit_cast(vec_type,_mm256_add_epi64(amm,bmm));
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m256h amm = __builtin_bit_cast(__m256h,a);
					__m256h bmm = __builtin_bit_cast(__m256h,b);
					return __builtin_bit_cast(vec_type,_mm256_add_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m256 amm = __builtin_bit_cast(__m256,a);
					__m256 bmm = __builtin_bit_cast(__m256,b);
					return __builtin_bit_cast(vec_type,_mm256_add_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m256d amm = __builtin_bit_cast(__m256d,a);
					__m256d bmm = __builtin_bit_cast(__m256d,b);
					return __builtin_bit_cast(vec_type,_mm256_add_pd(amm,bmm));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==64)
		{
			if constexpr(std::integral<T>&&::fast_io::details::cpu_flags::avx512bw_supported)
			{
				__m512i amm = __builtin_bit_cast(__m512i,a);
				__m512i bmm = __builtin_bit_cast(__m512i,b);
				if constexpr(sizeof(T)==1)
				{
					return __builtin_bit_cast(vec_type,_mm512_add_epi8(amm,bmm));
				}
				else if constexpr(sizeof(T)==2)
				{
					return __builtin_bit_cast(vec_type,_mm512_add_epi16(amm,bmm));
				}
				else if constexpr(sizeof(T)==4)
				{
					return __builtin_bit_cast(vec_type,_mm512_add_epi32(amm,bmm));
				}
				else if constexpr(sizeof(T)==8)
				{
					return __builtin_bit_cast(vec_type,_mm512_add_epi64(amm,bmm));
				}
			}
			else if constexpr(std::floating_point<T>&&::fast_io::details::cpu_flags::avx512f_supported)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m512h amm = __builtin_bit_cast(__m512h,a);
					__m512h bmm = __builtin_bit_cast(__m512h,b);
					return __builtin_bit_cast(vec_type,_mm512_add_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m512 amm = __builtin_bit_cast(__m512,a);
					__m512 bmm = __builtin_bit_cast(__m512,b);
					return __builtin_bit_cast(vec_type,_mm512_add_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m512d amm = __builtin_bit_cast(__m512d,a);
					__m512d bmm = __builtin_bit_cast(__m512d,b);
					return __builtin_bit_cast(vec_type,_mm512_add_pd(amm,bmm));
				}
			}
		}
	}
#endif
	return ::fast_io::details::generic_simd_create_op_impl(a,b,[](T va,T vb) noexcept -> T
	{
		if constexpr(std::signed_integral<T>)
		{
			using unsignedtype = std::make_unsigned_t<T>;
			return static_cast<T>(static_cast<unsignedtype>(va)+static_cast<unsignedtype>(vb));
		}
		else
		{
			return static_cast<T>(va+vb);
		}
	});
}

template<typename T,std::size_t N>
inline constexpr ::fast_io::intrinsics::simd_vector<T,N> wrap_minus_common(::fast_io::intrinsics::simd_vector<T,N> const& a,::fast_io::intrinsics::simd_vector<T,N> const& b) noexcept
{
#if defined(__x86_64__) || defined(_M_X64)
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
		using vec_type = ::fast_io::intrinsics::simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16)
		{
			if constexpr(std::integral<T>)
			{
				__m128i amm = __builtin_bit_cast(__m128i,a);
				__m128i bmm = __builtin_bit_cast(__m128i,b);
				if constexpr(sizeof(T)==1)
				{
					return __builtin_bit_cast(vec_type,_mm_sub_epi8(amm,bmm));
				}
				else if constexpr(sizeof(T)==2)
				{
					return __builtin_bit_cast(vec_type,_mm_sub_epi16(amm,bmm));
				}
				else if constexpr(sizeof(T)==4)
				{
					return __builtin_bit_cast(vec_type,_mm_sub_epi32(amm,bmm));
				}
				else if constexpr(sizeof(T)==8)
				{
					return __builtin_bit_cast(vec_type,_mm_sub_epi64(amm,bmm));
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m128h amm = __builtin_bit_cast(__m128h,a);
					__m128h bmm = __builtin_bit_cast(__m128h,b);
					return __builtin_bit_cast(vec_type,_mm_sub_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m128 amm = __builtin_bit_cast(__m128,a);
					__m128 bmm = __builtin_bit_cast(__m128,b);
					return __builtin_bit_cast(vec_type,_mm_sub_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m128d amm = __builtin_bit_cast(__m128d,a);
					__m128d bmm = __builtin_bit_cast(__m128d,b);
					return __builtin_bit_cast(vec_type,_mm_sub_pd(amm,bmm));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==32&&::fast_io::details::cpu_flags::avx2_supported)
		{
			if constexpr(std::integral<T>)
			{
				__m256i amm = __builtin_bit_cast(__m256i,a);
				__m256i bmm = __builtin_bit_cast(__m256i,b);
				if constexpr(sizeof(T)==1)
				{
					return __builtin_bit_cast(vec_type,_mm256_sub_epi8(amm,bmm));
				}
				else if constexpr(sizeof(T)==2)
				{
					return __builtin_bit_cast(vec_type,_mm256_sub_epi16(amm,bmm));
				}
				else if constexpr(sizeof(T)==4)
				{
					return __builtin_bit_cast(vec_type,_mm256_sub_epi32(amm,bmm));
				}
				else if constexpr(sizeof(T)==8)
				{
					return __builtin_bit_cast(vec_type,_mm256_sub_epi64(amm,bmm));
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m256h amm = __builtin_bit_cast(__m256h,a);
					__m256h bmm = __builtin_bit_cast(__m256h,b);
					return __builtin_bit_cast(vec_type,_mm256_sub_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m256 amm = __builtin_bit_cast(__m256,a);
					__m256 bmm = __builtin_bit_cast(__m256,b);
					return __builtin_bit_cast(vec_type,_mm256_sub_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m256d amm = __builtin_bit_cast(__m256d,a);
					__m256d bmm = __builtin_bit_cast(__m256d,b);
					return __builtin_bit_cast(vec_type,_mm256_sub_pd(amm,bmm));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==64)
		{
			if constexpr(std::integral<T>&&::fast_io::details::cpu_flags::avx512bw_supported)
			{
				__m512i amm = __builtin_bit_cast(__m512i,a);
				__m512i bmm = __builtin_bit_cast(__m512i,b);
				if constexpr(sizeof(T)==1)
				{
					return __builtin_bit_cast(vec_type,_mm512_sub_epi8(amm,bmm));
				}
				else if constexpr(sizeof(T)==2)
				{
					return __builtin_bit_cast(vec_type,_mm512_sub_epi16(amm,bmm));
				}
				else if constexpr(sizeof(T)==4)
				{
					return __builtin_bit_cast(vec_type,_mm512_sub_epi32(amm,bmm));
				}
				else if constexpr(sizeof(T)==8)
				{
					return __builtin_bit_cast(vec_type,_mm512_sub_epi64(amm,bmm));
				}
			}
			else if constexpr(std::floating_point<T>&&::fast_io::details::cpu_flags::avx512f_supported)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m512h amm = __builtin_bit_cast(__m512h,a);
					__m512h bmm = __builtin_bit_cast(__m512h,b);
					return __builtin_bit_cast(vec_type,_mm512_sub_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m512 amm = __builtin_bit_cast(__m512,a);
					__m512 bmm = __builtin_bit_cast(__m512,b);
					return __builtin_bit_cast(vec_type,_mm512_sub_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m512d amm = __builtin_bit_cast(__m512d,a);
					__m512d bmm = __builtin_bit_cast(__m512d,b);
					return __builtin_bit_cast(vec_type,_mm512_sub_pd(amm,bmm));
				}
			}
		}
	}
#endif
	return ::fast_io::details::generic_simd_create_op_impl(a,b,[](T va,T vb) noexcept -> T
	{
		if constexpr(std::signed_integral<T>)
		{
			using unsignedtype = std::make_unsigned_t<T>;
			return static_cast<T>(static_cast<unsignedtype>(va)-static_cast<unsignedtype>(vb));
		}
		else
		{
			return static_cast<T>(va-vb);
		}
	});
}

template<std::size_t sizeoft,
	::std::size_t N>
requires (sizeoft*N<
	::std::numeric_limits<char unsigned>::max()&&(sizeoft<=sizeof(std::uint_least64_t)))
inline constexpr auto create_byte_swap_mask() noexcept
{
	using T=
	std::conditional_t<sizeoft==8,std::uint_least64_t,
	std::conditional_t<sizeoft==4,std::uint_least32_t,
	std::conditional_t<sizeoft==2,std::uint_least16_t,
	std::uint_least8_t>>>;
	constexpr std::size_t totaln{sizeoft*N};
	constexpr char unsigned resn{totaln};
	::fast_io::freestanding::array<char unsigned,totaln> v1;
	for(char unsigned i{};i!=resn;++i)
	{
		v1[i]=i;
	}
	auto v2=__builtin_bit_cast(::fast_io::freestanding::array<T,N>,v1);
	for(auto & e : v2)
	{
		e=::fast_io::byte_swap(e);
	}
	return __builtin_bit_cast(::fast_io::intrinsics::simd_vector<char unsigned,totaln>,v2);
}
template<std::size_t sizeoft,
	::std::size_t N>
inline constexpr auto simd_byte_swap_shuffle_mask{create_byte_swap_mask<sizeoft,N>()};

}

namespace intrinsics
{

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator+(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
	return ::fast_io::details::wrap_add_common(a,b);
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> wrap_add(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
	return ::fast_io::details::wrap_add_common(a,b);
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator-(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
	return ::fast_io::details::wrap_minus_common(a,b);
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> wrap_minus(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
	return ::fast_io::details::wrap_minus_common(a,b);
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator*(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
#if defined(__x86_64__) || defined(_M_X64)
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
		using vec_type = simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16)
		{
			if constexpr(std::integral<T>)
			{
				__m128i amm = __builtin_bit_cast(__m128i,a);
				__m128i bmm = __builtin_bit_cast(__m128i,b);
				if constexpr(std::signed_integral<T>)
				{
					if constexpr(sizeof(T)==4)
					{
						return __builtin_bit_cast(vec_type,_mm_mul_epi32(amm,bmm));
					}
				}
				else
				{
					if constexpr(sizeof(T)==4)
					{
						return __builtin_bit_cast(vec_type,_mm_mul_epu32(amm,bmm));
					}
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m128h amm = __builtin_bit_cast(__m128h,a);
					__m128h bmm = __builtin_bit_cast(__m128h,b);
					return __builtin_bit_cast(vec_type,_mm_mul_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m128 amm = __builtin_bit_cast(__m128,a);
					__m128 bmm = __builtin_bit_cast(__m128,b);
					return __builtin_bit_cast(vec_type,_mm_mul_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m128d amm = __builtin_bit_cast(__m128d,a);
					__m128d bmm = __builtin_bit_cast(__m128d,b);
					return __builtin_bit_cast(vec_type,_mm_mul_pd(amm,bmm));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==32&&::fast_io::details::cpu_flags::avx2_supported)
		{
			if constexpr(std::integral<T>)
			{
				__m256i amm = __builtin_bit_cast(__m256i,a);
				__m256i bmm = __builtin_bit_cast(__m256i,b);
				if constexpr(std::signed_integral<T>)
				{
					if constexpr(sizeof(T)==4)
					{
						return __builtin_bit_cast(vec_type,_mm256_mul_epi32(amm,bmm));
					}
				}
				else
				{
					if constexpr(sizeof(T)==4)
					{
						return __builtin_bit_cast(vec_type,_mm256_mul_epu32(amm,bmm));
					}
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m256h amm = __builtin_bit_cast(__m256h,a);
					__m256h bmm = __builtin_bit_cast(__m256h,b);
					return __builtin_bit_cast(vec_type,_mm256_mul_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m256 amm = __builtin_bit_cast(__m256,a);
					__m256 bmm = __builtin_bit_cast(__m256,b);
					return __builtin_bit_cast(vec_type,_mm256_mul_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m256d amm = __builtin_bit_cast(__m256d,a);
					__m256d bmm = __builtin_bit_cast(__m256d,b);
					return __builtin_bit_cast(vec_type,_mm256_mul_pd(amm,bmm));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==64)
		{
			if constexpr(std::integral<T>&&::fast_io::details::cpu_flags::avx512bw_supported)
			{
				__m512i amm = __builtin_bit_cast(__m512i,a);
				__m512i bmm = __builtin_bit_cast(__m512i,b);
				if constexpr(std::signed_integral<T>)
				{
					if constexpr(sizeof(T)==4)
					{
						return __builtin_bit_cast(vec_type,_mm512_mul_epi32(amm,bmm));
					}
				}
				else
				{
					if constexpr(sizeof(T)==4)
					{
						return __builtin_bit_cast(vec_type,_mm512_mul_epu32(amm,bmm));
					}
				}
			}
			else if constexpr(std::floating_point<T>&&::fast_io::details::cpu_flags::avx512f_supported)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m512h amm = __builtin_bit_cast(__m512h,a);
					__m512h bmm = __builtin_bit_cast(__m512h,b);
					return __builtin_bit_cast(vec_type,_mm512_mul_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m512 amm = __builtin_bit_cast(__m512,a);
					__m512 bmm = __builtin_bit_cast(__m512,b);
					return __builtin_bit_cast(vec_type,_mm512_mul_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m512d amm = __builtin_bit_cast(__m512d,a);
					__m512d bmm = __builtin_bit_cast(__m512d,b);
					return __builtin_bit_cast(vec_type,_mm512_mul_pd(amm,bmm));
				}
			}
		}
	}
#endif
	return ::fast_io::details::generic_simd_create_op_impl(a,b,[](T va,T vb)
	{
		return va*vb;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator/(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
#if defined(__x86_64__) || defined(_M_X64)
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
		using vec_type = simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16)
		{
			if constexpr(std::integral<T>)
			{
#if __has_cpp_attribute(__gnu__::__vector_size__)
				using vec2_type [[__gnu__::__vector_size__ (N*sizeof(T))]] = T;
				vec2_type amm = __builtin_bit_cast(vec2_type,a);
				vec2_type bmm = __builtin_bit_cast(vec2_type,b);
				return __builtin_bit_cast(vec_type,(a/b));
#else
				__m128i amm = __builtin_bit_cast(__m128i,a);
				__m128i bmm = __builtin_bit_cast(__m128i,b);
				if constexpr(sizeof(T)==1)
				{
					return __builtin_bit_cast(vec_type,_mm_div_epi8(amm,bmm));
				}
				else if constexpr(sizeof(T)==2)
				{
					return __builtin_bit_cast(vec_type,_mm_div_epi16(amm,bmm));
				}
				else if constexpr(sizeof(T)==4)
				{
					return __builtin_bit_cast(vec_type,_mm_div_epi32(amm,bmm));
				}
				else if constexpr(sizeof(T)==8)
				{
					return __builtin_bit_cast(vec_type,_mm_div_epi64(amm,bmm));
				}
#endif
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m128h amm = __builtin_bit_cast(__m128h,a);
					__m128h bmm = __builtin_bit_cast(__m128h,b);
					return __builtin_bit_cast(vec_type,_mm_div_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m128 amm = __builtin_bit_cast(__m128,a);
					__m128 bmm = __builtin_bit_cast(__m128,b);
					return __builtin_bit_cast(vec_type,_mm_div_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m128d amm = __builtin_bit_cast(__m128d,a);
					__m128d bmm = __builtin_bit_cast(__m128d,b);
					return __builtin_bit_cast(vec_type,_mm_div_pd(amm,bmm));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==32&&::fast_io::details::cpu_flags::avx2_supported)
		{
			if constexpr(std::integral<T>)
			{
#if __has_cpp_attribute(__gnu__::__vector_size__)
				using vec2_type [[__gnu__::__vector_size__ (N*sizeof(T))]] = T;
				vec2_type amm = __builtin_bit_cast(vec2_type,a);
				vec2_type bmm = __builtin_bit_cast(vec2_type,b);
				return __builtin_bit_cast(vec_type,(a/b));
#else
				__m256i amm = __builtin_bit_cast(__m256i,a);
				__m256i bmm = __builtin_bit_cast(__m256i,b);
				if constexpr(sizeof(T)==1)
				{
					return __builtin_bit_cast(vec_type,_mm256_div_epi8(amm,bmm));
				}
				else if constexpr(sizeof(T)==2)
				{
					return __builtin_bit_cast(vec_type,_mm256_div_epi16(amm,bmm));
				}
				else if constexpr(sizeof(T)==4)
				{
					return __builtin_bit_cast(vec_type,_mm256_div_epi32(amm,bmm));
				}
				else if constexpr(sizeof(T)==8)
				{
					return __builtin_bit_cast(vec_type,_mm256_div_epi64(amm,bmm));
				}
#endif
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m256h amm = __builtin_bit_cast(__m256h,a);
					__m256h bmm = __builtin_bit_cast(__m256h,b);
					return __builtin_bit_cast(vec_type,_mm256_div_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m256 amm = __builtin_bit_cast(__m256,a);
					__m256 bmm = __builtin_bit_cast(__m256,b);
					return __builtin_bit_cast(vec_type,_mm256_div_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m256d amm = __builtin_bit_cast(__m256d,a);
					__m256d bmm = __builtin_bit_cast(__m256d,b);
					return __builtin_bit_cast(vec_type,_mm256_div_pd(amm,bmm));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==64)
		{
			if constexpr(std::integral<T>&&::fast_io::details::cpu_flags::avx512bw_supported)
			{
#if __has_cpp_attribute(__gnu__::__vector_size__)
				using vec2_type [[__gnu__::__vector_size__ (N*sizeof(T))]] = T;
				vec2_type amm = __builtin_bit_cast(vec2_type,a);
				vec2_type bmm = __builtin_bit_cast(vec2_type,b);
				return __builtin_bit_cast(vec_type,(a/b));
#else
				__m512i amm = __builtin_bit_cast(__m512i,a);
				__m512i bmm = __builtin_bit_cast(__m512i,b);
				if constexpr(sizeof(T)==1)
				{
					return __builtin_bit_cast(vec_type,_mm512_div_epi8(amm,bmm));
				}
				else if constexpr(sizeof(T)==2)
				{
					return __builtin_bit_cast(vec_type,_mm512_div_epi16(amm,bmm));
				}
				else if constexpr(sizeof(T)==4)
				{
					return __builtin_bit_cast(vec_type,_mm512_div_epi32(amm,bmm));
				}
				else if constexpr(sizeof(T)==8)
				{
					return __builtin_bit_cast(vec_type,_mm512_div_epi64(amm,bmm));
				}
#endif
			}
			else if constexpr(std::floating_point<T>&&::fast_io::details::cpu_flags::avx512f_supported)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m512h amm = __builtin_bit_cast(__m512h,a);
					__m512h bmm = __builtin_bit_cast(__m512h,b);
					return __builtin_bit_cast(vec_type,_mm512_div_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m512 amm = __builtin_bit_cast(__m512,a);
					__m512 bmm = __builtin_bit_cast(__m512,b);
					return __builtin_bit_cast(vec_type,_mm512_div_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m512d amm = __builtin_bit_cast(__m512d,a);
					__m512d bmm = __builtin_bit_cast(__m512d,b);
					return __builtin_bit_cast(vec_type,_mm512_div_pd(amm,bmm));
				}
			}
		}
	}
#endif
	return ::fast_io::details::generic_simd_create_op_impl(a,b,[](T va,T vb)
	{
		return va/vb;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator&(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
#if defined(__x86_64__) || defined(_M_X64)
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
		using vec_type = simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16)
		{
			__m128 amm = __builtin_bit_cast(__m128,a);
			__m128 bmm = __builtin_bit_cast(__m128,b);
			return __builtin_bit_cast(vec_type,_mm_and_ps(amm,bmm));
		}
		else if constexpr(sizeof(vec_type)==32&&::fast_io::details::cpu_flags::avx_supported)
		{
			__m256 amm = __builtin_bit_cast(__m256,a);
			__m256 bmm = __builtin_bit_cast(__m256,b);
			return __builtin_bit_cast(vec_type,_mm256_and_ps(amm,bmm));
		}
		else if constexpr(sizeof(vec_type)==64)
		{
			if constexpr(::fast_io::details::cpu_flags::avx512dq_supported)
			{
				__m512 amm = __builtin_bit_cast(__m512,a);
				__m512 bmm = __builtin_bit_cast(__m512,b);
				return __builtin_bit_cast(vec_type,_mm512_and_ps(amm,bmm));
			}
			else if constexpr(::fast_io::details::cpu_flags::avx512f_supported)
			{
				__m512i amm = __builtin_bit_cast(__m512i,a);
				__m512i bmm = __builtin_bit_cast(__m512i,b);
				return __builtin_bit_cast(vec_type,_mm512_and_epi32(amm,bmm));
			}
		}
	}
#endif
	return ::fast_io::details::generic_simd_create_op_impl(a,b,[](T va,T vb)
	{
		return va&vb;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator|(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
#if defined(__x86_64__) || defined(_M_X64)
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
		using vec_type = simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16)
		{
			__m128 amm = __builtin_bit_cast(__m128,a);
			__m128 bmm = __builtin_bit_cast(__m128,b);
			return __builtin_bit_cast(vec_type,_mm_or_ps(amm,bmm));
		}
		else if constexpr(sizeof(vec_type)==32&&::fast_io::details::cpu_flags::avx_supported)
		{
			__m256 amm = __builtin_bit_cast(__m256,a);
			__m256 bmm = __builtin_bit_cast(__m256,b);
			return __builtin_bit_cast(vec_type,_mm256_or_ps(amm,bmm));
		}
		else if constexpr(sizeof(vec_type)==64)
		{
			if constexpr(::fast_io::details::cpu_flags::avx512dq_supported)
			{
				__m512 amm = __builtin_bit_cast(__m512,a);
				__m512 bmm = __builtin_bit_cast(__m512,b);
				return __builtin_bit_cast(vec_type,_mm512_or_ps(amm,bmm));
			}
			else if constexpr(::fast_io::details::cpu_flags::avx512f_supported)
			{
				__m512i amm = __builtin_bit_cast(__m512i,a);
				__m512i bmm = __builtin_bit_cast(__m512i,b);
				return __builtin_bit_cast(vec_type,_mm512_or_epi32(amm,bmm));
			}
		}
	}
#endif
	return ::fast_io::details::generic_simd_create_op_impl(a,b,[](T va,T vb)
	{
		return va|vb;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator^(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
#if defined(__x86_64__) || defined(_M_X64)
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
		using vec_type = simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16)
		{
			__m128 amm = __builtin_bit_cast(__m128,a);
			__m128 bmm = __builtin_bit_cast(__m128,b);
			return __builtin_bit_cast(vec_type,_mm_xor_ps(amm,bmm));
		}
		else if constexpr(sizeof(vec_type)==32&&::fast_io::details::cpu_flags::avx_supported)
		{
			__m256 amm = __builtin_bit_cast(__m256,a);
			__m256 bmm = __builtin_bit_cast(__m256,b);
			return __builtin_bit_cast(vec_type,_mm256_xor_ps(amm,bmm));
		}
		else if constexpr(sizeof(vec_type)==64)
		{
			if constexpr(::fast_io::details::cpu_flags::avx512dq_supported)
			{
				__m512 amm = __builtin_bit_cast(__m512,a);
				__m512 bmm = __builtin_bit_cast(__m512,b);
				return __builtin_bit_cast(vec_type,_mm512_xor_ps(amm,bmm));
			}
			else if constexpr(::fast_io::details::cpu_flags::avx512f_supported)
			{
				__m512i amm = __builtin_bit_cast(__m512i,a);
				__m512i bmm = __builtin_bit_cast(__m512i,b);
				return __builtin_bit_cast(vec_type,_mm512_xor_epi32(amm,bmm));
			}
		}
	}
#endif
	return ::fast_io::details::generic_simd_create_op_impl(a,b,[](T va,T vb)
	{
		return va^vb;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator<<(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
	return ::fast_io::details::generic_simd_create_op_impl(a,b,[](T va,T vb)
	{
		return va<<vb;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator>>(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
	return ::fast_io::details::generic_simd_create_op_impl(a,b,[](T va,T vb)
	{
		return va>>vb;
	});
}

template<typename T,std::size_t N,std::integral I>
inline constexpr simd_vector<T,N> operator<<(simd_vector<T,N> const& a,I i) noexcept
{
	return ::fast_io::details::generic_simd_self_create_op_impl(a,[i](T va)
	{
		return va<<i;
	});
}

template<typename T,std::size_t N,std::integral I>
inline constexpr simd_vector<T,N> operator>>(simd_vector<T,N> const& a,I i) noexcept
{
	return ::fast_io::details::generic_simd_self_create_op_impl(a,[i](T va)
	{
		return va>>i;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator<(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
#if defined(__x86_64__) || defined(_M_X64)
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
		using vec_type = ::fast_io::intrinsics::simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16)
		{
			if constexpr(std::integral<T>)
			{
				if constexpr(::fast_io::details::cpu_flags::avx512vl_supported&&
				::fast_io::details::cpu_flags::avx512bw_supported)
				{
					__m128i amm = __builtin_bit_cast(__m128i,a);
					__m128i bmm = __builtin_bit_cast(__m128i,b);
					if constexpr(std::signed_integral<T>)
					{
						if constexpr(sizeof(T)==1)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi8(_mm_cmplt_epi8_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==2)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi16(_mm_cmplt_epi16_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==4)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi32(_mm_cmplt_epi32_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==8)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi64(_mm_cmplt_epi64_mask(amm,bmm)));
						}
					}
					else
					{
						if constexpr(sizeof(T)==1)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi8(_mm_cmplt_epu8_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==2)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi16(_mm_cmplt_epu16_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==4)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi32(_mm_cmplt_epu32_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==8)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi64(_mm_cmplt_epu64_mask(amm,bmm)));
						}
					}
				}
				else if constexpr(sizeof(T)<=4)
				{
					__m128i amm = __builtin_bit_cast(__m128i,a);
					__m128i bmm = __builtin_bit_cast(__m128i,b);
					if constexpr(std::unsigned_integral<T>)
					{
						__m128i mask;
						if constexpr(sizeof(T)==1)
						{
							constexpr auto mn{INT_LEAST8_MIN};
							mask=_mm_set1_epi8(INT_LEAST8_MIN);
						}
						else if constexpr(sizeof(T)==2)
						{
							constexpr auto mn{INT_LEAST16_MIN};
							mask=_mm_set1_epi16(mn);
						}
						else if constexpr(sizeof(T)==4)
						{
							constexpr auto mn{INT_LEAST32_MIN};
							mask=_mm_set1_epi32(mn);
						}
						amm=_mm_xor_si128(amm,mask);
						bmm=_mm_xor_si128(bmm,mask);
					}
					if constexpr(sizeof(T)==1)
					{
						return __builtin_bit_cast(vec_type,_mm_cmplt_epi8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						return __builtin_bit_cast(vec_type,_mm_cmplt_epi16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						return __builtin_bit_cast(vec_type,_mm_cmplt_epi32(amm,bmm));
					}
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float))
				{
					__m256 amm = __builtin_bit_cast(__m256,a);
					__m256 bmm = __builtin_bit_cast(__m256,b);
					return __builtin_bit_cast(vec_type,_mm256_cmp_ps(amm,bmm,_CMP_LT_OS));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m256d amm = __builtin_bit_cast(__m256d,a);
					__m256d bmm = __builtin_bit_cast(__m256d,b);
					return __builtin_bit_cast(vec_type,_mm256_cmp_pd(amm,bmm,_CMP_LT_OS));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==32&&::fast_io::details::cpu_flags::avx2_supported)
		{
			if constexpr(std::integral<T>)
			{
				if constexpr(::fast_io::details::cpu_flags::avx512vl_supported&&
				::fast_io::details::cpu_flags::avx512bw_supported)
				{
					__m256i amm = __builtin_bit_cast(__m256i,a);
					__m256i bmm = __builtin_bit_cast(__m256i,b);
					if constexpr(std::signed_integral<T>)
					{
						if constexpr(sizeof(T)==1)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi8(_mm256_cmplt_epi8_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==2)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi16(_mm256_cmplt_epi16_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==4)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi32(_mm256_cmplt_epi32_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==8)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi64(_mm256_cmplt_epi64_mask(amm,bmm)));
						}
					}
					else
					{
						if constexpr(sizeof(T)==1)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi8(_mm256_cmplt_epu8_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==2)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi16(_mm256_cmplt_epu16_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==4)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi32(_mm256_cmplt_epu32_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==8)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi64(_mm256_cmplt_epu64_mask(amm,bmm)));
						}
					}
				}
				else if constexpr(sizeof(T)<=4)
				{
					__m256i amm = __builtin_bit_cast(__m256i,a);
					__m256i bmm = __builtin_bit_cast(__m256i,b);
					if constexpr(std::unsigned_integral<T>)
					{
						__m256i mask;
						if constexpr(sizeof(T)==1)
						{
							constexpr auto mn{INT_LEAST8_MIN};
							mask=_mm256_set1_epi8(INT_LEAST8_MIN);
						}
						else if constexpr(sizeof(T)==2)
						{
							constexpr auto mn{INT_LEAST16_MIN};
							mask=_mm256_set1_epi16(mn);
						}
						else if constexpr(sizeof(T)==4)
						{
							constexpr auto mn{INT_LEAST32_MIN};
							mask=_mm256_set1_epi32(mn);
						}
						amm=_mm256_xor_si256(amm,mask);
						bmm=_mm256_xor_si256(bmm,mask);
					}
					__m128i alow = _mm256_castsi256_si128(amm);
					__m128i blow = _mm256_castsi256_si128(bmm);
					__m128i ahigh = _mm256_extractf128_si256(amm,1);
					__m128i bhigh = _mm256_extractf128_si256(bmm,1);

					if constexpr(sizeof(T)==1)
					{
						alow = _mm_cmplt_epi8(alow,blow);
						ahigh = _mm_cmplt_epi8(ahigh,blow);
					}
					else if constexpr(sizeof(T)==2)
					{
						alow = _mm_cmplt_epi16(alow,blow);
						ahigh = _mm_cmplt_epi16(ahigh,blow);
					}
					else if constexpr(sizeof(T)==4)
					{
						alow = _mm_cmplt_epi32(alow,blow);
						ahigh = _mm_cmplt_epi32(ahigh,blow);
					}
					__m256i res = _mm256_castsi128_si256(alow);
					res = _mm256_insertf128_si256(res,ahigh,1);
					return __builtin_bit_cast(vec_type,res);
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float))
				{
					__m256 amm = __builtin_bit_cast(__m256,a);
					__m256 bmm = __builtin_bit_cast(__m256,b);
					return __builtin_bit_cast(vec_type,_mm256_cmp_ps(amm,bmm,_CMP_LT_OS));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m256d amm = __builtin_bit_cast(__m256d,a);
					__m256d bmm = __builtin_bit_cast(__m256d,b);
					return __builtin_bit_cast(vec_type,_mm256_cmp_pd(amm,bmm,_CMP_LT_OS));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==64)
		{
			if constexpr(std::integral<T>&&::fast_io::details::cpu_flags::avx512bw_supported)
			{
				__m512i amm = __builtin_bit_cast(__m512i,a);
				__m512i bmm = __builtin_bit_cast(__m512i,b);
				if constexpr(std::signed_integral<T>)
				{
					if constexpr(sizeof(T)==1)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi8(_mm512_cmplt_epi8_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==2)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi16(_mm512_cmplt_epi16_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==4)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi32(_mm512_cmplt_epi32_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==8)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi64(_mm512_cmplt_epi64_mask(amm,bmm)));
					}
				}
				else
				{
					if constexpr(sizeof(T)==1)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi8(_mm512_cmplt_epu8_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==2)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi16(_mm512_cmplt_epu16_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==4)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi32(_mm512_cmplt_epu32_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==8)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi64(_mm512_cmplt_epu64_mask(amm,bmm)));
					}
				}
			}
			else if constexpr(std::floating_point<T>&&::fast_io::details::cpu_flags::avx512f_supported)
			{
				if constexpr(sizeof(T)==sizeof(float))
				{
					__m512 amm = __builtin_bit_cast(__m512,a);
					__m512 bmm = __builtin_bit_cast(__m512,b);
					return __builtin_bit_cast(vec_type,_mm512_movm_epi32(_mm512_cmplt_ps_mask(amm,bmm)));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m512d amm = __builtin_bit_cast(__m512d,a);
					__m512d bmm = __builtin_bit_cast(__m512d,b);
					return __builtin_bit_cast(vec_type,_mm512_movm_epi64(_mm512_cmplt_pd_mask(amm,bmm)));
				}
			}
		}
	}
#endif
	return ::fast_io::details::generic_simd_comparision_common_impl(a,b,[](T va,T vb) noexcept
	{
		return va<vb;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator>(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
#if defined(__x86_64__) || defined(_M_X64)
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
		using vec_type = ::fast_io::intrinsics::simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16)
		{
			if constexpr(std::integral<T>)
			{
				if constexpr(::fast_io::details::cpu_flags::avx512vl_supported&&
				::fast_io::details::cpu_flags::avx512bw_supported)
				{
					__m128i amm = __builtin_bit_cast(__m128i,a);
					__m128i bmm = __builtin_bit_cast(__m128i,b);
					if constexpr(std::signed_integral<T>)
					{
						if constexpr(sizeof(T)==1)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi8(_mm_cmpgt_epi8_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==2)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi16(_mm_cmpgt_epi16_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==4)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi32(_mm_cmpgt_epi32_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==8)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi64(_mm_cmpgt_epi64_mask(amm,bmm)));
						}
					}
					else
					{
						if constexpr(sizeof(T)==1)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi8(_mm_cmpgt_epu8_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==2)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi16(_mm_cmpgt_epu16_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==4)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi32(_mm_cmpgt_epu32_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==8)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi64(_mm_cmpgt_epu64_mask(amm,bmm)));
						}
					}
				}
				else if constexpr(sizeof(T)<=4)
				{
					__m128i amm = __builtin_bit_cast(__m128i,a);
					__m128i bmm = __builtin_bit_cast(__m128i,b);
					if constexpr(std::unsigned_integral<T>)
					{
						__m128i mask;
						if constexpr(sizeof(T)==1)
						{
							constexpr auto mn{INT_LEAST8_MIN};
							mask=_mm_set1_epi8(INT_LEAST8_MIN);
						}
						else if constexpr(sizeof(T)==2)
						{
							constexpr auto mn{INT_LEAST16_MIN};
							mask=_mm_set1_epi16(mn);
						}
						else if constexpr(sizeof(T)==4)
						{
							constexpr auto mn{INT_LEAST32_MIN};
							mask=_mm_set1_epi32(mn);
						}
						amm=_mm_xor_si128(amm,mask);
						bmm=_mm_xor_si128(bmm,mask);
					}
					if constexpr(sizeof(T)==1)
					{
						return __builtin_bit_cast(vec_type,_mm_cmpgt_epi8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						return __builtin_bit_cast(vec_type,_mm_cmpgt_epi16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						return __builtin_bit_cast(vec_type,_mm_cmpgt_epi32(amm,bmm));
					}
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float))
				{
					__m256 amm = __builtin_bit_cast(__m256,a);
					__m256 bmm = __builtin_bit_cast(__m256,b);
					return __builtin_bit_cast(vec_type,_mm256_cmp_ps(amm,bmm,_CMP_GT_OS));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m256d amm = __builtin_bit_cast(__m256d,a);
					__m256d bmm = __builtin_bit_cast(__m256d,b);
					return __builtin_bit_cast(vec_type,_mm256_cmp_pd(amm,bmm,_CMP_GT_OS));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==32&&::fast_io::details::cpu_flags::avx2_supported)
		{
			if constexpr(std::integral<T>)
			{
				if constexpr(::fast_io::details::cpu_flags::avx512vl_supported&&
				::fast_io::details::cpu_flags::avx512bw_supported)
				{
					__m256i amm = __builtin_bit_cast(__m256i,a);
					__m256i bmm = __builtin_bit_cast(__m256i,b);
					if constexpr(std::signed_integral<T>)
					{
						if constexpr(sizeof(T)==1)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi8(_mm256_cmpgt_epi8_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==2)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi16(_mm256_cmpgt_epi16_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==4)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi32(_mm256_cmpgt_epi32_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==8)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi64(_mm256_cmpgt_epi64_mask(amm,bmm)));
						}
					}
					else
					{
						if constexpr(sizeof(T)==1)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi8(_mm256_cmpgt_epu8_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==2)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi16(_mm256_cmpgt_epu16_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==4)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi32(_mm256_cmpgt_epu32_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==8)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi64(_mm256_cmpgt_epu64_mask(amm,bmm)));
						}
					}
				}
				else if constexpr(sizeof(T)<=4)
				{
					__m256i amm = __builtin_bit_cast(__m256i,a);
					__m256i bmm = __builtin_bit_cast(__m256i,b);
					if constexpr(std::unsigned_integral<T>)
					{
						__m256i mask;
						if constexpr(sizeof(T)==1)
						{
							constexpr auto mn{INT_LEAST8_MIN};
							mask=_mm256_set1_epi8(INT_LEAST8_MIN);
						}
						else if constexpr(sizeof(T)==2)
						{
							constexpr auto mn{INT_LEAST16_MIN};
							mask=_mm256_set1_epi16(mn);
						}
						else if constexpr(sizeof(T)==4)
						{
							constexpr auto mn{INT_LEAST32_MIN};
							mask=_mm256_set1_epi32(mn);
						}
						amm=_mm256_xor_si256(amm,mask);
						bmm=_mm256_xor_si256(bmm,mask);
					}
					__m128i alow = _mm256_castsi256_si128(amm);
					__m128i blow = _mm256_castsi256_si128(bmm);
					__m128i ahigh = _mm256_extractf128_si256(amm,1);
					__m128i bhigh = _mm256_extractf128_si256(bmm,1);

					if constexpr(sizeof(T)==1)
					{
						alow = _mm_cmpgt_epi8(alow,blow);
						ahigh = _mm_cmpgt_epi8(ahigh,blow);
					}
					else if constexpr(sizeof(T)==2)
					{
						alow = _mm_cmpgt_epi16(alow,blow);
						ahigh = _mm_cmpgt_epi16(ahigh,blow);
					}
					else if constexpr(sizeof(T)==4)
					{
						alow = _mm_cmpgt_epi32(alow,blow);
						ahigh = _mm_cmpgt_epi32(ahigh,blow);
					}
					__m256i res = _mm256_castsi128_si256(alow);
					res = _mm256_insertf128_si256(res,ahigh,1);
					return __builtin_bit_cast(vec_type,res);
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float))
				{
					__m256 amm = __builtin_bit_cast(__m256,a);
					__m256 bmm = __builtin_bit_cast(__m256,b);
					return __builtin_bit_cast(vec_type,_mm256_cmp_ps(amm,bmm,_CMP_GT_OS));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m256d amm = __builtin_bit_cast(__m256d,a);
					__m256d bmm = __builtin_bit_cast(__m256d,b);
					return __builtin_bit_cast(vec_type,_mm256_cmp_pd(amm,bmm,_CMP_GT_OS));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==64)
		{
			if constexpr(std::integral<T>&&::fast_io::details::cpu_flags::avx512bw_supported)
			{
				__m512i amm = __builtin_bit_cast(__m512i,a);
				__m512i bmm = __builtin_bit_cast(__m512i,b);
				if constexpr(std::signed_integral<T>)
				{
					if constexpr(sizeof(T)==1)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi8(_mm512_cmpgt_epi8_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==2)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi16(_mm512_cmpgt_epi16_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==4)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi32(_mm512_cmpgt_epi32_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==8)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi64(_mm512_cmpgt_epi64_mask(amm,bmm)));
					}
				}
				else
				{
					if constexpr(sizeof(T)==1)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi8(_mm512_cmpgt_epu8_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==2)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi16(_mm512_cmpgt_epu16_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==4)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi32(_mm512_cmpgt_epu32_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==8)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi64(_mm512_cmpgt_epu64_mask(amm,bmm)));
					}
				}
			}
			else if constexpr(std::floating_point<T>&&::fast_io::details::cpu_flags::avx512f_supported)
			{
				if constexpr(sizeof(T)==sizeof(float))
				{
					__m512 amm = __builtin_bit_cast(__m512,a);
					__m512 bmm = __builtin_bit_cast(__m512,b);
					return __builtin_bit_cast(vec_type,_mm512_movm_epi32(_mm512_cmp_ps_mask(amm,bmm,_CMP_GT_OS)));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m512d amm = __builtin_bit_cast(__m512d,a);
					__m512d bmm = __builtin_bit_cast(__m512d,b);
					return __builtin_bit_cast(vec_type,_mm512_movm_epi64(_mm512_cmp_pd_mask(amm,bmm,_CMP_GT_OS)));
				}
			}
		}
	}
#endif
	return ::fast_io::details::generic_simd_comparision_common_impl(a,b,[](T va,T vb) noexcept
	{
		return va>vb;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator<=(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
#if defined(__x86_64__) || defined(_M_X64)
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
		using vec_type = ::fast_io::intrinsics::simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16)
		{
			if constexpr(std::integral<T>)
			{
				if constexpr(::fast_io::details::cpu_flags::avx512vl_supported&&
				::fast_io::details::cpu_flags::avx512bw_supported)
				{
					__m128i amm = __builtin_bit_cast(__m128i,a);
					__m128i bmm = __builtin_bit_cast(__m128i,b);
					if constexpr(std::signed_integral<T>)
					{
						if constexpr(sizeof(T)==1)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi8(_mm_cmple_epi8_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==2)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi16(_mm_cmple_epi16_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==4)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi32(_mm_cmple_epi32_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==8)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi64(_mm_cmple_epi64_mask(amm,bmm)));
						}
					}
					else
					{
						if constexpr(sizeof(T)==1)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi8(_mm_cmple_epu8_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==2)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi16(_mm_cmple_epu16_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==4)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi32(_mm_cmple_epu32_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==8)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi64(_mm_cmple_epu64_mask(amm,bmm)));
						}
					}
				}
				else if constexpr(sizeof(T)<=4)
				{
					return b>a;
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float))
				{
					__m256 amm = __builtin_bit_cast(__m256,a);
					__m256 bmm = __builtin_bit_cast(__m256,b);
					return __builtin_bit_cast(vec_type,_mm256_cmp_ps(amm,bmm,_CMP_LE_OS));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m256d amm = __builtin_bit_cast(__m256d,a);
					__m256d bmm = __builtin_bit_cast(__m256d,b);
					return __builtin_bit_cast(vec_type,_mm256_cmp_pd(amm,bmm,_CMP_LE_OS));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==32&&::fast_io::details::cpu_flags::avx2_supported)
		{
			if constexpr(std::integral<T>)
			{
				if constexpr(::fast_io::details::cpu_flags::avx512vl_supported&&
				::fast_io::details::cpu_flags::avx512bw_supported)
				{
					__m256i amm = __builtin_bit_cast(__m256i,a);
					__m256i bmm = __builtin_bit_cast(__m256i,b);
					if constexpr(std::signed_integral<T>)
					{
						if constexpr(sizeof(T)==1)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi8(_mm256_cmple_epi8_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==2)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi16(_mm256_cmple_epi16_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==4)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi32(_mm256_cmple_epi32_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==8)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi64(_mm256_cmple_epi64_mask(amm,bmm)));
						}
					}
					else
					{
						if constexpr(sizeof(T)==1)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi8(_mm256_cmple_epu8_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==2)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi16(_mm256_cmple_epu16_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==4)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi32(_mm256_cmple_epu32_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==8)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi64(_mm256_cmple_epu64_mask(amm,bmm)));
						}
					}
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float))
				{
					__m256 amm = __builtin_bit_cast(__m256,a);
					__m256 bmm = __builtin_bit_cast(__m256,b);
					return __builtin_bit_cast(vec_type,_mm256_cmp_ps(amm,bmm,_CMP_LE_OS));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m256d amm = __builtin_bit_cast(__m256d,a);
					__m256d bmm = __builtin_bit_cast(__m256d,b);
					return __builtin_bit_cast(vec_type,_mm256_cmp_pd(amm,bmm,_CMP_LE_OS));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==64)
		{
			if constexpr(std::integral<T>&&::fast_io::details::cpu_flags::avx512bw_supported)
			{
				__m512i amm = __builtin_bit_cast(__m512i,a);
				__m512i bmm = __builtin_bit_cast(__m512i,b);
				if constexpr(std::signed_integral<T>)
				{
					if constexpr(sizeof(T)==1)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi8(_mm512_cmple_epi8_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==2)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi16(_mm512_cmple_epi16_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==4)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi32(_mm512_cmple_epi32_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==8)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi64(_mm512_cmple_epi64_mask(amm,bmm)));
					}
				}
				else
				{
					if constexpr(sizeof(T)==1)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi8(_mm512_cmple_epu8_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==2)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi16(_mm512_cmple_epu16_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==4)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi32(_mm512_cmple_epu32_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==8)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi64(_mm512_cmple_epu64_mask(amm,bmm)));
					}
				}
			}
			else if constexpr(std::floating_point<T>&&::fast_io::details::cpu_flags::avx512f_supported)
			{
				if constexpr(sizeof(T)==sizeof(float))
				{
					__m512 amm = __builtin_bit_cast(__m512,a);
					__m512 bmm = __builtin_bit_cast(__m512,b);
					return __builtin_bit_cast(vec_type,_mm512_movm_epi32(_mm512_cmp_ps_mask(amm,bmm,_CMP_LE_OS)));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m512d amm = __builtin_bit_cast(__m512d,a);
					__m512d bmm = __builtin_bit_cast(__m512d,b);
					return __builtin_bit_cast(vec_type,_mm512_movm_epi64(_mm512_cmp_pd_mask(amm,bmm,_CMP_LE_OS)));
				}
			}
		}
	}
#endif
	return ::fast_io::details::generic_simd_comparision_common_impl(a,b,[](T va,T vb) noexcept
	{
		return va<=vb;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator>=(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
#if defined(__x86_64__) || defined(_M_X64)
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
		using vec_type = ::fast_io::intrinsics::simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16)
		{
			if constexpr(std::integral<T>)
			{
				if constexpr(::fast_io::details::cpu_flags::avx512vl_supported&&
				::fast_io::details::cpu_flags::avx512bw_supported)
				{
					__m128i amm = __builtin_bit_cast(__m128i,a);
					__m128i bmm = __builtin_bit_cast(__m128i,b);
					if constexpr(std::signed_integral<T>)
					{
						if constexpr(sizeof(T)==1)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi8(_mm_cmpge_epi8_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==2)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi16(_mm_cmpge_epi16_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==4)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi32(_mm_cmpge_epi32_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==8)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi64(_mm_cmpge_epi64_mask(amm,bmm)));
						}
					}
					else
					{
						if constexpr(sizeof(T)==1)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi8(_mm_cmpge_epu8_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==2)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi16(_mm_cmpge_epu16_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==4)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi32(_mm_cmpge_epu32_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==8)
						{
							return __builtin_bit_cast(vec_type,_mm_movm_epi64(_mm_cmpge_epu64_mask(amm,bmm)));
						}
					}
				}
				else if constexpr(sizeof(T)<=4)
				{
					return b<a;
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float))
				{
					__m256 amm = __builtin_bit_cast(__m256,a);
					__m256 bmm = __builtin_bit_cast(__m256,b);
					return __builtin_bit_cast(vec_type,_mm256_cmp_ps(amm,bmm,_CMP_GE_OS));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m256d amm = __builtin_bit_cast(__m256d,a);
					__m256d bmm = __builtin_bit_cast(__m256d,b);
					return __builtin_bit_cast(vec_type,_mm256_cmp_pd(amm,bmm,_CMP_GE_OS));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==32&&::fast_io::details::cpu_flags::avx2_supported)
		{
			if constexpr(std::integral<T>)
			{
				if constexpr(::fast_io::details::cpu_flags::avx512vl_supported&&
				::fast_io::details::cpu_flags::avx512bw_supported)
				{
					__m256i amm = __builtin_bit_cast(__m256i,a);
					__m256i bmm = __builtin_bit_cast(__m256i,b);
					if constexpr(std::signed_integral<T>)
					{
						if constexpr(sizeof(T)==1)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi8(_mm256_cmpge_epi8_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==2)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi16(_mm256_cmpge_epi16_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==4)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi32(_mm256_cmpge_epi32_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==8)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi64(_mm256_cmpge_epi64_mask(amm,bmm)));
						}
					}
					else
					{
						if constexpr(sizeof(T)==1)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi8(_mm256_cmpge_epu8_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==2)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi16(_mm256_cmpge_epu16_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==4)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi32(_mm256_cmpge_epu32_mask(amm,bmm)));
						}
						else if constexpr(sizeof(T)==8)
						{
							return __builtin_bit_cast(vec_type,_mm256_movm_epi64(_mm256_cmpge_epu64_mask(amm,bmm)));
						}
					}
				}
				else if constexpr(sizeof(T)<=4)
				{
					return b<a;
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float))
				{
					__m256 amm = __builtin_bit_cast(__m256,a);
					__m256 bmm = __builtin_bit_cast(__m256,b);
					return __builtin_bit_cast(vec_type,_mm256_cmp_ps(amm,bmm,_CMP_GE_OS));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m256d amm = __builtin_bit_cast(__m256d,a);
					__m256d bmm = __builtin_bit_cast(__m256d,b);
					return __builtin_bit_cast(vec_type,_mm256_cmp_pd(amm,bmm,_CMP_GE_OS));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==64)
		{
			if constexpr(std::integral<T>&&::fast_io::details::cpu_flags::avx512bw_supported)
			{
				__m512i amm = __builtin_bit_cast(__m512i,a);
				__m512i bmm = __builtin_bit_cast(__m512i,b);
				if constexpr(std::signed_integral<T>)
				{
					if constexpr(sizeof(T)==1)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi8(_mm512_cmpge_epi8_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==2)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi16(_mm512_cmpge_epi16_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==4)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi32(_mm512_cmpge_epi32_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==8)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi64(_mm512_cmpge_epi64_mask(amm,bmm)));
					}
				}
				else
				{
					if constexpr(sizeof(T)==1)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi8(_mm512_cmpge_epu8_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==2)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi16(_mm512_cmpge_epu16_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==4)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi32(_mm512_cmpge_epu32_mask(amm,bmm)));
					}
					else if constexpr(sizeof(T)==8)
					{
						return __builtin_bit_cast(vec_type,_mm512_movm_epi64(_mm512_cmpge_epu64_mask(amm,bmm)));
					}
				}
			}
			else if constexpr(std::floating_point<T>&&::fast_io::details::cpu_flags::avx512f_supported)
			{
				if constexpr(sizeof(T)==sizeof(float))
				{
					__m512 amm = __builtin_bit_cast(__m512,a);
					__m512 bmm = __builtin_bit_cast(__m512,b);
					return __builtin_bit_cast(vec_type,_mm512_movm_epi32(_mm512_cmp_ps_mask(amm,bmm,_CMP_GE_OS)));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m512d amm = __builtin_bit_cast(__m512d,a);
					__m512d bmm = __builtin_bit_cast(__m512d,b);
					return __builtin_bit_cast(vec_type,_mm512_movm_epi64(_mm512_cmp_pd_mask(amm,bmm,_CMP_GE_OS)));
				}
			}
		}
	}
#endif
	return ::fast_io::details::generic_simd_comparision_common_impl(a,b,[](T va,T vb) noexcept
	{
		return va>=vb;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator==(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
#if defined(__x86_64__) || defined(_M_X64)
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
		using vec_type = simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16)
		{
			__m128i amm = __builtin_bit_cast(__m128i,a);
			__m128i bmm = __builtin_bit_cast(__m128i,b);
			if constexpr(sizeof(T)==1)
			{
				return __builtin_bit_cast(vec_type,_mm_cmpeq_epi8(amm,bmm));
			}
			else if constexpr(sizeof(T)==2)
			{
				return __builtin_bit_cast(vec_type,_mm_cmpeq_epi16(amm,bmm));
			}
			else if constexpr(sizeof(T)==4)
			{
				return __builtin_bit_cast(vec_type,_mm_cmpeq_epi32(amm,bmm));
			}
			else if constexpr(sizeof(T)==8)
			{
				return __builtin_bit_cast(vec_type,_mm_cmpeq_epi64(amm,bmm));
			}
		}
		else if constexpr(sizeof(vec_type)==32&&::fast_io::details::cpu_flags::avx2_supported)
		{
			__m256i amm = __builtin_bit_cast(__m256i,a);
			__m256i bmm = __builtin_bit_cast(__m256i,b);
			if constexpr(sizeof(T)==1)
			{
				return __builtin_bit_cast(vec_type,_mm256_cmpeq_epi8(amm,bmm));
			}
			else if constexpr(sizeof(T)==2)
			{
				return __builtin_bit_cast(vec_type,_mm256_cmpeq_epi16(amm,bmm));
			}
			else if constexpr(sizeof(T)==4)
			{
				return __builtin_bit_cast(vec_type,_mm256_cmpeq_epi32(amm,bmm));
			}
			else if constexpr(sizeof(T)==8)
			{
				return __builtin_bit_cast(vec_type,_mm256_cmpeq_epi64(amm,bmm));
			}
		}
		else if constexpr(sizeof(vec_type)==64&&::fast_io::details::cpu_flags::avx512f_supported)
		{
			__m512i amm = __builtin_bit_cast(__m512i,a);
			__m512i bmm = __builtin_bit_cast(__m512i,b);
			if constexpr(sizeof(T)==1)
			{
				return __builtin_bit_cast(vec_type,_mm512_movm_epi8(_mm512_cmpeq_epi8_mask(amm,bmm)));
			}
			else if constexpr(sizeof(T)==2)
			{
				return __builtin_bit_cast(vec_type,_mm512_movm_epi16(_mm512_cmpeq_epi16_mask(amm,bmm)));
			}
			else if constexpr(sizeof(T)==4)
			{
				return __builtin_bit_cast(vec_type,_mm512_movm_epi32(_mm512_cmpeq_epi32_mask(amm,bmm)));
			}
			else if constexpr(sizeof(T)==8)
			{
				return __builtin_bit_cast(vec_type,_mm512_movm_epi64(_mm512_cmpeq_epi64_mask(amm,bmm)));
			}
		}
	}
#endif
	return ::fast_io::details::generic_simd_comparision_common_impl(a,b,[](T va,T vb) noexcept
	{
		return va==vb;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator!=(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
	using vec_type = simd_vector<T,N>;
	constexpr bool using_avx512{::fast_io::details::cpu_flags::avx512f_supported&&::fast_io::details::cpu_flags::avx512bw_supported&&::fast_io::details::cpu_flags::avx512vl_supported};
	constexpr bool using_simd{sizeof(vec_type)==16||(sizeof(vec_type)==32&&::fast_io::details::cpu_flags::avx2_supported)};
	if constexpr(using_simd&&(!using_avx512))
	{
		return (::fast_io::details::all_zero_simd_vector_mask<T,N>)==(a==b);
	}
	else
	{
#if defined(__x86_64__) || defined(_M_X64)
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
		if constexpr(using_avx512)
		{
		if constexpr(sizeof(vec_type)==16)
		{
			__m128i amm = __builtin_bit_cast(__m128i,a);
			__m128i bmm = __builtin_bit_cast(__m128i,b);
			if constexpr(sizeof(T)==1)
			{
				return __builtin_bit_cast(vec_type,_mm_movm_epi8(_mm_cmpneq_epi8_mask(amm,bmm)));
			}
			else if constexpr(sizeof(T)==2)
			{
				return __builtin_bit_cast(vec_type,_mm_movm_epi16(_mm_cmpneq_epi16_mask(amm,bmm)));
			}
			else if constexpr(sizeof(T)==4)
			{
				return __builtin_bit_cast(vec_type,_mm_movm_epi32(_mm_cmpneq_epi32_mask(amm,bmm)));
			}
			else if constexpr(sizeof(T)==8)
			{
				return __builtin_bit_cast(vec_type,_mm_movm_epi64(_mm_cmpneq_epi64_mask(amm,bmm)));
			}
		}
		else if constexpr(sizeof(vec_type)==32)
		{
			__m256i amm = __builtin_bit_cast(__m256i,a);
			__m256i bmm = __builtin_bit_cast(__m256i,b);
			if constexpr(sizeof(T)==1)
			{
				return __builtin_bit_cast(vec_type,_mm256_movm_epi8(_mm256_cmpneq_epi8_mask(amm,bmm)));
			}
			else if constexpr(sizeof(T)==2)
			{
				return __builtin_bit_cast(vec_type,_mm256_movm_epi16(_mm256_cmpneq_epi16_mask(amm,bmm)));
			}
			else if constexpr(sizeof(T)==4)
			{
				return __builtin_bit_cast(vec_type,_mm256_movm_epi32(_mm256_cmpneq_epi32_mask(amm,bmm)));
			}
			else if constexpr(sizeof(T)==8)
			{
				return __builtin_bit_cast(vec_type,_mm256_movm_epi64(_mm256_cmpneq_epi64_mask(amm,bmm)));
			}
		}
		else if constexpr(sizeof(vec_type)==64)
		{
			__m512i amm = __builtin_bit_cast(__m512i,a);
			__m512i bmm = __builtin_bit_cast(__m512i,b);
			if constexpr(sizeof(T)==1)
			{
				return __builtin_bit_cast(vec_type,_mm512_movm_epi8(_mm512_cmpneq_epi8_mask(amm,bmm)));
			}
			else if constexpr(sizeof(T)==2)
			{
				return __builtin_bit_cast(vec_type,_mm512_movm_epi16(_mm512_cmpneq_epi16_mask(amm,bmm)));
			}
			else if constexpr(sizeof(T)==4)
			{
				return __builtin_bit_cast(vec_type,_mm512_movm_epi32(_mm512_cmpneq_epi32_mask(amm,bmm)));
			}
			else if constexpr(sizeof(T)==8)
			{
				return __builtin_bit_cast(vec_type,_mm512_movm_epi64(_mm512_cmpneq_epi64_mask(amm,bmm)));
			}
		}
		}
	}
#endif
		return ::fast_io::details::generic_simd_comparision_common_impl(a,b,[](T va,T vb) noexcept
		{
			return va!=vb;
		});
	}
}

}
}
