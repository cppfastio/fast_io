#pragma once

//fast_io_freestanding.h is usable when the underlining system implements dynamic memory allocations and exceptions
#if !defined(__cplusplus)
#error "You are not using a C++ compiler"
#endif

#if !defined(__cpp_concepts)
#error "fast_io requires at least C++20 standard compiler."
#else

#include"fast_io_core.h"

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(push)
#pragma warning( disable : 4514 )
#pragma warning( disable : 4554 )
#pragma warning( disable : 4668 )
#pragma warning( disable : 4820 )
#pragma warning( disable : 4710 )
#endif

#include"fast_io_freestanding_impl/exception.h"
//#include"fast_io_freestanding_impl/posix_error.h"
//compile floating point is slow since it requires algorithms like ryu
#ifndef FAST_IO_DISABLE_FLOATING_POINT
#include"fast_io_unit/floating/impl.h"
#endif
#include"fast_io_freestanding_impl/io_buffer/impl.h"
#include"fast_io_freestanding_impl/auto_indent.h"
#include"fast_io_freestanding_impl/serializations/impl.h"
#include"fast_io_freestanding_impl/space_reserve.h"
#include"fast_io_freestanding_impl/width.h"
#include"fast_io_freestanding_impl/scanners/impl.h"
#include"fast_io_freestanding_impl/cond.h"

#if defined(_GLIBCXX_BITSET)
#include"fast_io_unit/bitset.h"
#endif

namespace fast_io
{

template<input_stream input>
using basic_ibuf = basic_io_buffer<input,buffer_mode::in>;
template<output_stream output>
using basic_obuf = basic_io_buffer<output,buffer_mode::out>;
template<io_stream strm>
using basic_iobuf = basic_io_buffer<strm,buffer_mode::in|buffer_mode::out|buffer_mode::tie>;
#if !defined(FAST_IO_DISABLE_CODECVT)
template<std::integral internal_char_type,
	input_stream input,
	encoding_scheme internal_scheme=encoding_scheme::execution_charset,
	encoding_scheme external_scheme=encoding_scheme::execution_charset>
using basic_ibuf_code_cvt = basic_io_buffer<input,
	buffer_mode::in,
	basic_decorators<internal_char_type,
	basic_code_converter<typename input::char_type,external_scheme,internal_scheme>,
	empty_decorator>>;

template<std::integral internal_char_type,
	output_stream output,
	encoding_scheme internal_scheme=encoding_scheme::execution_charset,
	encoding_scheme external_scheme=encoding_scheme::execution_charset>
using basic_obuf_code_cvt = basic_io_buffer<output,
	buffer_mode::out,
	basic_decorators<internal_char_type,
	empty_decorator,
	basic_code_converter<internal_char_type,internal_scheme,external_scheme>>>;

template<std::integral internal_char_type,
	io_stream input_output,
	encoding_scheme internal_scheme=encoding_scheme::execution_charset,
	encoding_scheme external_scheme=encoding_scheme::execution_charset>
using basic_iobuf_code_cvt = basic_io_buffer<input_output,
	buffer_mode::in|buffer_mode::out|buffer_mode::tie,
	basic_decorators<internal_char_type,
	basic_code_converter<typename input_output::char_type,external_scheme,internal_scheme>,
	basic_code_converter<internal_char_type,internal_scheme,external_scheme>>>;
#endif

}

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)
#endif
#endif
