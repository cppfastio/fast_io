#pragma once

/*
https://gcc.gnu.org/onlinedocs/gcc/Integer-Overflow-Builtins.html

GCC document says we can use __builtin_add_overflow for adc pattern.
No need __builtin_addc which is complicated.
*/

namespace fast_io
{

namespace intrinsics
{

template<typename T>
inline constexpr T addc(T a,T b,T carryin,T& carryout) noexcept
{
#if __has_cpp_attribute(assume)
	[[assume(carryin==0||carryin==1)]];
#endif

#if defined(__cpp_if_consteval)
	if !consteval
#else
	if(!__builtin_is_constant_evaluated())
#endif
	{
#if defined(__has_builtin)
#if __has_builtin(__builtin_add_overflow)
	T s;
	auto c1 = __builtin_add_overflow(a, b, __builtin_addressof(s));
	auto c2 = __builtin_add_overflow(s, carryin, __builtin_addressof(s));
	carryout = c1 | c2;
	return s;
#endif
#endif
	}
	a+=b;
	carryout=a<b;
	a+=carryin;
	carryout+=a<carryin;
	return a;
}

template<typename T>
inline constexpr T subc(T a,T b,T carryin,T& carryout) noexcept
{
#if __has_cpp_attribute(assume)
	[[assume(carryin==0||carryin==1)]];
#endif

#if defined(__cpp_if_consteval)
	if !consteval
#else
	if(!__builtin_is_constant_evaluated())
#endif
	{
#if defined(__has_builtin)
#if __has_builtin(__builtin_sub_overflow)
	T s;
	auto c1 = __builtin_sub_overflow (a, b, __builtin_addressof(s));
	auto c2 = __builtin_sub_overflow (s, carryin, __builtin_addressof(s));
	carryout = c1 | c2;
	return s;
#endif
#endif
	}
	b=a-b;
	carryout=a<b;
	a=b-carryin;
	carryout+=b<a;
	return a;
}

}

}
