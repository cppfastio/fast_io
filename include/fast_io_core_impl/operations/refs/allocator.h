#pragma once

namespace fast_io
{

namespace operations::decay
{

namespace defines
{

template <typename T>
concept has_input_allocator_type = requires { typename T::input_allocator_type; };

template <typename T>
concept has_output_allocator_type = requires { typename T::output_allocator_type; };

template <typename T>
concept has_io_allocator_type = requires { typename T::io_allocator_type; };

template <typename T>
concept has_allocator_type = requires { typename T::allocator_type; };

template <typename T>
concept has_input_stream_allocator_handle_define = requires(T t) { input_stream_allocator_handle_define(t); };

template <typename T>
concept has_output_stream_allocator_handle_define = requires(T t) { output_stream_allocator_handle_define(t); };

template <typename T>
concept has_io_stream_allocator_handle_define = requires(T t) { io_stream_allocator_handle_define(t); };

template <typename T>
concept has_input_or_io_stream_allocator_handle_define =
	has_input_stream_allocator_handle_define<T> || has_io_stream_allocator_handle_define<T>;

template <typename T>
concept has_output_or_io_stream_allocator_handle_define =
	has_output_stream_allocator_handle_define<T> || has_io_stream_allocator_handle_define<T>;

} // namespace defines

/*
Streams may expose allocator member typedefs to steer internal allocations:
input_allocator_type/output_allocator_type for single direction streams,
io_allocator_type for io streams, or plain allocator_type covering everything.
The most direction specific typedef wins; otherwise allocator_type; otherwise
the fallback allocator.
*/
template <typename T, typename fallbacktype = ::fast_io::native_thread_local_allocator>
struct input_stream_allocator
{
private:
	template <typename U = T>
		requires(defines::has_input_allocator_type<U>)
	static auto pick(int) noexcept -> typename U::input_allocator_type;
	template <typename U = T>
		requires(defines::has_io_allocator_type<U>)
	static auto pick(long) noexcept -> typename U::io_allocator_type;
	template <typename U = T>
		requires(defines::has_allocator_type<U>)
	static auto pick(long long) noexcept -> typename U::allocator_type;
	template <typename U = T>
	static auto pick(...) noexcept -> fallbacktype;

public:
	using type = decltype(pick(0));
};

template <typename T, typename fallbacktype = ::fast_io::native_thread_local_allocator>
struct output_stream_allocator
{
private:
	template <typename U = T>
		requires(defines::has_output_allocator_type<U>)
	static auto pick(int) noexcept -> typename U::output_allocator_type;
	template <typename U = T>
		requires(defines::has_io_allocator_type<U>)
	static auto pick(long) noexcept -> typename U::io_allocator_type;
	template <typename U = T>
		requires(defines::has_allocator_type<U>)
	static auto pick(long long) noexcept -> typename U::allocator_type;
	template <typename U = T>
	static auto pick(...) noexcept -> fallbacktype;

public:
	using type = decltype(pick(0));
};

template <typename T, typename fallbacktype = ::fast_io::native_thread_local_allocator>
using input_stream_allocator_t = typename input_stream_allocator<T, fallbacktype>::type;

template <typename T, typename fallbacktype = ::fast_io::native_thread_local_allocator>
using output_stream_allocator_t = typename output_stream_allocator<T, fallbacktype>::type;

template <typename T>
	requires(::fast_io::operations::decay::defines::has_input_or_io_stream_allocator_handle_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) input_stream_allocator_handle_decay(T t) noexcept
{
	if constexpr (::fast_io::operations::decay::defines::has_input_stream_allocator_handle_define<T>)
	{
		return input_stream_allocator_handle_define(t);
	}
	else
	{
		return io_stream_allocator_handle_define(t);
	}
}

template <typename T>
	requires(::fast_io::operations::decay::defines::has_output_or_io_stream_allocator_handle_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) output_stream_allocator_handle_decay(T t) noexcept
{
	if constexpr (::fast_io::operations::decay::defines::has_output_stream_allocator_handle_define<T>)
	{
		return output_stream_allocator_handle_define(t);
	}
	else
	{
		return io_stream_allocator_handle_define(t);
	}
}

template <typename T>
	requires(::fast_io::operations::decay::defines::has_io_stream_allocator_handle_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) io_stream_allocator_handle_decay(T t) noexcept
{
	return io_stream_allocator_handle_define(t);
}

} // namespace operations::decay

} // namespace fast_io
