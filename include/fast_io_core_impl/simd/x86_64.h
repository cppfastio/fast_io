#pragma once

#ifdef __GNUC__
#pragma GCC system_header
#endif

namespace fast_io
{


namespace intrinsics
{
using x86_64_m128 [[gnu::__vector_size__(16),gnu::may_alias]] = float ;
using x86_64_m128i [[gnu::__vector_size__ (16),gnu::may_alias]] = long long;
using x86_64_v16qi [[gnu::__vector_size__ (16)]] = char;
using x86_64_v16qs [[gnu::__vector_size__ (16)]] = char signed;
using x86_64_v16qu [[gnu::__vector_size__ (16)]] = char unsigned;
using x86_64_v8hi [[gnu::__vector_size__ (16)]] = short;
using x86_64_v8hu [[gnu::__vector_size__ (16)]] = short unsigned;
using x86_64_v4si [[gnu::__vector_size__ (16)]] = int;
using x86_64_v4su [[gnu::__vector_size__ (16)]] = int unsigned;

using x86_64_m256 [[gnu::__vector_size__(32),gnu::may_alias]]  = float;
using x86_64_m256i [[gnu::__vector_size__ (32),gnu::may_alias]] = long long;
using x86_64_m512 [[gnu::__vector_size__(64),gnu::may_alias]]  = float ;
using x86_64_m512i [[gnu::__vector_size__ (64),gnu::may_alias]] = long long;
using x86_64_m1024 [[gnu::__vector_size__(128),gnu::may_alias]]  = float ;
using x86_64_m1024i [[gnu::__vector_size__ (128),gnu::may_alias]] = long long;
using x86_64_m2048 [[gnu::__vector_size__(256),gnu::may_alias]]  = float ;
using x86_64_m2048i [[gnu::__vector_size__ (256),gnu::may_alias]] = long long;
using x86_64_m4096 [[gnu::__vector_size__(512),gnu::may_alias]]  = float ;
using x86_64_m4096i [[gnu::__vector_size__ (512),gnu::may_alias]] = long long;


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
	using vec_type [[gnu::__vector_size__ (N*sizeof(T))]] = T;
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
#if __has_cpp_attribute(gnu::always_inline)
	[[gnu::always_inline]]
#endif
	inline void load(void const* address) noexcept
	{
		__builtin_memcpy(__builtin_addressof(value),address,sizeof(value));
	}
#if __has_cpp_attribute(gnu::always_inline)
	[[gnu::always_inline]]
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
		return value[N-1];
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
	inline constexpr simd_vector<T,N> operator~() const noexcept
	{
		return {~value};
	}
	template<typename T1,std::size_t N1>
	requires (sizeof(T1)*N1==sizeof(T)*N&&N!=N1)
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
#if __has_builtin(__builtin_ia32_pshufb128) || __has_builtin(__builtin_shufflevector)
	inline
#if __has_builtin(__builtin_bit_cast)
	constexpr
#endif
	void swap_endian() noexcept requires(::std::integral<value_type>&&(N*sizeof(T)==16||N*sizeof(T)==32))
	{
		if constexpr(sizeof(T)!=1)
		{
			if constexpr(N*sizeof(T)==32)
			{
				using temp_vec_type [[gnu::__vector_size__ (32)]] = char;
#if __has_builtin(__builtin_bit_cast)
				auto temp_vec{__builtin_bit_cast(temp_vec_type,this->value)};
#else
				temp_vec_type temp_vec;
				__builtin_memcpy(__builtin_addressof(temp_vec),__builtin_addressof(this->value),sizeof(vec_type));
#endif
#if __has_builtin(__builtin_shufflevector)
				if constexpr(sizeof(T)==16)
				{
					temp_vec=__builtin_shufflevector(temp_vec,temp_vec,
						15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,
						31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16);
				}
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
				if constexpr(sizeof(T)==16)
				{
					constexpr temp_vec_type mask{15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,
						31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16};
					temp_vec=__builtin_ia32_pshufb256(temp_vec,mask);
				}
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
				using temp_vec_type [[gnu::__vector_size__ (16)]] = char;
#if __has_builtin(__builtin_bit_cast)
				auto temp_vec{__builtin_bit_cast(temp_vec_type,this->value)};
#else
				temp_vec_type temp_vec;
				__builtin_memcpy(__builtin_addressof(temp_vec),__builtin_addressof(this->value),sizeof(vec_type));
#endif
#if __has_builtin(__builtin_shufflevector)
				if constexpr(sizeof(T)==16)
				{
					temp_vec=__builtin_shufflevector(temp_vec,temp_vec,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0);
				}
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
#else
				if constexpr(sizeof(T)==16)
				{
					constexpr temp_vec_type mask{15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0};
					temp_vec=__builtin_ia32_pshufb128(temp_vec,mask);
				}
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
#endif
#if __has_builtin(__builtin_bit_cast)
				this->value=__builtin_bit_cast(vec_type,temp_vec);
#else
				__builtin_memcpy(__builtin_addressof(this->value),__builtin_addressof(temp_vec),sizeof(vec_type));
#endif
			}
		}
	}
#endif
	template<std::integral T1,std::size_t N1>
	requires (simd_shuffle_size_is_supported(sizeof(vec_type))&&sizeof(simd_vector<T1,N1>)==sizeof(vec_type)&&sizeof(T1)==1)
	inline constexpr void shuffle([[maybe_unused]] simd_vector<T1,N1> const& mask) noexcept
	{
		if constexpr(sizeof(mask)==16)
		{
#if __has_builtin(__builtin_ia32_pshufb128)
			using value_type2 [[gnu::__vector_size__ (16)]] = char;
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
			using value_type2 [[gnu::__vector_size__ (32)]] = char;
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

}

namespace details
{

template<::fast_io::freestanding::forward_iterator Iter,typename T,std::size_t size>
inline constexpr Iter print_reserve_df_mm_impl(Iter iter,::fast_io::intrinsics::simd_vector<T,size> const& mm) noexcept
{
	if constexpr(size==0)
		return iter;
	using char_type = ::fast_io::freestanding::iter_value_t<Iter>;
	*iter=char_literal_v<u8'(',char_type>;
	++iter;
	iter=print_reserve_define(io_reserve_type<char_type,T>,iter,mm[0]);
	for(std::size_t i{1};i!=size;++i)
	{
		*iter=char_literal_v<u8',',char_type>;
		++iter;
		iter=print_reserve_define(io_reserve_type<char_type,T>,iter,mm[i]);
	}
	*iter=char_literal_v<u8')',char_type>;
	++iter;
	return iter;
}

}


template<std::integral char_type,typename T,std::size_t n>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,intrinsics::simd_vector<T,n>>) noexcept
{
	constexpr std::size_t p{print_reserve_size(io_reserve_type<char_type,T>)};
	constexpr std::size_t sz{(1+p)*n+1};
	return sz;
}
template<::fast_io::freestanding::forward_iterator Iter,typename T,std::size_t n>
inline constexpr Iter print_reserve_define(io_reserve_type_t<::fast_io::freestanding::iter_value_t<Iter>,intrinsics::simd_vector<T,n>>,Iter iter,intrinsics::simd_vector<T,n> const& t) noexcept
{
	return details::print_reserve_df_mm_impl(iter,t);
}

}