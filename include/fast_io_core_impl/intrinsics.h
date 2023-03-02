#pragma once
#if defined(_MSC_VER) && !defined(__clang__)
#include <intrin.h>
#pragma warning(push)
#pragma warning( disable : 4668 )
#pragma warning( disable : 4800 )
#endif

namespace fast_io::details::intrinsics
{

template<typename T1,typename T2>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_lib_bit_cast >= 201806L
constexpr
#endif
void typed_memcpy(T1 *dest, T2 const *src,::std::size_t bytes) noexcept
{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_lib_bit_cast >= 201806L
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if (__builtin_is_constant_evaluated())
#endif
	{
		if(dest == nullptr || src == nullptr || sizeof(T1)!=sizeof(T2) || bytes%sizeof(T1)!=0)
		{
#if defined(__has_builtin)
#if __has_builtin(__builtin_trap)
			__builtin_trap();
#elif __has_builtin(__builtin_abort)
			__builtin_abort();
#else
			::std::abort();
#endif
#else
			::std::abort();
#endif
		}
		::std::size_t n{bytes/sizeof(T1)};
		for(::std::size_t i{};i!=n;++i)
		{
			dest[i] = __builtin_bit_cast(T1,src[i]);
		}
	}
	else
#endif
	{
#if defined(__has_builtin)
#if __has_builtin(__builtin_memcpy)
		__builtin_memcpy(dest,src,bytes);
#else
		memcpy(dest,src,bytes);
#endif
#else
		memcpy(dest,src,bytes);
#endif
	}
}

template<typename T>
#if __cpp_lib_concepts >= 202002L
requires (std::unsigned_integral<T>)
#endif
inline constexpr bool add_carry_naive(bool carry,T a,T b,T& out) noexcept
{
	T temp{carry+a};
	out=temp+b;
	return (out < b) | (temp < a);
}

template<typename T>
#if __cpp_lib_concepts >= 202002L
requires (std::unsigned_integral<T>)
#endif
inline constexpr bool add_carry(bool carry,T a,T b,T& out) noexcept
{
#if __cpp_if_consteval >= 202106L
	if consteval
	{
		return add_carry_naive(carry,a,b,out);
	}
	else
#elif __cpp_lib_is_constant_evaluated >= 201811L
	if(std::is_constant_evaluated())
		return add_carry_naive(carry,a,b,out);
	else
#endif
	{
#if defined(_MSC_VER) && !defined(__clang__)
#if (defined(_M_IX86) || defined(_M_AMD64))
	if constexpr(sizeof(T)==8)
	{
#if defined(_M_AMD64)
		return _addcarry_u64(carry,a,b,reinterpret_cast<std::uint_least64_t*>(__builtin_addressof(out)));
#else
		return _addcarry_u32(_addcarry_u32(carry,
		*reinterpret_cast<std::uint_least32_t*>(__builtin_addressof(a)),*reinterpret_cast<std::uint_least32_t*>(__builtin_addressof(b)),reinterpret_cast<std::uint_least32_t*>(__builtin_addressof(out))),
		reinterpret_cast<std::uint_least32_t*>(__builtin_addressof(a))[1],reinterpret_cast<std::uint_least32_t*>(__builtin_addressof(b))[1],reinterpret_cast<std::uint_least32_t*>(__builtin_addressof(out))+1);
#endif
	}
	else if constexpr(sizeof(T)==4)
		return _addcarry_u32(carry,a,b,reinterpret_cast<std::uint_least32_t*>(__builtin_addressof(out)));
	else if constexpr(sizeof(T)==2)
		return _addcarry_u16(carry,a,b,reinterpret_cast<short unsigned*>(__builtin_addressof(out)));
	else if constexpr(sizeof(T)==1)
		return _addcarry_u8(carry,a,b,reinterpret_cast<char unsigned*>(__builtin_addressof(out)));
	else
		return add_carry_naive(carry,a,b,out);
#else
		return add_carry_naive(carry,a,b,out);
#endif
#elif defined(__has_builtin) && (__has_builtin(__builtin_addcb)&&__has_builtin(__builtin_addcs)&&__has_builtin(__builtin_addc)&&__has_builtin(__builtin_addcl)&&__has_builtin(__builtin_addcll))
	if constexpr(sizeof(T)==sizeof(long long unsigned))
	{
		long long unsigned carryout;
		out=__builtin_addcll(a,b,carry,__builtin_addressof(carryout));
		return carryout;
	}
	else if constexpr(sizeof(T)==sizeof(long unsigned))
	{
		long unsigned carryout;
		out=__builtin_addcl(a,b,carry,__builtin_addressof(carryout));
		return carryout;
	}
	else if constexpr(sizeof(T)==sizeof(unsigned))
	{
		unsigned carryout;
		out=__builtin_addc(a,b,carry,__builtin_addressof(carryout));
		return carryout;
	}
	else if constexpr(sizeof(T)==sizeof(short unsigned))
	{
		short unsigned carryout;
		out=__builtin_addcs(a,b,carry,__builtin_addressof(carryout));
		return carryout;
	}
	else if constexpr(sizeof(T)==sizeof(char unsigned))
	{
		char unsigned carryout;
		out=__builtin_addcb(a,b,carry,__builtin_addressof(carryout));
		return carryout;
	}
	else
	{
		return add_carry_naive(carry,a,b,out);
	}
#elif defined(__has_builtin) && (__has_builtin(__builtin_ia32_addcarryx_u32)||__has_builtin(__builtin_ia32_addcarry_u32)||__has_builtin(__builtin_ia32_addcarryx_u64))
	if constexpr(sizeof(T)==8)
	{
#if __has_builtin(__builtin_ia32_addcarryx_u64)
		using may_alias_ptr_type
		#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
		#endif
		= unsigned long long*;
		return __builtin_ia32_addcarryx_u64(carry,a,b,reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out)));
#elif __has_builtin(__builtin_ia32_addcarry_u64)
		using may_alias_ptr_type
		#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
		#endif
		= unsigned long long*;
		return __builtin_ia32_addcarry_u64(carry,a,b,reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out)));
#else
		std::uint_least32_t a_low;
		std::uint_least32_t a_high;
		__builtin_memcpy(__builtin_addressof(a_low),__builtin_addressof(a),4);
		__builtin_memcpy(__builtin_addressof(a_high),reinterpret_cast<char const*>(__builtin_addressof(a))+4,4);
		std::uint_least32_t b_low;
		std::uint_least32_t b_high;
		__builtin_memcpy(__builtin_addressof(b_low),__builtin_addressof(b),4);
		__builtin_memcpy(__builtin_addressof(b_high),reinterpret_cast<char const*>(__builtin_addressof(b))+4,4);
		using may_alias_ptr_type
		#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
		#endif
		= unsigned*;
	#if __has_builtin(__builtin_ia32_addcarryx_u32)
		return __builtin_ia32_addcarryx_u32(__builtin_ia32_addcarryx_u32(carry,a_low,b_low,reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out))),
		a_high,b_high,reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out))+1);
	#elif __has_builtin(__builtin_ia32_addcarry_u32)
		return __builtin_ia32_addcarry_u32(__builtin_ia32_addcarry_u32(carry,a_low,b_low,reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out))),
		a_high,b_high,reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out))+1);
	#else
		return add_carry_naive(carry,a,b,out);
	#endif
#endif
	}
	else if constexpr(sizeof(T)==4)
	{
		using may_alias_ptr_type
		#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
		#endif
		= unsigned*;
#if __has_builtin(__builtin_ia32_addcarryx_u32)
		return __builtin_ia32_addcarryx_u32(carry,a,b,reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out)));
#elif __has_builtin(__builtin_ia32_addcarry_u32)
		return __builtin_ia32_addcarry_u32(carry,a,b,reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out)));
#else
		return add_carry_naive(carry,a,b,out);
#endif
	}
	else
		return add_carry_naive(carry,a,b,out);	//16 bit addcarry simply does not exist on gcc and clang
#else
	return add_carry_naive(carry,a,b,out);
#endif
	}
}

template<typename T>
#if __cpp_lib_concepts >= 202002L
requires (std::unsigned_integral<T>)
#endif
inline constexpr bool sub_borrow_naive(bool carry,T a,T b,T& out) noexcept
{
	T temp{a-carry};
	out=temp-b;
	return (temp<out)|(a<temp);
}

template<typename T>
#if __cpp_lib_concepts >= 202002L
requires (std::unsigned_integral<T>)
#endif
inline constexpr bool sub_borrow(bool borrow,T a,T b,T& out) noexcept
{
#if __cpp_if_consteval >= 202106L
	if consteval
	{
		return sub_borrow_naive(borrow,a,b,out);
	}
#elif __cpp_lib_is_constant_evaluated >= 201811L
	if(std::is_constant_evaluated())
		return sub_borrow_naive(borrow,a,b,out);
	else
#endif
	{
#if defined(_MSC_VER) && !defined(__clang__)
#if (defined(_M_IX86) || defined(_M_AMD64))
	if constexpr(sizeof(T)==8)
	{
#if defined(_M_AMD64)
		return _subborrow_u64(borrow,a,b,reinterpret_cast<std::uint_least64_t*>(__builtin_addressof(out)));
#else
		return _subborrow_u32(_subborrow_u32(borrow,
		*reinterpret_cast<std::uint_least32_t*>(__builtin_addressof(a)),*reinterpret_cast<std::uint_least32_t*>(__builtin_addressof(b)),reinterpret_cast<std::uint_least32_t*>(__builtin_addressof(out))),
		reinterpret_cast<std::uint_least32_t*>(__builtin_addressof(a))[1],reinterpret_cast<std::uint_least32_t*>(__builtin_addressof(b))[1],reinterpret_cast<std::uint_least32_t*>(__builtin_addressof(out))+1);
#endif
	}
	else if constexpr(sizeof(T)==4)
		return _subborrow_u32(borrow,a,b,reinterpret_cast<std::uint_least32_t*>(__builtin_addressof(out)));
	else if constexpr(sizeof(T)==2)
		return _subborrow_u16(borrow,a,b,reinterpret_cast<short unsigned*>(__builtin_addressof(out)));
	else if constexpr(sizeof(T)==1)
		return _subborrow_u8(borrow,a,b,reinterpret_cast<char unsigned*>(__builtin_addressof(out)));
	else
		return sub_borrow_naive(borrow,a,b,out);
#else
		return sub_borrow_naive(borrow,a,b,out);
#endif
#elif defined(__has_builtin) && (__has_builtin(__builtin_subcb)&&__has_builtin(__builtin_subcs)&&__has_builtin(__builtin_subc)&&__has_builtin(__builtin_subcl)&&__has_builtin(__builtin_subcll))
	if constexpr(sizeof(T)==sizeof(long long unsigned))
	{
		long long unsigned borrowout;
		out=__builtin_subcll(a,b,borrow,__builtin_addressof(borrowout));
		return borrowout;
	}
	else if constexpr(sizeof(T)==sizeof(long unsigned))
	{
		long unsigned borrowout;
		out=__builtin_subcl(a,b,borrow,__builtin_addressof(borrowout));
		return borrowout;
	}
	else if constexpr(sizeof(T)==sizeof(unsigned))
	{
		unsigned borrowout;
		out=__builtin_subc(a,b,borrow,__builtin_addressof(borrowout));
		return borrowout;
	}
	else if constexpr(sizeof(T)==sizeof(short unsigned))
	{
		short unsigned borrowout;
		out=__builtin_subcs(a,b,borrow,__builtin_addressof(borrowout));
		return borrowout;
	}
	else if constexpr(sizeof(T)==sizeof(char unsigned))
	{
		char unsigned borrowout;
		out=__builtin_subcb(a,b,borrow,__builtin_addressof(borrowout));
		return borrowout;
	}
	else
	{
		return sub_borrow_naive(borrow,a,b,out);
	}
#elif defined(__has_builtin) && (__has_builtin(__builtin_ia32_sbb_u32)||__has_builtin(__builtin_ia32_sbb_u64) || __has_builtin(__builtin_ia32_subborrow_u64) || __has_builtin(__builtin_ia32_subborrow_u32))
	if constexpr(sizeof(T)==8)
	{
#if __has_builtin(__builtin_ia32_sbb_u64)
		using may_alias_ptr_type
		#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
		#endif
		= unsigned long long*;
		return __builtin_ia32_sbb_u64(borrow,a,b,reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out)));
#elif __has_builtin(__builtin_ia32_subborrow_u64)
		using may_alias_ptr_type
		#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
		#endif
		= unsigned long long*;
		return __builtin_ia32_subborrow_u64(borrow,a,b,reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out)));
#elif __has_builtin(__builtin_ia32_sbb_u32) || __has_builtin(__builtin_ia32_subborrow_u32)
		std::uint_least32_t a_low;
		std::uint_least32_t a_high;
		__builtin_memcpy(__builtin_addressof(a_low),__builtin_addressof(a),4);
		__builtin_memcpy(__builtin_addressof(a_high),reinterpret_cast<char const*>(__builtin_addressof(a))+4,4);
		std::uint_least32_t b_low;
		std::uint_least32_t b_high;
		__builtin_memcpy(__builtin_addressof(b_low),__builtin_addressof(b),4);
		__builtin_memcpy(__builtin_addressof(b_high),reinterpret_cast<char const*>(__builtin_addressof(b))+4,4);
		using may_alias_ptr_type
		#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
		#endif
		= unsigned*;
#if __has_builtin(__builtin_ia32_sbb_u32)
		return __builtin_ia32_sbb_u32(__builtin_ia32_sbb_u32(borrow,a_low,b_low,reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out))),
		a_high,b_high,reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out))+1);
#else
		return __builtin_ia32_subborrow_u32(__builtin_ia32_subborrow_u32(borrow,a_low,b_low,reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out))),
		a_high,b_high,reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out))+1);
#endif
#else
		return sub_borrow_naive(borrow,a,b,out);
#endif
	}
	else if constexpr(sizeof(T)==4)
	{
#if __has_builtin(__builtin_ia32_sbb_u32)
		using may_alias_ptr_type
		#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
		#endif
		= unsigned*;
		return __builtin_ia32_sbb_u32(borrow,a,b,reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out)));
#elif __has_builtin(__builtin_ia32_subborrow_u32)
		using may_alias_ptr_type
		#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
		#endif
		= unsigned*;
		return __builtin_ia32_subborrow_u32(borrow,a,b,reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out)));
#else
		return sub_borrow_naive(borrow,a,b,out);
#endif
	}
	else
		return sub_borrow_naive(borrow,a,b,out);	//16 bit subborrow simply does not exist on gcc and clang
#else
	return sub_borrow_naive(borrow,a,b,out);
#endif
	}
}

template<typename U>
struct ul_generic_x2_little_endian
{
	U low,high;
};
template<typename U>
struct ul_generic_x2_big_endian
{
	U high,low;
};

template<typename U>
using ul_generic_x2 = std::conditional_t<std::endian::native==std::endian::big,ul_generic_x2_big_endian<U>,ul_generic_x2_little_endian<U>>;

using ul32x2_little_endian=ul_generic_x2_little_endian<std::uint_least32_t>;
using ul32x2_big_endian=ul_generic_x2_big_endian<std::uint_least32_t>;
using ul32x2 = ul_generic_x2<std::uint_least32_t>;

inline constexpr std::uint_least32_t umul_least_32(std::uint_least32_t a,std::uint_least32_t b,std::uint_least32_t& high) noexcept
{
#if defined(__has_builtin) && defined(__GNUC__) && !defined(__clang__)
#if __has_builtin(__builtin_bit_cast)
	if constexpr(std::endian::native==std::endian::little||std::endian::native==std::endian::big)
	{
		auto ret{__builtin_bit_cast(ul32x2,static_cast<std::uint_least64_t>(a)*b)};
		high=ret.high;
		return ret.low;
	}
	else
#endif
#endif
	{
		std::uint_least64_t v{static_cast<std::uint_least64_t>(a)*b};
		high=static_cast<std::uint_least32_t>(v>>(::std::numeric_limits<::std::uint_least32_t>::digits));
		return static_cast<std::uint_least32_t>(v);
	}
}

inline constexpr std::uint_least32_t umulh_least_32(std::uint_least32_t a,std::uint_least32_t b) noexcept
{
#if defined(__has_builtin) && defined(__GNUC__) && !defined(__clang__)
#if __has_builtin(__builtin_bit_cast)
	if constexpr(std::endian::native==std::endian::little||std::endian::native==std::endian::big)
	{
		auto ret{__builtin_bit_cast(ul32x2,static_cast<std::uint_least64_t>(a)*b)};
		return ret.high;
	}
	else
#endif
#endif
	{
		std::uint_least64_t v{static_cast<std::uint_least64_t>(a)*b};
		return static_cast<std::uint_least32_t>(v>>(::std::numeric_limits<::std::uint_least32_t>::digits));
	}
}

inline constexpr std::uint_least32_t unpack_ul64(std::uint_least64_t a,std::uint_least32_t& high) noexcept
{
#if defined(__has_builtin) && defined(__GNUC__) && !defined(__clang__)
#if __has_builtin(__builtin_bit_cast)
	if constexpr(std::endian::native==std::endian::little)
	{
		auto [a0,a1]=__builtin_bit_cast(ul32x2_little_endian,a);	//get around gcc bug
		high=a1;
		return a0;
	}
	else if constexpr(std::endian::native==std::endian::big)
	{
		auto [a1,a0]=__builtin_bit_cast(ul32x2_big_endian,a);
		high=a1;
		return a0;
	}
	else
#endif
#endif
	{
		high=static_cast<std::uint_least32_t>(a>>std::numeric_limits<std::uint_least32_t>::digits);
		return static_cast<std::uint_least32_t>(a);
	}
}

template<typename T,typename U>
requires (sizeof(T)==sizeof(U)*2)
inline constexpr U unpack_generic(T a,U& high) noexcept
{
#if defined(__has_builtin) && defined(__GNUC__) && !defined(__clang__)
#if __has_builtin(__builtin_bit_cast)
	if constexpr(std::endian::native==std::endian::little)
	{
		auto [a0,a1]=__builtin_bit_cast(ul_generic_x2_little_endian<U>,a);	//get around gcc bug
		high=a1;
		return a0;
	}
	else if constexpr(std::endian::native==std::endian::big)
	{
		auto [a1,a0]=__builtin_bit_cast(ul_generic_x2_big_endian<U>,a);
		high=a1;
		return a0;
	}
	else
#endif
#endif
	{
		high=static_cast<U>(a>>std::numeric_limits<U>::digits);
		return static_cast<U>(a);
	}
}

inline constexpr std::uint_least64_t pack_ul64(std::uint_least32_t low,std::uint_least32_t high) noexcept
{
#if defined(__has_builtin) && defined(__GNUC__) && !defined(__clang__)
#if __has_builtin(__builtin_bit_cast)
	if constexpr(std::endian::native==std::endian::little)
	{
		return __builtin_bit_cast(std::uint_least64_t,ul32x2_little_endian{low,high});	//get around gcc bug
	}
	else if constexpr(std::endian::native==std::endian::big)
	{
		return __builtin_bit_cast(std::uint_least64_t,ul32x2_big_endian{high,low});	//get around gcc bug
	}
	else
#endif
#endif
	{
		return (static_cast<std::uint_least64_t>(high)<<32u)|low;
	}
}

inline constexpr std::uint_least64_t umul_least_64_emulated(std::uint_least64_t a,std::uint_least64_t b,std::uint_least64_t& high) noexcept
{
	std::uint_least32_t a1;
	std::uint_least32_t a0{unpack_ul64(a,a1)};
	std::uint_least32_t b1;
	std::uint_least32_t b0{unpack_ul64(b,b1)};
	std::uint_least32_t c1;
	std::uint_least32_t c0{umul_least_32(a0,b0,c1)};
	std::uint_least32_t a0b1h;
	std::uint_least32_t a0b1l{umul_least_32(a0,b1,a0b1h)};
	std::uint_least32_t a1b0h;
	std::uint_least32_t a1b0l{umul_least_32(a1,b0,a1b0h)};
	std::uint_least32_t c3;
	std::uint_least32_t c2{umul_least_32(a1,b1,c3)};
	constexpr std::uint_least32_t zero{};
	bool carry{add_carry(false,c1,a0b1l,c1)};
	carry=add_carry(carry,a0b1h,c2,c2);
	add_carry(carry,zero,c3,c3);
	carry=add_carry(false,c1,a1b0l,c1);
	carry=add_carry(carry,a1b0h,c2,c2);
	add_carry(carry,zero,c3,c3);
	high=pack_ul64(c2,c3);
	return pack_ul64(c0,c1);
}

inline
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
constexpr
#endif
std::uint_least64_t umul(std::uint_least64_t a,std::uint_least64_t b,std::uint_least64_t& high) noexcept
{
#ifdef __SIZEOF_INT128__
#if __cpp_if_consteval >= 202106L
	if consteval
	{
		__uint128_t res{static_cast<__uint128_t>(a)*b};
		high=static_cast<std::uint_least64_t>(res>>64u);
		return static_cast<std::uint_least64_t>(res);
	}
	else
#elif __cpp_lib_is_constant_evaluated >= 201811L
	if(std::is_constant_evaluated())
	{
		__uint128_t res{static_cast<__uint128_t>(a)*b};
		high=static_cast<std::uint_least64_t>(res>>64u);
		return static_cast<std::uint_least64_t>(res);
	}
	else

#endif
	{
#if defined(__has_builtin)
		if constexpr(std::endian::native==std::endian::little||std::endian::native==std::endian::big)
		{
			struct u64x2_little_endian_t
			{
				std::uint_least64_t low,high;
			};
			struct u64x2_big_endian_t
			{
				std::uint_least64_t high,low;
			};
			using u64x2_t = std::conditional_t<std::endian::native==std::endian::little,u64x2_little_endian_t,u64x2_big_endian_t>;
			static_assert(sizeof(__uint128_t)==sizeof(u64x2_t));
#if __has_builtin(__builtin_bit_cast)
			auto u{__builtin_bit_cast(u64x2_t,static_cast<__uint128_t>(a)*b)};
			high=u.high;
			return u.low;
#else
			__uint128_t res{static_cast<__uint128_t>(a)*b};
			u64x2_t u;
#if __has_builtin(__builtin_memcpy)
			__builtin_memcpy(__builtin_addressof(u),__builtin_addressof(res),sizeof(u64x2_t));
#else
			std::memcpy(__builtin_addressof(u),__builtin_addressof(res),sizeof(u64x2_t));
#endif
			high=u.high;
			return u.low;
#endif
		}
		else
#endif
		{
			__uint128_t res{static_cast<__uint128_t>(a)*b};
			high=static_cast<std::uint_least64_t>(res>>64u);
			return static_cast<std::uint_least64_t>(res);
		}
	}
#elif defined(_MSC_VER) && defined(_M_X64)
#if __cpp_if_consteval >= 202106L
	if consteval
	{
		return umul_least_64_emulated(a,b,high);
	}
	else
#elif __cpp_lib_is_constant_evaluated >= 201811L
	if(std::is_constant_evaluated())
	{
		return umul_least_64_emulated(a,b,high);
	}
	else
#endif
	{
		return _umul128(a,b,__builtin_addressof(high));
	}
#else
	return umul_least_64_emulated(a,b,high);
#endif
}

inline constexpr std::uint_least64_t umul_least64_high_emulated(std::uint_least64_t a,std::uint_least64_t b) noexcept
{
	std::uint_least32_t a1;
	std::uint_least32_t a0{unpack_ul64(a,a1)};
	std::uint_least32_t b1;
	std::uint_least32_t b0{unpack_ul64(b,b1)};
	std::uint_least32_t c1;
	umul_least_32(a0,b0,c1);
	std::uint_least32_t a0b1h;
	std::uint_least32_t a0b1l{umul_least_32(a0,b1,a0b1h)};
	std::uint_least32_t a1b0h;
	std::uint_least32_t a1b0l{umul_least_32(a1,b0,a1b0h)};
	std::uint_least32_t c3;
	std::uint_least32_t c2{umul_least_32(a1,b1,c3)};
	constexpr std::uint_least32_t zero{};
	bool carry{add_carry(false,c1,a0b1l,c1)};
	carry=add_carry(carry,a0b1h,c2,c2);
	add_carry(carry,zero,c3,c3);
	carry=add_carry(false,c1,a1b0l,c1);
	carry=add_carry(carry,a1b0h,c2,c2);
	add_carry(carry,zero,c3,c3);
	return pack_ul64(c2,c3);
}

inline
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
constexpr
#endif
std::uint_least64_t umul_least64_high(std::uint_least64_t a,std::uint_least64_t b) noexcept
{
#ifdef __SIZEOF_INT128__
	constexpr std::uint_least64_t ul64bits{sizeof(std::uint_least64_t)*
#if defined(__CHAR_BIT__)
	__CHAR_BIT__
#else
	8u
#endif
	};
	return static_cast<std::uint_least64_t>((static_cast<__uint128_t>(a)*b)>>ul64bits);
#elif defined(_MSC_VER) && defined(_M_X64)
#if __cpp_if_consteval >= 202106L
	if consteval
	{
		return umul_least64_high_emulated(a,b);
	}
	else
#elif __cpp_lib_is_constant_evaluated >= 201811L
	if(std::is_constant_evaluated())
	{
		return umul_least64_high_emulated(a,b);
	}
	else
#endif
	{
		return __umulh(a,b);
	}
#else
	return umul_least64_high_emulated(a,b);
#endif
}

inline constexpr std::size_t add_or_overflow_die(std::size_t a,std::size_t b) noexcept
{
#if defined(_MSC_VER) && !defined(__clang__)
#if __cpp_lib_is_constant_evaluated >= 201811L
	if(!std::is_constant_evaluated())
	{
#if defined(_M_X64)
		std::size_t res;
		if(_addcarry_u64(false,a,b,__builtin_addressof(res)))[[unlikely]]
			__debugbreak();
		return res;
#elif defined(_M_X32)
		std::size_t size{a+b};
		if(size<a)[[unlikely]]
			__debugbreak();
		return size;
#else
		std::size_t size{a+b};
		if(size<a)[[unlikely]]
			__debugbreak();
		return size;
#endif
	}
	else
#endif
	{
		std::size_t size{a+b};
		if(size<a)[[unlikely]]
			__debugbreak();
		return size;
	}
#elif __has_builtin(__builtin_add_overflow)&& __has_builtin(__builtin_trap)
	std::size_t size;
	if(__builtin_add_overflow(a,b,__builtin_addressof(size)))[[unlikely]]
		__builtin_trap();
	return size;
#else
	std::size_t size{a+b};
	if(size<a)[[unlikely]]
		fast_terminate();
	return size;
#endif
}

template<typename... Args>
requires (std::same_as<std::size_t,std::remove_cvref_t<Args>>&&...)
inline constexpr std::size_t add_or_overflow_die_chain(std::size_t size,Args... args) noexcept
{
	if constexpr(sizeof...(Args)==0)
		return size;
	else
		return add_or_overflow_die(size,add_or_overflow_die_chain(args...));
}

inline constexpr std::size_t mul_or_overflow_die(std::size_t a,std::size_t b) noexcept
{
#if defined(_MSC_VER) && !defined(__clang__)
	std::size_t const max{SIZE_MAX/b};
	if(max<a)[[unlikely]]
		__debugbreak();
	return a*b;
#elif __has_builtin(__builtin_mul_overflow)&& __has_builtin(__builtin_trap)
	std::size_t size;
	if(__builtin_mul_overflow(a,b,__builtin_addressof(size)))[[unlikely]]
		__builtin_trap();
	return size;
#else
	std::size_t size{a+b};
	if(size<a)[[unlikely]]
		fast_terminate();
	return size;
#endif
}

template<std::signed_integral int_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr bool sub_underflow_naive(int_type a,int_type b,int_type& c) noexcept
{
	int_type int_type_min{std::numeric_limits<int_type>::min()},
			int_type_max{std::numeric_limits<int_type>::max()};
	int_type zero{};
	if(zero<b)
		int_type_min+=b;
	else
		int_type_max+=b;
	if(a<int_type_min || a>int_type_max)
		return true;
	c=a-b;
	return false;
}

template<std::signed_integral int_type>
inline constexpr bool sub_underflow(int_type a,int_type b,int_type& c) noexcept
{
#if defined(_MSC_VER) && !defined(__clang__)
	return sub_underflow_naive(a,b,c);
#elif __has_builtin(__builtin_sub_underflow)
	return __builtin_sub_underflow(a,b,__builtin_addressof(c));
#else
	return sub_underflow_naive(a,b,c);
#endif
}

template<typename... Args>
requires (std::same_as<std::size_t,std::remove_cvref_t<Args>>&&...)
inline constexpr std::size_t mul_or_overflow_die_chain(std::size_t size,Args... args) noexcept
{
	if constexpr(sizeof...(Args)==0)
		return size;
	else
		return mul_or_overflow_die(size,mul_or_overflow_die_chain(args...));
}

template<typename T>
inline constexpr std::size_t cal_allocation_size_or_die(std::size_t size) noexcept
{
#if defined(_MSC_VER) && !defined(__clang__)
	constexpr std::size_t max_size{SIZE_MAX/sizeof(T)};
	if(size>max_size)
		__debugbreak();
	return size*sizeof(T);
#elif __has_builtin(__builtin_mul_overflow) && __has_builtin(__builtin_trap)
	if(__builtin_mul_overflow(size,sizeof(T),__builtin_addressof(size)))[[unlikely]]
		__builtin_trap();
	return size;
#else
	constexpr std::size_t max_size{SIZE_MAX/sizeof(T)};
	if(size>max_size)
		fast_terminate();
	return size*sizeof(T);
#endif
}

template<std::unsigned_integral U>
struct udiv_result
{
	using unsigned_type = U;
	U quotient_low,quotient_high;
	U remainder_low,remainder_high;
};

template<typename U>
inline constexpr U shiftright_naive(U low_part,U high_part,std::uint_least8_t shift) noexcept
{
	U const value{(static_cast<U>(1)<<shift)-1};
	constexpr std::uint_least8_t bits{sizeof(U)*8};
	U const v{high_part&value};
	if(shift==0)
		return low_part;
	return (low_part>>shift)|(v<<(bits-shift));
}

template<typename U>
inline constexpr U shiftright(U low_part,U high_part,std::uint_least8_t shift) noexcept
{
#if __cpp_lib_is_constant_evaluated >= 201811L
	if(std::is_constant_evaluated())
		return shiftright_naive(low_part,high_part,shift);
	else
#endif
	{
#if defined(_MSC_VER) && (defined(_M_X64) || defined(__i386__)) && !defined(__clang__)
		if constexpr(sizeof(U)==8&&sizeof(std::size_t)>=8)
		{
			return __shiftright128(low_part,high_part,shift);
		}
		else
#endif
		{
			return shiftright_naive(low_part,high_part,shift);
		}
	}
}

/*
https://code.woboq.org/llvm/compiler-rt/lib/builtins/udivmodti4.c.html#__udivmodti4
*/
template<std::unsigned_integral U>
inline constexpr udiv_result<U> udivmod(U n_low, U n_high,U d_low,U d_high) noexcept
{
	// special cases, X is unknown, K != 0
	if (n_high == 0)
	{
		if (d_high == 0)
		{
			return {n_low/d_low,0,n_low%d_low,0};
		}
		return {0,0,n_low,0};
	}
	constexpr std::uint_least32_t n_udword_bits{sizeof(U) * 8};
	constexpr std::uint_least32_t n_utword_bits{n_udword_bits<<1};
	constexpr std::uint_least32_t n_udword_bits_m1{n_udword_bits-1};
	constexpr std::uint_least32_t n_udword_bits_m2{n_udword_bits-2};
	// n_high != 0
	U q_low,q_high,r_low,r_high;
	std::uint_least32_t sr;
	if (d_low == 0)
	{
		if (d_high == 0)
			fast_terminate();
		// d_high != 0
		if (n_low == 0)
		{
			// K 0
			// ---
			// K 0
			return {n_high / d_high,0,0,n_high % d_high};
		}
		// K K
		// ---
		// K 0
		if ((d_high & (d_high - 1)) == 0)
		{
#if defined(__has_builtin)
#if __has_builtin(__builtin_ctzll)
			return {n_high >> __builtin_ctzll(d_high),0,n_low,n_high & (d_high - 1)};
#else
			return {n_high >> static_cast<std::uint_least32_t>(std::countl_zero(d_high)),0,n_low,n_high & (d_high - 1)};
#endif
#else
			return {n_high >> static_cast<std::uint_least32_t>(std::countl_zero(d_high)),0,n_low,n_high & (d_high - 1)};
#endif

		}
		// K K
		// ---
		// K 0
#if defined(__has_builtin)
#if __has_builtin(__builtin_ctzll)
		sr=static_cast<std::uint_least32_t>(__builtin_clzll(d_high) - __builtin_clzll(n_high));
#else
		sr=static_cast<std::uint_least32_t>(std::countl_zero(d_high) - std::countl_zero(n_high));
#endif
#else
		sr=static_cast<std::uint_least32_t>(std::countl_zero(d_high) - std::countl_zero(n_high));
#endif

		// 0 <= sr <= n_udword_bits - 2 or sr large
		if (sr > n_udword_bits_m2)
			return {0,0,n_low,n_high};
		++sr;
		// 1 <= sr <= n_udword_bits - 1
		// q.all = n.all << (n_utword_bits - sr);
		q_low=0;
		q_high=n_low << (n_udword_bits - sr);
		// r.all = n.all >> sr;
		r_high=n_high >> sr;
		r_low=(n_high << (n_udword_bits - sr)) | (n_low >> sr);
	}
	else /* d_low != 0 */
	{
		if (d_high == 0)
		{
			// K X
			// ---
			// 0 K
			if ((d_low & (d_low - 1)) == 0) /* if d is a power of 2 */
			{
				U rem{n_low & (d_low - 1)};
				if (d_low == 1)
					return {n_low,n_high,rem,0};
#if defined(__has_builtin)
#if __has_builtin(__builtin_ctzll)
				sr = static_cast<std::uint_least32_t>(__builtin_ctzll(d_low));
#else
				sr = static_cast<std::uint_least32_t>(std::countl_zero(d_low));
#endif
#else
				sr = static_cast<std::uint_least32_t>(std::countl_zero(d_low));
#endif
				q_high = n_high >> sr;
				q_low = (n_high << (n_udword_bits - sr)) | (n_low >> sr);
				return {q_low,q_high,rem,0};
			}
			// K X
			// ---
			// 0 K
#if defined(__has_builtin)
#if __has_builtin(__builtin_ctzll)
			sr = 1 + n_udword_bits + static_cast<std::uint_least32_t>(__builtin_clzll(d_low)) - static_cast<std::uint_least32_t>(__builtin_clzll(n_high));
#else
			sr = 1 + n_udword_bits + static_cast<std::uint_least32_t>(std::countl_zero(d_low)) - static_cast<std::uint_least32_t>(std::countl_zero(n_high));
#endif
#else
			sr = 1 + n_udword_bits + static_cast<std::uint_least32_t>(std::countl_zero(d_low)) - static_cast<std::uint_least32_t>(std::countl_zero(n_high));
#endif
			// 2 <= sr <= n_utword_bits - 1
			// q.all = n.all << (n_utword_bits - sr);
			// r.all = n.all >> sr;
			if (sr == n_udword_bits)
			{
				q_low = 0;
				q_high = n_low;
				r_high = 0;
				r_low = n_high;
			}
			else if (sr < n_udword_bits) /* 2 <= sr <= n_udword_bits - 1 */
			{
				q_low = 0;
				q_high = n_low << (n_udword_bits - sr);
				r_high = n_high >> sr;
				r_low = (n_high << (n_udword_bits - sr)) | (n_low >> sr);
			}
			else /* n_udword_bits + 1 <= sr <= n_utword_bits - 1 */
			{
				q_low = n_low << (n_utword_bits - sr);
				q_high = (n_high << (n_utword_bits - sr)) |
						(n_low >> (sr - n_udword_bits));
				r_high = 0;
				r_low = n_high >> (sr - n_udword_bits);
			}
		}
		else
		{
			// K X
			// ---
			// K K
#if defined(__has_builtin)
#if __has_builtin(__builtin_ctzll)
			sr = static_cast<std::uint_least32_t>(__builtin_clzll(d_high)) - static_cast<std::uint_least32_t>(__builtin_clzll(n_high));
#else
			sr = static_cast<std::uint_least32_t>(std::countl_zero(d_high)) - static_cast<std::uint_least32_t>(std::countl_zero(n_high));
#endif
#else
			sr = static_cast<std::uint_least32_t>(std::countl_zero(d_high)) - static_cast<std::uint_least32_t>(std::countl_zero(n_high));
#endif
			// 0 <= sr <= n_udword_bits - 1 or sr large
			if (sr > n_udword_bits_m1)
				return {0,0,n_low,n_high};
			++sr;
			// 1 <= sr <= n_udword_bits
			// q.all = n.all << (n_utword_bits - sr);
			// r.all = n.all >> sr;
			q_low = 0;
			if (sr == n_udword_bits)
			{
				q_high = n_low;
				r_high = 0;
				r_low = n_high;
			}
			else
			{
				r_high = n_high >> sr;
				r_low = (n_high << (n_udword_bits - sr)) | (n_low >> sr);
				q_high = n_low << (n_udword_bits - sr);
			}
		}
	}
	// Not a special case
	// q and r are initialized with:
	// q.all = n.all << (n_utword_bits - sr);
	// r.all = n.all >> sr;
	// 1 <= sr <= n_utword_bits - 1

	std::uint_least8_t carry{};
	for (;sr;--sr)
	{
		// r:q = ((r:q)	<< 1) | carry
		r_high = (r_high << 1) | (r_low >> n_udword_bits_m1);
		r_low = (r_low << 1) | (q_high >> n_udword_bits_m1);
		q_high = (q_high << 1) | (q_low >> n_udword_bits_m1);
		q_low = (q_low << 1) | static_cast<U>(carry);


		U s_low,s_high;
		sub_borrow(sub_borrow(true,d_low,r_low,s_low),d_high,r_high,s_high);
		s_low=shiftright(s_low,s_high,n_udword_bits_m1);
		s_high>>=n_udword_bits_m1;
		carry = s_low & 1;
		s_low&=d_low;
		s_high&=d_high;
		sub_borrow(sub_borrow(false,r_low,s_low,r_low),r_high,s_high,r_high);
//		r.all -= d.all & s;
	}
	q_high = (q_high << 1) | (q_low >> n_udword_bits_m1);
	q_low = (q_low << 1) | carry;
	return {q_low,q_high,r_low,r_high};
}

}

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning( pop )
#endif
