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

namespace fast_io::details
{

template<typename outstmtype,typename Func>
inline constexpr decltype(auto) lock_output_stream_callback(outstmtype outstm,Func fnc)
{
	::fast_io::operations::stream_ref_lock_guard lg{output_stream_mutex_ref_impl(outstm)};
	return fnc(::fast_io::details::output_stream_unlocked_ref_impl(outstm));
};

template<::fast_io::details::has_output_stream_mutex_ref_define T>
using output_stream_unlocked_ref_type = decltype(::fast_io::details::output_stream_unlocked_ref_impl(
	*static_cast<T*>(nullptr)
));

template<typename T>
concept mutex_unlocked_buffer_output_stream_impl = ::fast_io::details::has_output_stream_mutex_ref_define<T>&&
	::fast_io::details::streamreflect::has_obuffer_ops<output_stream_unlocked_ref_type<T>>;

template<typename outstmtype,typename Func>
inline constexpr decltype(auto) lock_input_stream_callback(outstmtype outstm,Func fnc)
{
	::fast_io::operations::stream_ref_lock_guard lg{input_stream_mutex_ref_impl(outstm)};
	return fnc(::fast_io::details::input_stream_unlocked_ref_impl(outstm));
};

template<::fast_io::details::has_input_stream_mutex_ref_define T>
using input_stream_unlocked_ref_type = decltype(::fast_io::details::input_stream_unlocked_ref_impl(
	*static_cast<T*>(nullptr)
));

template<typename T>
concept mutex_unlocked_buffer_input_stream_impl = ::fast_io::details::has_input_stream_mutex_ref_define<T>&&
	::fast_io::details::streamreflect::has_ibuffer_ops<input_stream_unlocked_ref_type<T>>;


template<typename outstmtype,typename Func>
inline constexpr decltype(auto) lock_io_stream_callback(outstmtype outstm,Func fnc)
{
	::fast_io::operations::stream_ref_lock_guard lg{io_stream_mutex_ref_impl(outstm)};
	return fnc(::fast_io::details::io_stream_unlocked_ref_impl(outstm));
};

template<::fast_io::details::has_io_stream_mutex_ref_define T>
using io_stream_unlocked_ref_type = decltype(::fast_io::details::io_stream_unlocked_ref_impl(
	*static_cast<T*>(nullptr)
));

}
