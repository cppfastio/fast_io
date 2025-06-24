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
	requires(sizeof(U) * 2 <= sizeof(T))
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
	requires(sizeof(U) * 2 <= sizeof(T))
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

inline constexpr ::fast_io::intrinsics::ul64x2 pack_ul64(::std::uint_least64_t low, ::std::uint_least64_t high) noexcept
{
	// This is a full 128-bit number that requires a size end-order conversion
	if constexpr (::std::endian::native == ::std::endian::big)
	{
		return {high, low};
	}
	else
	{
		return {low, high};
	}
}

namespace details
{
template <typename U>
concept umul_unsigned_integral = ::std::unsigned_integral<U> || ::std::same_as<U, ::fast_io::intrinsics::ul64x2>;
}

template <::fast_io::intrinsics::details::umul_unsigned_integral U, ::fast_io::intrinsics::details::umul_unsigned_integral T>
	requires(sizeof(U) <= sizeof(T))
inline constexpr T umul(U a, T b, U &high) noexcept;

template <::fast_io::intrinsics::details::umul_unsigned_integral U, ::fast_io::intrinsics::details::umul_unsigned_integral T>
	requires(sizeof(U) <= sizeof(T))
inline constexpr U umulh(U a, T b) noexcept;

template <::fast_io::intrinsics::details::umul_unsigned_integral U, ::fast_io::intrinsics::details::umul_unsigned_integral T>
	requires(sizeof(U) <= sizeof(T))
inline constexpr T umull(U a, T b) noexcept;

namespace details
{

template <typename T, typename U>
	requires(sizeof(U) * 2 <= sizeof(T))
inline constexpr U umul_least_generic(U a, U b, U &high) noexcept
{
#if defined(__GNUC__) && !defined(__clang__) && FAST_IO_HAS_BUILTIN(__builtin_bit_cast)
	if constexpr (::std::endian::native == ::std::endian::little || ::std::endian::native == ::std::endian::big)
	{
		auto ret{__builtin_bit_cast(::fast_io::intrinsics::ul_generic_x2<U>, static_cast<T>(a) * b)};
		high = ret.high;
		return ret.low;
	}
	else
#endif
	{
		T v{static_cast<T>(a) * b};
		high = static_cast<U>(v >> (::std::numeric_limits<U>::digits));
		return static_cast<U>(v);
	}
}

template <typename T, typename U>
	requires(sizeof(T) == sizeof(::std::uint_least64_t))
inline constexpr T umul_least64_generic_emulated(U a, T b, U &high) noexcept
{
	using S = ::std::uint_least32_t;
	if constexpr (sizeof(U) == sizeof(T))
	{
		S a1;
		S a0{::fast_io::intrinsics::unpack_generic<T, S>(a, a1)};
		S b1;
		S b0{::fast_io::intrinsics::unpack_generic<T, S>(b, b1)};
		S c1;
		S c0{::fast_io::intrinsics::details::umul_least_generic<T, S>(a0, b0, c1)};
		S a0b1h;
		S a0b1l{::fast_io::intrinsics::details::umul_least_generic<T, S>(a0, b1, a0b1h)};
		S a1b0h;
		S a1b0l{::fast_io::intrinsics::details::umul_least_generic<T, S>(a1, b0, a1b0h)};
		S c3;
		S c2{::fast_io::intrinsics::details::umul_least_generic<T, S>(a1, b1, c3)};
		constexpr S zero{};
		bool carry{};
		c1 = ::fast_io::intrinsics::addc(c1, a0b1l, carry, carry);
		c2 = ::fast_io::intrinsics::addc(c2, a0b1h, carry, carry);
		c3 = ::fast_io::intrinsics::addc(c3, zero, carry, carry);
		carry = false;
		c1 = ::fast_io::intrinsics::addc(c1, a1b0l, carry, carry);
		c2 = ::fast_io::intrinsics::addc(c2, a1b0h, carry, carry);
		c3 = ::fast_io::intrinsics::addc(c3, zero, carry, carry);

		high = ::fast_io::intrinsics::pack_generic<T, S>(c2, c3);
		return ::fast_io::intrinsics::pack_generic<T, S>(c0, c1);
	}
	else
	{
		S a0{a};
		S b1;
		S b0{::fast_io::intrinsics::unpack_generic<T, S>(b, b1)};
		S c1;
		S c0{::fast_io::intrinsics::details::umul_least_generic<T, S>(a0, b0, c1)};
		S a0b1h;
		S a0b1l{::fast_io::intrinsics::details::umul_least_generic<T, S>(a0, b1, a0b1h)};

		constexpr S zero{};
		bool carry{};
		c1 = ::fast_io::intrinsics::addc(c1, a0b1l, carry, carry);
		a0b1h = ::fast_io::intrinsics::addc(zero, a0b1h, carry, carry);

		high = a0b1h;
		return ::fast_io::intrinsics::pack_generic<T, S>(c0, c1);
	}
}

template <typename T, typename U>
	requires(sizeof(U) * 2 <= sizeof(T))
inline constexpr U umulh_least_generic(U a, U b) noexcept
{
#if defined(__GNUC__) && !defined(__clang__) && FAST_IO_HAS_BUILTIN(__builtin_bit_cast)
	if constexpr (::std::endian::native == ::std::endian::little || ::std::endian::native == ::std::endian::big)
	{
		auto ret{__builtin_bit_cast(::fast_io::intrinsics::ul_generic_x2<U>, static_cast<T>(a) * b)};
		return ret.high;
	}
	else
#endif
	{
		T v{static_cast<T>(a) * b};
		return static_cast<U>(v >> (::std::numeric_limits<U>::digits));
	}
}

template <typename T, typename U>
inline constexpr U umulh_least64_generic_emulated(U a, T b) noexcept
{
	using S = ::std::uint_least32_t;
	if constexpr (sizeof(U) == sizeof(T))
	{
		S a1;
		S a0{::fast_io::intrinsics::unpack_generic<T, S>(a, a1)};
		S b1;
		S b0{::fast_io::intrinsics::unpack_generic<T, S>(b, b1)};

		S c1{::fast_io::intrinsics::details::umulh_least_generic<T, S>(a0, b0)};
		S a0b1h;
		S a0b1l{::fast_io::intrinsics::details::umul_least_generic<T, S>(a0, b1, a0b1h)};
		S a1b0h;
		S a1b0l{::fast_io::intrinsics::details::umul_least_generic<T, S>(a1, b0, a1b0h)};
		S c3;
		S c2{::fast_io::intrinsics::details::umul_least_generic<T, S>(a1, b1, c3)};
		constexpr S zero{};
		bool carry{};
		c1 = ::fast_io::intrinsics::addc(c1, a0b1l, carry, carry);
		c2 = ::fast_io::intrinsics::addc(c2, a0b1h, carry, carry);
		c3 = ::fast_io::intrinsics::addc(c3, zero, carry, carry);
		carry = false;
		c1 = ::fast_io::intrinsics::addc(c1, a1b0l, carry, carry);
		c2 = ::fast_io::intrinsics::addc(c2, a1b0h, carry, carry);
		c3 = ::fast_io::intrinsics::addc(c3, zero, carry, carry);

		return ::fast_io::intrinsics::pack_generic<T, S>(c2, c3);
	}
	else
	{
		S a0{a};
		S b1;
		S b0{::fast_io::intrinsics::unpack_generic<T, S>(b, b1)};

		S c1{::fast_io::intrinsics::details::umulh_least_generic<T, S>(a0, b0)};
		S a0b1h;
		S a0b1l{::fast_io::intrinsics::details::umul_least_generic<T, S>(a0, b1, a0b1h)};
		constexpr S zero{};
		bool carry{};
		::fast_io::intrinsics::addc(c1, a0b1l, carry, carry);

		return ::fast_io::intrinsics::addc(zero, a0b1h, carry, carry);
	}
}

template <typename U>
	requires(sizeof(U) <= sizeof(::std::uint_least64_t))
inline constexpr ::fast_io::intrinsics::ul64x2 umul_a0_least128_generic_emulated(U a0, ::std::uint_least64_t b0, ::std::uint_least64_t b1, U &high) noexcept
{
	U a0b0h;
	::std::uint_least64_t c0{::fast_io::intrinsics::umul(a0, b0, a0b0h)};
	U c2;
	::std::uint_least64_t a0b1l{::fast_io::intrinsics::umul(a0, b1, c2)};
	bool carry{};
	constexpr ::std::uint_least64_t zero{};
	::std::uint_least64_t c1{::fast_io::intrinsics::addc(static_cast<::std::uint_least64_t>(a0b0h), a0b1l, carry, carry)};
	high = ::fast_io::intrinsics::addc(zero, c2, carry, carry);
	if constexpr (::std::endian::native == ::std::endian::big)
	{
		return {c1, c0};
	}
	else
	{
		return {c0, c1};
	}
}

inline constexpr ::fast_io::intrinsics::ul64x2 umul_least128_generic_emulated(::std::uint_least64_t a0, ::std::uint_least64_t a1, ::std::uint_least64_t b0, ::std::uint_least64_t b1, ::fast_io::intrinsics::ul64x2 &high) noexcept
{
	::std::uint_least64_t c1;
	::std::uint_least64_t c0{::fast_io::intrinsics::umul(a0, b0, c1)};
	::std::uint_least64_t a0b1h;
	::std::uint_least64_t a0b1l{::fast_io::intrinsics::umul(a0, b1, a0b1h)};
	::std::uint_least64_t a1b0h;
	::std::uint_least64_t a1b0l{::fast_io::intrinsics::umul(a1, b0, a1b0h)};
	::std::uint_least64_t c3;
	::std::uint_least64_t c2{::fast_io::intrinsics::umul(a1, b1, c3)};
	constexpr ::std::uint_least64_t zero{};
	bool carry{};
	c1 = ::fast_io::intrinsics::addc(c1, a0b1l, carry, carry);
	c2 = ::fast_io::intrinsics::addc(c2, a0b1h, carry, carry);
	c3 = ::fast_io::intrinsics::addc(c3, zero, carry, carry);
	carry = false;
	c1 = ::fast_io::intrinsics::addc(c1, a1b0l, carry, carry);
	c2 = ::fast_io::intrinsics::addc(c2, a1b0h, carry, carry);
	c3 = ::fast_io::intrinsics::addc(c3, zero, carry, carry);

	if constexpr (::std::endian::native == ::std::endian::big)
	{
		high = {c3, c2};
		return {c1, c0};
	}
	else
	{
		high = {c2, c3};
		return {c0, c1};
	}
}

template <typename U>
	requires(sizeof(U) <= sizeof(::std::uint_least64_t))
inline constexpr ::fast_io::intrinsics::ul64x2 umull_a0_least128_generic_emulated(U a0, ::std::uint_least64_t b0, ::std::uint_least64_t b1) noexcept
{
	U a0b0h;
	::std::uint_least64_t c0{::fast_io::intrinsics::umul(a0, b0, a0b0h)};
	::std::uint_least64_t c1{a0 * b1};
	c1 += a0b0h;
	if constexpr (::std::endian::native == ::std::endian::big)
	{
		return {c0, c1};
	}
	else
	{
		return {c1, c0};
	}
}

inline constexpr ::fast_io::intrinsics::ul64x2 umull_least128_generic_emulated(::std::uint_least64_t a0, ::std::uint_least64_t a1, ::std::uint_least64_t b0, ::std::uint_least64_t b1) noexcept
{
	::std::uint_least64_t c1;
	::std::uint_least64_t c0{::fast_io::intrinsics::umul(a0, b0, c1)};
	::std::uint_least64_t a0b1l{a0 * b1};
	::std::uint_least64_t a1b0l{a1 * b0};
	c1 += a0b1l;
	c1 += a1b0l;
	if constexpr (::std::endian::native == ::std::endian::big)
	{
		return {c1, c0};
	}
	else
	{
		return {c0, c1};
	}
}

template <typename U>
	requires(sizeof(U) <= sizeof(::std::uint_least64_t))
inline constexpr U umulh_a0_least128_generic_emulated(U a0, ::std::uint_least64_t b0, ::std::uint_least64_t b1) noexcept
{
	U a0b0h{::fast_io::intrinsics::umulh(a0, b0)};
	U c2;
	::std::uint_least64_t a0b1l{::fast_io::intrinsics::umul(a0, b1, c2)};
	bool carry{};
	constexpr ::std::uint_least64_t zero{};
	::fast_io::intrinsics::addc(static_cast<::std::uint_least64_t>(a0b0h), a0b1l, carry, carry);
	return ::fast_io::intrinsics::addc(zero, c2, carry, carry);
}

inline constexpr ::fast_io::intrinsics::ul64x2 umulh_least128_generic_emulated(::std::uint_least64_t a0, ::std::uint_least64_t a1, ::std::uint_least64_t b0, ::std::uint_least64_t b1) noexcept
{
	::std::uint_least64_t c1{::fast_io::intrinsics::umulh(a0, b0)};
	::std::uint_least64_t a0b1h;
	::std::uint_least64_t a0b1l{::fast_io::intrinsics::umul(a0, b1, a0b1h)};
	::std::uint_least64_t a1b0h;
	::std::uint_least64_t a1b0l{::fast_io::intrinsics::umul(a1, b0, a1b0h)};
	::std::uint_least64_t c3;
	::std::uint_least64_t c2{::fast_io::intrinsics::umul(a1, b1, c3)};
	constexpr ::std::uint_least64_t zero{};
	bool carry{};
	c1 = ::fast_io::intrinsics::addc(c1, a0b1l, carry, carry);
	c2 = ::fast_io::intrinsics::addc(c2, a0b1h, carry, carry);
	c3 = ::fast_io::intrinsics::addc(c3, zero, carry, carry);
	carry = false;
	::fast_io::intrinsics::addc(c1, a1b0l, carry, carry);
	c2 = ::fast_io::intrinsics::addc(c2, a1b0h, carry, carry);
	c3 = ::fast_io::intrinsics::addc(c3, zero, carry, carry);

	if constexpr (::std::endian::native == ::std::endian::big)
	{
		return {c3, c2};
	}
	else
	{
		return {c2, c3};
	}
}

} // namespace details

template <::fast_io::intrinsics::details::umul_unsigned_integral U, ::fast_io::intrinsics::details::umul_unsigned_integral T>
	requires(sizeof(U) <= sizeof(T))
inline constexpr T umul(U a, T b, U &high) noexcept
{
	if constexpr (::std::same_as<T, ::fast_io::intrinsics::ul64x2>)
	{
		if constexpr (::std::same_as<U, ::fast_io::intrinsics::ul64x2>)
		{
			return ::fast_io::intrinsics::details::umul_least128_generic_emulated(a.low, a.high, b.low, b.high, high);
		}
		else
		{
			return ::fast_io::intrinsics::details::umul_a0_least128_generic_emulated(a, b.low, b.high, high);
		}
	}
	else if constexpr (sizeof(T) == sizeof(::std::uint_least64_t))
	{
#ifdef __SIZEOF_INT128__
		if (__builtin_is_constant_evaluated())
		{
			__uint128_t res{a * static_cast<__uint128_t>(b)};
			high = static_cast<U>(res >> 64u);
			return static_cast<T>(res);
		}
		else
		{
#if defined(__cpp_lib_bit_cast)
			if constexpr (::std::endian::native == ::std::endian::little || ::std::endian::native == ::std::endian::big)
			{
				auto u{__builtin_bit_cast(::fast_io::intrinsics::ul64x2, a * static_cast<__uint128_t>(b))};
				high = static_cast<U>(u.high);
				return u.low;
			}
			else
#endif
			{
				__uint128_t res{a * static_cast<__uint128_t>(b)};
				high = static_cast<U>(res >> 64u);
				return static_cast<T>(res);
			}
		}
#elif defined(_MSC_VER) && defined(_M_X64) && !defined(__arm64ec__) && !defined(_M_ARM64EC)
		if (__builtin_is_constant_evaluated())
		{
			return ::fast_io::intrinsics::details::umul_least64_generic_emulated(a, b, high);
		}
		else
		{
			if constexpr (sizeof(U) == sizeof(::std::uint_least64_t))
			{
				return ::fast_io::intrinsics::msvc::x86::_umul128(a, b, __builtin_addressof(high));
			}
			else
			{
				T h;
				T low{::fast_io::intrinsics::msvc::x86::_umul128(a, b, __builtin_addressof(h))};
				high = static_cast<U>(h);
				return low;
			}
		}
#else
		return ::fast_io::intrinsics::details::umul_least64_generic_emulated(a, b, high);
#endif
	}
	else if constexpr (sizeof(T) == sizeof(::std::uint_least32_t))
	{
		return ::fast_io::intrinsics::details::umul_least_generic<::std::uint_least64_t, T>(a, b, high);
	}
	else if constexpr (sizeof(T) == sizeof(::std::uint_least16_t))
	{
		return ::fast_io::intrinsics::details::umul_least_generic<::std::uint_least32_t, T>(a, b, high);
	}
	else if constexpr (sizeof(T) == sizeof(::std::uint_least8_t))
	{
		return ::fast_io::intrinsics::details::umul_least_generic<::std::uint_least16_t, T>(a, b, high);
	}
}

template <::fast_io::intrinsics::details::umul_unsigned_integral U, ::fast_io::intrinsics::details::umul_unsigned_integral T>
	requires(sizeof(U) <= sizeof(T))
inline constexpr T umull(U a, T b) noexcept
{
	if constexpr (::std::same_as<T, ::fast_io::intrinsics::ul64x2>)
	{
		if constexpr (::std::same_as<U, ::fast_io::intrinsics::ul64x2>)
		{
			return ::fast_io::intrinsics::details::umull_least128_generic_emulated(a.low, a.high, b.low, b.high);
		}
		else
		{
			return ::fast_io::intrinsics::details::umull_a0_least128_generic_emulated(a, b.low, b.high);
		}
	}
	else if constexpr (sizeof(T) <= sizeof(::std::uint_least64_t))
	{
		return static_cast<T>(a * b);
	}
}

template <::fast_io::intrinsics::details::umul_unsigned_integral U, ::fast_io::intrinsics::details::umul_unsigned_integral T>
	requires(sizeof(U) <= sizeof(T))
inline constexpr U umulh(U a, T b) noexcept
{
	if constexpr (::std::same_as<T, ::fast_io::intrinsics::ul64x2>)
	{
		if constexpr (::std::same_as<U, ::fast_io::intrinsics::ul64x2>)
		{
			return ::fast_io::intrinsics::details::umulh_least128_generic_emulated(a.low, a.high, b.low, b.high);
		}
		else
		{
			return ::fast_io::intrinsics::details::umulh_a0_least128_generic_emulated(a, b.low, b.high);
		}
	}
	else if constexpr (sizeof(T) == sizeof(::std::uint_least64_t))
	{
#ifdef __SIZEOF_INT128__
		if (__builtin_is_constant_evaluated())
		{
			__uint128_t res{a * static_cast<__uint128_t>(b)};
			return static_cast<U>(res >> 64u);
		}
		else
		{
#if defined(__cpp_lib_bit_cast)
			if constexpr (::std::endian::native == ::std::endian::little || ::std::endian::native == ::std::endian::big)
			{
				auto u{__builtin_bit_cast(::fast_io::intrinsics::ul64x2, a * static_cast<__uint128_t>(b))};
				return static_cast<U>(u.high);
			}
			else
#endif
			{
				__uint128_t res{a * static_cast<__uint128_t>(b)};
				return static_cast<U>(res >> 64u);
			}
		}
#elif defined(_MSC_VER) && defined(_M_X64) && !defined(__arm64ec__) && !defined(_M_ARM64EC)
		if (__builtin_is_constant_evaluated())
		{
			return ::fast_io::intrinsics::details::umulh_least64_generic_emulated(a, b);
		}
		else
		{
			return static_cast<U>(::fast_io::intrinsics::msvc::x86::__umulh(a, b));
		}
#else
		return ::fast_io::intrinsics::details::umulh_least64_generic_emulated(a, b);
#endif
	}
	else if constexpr (sizeof(T) == sizeof(::std::uint_least32_t))
	{
		return ::fast_io::intrinsics::details::umulh_least_generic<::std::uint_least64_t, U>(a, b);
	}
	else if constexpr (sizeof(T) == sizeof(::std::uint_least16_t))
	{
		return ::fast_io::intrinsics::details::umulh_least_generic<::std::uint_least32_t, U>(a, b);
	}
	else if constexpr (sizeof(T) == sizeof(::std::uint_least8_t))
	{
		return ::fast_io::intrinsics::details::umulh_least_generic<::std::uint_least16_t, U>(a, b);
	}
}

} // namespace fast_io::intrinsics
