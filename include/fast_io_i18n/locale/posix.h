#pragma once

namespace fast_io
{

namespace details
{
#if defined(_GNU_SOURCE) && !defined(__ANDROID__)
extern char const* libc_secure_getenv(char const*) noexcept __asm__("secure_getenv");
#else
extern char const* libc_getenv(char const*) noexcept __asm__("getenv");
#endif

inline char const* my_u8getenv(char8_t const* env) noexcept
{
	return
#if defined(_GNU_SOURCE) && !defined(__ANDROID__)
	libc_secure_getenv(reinterpret_cast<char const*>(env));
#else
	libc_getenv(reinterpret_cast<char const*>(env));
#endif
}

inline void* posix_load_l10n_common_impl(char8_t const* cstr,std::size_t n,lc_locale& loc)
{
	constexpr std::size_t size_restriction{256u};
	constexpr std::size_t encoding_size_restriction{128u};
	using native_char_type = char8_t;
	using native_char_type_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= native_char_type const*;
	constexpr std::size_t msys2_encoding_size_restriction{size_restriction>>2};
	if(n>=size_restriction)	//locale should not contain so many characters
	{
		throw_posix_error(EINVAL);
	}
	else if(n==0)
	{
		constexpr std::size_t sz{3};
		constexpr char8_t const* candidates[sz]{u8"L10N",u8"LANG"};
		char const* lc_all_env{reinterpret_cast<char const*>(u8"C")};
		for(auto i{candidates},iend{candidates+sz};i!=iend;++i)
		{
			char const* ret{my_u8getenv(*i)};
			if(ret!=nullptr&&*ret!=0)
			{
				lc_all_env=ret;
				break;
			}
		}
		cstr=reinterpret_cast<native_char_type_may_alias_const_ptr>(lc_all_env);
		n=
#if defined(__has_builtin)
#if __has_builtin(__builtin_strlen)
		__builtin_strlen(lc_all_env);
#else
		::std::strlen(lc_all_env);
#endif
#else
		::std::strlen(lc_all_env);
#endif
		if(n>=msys2_encoding_size_restriction)
		{
			throw_posix_error(EINVAL);
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
				throw_posix_error(EINVAL);
			}
			case char_literal_v<u8'.',native_char_type>:
			{
				if(found_dot!=cstr_end)
				{
					throw_posix_error(EINVAL);
				}
				found_dot = i;
				break;
			}
		}
	}
	constexpr bool extension_is_dll{
#ifdef __CYGWIN__
true
#endif
	};
	constexpr std::size_t extension_size{extension_is_dll?sizeof(u8".dll"):sizeof(u8".so")};
	//fast_io_i18n.locale.{localename}.{dll/so}
	constexpr std::size_t total_size{::fast_io::details::intrinsics::add_or_overflow_die_chain(sizeof(u8"fast_io_i18n.locale."),size_restriction,encoding_size_restriction,extension_size)};
	native_char_type buffer[total_size];
	native_char_type* it{buffer};
	it=::fast_io::details::copy_string_literal(u8"fast_io_i18n.locale.",it);
	it=::fast_io::details::non_overlapped_copy_n(cstr,n,it);
	if(found_dot==cstr_end)
	{
		it=::fast_io::details::copy_string_literal(u8".UTF-8",it);
	}
	if constexpr(extension_is_dll)
	{
		it=::fast_io::details::copy_string_literal(u8".dll",it);
	}
	else
	{
		it=::fast_io::details::copy_string_literal(u8".so",it);
	}
	*it=0;
	auto p{buffer};
	posix_dll_file dllfile(::fast_io::mnp::os_c_str(p),::fast_io::dll_mode::posix_rtld_global | ::fast_io::dll_mode::posix_rtld_now | ::fast_io::dll_mode::posix_rtld_nodelete);
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
#if defined(__CYGWIN__) && (SIZE_MAX<=UINT_LEAST32_MAX &&(defined(__x86__) || defined(_M_IX86) || defined(__i386__)))
	u8"@export_v0@4"
#else
	u8"export_v0"
#endif
	))};
	func(__builtin_addressof(loc));
	return dllfile.release();
}

template<::fast_io::constructible_to_os_c_str path_type>
inline void* posix_load_l10n_impl(path_type const& p,lc_locale& loc)
{
	return ::fast_io::posix_api_common(p,[&loc](auto const* cstr_ptr,std::size_t n)
	{
		using native_char_type = char8_t;
		using native_char_type_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= native_char_type const*;
		return posix_load_l10n_common_impl(reinterpret_cast<native_char_type_may_alias_const_ptr>(cstr_ptr),n,loc);
	});
}

}

class posix_l10n
{
public:
	using native_handle_type = void*;
	lc_locale loc{};
	native_handle_type rtld_handle{};
	constexpr posix_l10n() noexcept=default;

	template<::fast_io::constructible_to_os_c_str path_type>
	explicit posix_l10n(path_type const& p)
	{
		this->rtld_handle=::fast_io::details::posix_load_l10n_impl(p,loc);
	}

	explicit constexpr operator bool() const noexcept
	{
		return rtld_handle!=nullptr;
	}
	void close() noexcept
	{
		if(rtld_handle)[[likely]]
		{
			noexcept_call(dlclose,this->rtld_handle);
			rtld_handle=nullptr;
		}
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{this->rtld_handle};
		this->rtld_handle=nullptr;
		return temp;
	}
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return this->rtld_handle;
	}
	posix_l10n& operator=(posix_l10n const&)=delete;
	posix_l10n(posix_l10n const&)=delete;
	posix_l10n& operator=(posix_l10n&& __restrict other) noexcept
	{
		close();
		loc=other.loc;
		rtld_handle=other.rtld_handle;
		other.rtld_handle=nullptr;
		other.loc={};
		return *this;
	}
	~posix_l10n()
	{
		close();
	}
};

template<std::integral char_type>
inline constexpr ::fast_io::parameter<basic_lc_all<char_type> const&> status_io_print_forward(io_alias_type_t<char_type>,posix_l10n const& loc) noexcept
{
	return status_io_print_forward(io_alias_type<char_type>,loc.loc);
}

template<stream stm>
requires (std::is_lvalue_reference_v<stm>||std::is_trivially_copyable_v<stm>)
inline constexpr auto imbue(posix_l10n& loc,stm&& out) noexcept
{
	using char_type = typename std::remove_cvref_t<stm>::char_type;
	return imbue(get_all<char_type>(loc.loc),::std::forward<stm>(out));
}

using native_l10n = posix_l10n;

}
