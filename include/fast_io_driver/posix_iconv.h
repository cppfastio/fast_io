#pragma once
#include"../fast_io_device.h"
#include"posix_iconv_impl/impl.h"

namespace fast_io
{

template<std::integral char_type>
using basic_oiconv_file = basic_io_buffer<onative_file,
buffer_mode::out|buffer_mode::construct_decorator,
basic_decorators<char_type,empty_decorator,iconv_deco_t>>;

template<std::integral char_type>
using basic_iiconv_file = basic_io_buffer<inative_file,
buffer_mode::in|buffer_mode::construct_decorator,
basic_decorators<char_type,iconv_deco_t>>;

template<std::integral char_type>
using basic_iconv_file = basic_io_buffer<native_file,
buffer_mode::in|buffer_mode::out|buffer_mode::tie|buffer_mode::construct_decorator,
basic_decorators<char_type,iconv_deco_t,iconv_deco_t>>;

using iiconv_file = basic_iiconv_file<char>;
using wiiconv_file = basic_iiconv_file<wchar_t>;
using u8iiconv_file = basic_iiconv_file<char8_t>;
using u16iiconv_file = basic_iiconv_file<char16_t>;
using u32iiconv_file = basic_iiconv_file<char16_t>;

using oiconv_file = basic_oiconv_file<char>;
using woiconv_file = basic_oiconv_file<wchar_t>;
using u8oiconv_file = basic_oiconv_file<char8_t>;
using u16oiconv_file = basic_oiconv_file<char16_t>;
using u32oiconv_file = basic_oiconv_file<char16_t>;

using iconv_file = basic_iconv_file<char>;
using wiconv_file = basic_iconv_file<wchar_t>;
using u8iconv_file = basic_iconv_file<char8_t>;
using u16iconv_file = basic_iconv_file<char16_t>;
using u32iconv_file = basic_iconv_file<char16_t>;

}