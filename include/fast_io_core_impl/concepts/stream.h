#pragma once

namespace fast_io
{

template<typename T>
concept stream = details::input_stream_impl<T>||details::output_stream_impl<T>;

template<typename T>
concept input_stream = stream<T>&&details::input_stream_impl<T>;

template<typename T>
concept output_stream = stream<T>&&details::output_stream_impl<T>;

template<typename T>
concept output_stream_with_writeln = output_stream<T>&&requires(T&& t,typename std::remove_cvref_t<T>::char_type const* p)
{
	writeln(t,p,p);
};

template<typename T>
concept mutex_stream = stream<T>&&details::mutex_stream_impl<T>;

template<typename T>
concept random_access_stream = stream<T>&&details::random_access_stream_impl<T>;

template<typename T>
concept io_stream = input_stream<T>&&output_stream<T>;

template<typename T>
concept flush_output_stream = output_stream<T>&&details::flush_output_stream_impl<T>;

template<typename T>
concept buffer_input_stream = input_stream<T>&&details::buffer_input_stream_impl<T>;

template<typename T>
concept contiguous_input_stream = buffer_input_stream<T>&&details::contiguous_input_stream_impl<T>;

template<typename T>
concept buffer_output_stream = output_stream<T>&&details::buffer_output_stream_impl<T>;

template<typename T>
concept constant_buffer_output_stream = buffer_output_stream<T>&&details::constant_buffer_output_stream_impl<T>;

template<typename T>
concept contiguous_output_stream = buffer_output_stream<T>&&details::contiguous_output_stream_impl<T>;

template<typename T>
concept buffer_io_stream = buffer_input_stream<T>&&buffer_output_stream<T>&&io_stream<T>;

template<typename T>
concept dynamic_output_stream = buffer_output_stream<T>&&details::dynamic_output_stream_impl<T>;

/*
noline_buffer_output_stream ensures obuffer_begin(out)<=obuffer_curr(out)<=obuffer_end(out)
line_buffer_output_stream may end up a situation obuffer_curr(out)>obuffer_end(out), triggering overflow.
That is how glibc implements FILE*.
In fast_io, all fast_io directly supported stream ensures noline_buffer_output_stream
*/
template<typename T>
concept noline_buffer_output_stream = buffer_output_stream<T>&&!requires(T t)
{
	{obuffer_is_line_buffering(t)}->std::convertible_to<bool>;
};

template<typename T>
concept zero_copy_input_stream = input_stream<T>&&details::zero_copy_input_stream_impl<T>;

template<typename T>
concept zero_copy_output_stream = output_stream<T>&&details::zero_copy_output_stream_impl<T>;

template<typename T>
concept zero_copy_io_stream = zero_copy_input_stream<T>&&zero_copy_output_stream<T>;

template<typename T>
concept scatter_input_stream = input_stream<T>&&details::scatter_input_stream_impl<T>;

template<typename T>
concept scatter_output_stream = output_stream<T>&&details::scatter_output_stream_impl<T>;

template<typename T>
concept scatter_constant_output_stream = output_stream<T>&&details::scatter_constant_output_stream_impl<T>;

#if 0

template<typename T>
concept async_stream = details::async_input_stream_impl<T>||details::async_output_stream_impl<T>;

template<typename T>
concept async_input_stream = async_stream<T>&&details::async_input_stream_impl<T>;

template<typename T>
concept async_output_stream = async_stream<T>&&details::async_output_stream_impl<T>;

template<typename T>
concept async_io_stream = async_input_stream<T>&&async_output_stream<T>;

template<typename T>
concept async_scatter_input_stream = async_stream<T>&&details::async_scatter_input_stream_impl<T>;

template<typename T>
concept async_scatter_output_stream = async_stream<T>&&details::async_scatter_output_stream_impl<T>;

template<typename T>
concept async_scatter_io_stream = async_input_stream<T>&&async_scatter_output_stream<T>;

#endif

template<typename T>
concept closable_stream = stream<T>&&requires(T t)
{
	{t.close()}->std::same_as<void>;
};

template<typename T>
concept secure_clear_requirement_stream = stream<T>&&requires(T stm)
{
	require_secure_clear(stm);
};

template<typename T>
concept redirect_stream = stream<T>&&details::redirect_stream_impl<T>;

template<typename T>
concept capacity_available_buffer_input_stream = buffer_input_stream<T>&&requires(T stm)
{
	{ibuffer_cap(stm)}->std::convertible_to<typename std::remove_cvref_t<T>::char_type*>;
};

template<typename T>
concept value_based_stream = requires(T t)
{
	{io_value_handle(t)};
}&&
(std::is_trivially_copyable_v<T> ||
requires(T t)
{
	typename T::native_handle_type;
	t.release();
	t.native_handle();
	requires std::is_trivially_copyable_v<typename T::native_handle_type>;
});

template<typename T>
concept try_get_input_stream=input_stream<T>&&requires(T in)
{
	{try_get(in)}->std::convertible_to<try_get_result<typename T::char_type>>;
};

/*
status streams deal with special stream types like streams which need locale
You can define your own behavior with it
*/

template<typename T>
concept status_output_stream = requires(T out)
{
	print_status_define<false>(out,1,1,1,1,1,1,1,1,1,1);
	print_status_define<true>(out,1,1,1,1,1,1,1,1,1,1);
};

/*
input requires a whole overhual because C++ exception is horrible.
*/

template<typename T>
concept status_input_stream = requires(T in)
{
	{scan_status_define(in)}->std::convertible_to<bool>;
};

}
