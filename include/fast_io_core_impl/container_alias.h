#pragma once

namespace fast_io
{

namespace details
{

template<typename T>
struct containeraliasempty
{
	using emptytype = T;
};

template<typename T>
inline constexpr auto computecontaineraliastype() noexcept
{
	if constexpr(::std::signed_integral<T>)
	{
		if constexpr(sizeof(T)==sizeof(::std::int_least8_t))
		{
			return containeraliasempty<::std::int_least8_t>{};
		}
		else if constexpr(sizeof(T)==sizeof(::std::int_least16_t))
		{
			return containeraliasempty<::std::int_least16_t>{};
		}
		else if constexpr(sizeof(T)==sizeof(::std::int_least32_t))
		{
			return containeraliasempty<::std::int_least32_t>{};
		}
		else if constexpr(sizeof(T)==sizeof(::std::int_least64_t))
		{
			return containeraliasempty<::std::int_least64_t>{};
		}
	}
	else if constexpr(::std::unsigned_integral<T>)
	{
		if constexpr(sizeof(T)==sizeof(::std::uint_least8_t))
		{
			return containeraliasempty<::std::uint_least8_t>{};
		}
		else if constexpr(sizeof(T)==sizeof(::std::uint_least16_t))
		{
			return containeraliasempty<::std::uint_least16_t>{};
		}
		else if constexpr(sizeof(T)==sizeof(::std::uint_least32_t))
		{
			return containeraliasempty<::std::uint_least32_t>{};
		}
		else if constexpr(sizeof(T)==sizeof(::std::uint_least64_t))
		{
			return containeraliasempty<::std::uint_least64_t>{};
		}
	}
	else if constexpr(::std::floating_point<T>)
	{
		if constexpr(::std::numeric_limits<T>::is_iec559)
		{
#ifdef __STDCPP_FLOAT32_T__
			if constexpr(sizeof(T)==sizeof(_Float32))
			{
				return containeraliasempty<_Float32>{};
			}
#endif
#ifdef __STDCPP_FLOAT64_T__
			if constexpr(sizeof(T)==sizeof(_Float64))
			{
				return containeraliasempty<_Float64>{};
			}
#endif
		}
	}
	return containeraliasempty<T>{};
}

}

template<typename T>
requires (!::std::is_const_v<T>)
using container_alias_type = decltype(::fast_io::details::computecontaineraliastype<T>)::emptytype;

}


