#pragma once

#include"gb18030.h"
#include"utf_ebcdic.h"
#include"utf_util_table.h"
#include"utf.h"
#include"general.h"
#include"code_cvt.h"


namespace fast_io
{

template<std::integral char_type,std::integral cross_char_type>
inline constexpr auto status_io_print_forward(io_alias_type_t<char_type>,cross_code_cvt_t<cross_char_type> const& ccvt) noexcept
{
	if constexpr(std::same_as<char_type,cross_char_type>)
	{
		return ccvt.scatter;
	}
	else
	{
		return ::fast_io::mnp::code_cvt_t<encoding_scheme::execution_charset,encoding_scheme::execution_charset,cross_char_type>{ccvt.scatter};
	}
}

namespace manipulators
{

template<::fast_io::details::character char_type>
inline constexpr cross_code_cvt_t<char_type> os_env(int argc,char_type const** argv,std::size_t n) noexcept
{
	if(argc<=0)[[unlikely]]
	{
		if(n==0)
		{
			if constexpr(std::same_as<char_type,char>)
			{
				return {{"",0}};
			}
			else if constexpr(std::same_as<char_type,wchar_t>)
			{
				return {{L"",0}};
			}
			else if constexpr(std::same_as<char_type,char16_t>)
			{
				return {{u"",0}};
			}
			else if constexpr(std::same_as<char_type,char16_t>)
			{
				return {{u"",0}};
			}
			else
			{
				return {{u8"",0}};
			}
		}
		else
		{
			::fast_io::fast_terminate();
		}
	}
	else
	{
		std::size_t argcsz{static_cast<std::size_t>(argc)};
		if(argcsz<=n)[[unlikely]]
		{
			::fast_io::fast_terminate();
		}
	}
	auto cstr{argv[n]};
	return {{cstr,::fast_io::cstr_len(cstr)}};
}

}

}