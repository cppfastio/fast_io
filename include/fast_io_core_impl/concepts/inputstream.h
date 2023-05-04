#pragma once

namespace fast_io::details::streamreflect
{

template<typename T>
concept has_ibuffer_basic_ops = requires(T instm,typename T::char_type *ptr)
{
	ibuffer_begin(instm);
	ibuffer_curr(instm);
	ibuffer_end(instm);
	ibuffer_set_curr(instm,ptr);
};

template<typename T>
concept has_ibuffer_all_ops = has_ibuffer_basic_ops<T>&&(requires(T instm,typename T::char_type *cptr)
{
	ibuffer_read_all_underflow_define(instm,cptr,cptr);
});

template<typename T>
concept has_ibuffer_some_ops = has_ibuffer_basic_ops<T>&&(requires(T instm,typename T::char_type *cptr)
{
	ibuffer_read_some_underflow_define(instm,cptr,cptr);
});

template<typename T>
concept has_ibuffer_ops = has_ibuffer_all_ops<T>||has_ibuffer_some_ops<T>;

template<typename T>
concept has_read_some_define = requires(T instm,typename T::char_type* ptr)
{
	read_some_define(instm,ptr,ptr);
};

template<typename T>
concept has_read_all_define = requires(T instm,typename T::char_type* ptr)
{
	read_all_define(instm,ptr,ptr);
};

template<typename T>
concept has_read_some_bytes_define = requires(T instm,::std::byte* ptr)
{
	read_some_bytes_define(instm,ptr,ptr);
};

template<typename T>
concept has_read_all_bytes_define = requires(T instm,::std::byte* ptr)
{
	read_all_bytes_define(instm,ptr,ptr);
};

template<typename T>
concept has_scatter_read_some_bytes_define = requires(T instm,::fast_io::io_scatter_t *scatter,::std::size_t len)
{
	scatter_read_some_bytes_define(instm,scatter,len);
};

template<typename T>
concept has_scatter_read_all_bytes_define = requires(T instm,::fast_io::io_scatter_t *scatter,::std::size_t len)
{
	scatter_read_all_bytes_define(instm,scatter,len);
};


template<typename T>
concept has_scatter_read_some_define = requires(T instm,::fast_io::basic_io_scatter_t<typename T::char_type> *pscatter,::std::size_t len)
{
	scatter_read_some_define(instm,pscatter,len);
};

template<typename T>
concept has_scatter_read_all_define = requires(T instm,::fast_io::basic_io_scatter_t<typename T::char_type> *pscatter,::std::size_t len)
{
	scatter_read_all_define(instm,pscatter,len);
};

template<typename T>
concept inputstreamdef = 
has_ibuffer_ops<T>||
has_read_some_define<T>||
has_read_all_define<T>||
has_read_some_bytes_define<T>||
has_read_all_bytes_define<T>||
has_scatter_read_some_bytes_define<T>||
has_scatter_read_all_bytes_define<T>||
has_scatter_read_some_define<T>||
has_scatter_read_all_define<T>;

template<typename T>
concept inputstreamdefref = requires(T&& t)
{
	requires inputstreamdef<decltype(io_ref(t))>;
};

}
