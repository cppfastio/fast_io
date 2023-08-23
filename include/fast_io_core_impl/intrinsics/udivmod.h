#pragma once

namespace fast_io
{

namespace intrinsics
{

template<typename T>
struct tuint
{
T quotient, remainder;
};

template<typename T>
struct tuints
{
T quotientlow,quotienthigh,remainderlow,remainderhigh;
};

template<typename T>
inline constexpr tuint<T> udivbigbysmalltosmalldefault(T u1, T u0, T v) noexcept
{
constexpr unsigned n_udword_bits = ::std::numeric_limits<T>::digits;
constexpr T one{1u};
constexpr T b = (one << (n_udword_bits / 2)); // Number base (32 bits)
constexpr T mask = (b-one);
T un1, un0;                                // Norm. dividend LSD's
T vn1, vn0;                                // Norm. divisor digits
T q1, q0;                                  // Quotient digits
T un64, un21, un10;                        // Dividend digit pairs
T rhat;                                    // A remainder
int s;                                       // Shift amount for normalization
s = ::std::countl_zero(v);
if (s > 0) {
// Normalize the divisor.
v = v << s;
un64 = (u1 << s) | (u0 >> (n_udword_bits - s));
un10 = u0 << s; // Shift dividend left
} else {
// Avoid undefined behavior of (u0 >> 64).
un64 = u1;
un10 = u0;
}
// Break divisor up into two 32-bit digits.
vn1 = v >> (n_udword_bits / 2);
vn0 = v & mask;
// Break right half of dividend into two digits.
un1 = un10 >> (n_udword_bits / 2);
un0 = un10 & mask;
// Compute the first quotient digit, q1.
q1 = un64 / vn1;
rhat = un64 - q1 * vn1;
// q1 has at most error 2. No more than 2 iterations.
while (q1 >= b || q1 * vn0 > b * rhat + un1) {
--q1;
rhat = rhat + vn1;
if (rhat >= b)
break;
}
un21 = un64 * b + un1 - q1 * v;
// Compute the second quotient digit.
q0 = un21 / vn1;
rhat = un21 - q0 * vn1;
// q0 has at most error 2. No more than 2 iterations.
while (q0 >= b || q0 * vn0 > b * rhat + un0) {
--q0;
rhat = rhat + vn1;
if (rhat >= b)
break;
}

return {q1 * b + q0,((un21 * b + un0 - q0 * v) >> s)};
}

template<typename T>
inline constexpr T shiftleft(T low,T high,unsigned shift) noexcept
{
	constexpr unsigned n_udword_bits = ::std::numeric_limits<T>::digits;
	if(shift==0u)
	{
		return high;
	}
	unsigned lowshifter{n_udword_bits-shift};
	return (high<<shift)|(low>>lowshifter);
}

template<typename T>
inline constexpr T shiftright(T low,T high,unsigned shift) noexcept
{
	constexpr unsigned n_udword_bits = ::std::numeric_limits<T>::digits;
	if(shift==0u)
	{
		return low;
	}
	unsigned lowshifter{n_udword_bits-shift};
	return (high<<lowshifter)|(low>>shift);
}

template<typename T>
inline constexpr tuints<T> udivmod(T dividendlow, T dividendhigh,
		T divisorlow, T divisorhigh) noexcept
{
	constexpr unsigned n_utword_bits = ::std::numeric_limits<T>::digits*2u;

	if (divisorhigh>dividendhigh||
		(divisorhigh==dividendhigh&&divisorlow>dividendlow))
	{
		return {0,0,dividendlow,dividendhigh};
	}
	T quotientlow,quotienthigh,remainderlow,remainderhigh;
	// When the divisor fits in 64 bits, we can use an optimized path.
	if (divisorhigh == 0)
	{
		remainderhigh = 0;
		if (dividendhigh < divisorlow)
		{
		// The result fits in 64 bits.
		auto temp = udivbigbysmalltosmalldefault(dividendhigh, dividendlow,
						divisorlow);
		quotientlow=temp.quotient;
		remainderlow=temp.remainder;
		quotienthigh = 0;
		}
		else
		{
		// First, divide with the high part to get the remainder in dividendhigh.
		// After that dividendhigh < divisorlow.
		quotienthigh = dividendhigh / divisorlow;
		dividendhigh = dividendhigh % divisorlow;
		auto temp = udivbigbysmalltosmalldefault(dividendhigh, dividendlow,
						divisorlow);
		quotientlow=temp.quotient;
		remainderlow=temp.remainder;
		}
		return {quotientlow,quotienthigh,remainderlow,remainderhigh};
	}
	// 0 <= shift <= 63.
	auto shift = ::std::countl_zero(divisorhigh) - ::std::countl_zero(dividendhigh);

	divisorhigh = shiftleft(divisorlow,divisorhigh,shift);
	divisorlow <<= shift;

	quotientlow = 0;
	T carry;
	do
	{
		if constexpr(false)
		{
			quotientlow<<=1;
			if(dividendhigh>divisorhigh||
				dividendhigh==dividendhigh&&dividendlow>=divisorlow)
			{
				carry=0;
				dividendlow=::fast_io::intrinsics::subc(dividendlow,divisorlow,carry,carry);
				dividendhigh=::fast_io::intrinsics::subc(dividendhigh,divisorhigh,carry,carry);
				++quotientlow;
			}
		}
		else
		{
			carry=0;
			dividendlow=::fast_io::intrinsics::subc(dividendlow,divisorlow,carry,carry);
			dividendhigh=::fast_io::intrinsics::subc(dividendhigh,divisorhigh,carry,carry);
			constexpr T zero{};
			T mask{zero-carry};
			T templow{divisorlow&mask},temphigh{divisorhigh&mask};
			carry=!carry;
			quotientlow=::fast_io::intrinsics::addc(quotientlow,quotientlow,carry,carry);
			carry=0;
			dividendlow=::fast_io::intrinsics::addc(dividendlow,templow,carry,carry);
			dividendhigh=::fast_io::intrinsics::addc(dividendhigh,temphigh,carry,carry);
		}
		divisorlow = shiftright(divisorlow,divisorhigh,1u);
		divisorhigh >>= 1u;
	}
	while(shift--);
	return {quotientlow,0,dividendlow,dividendhigh};
}

}

}
