#pragma once

namespace fast_io
{

namespace operations::decay::defines
{

template<typename T>
concept has_input_stream_require_secure_clear_define = requires(T t)
{
	input_stream_require_secure_clear_define(t);
};

template<typename T>
concept has_output_stream_require_secure_clear_define = requires(T t)
{
	output_stream_require_secure_clear_define(t);
};

template<typename T>
concept has_io_stream_require_secure_clear_define = requires(T t)
{
	io_stream_require_secure_clear_define(t);
};

template<typename T>
concept has_input_or_io_stream_require_secure_clear_define =
	has_input_stream_require_secure_clear_define<T>||
	has_io_stream_require_secure_clear_define<T>;

template<typename T>
concept has_output_or_io_stream_require_secure_clear_define =
	has_output_stream_require_secure_clear_define<T>||
	has_io_stream_require_secure_clear_define<T>;

}

}
