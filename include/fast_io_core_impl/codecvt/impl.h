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

}