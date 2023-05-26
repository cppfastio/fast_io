#pragma once

namespace fast_io::details::streamreflect
{

template<typename T>
concept has_output_stream_seek = requires(T outstm)
{
	::fast_io::details::output_stream_seek_impl(outstm,0,::fast_io::seekdir::cur);
};

template<typename T>
concept has_output_stream_seek_bytes = requires(T iostm)
{
	::fast_io::details::output_stream_seek_bytes_impl(::fast_io::manipulators::output_stream_ref(iostm),0,::fast_io::seekdir::cur);
};

template<typename T>
concept has_output_stream_mutex_ref = requires(T outstm)
{
	::fast_io::details::output_stream_mutex_ref_impl(outstm);
};

template<typename T>
concept has_output_stream_char_put_overflow_define = requires(T outstm,typename T::output_char_type ch)
{
	output_stream_char_put_overflow_define(outstm,ch);
};

template<typename T>
concept has_obuffer_ops = requires(T outstm,typename T::output_char_type *ptr)
{
	obuffer_begin(outstm);
	obuffer_curr(outstm);
	obuffer_end(outstm);
	obuffer_set_curr(outstm,ptr);
};

template<typename T>
concept has_output_stream_buffer_flush = requires(T outstm)
{
	::fast_io::details::output_stream_buffer_flush_impl(outstm);
};

template<typename T>
concept has_obuffer_is_line_buffering_define = requires(T outstm)
{
	{obuffer_is_line_buffering_define(outstm)}->std::convertible_to<bool>;
};

template<typename T>
concept has_obuffer_constant_size = requires(T outstm)
{
	{obuffer_constant_size_define(io_reserve_type<typename T::output_char_type,
		std::remove_cvref_t<decltype(outstm)>>)}->std::same_as<std::size_t>;
	obuffer_constant_flush_prepare_define(outstm);
};

template<typename T>
concept has_obuffer_reserve_define = requires(T outstm,::std::size_t size)
{
	obuffer_reserve_define(outstm,size);
	obuffer_shrink_to_fit_define(outstm);
};

template<typename T>
concept has_write_some_overflow_define = requires(T outstm,typename T::output_char_type const* ptr)
{
	write_some_overflow_define(outstm,ptr,ptr);
};

template<typename T>
concept has_write_all_overflow_define = requires(T outstm,typename T::output_char_type const* ptr)
{
	write_all_overflow_define(outstm,ptr,ptr);
};

template<typename T>
concept has_write_some_bytes_overflow_define = requires(T outstm,::std::byte const* ptr)
{
	write_some_bytes_overflow_define(outstm,ptr,ptr);
};

template<typename T>
concept has_write_all_bytes_overflow_define = requires(T outstm,::std::byte const* ptr)
{
	write_all_bytes_overflow_define(outstm,ptr,ptr);
};

template<typename T>
concept has_scatter_write_some_bytes_overflow_define = requires(T outstm,::fast_io::io_scatter_t const *scatter,::std::size_t len)
{
	scatter_write_some_bytes_overflow_define(outstm,scatter,len);
};

template<typename T>
concept has_scatter_write_all_bytes_overflow_define = requires(T outstm,::fast_io::io_scatter_t const *scatter,::std::size_t len)
{
	scatter_write_all_bytes_overflow_define(outstm,scatter,len);
};

template<typename T>
concept has_scatter_write_some_overflow_define = requires(T outstm,::fast_io::basic_io_scatter_t<typename T::output_char_type> const *pscatter,::std::size_t len)
{
	scatter_write_some_overflow_define(outstm,pscatter,len);
};

template<typename T>
concept has_scatter_write_all_overflow_define = requires(T outstm,::fast_io::basic_io_scatter_t<typename T::output_char_type> const *pscatter,::std::size_t len)
{
	scatter_write_all_overflow_define(outstm,pscatter,len);
};

template<typename T>
concept has_obuffer_overflow_never_define = requires(T outstm)
{
	obuffer_overflow_never_define(outstm);
};

template<typename T>
concept has_pwrite_some_overflow_define = requires(T outstm,typename T::output_char_type const* ptr)
{
	pwrite_some_overflow_define(outstm,ptr,ptr,0);
};

template<typename T>
concept has_pwrite_all_overflow_define = requires(T outstm,typename T::output_char_type const* ptr)
{
	pwrite_all_overflow_define(outstm,ptr,ptr,0);
};

template<typename T>
concept has_pwrite_some_bytes_overflow_define = requires(T outstm,::std::byte const* ptr)
{
	pwrite_some_bytes_overflow_define(outstm,ptr,ptr,0);
};

template<typename T>
concept has_pwrite_all_bytes_overflow_define = requires(T outstm,::std::byte const* ptr)
{
	pwrite_all_bytes_overflow_define(outstm,ptr,ptr,0);
};

template<typename T>
concept has_scatter_pwrite_some_bytes_overflow_define = requires(T outstm,::fast_io::io_scatter_t const *scatter,::std::size_t len)
{
	scatter_pwrite_some_bytes_overflow_define(outstm,scatter,len,0);
};

template<typename T>
concept has_scatter_pwrite_all_bytes_overflow_define = requires(T outstm,::fast_io::io_scatter_t const *scatter,::std::size_t len)
{
	scatter_pwrite_all_bytes_overflow_define(outstm,scatter,len,0);
};

template<typename T>
concept has_scatter_pwrite_some_overflow_define = requires(T outstm,::fast_io::basic_io_scatter_t<typename T::output_char_type> const *pscatter,::std::size_t len)
{
	scatter_pwrite_some_overflow_define(outstm,pscatter,len,0);
};

template<typename T>
concept has_scatter_pwrite_all_overflow_define = requires(T outstm,::fast_io::basic_io_scatter_t<typename T::output_char_type> const *pscatter,::std::size_t len)
{
	scatter_pwrite_all_overflow_define(outstm,pscatter,len,0);
};

template<typename T>
concept has_any_of_byte_write_operations =
::fast_io::details::streamreflect::has_write_some_bytes_overflow_define<T>||
::fast_io::details::streamreflect::has_write_all_bytes_overflow_define<T>||
::fast_io::details::streamreflect::has_scatter_write_some_bytes_overflow_define<T>||
::fast_io::details::streamreflect::has_scatter_write_all_bytes_overflow_define<T>;

template<typename T>
concept has_any_of_byte_pwrite_operations =
::fast_io::details::streamreflect::has_pwrite_some_bytes_overflow_define<T>||
::fast_io::details::streamreflect::has_pwrite_all_bytes_overflow_define<T>||
::fast_io::details::streamreflect::has_scatter_pwrite_some_bytes_overflow_define<T>||
::fast_io::details::streamreflect::has_scatter_pwrite_all_bytes_overflow_define<T>;

template<typename T>
concept has_zero_copy_out_handle = requires(T&& instm)
{
	zero_copy_out_handle(::fast_io::manipulators::output_stream_ref(instm));
};

template<typename T>
concept outputstreamdef = requires(T&& instm)
{
	{::fast_io::manipulators::output_stream_ref(instm)} noexcept;
};

template<typename T>
concept statusoutputstreamdef = requires(T&& instm)
{
	{::fast_io::manipulators::status_output_stream_ref(instm)} noexcept;
};

}
