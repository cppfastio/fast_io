#pragma once

namespace fast_io
{

namespace operations::defines
{

template<typename T>
concept has_input_decorators_ref_define = requires(T&& t)
{
	input_decorators_ref_define(t);
};

template<typename T>
concept has_output_decorators_ref_define = requires(T&& t)
{
	output_decorators_ref_define(t);
};

template<typename T>
concept has_io_decorators_ref_define = requires(T&& t)
{
	io_decorators_ref_define(t);
};

template<typename T>
concept has_input_or_io_decorators_ref_define = has_input_decorators_ref_define<T>||
					has_io_decorators_ref_define<T>;

template<typename T>
concept has_output_or_io_decorators_ref_define = has_output_decorators_ref_define<T>||
					has_io_decorators_ref_define<T>;

}

namespace operations::refs
{

template<typename T>
requires ::fast_io::operations::defines::has_input_or_io_decorators_ref_define<T>
inline constexpr decltype(auto) input_decorators_ref(T&& t)
{
	if constexpr(::fast_io::operations::defines::has_input_decorators_ref_define<T>)
		return input_decorators_ref_define(t);
	else
		return io_decorators_ref_define(t);
}

template<typename T>
requires ::fast_io::operations::defines::has_output_or_io_decorators_ref_define<T>
inline constexpr decltype(auto) output_decorators_ref(T&& t)
{
	if constexpr(::fast_io::operations::defines::has_output_decorators_ref_define<T>)
		return output_decorators_ref_define(t);
	else
		return io_decorators_ref_define(t);
}

template<typename T>
requires ::fast_io::operations::defines::has_io_decorators_ref_define<T>
inline constexpr decltype(auto) io_decorators_ref(T&& t)
{
	return io_decorators_ref_define(t);
}

}

}
