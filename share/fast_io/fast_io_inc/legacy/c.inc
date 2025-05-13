export namespace fast_io
{

using ::fast_io::c_family;

using ::fast_io::basic_c_family_io_observer;
using ::fast_io::basic_c_family_file;
using ::fast_io::c_family_file_factory;

using ::fast_io::basic_c_io_observer_unlocked;
using ::fast_io::basic_c_io_observer;
using ::fast_io::basic_c_file_unlocked;
using ::fast_io::basic_c_file;

using ::fast_io::c_io_observer_unlocked;
using ::fast_io::c_io_observer;
using ::fast_io::c_file;
using ::fast_io::c_file_unlocked;
using ::fast_io::wc_io_observer_unlocked;
using ::fast_io::wc_io_observer;
using ::fast_io::wc_file;
using ::fast_io::wc_file_unlocked;
using ::fast_io::u8c_io_observer_unlocked;
using ::fast_io::u8c_io_observer;
using ::fast_io::u8c_file;
using ::fast_io::u8c_file_unlocked;
using ::fast_io::u16c_io_observer_unlocked;
using ::fast_io::u16c_io_observer;
using ::fast_io::u16c_file;
using ::fast_io::u16c_file_unlocked;
using ::fast_io::u32c_io_observer_unlocked;
using ::fast_io::u32c_io_observer;
using ::fast_io::u32c_file;
using ::fast_io::u32c_file_unlocked;

using ::fast_io::c_file_factory;
using ::fast_io::c_file_factory_unlocked;

using ::fast_io::pread_some_bytes_underflow_define;
using ::fast_io::pwrite_some_bytes_overflow_define;
using ::fast_io::scatter_pread_some_bytes_underflow_define;
using ::fast_io::scatter_pwrite_some_bytes_overflow_define;

using ::fast_io::read_some_bytes_underflow_define;
using ::fast_io::write_some_bytes_overflow_define;

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

#if defined(__GLIBC__) || defined(__DARWIN_C_LEVEL) || defined(__MSDOS__)
using ::fast_io::obuffer_is_line_buffering_define;
#endif

using ::fast_io::c_stdin;
using ::fast_io::c_stdout;
using ::fast_io::c_stderr;
using ::fast_io::u8c_stdin;
using ::fast_io::u8c_stdout;
using ::fast_io::u8c_stderr;

} // namespace fast_io
