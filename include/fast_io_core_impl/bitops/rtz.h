#pragma once
/*
Reference from
https://github.com/cppalliance/decimal/blob/develop/include/boost/decimal/detail/remove_trailing_zeros.hpp
granlund_montgomery_branchless
*/
namespace fast_io::bitops
{

template <typename T>
struct rtz_result
{
	using value_type = T;
	T value;
	std::size_t n;
};

namespace details
{

template <bool iec559, typename T>
inline constexpr ::fast_io::bitops::rtz_result<T> rtz_impl(T n) noexcept
{
	if constexpr (::std::numeric_limits<T>::digits == 64)
	{
		if constexpr (!::std::same_as<T, ::std::uint_least64_t>)
		{
#ifdef __cpp_lib_bit_cast
			return __builtin_bit_cast(::fast_io::bitops::rtz_result<T>,
									  ::fast_io::bitops::details::rtz_impl<iec559>(static_cast<::std::uint_least64_t>(n)));
#else
			auto [v, s] = ::fast_io::bitops::details::rtz_impl<iec559>(static_cast<::std::uint_least64_t>(n));
			return {v, s};
#endif
		}
		else
		{
			std::size_t s{};

			if constexpr (!iec559)
			{
				constexpr ::std::uint_least64_t c0{230079197716545u};
				::std::uint_least64_t r{::std::rotr(static_cast<std::uint64_t>(n * c0), 16)};
				constexpr ::std::uint_least64_t d0{1845u};
				bool b{r < d0};
				s += s;
				s += b;
				n = b ? r : n;
			}

			constexpr ::std::uint_least64_t c0{28999941890838049u};
			::std::uint_least64_t r{::std::rotr(static_cast<std::uint64_t>(n * c0), 8)};
			constexpr ::std::uint_least64_t d0{184467440738u};
			bool b{r < d0};
			s += s;
			s += b;
			n = b ? r : n;

			constexpr ::std::uint_least64_t c1{182622766329724561u};
			r = ::std::rotr(static_cast<std::uint64_t>(n * c1), 4);
			constexpr ::std::uint_least64_t d1{1844674407370956u};
			b = r < d1;
			s += s;
			s += b;
			n = b ? r : n;

			constexpr ::std::uint_least64_t c2{10330176681277348905u};
			r = ::std::rotr(static_cast<std::uint64_t>(n * c2), 2);
			constexpr ::std::uint_least64_t d2{184467440737095517u};
			b = r < d2;
			s += s;
			s += b;
			n = b ? r : n;

			constexpr ::std::uint_least64_t c3{14757395258967641293u};
			r = ::std::rotr(static_cast<std::uint64_t>(n * c3), 1);
			constexpr ::std::uint_least64_t d3{1844674407370955162u};
			b = r < d3;
			s += s;
			s += b;
			n = b ? r : n;

			return {n, s};
		}
	}
	else if constexpr (::std::numeric_limits<T>::digits == 32)
	{
		if constexpr (!::std::same_as<T, ::std::uint_least32_t>)
		{
#ifdef __cpp_lib_bit_cast
			return __builtin_bit_cast(::fast_io::bitops::rtz_result<T>,
									  ::fast_io::bitops::details::rtz_impl<iec559>(static_cast<::std::uint_least32_t>(n)));
#else
			auto [v, s] = ::fast_io::bitops::details::rtz_impl<iec559>(static_cast<::std::uint_least32_t>(n));
			return {v, s};
#endif
		}
		else
		{
			std::size_t s{};

			if constexpr (!iec559)
			{
				constexpr ::std::uint_least64_t c0{15273505u};
				::std::uint_least64_t r{::std::rotr(static_cast<std::uint32_t>(n * c0), 8)};
				constexpr ::std::uint_least64_t d0{43u};
				bool b{r < d0};
				s += s;
				s += b;
				n = b ? r : n;
			}

			constexpr ::std::uint_least32_t c0{184254097u};
			::std::uint_least32_t r{::std::rotr(static_cast<std::uint32_t>(n * c0), 4)};
			constexpr ::std::uint_least32_t d0{429497u};
			bool b{r < d0};
			s += s;
			s += b;
			n = b ? r : n;

			// c1 == d1 == 42949673u
			constexpr ::std::uint_least32_t d1{42949673u};
			r = ::std::rotr(static_cast<std::uint32_t>(n * d1), 2);
			b = r < d1;
			s += s;
			s += b;
			n = b ? r : n;

			constexpr ::std::uint_least32_t c2{1288490189u};
			r = ::std::rotr(static_cast<std::uint32_t>(n * c2), 1);
			constexpr ::std::uint_least32_t d2{429496730u};
			b = r < d2;
			s += s;
			s += b;
			n = b ? r : n;

			return {n, s};
		}
	}
#if 0
	else if constexpr (::std::numeric_limits<T>::digits == 16)
	{
		if constexpr (!::std::same_as<T, ::std::uint_least16_t>)
		{
#ifdef __cpp_lib_bit_cast
			return __builtin_bit_cast(::fast_io::bitops::rtz_result<T>,
									  ::fast_io::bitops::details::rtz_impl<iec559>(static_cast<::std::uint_least32_t>(n)));
#else
			auto [v, s] = ::fast_io::bitops::details::rtz_impl<false>(static_cast<::std::uint_least32_t>(n));
			return {v, s};
#endif
		}
		else
		{

		}
	}
#endif
	else if constexpr (::std::numeric_limits<T>::digits == 16 || ::std::numeric_limits<T>::digits == 8)
	{
		auto [v, s] = ::fast_io::bitops::details::rtz_impl<true>(static_cast<::std::uint_least32_t>(n));
		return {static_cast<T>(v), s};
	}
	else
	{
		::std::size_t s{};
		while (true)
		{
			auto const r{n % 100};
			if (r == 0)
			{
				n /= 100;
				s += 2;
			}
			else
			{
				break;
			}
		}
		auto const r{n % 10};
		if (r == 0)
		{
			n /= 10;
			++s;
		}
		return {n, s};
	}
}


} // namespace details

template <typename T>
inline constexpr ::fast_io::bitops::rtz_result<T> rtz_iec559(T n) noexcept
{
	return ::fast_io::bitops::details::rtz_impl<true>(n);
}

template <typename T>
inline constexpr ::fast_io::bitops::rtz_result<T> rtz(T n) noexcept
{
	return ::fast_io::bitops::details::rtz_impl<false>(n);
}

} // namespace fast_io::bitops