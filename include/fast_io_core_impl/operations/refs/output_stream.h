#pragma once

namespace fast_io
{

namespace operations::decay::defines
{

template<typename T>
concept has_obuffer_basic_operations = requires(T instm,typename decltype(instm)::output_char_type *ptr)
{
	obuffer_begin(instm);
	obuffer_curr(instm);
	obuffer_end(instm);
	obuffer_set_curr(instm,ptr);
};

template<typename T>
concept has_status_print_define = requires(T optstm)
{
	status_print_define<true>(optstm,0);
};

template<typename T>
concept has_write_some_overflow_define = requires(T instm,typename decltype(instm)::output_char_type const* ptr)
{
	write_some_overflow_define(instm,ptr,ptr);
};

template<typename T>
concept has_write_all_overflow_define = requires(T instm,typename decltype(instm)::output_char_type const* ptr)
{
	write_all_overflow_define(instm,ptr,ptr);
};

template<typename T>
concept has_write_until_eof_overflow_define = requires(T instm,typename decltype(instm)::output_char_type const* ptr)
{
	write_until_eof_overflow_define(instm,ptr,ptr);
};

template<typename T>
concept has_write_some_bytes_overflow_define = requires(T instm,::std::byte* ptr)
{
	write_some_bytes_overflow_define(instm,ptr,ptr);
};

template<typename T>
concept has_write_all_bytes_overflow_define = requires(T instm,::std::byte* ptr)
{
	write_all_bytes_overflow_define(instm,ptr,ptr);
};

template<typename T>
concept has_write_until_eof_bytes_overflow_define = requires(T instm,::std::byte* ptr)
{
	write_until_eof_bytes_overflow_define(instm,ptr,ptr);
};

template<typename T>
concept has_scatter_write_some_bytes_overflow_define = requires(T instm,::fast_io::io_scatter_t const *scatter,::std::size_t len)
{
	scatter_write_some_bytes_overflow_define(instm,scatter,len);
};

template<typename T>
concept has_scatter_write_all_bytes_overflow_define = requires(T instm,::fast_io::io_scatter_t const *scatter,::std::size_t len)
{
	scatter_write_all_bytes_overflow_define(instm,scatter,len);
};

template<typename T>
concept has_scatter_write_until_eof_bytes_overflow_define = requires(T instm,::fast_io::io_scatter_t const *scatter,::std::size_t len)
{
	scatter_write_until_eof_bytes_overflow_define(instm,scatter,len);
};

template<typename T>
concept has_scatter_write_some_overflow_define = requires(T instm,::fast_io::basic_io_scatter_t<typename decltype(instm)::output_char_type> *pscatter,::std::size_t len)
{
	scatter_write_some_overflow_define(instm,pscatter,len);
};

template<typename T>
concept has_scatter_write_all_overflow_define = requires(T instm,::fast_io::basic_io_scatter_t<typename decltype(instm)::output_char_type> *pscatter,::std::size_t len)
{
	scatter_write_all_overflow_define(instm,pscatter,len);
};

template<typename T>
concept has_scatter_write_until_eof_overflow_define = requires(T instm,::fast_io::basic_io_scatter_t<typename decltype(instm)::output_char_type> *pscatter,::std::size_t len)
{
	scatter_write_until_eof_overflow_define(instm,pscatter,len);
};

template<typename T>
concept has_obuffer_overflow_never_define = requires(T instm)
{
	obuffer_overflow_never_define(instm);
};

template<typename T>
concept has_output_stream_char_put_overflow_define = requires(T instm,typename T::output_char_type ch)
{
	output_stream_char_put_overflow_define(instm,ch);
};

template<typename T>
concept has_pwrite_some_bytes_overflow_define = requires(T instm,::std::byte* ptr)
{
	pwrite_some_bytes_overflow_define(instm,ptr,ptr,0);
};

template<typename T>
concept has_pwrite_all_bytes_overflow_define = requires(T instm,::std::byte* ptr)
{
	pwrite_all_bytes_overflow_define(instm,ptr,ptr,0);
};

template<typename T>
concept has_pwrite_until_eof_bytes_overflow_define = requires(T instm,::std::byte* ptr)
{
	pwrite_until_eof_bytes_overflow_define(instm,ptr,ptr,0);
};

template<typename T>
concept has_scatter_pwrite_some_bytes_overflow_define = requires(T instm,::fast_io::io_scatter_t const *scatter,::std::size_t len)
{
	scatter_pwrite_some_bytes_overflow_define(instm,scatter,len,0);
};

template<typename T>
concept has_scatter_pwrite_all_bytes_overflow_define = requires(T instm,::fast_io::io_scatter_t const *scatter,::std::size_t len)
{
	scatter_pwrite_all_bytes_overflow_define(instm,scatter,len,0);
};

template<typename T>
concept has_scatter_pwrite_until_eof_bytes_overflow_define = requires(T instm,::fast_io::io_scatter_t const *scatter,::std::size_t len)
{
	scatter_pwrite_until_eof_bytes_overflow_define(instm,scatter,len,0);
};


template<typename T>
concept has_pwrite_some_overflow_define = requires(T instm,typename decltype(instm)::output_char_type const* ptr)
{
	pwrite_some_overflow_define(instm,ptr,ptr,0);
};

template<typename T>
concept has_pwrite_all_overflow_define = requires(T instm,typename decltype(instm)::output_char_type const* ptr)
{
	pwrite_all_overflow_define(instm,ptr,ptr,0);
};

template<typename T>
concept has_pwrite_until_eof_overflow_define = requires(T instm,typename decltype(instm)::output_char_type const* ptr)
{
	pwrite_until_eof_overflow_define(instm,ptr,ptr,0);
};

template<typename T>
concept has_scatter_pwrite_some_overflow_define = requires(T instm,::fast_io::basic_io_scatter_t<typename decltype(instm)::output_char_type>* scatter,::std::size_t len)
{
	scatter_pwrite_some_overflow_define(instm,scatter,len,0);
};

template<typename T>
concept has_scatter_pwrite_all_overflow_define = requires(T instm,::fast_io::basic_io_scatter_t<typename decltype(instm)::output_char_type>* scatter,::std::size_t len)
{
	scatter_pwrite_all_overflow_define(instm,scatter,len,0);
};

template<typename T>
concept has_scatter_pwrite_until_eof_overflow_define = requires(T instm,::fast_io::basic_io_scatter_t<typename decltype(instm)::output_char_type>* scatter,::std::size_t len)
{
	scatter_pwrite_until_eof_overflow_define(instm,scatter,len,0);
};

template<typename T>
concept has_obuffer_is_line_buffering_define = requires(T outstm)
{
	obuffer_is_line_buffering_define(outstm);
};

template<typename stmtype>
concept has_any_of_write_bytes_operations =
	::fast_io::operations::decay::defines::has_write_some_bytes_overflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_write_all_bytes_overflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_write_until_eof_bytes_overflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_write_some_bytes_overflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_write_all_bytes_overflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_write_until_eof_bytes_overflow_define<stmtype>;

template<typename stmtype>
concept has_any_of_write_operations =
	::fast_io::operations::decay::defines::has_write_some_overflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_write_all_overflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_write_until_eof_overflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_write_some_overflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_write_all_overflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_write_until_eof_overflow_define<stmtype>;

template<typename stmtype>
concept has_any_of_pwrite_bytes_operations =
	::fast_io::operations::decay::defines::has_pwrite_some_bytes_overflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_pwrite_all_bytes_overflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_pwrite_until_eof_bytes_overflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_pwrite_some_bytes_overflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_pwrite_all_bytes_overflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_pwrite_until_eof_bytes_overflow_define<stmtype>;

template<typename stmtype>
concept has_any_of_pwrite_operations =
	::fast_io::operations::decay::defines::has_pwrite_some_overflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_pwrite_all_overflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_pwrite_until_eof_overflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_pwrite_some_overflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_pwrite_all_overflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_pwrite_until_eof_overflow_define<stmtype>;

template<typename stmtype>
concept has_any_of_write_or_seek_pwrite_bytes_operations =
	::fast_io::operations::decay::defines::has_any_of_write_bytes_operations<stmtype>||
	(::fast_io::operations::decay::defines::has_output_stream_seek_bytes_define<stmtype>&&
	::fast_io::operations::decay::defines::has_any_of_pwrite_bytes_operations<stmtype>);

template<typename stmtype>
concept has_any_of_pwrite_or_seek_write_bytes_operations = 
	::fast_io::operations::decay::defines::has_any_of_pwrite_bytes_operations<stmtype>||
	(::fast_io::operations::decay::defines::has_output_stream_seek_bytes_define<stmtype>&&
	::fast_io::operations::decay::defines::has_any_of_write_bytes_operations<stmtype>);

template<typename stmtype>
concept has_any_of_write_or_seek_pwrite_operations =
	::fast_io::operations::decay::defines::has_any_of_write_operations<stmtype>||
	(::fast_io::operations::decay::defines::has_output_stream_seek_define<stmtype>&&
	::fast_io::operations::decay::defines::has_any_of_pwrite_operations<stmtype>);

template<typename stmtype>
concept has_any_of_pwrite_or_seek_write_operations =
	::fast_io::operations::decay::defines::has_any_of_pwrite_operations<stmtype>||
	(::fast_io::operations::decay::defines::has_output_stream_seek_define<stmtype>&&
	::fast_io::operations::decay::defines::has_any_of_write_operations<stmtype>);

template<typename stmtype>
concept writable = has_any_of_write_or_seek_pwrite_operations<stmtype>||
			(sizeof(typename stmtype::output_char_type)==1&&
				has_any_of_write_or_seek_pwrite_bytes_operations<stmtype>);

template<typename stmtype>
concept bytes_writable = has_any_of_write_or_seek_pwrite_bytes_operations<stmtype>||
			(sizeof(typename stmtype::output_char_type)==1&&
				has_any_of_write_or_seek_pwrite_operations<stmtype>);

template<typename stmtype>
concept pwritable = has_any_of_pwrite_or_seek_write_operations<stmtype>||
			(sizeof(typename stmtype::output_char_type)==1&&
				has_any_of_pwrite_or_seek_write_bytes_operations<stmtype>);

template<typename stmtype>
concept bytes_pwritable = has_any_of_pwrite_or_seek_write_bytes_operations<stmtype>||
			(sizeof(typename stmtype::output_char_type)==1&&
				has_any_of_pwrite_or_seek_write_operations<stmtype>);

template<typename T>
concept has_obuffer_minimum_size_operations = requires(T outstm)
{
	{obuffer_minimum_size_define(::fast_io::io_reserve_type<
		typename T::output_char_type,
		T>)}->std::same_as<::std::size_t>;
	obuffer_minimum_size_flush_prepare_define(outstm);
};

}

}
