#pragma once

#include"mode.h"
#include"io_buffer.h"
#include"io_buffer_ref.h"
#include"output.h"
#include"input.h"
#include"destroy.h"


namespace fast_io
{

template<typename handletype,typename allocator_type=::fast_io::native_global_allocator>
using basic_ibuf = basic_io_buffer<handletype,
	basic_io_buffer_traits<::fast_io::buffer_mode::in,
		allocator_type,typename handletype::input_char_type,void>>;

template<typename handletype,typename allocator_type=::fast_io::native_global_allocator>
using basic_obuf = basic_io_buffer<handletype,
	basic_io_buffer_traits<::fast_io::buffer_mode::out,
		allocator_type,void,typename handletype::output_char_type>>;

template<typename handletype,typename allocator_type=::fast_io::native_global_allocator>
using basic_iobuf = basic_io_buffer<handletype,
	basic_io_buffer_traits<::fast_io::buffer_mode::in|::fast_io::buffer_mode::out|::fast_io::buffer_mode::tie,
		allocator_type,typename handletype::input_char_type,typename handletype::output_char_type>>;

}
