#pragma once

namespace fast_io::intrinsics
{

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
using ul_generic_x2 = ::std::conditional_t<::std::endian::native == ::std::endian::big,
										::fast_io::intrinsics::ul_generic_x2_big_endian<U>,
										::fast_io::intrinsics::ul_generic_x2_little_endian<U>>;

using ul32x2 = ::fast_io::intrinsics::ul_generic_x2<::std::uint_least32_t>;
using ul64x2 = ::fast_io::intrinsics::ul_generic_x2<::std::uint_least64_t>;

template <typename T, typename U>
	requires(sizeof(T) == sizeof(U) * 2)
inline constexpr U unpack_generic(T a, U &high) noexcept
{
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
	{
		high = static_cast<U>(a >> ::std::numeric_limits<U>::digits);
		return static_cast<U>(a);
	}
}

template <typename T, typename U>
	requires(sizeof(T) == sizeof(U) * 2)
inline constexpr T pack_generic(U low, U high) noexcept
{
	if constexpr (::std::endian::native == ::std::endian::little)
	{
		return __builtin_bit_cast(T, ::fast_io::intrinsics::ul_generic_x2_little_endian<U>{low, high}); // get around gcc bug
	}
	else if constexpr (::std::endian::native == ::std::endian::big)
	{
		return __builtin_bit_cast(T, ::fast_io::intrinsics::ul_generic_x2_big_endian<U>{high, low}); // get around gcc bug
	}
	else
	{
		return static_cast<T>(high << ::std::numeric_limits<U>::digits) | low;
	}
}

namespace details
{

template <typename T, typename U>
	requires(sizeof(T) == sizeof(U) * 2)
inline constexpr U umul_least_generic(U a, U b, U &high) noexcept
{
#if defined(__has_builtin) && defined(__GNUC__) && !defined(__clang__)
#if __has_builtin(__builtin_bit_cast)
	if constexpr (::std::endian::native == ::std::endian::little || ::std::endian::native == ::std::endian::big)
	{
		auto ret{__builtin_bit_cast(::fast_io::intrinsics::ul_generic_x2<U>, static_cast<T>(a) * b)};
		high = ret.high;
		return ret.low;
	}
	else
#endif
#endif
	{
		T v{static_cast<T>(a) * b};
		high = static_cast<U>(v >> (::std::numeric_limits<U>::digits));
		return static_cast<U>(v);
	}
}

template <typename T>
	requires(sizeof(T) == sizeof(::std::uint_least64_t))
inline constexpr T umul_least_generic_emulated(T a, T b, T &high) noexcept
{
	using U = ::std::uint_least32_t;
	U a1;
	U a0{::fast_io::intrinsics::unpack_generic<T, U>(a, a1)};
	U b1;
	U b0{::fast_io::intrinsics::unpack_generic<T, U>(b, b1)};
	U c1;
	U c0{::fast_io::intrinsics::details::umul_least_generic<T, U>(a0, b0, c1)};
	U a0b1h;
	U a0b1l{::fast_io::intrinsics::details::umul_least_generic<T, U>(a0, b1, a0b1h)};
	U a1b0h;
	U a1b0l{::fast_io::intrinsics::details::umul_least_generic<T, U>(a1, b0, a1b0h)};
	U c3;
	U c2{::fast_io::intrinsics::details::umul_least_generic<T, U>(a1, b1, c3)};
	constexpr U zero{};
	bool carry{};
	c1 = ::fast_io::intrinsics::addc(c1, a0b1l, carry, carry);
	c2 = ::fast_io::intrinsics::addc(c2, a0b1h, carry, carry);
	c3 = ::fast_io::intrinsics::addc(c3, zero, carry, carry);
	carry = false;
	c1 = ::fast_io::intrinsics::addc(c1, a1b0l, carry, carry);
	c2 = ::fast_io::intrinsics::addc(c2, a1b0h, carry, carry);
	c3 = ::fast_io::intrinsics::addc(c3, zero, carry, carry);

	high = ::fast_io::intrinsics::pack_generic<T, U>(c2, c3);
	return ::fast_io::intrinsics::pack_generic<T, U>(c0, c1);
}


template <typename T, typename U>
	requires(sizeof(T) == sizeof(U) * 2)
inline constexpr U umulh_least_generic(U a, U b) noexcept
{
#if defined(__has_builtin) && defined(__GNUC__) && !defined(__clang__)
#if __has_builtin(__builtin_bit_cast)
	if constexpr (::std::endian::native == ::std::endian::little || ::std::endian::native == ::std::endian::big)
	{
		auto ret{__builtin_bit_cast(::fast_io::intrinsics::ul_generic_x2<U>, static_cast<T>(a) * b)};
		return ret.high;
	}
	else
#endif
#endif
	{
		T v{static_cast<T>(a) * b};
		return static_cast<U>(v >> (::std::numeric_limits<U>::digits));
	}
}

template <typename T>
	requires(sizeof(T) == sizeof(::std::uint_least64_t))
inline constexpr T umulh_least_generic_emulated(T a, T b) noexcept
{
	using U = ::std::uint_least32_t;
	U a1;
	U a0{::fast_io::intrinsics::unpack_generic<T, U>(a, a1)};
	U b1;
	U b0{::fast_io::intrinsics::unpack_generic<T, U>(b, b1)};

#if 0
	U c1;
	U c0{::fast_io::intrinsics::details::umul_least_generic<T, U>(a0, b0, c1)};
	U a0b1h;
	U a0b1l{::fast_io::intrinsics::details::umul_least_generic<T, U>(a0, b1, a0b1h)};
	U a1b0h;
	U a1b0l{::fast_io::intrinsics::details::umul_least_generic<T, U>(a1, b0, a1b0h)};
	U c3;
	U c2{::fast_io::intrinsics::details::umul_least_generic<T, U>(a1, b1, c3)};
	constexpr U zero{};
	bool carry{};
	c1 = ::fast_io::intrinsics::addc(c1, a0b1l, carry, carry);
	c2 = ::fast_io::intrinsics::addc(c2, a0b1h, carry, carry);
	c3 = ::fast_io::intrinsics::addc(c3, zero, carry, carry);
	carry = false;
	c1 = ::fast_io::intrinsics::addc(c1, a1b0l, carry, carry);
	c2 = ::fast_io::intrinsics::addc(c2, a1b0h, carry, carry);
	c3 = ::fast_io::intrinsics::addc(c3, zero, carry, carry);

	high = ::fast_io::intrinsics::pack_generic<T, U>(c2, c3);
	return ::fast_io::intrinsics::pack_generic<T, U>(c0, c1);
#else
	U c1{::fast_io::intrinsics::details::umulh_least_generic<T, U>(a0, b0)};
	U a0b1h;
	U a0b1l{::fast_io::intrinsics::details::umul_least_generic<T, U>(a0, b1, a0b1h)};
	U a1b0h;
	U a1b0l{::fast_io::intrinsics::details::umul_least_generic<T, U>(a1, b0, a1b0h)};
	U c3;
	U c2{::fast_io::intrinsics::details::umul_least_generic<T, U>(a1, b1, c3)};
	constexpr U zero{};
	bool carry{};
	c1 = ::fast_io::intrinsics::addc(c1, a0b1l, carry, carry);
	c2 = ::fast_io::intrinsics::addc(c2, a0b1h, carry, carry);
	c3 = ::fast_io::intrinsics::addc(c3, zero, carry, carry);
	carry = false;
	c1 = ::fast_io::intrinsics::addc(c1, a1b0l, carry, carry);
	c2 = ::fast_io::intrinsics::addc(c2, a1b0h, carry, carry);
	c3 = ::fast_io::intrinsics::addc(c3, zero, carry, carry);

	return ::fast_io::intrinsics::pack_generic<T, U>(c2, c3);
#endif
}

} // namespace details

template <::std::unsigned_integral U>
inline constexpr U umul(U a, U b, U &high) noexcept
{
	if constexpr (sizeof(U) == sizeof(::std::uint_least64_t))
	{
#ifdef __SIZEOF_INT128__
#if defined(__cpp_lib_is_constant_evaluated) || defined(__cpp_if_consteval)
#if defined(__cpp_if_consteval)
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			__uint128_t res{static_cast<__uint128_t>(a) * b};
			high = static_cast<::std::uint_least64_t>(res >> 64u);
			return static_cast<::std::uint_least64_t>(res);
		}
		else
#endif
		{
#if defined(__cpp_lib_bit_cast)
			if constexpr (::std::endian::native == ::std::endian::little || ::std::endian::native == ::std::endian::big)
			{
				auto u{__builtin_bit_cast(::fast_io::intrinsics::ul64x2, static_cast<__uint128_t>(a) * b)};
				high = u.high;
				return u.low;
			}
			else
#endif
			{
				__uint128_t res{static_cast<__uint128_t>(a) * b};
				high = static_cast<::std::uint_least64_t>(res >> 64u);
				return static_cast<::std::uint_least64_t>(res);
			}
		}
#elif defined(_MSC_VER) && defined(_M_X64)
#if defined(__cpp_lib_is_constant_evaluated) || defined(__cpp_if_consteval)
#if defined(__cpp_if_consteval)
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			return ::fast_io::intrinsics::details::umul_least_generic_emulated(a, b, high);
		}
		else
#endif
		{
			return ::fast_io::intrinsics::msvc::x86::_umul128(a, b, __builtin_addressof(high));
		}
#else
		return ::fast_io::intrinsics::details::umul_least_generic_emulated(a, b, high);
#endif
	}
	else if constexpr (sizeof(U) == sizeof(::std::uint_least32_t))
	{
		return ::fast_io::intrinsics::details::umul_least_generic<::std::uint_least64_t, U>(a, b, high);
	}
	else if constexpr (sizeof(U) == sizeof(::std::uint_least16_t))
	{
		return ::fast_io::intrinsics::details::umul_least_generic<::std::uint_least32_t, U>(a, b, high);
	}
	else if constexpr (sizeof(U) == sizeof(::std::uint_least8_t))
	{
		return ::fast_io::intrinsics::details::umul_least_generic<::std::uint_least16_t, U>(a, b, high);
	}
}

template <::std::unsigned_integral U>
inline constexpr U umulh(U a, U b) noexcept
{
	if constexpr (sizeof(U) == sizeof(::std::uint_least64_t))
	{
#ifdef __SIZEOF_INT128__
#if defined(__cpp_lib_is_constant_evaluated) || defined(__cpp_if_consteval)
#if defined(__cpp_if_consteval)
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			__uint128_t res{static_cast<__uint128_t>(a) * b};
			high = static_cast<::std::uint_least64_t>(res >> 64u);
			return static_cast<::std::uint_least64_t>(res);
		}
		else
#endif
		{
#if defined(__cpp_lib_bit_cast)
			if constexpr (::std::endian::native == ::std::endian::little || ::std::endian::native == ::std::endian::big)
			{
				auto u{__builtin_bit_cast(::fast_io::intrinsics::ul64x2, static_cast<__uint128_t>(a) * b)};
				high = u.high;
				return u.low;
			}
			else
#endif
			{
				__uint128_t res{static_cast<__uint128_t>(a) * b};
				high = static_cast<::std::uint_least64_t>(res >> 64u);
				return static_cast<::std::uint_least64_t>(res);
			}
		}
#elif defined(_MSC_VER) && defined(_M_X64)
#if defined(__cpp_lib_is_constant_evaluated) || defined(__cpp_if_consteval)
#if defined(__cpp_if_consteval)
		if consteval
#else
		if (__builtin_is_constant_evaluated())
#endif
		{
			return ::fast_io::intrinsics::details::umulh_least_generic_emulated(a, b);
		}
		else
#endif
		{
			return ::fast_io::intrinsics::msvc::x86::_umulh(a, b);
		}
#else
		return ::fast_io::intrinsics::details::umulh_least_generic_emulated(a, b);
#endif
	}
	else if constexpr (sizeof(U) == sizeof(::std::uint_least32_t))
	{
		return ::fast_io::intrinsics::details::umulh_least_generic<::std::uint_least64_t, U>(a, b);
	}
	else if constexpr (sizeof(U) == sizeof(::std::uint_least16_t))
	{
		return ::fast_io::intrinsics::details::umulh_least_generic<::std::uint_least32_t, U>(a, b);
	}
	else if constexpr (sizeof(U) == sizeof(::std::uint_least8_t))
	{
		return ::fast_io::intrinsics::details::umulh_least_generic<::std::uint_least16_t, U>(a, b);
	}
}

} // namespace fast_io::intrinsics