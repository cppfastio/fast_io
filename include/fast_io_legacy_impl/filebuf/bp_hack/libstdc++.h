#pragma once

namespace fast_io
{
namespace details::streambuf_hack
{
#if 0

/*
https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/include/std/streambuf
*/

/*
*  This is based on _IO_FILE, just reordered to be more consistent,
*  and is intended to be the most minimal abstraction for an
*  internal buffer.
*  -  get == input == read
*  -  put == output == write
*/
char_type* 		_M_in_beg;     ///< Start of get area.
char_type* 		_M_in_cur;     ///< Current read area.
char_type* 		_M_in_end;     ///< End of get area.
char_type* 		_M_out_beg;    ///< Start of put area.
char_type* 		_M_out_cur;    ///< Current put area.
char_type* 		_M_out_end;    ///< End of put area.

/// Current locale setting.
locale 			_M_buf_locale;

#endif

template<typename char_type,typename traits_type>
class libstdcxx_basic_streambuf_hack_model
{
public:
void* vptr;
char_type* 		_M_in_beg;     ///< Start of get area.
char_type* 		_M_in_cur;     ///< Current read area.
char_type* 		_M_in_end;     ///< End of get area.
char_type* 		_M_out_beg;    ///< Start of put area.
char_type* 		_M_out_cur;    ///< Current put area.
char_type* 		_M_out_end;    ///< End of put area.

/// Current locale setting.
::std::locale 			_M_buf_locale;
};

}

template<typename T>
inline typename T::char_type* ibuffer_begin(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::libstdcxx_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,_M_in_beg)};
	return *reinterpret_cast<char_type**>(reinterpret_cast<char*>(biob.fb)+offset);
}

template<typename T>
inline typename T::char_type* ibuffer_curr(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::libstdcxx_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,_M_in_cur)};
	return *reinterpret_cast<char_type**>(reinterpret_cast<char*>(biob.fb)+offset);
}

template<typename T>
inline typename T::char_type* ibuffer_end(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::libstdcxx_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,_M_in_end)};
	return *reinterpret_cast<char_type**>(reinterpret_cast<char*>(biob.fb)+offset);
}

template<typename T>
inline void ibuffer_set_curr(::fast_io::basic_general_streambuf_io_observer<T> biob,typename T::char_type* ptr) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::libstdcxx_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,_M_in_cur)};
	*reinterpret_cast<char_type**>(reinterpret_cast<char*>(biob.fb)+offset)=ptr;
}

template<typename T>
inline typename T::char_type* obuffer_begin(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::libstdcxx_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,_M_out_beg)};
	return *reinterpret_cast<char_type**>(reinterpret_cast<char*>(biob.fb)+offset);
}

template<typename T>
inline typename T::char_type* obuffer_curr(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::libstdcxx_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,_M_out_cur)};
	return *reinterpret_cast<char_type**>(reinterpret_cast<char*>(biob.fb)+offset);
}

template<typename T>
inline typename T::char_type* obuffer_end(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::libstdcxx_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,_M_out_end)};
	return *reinterpret_cast<char_type**>(reinterpret_cast<char*>(biob.fb)+offset);
}

template<typename T>
inline void obuffer_set_curr(::fast_io::basic_general_streambuf_io_observer<T> biob,typename T::char_type* ptr) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::libstdcxx_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,_M_out_cur)};
	*reinterpret_cast<char_type**>(reinterpret_cast<char*>(biob.fb)+offset)=ptr;
}

}

