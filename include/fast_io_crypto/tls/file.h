#pragma once

/*
basic_tls<socket_type, allocator_type> -- a plain aggregate-style bundle
of a socket (the transport) and a basic_tls_client (the TLS state). The
socket member OWNS whatever socket_type owns: basic_tls<native_socket_file>
owns the fd; basic_tls<native_socket_io_observer> borrows it.

Constructors forward to socket_type, mirroring basic_io_buffer's
forwarding to its handle; the client binds the socket's observer during
construction. The client itself is a plain aggregate -- operations are
the free-function customization points (operations::handshake, the
_bytes ops, tls_close_notify).
*/

namespace fast_io::tls
{

template <typename socket_type, typename allocator_type = ::fast_io::native_global_allocator,
		  typename crypto = tls_default_crypto>
struct basic_tls
{
	using char_type = typename socket_type::char_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
	/* the transport observer -- whatever the socket's stream ref yields
	   (posix fd observer, win32 socket observer, ...) */
	using socket_observer_type =
		decltype(::fast_io::io_stream_ref_define(::std::declval<socket_type &>()));
	using client_type = basic_tls_client<allocator_type, socket_observer_type, crypto>;
	using allocator_handle_type = typename client_type::allocator_handle_type;
	static inline constexpr bool alloc_with_status{client_type::alloc_with_status};

	socket_type socket;
	client_type client;

	inline constexpr basic_tls() noexcept
		requires(!alloc_with_status && ::std::is_default_constructible_v<socket_type>)
	= default;

	template <typename... Args>
		requires(!alloc_with_status && ::std::constructible_from<socket_type, Args...>)
	inline explicit constexpr basic_tls(Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(socket_type(::std::forward<Args>(args)...))
		: socket(::std::forward<Args>(args)...), client{::fast_io::io_stream_ref_define(socket)}
	{
	}

	inline explicit constexpr basic_tls(allocator_handle_type hdl)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(socket_type{}) || !noexcept(client_type{socket, hdl}))
		requires(alloc_with_status && ::std::is_default_constructible_v<socket_type>)
		: socket(), client{socket, hdl}
	{
	}
	template <typename... Args>
		requires(alloc_with_status && ::std::constructible_from<socket_type, Args...>)
	inline constexpr basic_tls(allocator_handle_type hdl, Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(socket_type(::std::forward<Args>(args)...))
		: socket(::std::forward<Args>(args)...), client{socket, hdl}
	{
	}

	basic_tls(basic_tls const &) = delete;
	basic_tls &operator=(basic_tls const &) = delete;

	inline basic_tls(basic_tls &&other) noexcept
		: socket(::std::move(other.socket)), client(::std::move(other.client))
	{
	}
	inline basic_tls &operator=(basic_tls &&other) noexcept
	{
		if (__builtin_addressof(other) == this)
		{
			return *this;
		}
		socket = ::std::move(other.socket);
		client = ::std::move(other.client);
		return *this;
	}

	/* detach ownership: the socket member releases the fd (so the
	   destructor cannot close it again -- the op consumes it) and the
	   stream's non-owning observer is returned -- the same role
	   posix_file::release()'s fd plays for posix_io_observer */
	inline constexpr basic_tls_io_observer<char_type, allocator_type, socket_observer_type, crypto> release() noexcept
		requires requires { socket.release(); }
	{
		socket.release();
		return {__builtin_addressof(client)};
	}
};

template <typename socket_type, typename allocator_type, typename crypto>
inline constexpr basic_tls_io_observer<typename socket_type::char_type, allocator_type,
									   typename basic_tls<socket_type, allocator_type, crypto>::socket_observer_type, crypto>
io_stream_ref_define(basic_tls<socket_type, allocator_type, crypto> &t) noexcept
{
	return {__builtin_addressof(t.client)};
}

template <typename socket_type, typename allocator_type, typename crypto>
inline constexpr basic_tls_io_observer<char, allocator_type,
									   typename basic_tls<socket_type, allocator_type, crypto>::socket_observer_type, crypto>
io_bytes_stream_ref_define(basic_tls<socket_type, allocator_type, crypto> &t) noexcept
{
	return {__builtin_addressof(t.client)};
}

/* owning variants: the socket member is a native socket file */
template <::std::integral ch_type, typename allocator_type = ::fast_io::native_global_allocator>
using basic_tls_socket_file = basic_tls<basic_native_socket_file<ch_type>, allocator_type>;

using tls_socket_file = basic_tls_socket_file<char>;
using wtls_socket_file = basic_tls_socket_file<wchar_t>;
using u8tls_socket_file = basic_tls_socket_file<char8_t>;
using u16tls_socket_file = basic_tls_socket_file<char16_t>;
using u32tls_socket_file = basic_tls_socket_file<char32_t>;

/* non-owning variants: the socket member is a native socket observer */
template <::std::integral ch_type, typename allocator_type = ::fast_io::native_global_allocator>
using basic_tls_socket = basic_tls<basic_native_socket_io_observer<ch_type>, allocator_type>;

using tls_socket = basic_tls_socket<char>;
using wtls_socket = basic_tls_socket<wchar_t>;
using u8tls_socket = basic_tls_socket<char8_t>;
using u16tls_socket = basic_tls_socket<char16_t>;
using u32tls_socket = basic_tls_socket<char32_t>;

/* buffered variants: basic_iobuf<basic_tls<socket_type>> */
template <::std::integral ch_type, typename allocator_type = ::fast_io::native_global_allocator>
using basic_iobuf_tls_socket_file =
	basic_iobuf<basic_tls_socket_file<ch_type, allocator_type>, allocator_type>;

using iobuf_tls_socket_file = basic_iobuf_tls_socket_file<char>;
using wiobuf_tls_socket_file = basic_iobuf_tls_socket_file<wchar_t>;
using u8iobuf_tls_socket_file = basic_iobuf_tls_socket_file<char8_t>;
using u16iobuf_tls_socket_file = basic_iobuf_tls_socket_file<char16_t>;
using u32iobuf_tls_socket_file = basic_iobuf_tls_socket_file<char32_t>;

} // namespace fast_io::tls
