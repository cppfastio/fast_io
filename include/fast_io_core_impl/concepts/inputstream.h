#pragma once

namespace fast_io::details::streamreflect
{

template<typename T>
concept has_input_stream_seek = requires(T instm)
{
	::fast_io::details::input_stream_seek_impl(instm,0,::fast_io::seekdir::cur);
};

template<typename T>
concept has_input_stream_seek_bytes = requires(T instm)
{
	::fast_io::details::input_stream_seek_bytes_impl(instm,0,::fast_io::seekdir::cur);
};

template<typename T>
concept has_input_stream_mutex_ref = requires(T instm)
{
	::fast_io::details::input_stream_mutex_ref_impl(instm);
};

template<typename T>
concept has_ibuffer_ops = requires(T instm,typename decltype(instm)::input_char_type *ptr)
{
	ibuffer_begin(instm);
	ibuffer_curr(instm);
	ibuffer_end(instm);
	ibuffer_set_curr(instm,ptr);
};

template<typename T>
concept has_read_some_underflow_define = requires(T instm,typename decltype(instm)::input_char_type* ptr)
{
	read_some_underflow_define(instm,ptr,ptr);
};

template<typename T>
concept has_read_all_underflow_define = requires(T instm,typename decltype(instm)::input_char_type* ptr)
{
	read_all_underflow_define(instm,ptr,ptr);
};

template<typename T>
concept has_read_until_eof_underflow_define = requires(T instm,typename decltype(instm)::input_char_type* ptr)
{
	read_until_eof_underflow_define(instm,ptr,ptr);
};

template<typename T>
concept has_read_some_bytes_underflow_define = requires(T instm,::std::byte* ptr)
{
	read_some_bytes_underflow_define(instm,ptr,ptr);
};

template<typename T>
concept has_read_all_bytes_underflow_define = requires(T instm,::std::byte* ptr)
{
	read_all_bytes_underflow_define(instm,ptr,ptr);
};

template<typename T>
concept has_read_until_eof_bytes_underflow_define = requires(T instm,::std::byte* ptr)
{
	read_until_eof_bytes_underflow_define(instm,ptr,ptr);
};

template<typename T>
concept has_scatter_read_some_bytes_underflow_define = requires(T instm,::fast_io::io_scatter_t *scatter,::std::size_t len)
{
	scatter_read_some_bytes_underflow_define(instm,scatter,len);
};

template<typename T>
concept has_scatter_read_all_bytes_underflow_define = requires(T instm,::fast_io::io_scatter_t *scatter,::std::size_t len)
{
	scatter_read_all_bytes_underflow_define(instm,scatter,len);
};

template<typename T>
concept has_scatter_read_until_eof_bytes_underflow_define = requires(T instm,::fast_io::io_scatter_t *scatter,::std::size_t len)
{
	scatter_read_until_eof_bytes_underflow_define(instm,scatter,len);
};

template<typename T>
concept has_scatter_read_some_underflow_define = requires(T instm,::fast_io::basic_io_scatter_t<typename decltype(instm)::input_char_type> *pscatter,::std::size_t len)
{
	scatter_read_some_underflow_define(instm,pscatter,len);
};

template<typename T>
concept has_scatter_read_all_underflow_define = requires(T instm,::fast_io::basic_io_scatter_t<typename decltype(instm)::input_char_type> *pscatter,::std::size_t len)
{
	scatter_read_all_underflow_define(instm,pscatter,len);
};

template<typename T>
concept has_scatter_read_until_eof_underflow_define = requires(T instm,::fast_io::basic_io_scatter_t<typename decltype(instm)::input_char_type> *pscatter,::std::size_t len)
{
	scatter_read_until_eof_underflow_define(instm,pscatter,len);
};

template<typename T>
concept has_ibuffer_underflow_never_define = requires(T instm)
{
	ibuffer_underflow_never_define(::fast_io::manipulators::output_stream_ref(instm));
};

template<typename stmtype>
concept has_any_of_byte_read_operations =
	::fast_io::details::streamreflect::has_read_some_bytes_underflow_define<stmtype>||
	::fast_io::details::streamreflect::has_read_all_bytes_underflow_define<stmtype>||
	::fast_io::details::streamreflect::has_read_until_eof_bytes_underflow_define<stmtype>||
	::fast_io::details::streamreflect::has_scatter_read_some_bytes_underflow_define<stmtype>||
	::fast_io::details::streamreflect::has_scatter_read_all_bytes_underflow_define<stmtype>||
	::fast_io::details::streamreflect::has_scatter_read_until_eof_bytes_underflow_define<stmtype>;


template<typename stmtype>
concept has_any_of_read_operations =
	::fast_io::details::streamreflect::has_read_some_underflow_define<stmtype>||
	::fast_io::details::streamreflect::has_read_all_underflow_define<stmtype>||
	::fast_io::details::streamreflect::has_read_until_eof_underflow_define<stmtype>||
	::fast_io::details::streamreflect::has_scatter_read_some_underflow_define<stmtype>||
	::fast_io::details::streamreflect::has_scatter_read_all_underflow_define<stmtype>||
	::fast_io::details::streamreflect::has_scatter_read_until_eof_underflow_define<stmtype>;



template<typename T>
concept has_pread_some_underflow_define = requires(T instm,typename decltype(instm)::input_char_type* ptr)
{
	pread_some_underflow_define(instm,ptr,ptr,0);
};

template<typename T>
concept has_pread_all_underflow_define = requires(T instm,typename decltype(instm)::input_char_type* ptr)
{
	pread_all_underflow_define(instm,ptr,ptr,0);
};

template<typename T>
concept has_pread_until_eof_underflow_define = requires(T instm,typename decltype(instm)::input_char_type* ptr)
{
	pread_until_eof_underflow_define(instm,ptr,ptr,0);
};

template<typename T>
concept has_pread_some_bytes_underflow_define = requires(T instm,::std::byte* ptr)
{
	pread_some_bytes_underflow_define(instm,ptr,ptr,0);
};

template<typename T>
concept has_pread_all_bytes_underflow_define = requires(T instm,::std::byte* ptr)
{
	pread_all_bytes_underflow_define(instm,ptr,ptr,0);
};

template<typename T>
concept has_pread_until_eof_bytes_underflow_define = requires(T instm,::std::byte* ptr)
{
	pread_until_eof_bytes_underflow_define(instm,ptr,ptr,0);
};

template<typename T>
concept has_scatter_pread_some_bytes_underflow_define = requires(T instm,::fast_io::io_scatter_t *scatter,::std::size_t len)
{
	scatter_pread_some_bytes_underflow_define(instm,scatter,len,0);
};

template<typename T>
concept has_scatter_pread_all_bytes_underflow_define = requires(T instm,::fast_io::io_scatter_t *scatter,::std::size_t len)
{
	scatter_pread_all_bytes_underflow_define(instm,scatter,len,0);
};

template<typename T>
concept has_scatter_pread_until_eof_bytes_underflow_define = requires(T instm,::fast_io::io_scatter_t *scatter,::std::size_t len)
{
	scatter_pread_until_eof_bytes_underflow_define(instm,scatter,len,0);
};

template<typename T>
concept has_scatter_pread_some_underflow_define = requires(T instm,::fast_io::basic_io_scatter_t<typename decltype(instm)::input_char_type> *pscatter,::std::size_t len)
{
	scatter_pread_some_underflow_define(instm,pscatter,len,0);
};

template<typename T>
concept has_scatter_pread_all_underflow_define = requires(T instm,::fast_io::basic_io_scatter_t<typename decltype(instm)::input_char_type> *pscatter,::std::size_t len)
{
	scatter_pread_all_underflow_define(instm,pscatter,len,0);
};

template<typename T>
concept has_scatter_pread_until_eof_underflow_define = requires(T instm,::fast_io::basic_io_scatter_t<typename decltype(instm)::input_char_type> *pscatter,::std::size_t len)
{
	scatter_pread_until_eof_underflow_define(instm,pscatter,len,0);
};

template<typename stmtype>
concept has_any_of_byte_pread_operations =
	::fast_io::details::streamreflect::has_pread_some_bytes_underflow_define<stmtype>||
	::fast_io::details::streamreflect::has_pread_all_bytes_underflow_define<stmtype>||
	::fast_io::details::streamreflect::has_pread_until_eof_bytes_underflow_define<stmtype>||
	::fast_io::details::streamreflect::has_scatter_pread_some_bytes_underflow_define<stmtype>||
	::fast_io::details::streamreflect::has_scatter_pread_all_bytes_underflow_define<stmtype>||
	::fast_io::details::streamreflect::has_scatter_pread_until_eof_bytes_underflow_define<stmtype>;

template<typename stmtype>
concept has_any_of_pread_operations =
	::fast_io::details::streamreflect::has_pread_some_underflow_define<stmtype>||
	::fast_io::details::streamreflect::has_pread_all_underflow_define<stmtype>||
	::fast_io::details::streamreflect::has_pread_until_eof_underflow_define<stmtype>||
	::fast_io::details::streamreflect::has_scatter_pread_some_underflow_define<stmtype>||
	::fast_io::details::streamreflect::has_scatter_pread_all_underflow_define<stmtype>||
	::fast_io::details::streamreflect::has_scatter_pread_until_eof_underflow_define<stmtype>;


template<typename T>
concept has_zero_copy_in_handle = requires(T instm)
{
	zero_copy_in_handle(::fast_io::manipulators::output_stream_ref(instm));
};

template<typename T>
concept inputstreamdef = requires(T instm)
{
	{instm} noexcept;
};

template<typename T>
concept statusinputstreamdef = requires(T instm)
{
	{::fast_io::manipulators::status_input_stream_ref(instm)} noexcept;
};

}
