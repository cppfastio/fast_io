#pragma once

#if !defined(_MSC_VER) || defined(__clang__)
#if defined(__x86_64__) || defined(__i386__)
#include <immintrin.h>
#elif defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
#include <arm_neon.h>
#endif
#endif

namespace fast_io
{

namespace intrinsics
{
template<typename T,std::size_t N>
struct
#if defined(__has_declspec_attribute)
#if __has_declspec_attribute(intrin_type)
__declspec(intrin_type)
#endif
#if __has_declspec_attribute(align)
__declspec(align(sizeof(T)*N/2))
#endif
#elif defined(_MSC_VER)
__declspec(intrin_type) __declspec(align(sizeof(T)*N/2))
#endif
simd_vector;
}
namespace details
{

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
	using value_type = typename std::remove_cvref_t<T>::value_type;
	return ::fast_io::details::generic_simd_create_op_impl(a,b,[&](value_type va,value_type vb) noexcept ->value_type
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
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
#if (defined(__x86_64__) || defined(_M_AMD64) || defined(__i386__) || defined(_M_IX86))
		using vec_type = ::fast_io::intrinsics::simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16&&::fast_io::details::cpu_flags::sse2_supported)
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
#if !defined(__clang__)
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m128h amm = __builtin_bit_cast(__m128h,a);
					__m128h bmm = __builtin_bit_cast(__m128h,b);
					return __builtin_bit_cast(vec_type,_mm_add_ph(amm,bmm));
				}
				else
#endif
				if constexpr(sizeof(T)==sizeof(float))
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
#if !defined(__clang__)
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m256h amm = __builtin_bit_cast(__m256h,a);
					__m256h bmm = __builtin_bit_cast(__m256h,b);
					return __builtin_bit_cast(vec_type,_mm256_add_ph(amm,bmm));
				}
				else
#endif
				if constexpr(sizeof(T)==sizeof(float))
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
#if !defined(__clang__)
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m512h amm = __builtin_bit_cast(__m512h,a);
					__m512h bmm = __builtin_bit_cast(__m512h,b);
					return __builtin_bit_cast(vec_type,_mm512_add_ph(amm,bmm));
				}
				else
#endif
				if constexpr(sizeof(T)==sizeof(float))
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
#elif defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
		using vec_type = ::fast_io::intrinsics::simd_vector<T,N>;
		if constexpr(::fast_io::details::cpu_flags::armneon_supported)
		{
			if constexpr(std::integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						uint8x8_t bmm = __builtin_bit_cast(uint8x8_t,b);
						return __builtin_bit_cast(vec_type,vadd_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						uint16x4_t bmm = __builtin_bit_cast(uint16x4_t,b);
						return __builtin_bit_cast(vec_type,vadd_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						uint32x2_t bmm = __builtin_bit_cast(uint32x2_t,b);
						return __builtin_bit_cast(vec_type,vadd_u32(amm,bmm));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						uint64x1_t bmm = __builtin_bit_cast(uint64x1_t,b);
						return __builtin_bit_cast(vec_type,vadd_u64(amm,bmm));
					}
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						uint8x16_t bmm = __builtin_bit_cast(uint8x16_t,b);
						return __builtin_bit_cast(vec_type,vaddq_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						uint16x8_t bmm = __builtin_bit_cast(uint16x8_t,b);
						return __builtin_bit_cast(vec_type,vaddq_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						uint32x4_t bmm = __builtin_bit_cast(uint32x4_t,b);
						return __builtin_bit_cast(vec_type,vaddq_u32(amm,bmm));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						uint64x2_t bmm = __builtin_bit_cast(uint64x2_t,b);
						return __builtin_bit_cast(vec_type,vaddq_u64(amm,bmm));
					}
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==sizeof(float))
					{
						float32x2_t amm = __builtin_bit_cast(float32x2_t,a);
						float32x2_t bmm = __builtin_bit_cast(float32x2_t,b);
						return __builtin_bit_cast(vec_type,vadd_f32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==sizeof(double))
					{
						float64x1_t amm = __builtin_bit_cast(float64x1_t,a);
						float64x1_t bmm = __builtin_bit_cast(float64x1_t,b);
						return __builtin_bit_cast(vec_type,vadd_f64(amm,bmm));
					}
#endif
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==sizeof(float))
					{
						float32x4_t amm = __builtin_bit_cast(float32x4_t,a);
						float32x4_t bmm = __builtin_bit_cast(float32x4_t,b);
						return __builtin_bit_cast(vec_type,vaddq_f32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==sizeof(double))
					{
						float64x2_t amm = __builtin_bit_cast(float64x2_t,a);
						float64x2_t bmm = __builtin_bit_cast(float64x2_t,b);
						return __builtin_bit_cast(vec_type,vaddq_f64(amm,bmm));
					}
#endif
				}
			}
		}
#endif
	}
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
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
#if (defined(__x86_64__) || defined(_M_AMD64) || defined(__i386__) || defined(_M_IX86))

		using vec_type = ::fast_io::intrinsics::simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16&&::fast_io::details::cpu_flags::sse2_supported)
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
#if !defined(__clang__)
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m128h amm = __builtin_bit_cast(__m128h,a);
					__m128h bmm = __builtin_bit_cast(__m128h,b);
					return __builtin_bit_cast(vec_type,_mm_sub_ph(amm,bmm));
				}
				else
#endif
				if constexpr(sizeof(T)==sizeof(float))
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
#if !defined(__clang__)
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m256h amm = __builtin_bit_cast(__m256h,a);
					__m256h bmm = __builtin_bit_cast(__m256h,b);
					return __builtin_bit_cast(vec_type,_mm256_sub_ph(amm,bmm));
				}
				else
#endif
				if constexpr(sizeof(T)==sizeof(float))
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
#if !defined(__clang__)
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m512h amm = __builtin_bit_cast(__m512h,a);
					__m512h bmm = __builtin_bit_cast(__m512h,b);
					return __builtin_bit_cast(vec_type,_mm512_sub_ph(amm,bmm));
				}
				else
#endif
				if constexpr(sizeof(T)==sizeof(float))
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
#elif defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
		using vec_type = ::fast_io::intrinsics::simd_vector<T,N>;
		if constexpr(::fast_io::details::cpu_flags::armneon_supported)
		{
			if constexpr(std::integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						uint8x8_t bmm = __builtin_bit_cast(uint8x8_t,b);
						return __builtin_bit_cast(vec_type,vsub_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						uint16x4_t bmm = __builtin_bit_cast(uint16x4_t,b);
						return __builtin_bit_cast(vec_type,vsub_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						uint32x2_t bmm = __builtin_bit_cast(uint32x2_t,b);
						return __builtin_bit_cast(vec_type,vsub_u32(amm,bmm));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						uint64x1_t bmm = __builtin_bit_cast(uint64x1_t,b);
						return __builtin_bit_cast(vec_type,vsub_u64(amm,bmm));
					}
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						uint8x16_t bmm = __builtin_bit_cast(uint8x16_t,b);
						return __builtin_bit_cast(vec_type,vsubq_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						uint16x8_t bmm = __builtin_bit_cast(uint16x8_t,b);
						return __builtin_bit_cast(vec_type,vsubq_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						uint32x4_t bmm = __builtin_bit_cast(uint32x4_t,b);
						return __builtin_bit_cast(vec_type,vsubq_u32(amm,bmm));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						uint64x2_t bmm = __builtin_bit_cast(uint64x2_t,b);
						return __builtin_bit_cast(vec_type,vsubq_u64(amm,bmm));
					}
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==sizeof(float))
					{
						float32x2_t amm = __builtin_bit_cast(float32x2_t,a);
						float32x2_t bmm = __builtin_bit_cast(float32x2_t,b);
						return __builtin_bit_cast(vec_type,vsub_f32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==sizeof(double))
					{
						float64x1_t amm = __builtin_bit_cast(float64x1_t,a);
						float64x1_t bmm = __builtin_bit_cast(float64x1_t,b);
						return __builtin_bit_cast(vec_type,vsub_f64(amm,bmm));
					}
#endif
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==sizeof(float))
					{
						float32x4_t amm = __builtin_bit_cast(float32x4_t,a);
						float32x4_t bmm = __builtin_bit_cast(float32x4_t,b);
						return __builtin_bit_cast(vec_type,vsubq_f32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==sizeof(double))
					{
						float64x2_t amm = __builtin_bit_cast(float64x2_t,a);
						float64x2_t bmm = __builtin_bit_cast(float64x2_t,b);
						return __builtin_bit_cast(vec_type,vsubq_f32(amm,bmm));
					}
#endif
				}
			}
		}
#endif
	}
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
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
#if (defined(__x86_64__) || defined(_M_AMD64) || defined(__i386__) || defined(_M_IX86))
		using vec_type = simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16&&::fast_io::details::cpu_flags::sse2_supported)
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
#if !defined(__clang__)
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m128h amm = __builtin_bit_cast(__m128h,a);
					__m128h bmm = __builtin_bit_cast(__m128h,b);
					return __builtin_bit_cast(vec_type,_mm_mul_ph(amm,bmm));
				}
				else
#endif
				if constexpr(sizeof(T)==sizeof(float))
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
#if !defined(__clang__)
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m256h amm = __builtin_bit_cast(__m256h,a);
					__m256h bmm = __builtin_bit_cast(__m256h,b);
					return __builtin_bit_cast(vec_type,_mm256_mul_ph(amm,bmm));
				}
				else
#endif
				if constexpr(sizeof(T)==sizeof(float))
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
#if !defined(__clang__)
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m512h amm = __builtin_bit_cast(__m512h,a);
					__m512h bmm = __builtin_bit_cast(__m512h,b);
					return __builtin_bit_cast(vec_type,_mm512_mul_ph(amm,bmm));
				}
				else
#endif
				if constexpr(sizeof(T)==sizeof(float))
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
#elif defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
		using vec_type = simd_vector<T,N>;
		if constexpr(::fast_io::details::cpu_flags::armneon_supported)
		{
			if constexpr(std::unsigned_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						uint8x8_t bmm = __builtin_bit_cast(uint8x8_t,b);
						return __builtin_bit_cast(vec_type,vmul_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						uint16x4_t bmm = __builtin_bit_cast(uint16x4_t,b);
						return __builtin_bit_cast(vec_type,vmul_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						uint32x2_t bmm = __builtin_bit_cast(uint32x2_t,b);
						return __builtin_bit_cast(vec_type,vmul_u32(amm,bmm));
					}
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						uint8x16_t bmm = __builtin_bit_cast(uint8x16_t,b);
						return __builtin_bit_cast(vec_type,vmulq_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						uint16x8_t bmm = __builtin_bit_cast(uint16x8_t,b);
						return __builtin_bit_cast(vec_type,vmulq_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						uint32x4_t bmm = __builtin_bit_cast(uint32x4_t,b);
						return __builtin_bit_cast(vec_type,vmulq_u32(amm,bmm));
					}
				}
			}
			else if constexpr(std::signed_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						int8x8_t amm = __builtin_bit_cast(int8x16_t,a);
						int8x8_t bmm = __builtin_bit_cast(int8x16_t,b);
						return __builtin_bit_cast(vec_type,vmul_s8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						int16x4_t amm = __builtin_bit_cast(int16x4_t,a);
						int16x4_t bmm = __builtin_bit_cast(int16x4_t,b);
						return __builtin_bit_cast(vec_type,vmul_s16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						int32x2_t amm = __builtin_bit_cast(int32x2_t,a);
						int32x2_t bmm = __builtin_bit_cast(int32x2_t,b);
						return __builtin_bit_cast(vec_type,vmul_s32(amm,bmm));
					}
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						int8x16_t amm = __builtin_bit_cast(int8x16_t,a);
						int8x16_t bmm = __builtin_bit_cast(int8x16_t,b);
						return __builtin_bit_cast(vec_type,vmulq_s8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						int16x8_t amm = __builtin_bit_cast(int16x8_t,a);
						int16x8_t bmm = __builtin_bit_cast(int16x8_t,b);
						return __builtin_bit_cast(vec_type,vmulq_s16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						int32x4_t amm = __builtin_bit_cast(int32x4_t,a);
						int32x4_t bmm = __builtin_bit_cast(int32x4_t,b);
						return __builtin_bit_cast(vec_type,vmulq_s32(amm,bmm));
					}
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==sizeof(float))
					{
						float32x2_t amm = __builtin_bit_cast(float32x2_t,a);
						float32x2_t bmm = __builtin_bit_cast(float32x2_t,b);
						return __builtin_bit_cast(vec_type,vmul_f32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==sizeof(double))
					{
						float64x1_t amm = __builtin_bit_cast(float64x1_t,a);
						float64x1_t bmm = __builtin_bit_cast(float64x1_t,b);
						return __builtin_bit_cast(vec_type,vmul_f64(amm,bmm));
					}
#endif
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==sizeof(float))
					{
						float32x4_t amm = __builtin_bit_cast(float32x4_t,a);
						float32x4_t bmm = __builtin_bit_cast(float32x4_t,b);
						return __builtin_bit_cast(vec_type,vmulq_f32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==sizeof(double))
					{
						float64x2_t amm = __builtin_bit_cast(float64x2_t,a);
						float64x2_t bmm = __builtin_bit_cast(float64x2_t,b);
						return __builtin_bit_cast(vec_type,vmulq_f64(amm,bmm));
					}
#endif
				}
			}
		}
#endif
	}
	return ::fast_io::details::generic_simd_create_op_impl(a,b,[](T va,T vb) noexcept -> T
	{
		return va*vb;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator/(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
#if (defined(__x86_64__) || defined(_M_AMD64) || defined(__i386__) || defined(_M_IX86))
		using vec_type = simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16&&::fast_io::details::cpu_flags::sse2_supported)
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
#if !defined(__clang__)
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m128h amm = __builtin_bit_cast(__m128h,a);
					__m128h bmm = __builtin_bit_cast(__m128h,b);
					return __builtin_bit_cast(vec_type,_mm_div_ph(amm,bmm));
				}
				else
#endif
				if constexpr(sizeof(T)==sizeof(float))
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
#if !defined(__clang__)
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m256h amm = __builtin_bit_cast(__m256h,a);
					__m256h bmm = __builtin_bit_cast(__m256h,b);
					return __builtin_bit_cast(vec_type,_mm256_div_ph(amm,bmm));
				}
				else
#endif
				if constexpr(sizeof(T)==sizeof(float))
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
#if !defined(__clang__)
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m512h amm = __builtin_bit_cast(__m512h,a);
					__m512h bmm = __builtin_bit_cast(__m512h,b);
					return __builtin_bit_cast(vec_type,_mm512_div_ph(amm,bmm));
				}
				else
#endif
				if constexpr(sizeof(T)==sizeof(float))
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
#elif defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
		using vec_type = simd_vector<T,N>;
		if constexpr(::fast_io::details::cpu_flags::armneon_supported)
		{
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
			if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==sizeof(float))
					{
						float32x2_t amm = __builtin_bit_cast(float32x2_t,a);
						float32x2_t bmm = __builtin_bit_cast(float32x2_t,b);
						return __builtin_bit_cast(vec_type,vdiv_f32(amm,bmm));
					}
					else if constexpr(sizeof(T)==sizeof(double))
					{
						float64x1_t amm = __builtin_bit_cast(float64x1_t,a);
						float64x1_t bmm = __builtin_bit_cast(float64x1_t,b);
						return __builtin_bit_cast(vec_type,vdiv_f64(amm,bmm));
					}
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==sizeof(float))
					{
						float32x4_t amm = __builtin_bit_cast(float32x4_t,a);
						float32x4_t bmm = __builtin_bit_cast(float32x4_t,b);
						return __builtin_bit_cast(vec_type,vdivq_f32(amm,bmm));
					}
					else if constexpr(sizeof(T)==sizeof(double))
					{
						float64x2_t amm = __builtin_bit_cast(float64x2_t,a);
						float64x2_t bmm = __builtin_bit_cast(float64x2_t,b);
						return __builtin_bit_cast(vec_type,vdivq_f64(amm,bmm));
					}
				}
			}
#endif
		}
#endif
	}
	return ::fast_io::details::generic_simd_create_op_impl(a,b,[](T va,T vb) noexcept -> T
	{
		return va/vb;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator&(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
#if (defined(__x86_64__) || defined(_M_AMD64) || defined(__i386__) || defined(_M_IX86))
		using vec_type = simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16&&::fast_io::details::cpu_flags::sse2_supported)
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
#elif defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
		using vec_type = simd_vector<T,N>;
		if constexpr(::fast_io::details::cpu_flags::armneon_supported)
		{
			if constexpr(std::signed_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						uint8x8_t bmm = __builtin_bit_cast(uint8x8_t,b);
						return __builtin_bit_cast(vec_type,vand_s8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						uint16x4_t bmm = __builtin_bit_cast(uint16x4_t,b);
						return __builtin_bit_cast(vec_type,vand_s16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						uint32x2_t bmm = __builtin_bit_cast(uint32x2_t,b);
						return __builtin_bit_cast(vec_type,vand_s32(amm,bmm));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						uint64x1_t bmm = __builtin_bit_cast(uint64x1_t,b);
						return __builtin_bit_cast(vec_type,vand_s64(amm,bmm));
					}
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						uint8x16_t bmm = __builtin_bit_cast(uint8x16_t,b);
						return __builtin_bit_cast(vec_type,vandq_s8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						uint16x8_t bmm = __builtin_bit_cast(uint16x8_t,b);
						return __builtin_bit_cast(vec_type,vandq_s16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						uint32x4_t bmm = __builtin_bit_cast(uint32x4_t,b);
						return __builtin_bit_cast(vec_type,vandq_s32(amm,bmm));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						uint64x2_t bmm = __builtin_bit_cast(uint64x2_t,b);
						return __builtin_bit_cast(vec_type,vandq_s64(amm,bmm));
					}
				}
			}
			else if constexpr(std::unsigned_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						uint8x8_t bmm = __builtin_bit_cast(uint8x8_t,b);
						return __builtin_bit_cast(vec_type,vand_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						uint16x4_t bmm = __builtin_bit_cast(uint16x4_t,b);
						return __builtin_bit_cast(vec_type,vand_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						uint32x2_t bmm = __builtin_bit_cast(uint32x2_t,b);
						return __builtin_bit_cast(vec_type,vand_u32(amm,bmm));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						uint64x1_t bmm = __builtin_bit_cast(uint64x1_t,b);
						return __builtin_bit_cast(vec_type,vand_u64(amm,bmm));
					}
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						uint8x16_t bmm = __builtin_bit_cast(uint8x16_t,b);
						return __builtin_bit_cast(vec_type,vandq_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						uint16x8_t bmm = __builtin_bit_cast(uint16x8_t,b);
						return __builtin_bit_cast(vec_type,vandq_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						uint32x4_t bmm = __builtin_bit_cast(uint32x4_t,b);
						return __builtin_bit_cast(vec_type,vandq_u32(amm,bmm));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						uint64x2_t bmm = __builtin_bit_cast(uint64x2_t,b);
						return __builtin_bit_cast(vec_type,vandq_u64(amm,bmm));
					}
				}
			}
		}
#endif
	}
	return ::fast_io::details::generic_simd_create_op_impl(a,b,[](T va,T vb) noexcept -> T
	{
		return va&vb;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator|(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
#if (defined(__x86_64__) || defined(_M_AMD64) || defined(__i386__) || defined(_M_IX86))
		using vec_type = simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16&&::fast_io::details::cpu_flags::sse2_supported)
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
#elif defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
		using vec_type = simd_vector<T,N>;
		if constexpr(::fast_io::details::cpu_flags::armneon_supported)
		{
			if constexpr(std::signed_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						uint8x8_t bmm = __builtin_bit_cast(uint8x8_t,b);
						return __builtin_bit_cast(vec_type,vorr_s8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						uint16x4_t bmm = __builtin_bit_cast(uint16x4_t,b);
						return __builtin_bit_cast(vec_type,vorr_s16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						uint32x2_t bmm = __builtin_bit_cast(uint32x2_t,b);
						return __builtin_bit_cast(vec_type,vorr_s32(amm,bmm));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						uint64x1_t bmm = __builtin_bit_cast(uint64x1_t,b);
						return __builtin_bit_cast(vec_type,vorr_s64(amm,bmm));
					}
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						uint8x16_t bmm = __builtin_bit_cast(uint8x16_t,b);
						return __builtin_bit_cast(vec_type,vorrq_s8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						uint16x8_t bmm = __builtin_bit_cast(uint16x8_t,b);
						return __builtin_bit_cast(vec_type,vorrq_s16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						uint32x4_t bmm = __builtin_bit_cast(uint32x4_t,b);
						return __builtin_bit_cast(vec_type,vorrq_s32(amm,bmm));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						uint64x2_t bmm = __builtin_bit_cast(uint64x2_t,b);
						return __builtin_bit_cast(vec_type,vorrq_s64(amm,bmm));
					}
				}
			}
			else if constexpr(std::unsigned_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						uint8x8_t bmm = __builtin_bit_cast(uint8x8_t,b);
						return __builtin_bit_cast(vec_type,vorr_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						uint16x4_t bmm = __builtin_bit_cast(uint16x4_t,b);
						return __builtin_bit_cast(vec_type,vorr_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						uint32x2_t bmm = __builtin_bit_cast(uint32x2_t,b);
						return __builtin_bit_cast(vec_type,vorr_u32(amm,bmm));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						uint64x1_t bmm = __builtin_bit_cast(uint64x1_t,b);
						return __builtin_bit_cast(vec_type,vorr_u64(amm,bmm));
					}
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						uint8x16_t bmm = __builtin_bit_cast(uint8x16_t,b);
						return __builtin_bit_cast(vec_type,vorrq_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						uint16x8_t bmm = __builtin_bit_cast(uint16x8_t,b);
						return __builtin_bit_cast(vec_type,vorrq_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						uint32x4_t bmm = __builtin_bit_cast(uint32x4_t,b);
						return __builtin_bit_cast(vec_type,vorrq_u32(amm,bmm));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						uint64x2_t bmm = __builtin_bit_cast(uint64x2_t,b);
						return __builtin_bit_cast(vec_type,vorrq_u64(amm,bmm));
					}
				}
			}
		}
#endif
	}
	return ::fast_io::details::generic_simd_create_op_impl(a,b,[](T va,T vb) noexcept -> T
	{
		return va|vb;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator^(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
#if (defined(__x86_64__) || defined(_M_AMD64) || defined(__i386__) || defined(_M_IX86))
		using vec_type = simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16&&::fast_io::details::cpu_flags::sse2_supported)
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
#elif defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
		using vec_type = simd_vector<T,N>;
		if constexpr(::fast_io::details::cpu_flags::armneon_supported)
		{
			if constexpr(std::signed_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						uint8x8_t bmm = __builtin_bit_cast(uint8x8_t,b);
						return __builtin_bit_cast(vec_type,veor_s8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						uint16x4_t bmm = __builtin_bit_cast(uint16x4_t,b);
						return __builtin_bit_cast(vec_type,veor_s16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						uint32x2_t bmm = __builtin_bit_cast(uint32x2_t,b);
						return __builtin_bit_cast(vec_type,veor_s32(amm,bmm));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						uint64x1_t bmm = __builtin_bit_cast(uint64x1_t,b);
						return __builtin_bit_cast(vec_type,veor_s64(amm,bmm));
					}
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						uint8x16_t bmm = __builtin_bit_cast(uint8x16_t,b);
						return __builtin_bit_cast(vec_type,veorq_s8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						uint16x8_t bmm = __builtin_bit_cast(uint16x8_t,b);
						return __builtin_bit_cast(vec_type,veorq_s16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						uint32x4_t bmm = __builtin_bit_cast(uint32x4_t,b);
						return __builtin_bit_cast(vec_type,veorq_s32(amm,bmm));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						uint64x2_t bmm = __builtin_bit_cast(uint64x2_t,b);
						return __builtin_bit_cast(vec_type,veorq_s64(amm,bmm));
					}
				}
			}
			else if constexpr(std::unsigned_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						uint8x8_t bmm = __builtin_bit_cast(uint8x8_t,b);
						return __builtin_bit_cast(vec_type,veor_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						uint16x4_t bmm = __builtin_bit_cast(uint16x4_t,b);
						return __builtin_bit_cast(vec_type,veor_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						uint32x2_t bmm = __builtin_bit_cast(uint32x2_t,b);
						return __builtin_bit_cast(vec_type,veor_u32(amm,bmm));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						uint64x1_t bmm = __builtin_bit_cast(uint64x1_t,b);
						return __builtin_bit_cast(vec_type,veor_u64(amm,bmm));
					}
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						uint8x16_t bmm = __builtin_bit_cast(uint8x16_t,b);
						return __builtin_bit_cast(vec_type,veorq_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						uint16x8_t bmm = __builtin_bit_cast(uint16x8_t,b);
						return __builtin_bit_cast(vec_type,veorq_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						uint32x4_t bmm = __builtin_bit_cast(uint32x4_t,b);
						return __builtin_bit_cast(vec_type,veorq_u32(amm,bmm));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						uint64x2_t bmm = __builtin_bit_cast(uint64x2_t,b);
						return __builtin_bit_cast(vec_type,veorq_u64(amm,bmm));
					}
				}
			}
		}
#endif
	}
	return ::fast_io::details::generic_simd_create_op_impl(a,b,[](T va,T vb) noexcept -> T
	{
		return va^vb;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator<<(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
#if (defined(__x86_64__) || defined(_M_AMD64) || defined(__i386__) || defined(_M_IX86))
#elif defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
		using vec_type = simd_vector<T,N>;
		if constexpr(::fast_io::details::cpu_flags::armneon_supported)
		{
			if constexpr(std::signed_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						uint8x8_t bmm = __builtin_bit_cast(uint8x8_t,b);
						return __builtin_bit_cast(vec_type,vshl_s8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						uint16x4_t bmm = __builtin_bit_cast(uint16x4_t,b);
						return __builtin_bit_cast(vec_type,vshl_s16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						uint32x2_t bmm = __builtin_bit_cast(uint32x2_t,b);
						return __builtin_bit_cast(vec_type,vshl_s32(amm,bmm));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						uint64x1_t bmm = __builtin_bit_cast(uint64x1_t,b);
						return __builtin_bit_cast(vec_type,vshl_s64(amm,bmm));
					}
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						uint8x16_t bmm = __builtin_bit_cast(uint8x16_t,b);
						return __builtin_bit_cast(vec_type,vshlq_s8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						uint16x8_t bmm = __builtin_bit_cast(uint16x8_t,b);
						return __builtin_bit_cast(vec_type,vshlq_s16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						uint32x4_t bmm = __builtin_bit_cast(uint32x4_t,b);
						return __builtin_bit_cast(vec_type,vshlq_s32(amm,bmm));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						uint64x2_t bmm = __builtin_bit_cast(uint64x2_t,b);
						return __builtin_bit_cast(vec_type,vshlq_s64(amm,bmm));
					}
				}
			}
			else if constexpr(std::unsigned_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						uint8x8_t bmm = __builtin_bit_cast(uint8x8_t,b);
						return __builtin_bit_cast(vec_type,vshl_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						uint16x4_t bmm = __builtin_bit_cast(uint16x4_t,b);
						return __builtin_bit_cast(vec_type,vshl_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						uint32x2_t bmm = __builtin_bit_cast(uint32x2_t,b);
						return __builtin_bit_cast(vec_type,vshl_u32(amm,bmm));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						uint64x1_t bmm = __builtin_bit_cast(uint64x1_t,b);
						return __builtin_bit_cast(vec_type,vshl_u64(amm,bmm));
					}
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						uint8x16_t bmm = __builtin_bit_cast(uint8x16_t,b);
						return __builtin_bit_cast(vec_type,vshlq_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						uint16x8_t bmm = __builtin_bit_cast(uint16x8_t,b);
						return __builtin_bit_cast(vec_type,vshlq_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						uint32x4_t bmm = __builtin_bit_cast(uint32x4_t,b);
						return __builtin_bit_cast(vec_type,vshlq_u32(amm,bmm));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						uint64x2_t bmm = __builtin_bit_cast(uint64x2_t,b);
						return __builtin_bit_cast(vec_type,vshlq_u64(amm,bmm));
					}
				}
			}
		}
#endif
	}
	return ::fast_io::details::generic_simd_create_op_impl(a,b,[](T va,T vb) noexcept -> T
	{
		return va<<vb;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator>>(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
	}
	return ::fast_io::details::generic_simd_create_op_impl(a,b,[](T va,T vb) noexcept -> T
	{
		return va>>vb;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator<<(simd_vector<T,N> const& a,unsigned i) noexcept
{
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
#if (defined(__x86_64__) || defined(_M_AMD64) || defined(__i386__) || defined(_M_IX86))
		using vec_type = simd_vector<T,N>;
		if constexpr(2<=sizeof(T)&&sizeof(T)<=8)
		{
			if constexpr(sizeof(vec_type)==16&&::fast_io::details::cpu_flags::sse2_supported)
			{
				__m128i amm = __builtin_bit_cast(__m128i,a);
				if constexpr(sizeof(T)==2)
				{
					amm = _mm_slli_epi16(amm,i);
				}
				else if constexpr(sizeof(T)==4)
				{
					amm = _mm_slli_epi32(amm,i);
				}
				else if constexpr(sizeof(T)==8)
				{
					amm = _mm_slli_epi64(amm,i);
				}
				return __builtin_bit_cast(vec_type,amm);
			}
			else if constexpr(sizeof(vec_type)==32)
			{
				if constexpr(::fast_io::details::cpu_flags::avx2_supported)
				{
					__m256i amm = __builtin_bit_cast(__m256i,a);
					if constexpr(sizeof(T)==2)
					{
						amm = _mm256_slli_epi16(amm,i);
					}
					else if constexpr(sizeof(T)==4)
					{
						amm = _mm256_slli_epi32(amm,i);
					}
					else if constexpr(sizeof(T)==8)
					{
						amm = _mm256_slli_epi64(amm,i);
					}
					return __builtin_bit_cast(vec_type,amm);
				}
				else if constexpr(::fast_io::details::cpu_flags::avx_supported)
				{
					__m256i amm = __builtin_bit_cast(__m256i,a);
					__m128i alow = _mm256_castsi256_si128(amm);
					__m128i ahigh = _mm256_extractf128_si256(amm,1);
					if constexpr(sizeof(T)==2)
					{
						alow = _mm_slli_epi16(alow,i);
						ahigh = _mm_slli_epi16(ahigh,i);
					}
					else if constexpr(sizeof(T)==4)
					{
						alow = _mm_slli_epi32(alow,i);
						ahigh = _mm_slli_epi32(ahigh,i);
					}
					else if constexpr(sizeof(T)==8)
					{
						alow = _mm_slli_epi64(alow,i);
						ahigh = _mm_slli_epi64(ahigh,i);
					}
					__m256i res = _mm256_castsi128_si256(alow);
					res = _mm256_insertf128_si256(res,ahigh,1);
					return __builtin_bit_cast(vec_type,res);
				}
			}
			else if constexpr(sizeof(vec_type)==64)
			{
				if constexpr(::fast_io::details::cpu_flags::avx512bw_supported)
				{
					__m512i amm = __builtin_bit_cast(__m512i,a);
					if constexpr(sizeof(T)==2)
					{
						amm = _mm512_slli_epi16(amm,i);
					}
					else if constexpr(sizeof(T)==4)
					{
						amm = _mm512_slli_epi32(amm,i);
					}
					else if constexpr(sizeof(T)==8)
					{
						amm = _mm512_slli_epi64(amm,i);
					}
					return __builtin_bit_cast(vec_type,amm);
				}
			}
		}
#elif defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
		using vec_type = simd_vector<T,N>;
		if constexpr(::fast_io::details::cpu_flags::armneon_supported)
		{
			if constexpr(std::signed_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						return __builtin_bit_cast(vec_type,vshl_n_s8(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						return __builtin_bit_cast(vec_type,vshl_n_s16(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						return __builtin_bit_cast(vec_type,vshl_n_s32(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						return __builtin_bit_cast(vec_type,vshl_n_s64(amm,static_cast<int>(i)));
					}
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						return __builtin_bit_cast(vec_type,vshlq_n_s8(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						return __builtin_bit_cast(vec_type,vshlq_n_s16(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						return __builtin_bit_cast(vec_type,vshlq_n_s32(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						return __builtin_bit_cast(vec_type,vshlq_n_s64(amm,static_cast<int>(i)));
					}
				}
			}
			else if constexpr(std::unsigned_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						return __builtin_bit_cast(vec_type,vshl_n_u8(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						return __builtin_bit_cast(vec_type,vshl_n_u16(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						return __builtin_bit_cast(vec_type,vshl_n_u32(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						return __builtin_bit_cast(vec_type,vshl_n_u64(amm,static_cast<int>(i)));
					}
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						return __builtin_bit_cast(vec_type,vshlq_n_u8(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						return __builtin_bit_cast(vec_type,vshlq_n_u16(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						return __builtin_bit_cast(vec_type,vshlq_n_u32(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						return __builtin_bit_cast(vec_type,vshlq_n_u64(amm,static_cast<int>(i)));
					}
				}
			}
		}
#endif
	}
	return ::fast_io::details::generic_simd_self_create_op_impl(a,[i](T va)
	{
		return va<<i;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator>>(simd_vector<T,N> const& a,unsigned i) noexcept
{
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
#if (defined(__x86_64__) || defined(_M_AMD64) || defined(__i386__) || defined(_M_IX86))
		using vec_type = simd_vector<T,N>;
		if constexpr(2<=sizeof(T)&&sizeof(T)<=8)
		{
			if constexpr(sizeof(vec_type)==16&&::fast_io::details::cpu_flags::sse2_supported)
			{
				__m128i amm = __builtin_bit_cast(__m128i,a);
				if constexpr(sizeof(T)==2)
				{
					amm = _mm_srli_epi16(amm,i);
				}
				else if constexpr(sizeof(T)==4)
				{
					amm = _mm_srli_epi32(amm,i);
				}
				else if constexpr(sizeof(T)==8)
				{
					amm = _mm_srli_epi64(amm,i);
				}
				return __builtin_bit_cast(vec_type,amm);
			}
			else if constexpr(sizeof(vec_type)==32)
			{
				if constexpr(::fast_io::details::cpu_flags::avx2_supported)
				{
					__m256i amm = __builtin_bit_cast(__m256i,a);
					if constexpr(sizeof(T)==2)
					{
						amm = _mm256_srli_epi16(amm,i);
					}
					else if constexpr(sizeof(T)==4)
					{
						amm = _mm256_srli_epi32(amm,i);
					}
					else if constexpr(sizeof(T)==8)
					{
						amm = _mm256_srli_epi64(amm,i);
					}
					return __builtin_bit_cast(vec_type,amm);
				}
				else if constexpr(::fast_io::details::cpu_flags::avx_supported)
				{
					__m256i amm = __builtin_bit_cast(__m256i,a);
					__m128i alow = _mm256_castsi256_si128(amm);
					__m128i ahigh = _mm256_extractf128_si256(amm,1);
					if constexpr(sizeof(T)==2)
					{
						alow = _mm_srli_epi16(alow,i);
						ahigh = _mm_srli_epi16(ahigh,i);
					}
					else if constexpr(sizeof(T)==4)
					{
						alow = _mm_srli_epi32(alow,i);
						ahigh = _mm_srli_epi32(ahigh,i);
					}
					else if constexpr(sizeof(T)==8)
					{
						alow = _mm_srli_epi64(alow,i);
						ahigh = _mm_srli_epi64(ahigh,i);
					}
					__m256i res = _mm256_castsi128_si256(alow);
					res = _mm256_insertf128_si256(res,ahigh,1);
					return __builtin_bit_cast(vec_type,res);
				}
			}
			else if constexpr(sizeof(vec_type)==64)
			{
				if constexpr(::fast_io::details::cpu_flags::avx512bw_supported)
				{
					__m512i amm = __builtin_bit_cast(__m512i,a);
					if constexpr(sizeof(T)==2)
					{
						amm = _mm512_srli_epi16(amm,i);
					}
					else if constexpr(sizeof(T)==4)
					{
						amm = _mm512_srli_epi32(amm,i);
					}
					else if constexpr(sizeof(T)==8)
					{
						amm = _mm512_srli_epi64(amm,i);
					}
					return __builtin_bit_cast(vec_type,amm);
				}
			}
		}
#elif defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
		using vec_type = simd_vector<T,N>;
		if constexpr(::fast_io::details::cpu_flags::armneon_supported)
		{
			if constexpr(std::signed_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						return __builtin_bit_cast(vec_type,vshr_n_s8(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						return __builtin_bit_cast(vec_type,vshr_n_s16(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						return __builtin_bit_cast(vec_type,vshr_n_s32(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						return __builtin_bit_cast(vec_type,vshr_n_s64(amm,static_cast<int>(i)));
					}
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						return __builtin_bit_cast(vec_type,vshrq_n_s8(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						return __builtin_bit_cast(vec_type,vshrq_n_s16(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						return __builtin_bit_cast(vec_type,vshrq_n_s32(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						return __builtin_bit_cast(vec_type,vshrq_n_s64(amm,static_cast<int>(i)));
					}
				}
			}
			else if constexpr(std::unsigned_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						return __builtin_bit_cast(vec_type,vshr_n_u8(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						return __builtin_bit_cast(vec_type,vshr_n_u16(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						return __builtin_bit_cast(vec_type,vshr_n_u32(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						return __builtin_bit_cast(vec_type,vshr_n_u64(amm,static_cast<int>(i)));
					}
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						return __builtin_bit_cast(vec_type,vshrq_n_u8(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						return __builtin_bit_cast(vec_type,vshrq_n_u16(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						return __builtin_bit_cast(vec_type,vshrq_n_u32(amm,static_cast<int>(i)));
					}
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						return __builtin_bit_cast(vec_type,vshrq_n_u64(amm,static_cast<int>(i)));
					}
				}
			}
		}
#endif
	}
	return ::fast_io::details::generic_simd_self_create_op_impl(a,[i](T va)
	{
		return va>>i;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator<(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
#if (defined(__x86_64__) || defined(_M_AMD64) || defined(__i386__) || defined(_M_IX86))
		using vec_type = ::fast_io::intrinsics::simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16&&::fast_io::details::cpu_flags::sse2_supported)
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
							mask=_mm_set1_epi8(mn);
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
							mask=_mm256_set1_epi8(mn);
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
						ahigh = _mm_cmplt_epi8(ahigh,bhigh);
					}
					else if constexpr(sizeof(T)==2)
					{
						alow = _mm_cmplt_epi16(alow,blow);
						ahigh = _mm_cmplt_epi16(ahigh,bhigh);
					}
					else if constexpr(sizeof(T)==4)
					{
						alow = _mm_cmplt_epi32(alow,blow);
						ahigh = _mm_cmplt_epi32(ahigh,bhigh);
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
#elif defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
		using vec_type = simd_vector<T,N>;
		if constexpr(::fast_io::details::cpu_flags::armneon_supported)
		{
			if constexpr(std::signed_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						uint8x8_t bmm = __builtin_bit_cast(uint8x8_t,b);
						return __builtin_bit_cast(vec_type,vclt_s8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						uint16x4_t bmm = __builtin_bit_cast(uint16x4_t,b);
						return __builtin_bit_cast(vec_type,vclt_s16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						uint32x2_t bmm = __builtin_bit_cast(uint32x2_t,b);
						return __builtin_bit_cast(vec_type,vclt_s32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						uint64x1_t bmm = __builtin_bit_cast(uint64x1_t,b);
						return __builtin_bit_cast(vec_type,vclt_s64(amm,bmm));
					}
#endif
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						uint8x16_t bmm = __builtin_bit_cast(uint8x16_t,b);
						return __builtin_bit_cast(vec_type,vcltq_s8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						uint16x8_t bmm = __builtin_bit_cast(uint16x8_t,b);
						return __builtin_bit_cast(vec_type,vcltq_s16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						uint32x4_t bmm = __builtin_bit_cast(uint32x4_t,b);
						return __builtin_bit_cast(vec_type,vcltq_s32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						uint64x2_t bmm = __builtin_bit_cast(uint64x2_t,b);
						return __builtin_bit_cast(vec_type,vcltq_s64(amm,bmm));
					}
#endif
				}
			}
			else if constexpr(std::unsigned_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						uint8x8_t bmm = __builtin_bit_cast(uint8x8_t,b);
						return __builtin_bit_cast(vec_type,vclt_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						uint16x4_t bmm = __builtin_bit_cast(uint16x4_t,b);
						return __builtin_bit_cast(vec_type,vclt_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						uint32x2_t bmm = __builtin_bit_cast(uint32x2_t,b);
						return __builtin_bit_cast(vec_type,vclt_u32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						uint64x1_t bmm = __builtin_bit_cast(uint64x1_t,b);
						return __builtin_bit_cast(vec_type,vclt_u64(amm,bmm));
					}
#endif
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						uint8x16_t bmm = __builtin_bit_cast(uint8x16_t,b);
						return __builtin_bit_cast(vec_type,vcltq_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						uint16x8_t bmm = __builtin_bit_cast(uint16x8_t,b);
						return __builtin_bit_cast(vec_type,vcltq_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						uint32x4_t bmm = __builtin_bit_cast(uint32x4_t,b);
						return __builtin_bit_cast(vec_type,vcltq_u32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						uint64x2_t bmm = __builtin_bit_cast(uint64x2_t,b);
						return __builtin_bit_cast(vec_type,vcltq_u64(amm,bmm));
					}
#endif
				}
			}
		}
#endif
	}
	return ::fast_io::details::generic_simd_comparision_common_impl(a,b,[](T va,T vb) noexcept -> bool
	{
		return va<vb;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator>(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
#if (defined(__x86_64__) || defined(_M_AMD64) || defined(__i386__) || defined(_M_IX86))
		using vec_type = ::fast_io::intrinsics::simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16&&::fast_io::details::cpu_flags::sse2_supported)
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
							mask=_mm_set1_epi8(mn);
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
							mask=_mm256_set1_epi8(mn);
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
						ahigh = _mm_cmpgt_epi8(ahigh,bhigh);
					}
					else if constexpr(sizeof(T)==2)
					{
						alow = _mm_cmpgt_epi16(alow,blow);
						ahigh = _mm_cmpgt_epi16(ahigh,bhigh);
					}
					else if constexpr(sizeof(T)==4)
					{
						alow = _mm_cmpgt_epi32(alow,blow);
						ahigh = _mm_cmpgt_epi32(ahigh,bhigh);
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
#elif defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
		using vec_type = simd_vector<T,N>;
		if constexpr(::fast_io::details::cpu_flags::armneon_supported)
		{
			if constexpr(std::signed_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						uint8x8_t bmm = __builtin_bit_cast(uint8x8_t,b);
						return __builtin_bit_cast(vec_type,vcgt_s8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						uint16x4_t bmm = __builtin_bit_cast(uint16x4_t,b);
						return __builtin_bit_cast(vec_type,vcgt_s16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						uint32x2_t bmm = __builtin_bit_cast(uint32x2_t,b);
						return __builtin_bit_cast(vec_type,vcgt_s32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						uint64x1_t bmm = __builtin_bit_cast(uint64x1_t,b);
						return __builtin_bit_cast(vec_type,vcgt_s64(amm,bmm));
					}
#endif
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						uint8x16_t bmm = __builtin_bit_cast(uint8x16_t,b);
						return __builtin_bit_cast(vec_type,vcgtq_s8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						uint16x8_t bmm = __builtin_bit_cast(uint16x8_t,b);
						return __builtin_bit_cast(vec_type,vcgtq_s16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						uint32x4_t bmm = __builtin_bit_cast(uint32x4_t,b);
						return __builtin_bit_cast(vec_type,vcgtq_s32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						uint64x2_t bmm = __builtin_bit_cast(uint64x2_t,b);
						return __builtin_bit_cast(vec_type,vcgtq_s64(amm,bmm));
					}
#endif
				}
			}
			else if constexpr(std::unsigned_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						uint8x8_t bmm = __builtin_bit_cast(uint8x8_t,b);
						return __builtin_bit_cast(vec_type,vcgt_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						uint16x4_t bmm = __builtin_bit_cast(uint16x4_t,b);
						return __builtin_bit_cast(vec_type,vcgt_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						uint32x2_t bmm = __builtin_bit_cast(uint32x2_t,b);
						return __builtin_bit_cast(vec_type,vcgt_u32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						uint64x1_t bmm = __builtin_bit_cast(uint64x1_t,b);
						return __builtin_bit_cast(vec_type,vcgt_u64(amm,bmm));
					}
#endif
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						uint8x16_t bmm = __builtin_bit_cast(uint8x16_t,b);
						return __builtin_bit_cast(vec_type,vcgtq_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						uint16x8_t bmm = __builtin_bit_cast(uint16x8_t,b);
						return __builtin_bit_cast(vec_type,vcgtq_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						uint32x4_t bmm = __builtin_bit_cast(uint32x4_t,b);
						return __builtin_bit_cast(vec_type,vcgtq_u32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						uint64x2_t bmm = __builtin_bit_cast(uint64x2_t,b);
						return __builtin_bit_cast(vec_type,vcgtq_u64(amm,bmm));
					}
#endif
				}
			}
		}
#endif
	}
	return ::fast_io::details::generic_simd_comparision_common_impl(a,b,[](T va,T vb) noexcept -> bool
	{
		return va>vb;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator<=(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
#if (defined(__x86_64__) || defined(_M_AMD64) || defined(__i386__) || defined(_M_IX86))
		using vec_type = ::fast_io::intrinsics::simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16&&::fast_io::details::cpu_flags::sse2_supported)
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
#elif defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
		using vec_type = simd_vector<T,N>;
		if constexpr(::fast_io::details::cpu_flags::armneon_supported)
		{
			if constexpr(std::signed_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						uint8x8_t bmm = __builtin_bit_cast(uint8x8_t,b);
						return __builtin_bit_cast(vec_type,vcle_s8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						uint16x4_t bmm = __builtin_bit_cast(uint16x4_t,b);
						return __builtin_bit_cast(vec_type,vcle_s16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						uint32x2_t bmm = __builtin_bit_cast(uint32x2_t,b);
						return __builtin_bit_cast(vec_type,vcle_s32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						uint64x1_t bmm = __builtin_bit_cast(uint64x1_t,b);
						return __builtin_bit_cast(vec_type,vcle_s64(amm,bmm));
					}
#endif
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						uint8x16_t bmm = __builtin_bit_cast(uint8x16_t,b);
						return __builtin_bit_cast(vec_type,vcleq_s8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						uint16x8_t bmm = __builtin_bit_cast(uint16x8_t,b);
						return __builtin_bit_cast(vec_type,vcleq_s16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						uint32x4_t bmm = __builtin_bit_cast(uint32x4_t,b);
						return __builtin_bit_cast(vec_type,vcleq_s32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						uint64x2_t bmm = __builtin_bit_cast(uint64x2_t,b);
						return __builtin_bit_cast(vec_type,vcleq_s64(amm,bmm));
					}
#endif
				}
			}
			else if constexpr(std::unsigned_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						uint8x8_t bmm = __builtin_bit_cast(uint8x8_t,b);
						return __builtin_bit_cast(vec_type,vcle_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						uint16x4_t bmm = __builtin_bit_cast(uint16x4_t,b);
						return __builtin_bit_cast(vec_type,vcle_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						uint32x2_t bmm = __builtin_bit_cast(uint32x2_t,b);
						return __builtin_bit_cast(vec_type,vcle_u32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						uint64x1_t bmm = __builtin_bit_cast(uint64x1_t,b);
						return __builtin_bit_cast(vec_type,vcle_u64(amm,bmm));
					}
#endif
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						uint8x16_t bmm = __builtin_bit_cast(uint8x16_t,b);
						return __builtin_bit_cast(vec_type,vcleq_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						uint16x8_t bmm = __builtin_bit_cast(uint16x8_t,b);
						return __builtin_bit_cast(vec_type,vcleq_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						uint32x4_t bmm = __builtin_bit_cast(uint32x4_t,b);
						return __builtin_bit_cast(vec_type,vcleq_u32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						uint64x2_t bmm = __builtin_bit_cast(uint64x2_t,b);
						return __builtin_bit_cast(vec_type,vcleq_u64(amm,bmm));
					}
#endif
				}
			}
		}
#endif
	}
	return ::fast_io::details::generic_simd_comparision_common_impl(a,b,[](T va,T vb) noexcept -> bool
	{
		return va<=vb;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator>=(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
#if (defined(__x86_64__) || defined(_M_AMD64) || defined(__i386__) || defined(_M_IX86))
		using vec_type = ::fast_io::intrinsics::simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16&&::fast_io::details::cpu_flags::sse2_supported)
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
#elif defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
		using vec_type = simd_vector<T,N>;
		if constexpr(::fast_io::details::cpu_flags::armneon_supported)
		{
			if constexpr(std::signed_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						uint8x8_t bmm = __builtin_bit_cast(uint8x8_t,b);
						return __builtin_bit_cast(vec_type,vcge_s8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						uint16x4_t bmm = __builtin_bit_cast(uint16x4_t,b);
						return __builtin_bit_cast(vec_type,vcge_s16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						uint32x2_t bmm = __builtin_bit_cast(uint32x2_t,b);
						return __builtin_bit_cast(vec_type,vcge_s32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						uint64x1_t bmm = __builtin_bit_cast(uint64x1_t,b);
						return __builtin_bit_cast(vec_type,vcge_s64(amm,bmm));
					}
#endif
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						uint8x16_t bmm = __builtin_bit_cast(uint8x16_t,b);
						return __builtin_bit_cast(vec_type,vcgeq_s8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						uint16x8_t bmm = __builtin_bit_cast(uint16x8_t,b);
						return __builtin_bit_cast(vec_type,vcgeq_s16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						uint32x4_t bmm = __builtin_bit_cast(uint32x4_t,b);
						return __builtin_bit_cast(vec_type,vcgeq_s32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						uint64x2_t bmm = __builtin_bit_cast(uint64x2_t,b);
						return __builtin_bit_cast(vec_type,vcgeq_s64(amm,bmm));
					}
#endif
				}
			}
			else if constexpr(std::unsigned_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						uint8x8_t bmm = __builtin_bit_cast(uint8x8_t,b);
						return __builtin_bit_cast(vec_type,vcge_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						uint16x4_t bmm = __builtin_bit_cast(uint16x4_t,b);
						return __builtin_bit_cast(vec_type,vcge_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						uint32x2_t bmm = __builtin_bit_cast(uint32x2_t,b);
						return __builtin_bit_cast(vec_type,vcge_u32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						uint64x1_t bmm = __builtin_bit_cast(uint64x1_t,b);
						return __builtin_bit_cast(vec_type,vcge_u64(amm,bmm));
					}
#endif
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						uint8x16_t bmm = __builtin_bit_cast(uint8x16_t,b);
						return __builtin_bit_cast(vec_type,vcgeq_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						uint16x8_t bmm = __builtin_bit_cast(uint16x8_t,b);
						return __builtin_bit_cast(vec_type,vcgeq_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						uint32x4_t bmm = __builtin_bit_cast(uint32x4_t,b);
						return __builtin_bit_cast(vec_type,vcgeq_u32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						uint64x2_t bmm = __builtin_bit_cast(uint64x2_t,b);
						return __builtin_bit_cast(vec_type,vcgeq_u64(amm,bmm));
					}
#endif
				}
			}
		}
#endif
	}
	return ::fast_io::details::generic_simd_comparision_common_impl(a,b,[](T va,T vb) noexcept -> bool
	{
		return va>=vb;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator==(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
#if (defined(__x86_64__) || defined(_M_AMD64) || defined(__i386__) || defined(_M_IX86))
		using vec_type = simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16&&::fast_io::details::cpu_flags::sse2_supported)
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
#elif defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
		using vec_type = simd_vector<T,N>;
		if constexpr(::fast_io::details::cpu_flags::armneon_supported)
		{
			if constexpr(std::signed_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						uint8x8_t bmm = __builtin_bit_cast(uint8x8_t,b);
						return __builtin_bit_cast(vec_type,vceq_s8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						uint16x4_t bmm = __builtin_bit_cast(uint16x4_t,b);
						return __builtin_bit_cast(vec_type,vceq_s16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						uint32x2_t bmm = __builtin_bit_cast(uint32x2_t,b);
						return __builtin_bit_cast(vec_type,vceq_s32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						uint64x1_t bmm = __builtin_bit_cast(uint64x1_t,b);
						return __builtin_bit_cast(vec_type,vceq_s64(amm,bmm));
					}
#endif
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						uint8x16_t bmm = __builtin_bit_cast(uint8x16_t,b);
						return __builtin_bit_cast(vec_type,vceqq_s8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						uint16x8_t bmm = __builtin_bit_cast(uint16x8_t,b);
						return __builtin_bit_cast(vec_type,vceqq_s16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						uint32x4_t bmm = __builtin_bit_cast(uint32x4_t,b);
						return __builtin_bit_cast(vec_type,vceqq_s32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						uint64x2_t bmm = __builtin_bit_cast(uint64x2_t,b);
						return __builtin_bit_cast(vec_type,vceqq_s64(amm,bmm));
					}
#endif
				}
			}
			else if constexpr(std::unsigned_integral<T>)
			{
				if constexpr(sizeof(vec_type)==8)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
						uint8x8_t bmm = __builtin_bit_cast(uint8x8_t,b);
						return __builtin_bit_cast(vec_type,vceq_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
						uint16x4_t bmm = __builtin_bit_cast(uint16x4_t,b);
						return __builtin_bit_cast(vec_type,vceq_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
						uint32x2_t bmm = __builtin_bit_cast(uint32x2_t,b);
						return __builtin_bit_cast(vec_type,vceq_u32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==8)
					{
						uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
						uint64x1_t bmm = __builtin_bit_cast(uint64x1_t,b);
						return __builtin_bit_cast(vec_type,vceq_u64(amm,bmm));
					}
#endif
				}
				else if constexpr(sizeof(vec_type)==16)
				{
					if constexpr(sizeof(T)==1)
					{
						uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
						uint8x16_t bmm = __builtin_bit_cast(uint8x16_t,b);
						return __builtin_bit_cast(vec_type,vceqq_u8(amm,bmm));
					}
					else if constexpr(sizeof(T)==2)
					{
						uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
						uint16x8_t bmm = __builtin_bit_cast(uint16x8_t,b);
						return __builtin_bit_cast(vec_type,vceqq_u16(amm,bmm));
					}
					else if constexpr(sizeof(T)==4)
					{
						uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
						uint32x4_t bmm = __builtin_bit_cast(uint32x4_t,b);
						return __builtin_bit_cast(vec_type,vceqq_u32(amm,bmm));
					}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
					else if constexpr(sizeof(T)==8)
					{
						uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
						uint64x2_t bmm = __builtin_bit_cast(uint64x2_t,b);
						return __builtin_bit_cast(vec_type,vceqq_u64(amm,bmm));
					}
#endif
				}
			}
		}
#endif
	}
	return ::fast_io::details::generic_simd_comparision_common_impl(a,b,[](T va,T vb) noexcept -> bool
	{
		return va==vb;
	});
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator!=(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
	using vec_type = simd_vector<T,N>;
	constexpr bool using_avx512{::fast_io::details::cpu_flags::avx512f_supported&&::fast_io::details::cpu_flags::avx512bw_supported&&::fast_io::details::cpu_flags::avx512vl_supported};
	constexpr bool using_simd{(sizeof(vec_type)==16&&::fast_io::details::cpu_flags::sse2_supported)||(sizeof(vec_type)==32&&::fast_io::details::cpu_flags::avx2_supported)};
	if constexpr(using_simd&&(!using_avx512))
	{
		return (::fast_io::details::all_zero_simd_vector_mask<T,N>)==(a==b);
	}
	else
	{
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
#if (defined(__x86_64__) || defined(_M_AMD64) || defined(__i386__) || defined(_M_IX86))
		if constexpr(using_avx512)
		{
			if constexpr(sizeof(vec_type)==16&&::fast_io::details::cpu_flags::sse2_supported)
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
#elif defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
		using vec_type = simd_vector<T,N>;
		if constexpr(std::signed_integral<T>)
		{
			if constexpr(sizeof(vec_type)==8)
			{
				if constexpr(sizeof(T)==1)
				{
					uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
					uint8x8_t bmm = __builtin_bit_cast(uint8x8_t,b);
					return __builtin_bit_cast(vec_type,vtst_s8(amm,bmm));
				}
				else if constexpr(sizeof(T)==2)
				{
					uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
					uint16x4_t bmm = __builtin_bit_cast(uint16x4_t,b);
					return __builtin_bit_cast(vec_type,vtst_s16(amm,bmm));
				}
				else if constexpr(sizeof(T)==4)
				{
					uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
					uint32x2_t bmm = __builtin_bit_cast(uint32x2_t,b);
					return __builtin_bit_cast(vec_type,vtst_s32(amm,bmm));
				}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
				else if constexpr(sizeof(T)==8)
				{
					uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
					uint64x1_t bmm = __builtin_bit_cast(uint64x1_t,b);
					return __builtin_bit_cast(vec_type,vtst_s64(amm,bmm));
				}
#endif
			}
			else if constexpr(sizeof(vec_type)==16)
			{
				if constexpr(sizeof(T)==1)
				{
					uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
					uint8x16_t bmm = __builtin_bit_cast(uint8x16_t,b);
					return __builtin_bit_cast(vec_type,vtstq_s8(amm,bmm));
				}
				else if constexpr(sizeof(T)==2)
				{
					uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
					uint16x8_t bmm = __builtin_bit_cast(uint16x8_t,b);
					return __builtin_bit_cast(vec_type,vtstq_s16(amm,bmm));
				}
				else if constexpr(sizeof(T)==4)
				{
					uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
					uint32x4_t bmm = __builtin_bit_cast(uint32x4_t,b);
					return __builtin_bit_cast(vec_type,vtstq_s32(amm,bmm));
				}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
				else if constexpr(sizeof(T)==8)
				{
					uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
					uint64x2_t bmm = __builtin_bit_cast(uint64x2_t,b);
					return __builtin_bit_cast(vec_type,vtstq_s64(amm,bmm));
				}
#endif
			}
		}
		else if constexpr(std::unsigned_integral<T>)
		{
			if constexpr(sizeof(vec_type)==8)
			{
				if constexpr(sizeof(T)==1)
				{
					uint8x8_t amm = __builtin_bit_cast(uint8x8_t,a);
					uint8x8_t bmm = __builtin_bit_cast(uint8x8_t,b);
					return __builtin_bit_cast(vec_type,vtst_u8(amm,bmm));
				}
				else if constexpr(sizeof(T)==2)
				{
					uint16x4_t amm = __builtin_bit_cast(uint16x4_t,a);
					uint16x4_t bmm = __builtin_bit_cast(uint16x4_t,b);
					return __builtin_bit_cast(vec_type,vtst_u16(amm,bmm));
				}
				else if constexpr(sizeof(T)==4)
				{
					uint32x2_t amm = __builtin_bit_cast(uint32x2_t,a);
					uint32x2_t bmm = __builtin_bit_cast(uint32x2_t,b);
					return __builtin_bit_cast(vec_type,vtst_u32(amm,bmm));
				}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
				else if constexpr(sizeof(T)==8)
				{
					uint64x1_t amm = __builtin_bit_cast(uint64x1_t,a);
					uint64x1_t bmm = __builtin_bit_cast(uint64x1_t,b);
					return __builtin_bit_cast(vec_type,vtst_u64(amm,bmm));
				}
#endif
			}
			else if constexpr(sizeof(vec_type)==16)
			{
				if constexpr(sizeof(T)==1)
				{
					uint8x16_t amm = __builtin_bit_cast(uint8x16_t,a);
					uint8x16_t bmm = __builtin_bit_cast(uint8x16_t,b);
					return __builtin_bit_cast(vec_type,vtstq_u8(amm,bmm));
				}
				else if constexpr(sizeof(T)==2)
				{
					uint16x8_t amm = __builtin_bit_cast(uint16x8_t,a);
					uint16x8_t bmm = __builtin_bit_cast(uint16x8_t,b);
					return __builtin_bit_cast(vec_type,vtstq_u16(amm,bmm));
				}
				else if constexpr(sizeof(T)==4)
				{
					uint32x4_t amm = __builtin_bit_cast(uint32x4_t,a);
					uint32x4_t bmm = __builtin_bit_cast(uint32x4_t,b);
					return __builtin_bit_cast(vec_type,vtstq_u32(amm,bmm));
				}
#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
				else if constexpr(sizeof(T)==8)
				{
					uint64x2_t amm = __builtin_bit_cast(uint64x2_t,a);
					uint64x2_t bmm = __builtin_bit_cast(uint64x2_t,b);
					return __builtin_bit_cast(vec_type,vtstq_u64(amm,bmm));
				}
#endif
			}
		}
#endif
	}
		return ::fast_io::details::generic_simd_comparision_common_impl(a,b,[](T va,T vb) noexcept -> bool
		{
			return va!=vb;
		});
	}
}

}
}
