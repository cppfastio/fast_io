#pragma once

namespace fast_io
{
namespace decorators
{

struct no_op_decorators
{};

}

namespace operations::decay
{

namespace defines
{

template<typename T,typename P>
concept has_output_stream_add_deco_filter_define = requires(T t,P p)
{
	output_stream_add_deco_filter_define(t,p);
}||(::std::same_as<::std::remove_cvref_t<P>,::fast_io::decorators::no_op_decorators>);

template<typename T,typename P>
concept has_input_stream_add_deco_filter_define = requires(T t,P p)
{
	input_stream_add_deco_filter_define(t,p);
}||(::std::same_as<::std::remove_cvref_t<P>,::fast_io::decorators::no_op_decorators>);

template<typename T,typename P>
concept has_io_stream_add_deco_filter_define = requires(T t,P p)
{
	io_stream_add_deco_filter_define(t,p);
}||(::std::same_as<::std::remove_cvref_t<P>,::fast_io::decorators::no_op_decorators>);

template<typename T,typename P>
concept has_output_or_io_stream_add_deco_filter_define =
	has_output_stream_add_deco_filter_define<T,P>||has_io_stream_add_deco_filter_define<T,P>;

template<typename T,typename P>
concept has_input_or_io_stream_add_deco_filter_define =
	has_input_stream_add_deco_filter_define<T,P>||has_io_stream_add_deco_filter_define<T,P>;

}

template<typename T,typename P>
requires (::fast_io::operations::decay::defines::has_output_stream_add_deco_filter_define<T,P>)
inline constexpr void output_stream_add_deco_filter_decay(T t,P p)
{
	if constexpr(!::std::same_as<::std::remove_cvref_t<P>,::fast_io::decorators::no_op_decorators>)
	{
		if constexpr(::fast_io::operations::decay::defines::has_output_stream_add_deco_filter_define<T,P>)
		{
			output_stream_add_deco_filter_define(t,p);
		}
		else
		{
			io_stream_add_deco_filter_define(t,p);
		}
	}
}

template<typename T,typename P>
requires (::fast_io::operations::decay::defines::has_input_stream_add_deco_filter_define<T,P>)
inline constexpr void input_stream_add_deco_filter_decay(T t,P p)
{
	if constexpr(!::std::same_as<::std::remove_cvref_t<P>,::fast_io::decorators::no_op_decorators>)
	{
		if constexpr(::fast_io::operations::decay::defines::has_input_stream_add_deco_filter_define<T,P>)
		{
			input_stream_add_deco_filter_define(t,p);
		}
		else
		{
			io_stream_add_deco_filter_define(t,p);
		}
	}
}

template<typename T,typename P>
requires (::fast_io::operations::decay::defines::has_io_stream_add_deco_filter_define<T,P>)
inline constexpr void io_stream_add_deco_filter_decay(T t,P p)
{
	if constexpr(!::std::same_as<::std::remove_cvref_t<P>,::fast_io::decorators::no_op_decorators>)
	{
		io_stream_add_deco_filter_define(t,p);
	}
}


template<typename T,typename... Args>
requires (::fast_io::operations::decay::defines::has_output_stream_add_deco_filter_define<T,Args>&&...)
inline constexpr void add_output_decos_decay(T t,Args ...args)
{
	(output_stream_add_deco_filter_decay(t,args),...);
}

template<typename T,typename... Args>
requires (::fast_io::operations::decay::defines::has_input_stream_add_deco_filter_define<T,Args>&&...)
inline constexpr void add_input_decos_decay(T t,Args ...args)
{
	(input_stream_add_deco_filter_decay(t,args),...);
}

template<typename T,typename... Args>
requires (::fast_io::operations::decay::defines::has_io_stream_add_deco_filter_define<T,Args>&&...)
inline constexpr void add_io_decos_decay(T t,Args ...args)
{
	(io_stream_add_deco_filter_decay(t,args),...);
}

}


namespace operations
{

namespace defines
{

template<typename T>
concept has_input_stream_deco_filter_ref_define = requires(T&& t)
{
	input_stream_deco_filter_ref_define(t);
};

template<typename T>
concept has_output_stream_deco_filter_ref_define = requires(T&& t)
{
	output_stream_deco_filter_ref_define(t);
};

template<typename T>
concept has_io_stream_deco_filter_ref_define = requires(T&& t)
{
	io_stream_deco_filter_ref_define(t);
};

template<typename T>
concept has_input_or_io_stream_deco_filter_ref_define = has_input_stream_deco_filter_ref_define<T>||
					has_io_stream_deco_filter_ref_define<T>;

template<typename T>
concept has_output_or_io_stream_deco_filter_ref_define = has_output_stream_deco_filter_ref_define<T>||
					has_io_stream_deco_filter_ref_define<T>;

}

template<typename T>
requires (::fast_io::operations::defines::has_input_or_io_stream_deco_filter_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) input_stream_deco_filter_ref(T &&t)
{
	if constexpr(::fast_io::operations::defines::has_input_or_io_stream_deco_filter_ref_define<T>)
	{
		return input_stream_deco_filter_ref_define(t);
	}
	else
	{
		return io_deco_filter_ref_define(t);
	}
}

template<typename T>
requires (::fast_io::operations::defines::has_output_or_io_stream_deco_filter_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) output_stream_deco_filter_ref(T &&t)
{
	if constexpr(::fast_io::operations::defines::has_output_or_io_stream_deco_filter_ref_define<T>)
	{
		return output_stream_deco_filter_ref_define(t);
	}
	else
	{
		return io_deco_filter_ref_define(t);
	}
}

template<typename T>
requires (::fast_io::operations::defines::has_io_stream_deco_filter_ref_define<T>)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr decltype(auto) io_stream_deco_filter_ref(T &&t)
{
	return io_stream_deco_filter_ref_define(t);
}

template<typename T,typename... Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void add_output_decos(T&& t,Args ...args)
{
	::fast_io::operations::decay::add_output_decos_decay(
		::fast_io::operations::output_stream_deco_filter_ref(t),args...);
}

template<typename T,typename... Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void add_input_decos(T&& t,Args ...args)
{
	::fast_io::operations::decay::add_input_decos_decay(
		::fast_io::operations::input_stream_deco_filter_ref(t),args...);
}

template<typename T,typename... Args>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr void add_io_decos(T&& t,Args ...args)
{
	::fast_io::operations::decay::add_io_decos_decay(
		::fast_io::operations::io_stream_deco_filter_ref(t),args...);
}

}

}
