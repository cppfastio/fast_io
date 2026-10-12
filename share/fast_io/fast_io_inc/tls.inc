export namespace fast_io
{
namespace manipulators
{
namespace scan_skippers
{
using ::fast_io::manipulators::scan_skippers::line_skipper_tag;
using ::fast_io::manipulators::scan_skippers::line_skipper;
using ::fast_io::manipulators::scan_skippers::lf;
using ::fast_io::manipulators::scan_skippers::crlf;
using ::fast_io::manipulators::scan_skippers::cr;
using ::fast_io::manipulators::scan_skippers::line_skipper_platform;
} // namespace scan_skippers
} // namespace manipulators
namespace tls
{
/* customization points -- must stay reachable from consumer modules
   since they are found by ADL on tls observer/composite types */
using ::fast_io::tls::io_stream_ref_define;
using ::fast_io::tls::io_bytes_stream_ref_define;
using ::fast_io::tls::read_some_bytes_underflow_define;
using ::fast_io::tls::write_some_bytes_overflow_define;
using ::fast_io::tls::handshake_define;
using ::fast_io::tls::input_transmit_handle_define;
using ::fast_io::tls::output_transmit_handle_define;
using ::fast_io::tls::tls_close_notify;

/* protocol surface */
using ::fast_io::tls::cipher_suite;
using ::fast_io::tls::extension_type;
using ::fast_io::tls::signature_scheme;
using ::fast_io::tls::content_type;
using ::fast_io::tls::handshake_type;
using ::fast_io::tls::named_group;
using ::fast_io::tls::alert_description;
using ::fast_io::tls::tls_client_config;
using ::fast_io::tls::tls_server_config;
using ::fast_io::tls::tls_platform;

/* crypto backend selection -- basic_tls<..., crypto> takes one of
   these; tls_default_crypto tracks the platform default */
using ::fast_io::tls::tls_default_crypto;
using ::fast_io::tls::fast_io_crypto_backend;
#if FAST_IO_TLS_HAS_OSSL_CRYPTO
using ::fast_io::tls::ossl_crypto_backend;
#endif
#if FAST_IO_TLS_HAS_GNUTLS_CRYPTO
using ::fast_io::tls::gnutls_crypto_backend;
#endif

/* client + stream types */
using ::fast_io::tls::basic_tls_client;
using ::fast_io::tls::tls_client;
using ::fast_io::tls::basic_tls_io_observer;
using ::fast_io::tls::basic_tls;
using ::fast_io::tls::basic_tls_socket;
using ::fast_io::tls::tls_socket;
using ::fast_io::tls::wtls_socket;
using ::fast_io::tls::u8tls_socket;
using ::fast_io::tls::u16tls_socket;
using ::fast_io::tls::u32tls_socket;
using ::fast_io::tls::basic_tls_socket_file;
using ::fast_io::tls::tls_socket_file;
using ::fast_io::tls::wtls_socket_file;
using ::fast_io::tls::u8tls_socket_file;
using ::fast_io::tls::u16tls_socket_file;
using ::fast_io::tls::u32tls_socket_file;
using ::fast_io::tls::basic_iobuf_tls_socket_file;
using ::fast_io::tls::iobuf_tls_socket_file;
using ::fast_io::tls::wiobuf_tls_socket_file;
using ::fast_io::tls::u8iobuf_tls_socket_file;
using ::fast_io::tls::u16iobuf_tls_socket_file;
using ::fast_io::tls::u32iobuf_tls_socket_file;
using ::fast_io::tls::basic_root_store;
using ::fast_io::tls::root_store;
using ::fast_io::tls::basic_peer_certificates;
using ::fast_io::tls::peer_certificates;
using ::fast_io::tls::basic_native_tls;
using ::fast_io::tls::native_tls_socket_file;
using ::fast_io::tls::u8native_tls_socket_file;
using ::fast_io::tls::basic_iobuf_native_tls_socket_file;
using ::fast_io::tls::iobuf_native_tls_socket_file;
using ::fast_io::tls::u8iobuf_native_tls_socket_file;

/* async customization points -- the callback-level defines found by
   ADL on the tls observer types; the decay/awaiter machinery above
   them lives in the operations namespaces */
#if defined(__HERBCEPTIONS__)
using ::fast_io::tls::async_close_define;
#if defined(__linux__)
using ::fast_io::tls::async_pread_some_bytes_underflow_callback_define;
using ::fast_io::tls::async_scatter_pread_some_bytes_underflow_callback_define;
using ::fast_io::tls::async_pwrite_some_bytes_overflow_callback_define;
using ::fast_io::tls::async_scatter_pwrite_some_bytes_overflow_callback_define;
#endif
#endif

#if (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
using ::fast_io::tls::details::schannel_out_token;
using ::fast_io::tls::basic_schannel_tls_client;
using ::fast_io::tls::basic_schannel_tls_io_observer;
using ::fast_io::tls::basic_schannel_tls;
using ::fast_io::tls::basic_schannel_tls_socket_file;
using ::fast_io::tls::schannel_tls_socket_file;
using ::fast_io::tls::u8schannel_tls_socket_file;
using ::fast_io::tls::basic_iobuf_schannel_tls_socket_file;
using ::fast_io::tls::iobuf_schannel_tls_socket_file;
using ::fast_io::tls::u8iobuf_schannel_tls_socket_file;
#if defined(__HERBCEPTIONS__)
using ::fast_io::tls::async_pread_some_bytes_underflow_callback_define;
using ::fast_io::tls::async_pwrite_some_bytes_overflow_callback_define;
#endif
#endif
} // namespace tls
namespace operations
{
using ::fast_io::operations::handshake;
#if defined(__HERBCEPTIONS__)
using ::fast_io::operations::async_handshake;
#endif
}
namespace operations::decay
{
#if defined(__HERBCEPTIONS__)
using ::fast_io::operations::decay::async_handshake_decay;
using ::fast_io::operations::decay::async_handshake_decay_callback;
#endif
}
} // namespace fast_io
