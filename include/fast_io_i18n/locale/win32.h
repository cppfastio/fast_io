#pragma once

namespace fast_io
{

namespace details
{

template<std::integral char_type>
inline char_type* win32_get_locale_name_from_lcid(::std::uint_least32_t lcid,char_type* p) noexcept
{
	if constexpr(std::same_as<char_type,char16_t>)
	{
		switch(lcid)
		{
		default:
		{
			return copy_string_literal(u"C",p);
		}
		}
	}
	else
	{
		switch(lcid)
		{
		default:
		{
			return copy_string_literal(u8"C",p);
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
		std::size_t env_size{};
		if constexpr(family==::fast_io::win32_family::wide_nt)
		{
			if(::fast_io::win32::_wgetenv_s(__builtin_addressof(env_size),msys2_encoding,msys2_encoding_size_restriction,u"LC_ALL"))
			{
				cstr=msys2_encoding;
				n=env_size;
			}
		}
		else
		{
			if(::fast_io::win32::getenv_s(__builtin_addressof(env_size),msys2_encoding,msys2_encoding_size_restriction,u8"LC_ALL"))
			{
				cstr=msys2_encoding;
				n=env_size;
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
	constexpr std::size_t total_size{::fast_io::details::add_or_overflow_die_chain(sizeof(u8"fast_io_i18n.locale."),size_restriction,encoding_size_restriction,sizeof(u8".dll"))};
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
	if(found_dot-cstr<2)
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
			int ret{::fast_io::win32::GetUserDefaultLocaleName(it,locale_name_max_len)};
			if(ret)
			{
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
	if(found_dot != cstr_end)
	{
		it=::fast_io::details::non_overlapped_copy_n(cstr,n,it);
	}
	else
	{
		switch(::fast_io::win32::GetACP())
		{
			case 936:case 54936:
			{
				if constexpr(family==::fast_io::win32_family::wide_nt)
				{
					it=copy_string_literal(u".GB18030",it);
				}
				else
				{
					it=copy_string_literal(u8".GB18030",it);
				}
			}
			default:
			{
				if constexpr(family==::fast_io::win32_family::wide_nt)
				{
					it=copy_string_literal(u".UTF-8",it);
				}
				else
				{
					it=copy_string_literal(u8".UTF-8",it);
				}
			}
		}
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
	win32_family_dll_file<family> dllfile(::fast_io::mnp::os_c_str(buffer),::fast_io::dll_mode::none);
	auto func{reinterpret_cast<void (*)(lc_locale*) noexcept>(dll_load_symbol(dllfile,u8"export_v0"))};
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
#if __has_cpp_attribute(gnu::may_alias)
		[[gnu::may_alias]]
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
	lc_locale loc{};
	void* dll_handle{};
	constexpr win32_family_l10n() noexcept=default;

	template<::fast_io::constructible_to_os_c_str path_type>
	explicit win32_family_l10n(path_type const& p)
	{
		this->dll_handle=::fast_io::details::win32_family_load_l10n_impl<family>(loc);
	}

	explicit constexpr operator bool() const noexcept
	{
		return dll_handle!=nullptr;
	}
	void close() noexcept
	{
		if(dll_handle)[[likely]]
		{
			fast_io::win32::FreeLibrary(dll_handle);
			dll_handle=nullptr;
		}
	}
	win32_family_l10n& operator=(win32_family_l10n const&)=delete;
	win32_family_l10n(win32_family_l10n const&)=delete;
	win32_family_l10n& operator=(win32_family_l10n&& __restrict other) noexcept
	{
		close();
		loc=other.loc;
		dll_handle=other.dll_handle;
		other.dll_handle=nullptr;
		other.loc={};
		return *this;
	}
	~win32_family_l10n()
	{
		close();
	}
};


template<std::integral char_type,::fast_io::win32_family family>
inline constexpr ::fast_io::parameter<basic_lc_all<char_type> const&> status_io_print_forward(io_alias_type_t<char_type>,win32_family_l10n<fam> const& ln) noexcept
{
	return status_io_print_forward(io_alias_type<char_type>,ln.loc);
}

template<::fast_io::win32_family family,stream stm>
requires (std::is_lvalue_reference_v<stm>||std::is_trivially_copyable_v<stm>)
inline constexpr auto imbue(win32_family_l10n<fam>& loc,stm&& out) noexcept
{
	using char_type = typename std::remove_cvref_t<stm>::char_type;
	return imbue(get_all<char_type>(loc.loc),out);
}

using win32_l10n_9xa = win32_family_l10n<::fast_io::win32::ansi_9x>;
using win32_l10n_ntw = win32_family_l10n<::fast_io::win32::wide_nt>;
using win32_l10n = win32_family_l10n<::fast_io::win32::native>;

}
