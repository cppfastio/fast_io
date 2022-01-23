#pragma once

#if defined(__GLIBCXX__) || defined(_LIBCPP_VERSION)
#include"libstdc++_libc++.h"
#elif defined(_MSVC_STL_UPDATE)
#include"msvc_stl.h"
#else
#error "Anyone knows how to hack brutal std::basic_streambuf internal on this platform?"
#endif

namespace fast_io
{
template<typename T>
inline auto ibuffer_begin(basic_general_streambuf_io_observer<T> cio) noexcept
{
	return details::streambuf_hack::hack_buffer_ptr<0>(cio.fb);
}

template<typename T>
inline auto ibuffer_curr(basic_general_streambuf_io_observer<T> cio) noexcept
{
	return details::streambuf_hack::hack_buffer_ptr<1>(cio.fb);
}

template<typename T>
inline auto ibuffer_end(basic_general_streambuf_io_observer<T> cio) noexcept
{
	return details::streambuf_hack::hack_buffer_ptr<2>(cio.fb);
}

template<typename T>
inline void ibuffer_set_curr(basic_general_streambuf_io_observer<T> cio,typename T::char_type* ptr) noexcept
{
	details::streambuf_hack::hack_set_buffer_curr<1>(cio.fb,ptr);
}

template<typename T>
inline bool ibuffer_underflow(basic_general_streambuf_io_observer<T> cio)
{
	ibuffer_set_curr(cio,ibuffer_end(cio));
	using traits_type = typename T::traits_type;
	bool test{cio.fb->sgetc()!=traits_type::eof()};
	if(test&&ibuffer_begin(cio)==ibuffer_end(cio))[[unlikely]]
		throw_posix_error(EIO);
	return test;
}

template<typename T>
inline auto obuffer_begin(basic_general_streambuf_io_observer<T> cio) noexcept
{
	return details::streambuf_hack::hack_buffer_ptr<3>(cio.fb);
}

template<typename T>
inline auto obuffer_curr(basic_general_streambuf_io_observer<T> cio) noexcept
{
	return details::streambuf_hack::hack_buffer_ptr<4>(cio.fb);
}

template<typename T>
inline auto obuffer_end(basic_general_streambuf_io_observer<T> cio) noexcept
{
	return details::streambuf_hack::hack_buffer_ptr<5>(cio.fb);
}

template<typename T>
inline void obuffer_set_curr(basic_general_streambuf_io_observer<T> cio,typename T::char_type* ptr) noexcept
{
	details::streambuf_hack::hack_set_buffer_curr<4>(cio.fb,ptr);
}

template<typename T>
inline void obuffer_overflow(basic_general_streambuf_io_observer<T> cio,typename T::char_type ch)
{
	obuffer_set_curr(cio,obuffer_end(cio));
	using traits_type = typename T::traits_type;
	if(cio.fb->sputc(ch)==traits_type::eof())
		throw_posix_error(EIO);
}

}
