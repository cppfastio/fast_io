#pragma once

namespace fast_io
{

namespace intrinsics
{

template <typename T>
struct tuint
{
	T quotient, remainder;
};

template <typename T>
struct tuints
{
	T quotientlow, quotienthigh, remainderlow, remainderhigh;
};

/*
Referenced from
https://github.com/llvm/llvm-project/blob/main/compiler-rt/lib/builtins/udivmodti4.c
*/

template <typename T>
inline constexpr tuint<T> udivbigbysmalltosmalldefault(T u1, T u0, T v) noexcept
{
#if defined(__x86_64__) || defined(_M_AMD64)
	if constexpr (sizeof(T) == sizeof(::std::uint_least64_t))
	{
#if defined(__cpp_if_consteval)
		if !consteval
#else
		if (!__builtin_is_constant_evaluated())
#endif
		{
#if defined(_MSC_VER) && !defined(__clang__)
			long long unsigned remainder;
			auto quotient{::fast_io::intrinsics::msvc::x86::_udiv128(u1, u0, v, __builtin_addressof(remainder))};
#else
			T quotient, remainder;
			__asm__("{divq\t%[v]|div\t%[v]}" : "=a"(quotient), "=d"(remainder) : [v] "r"(v), "a"(u0), "d"(u1));
#endif
			return {quotient, remainder};
		}
	}
#endif

	constexpr unsigned n_udword_bits = ::std::numeric_limits<T>::digits;
	constexpr unsigned n_udword_bitsdv2 = n_udword_bits / 2;
	constexpr T one{1u};
	constexpr T b = (one << (n_udword_bits / 2)); // Number base (32 bits)
	constexpr T mask = (b - one);
	T un1, un0;         // Norm. dividend LSD's
	T vn1, vn0;         // Norm. divisor digits
	T q1, q0;           // Quotient digits
	T un64, un21, un10; // Dividend digit pairs
	T rhat;             // A remainder

	unsigned s = static_cast<unsigned>(::std::countl_zero(v)); // Shift amount for normalization
	if (s)
	{
		// Normalize the divisor.
		v = v << s;
		un64 = (u1 << s) | (u0 >> (n_udword_bits - static_cast<unsigned>(s)));
		un10 = u0 << s; // Shift dividend left
	}
	else
	{
		// Avoid undefined behavior of (u0 >> 64).
		un64 = u1;
		un10 = u0;
	}
	// Break divisor up into two 32-bit digits.
	vn1 = v >> n_udword_bitsdv2;
	vn0 = v & mask;
	// Break right half of dividend into two digits.
	un1 = un10 >> n_udword_bitsdv2;
	un0 = un10 & mask;
	// Compute the first quotient digit, q1.
	q1 = un64 / vn1;
	rhat = un64 - q1 * vn1;
	// q1 has at most error 2. No more than 2 iterations.
	while (q1 >= b || q1 * vn0 > b * rhat + un1)
	{
		--q1;
		rhat = rhat + vn1;
		if (rhat >= b)
		{
			break;
		}
	}
	un21 = un64 * b + un1 - q1 * v;
	// Compute the second quotient digit.
	q0 = un21 / vn1;
	rhat = un21 - q0 * vn1;
	// q0 has at most error 2. No more than 2 iterations.
	while (q0 >= b || q0 * vn0 > b * rhat + un0)
	{
		--q0;
		rhat = rhat + vn1;
		if (rhat >= b)
		{
			break;
		}
	}

	return {q1 * b + q0, ((un21 * b + un0 - q0 * v) >> s)};
}

template <typename T>
inline constexpr T shiftleft(T low, T high, unsigned shift) noexcept
{
#if defined(_MSC_VER) && !defined(__clang__) && defined(_M_AMD64)
	if constexpr (sizeof(T) == sizeof(long long unsigned))
	{
#if defined(__cpp_if_consteval)
		if !consteval
#else
		if (!__builtin_is_constant_evaluated())
#endif
		{
			return ::fast_io::intrinsics::msvc::x86::__shiftleft128(low, high, static_cast<char unsigned>(shift));
		}
	}
#endif
	constexpr unsigned n_udword_bits = ::std::numeric_limits<T>::digits;
	if (shift == 0u)
	{
		return high;
	}
	unsigned lowshifter{n_udword_bits - shift};
	return (high << shift) | (low >> lowshifter);
}

template <typename T>
inline constexpr T shiftright(T low, T high, unsigned shift) noexcept
{
#if defined(_MSC_VER) && !defined(__clang__) && defined(_M_AMD64)
	if constexpr (sizeof(T) == sizeof(long long unsigned))
	{
#if defined(__cpp_if_consteval)
		if !consteval
#else
		if (!__builtin_is_constant_evaluated())
#endif
		{
			return ::fast_io::intrinsics::msvc::x86::__shiftright128(low, high, static_cast<char unsigned>(shift));
		}
	}
#endif
	constexpr unsigned n_udword_bits = ::std::numeric_limits<T>::digits;
	if (shift == 0u)
	{
		return low;
	}
	unsigned lowshifter{n_udword_bits - shift};
	return (high << lowshifter) | (low >> shift);
}

template <typename T>
inline constexpr ::fast_io::intrinsics::tuints<T> udivmod(T dividendlow, T dividendhigh, T divisorlow,
														  T divisorhigh) noexcept
{
	if (divisorhigh > dividendhigh || (divisorhigh == dividendhigh && divisorlow > dividendlow))
	{
		return {0, 0, dividendlow, dividendhigh};
	}
	T quotientlow, quotienthigh, remainderlow, remainderhigh;
	// When the divisor fits in 64 bits, we can use an optimized path.
	if (divisorhigh == 0)
	{
		remainderhigh = 0;
		if (dividendhigh < divisorlow)
		{
			// The result fits in 64 bits.
			auto temp = ::fast_io::intrinsics::udivbigbysmalltosmalldefault(dividendhigh, dividendlow, divisorlow);
			quotientlow = temp.quotient;
			remainderlow = temp.remainder;
			quotienthigh = 0;
		}
		else
		{
			// First, divide with the high part to get the remainder in dividendhigh.
			// After that dividendhigh < divisorlow.
			quotienthigh = dividendhigh / divisorlow;
			dividendhigh = dividendhigh % divisorlow;
			auto temp = ::fast_io::intrinsics::udivbigbysmalltosmalldefault(dividendhigh, dividendlow, divisorlow);
			quotientlow = temp.quotient;
			remainderlow = temp.remainder;
		}
		return {quotientlow, quotienthigh, remainderlow, remainderhigh};
	}
	// 0 <= shift <= 63.
	auto shift = static_cast<unsigned>(::std::countl_zero(divisorhigh) - ::std::countl_zero(dividendhigh));

	divisorhigh = ::fast_io::intrinsics::shiftleft(divisorlow, divisorhigh, shift);
	divisorlow <<= shift;

	quotientlow = 0;
	bool carry;
	do
	{
		carry = 0;
		dividendlow = ::fast_io::intrinsics::subc(dividendlow, divisorlow, carry, carry);
		dividendhigh = ::fast_io::intrinsics::subc(dividendhigh, divisorhigh, carry, carry);
		constexpr T zero{};
		T mask{zero - carry};
		T templow{divisorlow & mask}, temphigh{divisorhigh & mask};
		carry = !carry;
		quotientlow = ::fast_io::intrinsics::addc(quotientlow, quotientlow, carry, carry);
		carry = 0;
		dividendlow = ::fast_io::intrinsics::addc(dividendlow, templow, carry, carry);
		dividendhigh = ::fast_io::intrinsics::addc(dividendhigh, temphigh, carry, carry);
		divisorlow = ::fast_io::intrinsics::shiftright(divisorlow, divisorhigh, 1u);
		divisorhigh >>= 1u;
	} while (shift--);
	return {quotientlow, 0, dividendlow, dividendhigh};
}

} // namespace intrinsics

} // namespace fast_io
