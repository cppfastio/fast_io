#pragma once

namespace fast_io
{

namespace details::streambuf_hack
{
template<typename char_type,typename traits_type>
class basic_streambuf_hack_model : private ::std::basic_streambuf<char_type,traits_type>
{
public:
	using ::std::basic_streambuf<char_type,traits_type>::underflow;
	using ::std::basic_streambuf<char_type,traits_type>::overflow;
};
}

template<typename T>
inline bool ibuffer_underflow(::fast_io::basic_general_streambuf_io_observer<T> biob)
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::basic_streambuf_hack_model<char_type,traits_type>;
	constexpr auto underflow_method_ptr{&model::underflow};
	ibuffer_set_curr(biob,ibuffer_end(biob));
	return (biob.fb->*underflow_method_ptr)()!=EOF;
}

template<typename T>
inline void obuffer_overflow(::fast_io::basic_general_streambuf_io_observer<T> biob,typename T::char_type ch)
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::basic_streambuf_hack_model<char_type,traits_type>;
	constexpr auto overflow_method_ptr{&model::overflow};
	obuffer_set_curr(biob,obuffer_end(biob));
	if(((biob.fb->*overflow_method_ptr)(traits_type::to_int_type(ch)))==EOF)
	{
		throw_posix_error(
#if defined(EIO)
		EIO
#else
		EINVAL
#endif
		);
	}
}

}

