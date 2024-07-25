#pragma once

namespace fast_io
{

namespace freestanding
{

template <typename R, typename... Args>
struct make_noexcept
{
};

template <typename R, typename... Args>
struct make_noexcept<R(Args...)>
{
	using type = R(Args...) noexcept;
};

template <typename R, typename... Args>
struct make_noexcept<R(Args...) noexcept>
{
	using type = R(Args...) noexcept;
};

template <typename R, typename... Args>
using make_noexcept_t = typename make_noexcept<R, Args...>::type;

template <typename F>
	requires ::std::is_function_v<F>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr auto noexcept_cast(F *f) noexcept
{
#if __cpp_lib_bit_cast >= 201806L
	return __builtin_bit_cast(make_noexcept_t<F> *, f);
#else
	return reinterpret_cast<make_noexcept_t<F> *>(f);
#endif
}

template <typename F, typename... Args>
	requires ::std::is_function_v<F>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811
	constexpr
#endif
	decltype(auto) noexcept_call(F *f, Args &&...args) noexcept
{
#if __cpp_if_consteval >= 202106L
	if consteval
	{
		return f(::std::forward<Args>(args)...); // EH unwinding does not matter here
	}
	else
	{
		return noexcept_cast(f)(::std::forward<Args>(args)...);
	}
#else
#if __cpp_lib_is_constant_evaluated >= 201811
	if (::std::is_constant_evaluated())
	{
		return f(::std::forward<Args>(args)...); // EH unwinding does not matter here
	}
	else
#endif
		return noexcept_cast(f)(::std::forward<Args>(args)...);
#endif
}

}

using ::fast_io::freestanding::noexcept_call;

}
