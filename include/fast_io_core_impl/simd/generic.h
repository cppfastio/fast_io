#pragma once

namespace fast_io
{

namespace intrinsics
{

template<typename T,std::size_t N>
struct
#if defined(_MSC_VER)
__declspec(intrin_type) __declspec(align(sizeof(T)*N/2))
#endif
simd_vector
{
	using value_type = T;
#if __has_cpp_attribute(__gnu__::__vector_size__)
	using vec_type [[__gnu__::__vector_size__ (N*sizeof(T))]] = T;
#else
	using vec_type = value_type[N];
#endif
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
		constexpr simd_vector<T,N> empty{};
#if 0
		return ::fast_io::details::generic_simd_self_create_op_impl(*this,[](T v)
		{
			return -v;
		});
#endif
		return ::fast_io::details::wrap_minus_common(empty,*this);
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
	inline constexpr simd_vector<T,N> operator~() const noexcept
	{
		return ::fast_io::details::generic_simd_self_create_op_impl(*this,[](T v) noexcept
		{
			return ~v;
		});
	}
	inline constexpr void swap_endian() noexcept requires(::std::integral<value_type>)
	{
		::fast_io::details::generic_simd_self_op_impl(*this,[](T& t)
		{
			t=::fast_io::byte_swap(t);
		});
	}
};

}


}
