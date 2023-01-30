#pragma once

#if defined(__clang__)
#pragma GCC system_header
#endif

namespace fast_io::details::streambuf_hack
{

/*
https://github.com/microsoft/STL/blob/master/stl/inc/fstream#L775

    const _Cvt* _Pcvt; // pointer to codecvt facet (may be null)
    _Elem _Mychar; // putback character, when _Ungetc fails
    bool _Wrotesome; // true if homing sequence may be needed
    typename _Traits::state_type _State; // current conversion state
    bool _Closef; // true if C stream must be closed
    FILE* _Myfile; // pointer to C stream
*/

template <class _Elem, class _Traits>
class basic_filebuf_model : public std::basic_streambuf<_Elem, _Traits> 
{
public:
	using _Mysb = std::basic_streambuf<_Elem, _Traits>;
	using _Cvt  = std::codecvt<_Elem, char, typename _Traits::state_type>;
	using int_type = typename _Traits::int_type;
	using pos_type = typename _Traits::pos_type;
	using off_type = typename _Traits::off_type;
	const _Cvt* _Pcvt; // pointer to codecvt facet (may be null)
	_Elem _Mychar; // putback character, when _Ungetc fails
	bool _Wrotesome; // true if homing sequence may be needed
	typename _Traits::state_type _State; // current conversion state
	bool _Closef; // true if C stream must be closed
	FILE* _Myfile; // pointer to C stream

	_Elem* _Set_eback; // saves eback() during one-element putback
	_Elem* _Set_egptr; // saves egptr()
};

template<typename char_type,typename traits_type>
inline FILE* fp_hack_impl(std::basic_filebuf<char_type,traits_type>* fbuf) noexcept
{
	using filebuf_model_type = basic_filebuf_model<char_type,traits_type>;
	FILE* fp{};
	static_assert(sizeof(filebuf_model_type)==sizeof(std::basic_filebuf<char_type,traits_type>),"unmatched std::basic_filebuf model");
	// we can only do this or ubsanitizer will complain. Do not do down_cast
	::fast_io::details::my_memcpy(__builtin_addressof(fp),reinterpret_cast<std::byte*>(fbuf)+offsetof(filebuf_model_type,_Myfile),sizeof(fp));
	return fp;
}

template<typename char_type,typename traits_type>
inline FILE* fp_hack(std::basic_filebuf<char_type,traits_type>* fbuf) noexcept
{
	if(fbuf==nullptr)
		return nullptr;
	return fp_hack_impl(fbuf);
}

template<typename T>
requires (std::same_as<T,std::basic_streambuf<typename T::char_type,typename T::traits_type>>)
inline FILE* fp_hack([[maybe_unused]] T* cio) noexcept
{
#ifdef __cpp_rtti
	if(cio)[[likely]]
	{
	using filebuf_type = std::basic_filebuf<typename T::char_type,typename T::traits_type>;
	auto fptr{dynamic_cast<filebuf_type*>(cio)};
	if(fptr)
		return fp_hack_impl(fptr);
	}
#endif
	return nullptr;
}

template<typename char_type,typename traits_type>
inline void msvc_hack_set_close(std::basic_filebuf<char_type,traits_type>* fbuf) noexcept//not null
{
	using filebuf_model_type = basic_filebuf_model<char_type,traits_type>;
	static_assert(sizeof(filebuf_model_type)==sizeof(std::basic_filebuf<char_type,traits_type>),"unmatched std::basic_filebuf model");
	::fast_io::details::my_memset(reinterpret_cast<std::byte*>(fbuf)+offsetof(filebuf_model_type,_Closef),true,sizeof(bool));
}

}