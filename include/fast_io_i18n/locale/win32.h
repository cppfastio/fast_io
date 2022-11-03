#pragma once

namespace fast_io
{

namespace details
{

template<std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr char_type* win32_get_locale_name_from_lcid(::std::uint_least32_t lcid,char_type* p) noexcept
{
#include"win32_lcid_table.h"
}

template<std::integral char_type>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr char_type* win32_get_locale_encoding_from_code_page(::std::uint_least32_t acp,char_type* p) noexcept
{
	if constexpr(::std::same_as<char_type,char16_t>)
	{
		switch(acp)
		{
			case 936:case 54936:
			{
				return copy_string_literal(u".GB18030",p);
			}
#if 0
			case 1250:
			{
				return copy_string_literal(u".Windows-1250",p);
			}
			case 1251:
			{
				return copy_string_literal(u".Windows-1251",p);
			}
			case 1252:
			{
				return copy_string_literal(u".Windows-1252",p);
			}
			case 1253:
			{
				return copy_string_literal(u".Windows-1253",p);
			}
			case 1254:
			{
				return copy_string_literal(u".Windows-1254",p);
			}
			case 1255:
			{
				return copy_string_literal(u".Windows-1255",p);
			}
			case 1256:
			{
				return copy_string_literal(u".Windows-1256",p);
			}
			case 1257:
			{
				return copy_string_literal(u".Windows-1257",p);
			}
			case 1258:
			{
				return copy_string_literal(u".Windows-1258",p);
			}
#endif
			default:
			{
				return copy_string_literal(u".UTF-8",p);
			}
		}
	}
	else
	{
		switch(acp)
		{
			case 936:case 54936:
			{
				return copy_string_literal(u8".GB18030",p);
			}
#if 0
			case 1250:
			{
				return copy_string_literal(u8".Windows-1250",p);
			}
			case 1251:
			{
				return copy_string_literal(u8".Windows-1251",p);
			}
			case 1252:
			{
				return copy_string_literal(u8".Windows-1252",p);
			}
			case 1253:
			{
				return copy_string_literal(u8".Windows-1253",p);
			}
			case 1254:
			{
				return copy_string_literal(u8".Windows-1254",p);
			}
			case 1255:
			{
				return copy_string_literal(u8".Windows-1255",p);
			}
			case 1256:
			{
				return copy_string_literal(u8".Windows-1256",p);
			}
			case 1257:
			{
				return copy_string_literal(u8".Windows-1257",p);
			}
			case 1258:
			{
				return copy_string_literal(u8".Windows-1258",p);
			}
#endif
			default:
			{
				return copy_string_literal(u8".UTF-8",p);
			}
		}
	}
}

template<::fast_io::win32_family family>
inline void* win32_family_load_l10n_common_impl(::std::conditional_t<family==::fast_io::win32_family::wide_nt,char16_t,char8_t> const* cstr,std::size_t n,lc_locale& loc)
{
	constexpr std::size_t size_restriction{256u};
	constexpr std::size_t encoding_size_restriction{128u};
	using native_char_type = std::conditional_t<family==::fast_io::win32_family::wide_nt,char16_t,char8_t>;
	constexpr std::size_t msys2_encoding_size_restriction{size_restriction>>2};
	native_char_type msys2_encoding[msys2_encoding_size_restriction];
	if(n>=size_restriction)	//locale should not contain so many characters
	{
		throw_win32_error(0x0000203C);
	}
	else if(n==0)
	{
		constexpr std::uint_least32_t encoding_size_restriction_ul32{msys2_encoding_size_restriction};
		if constexpr(family==::fast_io::win32_family::ansi_9x)
		{
			constexpr std::size_t sz{3};
			constexpr char8_t const* candidates[sz]{u8"L10N",u8"LANG"};
			for(auto i{candidates},ed{i+sz};i!=ed;++i)
			{
				::std::uint_least32_t env_size{::fast_io::win32::GetEnvironmentVariableA(reinterpret_cast<char const*>(*i),reinterpret_cast<char*>(msys2_encoding),msys2_encoding_size_restriction)};
				if(env_size<2u)
				{
					continue;
				}
				else if(env_size>=encoding_size_restriction_ul32)
				{
					throw_win32_error(0x00000057);
				}
				cstr=msys2_encoding;
				n=static_cast<std::size_t>(env_size);
				break;
			}
		}
		else
		{
			constexpr std::size_t sz{3};
			constexpr char16_t const* candidates[sz]{u"L10N",u"LANG"};
			for(auto i{candidates},ed{i+sz};i!=ed;++i)
			{
				::std::uint_least32_t env_size{::fast_io::win32::GetEnvironmentVariableW(*i,msys2_encoding,msys2_encoding_size_restriction)};
				if(env_size<2u)
				{
					continue;
				}
				else if(env_size>=encoding_size_restriction_ul32)
				{
					throw_win32_error(0x00000057);
				}
				cstr=msys2_encoding;
				n=static_cast<std::size_t>(env_size);
				break;
			}
		}
	}
	auto const cstr_end{cstr+n};
	auto found_dot{cstr_end};
	for(auto i{cstr};i!=cstr_end;++i)
	{
		switch(*i)
		{
			case 0:
			case char_literal_v<u8'/',native_char_type>:
			case char_literal_v<u8'\\',native_char_type>:
			{
				throw_win32_error(0x0000203C);
			}
			case char_literal_v<u8'.',native_char_type>:
			{
				if(found_dot!=cstr_end)
				{
					throw_win32_error(0x0000203C);
				}
				found_dot = i;
				break;
			}
		}
	}
	//fast_io_i18n.locale.{localename}.{dll/so}
	constexpr std::size_t total_size{::fast_io::details::intrinsics::add_or_overflow_die_chain(sizeof(u8"fast_io_i18n.locale."),size_restriction,encoding_size_restriction,sizeof(u8".dll"))};
	native_char_type buffer[total_size];
	native_char_type* it{buffer};
	if constexpr(family==::fast_io::win32_family::wide_nt)
	{
		it=::fast_io::details::copy_string_literal(u"fast_io_i18n.locale.",it);
	}
	else
	{
		it=::fast_io::details::copy_string_literal(u8"fast_io_i18n.locale.",it);
	}
	if(n==0)
	{
		constexpr bool use_get_user_default_locale_name
{
#if !defined(_WIN32_WINDOWS) && (!defined(_WIN32_WINNT) || _WIN32_WINNT >= 0x0600)
family==::fast_io::win32_family::wide_nt
#endif
};
		if constexpr(use_get_user_default_locale_name)
		{
			constexpr int locale_name_max_len{85};
			static_assert(locale_name_max_len<size_restriction);
			using char16_may_alias_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
			[[__gnu__::__may_alias__]]
#endif
			= char16_t*;
			int ret{::fast_io::win32::GetUserDefaultLocaleName(reinterpret_cast<char16_may_alias_ptr>(it),locale_name_max_len)};
			if(ret)
			{
				--ret;
				for(auto p{it},pe{it+ret};p!=pe;++p)
				{
					switch(*p)
					{
						case char_literal_v<u8'.',native_char_type>:
						case char_literal_v<u8'/',native_char_type>:
						case char_literal_v<u8'\\',native_char_type>:
						{
							throw_win32_error(0x0000203C);
						}
						case char_literal_v<u8'-',native_char_type>:
						{
							*p=char_literal_v<u8'_',native_char_type>;
							break;
						}
					};
				}
				it+=ret;
			}
			else
			{
				*it=char_literal_v<u8'C',native_char_type>;
				++it;
			}
		}
		else
		{
			it=win32_get_locale_name_from_lcid(::fast_io::win32::GetUserDefaultLCID(),it);
		}
	}
	else
	{
		it=::fast_io::details::non_overlapped_copy_n(cstr,n,it);
	}
	if(found_dot == cstr_end)
	{
		it=win32_get_locale_encoding_from_code_page(::fast_io::win32::GetACP(),it);
	}
	if constexpr(family==::fast_io::win32_family::wide_nt)
	{
		it=::fast_io::details::copy_string_literal(u".dll",it);
	}
	else
	{
		it=::fast_io::details::copy_string_literal(u8".dll",it);
	}
	*it=0;
	auto p{buffer};
	win32_family_dll_file<family> dllfile(::fast_io::mnp::os_c_str(p),::fast_io::dll_mode::none);
	auto func{reinterpret_cast<
void (
#if defined(__CYGWIN__)
#if !__has_cpp_attribute(__gnu__::__fastcall__)&&defined(_MSC_VER)
__fastcall
#elif __has_cpp_attribute(__gnu__::__fastcall__)
__attribute__((__fastcall__))
#endif
#endif
	*)(lc_locale*) noexcept>(dll_load_symbol(dllfile,
#if SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__))
	u8"@export_v0@4"
#else
	u8"export_v0"
#endif
	))};
	func(__builtin_addressof(loc));
	return dllfile.release();
}

template<::fast_io::win32_family family,::fast_io::constructible_to_os_c_str path_type>
inline void* win32_family_load_l10n_impl(path_type const& p,lc_locale& loc)
{
	return ::fast_io::win32_family_api_common<family>(p,[&loc](auto const* cstr_ptr,std::size_t n)
	{
		using native_char_type = std::conditional_t<family==::fast_io::win32_family::wide_nt,char16_t,char8_t>;
		using native_char_type_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= native_char_type const*;
		return win32_family_load_l10n_common_impl<family>(reinterpret_cast<native_char_type_may_alias_const_ptr>(cstr_ptr),n,loc);
	});
}

}

template<::fast_io::win32_family family>
class win32_family_l10n
{
public:
	using native_handle_type = void*;
	lc_locale loc{};
	native_handle_type hmodule{};
	constexpr win32_family_l10n() noexcept=default;

	template<::fast_io::constructible_to_os_c_str path_type>
	explicit win32_family_l10n(path_type const& p)
	{
		this->hmodule=::fast_io::details::win32_family_load_l10n_impl<family>(p,loc);
	}

	explicit constexpr operator bool() const noexcept
	{
		return hmodule!=nullptr;
	}
	void close() noexcept
	{
		if(hmodule)[[likely]]
		{
			fast_io::win32::FreeLibrary(hmodule);
			hmodule=nullptr;
		}
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{this->hmodule};
		this->hmodule=nullptr;
		return temp;
	}
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return this->hmodule;
	}

	win32_family_l10n& operator=(win32_family_l10n const&)=delete;
	win32_family_l10n(win32_family_l10n const&)=delete;
	win32_family_l10n& operator=(win32_family_l10n&& __restrict other) noexcept
	{
		close();
		loc=other.loc;
		hmodule=other.hmodule;
		other.hmodule=nullptr;
		other.loc={};
		return *this;
	}
	~win32_family_l10n()
	{
		close();
	}
};

template<std::integral char_type,::fast_io::win32_family family>
inline constexpr ::fast_io::parameter<basic_lc_all<char_type> const&> status_io_print_forward(io_alias_type_t<char_type>,win32_family_l10n<family> const& loc) noexcept
{
	return status_io_print_forward(io_alias_type<char_type>,loc.loc);
}

template<::fast_io::win32_family family,stream stm>
requires (std::is_lvalue_reference_v<stm>||std::is_trivially_copyable_v<stm>)
inline constexpr auto imbue(win32_family_l10n<family>& loc,stm&& out) noexcept
{
	using char_type = typename std::remove_cvref_t<stm>::char_type;
	return imbue(get_all<char_type>(loc.loc),::std::forward<stm>(out));
}

using win32_l10n_9xa = win32_family_l10n<::fast_io::win32_family::ansi_9x>;
using win32_l10n_ntw = win32_family_l10n<::fast_io::win32_family::wide_nt>;
using win32_l10n = win32_family_l10n<::fast_io::win32_family::native>;

#if !defined(__CYGWIN__) && !defined(__WINE__)
using native_l10n = win32_l10n;
#endif

}
