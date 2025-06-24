#pragma once

#if defined(_MSC_VER) && !defined(__clang__)
#include <intrin.h>
#endif

namespace fast_io::details::intrinsics
{

template <typename T1, typename T2>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_lib_bit_cast >= 201806L
	constexpr
#endif
	void typed_memcpy(T1 *dest, T2 const *src, ::std::size_t bytes) noexcept
{
#if __cpp_lib_bit_cast >= 201806L
	if (__builtin_is_constant_evaluated())
	{
		if (dest == nullptr || src == nullptr || sizeof(T1) != sizeof(T2) || bytes % sizeof(T1) != 0) [[unlikely]]
		{
			fast_terminate();
		}
		::std::size_t n{bytes / sizeof(T1)};
		for (::std::size_t i{}; i != n; ++i)
		{
			dest[i] = __builtin_bit_cast(T1, src[i]);
		}
	}
	else
#endif
	{
		::fast_io::freestanding::my_memcpy(dest, src, bytes);
	}
}

template <typename T>
#if __cpp_lib_concepts >= 202002L
	requires(::std::unsigned_integral<T>)
#endif
inline constexpr bool add_carry_naive(bool carry, T a, T b, T &out) noexcept
{
	T temp{carry + a};
	out = temp + b;
	return (out < b) | (temp < a);
}

template <typename U>
struct ul_generic_x2_little_endian
{
	U low, high;
};
template <typename U>
struct ul_generic_x2_big_endian
{
	U high, low;
};

template <typename U>
using ul_generic_x2 = ::std::conditional_t<::std::endian::native == ::std::endian::big, ul_generic_x2_big_endian<U>,
										   ul_generic_x2_little_endian<U>>;

using ul32x2_little_endian = ul_generic_x2_little_endian<::std::uint_least32_t>;
using ul32x2_big_endian = ul_generic_x2_big_endian<::std::uint_least32_t>;
using ul32x2 = ul_generic_x2<::std::uint_least32_t>;

inline constexpr ::std::uint_least32_t umul_least_32(::std::uint_least32_t a, ::std::uint_least32_t b,
													 ::std::uint_least32_t &high) noexcept
{
#if defined(__GNUC__) && !defined(__clang__) && FAST_IO_HAS_BUILTIN(__builtin_bit_cast)
	if constexpr (::std::endian::native == ::std::endian::little || ::std::endian::native == ::std::endian::big)
	{
		auto ret{__builtin_bit_cast(ul32x2, static_cast<::std::uint_least64_t>(a) * b)};
		high = ret.high;
		return ret.low;
	}
	else
#endif
	{
		::std::uint_least64_t v{static_cast<::std::uint_least64_t>(a) * b};
		high = static_cast<::std::uint_least32_t>(v >> (::std::numeric_limits<::std::uint_least32_t>::digits));
		return static_cast<::std::uint_least32_t>(v);
	}
}

inline constexpr ::std::uint_least32_t umulh_least_32(::std::uint_least32_t a, ::std::uint_least32_t b) noexcept
{
#if defined(__GNUC__) && !defined(__clang__) && FAST_IO_HAS_BUILTIN(__builtin_bit_cast)
	if constexpr (::std::endian::native == ::std::endian::little || ::std::endian::native == ::std::endian::big)
	{
		auto ret{__builtin_bit_cast(ul32x2, static_cast<::std::uint_least64_t>(a) * b)};
		return ret.high;
	}
	else
#endif
	{
		::std::uint_least64_t v{static_cast<::std::uint_least64_t>(a) * b};
		return static_cast<::std::uint_least32_t>(v >> (::std::numeric_limits<::std::uint_least32_t>::digits));
	}
}

inline constexpr ::std::uint_least32_t unpack_ul64(::std::uint_least64_t a, ::std::uint_least32_t &high) noexcept
{
#if defined(__GNUC__) && !defined(__clang__) && FAST_IO_HAS_BUILTIN(__builtin_bit_cast)
	if constexpr (::std::endian::native == ::std::endian::little)
	{
		auto [a0, a1] = __builtin_bit_cast(ul32x2_little_endian, a); // get around gcc bug
		high = a1;
		return a0;
	}
	else if constexpr (::std::endian::native == ::std::endian::big)
	{
		auto [a1, a0] = __builtin_bit_cast(ul32x2_big_endian, a);
		high = a1;
		return a0;
	}
	else
#endif
	{
		high = static_cast<::std::uint_least32_t>(a >> ::std::numeric_limits<::std::uint_least32_t>::digits);
		return static_cast<::std::uint_least32_t>(a);
	}
}

template <typename T, typename U>
	requires(sizeof(T) == sizeof(U) * 2)
inline constexpr U unpack_generic(T a, U &high) noexcept
{
#if defined(__GNUC__) && !defined(__clang__) && FAST_IO_HAS_BUILTIN(__builtin_bit_cast)
	if constexpr (::std::endian::native == ::std::endian::little)
	{
		auto [a0, a1] = __builtin_bit_cast(ul_generic_x2_little_endian<U>, a); // get around gcc bug
		high = a1;
		return a0;
	}
	else if constexpr (::std::endian::native == ::std::endian::big)
	{
		auto [a1, a0] = __builtin_bit_cast(ul_generic_x2_big_endian<U>, a);
		high = a1;
		return a0;
	}
	else
#endif
	{
		high = static_cast<U>(a >> ::std::numeric_limits<U>::digits);
		return static_cast<U>(a);
	}
}

template <typename T>
#if __cpp_lib_concepts >= 202002L
	requires(::std::unsigned_integral<T>)
#endif
inline constexpr bool add_carry(bool carry, T a, T b, T &out) noexcept
{

	if constexpr (sizeof(T) > sizeof(::std::uint_least64_t))
	{
		::std::uint_least64_t ahigh;
		auto alow = unpack_generic(a, ahigh);
		::std::uint_least64_t bhigh;
		auto blow = unpack_generic(b, bhigh);
		carry = add_carry(carry, alow, blow, alow);
		carry = add_carry(carry, ahigh, bhigh, ahigh);
		out = static_cast<T>(ahigh) << 64u | alow;
		return carry;
	}
	else
	{
		if (__builtin_is_constant_evaluated())
		{
			return add_carry_naive(carry, a, b, out);
		}
		else
		{
#if defined(_MSC_VER) && !defined(__clang__)
#if (defined(_M_IX86) || defined(_M_AMD64))
			if constexpr (sizeof(T) == 8)
			{
#if defined(_M_AMD64)
				return _addcarry_u64(carry, a, b, reinterpret_cast<::std::uint_least64_t *>(__builtin_addressof(out)));
#else
				return _addcarry_u32(_addcarry_u32(carry,
												   *reinterpret_cast<::std::uint_least32_t *>(__builtin_addressof(a)),
												   *reinterpret_cast<::std::uint_least32_t *>(__builtin_addressof(b)),
												   reinterpret_cast<::std::uint_least32_t *>(__builtin_addressof(out))),
									 reinterpret_cast<::std::uint_least32_t *>(__builtin_addressof(a))[1],
									 reinterpret_cast<::std::uint_least32_t *>(__builtin_addressof(b))[1],
									 reinterpret_cast<::std::uint_least32_t *>(__builtin_addressof(out)) + 1);
#endif
			}
			else if constexpr (sizeof(T) == 4)
			{
				return _addcarry_u32(carry, a, b, reinterpret_cast<::std::uint_least32_t *>(__builtin_addressof(out)));
			}
			else if constexpr (sizeof(T) == 2)
			{
				return _addcarry_u16(carry, a, b, reinterpret_cast<short unsigned *>(__builtin_addressof(out)));
			}
			else if constexpr (sizeof(T) == 1)
			{
				return _addcarry_u8(carry, a, b, reinterpret_cast<char unsigned *>(__builtin_addressof(out)));
			}
			else
			{
				return add_carry_naive(carry, a, b, out);
			}
#else
			return add_carry_naive(carry, a, b, out);
#endif
#elif (FAST_IO_HAS_BUILTIN(__builtin_addcb) && FAST_IO_HAS_BUILTIN(__builtin_addcs) && FAST_IO_HAS_BUILTIN(__builtin_addc) && \
	 FAST_IO_HAS_BUILTIN(__builtin_addcl) && FAST_IO_HAS_BUILTIN(__builtin_addcll))
			if constexpr (sizeof(T) == sizeof(long long unsigned))
			{
				long long unsigned carryout;
				out = __builtin_addcll(a, b, carry, __builtin_addressof(carryout));
				return carryout;
			}
			else if constexpr (sizeof(T) == sizeof(long unsigned))
			{
				long unsigned carryout;
				out = __builtin_addcl(a, b, carry, __builtin_addressof(carryout));
				return carryout;
			}
			else if constexpr (sizeof(T) == sizeof(unsigned))
			{
				unsigned carryout;
				out = __builtin_addc(a, b, carry, __builtin_addressof(carryout));
				return carryout;
			}
			else if constexpr (sizeof(T) == sizeof(short unsigned))
			{
				short unsigned carryout;
				out = __builtin_addcs(a, b, carry, __builtin_addressof(carryout));
				return carryout;
			}
			else if constexpr (sizeof(T) == sizeof(char unsigned))
			{
				char unsigned carryout;
				out = __builtin_addcb(a, b, carry, __builtin_addressof(carryout));
				return carryout;
			}
			else
			{
				return add_carry_naive(carry, a, b, out);
			}
#elif (FAST_IO_HAS_BUILTIN(__builtin_ia32_addcarryx_u32) || FAST_IO_HAS_BUILTIN(__builtin_ia32_addcarry_u32) || \
	 FAST_IO_HAS_BUILTIN(__builtin_ia32_addcarryx_u64))
			if constexpr (sizeof(T) == 8)
			{
#if FAST_IO_HAS_BUILTIN(__builtin_ia32_addcarryx_u64)
				using may_alias_ptr_type
#if __has_cpp_attribute(__gnu__::__may_alias__)
					[[__gnu__::__may_alias__]]
#endif
					= unsigned long long *;
				return __builtin_ia32_addcarryx_u64(carry, a, b,
													reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out)));
#elif FAST_IO_HAS_BUILTIN(__builtin_ia32_addcarry_u64)
				using may_alias_ptr_type
#if __has_cpp_attribute(__gnu__::__may_alias__)
					[[__gnu__::__may_alias__]]
#endif
					= unsigned long long *;
				return __builtin_ia32_addcarry_u64(carry, a, b,
												   reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out)));
#else
				::std::uint_least32_t a_low;
				::std::uint_least32_t a_high;
				__builtin_memcpy(__builtin_addressof(a_low), __builtin_addressof(a), 4);
				__builtin_memcpy(__builtin_addressof(a_high),
								 reinterpret_cast<char const *>(__builtin_addressof(a)) + 4, 4);
				::std::uint_least32_t b_low;
				::std::uint_least32_t b_high;
				__builtin_memcpy(__builtin_addressof(b_low), __builtin_addressof(b), 4);
				__builtin_memcpy(__builtin_addressof(b_high),
								 reinterpret_cast<char const *>(__builtin_addressof(b)) + 4, 4);
				using may_alias_ptr_type
#if __has_cpp_attribute(__gnu__::__may_alias__)
					[[__gnu__::__may_alias__]]
#endif
					= unsigned *;
#if FAST_IO_HAS_BUILTIN(__builtin_ia32_addcarryx_u32)
				return __builtin_ia32_addcarryx_u32(
					__builtin_ia32_addcarryx_u32(carry, a_low, b_low,
												 reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out))),
					a_high, b_high, reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out)) + 1);
#elif FAST_IO_HAS_BUILTIN(__builtin_ia32_addcarry_u32)
				return __builtin_ia32_addcarry_u32(
					__builtin_ia32_addcarry_u32(carry, a_low, b_low,
												reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out))),
					a_high, b_high, reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out)) + 1);
#else
				return add_carry_naive(carry, a, b, out);
#endif
#endif
			}
			else if constexpr (sizeof(T) == 4)
			{
				using may_alias_ptr_type
#if __has_cpp_attribute(__gnu__::__may_alias__)
					[[__gnu__::__may_alias__]]
#endif
					= unsigned *;
#if FAST_IO_HAS_BUILTIN(__builtin_ia32_addcarryx_u32)
				return __builtin_ia32_addcarryx_u32(carry, a, b,
													reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out)));
#elif FAST_IO_HAS_BUILTIN(__builtin_ia32_addcarry_u32)
				return __builtin_ia32_addcarry_u32(carry, a, b,
												   reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out)));
#else
				return add_carry_naive(carry, a, b, out);
#endif
			}
			else
			{
				return add_carry_naive(carry, a, b, out); // 16 bit addcarry simply does not exist on gcc and clang
			}
#else
			return add_carry_naive(carry, a, b, out);
#endif
		}
	}
}

template <typename T>
#if __cpp_lib_concepts >= 202002L
	requires(::std::unsigned_integral<T>)
#endif
inline constexpr bool sub_borrow_naive(bool carry, T a, T b, T &out) noexcept
{
	T temp{a - carry};
	out = temp - b;
	return (temp < out) | (a < temp);
}

template <typename T>
	requires(sizeof(T) <= sizeof(::std::uint_least64_t) * 2)
inline constexpr bool sub_borrow(bool borrow, T a, T b, T &out) noexcept
{
	if constexpr (sizeof(T) > sizeof(::std::uint_least64_t))
	{
		::std::uint_least64_t ahigh;
		auto alow = unpack_generic(a, ahigh);
		::std::uint_least64_t bhigh;
		auto blow = unpack_generic(b, bhigh);
		borrow = sub_borrow(borrow, alow, blow, alow);
		borrow = sub_borrow(borrow, ahigh, bhigh, ahigh);
		out = static_cast<T>(ahigh) << 64u | alow;
		return borrow;
	}
	else
	{
		if (__builtin_is_constant_evaluated())
		{
			return sub_borrow_naive(borrow, a, b, out);
		}
		else
		{
#if defined(_MSC_VER) && !defined(__clang__)
#if (defined(_M_IX86) || defined(_M_AMD64))
			if constexpr (sizeof(T) == 8)
			{
#if defined(_M_AMD64)
				return _subborrow_u64(borrow, a, b,
									  reinterpret_cast<::std::uint_least64_t *>(__builtin_addressof(out)));
#else
				return _subborrow_u32(
					_subborrow_u32(borrow, *reinterpret_cast<::std::uint_least32_t *>(__builtin_addressof(a)),
								   *reinterpret_cast<::std::uint_least32_t *>(__builtin_addressof(b)),
								   reinterpret_cast<::std::uint_least32_t *>(__builtin_addressof(out))),
					reinterpret_cast<::std::uint_least32_t *>(__builtin_addressof(a))[1],
					reinterpret_cast<::std::uint_least32_t *>(__builtin_addressof(b))[1],
					reinterpret_cast<::std::uint_least32_t *>(__builtin_addressof(out)) + 1);
#endif
			}
			else if constexpr (sizeof(T) == 4)
			{
				return _subborrow_u32(borrow, a, b,
									  reinterpret_cast<::std::uint_least32_t *>(__builtin_addressof(out)));
			}
			else if constexpr (sizeof(T) == 2)
			{
				return _subborrow_u16(borrow, a, b, reinterpret_cast<short unsigned *>(__builtin_addressof(out)));
			}
			else if constexpr (sizeof(T) == 1)
			{
				return _subborrow_u8(borrow, a, b, reinterpret_cast<char unsigned *>(__builtin_addressof(out)));
			}
			else
			{
				return sub_borrow_naive(borrow, a, b, out);
			}
#else
			return sub_borrow_naive(borrow, a, b, out);
#endif
#elif (FAST_IO_HAS_BUILTIN(__builtin_subcb) && FAST_IO_HAS_BUILTIN(__builtin_subcs) && FAST_IO_HAS_BUILTIN(__builtin_subc) && \
	 FAST_IO_HAS_BUILTIN(__builtin_subcl) && FAST_IO_HAS_BUILTIN(__builtin_subcll))
			if constexpr (sizeof(T) == sizeof(long long unsigned))
			{
				long long unsigned borrowout;
				out = __builtin_subcll(a, b, borrow, __builtin_addressof(borrowout));
				return borrowout;
			}
			else if constexpr (sizeof(T) == sizeof(long unsigned))
			{
				long unsigned borrowout;
				out = __builtin_subcl(a, b, borrow, __builtin_addressof(borrowout));
				return borrowout;
			}
			else if constexpr (sizeof(T) == sizeof(unsigned))
			{
				unsigned borrowout;
				out = __builtin_subc(a, b, borrow, __builtin_addressof(borrowout));
				return borrowout;
			}
			else if constexpr (sizeof(T) == sizeof(short unsigned))
			{
				short unsigned borrowout;
				out = __builtin_subcs(a, b, borrow, __builtin_addressof(borrowout));
				return borrowout;
			}
			else if constexpr (sizeof(T) == sizeof(char unsigned))
			{
				char unsigned borrowout;
				out = __builtin_subcb(a, b, borrow, __builtin_addressof(borrowout));
				return borrowout;
			}
			else
			{
				return sub_borrow_naive(borrow, a, b, out);
			}
#elif (FAST_IO_HAS_BUILTIN(__builtin_ia32_sbb_u32) || FAST_IO_HAS_BUILTIN(__builtin_ia32_sbb_u64) || \
	 FAST_IO_HAS_BUILTIN(__builtin_ia32_subborrow_u64) || FAST_IO_HAS_BUILTIN(__builtin_ia32_subborrow_u32))
			if constexpr (sizeof(T) == 8)
			{
#if FAST_IO_HAS_BUILTIN(__builtin_ia32_sbb_u64)
				using may_alias_ptr_type
#if __has_cpp_attribute(__gnu__::__may_alias__)
					[[__gnu__::__may_alias__]]
#endif
					= unsigned long long *;
				return __builtin_ia32_sbb_u64(borrow, a, b,
											  reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out)));
#elif FAST_IO_HAS_BUILTIN(__builtin_ia32_subborrow_u64)
				using may_alias_ptr_type
#if __has_cpp_attribute(__gnu__::__may_alias__)
					[[__gnu__::__may_alias__]]
#endif
					= unsigned long long *;
				return __builtin_ia32_subborrow_u64(borrow, a, b,
													reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out)));
#elif FAST_IO_HAS_BUILTIN(__builtin_ia32_sbb_u32) || FAST_IO_HAS_BUILTIN(__builtin_ia32_subborrow_u32)
				::std::uint_least32_t a_low;
				::std::uint_least32_t a_high;
				__builtin_memcpy(__builtin_addressof(a_low), __builtin_addressof(a), 4);
				__builtin_memcpy(__builtin_addressof(a_high),
								 reinterpret_cast<char const *>(__builtin_addressof(a)) + 4, 4);
				::std::uint_least32_t b_low;
				::std::uint_least32_t b_high;
				__builtin_memcpy(__builtin_addressof(b_low), __builtin_addressof(b), 4);
				__builtin_memcpy(__builtin_addressof(b_high),
								 reinterpret_cast<char const *>(__builtin_addressof(b)) + 4, 4);
				using may_alias_ptr_type
#if __has_cpp_attribute(__gnu__::__may_alias__)
					[[__gnu__::__may_alias__]]
#endif
					= unsigned *;
#if FAST_IO_HAS_BUILTIN(__builtin_ia32_sbb_u32)
				return __builtin_ia32_sbb_u32(
					__builtin_ia32_sbb_u32(borrow, a_low, b_low,
										   reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out))),
					a_high, b_high, reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out)) + 1);
#else
				return __builtin_ia32_subborrow_u32(
					__builtin_ia32_subborrow_u32(borrow, a_low, b_low,
												 reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out))),
					a_high, b_high, reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out)) + 1);
#endif
#else
				return sub_borrow_naive(borrow, a, b, out);
#endif
			}
			else if constexpr (sizeof(T) == 4)
			{
#if FAST_IO_HAS_BUILTIN(__builtin_ia32_sbb_u32)
				using may_alias_ptr_type
#if __has_cpp_attribute(__gnu__::__may_alias__)
					[[__gnu__::__may_alias__]]
#endif
					= unsigned *;
				return __builtin_ia32_sbb_u32(borrow, a, b,
											  reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out)));
#elif FAST_IO_HAS_BUILTIN(__builtin_ia32_subborrow_u32)
				using may_alias_ptr_type
#if __has_cpp_attribute(__gnu__::__may_alias__)
					[[__gnu__::__may_alias__]]
#endif
					= unsigned *;
				return __builtin_ia32_subborrow_u32(borrow, a, b,
													reinterpret_cast<may_alias_ptr_type>(__builtin_addressof(out)));
#else
				return sub_borrow_naive(borrow, a, b, out);
#endif
			}
			else
			{
				return sub_borrow_naive(borrow, a, b, out); // 16 bit subborrow simply does not exist on gcc and clang
			}
#else
			return sub_borrow_naive(borrow, a, b, out);
#endif
		}
	}
}

inline constexpr ::std::uint_least64_t pack_ul64(::std::uint_least32_t low, ::std::uint_least32_t high) noexcept
{
#if defined(__GNUC__) && !defined(__clang__) && FAST_IO_HAS_BUILTIN(__builtin_bit_cast)
	if constexpr (::std::endian::native == ::std::endian::little)
	{
		return __builtin_bit_cast(::std::uint_least64_t, ul32x2_little_endian{low, high}); // get around gcc bug
	}
	else if constexpr (::std::endian::native == ::std::endian::big)
	{
		return __builtin_bit_cast(::std::uint_least64_t, ul32x2_big_endian{high, low}); // get around gcc bug
	}
	else
#endif
	{
		return (static_cast<::std::uint_least64_t>(high) << 32u) | low;
	}
}

inline constexpr ::std::uint_least64_t umul_least_64_emulated(::std::uint_least64_t a, ::std::uint_least64_t b,
															  ::std::uint_least64_t &high) noexcept
{
	::std::uint_least32_t a1;
	::std::uint_least32_t a0{unpack_ul64(a, a1)};
	::std::uint_least32_t b1;
	::std::uint_least32_t b0{unpack_ul64(b, b1)};
	::std::uint_least32_t c1;
	::std::uint_least32_t c0{umul_least_32(a0, b0, c1)};
	::std::uint_least32_t a0b1h;
	::std::uint_least32_t a0b1l{umul_least_32(a0, b1, a0b1h)};
	::std::uint_least32_t a1b0h;
	::std::uint_least32_t a1b0l{umul_least_32(a1, b0, a1b0h)};
	::std::uint_least32_t c3;
	::std::uint_least32_t c2{umul_least_32(a1, b1, c3)};
	constexpr ::std::uint_least32_t zero{};
	bool carry{add_carry(false, c1, a0b1l, c1)};
	carry = add_carry(carry, a0b1h, c2, c2);
	add_carry(carry, zero, c3, c3);
	carry = add_carry(false, c1, a1b0l, c1);
	carry = add_carry(carry, a1b0h, c2, c2);
	add_carry(carry, zero, c3, c3);
	high = pack_ul64(c2, c3);
	return pack_ul64(c0, c1);
}

inline
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
	constexpr
#endif
	::std::uint_least64_t
	umul(::std::uint_least64_t a, ::std::uint_least64_t b, ::std::uint_least64_t &high) noexcept
{
#ifdef __SIZEOF_INT128__
	if (__builtin_is_constant_evaluated())
	{
		__uint128_t res{static_cast<__uint128_t>(a) * b};
		high = static_cast<::std::uint_least64_t>(res >> 64u);
		return static_cast<::std::uint_least64_t>(res);
	}
	else
	{
#if defined(__has_builtin)
		if constexpr (::std::endian::native == ::std::endian::little || ::std::endian::native == ::std::endian::big)
		{
			struct u64x2_little_endian_t
			{
				::std::uint_least64_t low, high;
			};
			struct u64x2_big_endian_t
			{
				::std::uint_least64_t high, low;
			};
			using u64x2_t = ::std::conditional_t<::std::endian::native == ::std::endian::little, u64x2_little_endian_t,
												 u64x2_big_endian_t>;
			static_assert(sizeof(__uint128_t) == sizeof(u64x2_t));
#if FAST_IO_HAS_BUILTIN(__builtin_bit_cast)
			auto u{__builtin_bit_cast(u64x2_t, static_cast<__uint128_t>(a) * b)};
			high = u.high;
			return u.low;
#else
			__uint128_t res{static_cast<__uint128_t>(a) * b};
			u64x2_t u;
#if FAST_IO_HAS_BUILTIN(__builtin_memcpy)
			__builtin_memcpy(__builtin_addressof(u), __builtin_addressof(res), sizeof(u64x2_t));
#else
			::std::memcpy(__builtin_addressof(u), __builtin_addressof(res), sizeof(u64x2_t));
#endif
			high = u.high;
			return u.low;
#endif
		}
		else
#endif
		{
			__uint128_t res{static_cast<__uint128_t>(a) * b};
			high = static_cast<::std::uint_least64_t>(res >> 64u);
			return static_cast<::std::uint_least64_t>(res);
		}
	}
#elif defined(_MSC_VER) && defined(_M_X64) && !defined(_M_ARM64EC)
#if __cpp_if_consteval >= 202106L && 0
	if !consteval
	{
		return umul_least_64_emulated(a, b, high);
	}
	else
#elif __cpp_lib_is_constant_evaluated >= 201811L
	if (__builtin_is_constant_evaluated())
	{
		return umul_least_64_emulated(a, b, high);
	}
	else
#endif
	{
		return _umul128(a, b, __builtin_addressof(high));
	}
#else
	return umul_least_64_emulated(a, b, high);
#endif
}

inline constexpr ::std::uint_least64_t umul_least64_high_emulated(::std::uint_least64_t a,
																  ::std::uint_least64_t b) noexcept
{
	::std::uint_least32_t a1;
	::std::uint_least32_t a0{unpack_ul64(a, a1)};
	::std::uint_least32_t b1;
	::std::uint_least32_t b0{unpack_ul64(b, b1)};
	::std::uint_least32_t c1;
	umul_least_32(a0, b0, c1);
	::std::uint_least32_t a0b1h;
	::std::uint_least32_t a0b1l{umul_least_32(a0, b1, a0b1h)};
	::std::uint_least32_t a1b0h;
	::std::uint_least32_t a1b0l{umul_least_32(a1, b0, a1b0h)};
	::std::uint_least32_t c3;
	::std::uint_least32_t c2{umul_least_32(a1, b1, c3)};
	constexpr ::std::uint_least32_t zero{};
	bool carry{add_carry(false, c1, a0b1l, c1)};
	carry = add_carry(carry, a0b1h, c2, c2);
	add_carry(carry, zero, c3, c3);
	carry = add_carry(false, c1, a1b0l, c1);
	carry = add_carry(carry, a1b0h, c2, c2);
	add_carry(carry, zero, c3, c3);
	return pack_ul64(c2, c3);
}

inline
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
	constexpr
#endif
	::std::uint_least64_t
	umul_least64_high(::std::uint_least64_t a, ::std::uint_least64_t b) noexcept
{
#ifdef __SIZEOF_INT128__
	constexpr ::std::uint_least64_t ul64bits{sizeof(::std::uint_least64_t) *
#if defined(__CHAR_BIT__)
											 __CHAR_BIT__
#else
											 8u
#endif
	};
	return static_cast<::std::uint_least64_t>((static_cast<__uint128_t>(a) * b) >> ul64bits);
#elif defined(_MSC_VER) && defined(_M_X64) && !defined(_M_ARM64EC)
	if (__builtin_is_constant_evaluated())
	{
		return umul_least64_high_emulated(a, b);
	}
	else
	{
		return __umulh(a, b);
	}
#else
	return umul_least64_high_emulated(a, b);
#endif
}

inline constexpr ::std::size_t add_or_overflow_die(::std::size_t a, ::std::size_t b) noexcept
{
#if defined(_MSC_VER) && !defined(__clang__)
#if __cpp_lib_is_constant_evaluated >= 201811L
	if (!__builtin_is_constant_evaluated())
	{
#if defined(_M_X64)
		::std::size_t res;
		if (_addcarry_u64(false, a, b, __builtin_addressof(res))) [[unlikely]]
		{
			__debugbreak();
		}
		return res;
#elif defined(_M_X32)
		::std::size_t size{a + b};
		if (size < a) [[unlikely]]
		{
			__debugbreak();
		}
		return size;
#else
		::std::size_t size{a + b};
		if (size < a) [[unlikely]]
		{
			__debugbreak();
		}
		return size;
#endif
	}
	else
#endif
	{
		::std::size_t size{a + b};
		if (size < a) [[unlikely]]
		{
			__debugbreak();
		}
		return size;
	}
#elif FAST_IO_HAS_BUILTIN(__builtin_add_overflow)
	::std::size_t size;
	if (__builtin_add_overflow(a, b, __builtin_addressof(size))) [[unlikely]]
	{
		fast_terminate();
	}
	return size;
#else
	::std::size_t size{a + b};
	if (size < a) [[unlikely]]
	{
		fast_terminate();
	}
	return size;
#endif
}

template <typename... Args>
	requires(::std::same_as<::std::size_t, ::std::remove_cvref_t<Args>> && ...)
inline constexpr ::std::size_t add_or_overflow_die_chain(::std::size_t size, Args... args) noexcept
{
	if constexpr (sizeof...(Args) == 0)
	{
		return size;
	}
	else
	{
		return add_or_overflow_die(size, add_or_overflow_die_chain(args...));
	}
}

inline constexpr ::std::size_t mul_or_overflow_die(::std::size_t a, ::std::size_t b) noexcept
{
#if defined(_MSC_VER) && !defined(__clang__)
	::std::size_t const max{SIZE_MAX / b};
	if (max < a) [[unlikely]]
	{
		__debugbreak();
	}
	return a * b;
#elif FAST_IO_HAS_BUILTIN(__builtin_mul_overflow)
	::std::size_t size;
	if (__builtin_mul_overflow(a, b, __builtin_addressof(size))) [[unlikely]]
	{
		fast_terminate();
	}
	return size;
#else
	::std::size_t const max{SIZE_MAX / b};
	if (max < a) [[unlikely]]
	{
		fast_terminate();
	}
	return a * b;
#endif
}

template <::std::signed_integral int_type>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr bool sub_underflow_naive(int_type a, int_type b, int_type &c) noexcept
{
	int_type int_type_min{::std::numeric_limits<int_type>::min()}, int_type_max{::std::numeric_limits<int_type>::max()};
	int_type zero{};
	if (zero < b)
	{
		int_type_min += b;
	}
	else
	{
		int_type_max += b;
	}
	if (a < int_type_min || a > int_type_max)
	{
		return true;
	}
	c = a - b;
	return false;
}

template <::std::signed_integral int_type>
inline constexpr bool sub_underflow(int_type a, int_type b, int_type &c) noexcept
{
#if defined(_MSC_VER) && !defined(__clang__)
	return sub_underflow_naive(a, b, c);
#elif FAST_IO_HAS_BUILTIN(__builtin_sub_underflow)
	return __builtin_sub_underflow(a, b, __builtin_addressof(c));
#else
	return sub_underflow_naive(a, b, c);
#endif
}

template <typename... Args>
	requires(::std::same_as<::std::size_t, ::std::remove_cvref_t<Args>> && ...)
inline constexpr ::std::size_t mul_or_overflow_die_chain(::std::size_t size, Args... args) noexcept
{
	if constexpr (sizeof...(Args) == 0)
	{
		return size;
	}
	else
	{
		return mul_or_overflow_die(size, mul_or_overflow_die_chain(args...));
	}
}

template <typename T>
inline constexpr ::std::size_t cal_allocation_size_or_die(::std::size_t size) noexcept
{
#if defined(_MSC_VER) && !defined(__clang__)
	constexpr ::std::size_t max_size{SIZE_MAX / sizeof(T)};
	if (size > max_size)
	{
		__debugbreak();
	}
	return size * sizeof(T);
#elif FAST_IO_HAS_BUILTIN(__builtin_mul_overflow)
	if (__builtin_mul_overflow(size, sizeof(T), __builtin_addressof(size))) [[unlikely]]
	{
		fast_terminate();
	}
	return size;
#else
	constexpr ::std::size_t max_size{SIZE_MAX / sizeof(T)};
	if (size > max_size)
	{
		fast_terminate();
	}
	return size * sizeof(T);
#endif
}

template <typename U>
inline constexpr U shiftright_naive(U low_part, U high_part, ::std::uint_least8_t shift) noexcept
{
	U const value{(static_cast<U>(1) << shift) - 1};
	constexpr ::std::uint_least8_t bits{sizeof(U) * 8};
	U const v{high_part & value};
	if (shift == 0)
	{
		return low_part;
	}
	return (low_part >> shift) | (v << (bits - shift));
}

template <typename U>
inline constexpr U shiftright(U low_part, U high_part, ::std::uint_least8_t shift) noexcept
{
#if __cpp_lib_is_constant_evaluated >= 201811L
	if (__builtin_is_constant_evaluated())
	{
		return shiftright_naive(low_part, high_part, shift);
	}
	else
#endif
	{
#if defined(_MSC_VER) && (defined(_M_X64) || defined(__i386__)) && !defined(__clang__)
		if constexpr (sizeof(U) == 8 && sizeof(::std::size_t) >= 8)
		{
			return __shiftright128(low_part, high_part, shift);
		}
		else
#endif
		{
			return shiftright_naive(low_part, high_part, shift);
		}
	}
}

} // namespace fast_io::details::intrinsics
