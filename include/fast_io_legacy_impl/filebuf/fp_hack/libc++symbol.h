#pragma once

namespace fast_io::details
{

template<std::size_t n>
inline constexpr char8_t* compute_libcxx_copy_string_literal(char8_t const(&s)[n],char8_t* result) noexcept
{
	constexpr
		::std::size_t mn1{static_cast<::std::size_t>(n-1u)};
	for(::std::size_t i{};i!=mn1;++i)
	{
		result[i]=s[i];
	}
	return result+mn1;
}

constexpr std::size_t compute_libcxx_symbol_name_chars_len(::std::size_t value) noexcept
{
	std::size_t sz{1};
	for(;10u<=value;value/=10u)
		++sz;
	return sz;
}

#define FAST_IO_LIBCPP_ABI_NAMESPACE_STR_SSTRIFY(s) u8 ## #s 
#define FAST_IO_LIBCPP_ABI_NAMESPACE_STR_STRIFY(s) FAST_IO_LIBCPP_ABI_NAMESPACE_STR_SSTRIFY(s)
#define FAST_IO_LIBCPP_ABI_NAMESPACE_STR FAST_IO_LIBCPP_ABI_NAMESPACE_STR_STRIFY(_LIBCPP_ABI_NAMESPACE)


inline constexpr
	::std::size_t libcxx_symbol_len{sizeof(FAST_IO_LIBCPP_ABI_NAMESPACE_STR)-1u};

inline constexpr
	::std::size_t libcxx_symbol_name_chars_len{compute_libcxx_symbol_name_chars_len(libcxx_symbol_len)};

inline constexpr char8_t* compute_libcxx_itanium_prefix(char8_t* iter) noexcept
{
	auto ed{iter+libcxx_symbol_name_chars_len};
	auto rit{ed};
	for(::std::size_t v{libcxx_symbol_len};iter!=rit;--rit)
	{
		::std::size_t dv{static_cast<::std::size_t>(v/10u)};
		char8_t md{static_cast<char8_t>(v%10u)};
		rit[-1]=static_cast<char8_t>(md+u8'0');
		v=dv;
	}
	return ed;
}

template<std::size_t n>
struct libcxx_msvcabi_symbol_name_holder
{
	char8_t const* ptr{};
	inline constexpr auto data() const noexcept
	{
		return ptr;
	}
	inline static constexpr ::std::size_t size() noexcept
	{
		return n;
	}
};

template<std::size_t n>
inline constexpr libcxx_msvcabi_symbol_name_holder<n-1> compute_libcxx_msvcabi_symbol(char8_t const(&s)[n]) noexcept
{
	return {s};
}

template<bool isoutbuf,::std::integral char_type>
requires (::std::same_as<char_type,char>||
	::std::same_as<char_type,wchar_t>||
	::std::same_as<char_type,char8_t>||
	::std::same_as<char_type,char16_t>||
	::std::same_as<char_type,char32_t>)
inline constexpr auto compute_libcxx_stdinoutbufname() noexcept
{
	constexpr bool ismsvcabi{
#ifdef _LIBCPP_ABI_MICROSOFT
		true
#endif
	};
	if constexpr(ismsvcabi)
	{
		if constexpr(isoutbuf)
		{
			if constexpr(::std::same_as<char_type,char>)
			{
				return ::fast_io::details::compute_libcxx_msvcabi_symbol(u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::__stdoutbuf<char>");
			}
			else if constexpr(::std::same_as<char_type,wchar_t>)
			{
				return ::fast_io::details::compute_libcxx_msvcabi_symbol(u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::__stdoutbuf<wchar_t>");
			}
			else if constexpr(::std::same_as<char_type,char8_t>)
			{
				return ::fast_io::details::compute_libcxx_msvcabi_symbol(u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::__stdoutbuf<char8_t>");
			}
			else if constexpr(::std::same_as<char_type,char16_t>)
			{
				return ::fast_io::details::compute_libcxx_msvcabi_symbol(u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::__stdoutbuf<char16_t>");
			}
			else
			{
				return ::fast_io::details::compute_libcxx_msvcabi_symbol(u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::__stdoutbuf<char32_t>");
			}
		}
		else
		{
			if constexpr(::std::same_as<char_type,char>)
			{
				return ::fast_io::details::compute_libcxx_msvcabi_symbol(u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::__stdinbuf<char>");
			}
			else if constexpr(::std::same_as<char_type,wchar_t>)
			{
				return ::fast_io::details::compute_libcxx_msvcabi_symbol(u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::__stdinbuf<wchar_t>");
			}
			else if constexpr(::std::same_as<char_type,char8_t>)
			{
				return ::fast_io::details::compute_libcxx_msvcabi_symbol(u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::__stdinbuf<char8_t>");
			}
			else if constexpr(::std::same_as<char_type,char16_t>)
			{
				return ::fast_io::details::compute_libcxx_msvcabi_symbol(u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::__stdinbuf<char16_t>");
			}
			else
			{
				return ::fast_io::details::compute_libcxx_msvcabi_symbol(u8"class std::" FAST_IO_LIBCPP_ABI_NAMESPACE_STR u8"::__stdinbuf<char32_t>");
			}
		}
	}
	else
	{
/*
Itanium ABI
*/
		constexpr
			::std::size_t stdinoutsz{(isoutbuf?sizeof(u8"11__stdoutbufI"):
				sizeof(u8"10__stdinbufI"))-1};
		constexpr
			::std::size_t chartypesize{
			(::std::same_as<char_type,char>||::std::same_as<char_type,wchar_t>)?1:2
			};
		constexpr
			::std::size_t buffersize{
		sizeof(u8"NStEE")+stdinoutsz+chartypesize+
		::fast_io::details::libcxx_symbol_len+
		::fast_io::details::libcxx_symbol_name_chars_len-1};
		::fast_io::freestanding::array<char8_t,buffersize> buffer;
		auto it{buffer.data()};
		it=::fast_io::details::compute_libcxx_copy_string_literal(u8"NSt",it);
		it=::fast_io::details::compute_libcxx_itanium_prefix(it);
		it=::fast_io::details::compute_libcxx_copy_string_literal( FAST_IO_LIBCPP_ABI_NAMESPACE_STR ,it);
		if constexpr(isoutbuf)
		{
			it=::fast_io::details::compute_libcxx_copy_string_literal(u8"11__stdoutbufI",it);
		}
		else
		{
			it=::fast_io::details::compute_libcxx_copy_string_literal(u8"10__stdinbufI",it);
		}
		if constexpr(::std::same_as<char_type,char>)
		{
			*it=u8'c';
			++it;
		}
		else if constexpr(::std::same_as<char_type,wchar_t>)
		{
			*it=u8'w';
			++it;
		}
		else
		{
			*it=u8'D';
			++it;
			if constexpr(::std::same_as<char_type,char8_t>)
			{
				*it=u8'u';
			}
			else if constexpr(::std::same_as<char_type,char16_t>)
			{
				*it=u8's';
			}
			else
			{
				*it=u8'i';
			}
			++it;
		}
		::fast_io::details::compute_libcxx_copy_string_literal(u8"EE",it);
		return buffer;
	}
}

template<bool isoutbuf,::std::integral char_type>
inline constexpr auto libcxx_stdinoutbufname{::fast_io::details::compute_libcxx_stdinoutbufname<isoutbuf,char_type>()};


#undef FAST_IO_LIBCPP_ABI_NAMESPACE_STR_SSTRIFY
#undef FAST_IO_LIBCPP_ABI_NAMESPACE_STR_STRIFY
#undef FAST_IO_LIBCPP_ABI_NAMESPACE_STR

}
