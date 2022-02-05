#pragma once

#if !defined(_WIN32) || defined(__WINE__) || defined(__CYGWIN__)
#include <dlfcn.h>
#endif

namespace fast_io
{


#if !defined(_WIN32) || defined(__WINE__) || defined(__CYGWIN__)

class posix_dl_error:public std::exception
{
public:
};

inline basic_io_scatter_t<char> print_alias_define(io_alias_t,posix_dl_error const &)
{
	auto const c_str{dlerror()};
	return {c_str,cstr_len(c_str)};
}

[[noreturn]] inline void throw_posix_dl_error()
{
#ifdef __cpp_exceptions
	throw posix_dl_error();
#else
	fast_terminate();
#endif
}

#endif
namespace details
{

inline void load_lc_locale(void* dll_handle,lc_locale& loc)
{
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__)
	auto func(bit_cast<void (*)(lc_locale*) noexcept >(fast_io::win32::GetProcAddress(dll_handle,reinterpret_cast<char const*>(u8"export_v0"))));
#else
	auto func(bit_cast<void (*)(lc_locale*) noexcept >(dlsym(dll_handle,reinterpret_cast<char const*>(u8"export_v0"))));
#endif

	if(func==nullptr)
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__)
		throw_win32_error();
#else
		throw_posix_dl_error();
#endif
	func(__builtin_addressof(loc));
}

template<std::integral T>
requires (std::same_as<T,char>
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__)
||std::same_as<T,wchar_t>
#endif
)
inline void* load_library_general(T const* loc_file_path) noexcept
{
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__)
	if constexpr(std::same_as<T,wchar_t>)
		return fast_io::win32::LoadLibraryW(loc_file_path);
	else
		return fast_io::win32::LoadLibraryA(loc_file_path);
#else
	return dlopen(loc_file_path,RTLD_GLOBAL | RTLD_NOW);
#endif
}

template<typename T>
requires (std::same_as<T,char>
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__)
||std::same_as<T,wchar_t>
#endif
)
inline void* load_dll(T const* loc_file_path,lc_locale& loc)
{
	native_dll_file ptr{load_library_general(loc_file_path)};
	if(!ptr)
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__)
		throw_win32_error();
#else
		throw_posix_dl_error();
#endif
	load_lc_locale(ptr.native_handle(),loc);
	return ptr.release();
}

}

struct system_locale_t
{
explicit constexpr system_locale_t()=default;
};

inline constexpr system_locale_t system_locale{};

struct locale_fullname_t
{
explicit constexpr locale_fullname_t()=default;
};

inline constexpr locale_fullname_t locale_fullname{};

namespace details
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

template<std::integral char_type,std::size_t n>
inline constexpr ::fast_io::freestanding::array<char_type,n> create_l10n_string_literal(char8_t const (&arr)[n]) noexcept
{
	::fast_io::freestanding::array<char_type,n> v;
	for(std::size_t i{};i!=n;++i)
		v[i]=static_cast<char_type>(arr[i]);
	return v;
}

template<std::integral char_type>
requires (std::same_as<char_type,char>||(std::same_as<char_type,wchar_t>&&(sizeof(wchar_t)==sizeof(char16_t))))
inline constexpr auto cal_exec_encoding_dll_array() noexcept
{
	if constexpr(std::same_as<wchar_t,char_type>)
	{
		if constexpr(L'A'!=u8'A')
			return create_l10n_string_literal<char_type>(u8".UTF-EBCDIC.so");
		else if constexpr(compile_time_compare(L"我",u8"我"))
			return create_l10n_string_literal<char_type>(u8".UTF-8.so");
		else
			return create_l10n_string_literal<char_type>(u8".GB18030.so");
	}
	else
	{
		if constexpr('A'!=u8'A')
			return create_l10n_string_literal<char_type>(u8".UTF-EBCDIC.so");
		else if constexpr(compile_time_compare("我",u8"我"))
			return create_l10n_string_literal<char_type>(u8".UTF-8.so");
		else
			return create_l10n_string_literal<char_type>(u8".GB18030.so");
	}
}

template<std::integral char_type>
inline constexpr auto exec_encoding_dll_array{cal_exec_encoding_dll_array<char_type>()};

template<std::integral char_type>
inline constexpr auto exec_dll_array{create_l10n_string_literal<char_type>(u8".so")};

template<std::integral char_type>
requires (std::same_as<char_type,char>||(std::same_as<char_type,wchar_t>&&(sizeof(wchar_t)==sizeof(char16_t))))
inline constexpr auto cal_l10n_path_prefix_dll_array() noexcept
{
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__)
	return create_l10n_string_literal<char_type>(u8"fast_io_i18n_data\\locale\\");
#else
	return create_l10n_string_literal<char_type>(u8"/usr/local/lib/fast_io_i18n_data/locale/");
#endif
}

template<std::integral char_type>
inline constexpr auto l10n_path_prefix_dll_array{cal_l10n_path_prefix_dll_array<char_type>()};

template<bool full=false,std::integral char_type>
inline void sanitize_locale_name(::fast_io::freestanding::basic_string_view<char_type> locale_name)
{
	if(locale_name.empty())
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__)
			throw_win32_error(0x00000057);
#else
			throw_posix_error(EINVAL);
#endif
	if((locale_name.front()==u8'.')||(locale_name.back()==u8'.'))
	{
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__)
			throw_win32_error(0x00000057);
#else
			throw_posix_error(EINVAL);
#endif
	}
	if constexpr(full)
	{
		bool happened{};
		for(auto e : locale_name)
		{
			if(e==u8'.')
			{
				if(happened)
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__)
					throw_win32_error(0x00000057);
#else
					throw_posix_error(EINVAL);
#endif
				happened=true;
			}
			if((e==u8'\\')|(e==u8'/'))
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__)
				throw_win32_error(0x00000057);
#else
				throw_posix_error(EINVAL);
#endif
		}
	}
	else
	{
		for(auto e : locale_name)
		{
			if((e==u8'\\')|(e==u8'/')|(e==u8'.'))
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__)
			throw_win32_error(0x00000057);
#else
			throw_posix_error(EINVAL);
#endif
		}
	}
}

template<std::integral char_type>
inline void* load_l10n_with_real_name_impl(lc_locale& loc,::fast_io::freestanding::basic_string_view<char_type> locale_name)
{
	sanitize_locale_name(locale_name);
	constexpr auto& prefix(l10n_path_prefix_dll_array<char_type>);
	constexpr auto& encoding{exec_encoding_dll_array<char_type>};
	constexpr auto prefix_no_0_size{prefix.size()-1};
	local_operator_new_array_ptr<char_type> arrptr(prefix_no_0_size+locale_name.size()+encoding.size());
	::fast_io::details::my_memcpy(arrptr.get(),prefix.data(),prefix_no_0_size*sizeof(char_type));
	::fast_io::details::my_memcpy(arrptr.get()+prefix_no_0_size,locale_name.data(),locale_name.size()*sizeof(char_type));
	::fast_io::details::my_memcpy(arrptr.get()+prefix_no_0_size+locale_name.size(),encoding.data(),encoding.size()*sizeof(char_type));
	return load_dll(arrptr.get(),loc);
}

template<std::integral char_type>
inline void* load_l10n_with_real_name_impl(lc_locale& loc,::fast_io::freestanding::basic_string_view<char_type> locale_name,::fast_io::freestanding::basic_string_view<char_type> encoding)
{
	sanitize_locale_name(locale_name);
	sanitize_locale_name(encoding);
	constexpr auto& prefix(l10n_path_prefix_dll_array<char_type>);
	constexpr auto prefix_no_0_size{prefix.size()-1};
	constexpr auto& dll_postfix{exec_dll_array<char_type>};
	local_operator_new_array_ptr<char_type> arrptr(prefix_no_0_size+locale_name.size()+1+encoding.size()+dll_postfix.size());
	::fast_io::details::my_memcpy(arrptr.get(),prefix.data(),prefix_no_0_size*sizeof(char_type));
	::fast_io::details::my_memcpy(arrptr.get()+prefix_no_0_size,locale_name.data(),locale_name.size()*sizeof(char_type));
	arrptr[prefix_no_0_size+locale_name.size()]=u8'.';
	if(encoding.size())
		::fast_io::details::my_memcpy(arrptr.get()+prefix_no_0_size+locale_name.size()+1,encoding.data(),encoding.size()*sizeof(char_type));
	::fast_io::details::my_memcpy(arrptr.get()+prefix_no_0_size+locale_name.size()+1+encoding.size(),dll_postfix.data(),dll_postfix.size()*sizeof(char_type));
	return load_dll(arrptr.get(),loc);
}

template<std::integral char_type>
inline void* load_l10n_with_full_name_impl(lc_locale& loc,::fast_io::freestanding::basic_string_view<char_type> locale_fullname)
{
	sanitize_locale_name<true>(locale_fullname);
	constexpr auto& prefix(l10n_path_prefix_dll_array<char_type>);
	constexpr auto prefix_no_0_size{prefix.size()-1};
	constexpr auto& dll_postfix{exec_dll_array<char_type>};
	local_operator_new_array_ptr<char_type> arrptr(prefix_no_0_size+locale_fullname.size()+dll_postfix.size());
	::fast_io::details::my_memcpy(arrptr.get(),prefix.data(),prefix_no_0_size*sizeof(char_type));
	::fast_io::details::my_memcpy(arrptr.get()+prefix_no_0_size,locale_fullname.data(),locale_fullname.size()*sizeof(char_type));
	::fast_io::details::my_memcpy(arrptr.get()+prefix_no_0_size+locale_fullname.size(),dll_postfix.data(),dll_postfix.size()*sizeof(char_type));
	return load_dll(arrptr.get(),loc);
}

#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__)

inline ::fast_io::freestanding::string_view get_win32_lang_env(::fast_io::freestanding::array<char,64>& buffer) noexcept
{
	::fast_io::freestanding::array<wchar_t,64> wbuffer;
	std::size_t size{static_cast<std::size_t>(static_cast<unsigned>(::fast_io::win32::GetUserDefaultLocaleName(wbuffer.data(),static_cast<int>(wbuffer.size()))))};
	if(!size)
		return reinterpret_cast<char const*>(u8"C");
	--size;
	for(std::size_t i{};i!=size;++i)
		if(127<static_cast<std::make_unsigned_t<wchar_t>>(wbuffer[i]))
			return reinterpret_cast<char const*>(u8"C");
		else if(wbuffer[i]==u'-')
			buffer[i]=u8'_';
		else
			buffer[i]=static_cast<char>(wbuffer[i]);
	return ::fast_io::freestanding::string_view(buffer.data(),size);
}



inline constexpr ::fast_io::freestanding::u8string_view get_code_page_encoding(std::uint32_t codepage) noexcept
{
//we only deal with GB2312/GB18030 and UTF-8
	switch(codepage)
	{
	case 936:case 54936:return u8"GB18030";
	default:
		return u8"UTF-8";
	}
}

/*
Since MSYS2 console respects POSIX locales. We should support it too.
*/


inline ::fast_io::freestanding::string_view acp_encoding_name() noexcept
{
	auto strvw{get_code_page_encoding(fast_io::win32::GetACP())};
	return {reinterpret_cast<char const*>(strvw.data()),strvw.size()};
}

inline std::size_t get_lc_all_or_lang(::fast_io::freestanding::array<char,64>& buffer) noexcept
{
	std::size_t size_buffer{};
	if(!fast_io::win32::getenv_s(__builtin_addressof(size_buffer),buffer.data(),buffer.size(),reinterpret_cast<char const*>(u8"LC_ALL")))
		return size_buffer;
	if(!fast_io::win32::getenv_s(__builtin_addressof(size_buffer),buffer.data(),buffer.size(),reinterpret_cast<char const*>(u8"LANG")))
		return size_buffer;
	return 0;
}

struct language_encoding_views
{
	::fast_io::freestanding::string_view language,encoding;
};

inline language_encoding_views win32_get_language_encoding_based_on_lc_all_or_lang(::fast_io::freestanding::array<char,64>& buffer) noexcept
{
	std::size_t size{get_lc_all_or_lang(buffer)};
	if(size)
		--size;
	::fast_io::freestanding::string_view buffer_strvw(buffer.data(),size);
	std::size_t dot_pos{buffer_strvw.find(u8'.')};
	::fast_io::freestanding::string_view language;
	::fast_io::freestanding::string_view encoding;
	if(dot_pos==::fast_io::freestanding::string_view::npos)
		language=buffer_strvw;
	else
	{
#if 0
		language=buffer_strvw.substr(0,dot_pos);
		encoding=buffer_strvw.substr(dot_pos+1);
#else
		language=::fast_io::freestanding::string_view(buffer_strvw.data(),dot_pos);
		encoding=::fast_io::freestanding::string_view(buffer_strvw.data()+(dot_pos+1),buffer_strvw.size()-(dot_pos+1));
#endif
	}
	return {language,encoding};
}

inline ::fast_io::freestanding::string_view win32_only_get_language_not_encoding(::fast_io::freestanding::array<char,64>& buffer) noexcept
{
	auto language{win32_get_language_encoding_based_on_lc_all_or_lang(buffer).language};
	if(language.empty())
		return get_win32_lang_env(buffer);
	return language;
}

inline void* load_l10n_with_extracting_local_name_impl(lc_locale& loc)
{
	::fast_io::freestanding::array<char,64> buffer;
	return load_l10n_with_real_name_impl(loc,win32_only_get_language_not_encoding(buffer));
}
inline void* load_l10n_with_extracting_local_name_impl(lc_locale& loc,::fast_io::freestanding::string_view encoding)
{
	::fast_io::freestanding::array<char,64> buffer;
	return load_l10n_with_real_name_impl(loc,win32_only_get_language_not_encoding(buffer),encoding);
}

inline void* deal_with_local_locale_name_local_encoding(lc_locale& loc)
{
	::fast_io::freestanding::array<char,64> buffer;
	auto [language,encoding]=win32_get_language_encoding_based_on_lc_all_or_lang(buffer);
	::fast_io::freestanding::array<char,64> lang_buffer;
	if(language.empty())
		language=get_win32_lang_env(lang_buffer);
	if(encoding.empty())
		encoding=acp_encoding_name();
	return load_l10n_with_real_name_impl(loc,language,encoding);
}

inline void* deal_with_local_locale_name_encoding(lc_locale& loc,::fast_io::freestanding::string_view encoding)
{
	::fast_io::freestanding::array<char,64> buffer;
	return load_l10n_with_real_name_impl(loc,win32_only_get_language_not_encoding(buffer),encoding);
}

inline void* deal_with_locale_name_local_encoding(lc_locale& loc,::fast_io::freestanding::string_view locale_name)
{
	::fast_io::freestanding::array<char,64> buffer;
	auto encoding{win32_get_language_encoding_based_on_lc_all_or_lang(buffer).encoding};
	if(encoding.empty())
		encoding=acp_encoding_name();
	return load_l10n_with_real_name_impl(loc,locale_name,encoding);
}

#else

#if defined(_GNU_SOURCE)
extern char const* libc_secure_getenv(char const*) noexcept __asm__("secure_getenv");
#endif
inline char const* my_getenv(char8_t const* env) noexcept
{
	return
#if defined(_GNU_SOURCE)
	libc_secure_getenv(reinterpret_cast<char const*>(env));
#else
	std::getenv(reinterpret_cast<char const*>(env));
#endif
}

inline ::fast_io::freestanding::string_view get_lc_all_or_lang() noexcept
{
	auto lc_all_ptr = my_getenv(u8"LC_ALL");
	if(lc_all_ptr)
		return ::fast_io::freestanding::string_view(lc_all_ptr);
	lc_all_ptr = my_getenv(u8"LANG");
	if(lc_all_ptr)
		return ::fast_io::freestanding::string_view(lc_all_ptr);
	return ::fast_io::freestanding::string_view(reinterpret_cast<char const*>(u8"C.UTF-8"),sizeof(u8"C.UTF-8")-1);
}

inline ::fast_io::freestanding::string_view get_posix_lang_env() noexcept
{
	::fast_io::freestanding::string_view lang_env{get_lc_all_or_lang()};
	auto dot{lang_env.find(u8'.')};
	if(dot!=::fast_io::freestanding::string_view::npos)
#if 0
		lang_env=lang_env.substr(0,dot);
#else
		lang_env=::fast_io::freestanding::string_view(lang_env.data(),lang_env.data()+dot);
#endif
	return lang_env;
}

inline void* load_l10n_with_extracting_local_name_impl(lc_locale& loc)
{
	return load_l10n_with_real_name_impl(loc,get_posix_lang_env());
}
inline void* load_l10n_with_extracting_local_name_impl(lc_locale& loc,::fast_io::freestanding::string_view encoding)
{
	return load_l10n_with_real_name_impl(loc,get_posix_lang_env(),encoding);
}

inline ::fast_io::freestanding::string_view get_lc_all_or_lang_non_empty() noexcept
{
	::fast_io::freestanding::string_view lang_env=get_lc_all_or_lang();
	if(lang_env.empty())
		lang_env=reinterpret_cast<char const*>(u8"C");
	return lang_env;
}

inline constexpr ::fast_io::freestanding::u8string_view exec_encoding_u8strvw() noexcept
{
	if constexpr('A'!=u8'A')
		return u8"UTF-EBCDIC";
	else if constexpr(compile_time_compare("我",u8"我"))
		return u8"UTF-8";
	else
		return u8"GB18030";
}

inline ::fast_io::freestanding::string_view exec_charset_encoding_strvw() noexcept
{
	constexpr auto u8sv{exec_encoding_u8strvw()};
	return {reinterpret_cast<char const*>(u8sv.data()),u8sv.size()};
}


inline void* deal_with_local_locale_name_local_encoding(lc_locale& loc)
{
	auto strvw{get_lc_all_or_lang_non_empty()};
	auto pos{strvw.find(u8'.')};
	::fast_io::freestanding::string_view name,coding;
	if(pos==::fast_io::freestanding::string_view::npos)
	{
		name=strvw;
		coding=exec_charset_encoding_strvw();
	}
	else
	{
#if 0
		name=strvw.substr(0,pos);
		coding=strvw.substr(pos+1);
#else
		name=::fast_io::freestanding::string_view(strvw.data(),strvw.data()+pos);
		coding=::fast_io::freestanding::string_view(strvw.data()+pos+1,strvw.data()+strvw.size());
#endif
	}
	return load_l10n_with_real_name_impl(loc,name,coding);
}

inline void* deal_with_local_locale_name_encoding(lc_locale& loc,::fast_io::freestanding::string_view encoding)
{
	auto name{get_lc_all_or_lang_non_empty()};
	auto pos{name.find(u8'.')};
	if(pos!=::fast_io::freestanding::string_view::npos)
#if 0
		name=name.substr(0,pos);
#else
		name=::fast_io::freestanding::string_view(name.data(),name.data()+pos);
#endif
	return load_l10n_with_real_name_impl(loc,name,encoding);
}

inline void* deal_with_locale_name_local_encoding(lc_locale& loc,::fast_io::freestanding::string_view locale_name)
{
	auto coding{get_lc_all_or_lang_non_empty()};
	auto pos{coding.find(u8'.')};
	if(pos==::fast_io::freestanding::string_view::npos)
		coding=exec_charset_encoding_strvw();
	else
#if 0
		coding=coding.substr(pos+1);
#else
		coding=::fast_io::freestanding::string_view(coding.data()+pos+1,coding.data()+coding.size());
#endif
	return load_l10n_with_real_name_impl(loc,locale_name,coding);
}

#endif

inline void* deal_with_local_locale_name_init_encoding(lc_locale& loc,::fast_io::freestanding::string_view encoding)
{
	if(encoding.empty())
		return deal_with_local_locale_name_local_encoding(loc);
	else
		return deal_with_local_locale_name_encoding(loc,encoding);
}

inline void* deal_with_locale_name_init_encoding(lc_locale& loc,::fast_io::freestanding::string_view locale_name,::fast_io::freestanding::string_view encoding)
{
	if(encoding.empty())
		return deal_with_locale_name_local_encoding(loc,locale_name);
	else
		return load_l10n_with_real_name_impl(loc,locale_name,encoding);
}

inline void* deal_with_locale_fullname(lc_locale& loc,::fast_io::freestanding::string_view fullname)
{
	if(fullname.empty())
		return deal_with_local_locale_name_local_encoding(loc);
	else
	{
		auto pos{fullname.find(u8'.')};
		if(pos==0)
			return deal_with_local_locale_name_init_encoding(loc,::fast_io::freestanding::string_view(fullname.data()+1u,fullname.size()-1u));
		else if(pos==::fast_io::freestanding::string_view::npos)
			return load_l10n_with_real_name_impl(loc,fullname);
		else
			return load_l10n_with_full_name_impl(loc,fullname);
	}
}

}

struct l10n_fullname_t
{
explicit constexpr l10n_fullname_t() noexcept = default;
};

inline constexpr l10n_fullname_t l10n_fullname{};

class l10n
{
public:
	lc_locale loc{};
	void* dll_handle{};
	constexpr l10n() noexcept=default;
	explicit l10n(::fast_io::freestanding::string_view locale_name)
	{
		if(locale_name.empty())
			dll_handle=details::load_l10n_with_extracting_local_name_impl(loc);
		else
			dll_handle=details::load_l10n_with_real_name_impl(loc,locale_name);
	}
	explicit l10n(::fast_io::freestanding::string_view locale_name,::fast_io::freestanding::string_view encoding)
	{
		if(locale_name.empty())
			dll_handle=details::deal_with_local_locale_name_init_encoding(loc,encoding);
		else
			dll_handle=details::deal_with_locale_name_init_encoding(loc,locale_name,encoding);
	}
	explicit l10n(l10n_fullname_t,::fast_io::freestanding::string_view fullname)
	{
		dll_handle=details::deal_with_locale_fullname(loc,fullname);
	}
	constexpr l10n(l10n const&)=delete;
	constexpr l10n(l10n&& __restrict other) noexcept : loc(::fast_io::freestanding::move(other.loc)),dll_handle(other.dll_handle)
	{
		dll_handle=nullptr;
	}
	explicit constexpr operator bool() noexcept
	{
		return dll_handle!=nullptr;
	}
	void close() noexcept
	{
		if(dll_handle)[[likely]]
		{
#if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__WINE__)
			fast_io::win32::FreeLibrary(dll_handle);
#else
			noexcept_call(::dlclose,dll_handle);
#endif
			dll_handle=nullptr;
		}
	}
	l10n& operator=(l10n const&)=delete;
	l10n& operator=(l10n&& __restrict other) noexcept
	{
		close();
		loc=::fast_io::freestanding::move(other.loc);
		dll_handle=other.dll_handle;
		other.dll_handle=nullptr;
		return *this;
	}
	~l10n()
	{
		close();
	}
};

template<std::integral char_type>
inline constexpr ::fast_io::parameter<basic_lc_all<char_type> const&> status_io_print_forward(io_alias_type_t<char_type>,l10n const& ln) noexcept
{
	return status_io_print_forward(io_alias_type<char_type>,ln.loc);
}

template<stream stm>
requires (std::is_lvalue_reference_v<stm>||std::is_trivially_copyable_v<stm>)
inline constexpr auto imbue(l10n& loc,stm&& out) noexcept
{
	using char_type = typename std::remove_cvref_t<stm>::char_type;
	return imbue(get_all<char_type>(loc.loc),out);
}

}
