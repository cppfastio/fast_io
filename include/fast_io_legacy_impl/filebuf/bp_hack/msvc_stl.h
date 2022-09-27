#pragma once

namespace fast_io
{
namespace details::streambuf_hack
{
#if 0
/*
https://github.com/microsoft/STL/blob/master/stl/inc/streambuf
*/
private:
    _Elem* _Gfirst; // beginning of read buffer
    _Elem* _Pfirst; // beginning of write buffer
    _Elem** _IGfirst; // pointer to beginning of read buffer
    _Elem** _IPfirst; // pointer to beginning of write buffer
    _Elem* _Gnext; // current position in read buffer
    _Elem* _Pnext; // current position in write buffer
    _Elem** _IGnext; // pointer to current position in read buffer
    _Elem** _IPnext; // pointer to current position in write buffer

    int _Gcount; // length of read buffer
    int _Pcount; // length of write buffer
    int* _IGcount; // pointer to length of read buffer
    int* _IPcount; // pointer to length of write buffer

protected:
    locale* _Plocale; // pointer to imbued locale object
#endif

template<typename _Elem,typename traits_type>
class msvc_stl_basic_streambuf_hack_model
{
public:
	void* vptr;
	_Elem* _Gfirst; // beginning of read buffer
	_Elem* _Pfirst; // beginning of write buffer
	_Elem** _IGfirst; // pointer to beginning of read buffer
	_Elem** _IPfirst; // pointer to beginning of write buffer
	_Elem* _Gnext; // current position in read buffer
	_Elem* _Pnext; // current position in write buffer
	_Elem** _IGnext; // pointer to current position in read buffer
	_Elem** _IPnext; // pointer to current position in write buffer

	int _Gcount; // length of read buffer
	int _Pcount; // length of write buffer
	int* _IGcount; // pointer to length of read buffer
	int* _IPcount; // pointer to length of write buffer

	::std::locale* _Plocale; // pointer to imbued locale object
};

}

template<typename T>
inline typename T::char_type* ibuffer_begin(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::msvc_stl_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,_IGfirst)};
	return **reinterpret_cast<char_type***>(reinterpret_cast<char*>(biob.fb)+offset);
}

template<typename T>
inline typename T::char_type* ibuffer_curr(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::msvc_stl_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,_IGnext)};
	return **reinterpret_cast<char_type***>(reinterpret_cast<char*>(biob.fb)+offset);
}

template<typename T>
inline typename T::char_type* ibuffer_end(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::msvc_stl_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,_IGnext)};
	constexpr std::size_t count_offset{__builtin_offsetof(model,_IGcount)};
	return **reinterpret_cast<char_type***>(reinterpret_cast<char*>(biob.fb)+offset)+
		**reinterpret_cast<int**>(reinterpret_cast<char*>(biob.fb)+count_offset);
}

template<typename T>
inline void ibuffer_set_curr(::fast_io::basic_general_streambuf_io_observer<T> biob,typename T::char_type* ptr) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::msvc_stl_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,_IGnext)};
	constexpr std::size_t count_offset{__builtin_offsetof(model,_IGcount)};
	char_type** addr_of_currptr{*reinterpret_cast<char_type***>(reinterpret_cast<char*>(biob.fb)+offset)};
	char_type* currptr{*addr_of_currptr};
	*addr_of_currptr=ptr;
	**reinterpret_cast<int**>(reinterpret_cast<char*>(biob.fb)+count_offset)-=static_cast<int>(ptr-currptr);
}

template<typename T>
inline typename T::char_type* obuffer_begin(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::msvc_stl_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,_IPfirst)};
	return **reinterpret_cast<char_type***>(reinterpret_cast<char*>(biob.fb)+offset);
}

template<typename T>
inline typename T::char_type* obuffer_curr(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::msvc_stl_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,_IPnext)};
	return **reinterpret_cast<char_type***>(reinterpret_cast<char*>(biob.fb)+offset);
}

template<typename T>
inline typename T::char_type* obuffer_end(::fast_io::basic_general_streambuf_io_observer<T> biob) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::msvc_stl_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,_IPnext)};
	constexpr std::size_t count_offset{__builtin_offsetof(model,_IPcount)};
	return **reinterpret_cast<char_type***>(reinterpret_cast<char*>(biob.fb)+offset)+
		**reinterpret_cast<int**>(reinterpret_cast<char*>(biob.fb)+count_offset);
}

template<typename T>
inline void obuffer_set_curr(::fast_io::basic_general_streambuf_io_observer<T> biob,typename T::char_type* ptr) noexcept
{
	using char_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::char_type;
	using traits_type = typename ::fast_io::basic_general_streambuf_io_observer<T>::traits_type;
	using model = ::fast_io::details::streambuf_hack::msvc_stl_basic_streambuf_hack_model<char_type,traits_type>;
	constexpr std::size_t offset{__builtin_offsetof(model,_IPnext)};
	constexpr std::size_t count_offset{__builtin_offsetof(model,_IPcount)};
	char_type** addr_of_currptr{*reinterpret_cast<char_type***>(reinterpret_cast<char*>(biob.fb)+offset)};
	char_type* currptr{*addr_of_currptr};
	*addr_of_currptr=ptr;
	**reinterpret_cast<int**>(reinterpret_cast<char*>(biob.fb)+count_offset)-=static_cast<int>(ptr-currptr);
}

}
