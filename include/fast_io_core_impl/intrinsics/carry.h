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

template <typename T>
inline constexpr T addc(T a, T b, bool carryin, bool &carryout) noexcept
{
#if __has_cpp_attribute(assume)
	[[assume(carryin == 0 || carryin == 1)]];
#endif

#if defined(__cpp_if_consteval)
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
#if defined(_MSC_VER) && !defined(__clang__) && (defined(_M_IX86) || defined(_M_AMD64))
#if !__has_cpp_attribute(assume)
		__assume(carryin == 0 || carryin == 1);
#endif
		if constexpr (sizeof(T) == 1)
		{
			char unsigned res;
			carryout = static_cast<bool>(::fast_io::intrinsics::msvc::x86::_addcarry_u8(
				static_cast<char unsigned>(carryin), a, b, __builtin_addressof(res)));
			return res;
		}
		else if constexpr (sizeof(T) == 2)
		{
			short unsigned res;
			carryout = static_cast<bool>(::fast_io::intrinsics::msvc::x86::_addcarry_u16(
				static_cast<char unsigned>(carryin), a, b, __builtin_addressof(res)));
			return res;
		}
		else if constexpr (sizeof(T) == 4)
		{
			unsigned res;
			carryout = static_cast<bool>(::fast_io::intrinsics::msvc::x86::_addcarry_u32(
				static_cast<char unsigned>(carryin), a, b, __builtin_addressof(res)));
			return res;
		}
		else if constexpr (sizeof(T) == 8)
		{
			if constexpr (sizeof(::std::uint_least64_t) <= sizeof(::std::size_t))
			{
				long long unsigned res;
				carryout = static_cast<bool>(::fast_io::intrinsics::msvc::x86::_addcarry_u64(
					static_cast<char unsigned>(carryin), a, b, __builtin_addressof(res)));
				return res;
			}
			else
			{
				unsigned alow = static_cast<unsigned>(a);
				unsigned ahigh = static_cast<unsigned>(a >> 32u);
				unsigned blow = static_cast<unsigned>(b);
				unsigned bhigh = static_cast<unsigned>(b >> 32u);
				unsigned reslow, reshigh;
				bool carrytemp{static_cast<bool>(::fast_io::intrinsics::msvc::x86::_addcarry_u32(
					static_cast<char unsigned>(carryin), alow, blow, __builtin_addressof(reslow)))};
				carryout = ::fast_io::intrinsics::msvc::x86::_addcarry_u32(carrytemp, ahigh, bhigh,
																		   __builtin_addressof(reshigh));
				return (static_cast<long long unsigned>(reshigh) << 32u) | reslow;
			}
		}
#elif defined(__has_builtin)
#if __has_builtin(__builtin_add_overflow)
		T s;
		auto c1 = __builtin_add_overflow(a, b, __builtin_addressof(s));
		auto c2 = __builtin_add_overflow(s, carryin, __builtin_addressof(s));
		carryout = c1 | c2;
		return s;
#endif
#endif
	}
	a += b;
	carryout = a < b;
	a += carryin;
	carryout |= a < static_cast<T>(carryin);
	return a;
}

template <typename T>
inline constexpr T subc(T a, T b, bool carryin, bool &carryout) noexcept
{
#if __has_cpp_attribute(assume)
	[[assume(carryin == 0 || carryin == 1)]];
#endif

#if defined(__cpp_if_consteval)
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
#if defined(_MSC_VER) && !defined(__clang__) && (defined(_M_IX86) || defined(_M_AMD64))
#if !__has_cpp_attribute(assume)
		__assume(carryin == 0 || carryin == 1);
#endif
		if constexpr (sizeof(T) == 1)
		{
			char unsigned res;
			carryout = static_cast<bool>(::fast_io::intrinsics::msvc::x86::_subborrow_u8(
				static_cast<char unsigned>(carryin), a, b, __builtin_addressof(res)));
			return res;
		}
		else if constexpr (sizeof(T) == 2)
		{
			short unsigned res;
			carryout = static_cast<bool>(::fast_io::intrinsics::msvc::x86::_subborrow_u16(
				static_cast<char unsigned>(carryin), a, b, __builtin_addressof(res)));
			return res;
		}
		else if constexpr (sizeof(T) == 4)
		{
			unsigned res;
			carryout = static_cast<bool>(::fast_io::intrinsics::msvc::x86::_subborrow_u32(
				static_cast<char unsigned>(carryin), a, b, __builtin_addressof(res)));
			return res;
		}
		else if constexpr (sizeof(T) == 8)
		{
			if constexpr (sizeof(::std::uint_least64_t) <= sizeof(::std::size_t))
			{
				long long unsigned res;
				carryout = static_cast<bool>(::fast_io::intrinsics::msvc::x86::_subborrow_u64(
					static_cast<char unsigned>(carryin), a, b, __builtin_addressof(res)));
				return res;
			}
			else
			{
				unsigned alow = static_cast<unsigned>(a);
				unsigned ahigh = static_cast<unsigned>(a >> 32u);
				unsigned blow = static_cast<unsigned>(b);
				unsigned bhigh = static_cast<unsigned>(b >> 32u);
				unsigned reslow, reshigh;
				bool carrytemp{static_cast<bool>(::fast_io::intrinsics::msvc::x86::_subborrow_u32(
					static_cast<char unsigned>(carryin), alow, blow, __builtin_addressof(reslow)))};
				carryout = static_cast<bool>(::fast_io::intrinsics::msvc::x86::_subborrow_u32(
					carrytemp, alow, bhigh, __builtin_addressof(reshigh)));
				return (static_cast<long long unsigned>(reshigh) << 32u) | reslow;
			}
		}
#elif defined(__has_builtin)
#if __has_builtin(__builtin_sub_overflow)
		T s;
		auto c1 = __builtin_sub_overflow(a, b, __builtin_addressof(s));
		auto c2 = __builtin_sub_overflow(s, carryin, __builtin_addressof(s));
		carryout = c1 | c2;
		return s;
#endif
#endif
	}
	b = a - b;
	carryout = a < b;
	a = b - carryin;
	carryout |= b < a;
	return a;
}

} // namespace intrinsics

} // namespace fast_io
