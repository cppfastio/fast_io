#pragma once

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
simd_vector
{
	using value_type = T;
	using vec_type = value_type[N];
	vec_type value;
	constexpr T const* data() const noexcept
	{
		return __builtin_addressof(value[0]);
	}
	constexpr T* data() noexcept
	{
		return __builtin_addressof(value[0]);
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#endif
	inline void load(void const* address) noexcept
	{
#if defined(__has_builtin)
#if __has_builtin(__builtin_memcpy)
		__builtin_memcpy(__builtin_addressof(value),address,sizeof(value));
#else
		::std::memcpy(__builtin_addressof(value),address,sizeof(value));
#endif
#else
		::std::memcpy(__builtin_addressof(value),address,sizeof(value));
#endif
	}
	inline static constexpr std::size_t size() noexcept
	{
		return N;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#endif
	inline void store(void* address) noexcept
	{
#if defined(__has_builtin)
#if __has_builtin(__builtin_memcpy)
		__builtin_memcpy(address,__builtin_addressof(value),sizeof(value));
#else
		::std::memcpy(address,__builtin_addressof(value),sizeof(value));
#endif
#else
		::std::memcpy(address,__builtin_addressof(value),sizeof(value));
#endif
	}
	inline constexpr value_type front() const noexcept
	{
		return value[0];
	}
	inline constexpr value_type back() const noexcept
	{
		constexpr std::size_t nm1{N-1};
		return value[nm1];
	}

	inline static constexpr bool empty() noexcept
	{
		return !N;
	}
	inline static constexpr std::size_t max_size() noexcept
	{
		constexpr std::size_t v{static_cast<std::size_t>(-1)/sizeof(value_type)};
		return v;
	}
	inline constexpr value_type operator[](std::size_t n) const noexcept
	{
		return value[n];
	}

	inline constexpr simd_vector<T,N>& operator+=(simd_vector<T,N> const& other) noexcept
	{
		return (*this)=(*this+other);
	}
	inline constexpr simd_vector<T,N>& operator-=(simd_vector<T,N> const& other) noexcept
	{
		return (*this)=(*this-other);
	}
	inline constexpr void wrap_add_assign(simd_vector<T,N> const& other) noexcept
	{
		(*this)=::fast_io::details::wrap_add_common(*this,other);
	}
	inline constexpr simd_vector<T,N>& operator*=(simd_vector<T,N> const& other) noexcept
	{
		return *this=(*this)*other;
	}
	inline constexpr simd_vector<T,N>& operator/=(simd_vector<T,N> const& other) noexcept
	{
		return *this=(*this)/other;
	}
	inline constexpr simd_vector<T,N> operator-() const noexcept
	{
		return ::fast_io::details::wrap_minus_common(::fast_io::details::all_zero_simd_vector_mask<T,N>,*this);
	}

	template<typename T1,std::size_t N1>
	requires (sizeof(T1)*N1==sizeof(T)*N&&!std::same_as<T1,value_type>)
	explicit constexpr operator simd_vector<T1,N1>() const noexcept
	{
       		return __builtin_bit_cast(simd_vector<T1,N1>,*this);
	}
	inline constexpr simd_vector<T,N>& operator&=(simd_vector<T,N> const& other) noexcept
	{
		return *this=(*this)&other;
	}
	inline constexpr simd_vector<T,N>& operator^=(simd_vector<T,N> const& other) noexcept
	{
		return *this=(*this)^other;
	}
	inline constexpr simd_vector<T,N>& operator|=(simd_vector<T,N> const& other) noexcept
	{
		return *this=(*this)|other;
	}
	inline constexpr simd_vector<T,N>& operator<<=(simd_vector<T,N> const& other) noexcept
	{
		return *this=(*this)<<other;
	}
	inline constexpr simd_vector<T,N>& operator>>=(simd_vector<T,N> const& other) noexcept
	{
		return *this=(*this)>>other;
	}
	inline constexpr simd_vector<T,N>& operator<<=(unsigned v) noexcept
	{
		return *this=(*this)<<v;
	}
	inline constexpr simd_vector<T,N>& operator>>=(unsigned v) noexcept
	{
		return *this=(*this)>>v;
	}
	inline constexpr simd_vector<T,N> operator~() const noexcept
	{
		constexpr bool using_simd{(sizeof(vec_type)==16&&::fast_io::details::cpu_flags::sse2_supported)||(sizeof(vec_type)==32&&::fast_io::details::cpu_flags::avx2_supported)
		||(sizeof(vec_type)==64&&(::fast_io::details::cpu_flags::avx512dq_supported||::fast_io::details::cpu_flags::avx512f_supported))};
		if constexpr(using_simd)
		{
			return (::fast_io::details::all_one_simd_vector_mask<T,N>)^(*this);
		}
		else
		{
			return ::fast_io::details::generic_simd_self_create_op_impl(*this,[](T v) noexcept
			{
				return ~v;
			});
		}
	}
#if __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
	inline constexpr void swap_endian() noexcept requires(::std::integral<value_type>&&(N*sizeof(T)==16||N*sizeof(T)==32))
	{
		if constexpr(sizeof(value_type)==1)
		{
			return;
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
			if constexpr(sizeof(vec_type)==16&&::fast_io::details::cpu_flags::sse2_supported)
			{
				if constexpr(::fast_io::details::cpu_flags::sse3_supported)
				{
					__m128i temp_vec = __builtin_bit_cast(__m128i,*this);
					__m128i mask = __builtin_bit_cast(__m128i,::fast_io::details::simd_byte_swap_shuffle_mask<sizeof(value_type),N>);
					temp_vec = _mm_shuffle_epi8(temp_vec,mask);
					*this=__builtin_bit_cast(simd_vector<T,N>,temp_vec);
					return;
				}
				else
				{
					__m128i temp_vec = __builtin_bit_cast(__m128i,*this);
					__m128i const zero{};
					if constexpr(sizeof(T)==8)
					{
						auto res0{_mm_unpacklo_epi8(temp_vec,zero)};
						auto res1{_mm_shuffle_epi32(res0,78)};
						auto res2{_mm_shufflelo_epi16(res1,27)};
						auto res3{_mm_shufflehi_epi16(res2,27)};
						auto res4{_mm_unpackhi_epi8(temp_vec,zero)};
						auto res5{_mm_shuffle_epi32(res4,78)};
						auto res6{_mm_shufflelo_epi16(res5,27)};
						auto res7{_mm_shufflehi_epi16(res6,27)};
						temp_vec=_mm_packus_epi16(res3,res7);
					}
					else if constexpr(sizeof(T)==4)
					{
						auto res0{_mm_unpacklo_epi8(temp_vec,zero)};
						auto res2{_mm_shufflelo_epi16(res0,27)};
						auto res3{_mm_shufflehi_epi16(res2,27)};
						auto res4{_mm_unpackhi_epi8(temp_vec,zero)};
						auto res6{_mm_shufflelo_epi16(res4,27)};
						auto res7{_mm_shufflehi_epi16(res6,27)};
						temp_vec=_mm_packus_epi16(res3,res7);
					}
					else if constexpr(sizeof(T)==2)
					{
						__m128i a = _mm_srli_epi16(temp_vec,8);
						__m128i b = _mm_slli_epi16(temp_vec,8);
						__m128 aa = __builtin_bit_cast(__m128,a);
						__m128 bb = __builtin_bit_cast(__m128,b);
						temp_vec=__builtin_bit_cast(__m128i,_mm_or_ps(aa,bb));
					}
					*this=__builtin_bit_cast(simd_vector<T,N>,temp_vec);
					return;
				}
			}
			else if constexpr(sizeof(vec_type)==32&&::fast_io::details::cpu_flags::avx2_supported)
			{
				__m256i temp_vec = __builtin_bit_cast(__m256i,*this);
				__m256i mask = __builtin_bit_cast(__m256i,::fast_io::details::simd_byte_swap_shuffle_mask<sizeof(value_type),N>);
				temp_vec = _mm256_shuffle_epi8(temp_vec,mask);
				*this=__builtin_bit_cast(simd_vector<T,N>,temp_vec);
				return;
			}
			else
			{
				__m512i temp_vec = __builtin_bit_cast(__m512i,*this);
				__m512i mask = __builtin_bit_cast(__m512i,::fast_io::details::simd_byte_swap_shuffle_mask<sizeof(value_type),N>);
				temp_vec = _mm512_shuffle_epi8(temp_vec,mask);
				*this=__builtin_bit_cast(simd_vector<T,N>,temp_vec);
				return;
			}
#elif defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
			if constexpr(sizeof(vec_type)==16&&::fast_io::details::cpu_flags::armneon_supported)
			{
				if constexpr(sizeof(T)==8)
				{
					poly8x16_t temp_vec = __builtin_bit_cast(poly8x16_t,*this);
					temp_vec=vrev64q_p8(temp_vec);
					*this=__builtin_bit_cast(simd_vector<T,N>,temp_vec);
				}
				else if constexpr(sizeof(T)==4)
				{
					uint32x4_t temp_vec = __builtin_bit_cast(uint32x4_t,*this);
					temp_vec=vrev32q_p8(temp_vec);
					*this=__builtin_bit_cast(simd_vector<T,N>,temp_vec);
				}
				else if constexpr(sizeof(T)==2)
				{
					uint16x8_t temp_vec = __builtin_bit_cast(uint16x8_t,*this);
					temp_vec=vrev16q_p8(temp_vec);
					*this=__builtin_bit_cast(simd_vector<T,N>,temp_vec);
				}
				return;
			}
#endif
		}
		::fast_io::details::generic_simd_self_op_impl(*this,[](T& t)
		{
			t=::fast_io::byte_swap(t);
		});
		}
	}
};

}


}
