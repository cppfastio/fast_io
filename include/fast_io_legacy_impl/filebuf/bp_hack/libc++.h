#pragma once

namespace fast_io
{

namespace details::streambuf_hack
{
#if 0
/*
libc++
https://github.com/llvm-mirror/libcxx/blob/master/include/streambuf
*/

locale __loc_;
char_type* __binp_;
char_type* __ninp_;
char_type* __einp_;
char_type* __bout_;
char_type* __nout_;
char_type* __eout_;

#endif
template<typename char_type,typename traits_type>
class libcxx_basic_streambuf_hack_model
{
public:
	void* vptr;
	::std::locale __loc_;
	char_type* __binp_;
	char_type* __ninp_;
	char_type* __einp_;
	char_type* __bout_;
	char_type* __nout_;
	char_type* __eout_;
};

}

template<typename T>
inline typename T::char_type* ibuffer_begin(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::libcxx_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,__binp_)};
	return *reinterpret_cast<char_type**>(reinterpret_cast<char*>(biob.fb)+offset);
}

template<typename T>
inline typename T::char_type* ibuffer_curr(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::libcxx_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,__ninp_)};
	return *reinterpret_cast<char_type**>(reinterpret_cast<char*>(biob.fb)+offset);
}

template<typename T>
inline typename T::char_type* ibuffer_end(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::libcxx_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,__einp_)};
	return *reinterpret_cast<char_type**>(reinterpret_cast<char*>(biob.fb)+offset);
}

template<typename T>
inline void ibuffer_set_curr(::fast_io::basic_general_streambuf_io_observer<T> biob,typename T::char_type* ptr) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::libcxx_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,__ninp_)};
	*reinterpret_cast<char_type**>(reinterpret_cast<char*>(biob.fb)+offset)=ptr;
}

template<typename T>
inline typename T::char_type* obuffer_begin(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::libcxx_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,__bout_)};
	return *reinterpret_cast<char_type**>(reinterpret_cast<char*>(biob.fb)+offset);
}

template<typename T>
inline typename T::char_type* obuffer_curr(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::libcxx_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,__nout_)};
	return *reinterpret_cast<char_type**>(reinterpret_cast<char*>(biob.fb)+offset);
}

template<typename T>
inline typename T::char_type* obuffer_end(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::libcxx_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,__eout_)};
	return *reinterpret_cast<char_type**>(reinterpret_cast<char*>(biob.fb)+offset);
}

template<typename T>
inline void obuffer_set_curr(::fast_io::basic_general_streambuf_io_observer<T> biob,typename T::char_type* ptr) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::libcxx_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,__nout_)};
	*reinterpret_cast<char_type**>(reinterpret_cast<char*>(biob.fb)+offset)=ptr;
}

}
