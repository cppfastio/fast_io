#pragma once

namespace fast_io::operations
{

template<typename T>
requires (::fast_io::details::has_io_stream_mutex_ref_define<T>
	||::fast_io::details::has_input_stream_mutex_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) input_stream_mutex_ref(T&& t)
{
	if constexpr(::fast_io::details::has_input_stream_mutex_ref_define<T>)
	{
		return input_stream_mutex_ref_define(::fast_io::freestanding::forward<T>(t));
	}
	else
	{
		return io_stream_mutex_ref_define(::fast_io::freestanding::forward<T>(t));
	}
}

template<typename T>
requires (::fast_io::details::has_io_stream_mutex_ref_define<T>
	||::fast_io::details::has_output_stream_mutex_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) output_stream_mutex_ref(T&& t)
{
	if constexpr(::fast_io::details::has_output_stream_mutex_ref_define<T>)
	{
		return output_stream_mutex_ref_define(::fast_io::freestanding::forward<T>(t));
	}
	else
	{
		return io_stream_mutex_ref_define(::fast_io::freestanding::forward<T>(t));
	}
}

template<typename T>
requires (::fast_io::details::has_io_stream_mutex_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) io_stream_mutex_ref(T&& t)
{
	return io_stream_mutex_ref_define(::fast_io::freestanding::forward<T>(t));
}


template<typename T>
requires (::fast_io::details::has_io_stream_mutex_ref_define<T>
	||::fast_io::details::has_input_stream_mutex_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr auto input_stream_unlocked_ref(T&& t)
{
	if constexpr(::fast_io::details::has_input_stream_mutex_ref_define<T>)
	{
		return input_stream_unlocked_ref_define(::fast_io::freestanding::forward<T>(t));
	}
	else
	{
		return io_stream_unlocked_ref_define(::fast_io::freestanding::forward<T>(t));
	}
}

template<typename T>
requires (::fast_io::details::has_io_stream_mutex_ref_define<T>
	||::fast_io::details::has_output_stream_mutex_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr auto output_stream_unlocked_ref(T&& t)
{
	if constexpr(::fast_io::details::has_output_stream_mutex_ref_define<T>)
	{
		return output_stream_unlocked_ref_define(::fast_io::freestanding::forward<T>(t));
	}
	else
	{
		return io_stream_unlocked_ref_define(::fast_io::freestanding::forward<T>(t));
	}
}

template<typename T>
requires (::fast_io::details::has_io_stream_mutex_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr auto io_stream_unlocked_ref(T&& t)
{
	return io_stream_unlocked_ref_define(::fast_io::freestanding::forward<T>(t));
}

template<typename mtx_type>
struct stream_ref_lock_guard
{
	using mutex_type = mtx_type;
	mutex_type device;
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
	explicit constexpr stream_ref_lock_guard(mutex_type d): device(d)
	{
		device.lock();
	}
	stream_ref_lock_guard(stream_ref_lock_guard const&)=delete;
	stream_ref_lock_guard& operator=(stream_ref_lock_guard const&)=delete;
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
	constexpr ~stream_ref_lock_guard()
	{
		device.unlock();
	}
};

}
