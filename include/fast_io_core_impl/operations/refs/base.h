#pragma once

namespace fast_io::operations
{

namespace defines
{

template <typename T>
concept has_input_stream_ref_define = requires(T &&t) {
	{ input_stream_ref_define(::fast_io::freestanding::forward<T>(t)) };
};

template <typename T>
concept has_output_stream_ref_define = requires(T &&t) {
	{ output_stream_ref_define(::fast_io::freestanding::forward<T>(t)) };
};

template <typename T>
concept has_io_stream_ref_define = requires(T &&t) {
	{ io_stream_ref_define(::fast_io::freestanding::forward<T>(t)) };
};

template <typename T>
concept has_input_or_io_stream_ref_define = ::fast_io::operations::defines::has_input_stream_ref_define<T> ||
											::fast_io::operations::defines::has_io_stream_ref_define<T>;

template <typename T>
concept has_output_or_io_stream_ref_define = ::fast_io::operations::defines::has_output_stream_ref_define<T> ||
											 ::fast_io::operations::defines::has_io_stream_ref_define<T>;

} // namespace defines

namespace details
{
template <typename T>
inline constexpr bool input_stream_ref_may_throw{
	[]() consteval -> bool {
		if constexpr (::fast_io::operations::defines::has_input_stream_ref_define<T>)
		{
			return !noexcept(input_stream_ref_define(::std::declval<T>()));
		}
		else
		{
			return !noexcept(io_stream_ref_define(::std::declval<T>()));
		}
	}()};

template <typename T>
inline constexpr bool output_stream_ref_may_throw{
	[]() consteval -> bool {
		if constexpr (::fast_io::operations::defines::has_output_stream_ref_define<T>)
		{
			return !noexcept(output_stream_ref_define(::std::declval<T>()));
		}
		else
		{
			return !noexcept(io_stream_ref_define(::std::declval<T>()));
		}
	}()};

template <typename T>
inline constexpr bool io_stream_ref_may_throw{
	!noexcept(io_stream_ref_define(::std::declval<T>()))};
} // namespace details

template <typename T>
	requires(::fast_io::operations::defines::has_input_or_io_stream_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) input_stream_ref(T &&t)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::operations::details::input_stream_ref_may_throw<T>)
{
	if constexpr (::fast_io::operations::defines::has_input_stream_ref_define<T>)
	{
		return input_stream_ref_define(::fast_io::freestanding::forward<T>(t));
	}
	else
	{
		return io_stream_ref_define(::fast_io::freestanding::forward<T>(t));
	}
}

template <typename T>
	requires(::fast_io::operations::defines::has_output_or_io_stream_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) output_stream_ref(T &&t)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::operations::details::output_stream_ref_may_throw<T>)
{
	if constexpr (::fast_io::operations::defines::has_output_stream_ref_define<T>)
	{
		return output_stream_ref_define(::fast_io::freestanding::forward<T>(t));
	}
	else
	{
		return io_stream_ref_define(::fast_io::freestanding::forward<T>(t));
	}
}

template <typename T>
	requires(::fast_io::operations::defines::has_io_stream_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) io_stream_ref(T &&t)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::operations::details::io_stream_ref_may_throw<T>)
{
	return io_stream_ref_define(::fast_io::freestanding::forward<T>(t));
}

} // namespace fast_io::operations
