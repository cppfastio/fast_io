#pragma once

namespace fast_io
{

using uintfpos_t = ::std::uintmax_t;
using intfpos_t = ::std::intmax_t;

namespace details
{

template<typename T>
concept has_input_stream_ref_define = requires(T&& t)
{
	{input_stream_ref_define(::fast_io::freestanding::forward<T>(t))};
};

template<typename T>
concept has_input_bytes_stream_ref_define = requires(T&& t)
{
	{input_stream_ref_define(::fast_io::freestanding::forward<T>(t))};
};

template<typename T>
concept has_output_stream_ref_define = requires(T&& t)
{
	{output_stream_ref_define(::fast_io::freestanding::forward<T>(t))};
};

template<typename T>
concept has_output_bytes_stream_ref_define = requires(T&& t)
{
	{output_bytes_stream_ref_define(::fast_io::freestanding::forward<T>(t))};
};

template<typename T>
concept has_io_stream_ref_define = requires(T&& t)
{
	{io_stream_ref_define(::fast_io::freestanding::forward<T>(t))};
};

template<typename T>
concept has_io_bytes_stream_ref_define = requires(T&& t)
{
	{io_bytes_stream_ref_define(::fast_io::freestanding::forward<T>(t))};
};

template<typename T>
concept has_input_or_io_stream_ref_define =
	::fast_io::details::has_input_stream_ref_define<T>||
	::fast_io::details::has_io_stream_ref_define<T>;

template<typename T>
concept has_output_or_io_stream_ref_define =
	::fast_io::details::has_output_stream_ref_define<T>||
	::fast_io::details::has_io_stream_ref_define<T>;

template<typename T>
concept has_input_or_io_bytes_stream_ref_define =
	::fast_io::details::has_input_stream_ref_define<T>||
	::fast_io::details::has_io_stream_ref_define<T>;

template<typename T>
concept has_output_or_io_bytes_stream_ref_define =
	::fast_io::details::has_output_stream_ref_define<T>||
	::fast_io::details::has_io_stream_ref_define<T>;

template<typename T>
concept has_status_input_stream_ref_define = requires(T&& t)
{
	{status_input_stream_ref_define(::fast_io::freestanding::forward<T>(t))};
};

template<typename T>
concept has_status_output_stream_ref_define = requires(T&& t)
{
	{status_output_stream_ref_define(::fast_io::freestanding::forward<T>(t))};
};

template<typename T>
concept has_status_io_stream_ref_define = requires(T&& t)
{
	{status_io_stream_ref_define(::fast_io::freestanding::forward<T>(t))};
};

template<typename T>
concept has_status_input_or_io_stream_ref_define =
	::fast_io::details::has_status_input_stream_ref_define<T>||
	::fast_io::details::has_status_io_stream_ref_define<T>;


template<typename T>
concept has_status_output_or_io_stream_ref_define =
	::fast_io::details::has_status_output_stream_ref_define<T>||
	::fast_io::details::has_status_io_stream_ref_define<T>;


}

namespace manipulators
{

template<typename T>
requires (::fast_io::details::has_input_or_io_stream_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) input_stream_ref(T &&t)
{
	if constexpr(::fast_io::details::has_input_stream_ref_define<T>)
	{
		return input_stream_ref_define(::fast_io::freestanding::forward<T>(t));
	}
	else
	{
		return io_stream_ref_define(::fast_io::freestanding::forward<T>(t));
	}
}

template<typename T>
requires (::fast_io::details::has_output_or_io_stream_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) output_stream_ref(T &&t)
{
	if constexpr(::fast_io::details::has_output_stream_ref_define<T>)
	{
		return output_stream_ref_define(::fast_io::freestanding::forward<T>(t));
	}
	else
	{
		return io_stream_ref_define(::fast_io::freestanding::forward<T>(t));
	}
}

template<typename T>
requires (::fast_io::details::has_io_stream_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) io_stream_ref(T &&t)
{
	return io_stream_ref_define(::fast_io::freestanding::forward<T>(t));
}

template<typename T>
requires (::fast_io::details::has_input_or_io_bytes_stream_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) input_bytes_stream_ref(T &&t)
{
	if constexpr(::fast_io::details::has_input_bytes_stream_ref_define<T>)
	{
		return input_bytes_stream_ref_define(::fast_io::freestanding::forward<T>(t));
	}
	else
	{
		return io_bytes_stream_ref_define(::fast_io::freestanding::forward<T>(t));
	}
}

template<typename T>
requires (::fast_io::details::has_output_or_io_bytes_stream_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) output_bytes_stream_ref(T &&t)
{
	if constexpr(::fast_io::details::has_output_bytes_stream_ref_define<T>)
	{
		return output_bytes_stream_ref_define(::fast_io::freestanding::forward<T>(t));
	}
	else
	{
		return io_bytes_stream_ref_define(::fast_io::freestanding::forward<T>(t));
	}
}

template<typename T>
requires (::fast_io::details::has_io_bytes_stream_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) io_bytes_stream_ref(T &&t)
{
	return io_bytes_stream_ref_define(::fast_io::freestanding::forward<T>(t));
}

template<typename T>
requires (::fast_io::details::has_status_input_or_io_stream_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) status_input_stream_ref(T &&t)
{
	if constexpr(::fast_io::details::has_status_input_stream_ref_define<T>)
	{
		return status_input_stream_ref_define(::fast_io::freestanding::forward<T>(t));
	}
	else
	{
		return status_io_stream_ref_define(::fast_io::freestanding::forward<T>(t));
	}
}

template<typename T>
requires (::fast_io::details::has_status_output_or_io_stream_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) status_output_stream_ref(T &&t)
{
	if constexpr(::fast_io::details::has_output_stream_ref_define<T>)
	{
		return status_output_stream_ref_define(::fast_io::freestanding::forward<T>(t));
	}
	else
	{
		return status_io_stream_ref_define(::fast_io::freestanding::forward<T>(t));
	}
}

template<typename T>
requires (::fast_io::details::has_status_io_stream_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) status_io_stream_ref(T &&t)
{
	return status_io_stream_ref_define(::fast_io::freestanding::forward<T>(t));
}

}

namespace details
{

template<typename T>
concept has_input_stream_seek_define = requires(T t)
{
input_stream_seek_define(t,0,::fast_io::seekdir::cur);
};

template<typename T>
concept has_output_stream_seek_define = requires(T t)
{
output_stream_seek_define(t,0,::fast_io::seekdir::cur);
};

template<typename T>
concept has_io_stream_seek_define = requires(T t)
{
io_stream_seek_define(t,0,::fast_io::seekdir::cur);
};

template<typename T>
concept has_input_stream_buffer_flush_define = requires(T t)
{
input_stream_buffer_flush_define(t);
};

template<typename T>
concept has_output_stream_buffer_flush_define = requires(T t)
{
output_stream_buffer_flush_define(t);
};

template<typename T>
concept has_io_stream_buffer_flush_define = requires(T t)
{
io_stream_buffer_flush_define(t);
};

template<typename T>
concept has_input_or_io_stream_buffer_flush_define = ::fast_io::details::has_io_stream_buffer_flush_define<T>
	||::fast_io::details::has_input_stream_buffer_flush_define<T>;

template<typename T>
concept has_output_or_io_stream_buffer_flush_define = ::fast_io::details::has_io_stream_buffer_flush_define<T>
	||::fast_io::details::has_output_stream_buffer_flush_define<T>;

template<typename T>
requires (::fast_io::details::has_output_or_io_stream_buffer_flush_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void input_stream_buffer_flush_impl(T t)
{
	if constexpr(::fast_io::details::has_input_stream_buffer_flush_define<T>)
	{
		return input_stream_buffer_flush_define(t);
	}
	else
	{
		return io_stream_buffer_flush_define(t);
	}
}

template<typename T>
requires (::fast_io::details::has_output_or_io_stream_buffer_flush_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void output_stream_buffer_flush_impl(T t)
{
	if constexpr(::fast_io::details::has_output_stream_buffer_flush_define<T>)
	{
		return output_stream_buffer_flush_define(t);
	}
	else
	{
		return io_stream_buffer_flush_define(t);
	}
}

template<typename T>
requires (::fast_io::details::has_io_stream_seek_define<T>
	||::fast_io::details::has_input_stream_seek_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr intfpos_t input_stream_seek_impl(T t,intfpos_t off,::fast_io::seekdir skd)
{
	if constexpr(::fast_io::details::has_input_or_io_stream_buffer_flush_define<T>)
	{
		::fast_io::details::input_stream_buffer_flush_impl(t);
	}
	if constexpr(::fast_io::details::has_input_stream_seek_define<T>)
	{
		return input_stream_seek_define(t,off,skd);
	}
	else
	{
		return io_stream_seek_define(t,off,skd);
	}
}


template<typename T>
concept has_input_or_io_stream_seek_define = ::fast_io::details::has_io_stream_seek_define<T>
	||::fast_io::details::has_input_stream_seek_define<T>;

template<typename T>
concept has_output_or_io_stream_seek_define = ::fast_io::details::has_io_stream_seek_define<T>
	||::fast_io::details::has_output_stream_seek_define<T>;

template<typename T>
requires (::fast_io::details::has_output_or_io_stream_seek_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr intfpos_t output_stream_seek_impl(T t,intfpos_t off,::fast_io::seekdir skd)
{
	if constexpr(::fast_io::details::has_output_or_io_stream_buffer_flush_define<T>)
	{
		::fast_io::details::output_stream_buffer_flush_impl(t);
	}
	if constexpr(::fast_io::details::has_output_stream_seek_define<T>)
	{
		return output_stream_seek_define(t,off,skd);
	}
	else
	{
		return io_stream_seek_define(t,off,skd);
	}
}

template<typename T>
requires (::fast_io::details::has_io_stream_seek_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr intfpos_t io_stream_seek_impl(T t,intfpos_t off,::fast_io::seekdir skd)
{
	if constexpr(::fast_io::details::has_io_stream_buffer_flush_define<T>)
	{
		io_stream_buffer_flush_define(t);
	}
	return io_stream_seek_define(t,off,skd);
}



template<typename T>
concept has_input_stream_seek_bytes_define = requires(T t)
{
input_stream_seek_bytes_define(t,0,::fast_io::seekdir::cur);
};

template<typename T>
concept has_output_stream_seek_bytes_define = requires(T t)
{
output_stream_seek_bytes_define(t,0,::fast_io::seekdir::cur);
};

template<typename T>
concept has_io_stream_seek_bytes_define = requires(T t)
{
io_stream_seek_bytes_define(t,0,::fast_io::seekdir::cur);
};

template<typename T>
concept has_input_or_io_stream_seek_bytes_define = ::fast_io::details::has_io_stream_seek_bytes_define<T>
	||::fast_io::details::has_input_stream_seek_bytes_define<T>;

template<typename T>
requires (::fast_io::details::has_input_or_io_stream_seek_bytes_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::intfpos_t input_stream_seek_bytes_impl(T t,intfpos_t off,::fast_io::seekdir skd)
{
	if constexpr(::fast_io::details::has_input_or_io_stream_buffer_flush_define<T>)
	{
		::fast_io::details::input_stream_buffer_flush_impl(t);
	}
	if constexpr(::fast_io::details::has_input_stream_seek_bytes_define<T>)
	{
		return input_stream_seek_bytes_define(t,off,skd);
	}
	else
	{
		return io_stream_seek_bytes_define(t,off,skd);
	}
}

template<typename T>
concept has_output_or_io_stream_seek_bytes_define = ::fast_io::details::has_io_stream_seek_bytes_define<T>
	||::fast_io::details::has_output_stream_seek_bytes_define<T>;

template<typename T>
requires (has_output_or_io_stream_seek_bytes_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr intfpos_t output_stream_seek_bytes_impl(T t,intfpos_t off,::fast_io::seekdir skd)
{
	if constexpr(::fast_io::details::has_output_or_io_stream_buffer_flush_define<T>)
	{
		::fast_io::details::output_stream_buffer_flush_impl(t);
	}
	if constexpr(::fast_io::details::has_output_stream_seek_bytes_define<T>)
	{
		return output_stream_seek_bytes_define(t,off,skd);
	}
	else
	{
		return io_stream_seek_bytes_define(t,off,skd);
	}
}

template<typename T>
requires (::fast_io::details::has_io_stream_seek_bytes_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr intfpos_t io_stream_seek_bytes_impl(T t,intfpos_t off,::fast_io::seekdir skd)
{
	if constexpr(::fast_io::details::has_io_stream_buffer_flush_define<T>)
	{
		io_stream_buffer_flush_define(t);
	}
	return io_stream_seek_bytes_define(t,off,skd);
}

template<typename T>
concept has_input_stream_mutex_ref_define = requires(T t)
{
input_stream_mutex_ref_define(t);
input_stream_unlocked_ref_define(t);
};

template<typename T>
concept has_output_stream_mutex_ref_define = requires(T t)
{
output_stream_mutex_ref_define(t);
output_stream_unlocked_ref_define(t);
};

template<typename T>
concept has_io_stream_mutex_ref_define = requires(T t)
{
io_stream_mutex_ref_define(t);
io_stream_unlocked_ref_define(t);
};

template<typename T>
concept has_input_or_io_stream_mutex_ref_define = has_input_stream_mutex_ref_define<T>||
	has_io_stream_mutex_ref_define<T>;

template<typename T>
concept has_output_or_io_stream_mutex_ref_define = has_output_stream_mutex_ref_define<T>||
	has_io_stream_mutex_ref_define<T>;

template<typename T>
requires (::fast_io::details::has_input_or_io_stream_mutex_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) input_stream_mutex_ref_impl(T t)
{
	if constexpr(::fast_io::details::has_input_stream_mutex_ref_define<T>)
	{
		return input_stream_mutex_ref_define(t);
	}
	else
	{
		return io_stream_mutex_ref_define(t);
	}
}

template<typename T>
requires (::fast_io::details::has_output_or_io_stream_mutex_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) output_stream_mutex_ref_impl(T t)
{
	if constexpr(::fast_io::details::has_output_stream_mutex_ref_define<T>)
	{
		return output_stream_mutex_ref_define(t);
	}
	else
	{
		return io_stream_mutex_ref_define(t);
	}
}

template<typename T>
requires (::fast_io::details::has_io_stream_mutex_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) io_stream_mutex_ref_impl(T t)
{
	return io_stream_mutex_ref_define(t);
}

template<typename T>
requires (::fast_io::details::has_input_or_io_stream_mutex_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr auto input_stream_unlocked_ref_impl(T t)
{
	if constexpr(::fast_io::details::has_input_stream_mutex_ref_define<T>)
	{
		return input_stream_unlocked_ref_define(t);
	}
	else
	{
		return io_stream_unlocked_ref_define(t);
	}
}

template<typename T>
requires (::fast_io::details::has_output_or_io_stream_mutex_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr auto output_stream_unlocked_ref_impl(T t)
{
	if constexpr(::fast_io::details::has_output_stream_mutex_ref_define<T>)
	{
		return output_stream_unlocked_ref_define(t);
	}
	else
	{
		return io_stream_unlocked_ref_define(t);
	}
}

template<typename T>
requires (::fast_io::details::has_io_stream_mutex_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr auto io_stream_unlocked_ref_impl(T t)
{
	return io_stream_unlocked_ref_define(t);
}

}

}
