export namespace fast_io
{

using ::fast_io::basic_general_streambuf_io_observer;
using ::fast_io::basic_streambuf_io_observer;

using ::fast_io::operator==;
using ::fast_io::operator<=>;

namespace manipulators
{
using ::fast_io::manipulators::operator_output;
using ::fast_io::manipulators::woperator_output;
} // namespace manipulators

using ::fast_io::streambuf_io_observer;
using ::fast_io::wstreambuf_io_observer;
using ::fast_io::filebuf_io_observer;
using ::fast_io::wfilebuf_io_observer;
using ::fast_io::filebuf_file;
using ::fast_io::wfilebuf_file;

using ::fast_io::io_stream_seek_define;
using ::fast_io::input_stream_seek_define;
using ::fast_io::output_stream_seek_define;

using ::fast_io::write_some_overflow_define;
using ::fast_io::read_some_underflow_define;
using ::fast_io::io_stream_ref_define;
using ::fast_io::io_stream_buffer_flush_define;

using ::fast_io::ibuffer_begin;
using ::fast_io::ibuffer_curr;
using ::fast_io::ibuffer_end;
using ::fast_io::ibuffer_set_curr;
using ::fast_io::ibuffer_underflow;

using ::fast_io::obuffer_begin;
using ::fast_io::obuffer_curr;
using ::fast_io::obuffer_end;
using ::fast_io::obuffer_set_curr;
using ::fast_io::obuffer_overflow;

using ::fast_io::pread_some_bytes_underflow_define;
using ::fast_io::pwrite_some_bytes_overflow_define;
using ::fast_io::scatter_pread_some_bytes_underflow_define;
using ::fast_io::scatter_pwrite_some_bytes_overflow_define;

using ::fast_io::read_some_bytes_underflow_define;
using ::fast_io::write_some_bytes_overflow_define;

} // namespace fast_io