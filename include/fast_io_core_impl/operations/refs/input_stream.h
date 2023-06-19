#pragma once

namespace fast_io
{

namespace operations::decay::defines
{

template<typename T>
concept has_ibuffer_basic_operations = requires(T instm,typename decltype(instm)::input_char_type *ptr)
{
	ibuffer_begin(instm);
	ibuffer_curr(instm);
	ibuffer_end(instm);
	ibuffer_set_curr(instm,ptr);
};

template<typename T>
concept has_status_scan_define = requires(T optstm)
{
	status_scan_define<true>(optstm,0);
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
	ibuffer_underflow_never_define(::fast_io::operations::output_stream_ref(instm));
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
concept has_scatter_pread_some_underflow_define = requires(T instm,::fast_io::basic_io_scatter_t<typename decltype(instm)::input_char_type>* scatter,::std::size_t len)
{
	scatter_pread_some_underflow_define(instm,scatter,len,0);
};

template<typename T>
concept has_scatter_pread_all_underflow_define = requires(T instm,::fast_io::basic_io_scatter_t<typename decltype(instm)::input_char_type>* scatter,::std::size_t len)
{
	scatter_pread_all_underflow_define(instm,scatter,len,0);
};

template<typename T>
concept has_scatter_pread_until_eof_underflow_define = requires(T instm,::fast_io::basic_io_scatter_t<typename decltype(instm)::input_char_type>* scatter,::std::size_t len)
{
	scatter_pread_until_eof_underflow_define(instm,scatter,len,0);
};

template<typename stmtype>
concept has_any_of_read_bytes_operations =
	::fast_io::operations::decay::defines::has_read_some_bytes_underflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_read_all_bytes_underflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_read_until_eof_bytes_underflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_read_some_bytes_underflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_read_all_bytes_underflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_read_until_eof_bytes_underflow_define<stmtype>;

template<typename stmtype>
concept has_any_of_read_operations =
	::fast_io::operations::decay::defines::has_read_some_underflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_read_all_underflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_read_until_eof_underflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_read_some_underflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_read_all_underflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_read_until_eof_underflow_define<stmtype>;

template<typename stmtype>
concept has_any_of_pread_bytes_operations =
	::fast_io::operations::decay::defines::has_pread_some_bytes_underflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_pread_all_bytes_underflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_pread_until_eof_bytes_underflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_pread_some_bytes_underflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_pread_all_bytes_underflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_pread_until_eof_bytes_underflow_define<stmtype>;

template<typename stmtype>
concept has_any_of_pread_operations =
	::fast_io::operations::decay::defines::has_pread_some_underflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_pread_all_underflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_pread_until_eof_underflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_pread_some_underflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_pread_all_underflow_define<stmtype>||
	::fast_io::operations::decay::defines::has_scatter_pread_until_eof_underflow_define<stmtype>;

template<typename stmtype>
concept has_any_of_read_or_seek_pread_bytes_operations =
	::fast_io::operations::decay::defines::has_any_of_read_bytes_operations<stmtype>||
	(::fast_io::operations::decay::defines::has_input_stream_seek_bytes_define<stmtype>&&
	::fast_io::operations::decay::defines::has_any_of_pread_bytes_operations<stmtype>);

template<typename stmtype>
concept has_any_of_pread_or_seek_read_bytes_operations = 
	::fast_io::operations::decay::defines::has_any_of_pread_bytes_operations<stmtype>||
	(::fast_io::operations::decay::defines::has_input_stream_seek_bytes_define<stmtype>&&
	::fast_io::operations::decay::defines::has_any_of_read_bytes_operations<stmtype>);

template<typename stmtype>
concept has_any_of_read_or_seek_pread_operations =
	::fast_io::operations::decay::defines::has_any_of_read_operations<stmtype>||
	(::fast_io::operations::decay::defines::has_input_stream_seek_define<stmtype>&&
	::fast_io::operations::decay::defines::has_any_of_pread_operations<stmtype>);

template<typename stmtype>
concept has_any_of_pread_or_seek_read_operations =
	::fast_io::operations::decay::defines::has_any_of_pread_operations<stmtype>||
	(::fast_io::operations::decay::defines::has_input_stream_seek_define<stmtype>&&
	::fast_io::operations::decay::defines::has_any_of_read_operations<stmtype>);

template<typename stmtype>
concept readable = has_any_of_read_or_seek_pread_operations<stmtype>||
			(sizeof(typename stmtype::output_char_type)==1&&
				has_any_of_read_or_seek_pread_bytes_operations<stmtype>);

template<typename stmtype>
concept bytes_readable = has_any_of_read_or_seek_pread_bytes_operations<stmtype>||
			(sizeof(typename stmtype::output_char_type)==1&&
				has_any_of_read_or_seek_pread_operations<stmtype>);

template<typename stmtype>
concept preadable = has_any_of_pread_or_seek_read_operations<stmtype>||
			(sizeof(typename stmtype::output_char_type)==1&&
				has_any_of_pread_or_seek_read_bytes_operations<stmtype>);

template<typename stmtype>
concept bytes_preadable = has_any_of_pread_or_seek_read_bytes_operations<stmtype>||
			(sizeof(typename stmtype::output_char_type)==1&&
				has_any_of_pread_or_seek_read_operations<stmtype>);

template<typename T>
concept has_ibuffer_minimum_size_operations = requires(T instm)
{
	{ibuffer_minimum_size_define(::fast_io::io_reserve_type<
		typename T::input_char_type,
		T>)}->std::same_as<::std::size_t>;
	ibuffer_minimum_size_underflow_all_prepare_define(instm);
};

}

}
