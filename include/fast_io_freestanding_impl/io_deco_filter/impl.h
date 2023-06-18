#pragma once

#include"destroy.h"
#include"io_deco_filter.h"
#include"io_deco_filter_ref.h"
#include"read.h"
#include"write.h"

namespace fast_io
{

template<typename handletype,typename decorators,typename allocator_type=::fast_io::native_global_allocator>
using basic_iodecfilt = basic_io_deco_filter<handletype,
	basic_io_buffer_traits<::fast_io::buffer_mode::in|::fast_io::buffer_mode::out|::fast_io::buffer_mode::tie,
		allocator_type,
		typename decltype(::fast_io::operations::input_stream_ref(*static_cast<handletype*>(nullptr)))::input_char_type,
		typename decltype(::fast_io::operations::output_stream_ref(*static_cast<handletype*>(nullptr)))::output_char_type>,
		decorators>;


}
