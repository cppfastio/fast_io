#pragma once

namespace fast_io::details::streamreflect
{

template<typename T>
concept has_obuffer_basic_ops = requires(T outstm,typename T::char_type *ptr)
{
	obuffer_begin(outstm);
	obuffer_curr(outstm);
	obuffer_end(outstm);
	obuffer_set_curr(outstm,ptr);
};

template<typename T>
concept has_obuffer_all_ops = has_obuffer_basic_ops<T>&&(requires(T outstm,typename T::char_type const *cptr)
{
	obuffer_write_all_overflow_define(outstm,cptr,cptr);
});

template<typename T>
concept has_obuffer_some_ops = has_obuffer_basic_ops<T>&&(requires(T outstm,typename T::char_type const *cptr)
{
	obuffer_write_some_overflow_define(outstm,cptr,cptr);
});

template<typename T>
concept has_obuffer_ops = has_obuffer_all_ops<T>||has_obuffer_some_ops<T>;

template<typename T>
concept has_write_some_define = requires(T outstm,typename T::char_type const* ptr)
{
	write_some_define(outstm,ptr,ptr);
};

template<typename T>
concept has_write_all_define = requires(T outstm,typename T::char_type const* ptr)
{
	write_all_define(outstm,ptr,ptr);
};

template<typename T>
concept has_write_some_bytes_define = requires(T outstm,::std::byte const* ptr)
{
	write_some_bytes_define(outstm,ptr,ptr);
};

template<typename T>
concept has_write_all_bytes_define = requires(T outstm,::std::byte const* ptr)
{
	write_all_bytes_define(outstm,ptr,ptr);
};

template<typename T>
concept has_scatter_write_some_bytes_define = requires(T outstm,::fast_io::io_scatter_t const *scatter,::std::size_t len)
{
	scatter_write_some_bytes_define(outstm,scatter,len);
};

template<typename T>
concept has_scatter_write_all_bytes_define = requires(T outstm,::fast_io::io_scatter_t const *scatter,::std::size_t len)
{
	scatter_write_all_bytes_define(outstm,scatter,len);
};

template<typename T>
concept has_scatter_write_some_define = requires(T outstm,::fast_io::basic_io_scatter_t<typename T::char_type> const *pscatter,::std::size_t len)
{
	scatter_write_some_define(outstm,pscatter,len);
};

template<typename T>
concept has_scatter_write_all_define = requires(T outstm,::fast_io::basic_io_scatter_t<typename T::char_type> const *pscatter,::std::size_t len)
{
	scatter_write_all_define(outstm,pscatter,len);
};

template<typename T>
concept has_seek_define = requires(T outstm)
{
	seek_define(outstm,0,::fast_io::seekdir::cur);
};

template<typename T>
concept outputstreamdef = 
has_obuffer_ops<T>||
has_write_some_define<T>||
has_write_all_define<T>||
has_write_some_bytes_define<T>||
has_write_all_bytes_define<T>||
has_scatter_write_some_bytes_define<T>||
has_scatter_write_all_bytes_define<T>||
has_scatter_write_some_define<T>||
has_scatter_write_all_define<T>;

template<typename T>
concept outputstreamdefref = requires(T&& t)
{
	requires outputstreamdef<decltype(io_ref(t))>;
};

}
