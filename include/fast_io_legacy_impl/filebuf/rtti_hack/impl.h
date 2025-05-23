#pragma once

#include <typeinfo>

#if defined(_MSC_VER)
#include "msvc.h"
#else
#include "itanium.h"
#endif

namespace fast_io::details
{

template <::std::integral char_type>
inline constexpr char8_t *compute_itanium_char_type_identification(char8_t *it) noexcept
{
	if constexpr (::std::same_as<char_type, char>)
	{
		*it = u8'c';
		++it;
	}
	else if constexpr (::std::same_as<char_type, wchar_t>)
	{
		*it = u8'w';
		++it;
	}
	else
	{
		*it = u8'D';
		++it;
		if constexpr (::std::same_as<char_type, char8_t>)
		{
			*it = u8'u';
		}
		else if constexpr (::std::same_as<char_type, char16_t>)
		{
			*it = u8's';
		}
		else
		{
			*it = u8'i';
		}
		++it;
	}
	return it;
}

template <::std::size_t n>
struct symbol_name_holder
{
	char8_t const *ptr{};
	inline constexpr auto data() const noexcept
	{
		return ptr;
	}
	inline static constexpr ::std::size_t size() noexcept
	{
		return n;
	}
};

template <::std::size_t n>
inline constexpr symbol_name_holder<n - 1> compute_symbol_name(char8_t const (&s)[n]) noexcept
{
	return {s};
}

inline constexpr bool symbol_cmp_equal_commom(char8_t const *sym, char const *strp, ::std::size_t len, ::std::size_t N) noexcept
{
	if (N == len)
	{
#if __cpp_if_consteval >= 202106L
		if !consteval
#else
		if(!__builtin_is_constant_evaluated())
#endif
		{
			for (::std::size_t i{}; i != len; ++i)
			{
				if (sym[i] != static_cast<char8_t>(strp[i]))
				{
					return false;
				}
			}
			return true;
		}
		else
		{
#if FAST_IO_HAS_BUILTIN(__builtin_memcmp)
			return __builtin_memcmp(sym, strp, N) == 0;
#else
			return ::std::memcmp(sym, strp, N) == 0;
#endif
		}
	}
	return false;
}

template <::std::size_t N>
inline constexpr bool symbol_cmp_equal(symbol_name_holder<N> const &sym, char const *strp, ::std::size_t len) noexcept
{
	return ::fast_io::details::symbol_cmp_equal_commom(sym.ptr, strp, len, N);
}

template <::std::size_t N>
inline constexpr bool symbol_cmp_equal(::fast_io::freestanding::array<char8_t, N> const &sym, char const *strp, ::std::size_t len) noexcept
{
	return ::fast_io::details::symbol_cmp_equal_commom(sym.data(), strp, len, N);
}

template <::std::integral char_type>
inline constexpr ::std::size_t itanium_char_type_symbol_size{
	(::std::same_as<char_type, char> || ::std::same_as<char_type, wchar_t>) ? 1 : 2};
} // namespace fast_io::details
