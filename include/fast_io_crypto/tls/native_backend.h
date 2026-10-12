#pragma once

/*
native TLS backend selection: prefer the OS/OpenSSL record layer when
available -- Schannel on Windows, OpenSSL when its headers are visible --
and fall back to the userspace TLS 1.3 implementation (basic_tls)
everywhere else. The aliases mirror basic_tls's names so code is
backend-agnostic.

-D FAST_IO_TLS_FORCE_FAST_IO=1 pins the aliases to the fast_io userspace
backend even where a platform backend exists -- e.g. wine's schannel
still shells out to gnutls, which a wine user may not provide. The macro
itself is defined in defs.h since the crypto backend also reads it.
*/

namespace fast_io::tls
{

/*
which record-layer backend native_tls resolves to on this build --
schannel on Windows, fast_io otherwise (or when FAST_IO_TLS_FORCE_FAST_IO
is set). `tls_platform::platform` names the selected backend.
*/
enum class tls_platform : ::std::uint_least8_t
{
	fast_io,  /* fast_io userspace TLS 1.3 (always available) */
	schannel, /* Windows SSPI */
#if FAST_IO_TLS_FORCE_FAST_IO
	platform = fast_io,
#elif (defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__)
	platform = schannel,
#else
	platform = fast_io,
#endif
};

#if !FAST_IO_TLS_FORCE_FAST_IO && ((defined(_WIN32) && !defined(__WINE__)) || defined(__CYGWIN__))

template <::std::integral ch_type, typename allocator_type = ::fast_io::native_global_allocator>
using basic_native_tls = basic_schannel_tls<basic_native_socket_file<ch_type>, allocator_type>;

using native_tls_socket_file = basic_native_tls<char>;
using u8native_tls_socket_file = basic_native_tls<char8_t>;

template <::std::integral ch_type, typename allocator_type = ::fast_io::native_global_allocator>
using basic_iobuf_native_tls_socket_file =
	basic_iobuf<basic_native_tls<ch_type, allocator_type>, allocator_type>;

using iobuf_native_tls_socket_file = basic_iobuf_native_tls_socket_file<char>;
using u8iobuf_native_tls_socket_file = basic_iobuf_native_tls_socket_file<char8_t>;

#else

/* the educational userspace TLS 1.3 client is always available */
template <::std::integral ch_type, typename allocator_type = ::fast_io::native_global_allocator>
using basic_native_tls = basic_tls<basic_native_socket_file<ch_type>, allocator_type>;

using native_tls_socket_file = basic_native_tls<char>;
using u8native_tls_socket_file = basic_native_tls<char8_t>;

template <::std::integral ch_type, typename allocator_type = ::fast_io::native_global_allocator>
using basic_iobuf_native_tls_socket_file =
	basic_iobuf<basic_tls_socket_file<ch_type, allocator_type>, allocator_type>;

using iobuf_native_tls_socket_file = basic_iobuf_native_tls_socket_file<char>;
using u8iobuf_native_tls_socket_file = basic_iobuf_native_tls_socket_file<char8_t>;

#endif

} // namespace fast_io::tls
