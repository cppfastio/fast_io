#pragma once
#include<type_traits>
namespace fast_io_i18n
{

template<typename char_type1,std::size_t n1,typename char_type2,std::size_t n2>
inline constexpr bool compile_time_compare(char_type1 const (&a)[n1],char_type2 const (&b)[n2]) noexcept
{
	if constexpr((sizeof(char_type1)!=sizeof(char_type2))||(n1!=n2))
		return false;
	for(std::size_t i{};i!=n1;++i)
	{
		if(static_cast<std::make_unsigned_t<char_type1>>(a[i])!=static_cast<std::make_unsigned_t<char_type2>>(b[i]))
			return false;
	}
	return true;
}

extern "C"
#if (defined(_WIN32)||defined(__CYGWIN__)) && !defined(__WINE__)
#if __has_cpp_attribute(__gnu__::__dllexport__)
[[__gnu__::__dllexport__]]
#elif !__has_cpp_attribute(__gnu__::__dllexport__)
__declspec(dllexport)
#endif
#if __has_cpp_attribute(__gnu__::__fastcall__)
[[__gnu__::__fastcall__]]
#endif
#endif
void
#if (defined(_WIN32)||defined(__CYGWIN__)) && !defined(__WINE__)
#if !__has_cpp_attribute(__gnu__::__fastcall__)
__fastcall
#endif
#endif
export_v0(lc_locale* lc_ptr) noexcept
{
	using lc_all_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= lc_all const*;
	lc_all_ptr ptr;
#if defined(FAST_IO_LOCALE_u8ENCODING)
	if constexpr(compile_time_compare(FAST_IO_LOCALE_u8ENCODING,u8"UTF-8"))
#else
	if constexpr(compile_time_compare("我",u8"我"))
#endif
		ptr=reinterpret_cast<lc_all_ptr>(&u8lc_all_global);
	else
		ptr=&lc_all_global;
	using wlc_all_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= wlc_all const*;
	wlc_all_ptr wptr;
	if constexpr(compile_time_compare(u8"我",L"我"))
		wptr=reinterpret_cast<wlc_all_ptr>(&u8lc_all_global);
	else if constexpr(compile_time_compare(u"我",L"我"))
		wptr=reinterpret_cast<wlc_all_ptr>(&u16lc_all_global);
	else if constexpr(compile_time_compare(U"我",L"我"))
		wptr=reinterpret_cast<wlc_all_ptr>(&u32lc_all_global);
	else
		wptr=&wlc_all_global;
	*lc_ptr={ptr,wptr,&u8lc_all_global,&u16lc_all_global,&u32lc_all_global};
}

}
