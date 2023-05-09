#pragma once

namespace fast_io::details::streamreflect
{

template<typename T>
concept has_input_stream_seek = requires(T&& instm)
{
	::fast_io::details::input_stream_seek_impl(::fast_io::manipulators::input_stream_ref(instm),0,::fast_io::seekdir::cur);
};

template<typename T>
concept has_input_stream_mutex_ref = requires(T&& instm)
{
	::fast_io::details::input_stream_mutex_ref_impl(::fast_io::manipulators::input_stream_ref(instm));
};

template<typename T>
concept has_ibuffer_ops = requires(T &&instm,typename decltype(::fast_io::manipulators::input_stream_ref(instm))::input_char_type *ptr)
{
	ibuffer_begin(::fast_io::manipulators::input_stream_ref(instm));
	ibuffer_curr(::fast_io::manipulators::input_stream_ref(instm));
	ibuffer_end(::fast_io::manipulators::input_stream_ref(instm));
	ibuffer_set_curr(::fast_io::manipulators::input_stream_ref(instm),ptr);
};

template<typename T>
concept has_read_some_underflow_define = requires(T &&instm,typename decltype(::fast_io::manipulators::input_stream_ref(instm))::input_char_type* ptr)
{
	read_some_underflow_define(::fast_io::manipulators::input_stream_ref(instm),ptr,ptr);
};

template<typename T>
concept has_read_underflow_define = requires(T &&instm,typename decltype(::fast_io::manipulators::input_stream_ref(instm))::input_char_type* ptr)
{
	read_underflow_define(::fast_io::manipulators::input_stream_ref(instm),ptr,ptr);
};

template<typename T>
concept has_read_some_bytes_underflow_define = requires(T &&instm,::std::byte* ptr)
{
	read_some_bytes_underflow_define(::fast_io::manipulators::input_stream_ref(instm),ptr,ptr);
};

template<typename T>
concept has_read_all_bytes_underflow_define = requires(T &&instm,::std::byte* ptr)
{
	read_all_bytes_underflow_define(::fast_io::manipulators::input_stream_ref(instm),ptr,ptr);
};

template<typename T>
concept has_scatter_read_some_bytes_underflow_define = requires(T &&instm,::fast_io::io_scatter_t *scatter,::std::size_t len)
{
	scatter_read_some_bytes_underflow_define(::fast_io::manipulators::input_stream_ref(instm),scatter,len);
};

template<typename T>
concept has_scatter_read_all_bytes_underflow_define = requires(T &&instm,::fast_io::io_scatter_t *scatter,::std::size_t len)
{
	scatter_read_all_bytes_underflow_define(::fast_io::manipulators::input_stream_ref(instm),scatter,len);
};

template<typename T>
concept has_scatter_read_some_underflow_define = requires(T &&instm,::fast_io::basic_io_scatter_t<typename decltype(::fast_io::manipulators::input_stream_ref(instm))::input_char_type> *pscatter,::std::size_t len)
{
	scatter_read_some_underflow_define(::fast_io::manipulators::input_stream_ref(instm),pscatter,len);
};

template<typename T>
concept has_scatter_read_underflow_define = requires(T &&instm,::fast_io::basic_io_scatter_t<typename decltype(::fast_io::manipulators::input_stream_ref(instm))::input_char_type> *pscatter,::std::size_t len)
{
	scatter_read_underflow_define(::fast_io::manipulators::input_stream_ref(instm),pscatter,len);
};

template<typename T>
concept has_ibuffer_underflow_never_define = requires(T&& instm)
{
	ibuffer_underflow_never_define(::fast_io::manipulators::output_stream_ref(instm));
};

template<typename stmtype>
concept has_any_of_byte_read_operations =
	::fast_io::details::streamreflect::has_read_some_bytes_underflow_define<stmtype>||
	::fast_io::details::streamreflect::has_read_all_bytes_underflow_define<stmtype>||
	::fast_io::details::streamreflect::has_scatter_read_some_bytes_underflow_define<stmtype>||
	::fast_io::details::streamreflect::has_scatter_read_all_bytes_underflow_define<stmtype>;

template<typename T>
concept has_zero_copy_in_handle = requires(T&& instm)
{
	zero_copy_in_handle(::fast_io::manipulators::output_stream_ref(instm));
};

template<typename T>
concept inputstreamdef = requires(T&& instm)
{
	{::fast_io::manipulators::input_stream_ref(instm)} noexcept;
};

template<typename T>
concept statusinputstreamdef = requires(T&& instm)
{
	{::fast_io::manipulators::status_input_stream_ref(instm)} noexcept;
};

}
