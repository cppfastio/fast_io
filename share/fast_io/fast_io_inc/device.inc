export namespace fast_io
{

using ::fast_io::dir_file;
using ::fast_io::basic_iobuf_pipe;
using ::fast_io::basic_ibuf_file;
using ::fast_io::basic_obuf_file;
using ::fast_io::basic_iobuf_file;

using ::fast_io::basic_ibuf_file_lockable;
using ::fast_io::basic_obuf_file_lockable;
using ::fast_io::basic_iobuf_file_lockable;

using ::fast_io::iobuf_pipe;
using ::fast_io::ibuf_file;
using ::fast_io::obuf_file;
using ::fast_io::iobuf_file;
using ::fast_io::ibuf_file_lockable;
using ::fast_io::obuf_file_lockable;
using ::fast_io::iobuf_file_lockable;

using ::fast_io::wiobuf_pipe;
using ::fast_io::wibuf_file;
using ::fast_io::wobuf_file;
using ::fast_io::wiobuf_file;
using ::fast_io::wibuf_file_lockable;
using ::fast_io::wobuf_file_lockable;
using ::fast_io::wiobuf_file_lockable;

using ::fast_io::u8iobuf_pipe;
using ::fast_io::u8ibuf_file;
using ::fast_io::u8obuf_file;
using ::fast_io::u8iobuf_file;
using ::fast_io::u8ibuf_file_lockable;
using ::fast_io::u8obuf_file_lockable;
using ::fast_io::u8iobuf_file_lockable;

using ::fast_io::u16iobuf_pipe;
using ::fast_io::u16ibuf_file;
using ::fast_io::u16obuf_file;
using ::fast_io::u16iobuf_file;
using ::fast_io::u16ibuf_file_lockable;
using ::fast_io::u16obuf_file_lockable;
using ::fast_io::u16iobuf_file_lockable;

using ::fast_io::u32iobuf_pipe;
using ::fast_io::u32ibuf_file;
using ::fast_io::u32obuf_file;
using ::fast_io::u32iobuf_file;
using ::fast_io::u32ibuf_file_lockable;
using ::fast_io::u32obuf_file_lockable;
using ::fast_io::u32iobuf_file_lockable;

using ::fast_io::io_strlike_reference_wrapper;

#if defined(_WIN32) || (__has_include(<sys/socket.h>) && __has_include(<netinet/in.h>) && !defined(__wasi__))

using ::fast_io::basic_iobuf_socket_file;
using ::fast_io::iobuf_socket_file;
using ::fast_io::wiobuf_socket_file;
using ::fast_io::u8iobuf_socket_file;
using ::fast_io::u16iobuf_socket_file;
using ::fast_io::u32iobuf_socket_file;

#endif

} // namespace fast_io