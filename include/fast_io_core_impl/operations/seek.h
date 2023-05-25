#pragma once

namespace fast_io::operations
{

namespace decay
{

template<typename T>
requires (::fast_io::details::has_input_or_io_stream_seek_bytes_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::intfpos_t input_stream_seek_bytes_decay(T t,::fast_io::intfpos_t off,::fast_io::seekdir skd)
{
	if constexpr(::fast_io::details::has_input_or_io_stream_seek_bytes_define<T>)
	{
		if constexpr(::fast_io::details::streamreflect::has_ibuffer_ops<T>)
		{
			if(skd==::fast_io::seekdir::cur)
			{
				off=::fast_io::details::adjust_instm_offset(ibuffer_end(t)-ibuffer_curr(t));
			}
		}
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
	else if constexpr(::fast_io::details::has_input_or_io_stream_mutex_ref_define<T>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::input_stream_mutex_ref_decay(t)};
		return ::fast_io::operations::decay::input_stream_seek_bytes_decay(
			::fast_io::operations::decay::input_stream_unlocked_ref_decay(t),off,skd);
	}
}

template<typename T>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void input_stream_rewind_bytes_decay(T t)
{
	::fast_io::operations::decay::input_stream_seek_bytes_decay(t,0,::fast_io::seekdir::beg);
}

template<typename T>
requires (::fast_io::details::has_output_or_io_stream_seek_bytes_define<T>||
	::fast_io::details::has_output_or_io_stream_mutex_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::intfpos_t output_stream_seek_bytes_decay(T t,::fast_io::intfpos_t off,::fast_io::seekdir skd)
{
	if constexpr(::fast_io::details::has_output_or_io_stream_seek_bytes_define<T>)
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
	else if constexpr(::fast_io::details::has_output_or_io_stream_mutex_ref_define<T>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::input_stream_mutex_ref_decay(t)};
		return ::fast_io::operations::decay::output_stream_seek_bytes_decay(
			::fast_io::operations::decay::output_stream_unlocked_ref_decay(t),off,skd);
	}
}

template<typename T>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void output_stream_rewind_bytes_decay(T t)
{
	::fast_io::operations::decay::output_stream_seek_bytes_decay(t,0,::fast_io::seekdir::beg);
}

template<typename T>
requires (::fast_io::details::has_io_stream_mutex_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::intfpos_t io_stream_seek_bytes_decay(T t,::fast_io::intfpos_t off,::fast_io::seekdir skd)
{
	if constexpr(::fast_io::details::has_io_stream_mutex_ref_define<T>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::input_stream_mutex_ref_decay(t)};
		return ::fast_io::operations::decay::output_stream_seek_bytes_decay(
			::fast_io::operations::decay::output_stream_unlocked_ref_decay(t),off,skd);
	}
	else
	{
		if constexpr(::fast_io::details::streamreflect::has_ibuffer_ops<T>)
		{
			if(skd==::fast_io::seekdir::cur)
			{
				off=::fast_io::details::adjust_instm_offset(ibuffer_end(t)-ibuffer_curr(t));
			}
		}
		if constexpr(::fast_io::details::has_io_stream_buffer_flush_define<T>)
		{
			io_stream_buffer_flush_define(t);
		}
		return io_stream_seek_bytes_define(t,off,skd);
	}
}

template<typename T>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void io_stream_rewind_bytes_decay(T t)
{
	::fast_io::operations::decay::io_stream_seek_bytes_decay(t,0,::fast_io::seekdir::beg);
}

template<typename T>
requires (::fast_io::details::has_input_or_io_stream_seek_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::intfpos_t input_stream_seek_decay(T t,::fast_io::intfpos_t off,::fast_io::seekdir skd)
{
	if constexpr(::fast_io::details::has_input_or_io_stream_seek_define<T>)
	{
		if constexpr(::fast_io::details::streamreflect::has_ibuffer_ops<T>)
		{
			if(skd==::fast_io::seekdir::cur)
			{
				off=::fast_io::details::adjust_instm_offset(ibuffer_end(t)-ibuffer_curr(t));
			}
		}
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
	else if constexpr(::fast_io::details::has_input_or_io_stream_mutex_ref_define<T>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::input_stream_mutex_ref_decay(t)};
		return ::fast_io::operations::decay::input_stream_seek_decay(
			::fast_io::operations::decay::input_stream_unlocked_ref_decay(t),off,skd);
	}
}

template<typename T>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void input_stream_rewind_decay(T t)
{
	::fast_io::operations::decay::input_stream_seek_decay(t,0,::fast_io::seekdir::beg);
}

template<typename T>
requires (::fast_io::details::has_output_or_io_stream_seek_define<T>||
	::fast_io::details::has_output_or_io_stream_mutex_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::intfpos_t output_stream_seek_decay(T t,::fast_io::intfpos_t off,::fast_io::seekdir skd)
{
	if constexpr(::fast_io::details::has_output_or_io_stream_seek_define<T>)
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
	else if constexpr(::fast_io::details::has_output_or_io_stream_mutex_ref_define<T>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::input_stream_mutex_ref_decay(t)};
		return ::fast_io::operations::decay::output_stream_seek_decay(
			::fast_io::operations::decay::output_stream_unlocked_ref_decay(t),off,skd);
	}
}

template<typename T>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void output_stream_rewind_decay(T t)
{
	::fast_io::operations::decay::output_stream_seek_decay(t,0,::fast_io::seekdir::beg);
}

template<typename T>
requires (::fast_io::details::has_io_stream_mutex_ref_define<T>||
	::fast_io::details::has_io_stream_seek_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::intfpos_t io_stream_seek_decay(T t,::fast_io::intfpos_t off,::fast_io::seekdir skd)
{
	if constexpr(::fast_io::details::has_io_stream_mutex_ref_define<T>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{::fast_io::operations::decay::input_stream_mutex_ref_decay(t)};
		return ::fast_io::operations::decay::output_stream_seek_decay(
			::fast_io::operations::decay::output_stream_unlocked_ref_decay(t),off,skd);
	}
	else
	{
		if constexpr(::fast_io::details::streamreflect::has_ibuffer_ops<T>)
		{
			if(skd==::fast_io::seekdir::cur)
			{
				off=::fast_io::details::adjust_instm_offset(ibuffer_end(t)-ibuffer_curr(t));
			}
		}
		if constexpr(::fast_io::details::has_io_stream_buffer_flush_define<T>)
		{
			io_stream_buffer_flush_define(t);
		}
		return io_stream_seek_define(t,off,skd);
	}
}

template<typename T>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void io_stream_rewind_decay(T t)
{
	::fast_io::operations::decay::io_stream_seek_decay(t,0,::fast_io::seekdir::beg);
}

}

template<typename T>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::intfpos_t input_stream_seek_bytes(T&& t,::fast_io::intfpos_t off,::fast_io::seekdir skd)
{
	return ::fast_io::operations::decay::input_stream_seek_bytes_decay(::fast_io::manipulators::input_stream_ref(t),
		off,skd);
}

template<typename T>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::intfpos_t output_stream_seek_bytes(T&& t,::fast_io::intfpos_t off,::fast_io::seekdir skd)
{
	return ::fast_io::operations::decay::output_stream_seek_bytes_decay(::fast_io::manipulators::output_stream_ref(t),
		off,skd);
}

template<typename T>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::intfpos_t io_stream_seek_bytes(T&& t,::fast_io::intfpos_t off,::fast_io::seekdir skd)
{
	return ::fast_io::operations::decay::io_stream_seek_bytes_decay(::fast_io::manipulators::io_stream_ref(t),
		off,skd);
}

template<typename T>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::intfpos_t input_stream_seek(T&& t,::fast_io::intfpos_t off,::fast_io::seekdir skd)
{
	return ::fast_io::operations::decay::input_stream_seek_decay(::fast_io::manipulators::input_stream_ref(t),
		off,skd);
}

template<typename T>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::intfpos_t output_stream_seek(T&& t,::fast_io::intfpos_t off,::fast_io::seekdir skd)
{
	return ::fast_io::operations::decay::output_stream_seek_decay(::fast_io::manipulators::output_stream_ref(t),
		off,skd);
}

template<typename T>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::fast_io::intfpos_t io_stream_seek(T&& t,::fast_io::intfpos_t off,::fast_io::seekdir skd)
{
	return ::fast_io::operations::decay::io_stream_seek_decay(::fast_io::manipulators::io_stream_ref(t),
		off,skd);
}

template<typename T>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void input_stream_rewind_bytes(T&& t)
{
	::fast_io::operations::decay::input_stream_rewind_bytes_decay(::fast_io::manipulators::input_stream_ref(t));
}

template<typename T>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void output_stream_rewind_bytes(T&& t)
{
	::fast_io::operations::decay::output_stream_rewind_bytes_decay(::fast_io::manipulators::output_stream_ref(t));
}

template<typename T>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void io_stream_rewind_bytes(T&& t)
{
	::fast_io::operations::decay::io_stream_rewind_bytes_decay(::fast_io::manipulators::io_stream_ref(t));
}

template<typename T>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void input_stream_rewind(T&& t)
{
	::fast_io::operations::decay::input_stream_rewind_decay(::fast_io::manipulators::input_stream_ref(t));
}

template<typename T>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void output_stream_rewind(T&& t)
{
	::fast_io::operations::decay::output_stream_rewind_decay(::fast_io::manipulators::output_stream_ref(t));
}

template<typename T>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void io_stream_rewind(T&& t)
{
	::fast_io::operations::decay::io_stream_rewind_decay(::fast_io::manipulators::io_stream_ref(t));
}

}
