#pragma once

#ifdef __GNUC__
#pragma GCC system_header
#endif

namespace fast_io
{

namespace intrinsics
{

inline constexpr bool simd_shuffle_size_is_supported(std::size_t n) noexcept
{
	if(n==16)
	{
#if __has_builtin(__builtin_ia32_pshufb128)
		return true;
#else
		return false;
#endif
	}
	else if(n==32)
	{
#if __has_builtin(__builtin_ia32_pshufb256)
		return true;
#else
		return false;
#endif
	}
	return false;
}

template<typename T,std::size_t N>
struct simd_vector
{
	using value_type = T;
	using vec_type [[__gnu__::__vector_size__ (N*sizeof(T))]] = T;
	vec_type value;
	constexpr T const* data() const noexcept
	{
		return __builtin_addressof(value[0]);
	}
	constexpr T* data() noexcept
	{
		return __builtin_addressof(value[0]);
	}
	inline static constexpr std::size_t size() noexcept
	{
		return N;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#endif
	inline void load(void const* address) noexcept
	{
		__builtin_memcpy(__builtin_addressof(value),address,sizeof(value));
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#endif
	inline void store(void* address) noexcept
	{
		__builtin_memcpy(address,__builtin_addressof(value),sizeof(value));
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
		value+=other.value;
		return *this;
	}
	inline constexpr simd_vector<T,N>& operator-=(simd_vector<T,N> const& other) noexcept
	{
		value-=other.value;
		return *this;
	}
	inline constexpr simd_vector<T,N>& operator*=(simd_vector<T,N> const& other) noexcept
	{
		value*=other.value;
		return *this;
	}
	inline constexpr simd_vector<T,N>& operator/=(simd_vector<T,N> const& other) noexcept
	{
		value/=other.value;
		return *this;
	}
	inline constexpr simd_vector<T,N> operator-() const noexcept
	{
		return {-value};
	}

	template<typename T2>
	requires ((sizeof(T2)==sizeof(T)*N)&&std::is_trivially_copyable_v<T2>)
	inline
#if __has_builtin(__builtin_bit_cast)
	constexpr
#endif
	void wrap_add_assign(T2 const& b) noexcept requires(std::integral<value_type>)
	{
		if constexpr(std::same_as<simd_vector<T,N>,T2>&&std::unsigned_integral<value_type>)
		{
			value+=b.value;
		}
		else
		{
			using unsigned_type = std::make_unsigned_t<T>;
			using unsigned_vec_type = typename simd_vector<unsigned_type,N>::vec_type;
#if __has_builtin(__builtin_bit_cast)
			this->value=__builtin_bit_cast(vec_type,__builtin_bit_cast(unsigned_vec_type,this->value)+__builtin_bit_cast(unsigned_vec_type,b));
#else
			unsigned_vec_type asv;
			__builtin_memcpy(__builtin_addressof(asv),this,sizeof(unsigned_vec_type));
			unsigned_vec_type bsv;
			__builtin_memcpy(__builtin_addressof(bsv),__builtin_addressof(b),sizeof(unsigned_vec_type));
			asv+=bsv;
			__builtin_memcpy(this,__builtin_addressof(asv),sizeof(unsigned_vec_type));
#endif
		}
	}

	template<typename T2>
	requires ((sizeof(T2)==sizeof(T)*N)&&std::is_trivially_copyable_v<T2>)
	inline
#if __has_builtin(__builtin_bit_cast)
	constexpr
#endif
	void wrap_sub_assign(T2 const& b) noexcept requires(std::integral<value_type>)
	{
		if constexpr(std::same_as<simd_vector<T,N>,T2>&&std::unsigned_integral<value_type>)
		{
			value-=b.value;
		}
		else
		{
			using unsigned_type = std::make_unsigned_t<T>;
			using unsigned_vec_type = typename simd_vector<unsigned_type,N>::vec_type;
#if __has_builtin(__builtin_bit_cast)
			this->value=__builtin_bit_cast(vec_type,__builtin_bit_cast(unsigned_vec_type,this->value)-__builtin_bit_cast(unsigned_vec_type,b));
#else
			unsigned_vec_type asv;
			__builtin_memcpy(__builtin_addressof(asv),this,sizeof(unsigned_vec_type));
			unsigned_vec_type bsv;
			__builtin_memcpy(__builtin_addressof(bsv),__builtin_addressof(b),sizeof(unsigned_vec_type));
			asv-=bsv;
			__builtin_memcpy(this,__builtin_addressof(asv),sizeof(unsigned_vec_type));
#endif
		}
	}
	inline constexpr simd_vector<T,N>& operator&=(simd_vector<T,N> const& other) noexcept
	{
		value&=other.value;
		return *this;
	}
	inline constexpr simd_vector<T,N>& operator^=(simd_vector<T,N> const& other) noexcept
	{
		value^=other.value;
		return *this;
	}
	inline constexpr simd_vector<T,N>& operator|=(simd_vector<T,N> const& other) noexcept
	{
		value|=other.value;
		return *this;
	}
	inline constexpr simd_vector<T,N>& operator<<=(simd_vector<T,N> const& other) noexcept
	{
		value<<=other.value;
		return *this;
	}
	inline constexpr simd_vector<T,N>& operator>>=(simd_vector<T,N> const& other) noexcept
	{
		value<<=other.value;
		return *this;
	}

	template<std::integral I>
	inline constexpr simd_vector<T,N>& operator<<=(I other) noexcept
	{
		value<<=other;
		return *this;
	}
	template<std::integral I>
	inline constexpr simd_vector<T,N>& operator>>=(I other) noexcept
	{
		value>>=other;
		return *this;
	}
	inline constexpr simd_vector<T,N> operator~() const noexcept
	{
		return {~value};
	}
	template<typename T1,std::size_t N1>
	requires (sizeof(T1)*N1==sizeof(T)*N&&!std::same_as<T1,value_type>)
	explicit
#if __has_builtin(__builtin_bit_cast)
	constexpr
#endif
	operator simd_vector<T1,N1>() const noexcept
	{
#if __has_builtin(__builtin_bit_cast)
        return __builtin_bit_cast(simd_vector<T1,N1>,*this);
#else
        simd_vector<T1,N1> v;
        __builtin_memcpy(__builtin_addressof(v),this,sizeof(v));
        return v;
#endif
	}
	constexpr operator vec_type const&() const noexcept
	{
		return value;
	}
	inline constexpr simd_vector& operator=(vec_type const& b) noexcept
	{
		value=b;
		return *this;
	}
	inline
#if __has_builtin(__builtin_bit_cast)
	constexpr
#endif
	void swap_endian() noexcept requires(::std::integral<value_type>&&(sizeof(value_type)<=sizeof(std::uint_least64_t))&&(N*sizeof(T)==16||N*sizeof(T)==32||N*sizeof(T)==64))
	{
		if constexpr(sizeof(T)==1)
		{
			return;
		}
		else if constexpr(N*sizeof(T)==64)
		{
			using temp_vec_type [[__gnu__::__vector_size__ (64)]] = char;
#if __has_builtin(__builtin_bit_cast)
			auto temp_vec{__builtin_bit_cast(temp_vec_type,this->value)};
#else
			temp_vec_type temp_vec;
			__builtin_memcpy(__builtin_addressof(temp_vec),__builtin_addressof(this->value),sizeof(vec_type));
#endif

#if __has_builtin(__builtin_shufflevector)
			if constexpr(sizeof(T)==8)
			{
				temp_vec=__builtin_shufflevector(temp_vec,temp_vec,
					7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8,
					23,22,21,20,19,18,17,16,31,30,29,28,27,26,25,24,
					39,38,37,36,35,34,33,32,47,46,45,44,43,42,41,40,
					55,54,53,52,51,50,49,48,63,62,61,60,59,58,57,56);
			}
			else if constexpr(sizeof(T)==4)
			{
				temp_vec=__builtin_shufflevector(temp_vec,temp_vec,
					3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12,
					19,18,17,16,23,22,21,20,27,26,25,24,31,30,29,28,
					35,34,33,32,39,38,37,36,43,42,41,40,47,46,45,44,
					51,50,49,48,55,54,53,52,59,58,57,56,63,62,61,60);
			}
			else if constexpr(sizeof(T)==2)
			{
				temp_vec=__builtin_shufflevector(temp_vec,temp_vec,
					1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14,
					17,16,19,18,21,20,23,22,25,24,27,26,29,28,31,30,
					33,32,35,34,37,36,39,38,41,40,43,42,45,44,47,46,
					49,48,51,50,53,52,55,54,57,56,59,58,61,60,63,62);
			}
#elif __has_builtin(__builtin_ia32_pshufb512)
			if constexpr(sizeof(T)==8)
			{
				constexpr temp_vec_type mask{7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8,
					23,22,21,20,19,18,17,16,31,30,29,28,27,26,25,24,
					39,38,37,36,35,34,33,32,47,46,45,44,43,42,41,40,
					55,54,53,52,51,50,49,48,63,62,61,60,59,58,57,56};
				temp_vec=__builtin_ia32_pshufb512(temp_vec,mask);
			}
			else if constexpr(sizeof(T)==4)
			{
				constexpr temp_vec_type mask{3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12,
					19,18,17,16,23,22,21,20,27,26,25,24,31,30,29,28,
					35,34,33,32,39,38,37,36,43,42,41,40,47,46,45,44,
					51,50,49,48,55,54,53,52,59,58,57,56,63,62,61,60};
				temp_vec=__builtin_ia32_pshufb512(temp_vec,mask);
			}
			else if constexpr(sizeof(T)==2)
			{
				constexpr temp_vec_type mask{1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14,
					17,16,19,18,21,20,23,22,25,24,27,26,29,28,31,30,
					33,32,35,34,37,36,39,38,41,40,43,42,45,44,47,46,
					49,48,51,50,53,52,55,54,57,56,59,58,61,60,63,62};
				temp_vec=__builtin_ia32_pshufb512(temp_vec,mask);
			}
#endif
#if __has_builtin(__builtin_bit_cast)
			this->value=__builtin_bit_cast(vec_type,temp_vec);
#else
			__builtin_memcpy(__builtin_addressof(this->value),__builtin_addressof(temp_vec),sizeof(vec_type));
#endif
		}
		else if constexpr(N*sizeof(T)==32)
		{
			using temp_vec_type [[__gnu__::__vector_size__ (32)]] = char;
#if __has_builtin(__builtin_bit_cast)
			auto temp_vec{__builtin_bit_cast(temp_vec_type,this->value)};
#else
			temp_vec_type temp_vec;
			__builtin_memcpy(__builtin_addressof(temp_vec),__builtin_addressof(this->value),sizeof(vec_type));
#endif
#if __has_builtin(__builtin_shufflevector)
			if constexpr(sizeof(T)==8)
			{
				temp_vec=__builtin_shufflevector(temp_vec,temp_vec,
					7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8,
					23,22,21,20,19,18,17,16,31,30,29,28,27,26,25,24);
			}
			else if constexpr(sizeof(T)==4)
			{
				temp_vec=__builtin_shufflevector(temp_vec,temp_vec,
					3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12,
					19,18,17,16,23,22,21,20,27,26,25,24,31,30,29,28);
			}
			else if constexpr(sizeof(T)==2)
			{
				temp_vec=__builtin_shufflevector(temp_vec,temp_vec,
					1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14,
					17,16,19,18,21,20,23,22,25,24,27,26,29,28,31,30);
			}
#elif __has_builtin(__builtin_ia32_pshufb256)
			if constexpr(sizeof(T)==8)
			{
				constexpr temp_vec_type mask{7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8,
					23,22,21,20,19,18,17,16,31,30,29,28,27,26,25,24};
				temp_vec=__builtin_ia32_pshufb256(temp_vec,mask);
			}
			else if constexpr(sizeof(T)==4)
			{
				constexpr temp_vec_type mask{3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12,
					19,18,17,16,23,22,21,20,27,26,25,24,31,30,29,28};
				temp_vec=__builtin_ia32_pshufb256(temp_vec,mask);
			}
			else if constexpr(sizeof(T)==2)
			{
				constexpr temp_vec_type mask{1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14,
					17,16,19,18,21,20,23,22,25,24,27,26,29,28,31,30};
				temp_vec=__builtin_ia32_pshufb256(temp_vec,mask);
			}
#endif
#if __has_builtin(__builtin_bit_cast)
			this->value=__builtin_bit_cast(vec_type,temp_vec);
#else
			__builtin_memcpy(__builtin_addressof(this->value),__builtin_addressof(temp_vec),sizeof(vec_type));
#endif
		}
		else
		{
			using temp_vec_type [[__gnu__::__vector_size__ (16)]] = char;
#if __has_builtin(__builtin_bit_cast)
			auto temp_vec{__builtin_bit_cast(temp_vec_type,this->value)};
#else
			temp_vec_type temp_vec;
			__builtin_memcpy(__builtin_addressof(temp_vec),__builtin_addressof(this->value),sizeof(vec_type));
#endif
#if __has_builtin(__builtin_shufflevector) && ((!defined(__x86_64__)&&!defined(__i386__))||(!defined(__GNUC__)||defined(__clang__))||defined(__SSE4_2__))
			if constexpr(sizeof(T)==8)
			{
				temp_vec=__builtin_shufflevector(temp_vec,temp_vec,7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8);
			}
			else if constexpr(sizeof(T)==4)
			{
				temp_vec=__builtin_shufflevector(temp_vec,temp_vec,3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12);
			}
			else if constexpr(sizeof(T)==2)
			{
				temp_vec=__builtin_shufflevector(temp_vec,temp_vec,1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14);
			}
#elif __has_builtin(__builtin_ia32_pshufb128) && defined(__SSE3__)
			if constexpr(sizeof(T)==8)
			{
				constexpr temp_vec_type mask{7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8};
				temp_vec=__builtin_ia32_pshufb128(temp_vec,mask);
			}
			else if constexpr(sizeof(T)==4)
			{
				constexpr temp_vec_type mask{3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12};
				temp_vec=__builtin_ia32_pshufb128(temp_vec,mask);
			}
			else if constexpr(sizeof(T)==2)
			{
				constexpr temp_vec_type mask{1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14};
				temp_vec=__builtin_ia32_pshufb128(temp_vec,mask);
			}
#elif defined(__SSE2__) && __has_builtin(__builtin_ia32_punpcklbw128) && __has_builtin(__builtin_ia32_punpckhbw128)
			using x86_64_v4si [[__gnu__::__vector_size__ (16)]] = int;
			using x86_64_v16qi [[__gnu__::__vector_size__ (16)]] = char;
			using x86_64_v8hi [[__gnu__::__vector_size__ (16)]] = short;
			constexpr x86_64_v16qi zero{};
			if constexpr(sizeof(T)==8)
			{
				auto res0{__builtin_ia32_punpcklbw128(temp_vec,zero)};
				auto res1{__builtin_ia32_pshufd((x86_64_v4si)res0,78)};
				auto res2{__builtin_ia32_pshuflw((x86_64_v8hi)res1,27)};
				auto res3{__builtin_ia32_pshufhw(res2,27)};
				auto res4{__builtin_ia32_punpckhbw128(temp_vec,zero)};
				auto res5{__builtin_ia32_pshufd((x86_64_v4si)res4,78)};
				auto res6{__builtin_ia32_pshuflw((x86_64_v8hi)res5,27)};
				auto res7{__builtin_ia32_pshufhw(res6,27)};
				temp_vec=__builtin_ia32_packuswb128(res3,res7);
			}
			else if constexpr(sizeof(T)==4)
			{
				auto res0{__builtin_ia32_punpcklbw128(temp_vec,zero)};
				auto res2{__builtin_ia32_pshuflw((x86_64_v8hi)res0,27)};
				auto res3{__builtin_ia32_pshufhw(res2,27)};
				auto res4{__builtin_ia32_punpckhbw128(temp_vec,zero)};
				auto res6{__builtin_ia32_pshuflw((x86_64_v8hi)res4,27)};
				auto res7{__builtin_ia32_pshufhw(res6,27)};
				temp_vec=__builtin_ia32_packuswb128(res3,res7);
			}
			else if constexpr(sizeof(T)==2)
			{
				using x86_64_v8hu [[__gnu__::__vector_size__ (16)]] = unsigned short;
				auto res0{(x86_64_v8hu)temp_vec};
				temp_vec=(x86_64_v16qi)((res0>>8)|(res0<<8));
			}
#endif
#if __has_builtin(__builtin_bit_cast)
			this->value=__builtin_bit_cast(vec_type,temp_vec);
#else
			__builtin_memcpy(__builtin_addressof(this->value),__builtin_addressof(temp_vec),sizeof(vec_type));
#endif
		}
	}
	template<std::integral T1,std::size_t N1>
	requires (simd_shuffle_size_is_supported(sizeof(vec_type))&&sizeof(simd_vector<T1,N1>)==sizeof(vec_type)&&sizeof(T1)==1)
	inline constexpr void shuffle([[maybe_unused]] simd_vector<T1,N1> const& mask) noexcept
	{
		if constexpr(sizeof(mask)==16)
		{
#if __has_builtin(__builtin_ia32_pshufb128)
			using value_type2 [[__gnu__::__vector_size__ (16)]] = char;
#if __has_builtin(__builtin_bit_cast)
			value=__builtin_bit_cast(vec_type,__builtin_ia32_pshufb128(__builtin_bit_cast(value_type2,value),mask.value));
#else
			value_type2 v;
			__builtin_memcpy(__builtin_addressof(v),__builtin_addressof(mask),sizeof(value_type2));
			v=__builtin_ia32_pshufb128(v,mask.value);
			__builtin_memcpy(__builtin_addressof(value),__builtin_addressof(v),sizeof(value_type2));
#endif
#endif
		}
		else if constexpr(sizeof(mask)==32)
		{
#if __has_builtin(__builtin_ia32_pshufb256)
			using value_type2 [[__gnu__::__vector_size__ (32)]] = char;
#if __has_builtin(__builtin_bit_cast)
			value=__builtin_bit_cast(vec_type,__builtin_ia32_pshufb256(__builtin_bit_cast(value_type2,value),mask.value));
#else
			value_type2 v;
			__builtin_memcpy(__builtin_addressof(v),__builtin_addressof(mask),sizeof(value_type2));
			v=__builtin_ia32_pshufb256(v,mask.value);
			__builtin_memcpy(__builtin_addressof(value),__builtin_addressof(v),sizeof(value_type2));
#endif
#endif
		}
	}
};

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator+(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
	return {a.value+b.value};
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator-(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
	return {a.value-b.value};
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator*(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
	return {a.value*b.value};
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator/(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
	return {a.value/b.value};
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator&(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
	return {a.value&b.value};
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator|(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
	return {a.value|b.value};
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator^(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
	return {a.value^b.value};
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator<<(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
	return {a.value<<b.value};
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator>>(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
	return {a.value>>b.value};
}

template<typename T,std::size_t N,std::integral I>
inline constexpr simd_vector<T,N> operator<<(simd_vector<T,N> const& a,I i) noexcept
{
	return {a.value<<i};
}

template<typename T,std::size_t N,std::integral I>
inline constexpr simd_vector<T,N> operator>>(simd_vector<T,N> const& a,I i) noexcept
{
	return {a.value>>i};
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator<(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
	return {a.value<b.value};
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator>(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
	return {a.value>b.value};
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator<=(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
	return {a.value<=b.value};
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator>=(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
	return {a.value>=b.value};
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator==(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
	return {a.value==b.value};
}

template<typename T,std::size_t N>
inline constexpr simd_vector<T,N> operator!=(simd_vector<T,N> const& a,simd_vector<T,N> const& b) noexcept
{
	return {a.value!=b.value};
}

template<std::integral T,std::size_t N>
inline
#if __has_builtin(__builtin_bit_cast)
constexpr
#endif
simd_vector<T,N> wrap_add(simd_vector<T,N> a,simd_vector<T,N> b) noexcept
{
	if constexpr(std::signed_integral<T>)
	{
		using unsigned_type = std::make_unsigned_t<T>;
		using vec_type = typename simd_vector<unsigned_type,N>::vec_type;
#if __has_builtin(__builtin_bit_cast)
		return __builtin_bit_cast(simd_vector<T,N>,__builtin_bit_cast(vec_type,a)+__builtin_bit_cast(vec_type,b));
#else
		vec_type asv;
		__builtin_memcpy(__builtin_addressof(asv),__builtin_addressof(a),sizeof(asv));
		vec_type bsv;
		__builtin_memcpy(__builtin_addressof(bsv),__builtin_addressof(b),sizeof(bsv));
		asv+=bsv;
		simd_vector<T,N> res;
		__builtin_memcpy(__builtin_addressof(res),__builtin_addressof(asv),sizeof(asv));
		return res;
#endif
	}
	else
	{
		return a+b;
	}
}

template<std::integral T,std::size_t N>
inline
#if __has_builtin(__builtin_bit_cast)
constexpr
#endif
simd_vector<T,N> wrap_sub(simd_vector<T,N> a,simd_vector<T,N> b) noexcept
{
	if constexpr(std::signed_integral<T>)
	{
		using unsigned_type = std::make_unsigned_t<T>;
		using vec_type = typename simd_vector<unsigned_type,N>::vec_type;
#if __has_builtin(__builtin_bit_cast)
		return __builtin_bit_cast(simd_vector<T,N>,__builtin_bit_cast(vec_type,a)-__builtin_bit_cast(vec_type,b));
#else
		vec_type asv;
		__builtin_memcpy(__builtin_addressof(asv),__builtin_addressof(a),sizeof(asv));
		vec_type bsv;
		__builtin_memcpy(__builtin_addressof(bsv),__builtin_addressof(b),sizeof(bsv));
		asv-=bsv;
		simd_vector<T,N> res;
		__builtin_memcpy(__builtin_addressof(res),__builtin_addressof(asv),sizeof(asv));
		return res;
#endif
	}
	else
	{
		return a-b;
	}
}

}

}
