#pragma once


namespace fast_io
{

namespace operations::decay
{

namespace defines
{

template<typename T>
concept has_input_stream_mutex_ref_define = requires(T t)
{
input_stream_mutex_ref_define(t);
};

template<typename T>
concept has_output_stream_mutex_ref_define = requires(T t)
{
output_stream_mutex_ref_define(t);
};

template<typename T>
concept has_io_stream_mutex_ref_define = requires(T t)
{
io_stream_mutex_ref_define(t);
};

template<typename T>
concept has_input_or_io_stream_mutex_ref_define = has_input_stream_mutex_ref_define<T>||
	has_io_stream_mutex_ref_define<T>;

template<typename T>
concept has_output_or_io_stream_mutex_ref_define = has_output_stream_mutex_ref_define<T>||
	has_io_stream_mutex_ref_define<T>;



template<typename T>
concept has_input_stream_unlocked_ref_define = requires(T t)
{
input_stream_unlocked_ref_define(t);
};

template<typename T>
concept has_output_stream_unlocked_ref_define = requires(T t)
{
output_stream_unlocked_ref_define(t);
};

template<typename T>
concept has_io_stream_unlocked_ref_define = requires(T t)
{
io_stream_unlocked_ref_define(t);
};

template<typename T>
concept has_input_or_io_stream_unlocked_ref_define = has_input_stream_unlocked_ref_define<T>||
	has_io_stream_unlocked_ref_define<T>;

template<typename T>
concept has_output_or_io_stream_unlocked_ref_define = has_output_stream_unlocked_ref_define<T>||
	has_io_stream_unlocked_ref_define<T>;


}

template<typename T>
requires (::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) output_stream_mutex_ref_decay(T t)
{
	if constexpr(::fast_io::operations::decay::defines::has_output_stream_mutex_ref_define<T>)
	{
		return output_stream_mutex_ref_define(t);
	}
	else
	{
		return io_stream_mutex_ref_define(t);
	}
}

template<typename T>
requires (::fast_io::operations::decay::defines::has_input_or_io_stream_mutex_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) input_stream_mutex_ref_decay(T t)
{
	if constexpr(::fast_io::operations::decay::defines::has_input_stream_mutex_ref_define<T>)
	{
		return input_stream_mutex_ref_define(t);
	}
	else
	{
		return io_stream_mutex_ref_define(t);
	}
}

template<typename T>
requires (::fast_io::operations::decay::defines::has_io_stream_mutex_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) io_stream_mutex_ref_decay(T t)
{
	return io_stream_mutex_ref_define(t);
}


template<typename T>
requires (::fast_io::operations::decay::defines::has_output_or_io_stream_unlocked_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) output_stream_unlocked_ref_decay(T t)
{
	if constexpr(::fast_io::operations::decay::defines::has_output_stream_unlocked_ref_define<T>)
	{
		return output_stream_unlocked_ref_define(t);
	}
	else
	{
		return io_stream_unlocked_ref_define(t);
	}
}

template<typename T>
requires (::fast_io::operations::decay::defines::has_input_or_io_stream_unlocked_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) input_stream_unlocked_ref_decay(T t)
{
	if constexpr(::fast_io::operations::decay::defines::has_input_stream_unlocked_ref_define<T>)
	{
		return input_stream_unlocked_ref_define(t);
	}
	else
	{
		return io_stream_unlocked_ref_define(t);
	}
}

template<typename T>
requires (::fast_io::operations::decay::defines::has_io_stream_unlocked_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) io_stream_unlocked_ref_unlocked_decay(T t)
{
	return io_stream_unlocked_ref_define(t);
}



}
}
