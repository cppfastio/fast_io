#include<fast_io_concept.h>

namespace fast_ios
{

template<typename T>
concept output_stream = ::fast_io::details::has_output_or_io_stream_ref_define<T>;

template<typename T>
concept input_stream = ::fast_io::details::has_input_or_io_stream_ref_define<T>;

template<typename T>
concept io_stream = output_stream<T>&&input_stream<T>&&::fast_io::details::has_io_stream_ref_define<T>;

template<typename T>
concept buffer_output_stream = output_stream<T>&&::fast_io::details::streamreflect::has_obuffer_basic_ops<T>;

template<typename T>
concept buffer_input_stream = input_stream<T>&&::fast_io::details::streamreflect::has_ibuffer_basic_ops<T>;

template<typename T>
concept buffer_io_stream = io_stream<T>&&buffer_input_stream<T>&&buffer_output_stream<T>;

template<typename T>
concept random_access_output_stream = output_stream<T>&&::fast_io::details::streamreflect::has_output_stream_seek<T>;

template<typename T>
concept random_access_input_stream = input_stream<T>&&::fast_io::details::streamreflect::has_input_stream_seek<T>;

template<typename T>
concept random_access_io_stream = io_stream<T>&&
	random_access_input_stream<T>&&
	random_access_output_stream<T>&&
	::fast_io::details::streamreflect::has_io_stream_seek<T>;

template<typename T>
concept mutex_output_stream = output_stream<T>&&::fast_io::details::streamreflect::has_output_stream_lock<T>;

template<typename T>
concept mutex_input_stream = input_stream<T>&&::fast_io::details::streamreflect::has_input_stream_lock<T>;

template<typename T>
concept mutex_io_stream = io_stream<T>&&
	mutex_input_stream<T>&&
	mutex_output_stream<T>&&
	::fast_io::details::streamreflect::has_io_stream_lock<T>;

}

struct foo
{
	using input_char_type = char;
	using output_char_type = char;
};

inline constexpr foo io_stream_ref_define(foo f) noexcept
{
	return f;
}

static_assert(::fast_ios::output_stream<foo>);
static_assert(::fast_ios::input_stream<foo>);

int main()
{

}
